// ***************************************************************************
// bamtools_random.cpp (c) 2010 Derek Barnett, Erik Garrison
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 20 July 2010 (DB)
// ---------------------------------------------------------------------------
// Grab a random subset of alignments.
// ***************************************************************************

#include <ctime>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include "bamtools_random.h"
#include "bamtools_options.h"
#include "bamtools_utilities.h"
#include "BamMultiReader.h"
#include "BamWriter.h"
using namespace std;
using namespace BamTools; 
  
namespace BamTools {
  
    // define constants
    const unsigned int RANDOM_MAX_ALIGNMENT_COUNT = 10000;
    
} // namespace BamTools
  
// ---------------------------------------------  
// RandomSettings implementation

struct RandomTool::RandomSettings {

    // flags
    bool HasAlignmentCount;
    bool HasInput;
    bool HasOutput;
    bool HasRegion;

    // parameters
    unsigned int AlignmentCount;
    vector<string> InputFiles;
    string OutputFilename;
    string Region;
    
    // constructor
    RandomSettings(void)
        : HasAlignmentCount(false)
        , HasInput(false)
        , HasOutput(false)
        , HasRegion(false)
        , AlignmentCount(RANDOM_MAX_ALIGNMENT_COUNT)
    { }  
};  

// ---------------------------------------------
// RandomTool implementation

RandomTool::RandomTool(void) 
    : AbstractTool()
    , m_settings(new RandomSettings)
{ 
    // set program details
    Options::SetProgramInfo("bamtools random", "grab a random subset of alignments", "[-in <filename> -in <filename> ...] [-out <filename>] [-region <REGION>]");
    
    // set up options 
    OptionGroup* IO_Opts = Options::CreateOptionGroup("Input & Output");
    Options::AddValueOption("-in",  "BAM filename", "the input BAM file",  "", m_settings->HasInput,  m_settings->InputFiles,     IO_Opts, Options::StandardIn());
    Options::AddValueOption("-out", "BAM filename", "the output BAM file", "", m_settings->HasOutput, m_settings->OutputFilename, IO_Opts, Options::StandardOut());
    
    OptionGroup* FilterOpts = Options::CreateOptionGroup("Filters");
    Options::AddValueOption("-n", "count", "number of alignments to grab.  Note - no duplicate checking is performed (currently)", "", m_settings->HasAlignmentCount, m_settings->AlignmentCount, FilterOpts, RANDOM_MAX_ALIGNMENT_COUNT);
    Options::AddValueOption("-region", "REGION", "limit source of random alignment subset to a particular genomic region. Index file is recommended for better performance, and is read automatically if it exists as <filename>.bai or <filename>.bti. See \'bamtools help index\' for more details on creating one", "", m_settings->HasRegion, m_settings->Region, FilterOpts);
}

RandomTool::~RandomTool(void) { 
    delete m_settings;
    m_settings = 0;
}

int RandomTool::Help(void) { 
    Options::DisplayHelp();
    return 0;
} 

int RandomTool::Run(int argc, char* argv[]) { 

    // TODO: Handle BAM input WITHOUT index files.
  
    // parse command line arguments
    Options::Parse(argc, argv, 1);

    // set to default input if none provided
    if ( !m_settings->HasInput ) 
        m_settings->InputFiles.push_back(Options::StandardIn());
    
    // open our BAM reader
    BamMultiReader reader;
    reader.Open(m_settings->InputFiles);
    string headerText    = reader.GetHeaderText();
    RefVector references = reader.GetReferenceData();
    
    // check that reference data is available, used for generating random jumps
    if ( references.empty() ) {
        cerr << "No reference data available... quitting." << endl;
        reader.Close();
        return 1;
    }
    
    // see if user specified a REGION
    BamRegion region;
    if ( m_settings->HasRegion ) {   
        if ( Utilities::ParseRegionString(m_settings->Region, reader, region) )
            reader.SetRegion(region);
    }
    
    // open out BAM writer
    BamWriter writer;
    writer.Open(m_settings->OutputFilename, headerText, references);
    
    // seed our random number generator
    srand (time(NULL) );
    
    // grab random alignments 
    BamAlignment al;
    unsigned int i = 0;
    while ( i < m_settings->AlignmentCount ) {
      
        int randomRefId    = 0;
        int randomPosition = 0;
      
        // use REGION constraints to generate random refId & position
        if ( m_settings->HasRegion ) {
          
            int lowestRefId  = region.LeftRefID;
            int highestRefId = region.RightRefID;
            int rangeRefId   = (highestRefId - lowestRefId) + 1;
            randomRefId = lowestRefId + (int)(rangeRefId * (double)(rand()/((double)RAND_MAX + 1)));
            
            int lowestPosition  = ( (randomRefId == region.LeftRefID)  ? region.LeftPosition  : 0 );
            int highestPosition = ( (randomRefId == region.RightRefID) ? region.RightPosition : references.at(randomRefId).RefLength - 1 ); 
            int rangePosition = (highestPosition - lowestPosition) + 1;
            randomPosition = lowestPosition + (int)(rangePosition * (double)(rand()/((double)RAND_MAX + 1))); 
        } 
        
        // otherwise generate 'normal' random refId & position
        else {
          
            // generate random refId
            int lowestRefId = 0;
            int highestRefId = references.size() - 1;
            int rangeRefId = (highestRefId - lowestRefId) + 1;            
            randomRefId = lowestRefId + (int)(rangeRefId * (double)(rand()/((double)RAND_MAX + 1)));
            
            // generate random position
            int lowestPosition = 0;
            int highestPosition = references.at(randomRefId).RefLength - 1;
            int rangePosition = (highestPosition - lowestPosition) + 1;
            randomPosition = lowestPosition + (int)(rangePosition * (double)(rand()/((double)RAND_MAX + 1))); 
        }
      
        // if jump & read successful, save alignment
        if ( reader.Jump(randomRefId, randomPosition) ) {
            while ( reader.GetNextAlignmentCore(al) ) {
                if ( al.RefID == randomRefId && al.Position >= randomPosition ) {
                    writer.SaveAlignment(al);
                    ++i;
                    break;
                }
            }
        }
    }
    
    // close reader & writer
    reader.Close();
    writer.Close();
    return 0;
}