// ***************************************************************************
// bamtools_coverage.h (c) 2010 Derek Barnett, Erik Garrison
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 1 June 2010
// ---------------------------------------------------------------------------
// Prints coverage statistics for a single BAM file 
//
// ** Expand to multiple?? 
//
// ***************************************************************************

#ifndef BAMTOOLS_COVERAGE_H
#define BAMTOOLS_COVERAGE_H

#include "bamtools_tool.h"

namespace BamTools {
  
class CoverageTool : public AbstractTool {
  
    public:
        CoverageTool(void);
        ~CoverageTool(void);
  
    public:
        int Help(void);
        int Run(int argc, char* argv[]); 
        
    private:  
        struct CoverageSettings;
        CoverageSettings* m_settings;
};
  
} // namespace BamTools

#endif // BAMTOOLS_COVERAGE_H
