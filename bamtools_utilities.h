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
#include "BamAux.h"

namespace BamTools {

class BamReader;
class BamMultiReader;

class Utilities {
  
    public:                          
        // Parses a region string, uses reader to do validation (valid ID's, positions), stores in Region struct
        // Returns success (true/false)
        static bool ParseRegionString(const std::string& regionString, const BamReader& reader, BamRegion& region);
        // Same as above, but accepts a BamMultiReader
        static bool ParseRegionString(const std::string& regionString, const BamMultiReader& reader, BamRegion& region);

        // check if a file exists
        static bool FileExists(const std::string& fname); 
};

} // namespace BamTools
  
#endif // BAMTOOLS_UTILITIES_H
