// ***************************************************************************
// PBgzfStream_p.h (c) 2011 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// ---------------------------------------------------------------------------
// Last modified: 17 January 2012(DB)
// ---------------------------------------------------------------------------
// Based on BGZF routines developed at the Broad Institute.
// Provides the basic functionality for reading & writing BGZF files
// Replaces the old BGZF.* files to avoid clashing with other toolkits
// ***************************************************************************

#ifndef PBGZFSTREAM_P_H
#define PBGZFSTREAM_P_H

//  -------------
//  W A R N I N G
//  -------------
//
// This file is not part of the BamTools API.  It exists purely as an
// implementation detail. This header file may change from version to version
// without notice, or even be removed.
//
// We mean it.

#include "api/api_global.h"
#include "api/BamAux.h"
#include "api/IBamIODevice.h"
#include "api/internal/io/pbgzf/pbgzf.h"
#include <string>

namespace BamTools {
namespace Internal {

class PBgzfStream : public BgzfStream {

    // constructor & destructor
    public:
        PBgzfStream(void);
        ~PBgzfStream(void);

    // main interface methods
    public:
        // closes BGZF file
        void Close(void);
        // returns true if PBgzfStream open for IO
        bool IsOpen(void) const;
        // opens the BGZF file
        void Open(const std::string& filename, const IBamIODevice::OpenMode mode);
        // reads BGZF data into a byte buffer
        size_t Read(char* data, const size_t dataLength);
        // seek to position in BGZF file
        void Seek(const int64_t& position);
        // sets IO device (closes previous, if any, but does not attempt to open)
        void SetIODevice(IBamIODevice* device);
        // enable/disable compressed output
        void SetWriteCompressed(bool ok);
        // get file position in BGZF file
        int64_t Tell(void) const;
        // writes the supplied data into the BGZF buffer
        size_t Write(const char* data, const size_t dataLength);

    // internal methods
    // NB: these do not do anything
    private:
        // compresses the current block
        size_t DeflateBlock(int32_t blockLength);
        // flushes the data in the BGZF block
        void FlushBlock(void);
        // de-compresses the current block
        size_t InflateBlock(const size_t& blockLength);
        // reads a BGZF block
        void ReadBlock(void);

    // static 'utility' methods
    public:
        // checks BGZF block header
        static bool CheckBlockHeader(char* header);

    // data members
    public:
        // TODO...
        /*
        int32_t m_blockLength;
        int32_t m_blockOffset;
        int64_t m_blockAddress;

        bool m_isWriteCompressed;
        IBamIODevice* m_device;

        RaiiBuffer m_uncompressedBlock;
        RaiiBuffer m_compressedBlock;
        */

        PBGZF *m_pbgzf;
};

} // namespace Internal
} // namespace BamTools

#endif // PBGZFSTREAM_P_H
