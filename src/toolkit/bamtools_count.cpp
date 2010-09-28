// ***************************************************************************
// bamtools_count.cpp (c) 2010 Derek Barnett, Erik Garrison
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 3 September 2010
// ---------------------------------------------------------------------------
// Prints alignment count for BAM file(s)
// ***************************************************************************

#include <iostream>
#include <string>
#include <vector>

#include "bamtools_count.h"
#include "bamtools_options.h"
#include "bamtools_utilities.h"
#include "BamReader.h"
#include "BamMultiReader.h"

using namespace std;
using namespace BamTools;
  
// ---------------------------------------------  
// CountSettings implementation

struct CountTool::CountSettings {

    // flags
    bool HasInput;
    bool HasRegion;

    // filenames
    vector<string> InputFiles;
    string Region;
    
    // constructor
    CountSettings(void)
        : HasInput(false)
        , HasRegion(false)
    { }  
}; 
  
// ---------------------------------------------
// CountTool implementation

CountTool::CountTool(void) 
    : AbstractTool()
    , m_settings(new CountSettings)
{ 
    // set program details
    Options::SetProgramInfo("bamtools count", "prints alignment counts for a BAM file", "[-in <filename> -in <filename> ...] [-region <REGION>]");
    
    // set up options 
    OptionGroup* IO_Opts = Options::CreateOptionGroup("Input & Output");
    Options::AddValueOption("-in",  "BAM filename", "the input BAM file(s)", "", m_settings->HasInput,  m_settings->InputFiles, IO_Opts, Options::StandardIn());
    
    OptionGroup* FilterOpts = Options::CreateOptionGroup("Filters");
    Options::AddValueOption("-region", "REGION", "genomic region. Index file is required and is read automatically if it exists as <filename>.bai or <filename>.bti. See \'bamtools help index\' for more details on creating one", "", m_settings->HasRegion, m_settings->Region, FilterOpts);
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

    // if no '-in' args supplied, default to stdin
    if ( !m_settings->HasInput ) 
        m_settings->InputFiles.push_back(Options::StandardIn());
    
    // open reader without index
    BamMultiReader reader;
    if (!reader.Open(m_settings->InputFiles, false, true)) {
        cerr << "ERROR: Could not open input BAM file(s)... Aborting." << endl;
        return 1;
    }

    // alignment counter
    BamAlignment al;
    int alignmentCount(0);
    
    // if no region specified, count entire file 
    if ( !m_settings->HasRegion ) {
        while ( reader.GetNextAlignmentCore(al) ) 
            ++alignmentCount;
    }
    
    // otherwise attempt to use region as constraint
    else {
        
        // if region string parses OK
        BamRegion region;
        if ( Utilities::ParseRegionString(m_settings->Region, reader, region) ) {

            // attempt to re-open reader with index files
            reader.Close();
            bool openedOK = reader.Open(m_settings->InputFiles, true, true );
            
            // if error
            if ( !openedOK ) {
                cerr << "ERROR: Could not open input BAM file(s)... Aborting." << endl;
                return 1;
            }
            
            // if index data available, we can use SetRegion
            if ( reader.IsIndexLoaded() ) {
              
                // attempt to use SetRegion(), if failed report error
                if ( !reader.SetRegion(region.LeftRefID, region.LeftPosition, region.RightRefID, region.RightPosition) ) {
                    cerr << "ERROR: Region requested, but could not set BamReader region to REGION: " << m_settings->Region << " Aborting." << endl;
                    reader.Close();
                    return 1;
                } 
              
                // everything checks out, just iterate through specified region, counting alignments
                while ( reader.GetNextAlignmentCore(al) )
                    ++alignmentCount;
            } 
            
            // no index data available, we have to iterate through until we
            // find overlapping alignments
            else {
                while( reader.GetNextAlignmentCore(al) ) {
                    if ( (al.RefID >= region.LeftRefID)  && ( (al.Position + al.Length) >= region.LeftPosition ) &&
                          (al.RefID <= region.RightRefID) && ( al.Position <= region.RightPosition) ) 
                    {
                        ++alignmentCount;
                    }
                }
            }
        } 
        
        // error parsing REGION string
        else {
            cerr << "ERROR: Could not parse REGION - " << m_settings->Region << endl;
            cerr << "Be sure REGION is in valid format (see README) and that coordinates are valid for selected references" << endl;
            reader.Close();
            return 1;
        }
    }
    
    // print results 
    cout << alignmentCount << endl;
    
    // clean & exit
    reader.Close();
    return 0;
}
