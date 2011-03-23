// ***************************************************************************
// bamtools_utilities.h (c) 2010 Derek Barnett, Erik Garrison
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 26 January 2011
// ---------------------------------------------------------------------------
// Provides general utilities used by BamTools sub-tools.
// ***************************************************************************

#ifndef BAMTOOLS_UTILITIES_H
#define BAMTOOLS_UTILITIES_H

#include <api/BamAux.h>
#include <utils/utils_global.h>
#include <cassert>
#include <stdexcept>
#include <string>

#define BAMTOOLS_ASSERT_UNREACHABLE assert( false )
#define BAMTOOLS_ASSERT_MESSAGE( condition, message ) if (!( condition )) throw std::runtime_error( message );

namespace BamTools {

class BamReader;
class BamMultiReader;

class UTILS_EXPORT Utilities {
  
    public: 
        // check if a file exists
        static bool FileExists(const std::string& fname);
        
        // Parses a region string, uses reader to do validation (valid ID's, positions), stores in Region struct
        // Returns success (true/false)
        static bool ParseRegionString(const std::string& regionString,
                                      const BamReader& reader,
                                      BamRegion& region);
        // Same as above, but accepts a BamMultiReader
        static bool ParseRegionString(const std::string& regionString,
                                      const BamMultiReader& reader,
                                      BamRegion& region);

        // sequence utilities
        static void Reverse(std::string& sequence);
        static void ReverseComplement(std::string& sequence);
};

} // namespace BamTools
  
#endif // BAMTOOLS_UTILITIES_H
