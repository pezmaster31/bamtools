// ***************************************************************************
// bamtools_sort.cpp (c) 2010 Derek Barnett, Erik Garrison
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 26 May 2010
// ---------------------------------------------------------------------------
// Sorts an input BAM file (default by position) and stores in a new BAM file.
// ***************************************************************************

#include <iostream>
#include <string>

#include "bamtools_sort.h"
#include "bamtools_options.h"
#include "BamReader.h"

using namespace std;
using namespace BamTools;

// ---------------------------------------------
// SortSettings implementation

struct SortTool::SortSettings {

    // flags
    bool HasInputBamFilename;
    bool HasOutputBamFilename;

    // filenames
    string InputBamFilename;
    string OutputBamFilename;
    
    // constructor
    SortSettings(void)
        : HasInputBamFilename(false)
        , HasOutputBamFilename(false)
        , InputBamFilename(Options::StandardIn())
        , OutputBamFilename(Options::StandardOut())
    { }
};  

// ---------------------------------------------
// SortTool implementation

SortTool::SortTool(void)
    : AbstractTool()
    , m_settings(new SortSettings)
{
    // set program details
    Options::SetProgramInfo("bamtools sort", "sorts a BAM file", "[-in <filename>] [-out <filename>]");
    
    // set up options 
    OptionGroup* IO_Opts = Options::CreateOptionGroup("Input & Output");
    Options::AddValueOption("-in",  "BAM filename", "the input BAM file",  "", m_settings->HasInputBamFilename,  m_settings->InputBamFilename,  IO_Opts, Options::StandardIn());
    Options::AddValueOption("-out", "BAM filename", "the output BAM file", "", m_settings->HasOutputBamFilename, m_settings->OutputBamFilename, IO_Opts, Options::StandardOut());
}

SortTool::~SortTool(void) {
    delete m_settings;
    m_settings = 0;
}

int SortTool::Help(void) {
    Options::DisplayHelp();
    return 0;
}

int SortTool::Run(int argc, char* argv[]) {
  
    // parse command line arguments
    Options::Parse(argc, argv, 1);
    
    // do sorting
    
    return 0;
}
