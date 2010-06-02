// ***************************************************************************
// bamtools_count.cpp (c) 2010 Derek Barnett, Erik Garrison
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 1 June 2010
// ---------------------------------------------------------------------------
// Prints alignment count for BAM file
//
// ** Expand to multiple?? 
//
// ***************************************************************************

#include <iostream>
#include <string>
#include <vector>

#include "bamtools_count.h"
#include "bamtools_options.h"
#include "bamtools_utilities.h"
#include "BamReader.h"

using namespace std;
using namespace BamTools;
  
// ---------------------------------------------  
// CountSettings implementation

struct CountTool::CountSettings {

    // flags
    bool HasInputBamFilename;
    bool HasRegion;

    // filenames
    std::string InputBamFilename;
    std::string Region;
    
    // constructor
    CountSettings(void)
        : HasInputBamFilename(false)
        , HasRegion(false)
        , InputBamFilename(Options::StandardIn())
    { }  
}; 
  
// ---------------------------------------------
// CountTool implementation

CountTool::CountTool(void) 
    : AbstractTool()
    , m_settings(new CountSettings)
{ 
    // set program details
    Options::SetProgramInfo("bamtools count", "prints alignment counts for a BAM file", "-in <filename> ");
    
    // set up options 
    OptionGroup* IO_Opts = Options::CreateOptionGroup("Input & Output");
    Options::AddValueOption("-in", "BAM filename", "the input BAM file", "", m_settings->HasInputBamFilename, m_settings->InputBamFilename, IO_Opts, Options::StandardIn());
    
    OptionGroup* FilterOpts = Options::CreateOptionGroup("Filters");
    Options::AddValueOption("-region", "REGION", "genomic region. See README for more details", "", m_settings->HasRegion, m_settings->Region, FilterOpts);
}

CountTool::~CountTool(void) { 
    delete m_settings;
    m_settings = 0;
}

int CountTool::Help(void) { 
    Options::DisplayHelp();
    return 0;
} 

int CountTool::Run(int argc, char* argv[]) { 

    // parse command line arguments
    Options::Parse(argc, argv, 1);

    //open our BAM reader
//     BamReader reader;
//     reader.Open(m_settings.InputBamFilename);

    // count alignments
    string startChrom;
    string stopChrom;
    int startPos;
    int stopPos;
    
    if ( !m_settings->HasRegion ) {
        cerr << "Counting all alignments " << endl;
    } else {
        if ( ParseRegionString(m_settings->Region, startChrom, startPos, stopChrom, stopPos) ) {
            cerr << "Counting only alignments in region " << m_settings->Region << endl;
            cerr << "StartChrom: " << startChrom << endl;
            cerr << "StartPos:   " << startPos << endl;
            cerr << "StopChrom:  " << stopChrom << endl;
            cerr << "StopPos:    " << stopPos << endl;
        }
    }
     
    cerr << " from " << m_settings->InputBamFilename << endl;
    cerr << "FEATURE NOT YET IMPLEMENTED!" << endl;

    // clean & exit
//     reader.Close();
    return 0;
}