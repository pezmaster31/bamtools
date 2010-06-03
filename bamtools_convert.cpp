// ***************************************************************************
// bamtools_convert.cpp (c) 2010 Derek Barnett, Erik Garrison
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 2 June 2010
// ---------------------------------------------------------------------------
// Converts between BAM and a number of other formats
// ***************************************************************************

#include <iostream>
#include <string>
#include <vector>

#include "bamtools_convert.h"
#include "bamtools_options.h"
#include "BamReader.h"
#include "BamMultiReader.h"

using namespace std;
using namespace BamTools; 
  
// ---------------------------------------------
// ConvertSettings implementation

struct ConvertTool::ConvertSettings {

    // flags
    bool HasInputBamFilename;
    bool HasOutputBamFilename;
    bool HasFormat;

    // filenames
    string InputFilename;
    string OutputFilename;
    string Format;
    
    // constructor
    ConvertSettings(void)
        : HasInputBamFilename(false)
        , HasOutputBamFilename(false)
        , OutputFilename(Options::StandardOut())
    { }
};  

// ---------------------------------------------
// ConvertTool implementation

ConvertTool::ConvertTool(void)
    : AbstractTool()
    , m_settings(new ConvertSettings)
{
    // set program details
    Options::SetProgramInfo("bamtools convert", "converts between BAM and a number of other formats)", "-in <filename> [-in <filename> ... ] -out <filename> -format FORMAT");
    
    // set up options 
    OptionGroup* IO_Opts = Options::CreateOptionGroup("Input & Output");
    Options::AddValueOption("-in",     "BAM filename", "the input BAM file(s)", "", m_settings->HasInputBamFilename,  m_settings->InputFilename,  IO_Opts, Options::StandardIn());
    Options::AddValueOption("-out",    "BAM filename", "the output BAM file",   "", m_settings->HasOutputBamFilename, m_settings->OutputFilename, IO_Opts, Options::StandardOut());
    Options::AddValueOption("-format", "FORMAT", "the output file format - see README for recognized formats", "", m_settings->HasFormat, m_settings->Format, IO_Opts);
}

ConvertTool::~ConvertTool(void) {
    delete m_settings;
    m_settings = 0;
}

int ConvertTool::Help(void) {
    Options::DisplayHelp();
    return 0;
}

int ConvertTool::Run(int argc, char* argv[]) {
  
    // parse command line arguments
    Options::Parse(argc, argv, 1);
    
    // open files
    BamReader reader;
    reader.Open(m_settings->InputFilename, false);
        
    // do conversion
    
    // clean up & exit
    reader.Close();
    return 0;
}