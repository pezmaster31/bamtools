// ***************************************************************************
// bamtools_header.cpp (c) 2010 Derek Barnett, Erik Garrison
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 1 June 2010
// ---------------------------------------------------------------------------
// Prints the SAM-style header from a single BAM file ( or merged header from
// multiple BAM files) to stdout
// ***************************************************************************

#include <iostream>
#include <string>
#include <vector>

#include "bamtools_header.h"
#include "bamtools_options.h"
#include "BamReader.h"
#include "BamMultiReader.h"

using namespace std;
using namespace BamTools; 
  
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
    Options::SetProgramInfo("bamtools header", "prints header from BAM file(s)", "-in <filename> [-in <filename> ... ] ");
    
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
    if ( !m_settings->HasInputBamFilename ) m_settings->InputFiles.push_back(Options::StandardIn());
    
    // open files
    BamMultiReader reader;
    reader.Open(m_settings->InputFiles, false);
        
    // dump header contents to stdout
    cout << reader.GetHeaderText() << endl;
    
    // clean up & exit
    reader.Close();
    return 0;
}