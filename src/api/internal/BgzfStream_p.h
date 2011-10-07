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
#include "zlib.h"
#include <cstdio>
#include <memory>
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
        // opens the BGZF file (mode is either "rb" for reading, or "wb" for writing)
        void Open(const std::string& filename, const char* mode);
        // reads BGZF data into a byte buffer
        size_t Read(char* data, const size_t dataLength);
        // seek to position in BGZF file
        void Seek(const int64_t& position);
        // enable/disable compressed output
        void SetWriteCompressed(bool ok);
        // get file position in BGZF file
        int64_t Tell(void) const;
        // writes the supplied data into the BGZF buffer
        size_t Write(const char* data, const size_t dataLength);

    // internal methods
    private:
        // compresses the current block
        size_t DeflateBlock(void);
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
        unsigned int BlockLength;
        unsigned int BlockOffset;
        int64_t BlockAddress;
        bool IsOpen;
        bool IsWriteOnly;
        bool IsWriteCompressed;

        struct RaiiWrapper {
            RaiiWrapper(void);
            ~RaiiWrapper(void);
            char* UncompressedBlock;
            char* CompressedBlock;
            FILE* Stream;
        };
        RaiiWrapper Resources;

};

} // namespace Internal
} // namespace BamTools

#endif // BGZFSTREAM_P_H
