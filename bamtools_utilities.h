// ***************************************************************************
// bamtools_utilities.h (c) 2010 Derek Barnett, Erik Garrison
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 2 June 2010
// ---------------------------------------------------------------------------
// Provides general utilities used by BamTools sub-tools.
// ***************************************************************************

#ifndef BAMTOOLS_UTILITIES_H
#define BAMTOOLS_UTILITIES_H

#include <string>

namespace BamTools {

class BamReader;  
  
class Utilities {
  
    public:
        // Parses a REGION string, stores in (startChrom, startPos, stopChrom, stopPos) variables
        // Returns successful parse (true/false)
        static bool ParseRegionString(const std::string& regionString,
                                      std::string& startChrom,
                                      int& startPos,
                                      std::string& stopChrom,
                                      int& stopPos); 
};

} // namespace BamTools
  
#endif // BAMTOOLS_UTILITIES_H