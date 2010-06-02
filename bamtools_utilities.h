// ***************************************************************************
// bamtools_utilities.h (c) 2010 Derek Barnett, Erik Garrison
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 27 May 2010
// ---------------------------------------------------------------------------
// Provides general utilities used by BamTools sub-tools.
// ***************************************************************************

#ifndef BAMTOOLS_UTILITIES_H
#define BAMTOOLS_UTILITIES_H

#include <cstdlib>
#include <iostream>
#include <string>

namespace BamTools {

// Parses a REGION string, stores in (startChrom, startPos, stopChrom, stopPos) variables
// Returns successful parse (true/false)
static inline
bool ParseRegionString(const std::string& regionString, std::string& startChrom, int& startPos, std::string& stopChrom, int& stopPos) {
    
    // shouldn't call this function with empty string but worth checking 
    // checked first for clarity purposes later on, since we can assume at least some content in the string
    if ( regionString.empty() ) { 
        std::cerr << "Empty REGION. Usual format (e.g. chr2:1000..2000). See README for more detailed uses." << std::endl;
        return false; 
    }
  
    // non-empty string, look for a colom
    size_t foundFirstColon = regionString.find(':');
    
    // no colon found
    // going to use entire contents of requested chromosome 
    // just store entire region string as startChrom name
    // use BamReader methods to check if its valid for current BAM file
    if ( foundFirstColon == std::string::npos ) {
        startChrom = regionString;
        startPos   = -1;                                                        // ** not sure about these defaults (should stopChrom == startChrom if same?)
        stopChrom  = "";
        stopPos    = -1;
        return true;
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
        if ( foundRangeDots == std::string::npos ) {
            startPos   = atoi( regionString.substr(foundFirstColon+1).c_str() ); 
            stopChrom  = "";
            stopPos    = -1;
            return true;
        } 
        
        // ".." found, so we have some sort of range selected
        else {
          
            // store startPos between first colon and range dots ".."
            startPos = atoi( regionString.substr(foundFirstColon+1, foundRangeDots-foundFirstColon-1).c_str() );
          
            // look for second colon
            size_t foundSecondColon = regionString.find(':', foundRangeDots+1);
            
            // no second colon found
            // so we have a "standard" chrom:start..stop input format (on single chrom)
            if ( foundSecondColon == std::string::npos ) {
                stopChrom  = "";
                stopPos    = atoi( regionString.substr(foundRangeDots+2).c_str() );
                return true;
            }
            
            // second colon found
            // so we have a range requested across 2 chrom's
            else {
                stopChrom  = regionString.substr(foundRangeDots+2, regionString.length()-foundSecondColon-1);
                stopPos    = atoi( regionString.substr(foundSecondColon+1).c_str() );
                return true;
            }
        }
    }
  
    // shouldn't get here - any code path that does?
    // if not, what does true/false really signify?
    return false;
}

} // namespace BamTools
  
#endif // BAMTOOLS_UTILITIES_H