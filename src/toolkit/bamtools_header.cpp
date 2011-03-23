// ***************************************************************************
// bamtools_header.cpp (c) 2010 Derek Barnett, Erik Garrison
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 21 March 2011
// ---------------------------------------------------------------------------
// Prints the SAM-style header from a single BAM file ( or merged header from
// multiple BAM files) to stdout
// ***************************************************************************

#include "bamtools_header.h"

#include <api/BamMultiReader.h>
#include <utils/bamtools_options.h>
using namespace BamTools;

#include <iostream>
#include <string>
#include <vector>
using namespace std;
  
// ---------------------------------------------
// HeaderSettings implementation

struct HeaderTool::HeaderSettings {

    // flags
    bool HasInputBamFilename;

    // filenames
    vector<string> InputFiles;
    
    // constructor
    HeaderSettings(void)
        : HasInputBamFilename(false)
    { }
};  

// ---------------------------------------------
// HeaderTool implementation

HeaderTool::HeaderTool(void)
    : AbstractTool()
    , m_settings(new HeaderSettings)
{
    // set program details
    Options::SetProgramInfo("bamtools header", "prints header from BAM file(s)", "[-in <filename> -in <filename> ...] ");
    
    // set up options 
    OptionGroup* IO_Opts = Options::CreateOptionGroup("Input & Output");
    Options::AddValueOption("-in", "BAM filename", "the input BAM file(s)", "", m_settings->HasInputBamFilename, m_settings->InputFiles, IO_Opts, Options::StandardIn());
}

HeaderTool::~HeaderTool(void) {
    delete m_settings;
    m_settings = 0;
}

int HeaderTool::Help(void) {
    Options::DisplayHelp();
    return 0;
}

int HeaderTool::Run(int argc, char* argv[]) {
  
    // parse command line arguments
    Options::Parse(argc, argv, 1);
  
    // set to default input if none provided
    if ( !m_settings->HasInputBamFilename ) 
        m_settings->InputFiles.push_back(Options::StandardIn());
    
    // attemp to open BAM files
    BamMultiReader reader;
    if ( !reader.Open(m_settings->InputFiles) ) {
        cerr << "bamtools header ERROR: could not open BAM file(s) for reading... Aborting." << endl;
        return 1;
    }

    // dump (merged) header contents to stdout
    cout << reader.GetHeaderText() << endl;

    // clean up & exit
    reader.Close();
    return 0;
}
