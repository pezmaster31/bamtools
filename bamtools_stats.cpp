// ***************************************************************************
// bamtools_stats.cpp (c) 2010 Derek Barnett, Erik Garrison
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 1 June 2010
// ---------------------------------------------------------------------------
// Prints general statistics for a single BAM file
//
// ** Expand to multiple??
//
// ***************************************************************************

#include <iostream>
#include <string>

#include "bamtools_stats.h"
#include "bamtools_options.h"
#include "BamReader.h"

using namespace std;
using namespace BamTools;

// ---------------------------------------------
// StatsSettings implementation

struct StatsTool::StatsSettings {

    // flags
    bool HasInputBamFilename;

    // filenames
    string InputBamFilename;
    
    // constructor
    StatsSettings(void)
        : HasInputBamFilename(false)
        , InputBamFilename(Options::StandardIn())
    { }
};  

// ---------------------------------------------
// StatsTool implementation

StatsTool::StatsTool(void)
    : AbstractTool()
    , m_settings(new StatsSettings)
{
    // set program details
    Options::SetProgramInfo("bamtools stats", "prints general stats for a BAM file", "[-in <filename>]");
    
    // set up options 
    OptionGroup* IO_Opts = Options::CreateOptionGroup("Input & Output");
    Options::AddValueOption("-in", "BAM filename", "the input BAM file", "", m_settings->HasInputBamFilename,  m_settings->InputBamFilename,  IO_Opts, Options::StandardIn());
}

StatsTool::~StatsTool(void) {
    delete m_settings;
    m_settings = 0;
}

int StatsTool::Help(void) {
    Options::DisplayHelp();
    return 0;
}

int StatsTool::Run(int argc, char* argv[]) {
  
    // parse command line arguments
    Options::Parse(argc, argv, 1);
    
    // calculate stats
    
    return 0;
}
