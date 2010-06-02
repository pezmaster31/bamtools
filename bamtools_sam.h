// ***************************************************************************
// bamtools_sam.h (c) 2010 Derek Barnett, Erik Garrison
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 1 June 2010
// ---------------------------------------------------------------------------
// Prints a BAM file in the text-based SAM format.
// ***************************************************************************

#ifndef BAMTOOLS_SAM_H
#define BAMTOOLS_SAM_H

#include "bamtools_tool.h"

namespace BamTools {
  
class SamTool : public AbstractTool {
  
    public:
        SamTool(void);
        ~SamTool(void);
  
    public:
        int Help(void);
        int Run(int argc, char* argv[]); 
        
    private:
        struct SamSettings;
        SamSettings* m_settings;
};
  
} // namespace BamTools

#endif // BAMTOOLS_SAM_H
