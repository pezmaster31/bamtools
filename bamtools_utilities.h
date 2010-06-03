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

struct Region {
    int StartChromID;
    int StopChromID;
    int StartPosition;
    int StopPosition;
};

class Utilities {
  
    public:                          
        // Parses a region string, uses reader to do validation (valid ID's, positions), stores in Region struct
        static bool ParseRegionString(const std::string& regionString, const BamReader& reader, Region& region);
};

} // namespace BamTools
  
#endif // BAMTOOLS_UTILITIES_H