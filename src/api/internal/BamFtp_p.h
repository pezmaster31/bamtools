// ***************************************************************************
// BamFtp_p.h (c) 2011 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// ---------------------------------------------------------------------------
// Last modified: 10 October 2011 (DB)
// ---------------------------------------------------------------------------
// Provides reading/writing of BAM files on FTP server
// ***************************************************************************

#ifndef BAMFTP_P_H
#define BAMFTP_P_H

//  -------------
//  W A R N I N G
//  -------------
//
// This file is not part of the BamTools API.  It exists purely as an
// implementation detail. This header file may change from version to version
// without notice, or even be removed.
//
// We mean it.

#include "api/IBamIODevice.h"
#include <string>

namespace BamTools {
namespace Internal {

class BamFtp : public IBamIODevice {

    // ctor & dtor
    public:
        BamFtp(const std::string& url);
        ~BamFtp(void);

    // IBamIODevice implementation
    public:
        void Close(void);
        bool IsRandomAccess(void) const;
        bool Open(const IBamIODevice::OpenMode mode);
        size_t Read(char* data, const unsigned int numBytes);
        bool Seek(const int64_t& position);
        int64_t Tell(void) const;
        size_t Write(const char* data, const unsigned int numBytes);

    // internal methods
    private:

    // data members
    private:
};

} // namespace Internal
} // namespace BamTools

#endif // BAMFTP_P_H
