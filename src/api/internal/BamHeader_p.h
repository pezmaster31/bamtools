// ***************************************************************************
// BamHeader_p.h (c) 2010 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 25 December 2010 (DB)
// ---------------------------------------------------------------------------
// Provides the basic functionality for handling BAM headers.
// ***************************************************************************

#ifndef BAMHEADER_P_H
#define BAMHEADER_P_H

//  -------------
//  W A R N I N G
//  -------------
//
// This file is not part of the BamTools API.  It exists purely as an
// implementation detail. This header file may change from version to version
// without notice, or even be removed.
//
// We mean it.

#include <api/SamHeader.h>
#include <string>

namespace BamTools {

class BgzfData;

namespace Internal {

class BamHeader {

    public:
        BamHeader(void);
        ~BamHeader(void);

    public:
        void Clear(void);
        bool IsValid(void) const;
        bool Load(BgzfData* stream);

    public:
        SamHeader ToSamHeader(void) const;
        std::string ToString(void) const;

    private:
        struct BamHeaderPrivate;
        BamHeaderPrivate* d;
};

} // namespace Internal
} // namespace BamTools

#endif // BAMHEADER_P_H
