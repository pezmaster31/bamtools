// ***************************************************************************
// bamtools_pileup.h (c) 2010 Derek Barnett, Erik Garrison
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 13 July 2010
// ---------------------------------------------------------------------------
// Provides pileup conversion functionality.  
// 
// The 'assembly' aspect of pileup makes this more complicated than the 
// simpler one-to-one conversion methods for other formats.
// ***************************************************************************

#ifndef BAMTOOLS_PILEUP_H
#define BAMTOOLS_PILEUP_H

#include <iostream>
#include <string>

namespace BamTools {

class BamMultiReader;
class BamRegion;

class Pileup {
  
    public:
        Pileup(BamMultiReader* reader, std::ostream* outStream);
        ~Pileup(void);
        
    public:
        bool Run(void);
        void SetFastaFilename(const std::string& filename);
        void SetIsPrintingMapQualities(bool ok);  
        void SetRegion(const BamRegion& region);
  
    private:
        struct PileupPrivate;
        PileupPrivate* d;
};

} // namespace BamTools

#endif // BAMTOOLS_PILEUP_H