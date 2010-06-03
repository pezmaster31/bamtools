// ***************************************************************************
// bamtools_filter.cpp (c) 2010 Derek Barnett, Erik Garrison
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 1 June 2010
// ---------------------------------------------------------------------------
// Filters a single BAM file (or filters multiple BAM files and merges) 
// according to some user-specified criteria.
// ***************************************************************************

#include <iostream>
#include <string>
#include <vector>

#include "bamtools_filter.h"
#include "bamtools_options.h"
#include "BamReader.h"
#include "BamMultiReader.h"

using namespace std;
using namespace BamTools; 
  
// ---------------------------------------------
// FilterSettings implementation

struct FilterTool::FilterSettings {

    // flags
    bool HasInputBamFilename;
    bool HasOutputBamFilename;

    // filenames
    vector<string> InputFiles;
    string OutputFilename;
    
    // constructor
    FilterSettings(void)
        : HasInputBamFilename(false)
        , HasOutputBamFilename(false)
        , OutputFilename(Options::StandardOut())
    { }
};  

// ---------------------------------------------
// FilterTool implementation

FilterTool::FilterTool(void)
    : AbstractTool()
    , m_settings(new FilterSettings)
{
    // set program details
    Options::SetProgramInfo("bamtools filter", "filters BAM file(s)", "-in <filename> [-in <filename> ... ] -out <filename> ");
    
    // set up options 
    OptionGroup* IO_Opts = Options::CreateOptionGroup("Input & Output");
    Options::AddValueOption("-in",  "BAM filename", "the input BAM file(s)", "", m_settings->HasInputBamFilename,  m_settings->InputFiles,     IO_Opts, Options::StandardIn());
    Options::AddValueOption("-out", "BAM filename", "the output BAM file",   "", m_settings->HasOutputBamFilename, m_settings->OutputFilename, IO_Opts, Options::StandardOut());
}

FilterTool::~FilterTool(void) {
    delete m_settings;
    m_settings = 0;
}

int FilterTool::Help(void) {
    Options::DisplayHelp();
    return 0;
}

int FilterTool::Run(int argc, char* argv[]) {
  
    // parse command line arguments
    Options::Parse(argc, argv, 1);
  
    // set to default input if none provided
    if ( !m_settings->HasInputBamFilename ) 
        m_settings->InputFiles.push_back(Options::StandardIn());
    
    // open files
    BamMultiReader reader;
    reader.Open(m_settings->InputFiles, false);
        
    // do filtering
    
    // clean up & exit
    reader.Close();
    return 0;
}