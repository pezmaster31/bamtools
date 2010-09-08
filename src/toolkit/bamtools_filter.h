// ***************************************************************************
// bamtools_filter.h (c) 2010 Derek Barnett, Erik Garrison
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 28 August 2010
// ---------------------------------------------------------------------------
// Filters a single BAM file (or filters multiple BAM files and merges) 
// according to some user-specified criteria.
// ***************************************************************************

#ifndef BAMTOOLS_FILTER_H
#define BAMTOOLS_FILTER_H

#include "bamtools_tool.h"

namespace BamTools {
  
class FilterTool : public AbstractTool {
  
    public:
        FilterTool(void);
        ~FilterTool(void);
  
    public:
        int Help(void);
        int Run(int argc, char* argv[]); 
        
    private:
        struct FilterSettings;
        FilterSettings* m_settings;
        
        struct FilterToolPrivate;
        FilterToolPrivate* m_impl;
};
  
} // namespace BamTools

#endif // BAMTOOLS_FILTER_H
