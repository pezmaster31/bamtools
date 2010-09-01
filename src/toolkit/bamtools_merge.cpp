// ***************************************************************************
// bamtools_merge.cpp (c) 2010 Derek Barnett, Erik Garrison
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 31 August 2010
// ---------------------------------------------------------------------------
// Merges multiple BAM files into one.
//
// ** Provide selectable region? eg chr2:10000..20000
//
// ***************************************************************************

#include <iostream>
#include <string>
#include <vector>

#include "bamtools_merge.h"
#include "bamtools_options.h"
#include "bamtools_utilities.h"
#include "BamMultiReader.h"
#include "BamWriter.h"

using namespace std;
using namespace BamTools;

// ---------------------------------------------
// MergeSettings implementation

struct MergeTool::MergeSettings {

    // flags
    bool HasInputBamFilename;
    bool HasOutputBamFilename;
    bool IsForceCompression;
//     bool HasRegion;
    
    // filenames
    vector<string> InputFiles;
    
    // other parameters
    string OutputFilename;
//     string Region;
    
    // constructor
    MergeSettings(void)
        : HasInputBamFilename(false)
        , HasOutputBamFilename(false)
        , IsForceCompression(false)
//         , HasRegion(false)
        , OutputFilename(Options::StandardOut())
    { }
};  

// ---------------------------------------------
// MergeTool implementation

MergeTool::MergeTool(void)
    : AbstractTool()
    , m_settings(new MergeSettings)
{
    // set program details
    Options::SetProgramInfo("bamtools merge", "merges multiple BAM files into one", "[-in <filename> -in <filename> ...] [-out <filename>]");
    
    // set up options 
    OptionGroup* IO_Opts = Options::CreateOptionGroup("Input & Output");
    Options::AddValueOption("-in",  "BAM filename", "the input BAM file(s)", "", m_settings->HasInputBamFilename,  m_settings->InputFiles,     IO_Opts);
    Options::AddValueOption("-out", "BAM filename", "the output BAM file",   "", m_settings->HasOutputBamFilename, m_settings->OutputFilename, IO_Opts);
    Options::AddOption("-forceCompression", "if results are sent to stdout (like when piping to another tool), default behavior is to leave output uncompressed. Use this flag to override and force compression", m_settings->IsForceCompression, IO_Opts);
    
//     OptionGroup* FilterOpts = Options::CreateOptionGroup("Filters");
//     Options::AddValueOption("-region", "REGION", "genomic region. See README for more details", "", m_settings->HasRegion, m_settings->Region, FilterOpts);
}

MergeTool::~MergeTool(void) {
    delete m_settings;
    m_settings = 0;
}

int MergeTool::Help(void) {
    Options::DisplayHelp();
    return 0;
}

int MergeTool::Run(int argc, char* argv[]) {
  
    // parse command line arguments
    Options::Parse(argc, argv, 1);
    
     // set to default input if none provided
    if ( !m_settings->HasInputBamFilename ) m_settings->InputFiles.push_back(Options::StandardIn());
    
    // opens the BAM files without checking for indexes
    BamMultiReader reader;
    reader.Open(m_settings->InputFiles, false, true); 

    // retrieve header & reference dictionary info
    std::string mergedHeader = reader.GetHeaderText();
    RefVector references = reader.GetReferenceData();

    // open writer
    BamWriter writer;
    bool writeUncompressed = ( m_settings->OutputFilename == Options::StandardOut() && !m_settings->IsForceCompression );
    writer.Open(m_settings->OutputFilename, mergedHeader, references, writeUncompressed);

    // store alignments to output file
    BamAlignment bAlignment;
    while (reader.GetNextAlignmentCore(bAlignment)) {
        writer.SaveAlignment(bAlignment);
    }
    
    // clean & exit
    reader.Close();
    writer.Close();
    return 0;  
}
