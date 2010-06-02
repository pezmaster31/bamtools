// ***************************************************************************
// bamtools_index.h (c) 2010 Derek Barnett, Erik Garrison
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 1 June 2010
// ---------------------------------------------------------------------------
// Creates a BAM index (".bai") file for the provided BAM file
// ***************************************************************************

#ifndef BAMTOOLS_INDEX_H
#define BAMTOOLS_INDEX_H

#include "bamtools_tool.h"

namespace BamTools {
  
class IndexTool : public AbstractTool {
  
    public:
        IndexTool(void);
        ~IndexTool(void);
  
    public:
        int Help(void);
        int Run(int argc, char* argv[]); 
        
    private:
        struct IndexSettings;
        IndexSettings* m_settings;
};
  
} // namespace BamTools

#endif // BAMTOOLS_INDEX_H
