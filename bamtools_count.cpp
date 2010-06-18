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
    Options::SetProgramInfo("bamtools count", "prints alignment counts for a BAM file", "-in <filename> [-region <REGION>]");
    
    // set up options 
    OptionGroup* IO_Opts = Options::CreateOptionGroup("Input & Output");
    //Options::AddValueOption("-in",    "BAM filename",        "the input BAM file", "", m_settings->HasInputBamFilename, m_settings->InputBamFilename, IO_Opts, Options::StandardIn());
    Options::AddValueOption("-in",  "BAM filename", "the input BAM file(s)", "", m_settings->HasInput,  m_settings->InputFiles, IO_Opts);
    //Options::AddValueOption("-index", "BAM index filename", "the BAM index file",  "", m_settings->HasBamIndexFilename, m_settings->BamIndexFilename, IO_Opts);
    
    OptionGroup* FilterOpts = Options::CreateOptionGroup("Filters");
    Options::AddValueOption("-region", "REGION", "genomic region. Index file is recommended for better performance, and is read automatically if it exists as <filename>.bai. See \'bamtools help index\' for more details on creating one", "", m_settings->HasRegion, m_settings->Region, FilterOpts);
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

    BamMultiReader reader;
    reader.Open(m_settings->InputFiles, false, true);

    // alignment counter
    int alignmentCount(0);
    
    // set up error handling
    ostringstream errorStream("");
    bool foundError(false);
    
    // if no region specified, count entire file 
    if ( !m_settings->HasRegion ) {
        BamAlignment al;
        while ( reader.GetNextAlignmentCore(al) ) 
            ++alignmentCount;
    }
    
    // more complicated - region specified
    else {
        
        BamRegion region;
        if ( Utilities::ParseRegionString(m_settings->Region, reader, region) ) {

            // check if there are index files *.bai corresponding to the input files
            bool hasIndexes = true;
            for (vector<string>::const_iterator f = m_settings->InputFiles.begin(); f != m_settings->InputFiles.end(); ++f) {
                if (!Utilities::FileExists(*f + ".bai")) {
                    errorStream << "could not find index file " << *f + ".bai" 
                        << ", parsing whole file(s) to get alignment counts for target region" << endl;
                    hasIndexes = false;
                    break;
                }
            }
            // has index, we can jump directly to 
            if ( hasIndexes ) {
              
                // this is kind of a hack...?
                BamMultiReader reader;
                reader.Open(m_settings->InputFiles, true, true);
              
                if ( !reader.SetRegion(region.LeftRefID, region.LeftPosition, region.RightRefID, region.RightPosition) ) {
                   foundError = true;
                   errorStream << "Could not set BamReader region to REGION: " << m_settings->Region << endl;
                } else {
                    BamAlignment al;
                    while ( reader.GetNextAlignmentCore(al) )
                        ++alignmentCount;
                }

            } else {
              
                // read through sequentially, until first overlapping read is found
                BamAlignment al;
                bool alignmentFound(false);
                //reader.Open(m_settings->InputFiles, false, true);
                while( reader.GetNextAlignmentCore(al) ) {
                    if ( (al.RefID == region.LeftRefID) && ( (al.Position + al.Length) >= region.LeftPosition ) ) {
                        alignmentFound = true;
                        break;
                    }
                }
                
                // if overlapping alignment found (not EOF), increment counter
                if ( alignmentFound ) ++ alignmentCount;
            }
            
            // -----------------------------
            // count alignments until stop hit
            
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
