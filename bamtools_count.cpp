// ***************************************************************************
// bamtools_count.cpp (c) 2010 Derek Barnett, Erik Garrison
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 2 June 2010
// ---------------------------------------------------------------------------
// Prints alignment count for BAM file
//
// ** Expand to multiple?? 
//
// ***************************************************************************

#include <iostream>
#include <sstream>
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
    bool HasBamIndexFilename;
    bool HasInputBamFilename;
    bool HasRegion;

    // filenames
    string BamIndexFilename;
    string InputBamFilename;
    string Region;
    
    // constructor
    CountSettings(void)
        : HasBamIndexFilename(false)
        , HasInputBamFilename(false)
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
    Options::SetProgramInfo("bamtools count", "prints alignment counts for a BAM file", "-in <filename> [-region <REGION> [-index <filename>]]");
    
    // set up options 
    OptionGroup* IO_Opts = Options::CreateOptionGroup("Input & Output");
    Options::AddValueOption("-in",    "BAM filename",        "the input BAM file", "", m_settings->HasInputBamFilename, m_settings->InputBamFilename, IO_Opts, Options::StandardIn());
    Options::AddValueOption("-index", "BAM index filename", "the BAM index file",  "", m_settings->HasBamIndexFilename, m_settings->BamIndexFilename, IO_Opts);
    
    OptionGroup* FilterOpts = Options::CreateOptionGroup("Filters");
    Options::AddValueOption("-region", "REGION", "genomic region. Index file is recommended for better performance. See \'bamtools help index\' for more details on creating one", "", m_settings->HasRegion, m_settings->Region, FilterOpts);
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
    BamReader reader;
    reader.Open(m_settings->InputBamFilename);

    // alignment counter
    int alignmentCount(0);
    
    // set up error handling
    ostringstream errorStream("");
    bool foundError(false);
    
    // if no region specified, count entire file 
    if ( !m_settings->HasRegion ) {
        BamAlignment al;
        while ( reader.GetNextAlignment(al) ) 
            ++alignmentCount;
    } 
    
    // more complicated - region specified
    else {
        
        Region region;
        if ( Utilities::ParseRegionString(m_settings->Region, reader, region) ) {

            // has index, we can jump directly to 
            if ( m_settings->HasBamIndexFilename ) {
              
                // this is kind of a hack...?
                // re-open BamReader, this time with the index file
                reader.Close();
                reader.Open(m_settings->InputBamFilename, m_settings->BamIndexFilename);
              
                // attempt Jump(), catch error
               // if ( !reader.Jump(region.StartChromID, region.StartPosition) ) {
               //     foundError = true;
               //     errorStream << "Could not jump to desired REGION: " << m_settings->Region << endl;
               // }
               //
                if ( !reader.SetRegion(region.StartChromID, region.StartPosition, region.StopChromID, region.StopPosition) ) {
                   foundError = true;
                   errorStream << "Could not set BamReader region to REGION: " << m_settings->Region << endl;
                }

            }
            
            else {
              
                // read through sequentially, until first overlapping read is found
                BamAlignment al;
                bool alignmentFound(false);
                while( reader.GetNextAlignment(al) ) {
                    if ( (al.RefID == region.StartChromID ) && ( (al.Position + al.Length) >= region.StartPosition) ) {
                        alignmentFound = true;
                        break;
                    }
                }
                
                // if overlapping alignment found (not EOF), increment counter
                if ( alignmentFound) ++ alignmentCount; 
            }
            
            // -----------------------------
            // count alignments until stop hit
            
            if ( !foundError ) {
                // while valid alignment AND
                // ( either current ref is before stopRefID OR
                //   current ref stopRefID but we're before stopPos )
                BamAlignment al;
                //while ( reader.GetNextAlignment(al) && ((al.RefID < region.StopChromID ) || (al.RefID == region.StopChromID && al.Position <= region.StopPosition)) )
                //    ++alignmentCount;
                //
                while ( reader.GetNextAlignment(al) )
                    ++alignmentCount;

            }

        } else {
            foundError = true;
            errorStream << "Could not parse REGION: " << m_settings->Region << endl;
            errorStream << "Be sure REGION is in valid format (see README) and that coordinates are valid for selected references" << endl;
        }
    }
     
    // print errors OR results 
    if ( foundError )
        cerr << errorStream.str() << endl;
    else
        cout << alignmentCount << endl;
    
    // clean & exit
    reader.Close();
    return (int)foundError;
}
