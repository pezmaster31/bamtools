// ***************************************************************************
// bamtools_sort.cpp (c) 2010 Derek Barnett, Erik Garrison
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 21 June 2010 (DB)
// ---------------------------------------------------------------------------
// Sorts an input BAM file (default by position) and stores in a new BAM file.
// ***************************************************************************

#include <cstdio>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "bamtools_sort.h"
#include "bamtools_options.h"
#include "BamReader.h"
#include "BamMultiReader.h"
#include "BamWriter.h"

using namespace std;
using namespace BamTools;

namespace BamTools {
  
    // defaults
    //
    // ** These defaults should be tweaked & 'optimized' per testing ** //
    //    I say 'optimized' because each system will naturally perform
    //    differently.  We will attempt to determine a sensible 
    //    compromise that should perform well on average.
    const unsigned int SORT_DEFAULT_MAX_BUFFER_COUNT  = 10000; // max numberOfAlignments for buffer
    const unsigned int SORT_DEFAULT_MAX_BUFFER_MEMORY = 1024;  // Mb

    // -----------------------------------
    // comparison objects (for sorting) 

    struct SortLessThanPosition {
        bool operator() (const BamAlignment& lhs, const BamAlignment& rhs) {
            if ( lhs.RefID != rhs.RefID )
                return lhs.RefID < rhs.RefID;
            else 
                return lhs.Position < rhs.Position;
        }
    };
    
    struct SortLessThanName {
        bool operator() (const BamAlignment& lhs, const BamAlignment& rhs) {
            return lhs.Name < rhs.Name;
        }
    };
    
} // namespace BamTools

// ---------------------------------------------
// SortToolPrivate declaration
class SortTool::SortToolPrivate {
      
    // ctor & dtor
    public:
        SortToolPrivate(SortTool::SortSettings* settings);
        ~SortToolPrivate(void);
        
    // 'public' interface
    public:
        bool Run(void);
        
    // internal methods
    private:
        void ClearBuffer(vector<BamAlignment>& buffer);
        bool GenerateSortedRuns(void);
        bool HandleBufferContents(vector<BamAlignment>& buffer);
        bool MergeSortedRuns(void);
        bool WriteTempFile(const vector<BamAlignment>& buffer, const string& tempFilename);
        void SortBuffer(vector<BamAlignment>& buffer);
        
    // data members
    private:
        SortTool::SortSettings* m_settings;
        string m_tempFilenameStub;
        int m_numberOfRuns;
        string m_headerText;
        RefVector m_references;
        vector<string> m_tempFilenames;
};

// ---------------------------------------------
// SortSettings implementation

struct SortTool::SortSettings {

    // flags
    bool HasInputBamFilename;
    bool HasMaxBufferCount;
    bool HasMaxBufferMemory;
    bool HasOutputBamFilename;
    bool IsSortingByName;

    // filenames
    string InputBamFilename;
    string OutputBamFilename;
    
    // parameters
    unsigned int MaxBufferCount;
    unsigned int MaxBufferMemory;
    
    // constructor
    SortSettings(void)
        : HasInputBamFilename(false)
        , HasMaxBufferCount(false)
        , HasMaxBufferMemory(false)
        , HasOutputBamFilename(false)
        , IsSortingByName(false)
        , InputBamFilename(Options::StandardIn())
        , OutputBamFilename(Options::StandardOut())
        , MaxBufferCount(SORT_DEFAULT_MAX_BUFFER_COUNT)
        , MaxBufferMemory(SORT_DEFAULT_MAX_BUFFER_MEMORY)
    { } 
};  

// ---------------------------------------------
// SortTool implementation

SortTool::SortTool(void)
    : AbstractTool()
    , m_settings(new SortSettings)
    , m_impl(0)
{
    // set program details
    Options::SetProgramInfo("bamtools sort", "sorts a BAM file", "[-in <filename>] [-out <filename>]");
    
    // set up options 
    OptionGroup* IO_Opts = Options::CreateOptionGroup("Input & Output");
    Options::AddValueOption("-in",  "BAM filename", "the input BAM file",  "", m_settings->HasInputBamFilename,  m_settings->InputBamFilename,  IO_Opts, Options::StandardIn());
    Options::AddValueOption("-out", "BAM filename", "the output BAM file", "", m_settings->HasOutputBamFilename, m_settings->OutputBamFilename, IO_Opts, Options::StandardOut());
    
    OptionGroup* SortOpts = Options::CreateOptionGroup("Sorting Methods");
    Options::AddOption("-byname", "sort by alignment name", m_settings->IsSortingByName, SortOpts);
    
    OptionGroup* MemOpts = Options::CreateOptionGroup("Memory Settings");
    Options::AddValueOption("-n",   "count", "max number of alignments per tempfile", "", m_settings->HasMaxBufferCount,  m_settings->MaxBufferCount,  MemOpts, SORT_DEFAULT_MAX_BUFFER_COUNT);
    Options::AddValueOption("-mem", "Mb",    "max memory to use",                     "", m_settings->HasMaxBufferMemory, m_settings->MaxBufferMemory, MemOpts, SORT_DEFAULT_MAX_BUFFER_MEMORY);
}

SortTool::~SortTool(void) {
    
    delete m_settings;
    m_settings = 0;
    
    delete m_impl;
    m_impl = 0;
}

int SortTool::Help(void) {
    Options::DisplayHelp();
    return 0;
}

int SortTool::Run(int argc, char* argv[]) {
  
    // parse command line arguments
    Options::Parse(argc, argv, 1);
    
    // run internal SortTool implementation, return success/fail
    m_impl = new SortToolPrivate(m_settings);
    
    if ( m_impl->Run() ) return 0;
    else return 1;
}

// ---------------------------------------------
// SortToolPrivate implementation

// constructor
SortTool::SortToolPrivate::SortToolPrivate(SortTool::SortSettings* settings) 
    : m_settings(settings)
    , m_numberOfRuns(0) 
{ 
    // set filename stub depending on inputfile path
    // that way multiple sort runs don't trip on each other's temp files
    if ( m_settings) {
        size_t extensionFound = m_settings->InputBamFilename.find(".bam");
        if (extensionFound != string::npos )
            m_tempFilenameStub = m_settings->InputBamFilename.substr(0,extensionFound);
        m_tempFilenameStub.append(".sort.temp.");
    }
}

// destructor
SortTool::SortToolPrivate::~SortToolPrivate(void) { }

// generates mutiple sorted temp BAM files from single unsorted BAM file
bool SortTool::SortToolPrivate::GenerateSortedRuns(void) {
    
    // open input BAM file
    BamReader inputReader;
    if (!inputReader.Open(m_settings->InputBamFilename)) {
        cerr << "Could not open " << m_settings->InputBamFilename << " for reading." << endl;
        return false;
    }
    
    // get basic data that will be shared by all temp/output files 
    m_headerText = inputReader.GetHeaderText();
    m_references = inputReader.GetReferenceData();
    
    // set up alignments buffer
    vector<BamAlignment> buffer;
    buffer.reserve(m_settings->MaxBufferCount);
    
    // while data available
    BamAlignment al;
    while ( inputReader.GetNextAlignmentCore(al)) {
        
        // store alignments in buffer
        buffer.push_back(al);
        
        // if buffer is full, handle contents (sort & write to temp file)
        if ( buffer.size() == m_settings->MaxBufferCount )
            HandleBufferContents(buffer);
    }
    
    // handle any remaining buffer contents
    if ( buffer.size() > 0 )
        HandleBufferContents(buffer);
    
    // close reader & return success
    inputReader.Close();
    return true;
}

bool SortTool::SortToolPrivate::HandleBufferContents(vector<BamAlignment>& buffer ) {
 
    // do sorting
    SortBuffer(buffer);
  
    // write sorted contents to temp file, store success/fail
    stringstream tempStr;
    tempStr << m_tempFilenameStub << m_numberOfRuns;
    bool success = WriteTempFile( buffer, tempStr.str() );
    
    // save temp filename for merging later
    m_tempFilenames.push_back(tempStr.str());
    
    // clear buffer contents & update run counter
    buffer.clear();
    ++m_numberOfRuns;
    
    // return success/fail of writing to temp file
    return success;
}

// merges sorted temp BAM files into single sorted output BAM file
bool SortTool::SortToolPrivate::MergeSortedRuns(void) {
  
    // open up multi reader for all of our temp files
    // this might get broken up if we do a multi-pass system later ??
    BamMultiReader multiReader;
    multiReader.Open(m_tempFilenames, false, true);
    
    // open writer for our completely sorted output BAM file
    BamWriter mergedWriter;
    mergedWriter.Open(m_settings->OutputBamFilename, m_headerText, m_references);
    
    // while data available in temp files
    BamAlignment al;
    while ( multiReader.GetNextAlignmentCore(al) ) {
        mergedWriter.SaveAlignment(al);
    }
  
    // close readers
    multiReader.Close();
    mergedWriter.Close();
    
    // delete all temp files
    vector<string>::const_iterator tempIter = m_tempFilenames.begin();
    vector<string>::const_iterator tempEnd  = m_tempFilenames.end();
    for ( ; tempIter != tempEnd; ++tempIter ) {
        const string& tempFilename = (*tempIter);
        remove(tempFilename.c_str());
    }
  
    return true;
}

bool SortTool::SortToolPrivate::Run(void) {
 
    // this does a single pass, chunking up the input file into smaller sorted temp files, 
    // then write out using BamMultiReader to handle merging
    
    if ( GenerateSortedRuns() )
        return MergeSortedRuns();
    else 
        return false;
} 
    
void SortTool::SortToolPrivate::SortBuffer(vector<BamAlignment>& buffer) {
 
    // ** add further custom sort options later ?? **
    
    // sort buffer by desired method
    if ( m_settings->IsSortingByName )
        sort ( buffer.begin(), buffer.end(), SortLessThanName() );
    else 
        sort ( buffer.begin(), buffer.end(), SortLessThanPosition() );
}
    
    
bool SortTool::SortToolPrivate::WriteTempFile(const vector<BamAlignment>& buffer, const string& tempFilename) {

    // open temp file for writing
    BamWriter tempWriter;
    tempWriter.Open(tempFilename, m_headerText, m_references);
  
    // write data
    vector<BamAlignment>::const_iterator buffIter = buffer.begin();
    vector<BamAlignment>::const_iterator buffEnd  = buffer.end();
    for ( ; buffIter != buffEnd; ++buffIter )  {
        const BamAlignment& al = (*buffIter);
        tempWriter.SaveAlignment(al);
    }
  
    // close temp file & return success
    tempWriter.Close();
    return true;
}
