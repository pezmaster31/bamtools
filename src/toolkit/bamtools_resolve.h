// ***************************************************************************
// bamtools_resolve.h (c) 2011 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 19 April 2011
// ---------------------------------------------------------------------------
// Resolves paired-end reads (marking the IsProperPair flag as needed) in a
// BAM file.
// ***************************************************************************

#ifndef BAMTOOLS_RESOLVE_H
#define BAMTOOLS_RESOLVE_H

#include "bamtools_tool.h"

namespace BamTools {

class ResolveTool : public AbstractTool {

    public:
        ResolveTool(void);
        ~ResolveTool(void);

    public:
        int Help(void);
        int Run(int argc, char* argv[]);

    private:
        struct ResolveSettings;
        ResolveSettings* m_settings;

        struct ResolveToolPrivate;
        ResolveToolPrivate* m_impl;
};

} // namespace BamTools

#endif // BAMTOOLS_RESOLVE_H
