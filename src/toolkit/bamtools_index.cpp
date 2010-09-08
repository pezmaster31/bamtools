// ***************************************************************************
// bamtools_index.cpp (c) 2010 Derek Barnett, Erik Garrison
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 2 September 2010
// ---------------------------------------------------------------------------
// Creates a BAM index (".bai") file for the provided BAM file.
// ***************************************************************************

#include <iostream>
#include <string>

#include "bamtools_index.h"
#include "bamtools_options.h"
#include "BamReader.h"

using namespace std;
using namespace BamTools;

// ---------------------------------------------
// IndexSettings implementation

struct IndexTool::IndexSettings {

    // flags
    bool HasInputBamFilename;
    bool IsUsingBamtoolsIndex;

    // filenames
    string InputBamFilename;
    
    // constructor
    IndexSettings(void)
        : HasInputBamFilename(false)
        , IsUsingBamtoolsIndex(false)
        , InputBamFilename(Options::StandardIn())
    { }
};  

// ---------------------------------------------
// IndexTool implementation

IndexTool::IndexTool(void)
    : AbstractTool()
    , m_settings(new IndexSettings)
{
    // set program details
    Options::SetProgramInfo("bamtools index", "creates index for BAM file", "[-in <filename>] [-bti]");
    
    // set up options 
    OptionGroup* IO_Opts = Options::CreateOptionGroup("Input & Output");
    Options::AddValueOption("-in", "BAM filename", "the input BAM file", "", m_settings->HasInputBamFilename, m_settings->InputBamFilename, IO_Opts, Options::StandardIn());
    Options::AddOption("-bti", "create (non-standard) BamTools index file", m_settings->IsUsingBamtoolsIndex, IO_Opts);
}

IndexTool::~IndexTool(void) {
    delete m_settings;
    m_settings = 0;
}

int IndexTool::Help(void) {
    Options::DisplayHelp();
    return 0;
}

int IndexTool::Run(int argc, char* argv[]) {
  
    // parse command line arguments
    Options::Parse(argc, argv, 1);
    
    // open our BAM reader
    BamReader reader;
    reader.Open(m_settings->InputBamFilename);
    
    // create index for BAM file
    bool useDefaultIndex = !m_settings->IsUsingBamtoolsIndex;
    reader.CreateIndex(useDefaultIndex);
    
    // clean & exit
    reader.Close();
    return 0;
}
