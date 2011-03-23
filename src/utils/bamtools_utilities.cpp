// ***************************************************************************
// bamtools_utilities.cpp (c) 2010 Derek Barnett, Erik Garrison
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 26 January 2011
// ---------------------------------------------------------------------------
// Provides general utilities used by BamTools sub-tools.
// ***************************************************************************

#include <api/BamMultiReader.h>
#include <api/BamReader.h>
#include <utils/bamtools_utilities.h>
using namespace BamTools;

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
using namespace std;

namespace BamTools {
  
const char REVCOMP_LOOKUP[] = {'T',  0,  'G', 'H',
                                0,   0,  'C', 'D',
                                0,   0,   0,   0,
                               'K', 'N',  0,   0,
                                0,  'Y', 'W', 'A',
                               'A', 'B', 'S', 'X',
                               'R',  0 };
  
} // namespace BamTools 
  
// check if a file exists
bool Utilities::FileExists(const string& filename) {
    ifstream f(filename.c_str(), ifstream::in);
    return !f.fail();
}

// Parses a region string, does validation (valid ID's, positions), stores in Region struct
// Returns success (true/false)
bool Utilities::ParseRegionString(const string& regionString,
                                  const BamReader& reader,
                                  BamRegion& region)
{
    // -------------------------------
    // parse region string
  
    // check first for empty string
    if ( regionString.empty() ) 
        return false;   
    
    // non-empty string, look for a colom
    size_t foundFirstColon = regionString.find(':');
    
    // store chrom strings, and numeric positions
    string startChrom;
    string stopChrom;
    int startPos;
    int stopPos;
    
    // no colon found
    // going to use entire contents of requested chromosome 
    // just store entire region string as startChrom name
    // use BamReader methods to check if its valid for current BAM file
    if ( foundFirstColon == string::npos ) {
        startChrom = regionString;
        startPos   = 0;
        stopChrom  = regionString;
        stopPos    = -1;
    }
    
    // colon found, so we at least have some sort of startPos requested
    else {
      
        // store start chrom from beginning to first colon
        startChrom = regionString.substr(0,foundFirstColon);
        
        // look for ".." after the colon
        size_t foundRangeDots = regionString.find("..", foundFirstColon+1);
        
        // no dots found
        // so we have a startPos but no range
        // store contents before colon as startChrom, after as startPos
        if ( foundRangeDots == string::npos ) {
            startPos   = atoi( regionString.substr(foundFirstColon+1).c_str() ); 
            stopChrom  = startChrom;
            stopPos    = -1;
        } 
        
        // ".." found, so we have some sort of range selected
        else {
          
            // store startPos between first colon and range dots ".."
            startPos = atoi( regionString.substr(foundFirstColon+1, foundRangeDots-foundFirstColon-1).c_str() );
          
            // look for second colon
            size_t foundSecondColon = regionString.find(':', foundRangeDots+1);
            
            // no second colon found
            // so we have a "standard" chrom:start..stop input format (on single chrom)
            if ( foundSecondColon == string::npos ) {
                stopChrom  = startChrom;
                stopPos    = atoi( regionString.substr(foundRangeDots+2).c_str() );
            }
            
            // second colon found
            // so we have a range requested across 2 chrom's
            else {
                stopChrom  = regionString.substr(foundRangeDots+2, foundSecondColon-(foundRangeDots+2));
                stopPos    = atoi( regionString.substr(foundSecondColon+1).c_str() );
            }
        }
    }

    // -------------------------------
    // validate reference IDs & genomic positions
    
    const RefVector references = reader.GetReferenceData();
    
    // if startRefID not found, return false
    int startRefID = reader.GetReferenceID(startChrom);
    if ( startRefID == (int)references.size() ) return false;  
    
    // if startPos is larger than reference, return false
    const RefData& startReference = references.at(startRefID);
    if ( startPos > startReference.RefLength ) return false;
    
    // if stopRefID not found, return false
    int stopRefID = reader.GetReferenceID(stopChrom);
    if ( stopRefID == (int)references.size() ) return false;
    
    // if stopPosition larger than reference, return false
    const RefData& stopReference = references.at(stopRefID);
    if ( stopPos > stopReference.RefLength ) return false;
    
    // if no stopPosition specified, set to reference end
    if ( stopPos == -1 ) stopPos = stopReference.RefLength;  
    
    // -------------------------------
    // set up Region struct & return
    
    region.LeftRefID = startRefID;
    region.LeftPosition = startPos;
    region.RightRefID = stopRefID;;
    region.RightPosition = stopPos;
    return true;
}

// Same as ParseRegionString() above, but accepts a BamMultiReader
bool Utilities::ParseRegionString(const string& regionString,
                                  const BamMultiReader& reader,
                                  BamRegion& region)
{
    // -------------------------------
    // parse region string
  
    // check first for empty string
    if ( regionString.empty() ) 
        return false;   
    
    // non-empty string, look for a colom
    size_t foundFirstColon = regionString.find(':');
    
    // store chrom strings, and numeric positions
    string startChrom;
    string stopChrom;
    int startPos;
    int stopPos;
    
    // no colon found
    // going to use entire contents of requested chromosome 
    // just store entire region string as startChrom name
    // use BamReader methods to check if its valid for current BAM file
    if ( foundFirstColon == string::npos ) {
        startChrom = regionString;
        startPos   = 0;
        stopChrom  = regionString;
        stopPos    = -1;
    }
    
    // colon found, so we at least have some sort of startPos requested
    else {
      
        // store start chrom from beginning to first colon
        startChrom = regionString.substr(0,foundFirstColon);
        
        // look for ".." after the colon
        size_t foundRangeDots = regionString.find("..", foundFirstColon+1);
        
        // no dots found
        // so we have a startPos but no range
        // store contents before colon as startChrom, after as startPos
        if ( foundRangeDots == string::npos ) {
            startPos   = atoi( regionString.substr(foundFirstColon+1).c_str() ); 
            stopChrom  = startChrom;
            stopPos    = -1;
        } 
        
        // ".." found, so we have some sort of range selected
        else {
          
            // store startPos between first colon and range dots ".."
            startPos = atoi( regionString.substr(foundFirstColon+1, foundRangeDots-foundFirstColon-1).c_str() );
          
            // look for second colon
            size_t foundSecondColon = regionString.find(':', foundRangeDots+1);
            
            // no second colon found
            // so we have a "standard" chrom:start..stop input format (on single chrom)
            if ( foundSecondColon == string::npos ) {
                stopChrom  = startChrom;
                stopPos    = atoi( regionString.substr(foundRangeDots+2).c_str() );
            }
            
            // second colon found
            // so we have a range requested across 2 chrom's
            else {
                stopChrom  = regionString.substr(foundRangeDots+2, foundSecondColon-(foundRangeDots+2));
                stopPos    = atoi( regionString.substr(foundSecondColon+1).c_str() );
            }
        }
    }

    // -------------------------------
    // validate reference IDs & genomic positions
    
    const RefVector references = reader.GetReferenceData();
    
    // if startRefID not found, return false
    int startRefID = reader.GetReferenceID(startChrom);
    if ( startRefID == (int)references.size() ) return false;  
    
    // if startPos is larger than reference, return false
    const RefData& startReference = references.at(startRefID);
    if ( startPos > startReference.RefLength ) return false;
    
    // if stopRefID not found, return false
    int stopRefID = reader.GetReferenceID(stopChrom);
    if ( stopRefID == (int)references.size() ) return false;
    
    // if stopPosition larger than reference, return false
    const RefData& stopReference = references.at(stopRefID);
    if ( stopPos > stopReference.RefLength ) return false;
    
    // if no stopPosition specified, set to reference end
    if ( stopPos == -1 ) stopPos = stopReference.RefLength;  
    
    // -------------------------------
    // set up Region struct & return
    
    region.LeftRefID = startRefID;
    region.LeftPosition = startPos;
    region.RightRefID = stopRefID;;
    region.RightPosition = stopPos;
    return true;
}

void Utilities::Reverse(string& sequence) {
    reverse(sequence.begin(), sequence.end());
}

void Utilities::ReverseComplement(string& sequence) {
    
    // do complement, in-place
    size_t seqLength = sequence.length();
    for ( size_t i = 0; i < seqLength; ++i )
        sequence.replace(i, 1, 1, REVCOMP_LOOKUP[(int)sequence.at(i) - 65]);
    
    // reverse it
    Reverse(sequence);
}
