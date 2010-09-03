// ***************************************************************************
// bamtools_random.cpp (c) 2010 Derek Barnett, Erik Garrison
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 3 September 2010 (DB)
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

// utility methods for RandomTool
const int getRandomInt(const int& lowerBound, const int& upperBound) {
    const int range = (upperBound - lowerBound) + 1;
    return ( lowerBound + (int)(range * (double)rand()/((double)RAND_MAX + 1)) );
}
    
} // namespace BamTools
  
// ---------------------------------------------  
// RandomSettings implementation

struct RandomTool::RandomSettings {

    // flags
    bool HasAlignmentCount;
    bool HasInput;
    bool HasOutput;
    bool HasRegion;
    bool IsForceCompression;

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
        , IsForceCompression(false)
        , AlignmentCount(RANDOM_MAX_ALIGNMENT_COUNT)
        , OutputFilename(Options::StandardOut())
    { }  
};  

// ---------------------------------------------
// RandomTool implementation

RandomTool::RandomTool(void) 
    : AbstractTool()
    , m_settings(new RandomSettings)
{ 
    // set program details
    Options::SetProgramInfo("bamtools random", "grab a random subset of alignments", "[-in <filename> -in <filename> ...] [-out <filename>] [-forceCompression] [-n] [-region <REGION>]");
    
    // set up options 
    OptionGroup* IO_Opts = Options::CreateOptionGroup("Input & Output");
    Options::AddValueOption("-in",  "BAM filename", "the input BAM file",  "", m_settings->HasInput,  m_settings->InputFiles,     IO_Opts, Options::StandardIn());
    Options::AddValueOption("-out", "BAM filename", "the output BAM file", "", m_settings->HasOutput, m_settings->OutputFilename, IO_Opts, Options::StandardOut());
    Options::AddOption("-forceCompression", "if results are sent to stdout (like when piping to another tool), default behavior is to leave output uncompressed. Use this flag to override and force compression", m_settings->IsForceCompression, IO_Opts);
    
    OptionGroup* FilterOpts = Options::CreateOptionGroup("Filters");
    Options::AddValueOption("-n", "count", "number of alignments to grab.  Note - no duplicate checking is performed", "", m_settings->HasAlignmentCount, m_settings->AlignmentCount, FilterOpts, RANDOM_MAX_ALIGNMENT_COUNT);
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

    // parse command line arguments
    Options::Parse(argc, argv, 1);

    // set to default stdin if no input files provided
    if ( !m_settings->HasInput ) 
        m_settings->InputFiles.push_back(Options::StandardIn());
    
    // open our reader
    BamMultiReader reader;
    if ( !reader.Open(m_settings->InputFiles) ) {
        cerr << "ERROR: Could not open input BAM file(s)." << endl;
        return 1;
    }
     
    // make sure index data is available
    if ( !reader.IsIndexLoaded() ) {
        cerr << "ERROR: Could not load index data for all input BAM file(s)." << endl;
        cerr << "\'bamtools random\' requires valid index files to provide efficient performance." << endl;
        reader.Close();
        return 1;
      
    }
     
    // get BamReader metadata  
    const string headerText = reader.GetHeaderText();
    const RefVector references = reader.GetReferenceData();
    if ( references.empty() ) {
        cerr << "ERROR: No reference data available - required to perform random access throughtout input file(s)." << endl;
        reader.Close();
        return 1;
    }
    
    // open our writer
    BamWriter writer;
    bool writeUncompressed = ( m_settings->OutputFilename == Options::StandardOut() && !m_settings->IsForceCompression );
    if ( !writer.Open(m_settings->OutputFilename, headerText, references, writeUncompressed) ) {
        cerr << "ERROR: Could not open BamWriter." << endl;
        reader.Close();
        return 1;
    }

    // if user specified a REGION constraint, attempt to parse REGION string 
    BamRegion region; 
    if ( m_settings->HasRegion && !Utilities::ParseRegionString(m_settings->Region, reader, region) ) {
        cerr << "ERROR: Could not parse REGION: " << m_settings->Region << endl;
        cerr << "Be sure REGION is in valid format (see README) and that coordinates are valid for selected references" << endl;
        reader.Close();
        writer.Close();
        return 1;
    }
      
    // seed our random number generator
    srand( time(NULL) );
    
    // grab random alignments 
    BamAlignment al;
    unsigned int i = 0;
    while ( i < m_settings->AlignmentCount ) {
      
        int randomRefId    = 0;
        int randomPosition = 0;
      
        // use REGION constraints to select random refId & position
        if ( m_settings->HasRegion ) {
          
            // select a random refId
            randomRefId = getRandomInt(region.LeftRefID, region.RightRefID);
            
            // select a random position based on randomRefId
            const int lowerBoundPosition = ( (randomRefId == region.LeftRefID)  ? region.LeftPosition  : 0 );
            const int upperBoundPosition = ( (randomRefId == region.RightRefID) ? region.RightPosition : (references.at(randomRefId).RefLength - 1) );
            randomPosition = getRandomInt(lowerBoundPosition, upperBoundPosition);
        } 
        
        // otherwise select from all possible random refId & position
        else {
          
            // select random refId
            randomRefId = getRandomInt(0, (int)references.size() - 1);
            
            // select random position based on randomRefId
            const int lowerBoundPosition = 0;
            const int upperBoundPosition = references.at(randomRefId).RefLength - 1;
            randomPosition = getRandomInt(lowerBoundPosition, upperBoundPosition); 
        }
      
        // if jump & read successful, save first alignment that overlaps random refId & position
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
    
    // cleanup & exit
    reader.Close();
    writer.Close();
    return 0;
}