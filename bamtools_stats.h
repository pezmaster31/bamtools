// ***************************************************************************
// bamtools_stats.h (c) 2010 Derek Barnett, Erik Garrison
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 1 June 2010
// ---------------------------------------------------------------------------
// Prints general statistics for a single BAM file.
//
// ** Expand to multiple? **
//
// ***************************************************************************

#ifndef BAMTOOLS_STATS_H
#define BAMTOOLS_STATS_H

#include "bamtools_tool.h"

namespace BamTools {
  
class StatsTool : public AbstractTool {
  
    public:
        StatsTool(void);
        ~StatsTool(void);
  
    public:
        int Help(void);
        int Run(int argc, char* argv[]); 
        
    private:
        struct StatsSettings;
        StatsSettings* m_settings;
};
  
} // namespace BamTools

#endif // BAMTOOLS_STATS_H
