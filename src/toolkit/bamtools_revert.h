// ***************************************************************************
// bamtools_stats.h (c) 2010 Derek Barnett, Alistair Ward
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 5 December 2010
// ---------------------------------------------------------------------------
//
// ***************************************************************************

#ifndef BAMTOOLS_REVERT_H
#define BAMTOOLS_REVERT_H

#include "bamtools_tool.h"

namespace BamTools {
  
class RevertTool : public AbstractTool {
  
    public:
        RevertTool(void);
        ~RevertTool(void);
  
    public:
        int Help(void);
        int Run(int argc, char* argv[]); 
        
    private:
        struct RevertSettings;
        RevertSettings* m_settings;
        
        struct RevertToolPrivate;
        RevertToolPrivate* m_impl;
};
  
} // namespace BamTools

#endif // BAMTOOLS_REVERT_H
