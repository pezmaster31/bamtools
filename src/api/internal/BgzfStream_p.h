// ***************************************************************************
// BgzfStream_p.h (c) 2011 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// ---------------------------------------------------------------------------
// Last modified: 5 April 2011(DB)
// ---------------------------------------------------------------------------
// Based on BGZF routines developed at the Broad Institute.
// Provides the basic functionality for reading & writing BGZF files
// Replaces the old BGZF.* files to avoid clashing with other toolkits
// ***************************************************************************

#ifndef BGZFSTREAM_P_H
#define BGZFSTREAM_P_H

//  -------------
//  W A R N I N G
//  -------------
//
// This file is not part of the BamTools API.  It exists purely as an
// implementation detail. This header file may change from version to version
// without notice, or even be removed.
//
// We mean it.

#include <api/BamAux.h>
#include <api/BamConstants.h>
#include <api/IBamIODevice.h>
#include "zlib.h"
#include <cstdio>
#include <string>

namespace BamTools {
namespace Internal {

class BgzfStream {

    // constructor & destructor
    public:
        BgzfStream(void);
        ~BgzfStream(void);

    // main interface methods
    public:
        // closes BGZF file
        void Close(void);
        bool IsOpen(void) const;
        // opens the BGZF stream in requested mode
        bool Open(const std::string& filename, const char* mode);
        bool Open(const std::string& filename, const IBamIODevice::OpenMode mode);
        // reads BGZF data into a byte buffer
        unsigned int Read(char* data, const unsigned int dataLength);
        // seek to position in BGZF file
        bool Seek(const int64_t& position);
        // sets IO device (closes previous, if any, but does not attempt to open)
        void SetIODevice(IBamIODevice* device);
        // enable/disable compressed output
        void SetWriteCompressed(bool ok);
        // get file position in BGZF file
        int64_t Tell(void) const;
        // writes the supplied data into the BGZF buffer
        unsigned int Write(const char* data, const unsigned int dataLength);

    // internal methods
    private:
        // compresses the current block
        unsigned int DeflateBlock(void);
        // flushes the data in the BGZF block
        void FlushBlock(void);
        // de-compresses the current block
        int InflateBlock(const int& blockLength);
        // reads a BGZF block
        bool ReadBlock(void);

    // static 'utility' methods
    public:
        // checks BGZF block header
        static inline bool CheckBlockHeader(char* header);

    // data members
    public:
        unsigned int m_uncompressedBlockSize;
        unsigned int m_compressedBlockSize;
        unsigned int m_blockLength;
        unsigned int m_blockOffset;
        uint64_t     m_blockAddress;

        char* m_uncompressedBlock;
        char* m_compressedBlock;

        bool m_isOpen;
        bool m_isWriteOnly;
        bool m_isWriteCompressed;

        IBamIODevice* m_device;
        FILE* m_stream;
};

// -------------------------------------------------------------
// static 'utility' method implementations

// checks BGZF block header
inline
bool BgzfStream::CheckBlockHeader(char* header) {
    return (header[0] == Constants::GZIP_ID1 &&
            header[1] == (char)Constants::GZIP_ID2 &&
            header[2] == Z_DEFLATED &&
            (header[3] & Constants::FLG_FEXTRA) != 0 &&
            BamTools::UnpackUnsignedShort(&header[10]) == Constants::BGZF_XLEN &&
            header[12] == Constants::BGZF_ID1 &&
            header[13] == Constants::BGZF_ID2 &&
            BamTools::UnpackUnsignedShort(&header[14]) == Constants::BGZF_LEN );
}

} // namespace Internal
} // namespace BamTools

#endif // BGZFSTREAM_P_H
