// ***************************************************************************
// bamtools_coverage.cpp (c) 2010 Derek Barnett, Erik Garrison
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 1 June 2010
// ---------------------------------------------------------------------------
// Prints coverage statistics for a single BAM file 
//
// ** Expand to multiple?? 
//
// ***************************************************************************

#include <iostream>
#include <string>
#include <vector>

#include "bamtools_coverage.h"
#include "bamtools_options.h"
#include "BamReader.h"

using namespace std;
using namespace BamTools; 
  
// ---------------------------------------------  
// CoverageSettings implementation

struct CoverageTool::CoverageSettings {

    // flags
    bool HasInputBamFilename;

    // filenames
    std::string InputBamFilename;
    
    // constructor
    CoverageSettings(void)
        : HasInputBamFilename(false)
        , InputBamFilename(Options::StandardIn())
    { } 
};  

// ---------------------------------------------
// CoverageTool implementation

CoverageTool::CoverageTool(void) 
    : AbstractTool()
    , m_settings(new CoverageSettings)
{ 
    // set program details
    Options::SetProgramInfo("bamtools coverage", "prints coverage stats for a BAM file", "-in <filename> ");
    
    // set up options 
    OptionGroup* IO_Opts = Options::CreateOptionGroup("Input & Output");
    Options::AddValueOption("-in", "BAM filename", "the input BAM file", "", m_settings->HasInputBamFilename, m_settings->InputBamFilename, IO_Opts, Options::StandardIn());
}

CoverageTool::~CoverageTool(void) { 
    delete m_settings;
    m_settings = 0;
}

int CoverageTool::Help(void) { 
    Options::DisplayHelp();
    return 0;
} 

int CoverageTool::Run(int argc, char* argv[]) { 

    // parse command line arguments
    Options::Parse(argc, argv, 1);

    //open our BAM reader
    BamReader reader;
    reader.Open(m_settings->InputBamFilename);

    // generate coverage stats
    cerr << "Generating coverage stats for " << m_settings->InputBamFilename << endl;
    cerr << "FEATURE NOT YET IMPLEMENTED!" << endl;

    // clean & exit
    reader.Close();
    return 0;
}