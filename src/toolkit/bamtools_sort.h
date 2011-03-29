// ***************************************************************************
// bamtools_sort.h (c) 2010 Derek Barnett, Erik Garrison
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 21 June 2010 (DB)
// ---------------------------------------------------------------------------
// Sorts a BAM file.
// ***************************************************************************

#ifndef BAMTOOLS_SORT_H
#define BAMTOOLS_SORT_H

#include "bamtools_tool.h"
#include "api/BamSortCriteria.h"
namespace BamTools {
  
class SortTool : public AbstractTool {
  
    public:
        SortTool(void);
        ~SortTool(void);
  
    public:
        int Help(void);
        int Run(int argc, char* argv[]); 
        
    private:
        struct SortSettings;
        SortSettings* m_settings;
        BamSortCriteria m_sort;
        struct SortToolPrivate;
        SortToolPrivate* m_impl;
};
  
} // namespace BamTools

#endif // BAMTOOLS_SORT_H
