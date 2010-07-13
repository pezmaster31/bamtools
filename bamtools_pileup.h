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