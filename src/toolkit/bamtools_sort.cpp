// ***************************************************************************
// bamtools_sort.cpp (c) 2010 Derek Barnett, Erik Garrison
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 21 March 2011 (DB)
// ---------------------------------------------------------------------------
// Sorts an input BAM file (default by position) and stores in a new BAM file.
// ***************************************************************************

#include "bamtools_sort.h"

#include <api/SamConstants.h>
#include <api/BamMultiReader.h>
#include <api/BamWriter.h>
#include <utils/bamtools_options.h>
#include <api/BamSortCriteria.h>
using namespace BamTools;

#include <cstdio>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

namespace BamTools {
  
    // defaults
    //
    // ** These defaults should be tweaked & 'optimized' per testing ** //
    //
    //    I say 'optimized' because each system will naturally perform
    //    differently.  We will attempt to determine a sensible 
    //    compromise that should perform well on average.
    const unsigned int SORT_DEFAULT_MAX_BUFFER_COUNT  = 500000;  // max numberOfAlignments for buffer
    const unsigned int SORT_DEFAULT_MAX_BUFFER_MEMORY = 1024;    // Mb

    // -----------------------------------
    // comparison objects (for sorting) 

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
        //void SortBuffer(vector<BamAlignment>& buffer);
        template<typename T>
        BamAlignmentBFunction& getSortingFunction();
        
    // data members
    private:
        SortTool::SortSettings* m_settings;
        BamSortCriteria m_sort;
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
    bool IsSortDescending;
    string SortCriteria;
    bool HasSortCriteria;

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
        , IsSortDescending(false)
        , SortCriteria("QNAME")
        , HasSortCriteria(false)
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
    Options::SetProgramInfo("bamtools sort", "sorts a BAM file", "[-in <filename>] [-out <filename>] [sortOptions]");
    
    // set up options 
    OptionGroup* IO_Opts = Options::CreateOptionGroup("Input & Output");
    Options::AddValueOption("-in",  "BAM filename", "the input BAM file",  "", m_settings->HasInputBamFilename,  m_settings->InputBamFilename,  IO_Opts, Options::StandardIn());
    Options::AddValueOption("-out", "BAM filename", "the output BAM file", "", m_settings->HasOutputBamFilename, m_settings->OutputBamFilename, IO_Opts, Options::StandardOut());
    
    OptionGroup* SortOpts = Options::CreateOptionGroup("Sorting Methods");
    Options::AddValueOption("-tagname", "sort by tag name", "("+BamSortCriteria::getAllowedTags()+")", "", m_settings->HasSortCriteria, m_settings->SortCriteria, SortOpts, Options::StandardOut());
    Options::AddOption("-desc", "sort values descending", m_settings->IsSortDescending, SortOpts);
//    Options::AddOption("-byname", "sort by alignment name", m_settings->SortCriteria, SortOpts);
    
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
     if(!BamSortCriteria::isTagValid(m_settings->SortCriteria)){
      cerr << "bamtools sort ERROR: The tag "<<m_settings->SortCriteria<<" is not valid...aborting\n    ";
      return 1; 
    }
    
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
    , m_sort(settings->SortCriteria,settings->IsSortDescending)
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
    if ( !inputReader.Open(m_settings->InputBamFilename) ) {
        cerr << "bamtools sort ERROR: could not open " << m_settings->InputBamFilename
             << " for reading... Aborting." << endl;
        return false;
    }
    
    // get basic data that will be shared by all temp/output files 
    SamHeader header = inputReader.GetHeader();
    header.SortOrder = m_sort.getSamHeaderSort();
    m_headerText = header.ToString();
    m_references = inputReader.GetReferenceData();
    
    // set up alignments buffer
    BamAlignment al;
    vector<BamAlignment> buffer;
    buffer.reserve(m_settings->MaxBufferCount);
    
    // if sorting by name, we need to generate full char data
    // so can't use GetNextAlignmentCore()
    if (!m_sort.isTagCoreAttribute() ) {

        // iterate through file
        while ( inputReader.GetNextAlignment(al)) {

            // store alignments in buffer
            buffer.push_back(al);

            // if buffer is full, handle contents (sort & write to temp file)
            if ( buffer.size() == m_settings->MaxBufferCount )
                HandleBufferContents(buffer);
        }

    }

    // sorting by position, can take advantage of GNACore() speedup
    else {

        // iterate through file
        while ( inputReader.GetNextAlignmentCore(al) ) {

            // store alignments in buffer
            buffer.push_back(al);

            // if buffer is full, handle contents (sort & write to temp file)
            if ( buffer.size() == m_settings->MaxBufferCount ){
                HandleBufferContents(buffer);
            }
        }
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
    m_sort.sortBuffer(buffer.begin(),buffer.end());
    //SortBuffer(buffer);
  
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
    // TODO: a failure returned here is not actually caught and handled anywhere
    return success;
}

// merges sorted temp BAM files into single sorted output BAM file
bool SortTool::SortToolPrivate::MergeSortedRuns(void) {
  
    // open up multi reader for all of our temp files
    // this might get broken up if we do a multi-pass system later ??
    BamMultiReader multiReader;
    if ( !multiReader.Open(m_tempFilenames) ) {
        cerr << "bamtools sort ERROR: could not open BamMultiReader for merging temp files... Aborting." << endl;
        return false;
    }

    // set sort order for merge
    multiReader.SetSortOrder(m_sort);
    
    // open writer for our completely sorted output BAM file
    BamWriter mergedWriter;
    if ( !mergedWriter.Open(m_settings->OutputBamFilename, m_headerText, m_references) ) {
        cerr << "bamtools sort ERROR: could not open " << m_settings->OutputBamFilename
             << " for writing... Aborting." << endl;
        multiReader.Close();
        return false;
    }
    
    // while data available in temp files
    BamAlignment al;
    while ( multiReader.GetNextAlignmentCore(al) )
        mergedWriter.SaveAlignment(al);
  
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



    
bool SortTool::SortToolPrivate::WriteTempFile(const vector<BamAlignment>& buffer, const string& tempFilename) {

    // open temp file for writing
    BamWriter tempWriter;
    if ( !tempWriter.Open(tempFilename, m_headerText, m_references) ) {
        cerr << "bamtools sort ERROR: could not open " << tempFilename
             << " for writing." << endl;
        return false;
    }
  
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
