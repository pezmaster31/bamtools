// ***************************************************************************
// BgzfStream_p.cpp (c) 2011 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// ---------------------------------------------------------------------------
// Last modified: 6 October 2011(DB)
// ---------------------------------------------------------------------------
// Based on BGZF routines developed at the Broad Institute.
// Provides the basic functionality for reading & writing BGZF files
// Replaces the old BGZF.* files to avoid clashing with other toolkits
// ***************************************************************************

#include <api/internal/BamException_p.h>
#include <api/internal/BgzfStream_p.h>
using namespace BamTools;
using namespace BamTools::Internal;

#include <cstring>
#include <algorithm>
#include <sstream>
using namespace std;

// ----------------------------
// RaiiWrapper implementation
// ----------------------------

BgzfStream::RaiiWrapper::RaiiWrapper(void)
    : Stream(0)
{
    CompressedBlock   = new char[Constants::BGZF_MAX_BLOCK_SIZE];
    UncompressedBlock = new char[Constants::BGZF_DEFAULT_BLOCK_SIZE];
}

BgzfStream::RaiiWrapper::~RaiiWrapper(void) {

    // clean up buffers
    delete[] CompressedBlock;
    delete[] UncompressedBlock;
    CompressedBlock = 0;
    UncompressedBlock = 0;

    if ( Stream ) {
        fflush(Stream);
        fclose(Stream);
        Stream = 0;
    }
}

// ---------------------------
// BgzfStream implementation
// ---------------------------

// constructor
BgzfStream::BgzfStream(void)
    : BlockLength(0)
    , BlockOffset(0)
    , BlockAddress(0)
    , IsOpen(false)
    , IsWriteOnly(false)
    , IsWriteCompressed(true)
{ }

// destructor
BgzfStream::~BgzfStream(void) {
    Close();
}

// checks BGZF block header
bool BgzfStream::CheckBlockHeader(char* header) {
    return (header[0] == Constants::GZIP_ID1 &&
            header[1] == Constants::GZIP_ID2 &&
            header[2] == Z_DEFLATED &&
            (header[3] & Constants::FLG_FEXTRA) != 0 &&
            BamTools::UnpackUnsignedShort(&header[10]) == Constants::BGZF_XLEN &&
            header[12] == Constants::BGZF_ID1 &&
            header[13] == Constants::BGZF_ID2 &&
            BamTools::UnpackUnsignedShort(&header[14]) == Constants::BGZF_LEN );
}

// closes BGZF file
void BgzfStream::Close(void) {

    // skip if file not open
    if ( !IsOpen )
        return;

    // if writing to file, flush the current BGZF block,
    // then write an empty block (as EOF marker)
    if ( IsWriteOnly ) {
        FlushBlock();
        const size_t blockLength = DeflateBlock();
        fwrite(Resources.CompressedBlock, 1, blockLength, Resources.Stream);
    }

    // flush and close stream
    fflush(Resources.Stream);
    fclose(Resources.Stream);
    Resources.Stream = 0;

    // reset initial state
    BlockLength = 0;
    BlockOffset = 0;
    BlockAddress = 0;
    IsOpen = false;
    IsWriteOnly = false;
    IsWriteCompressed = true;
}

// compresses the current block
size_t BgzfStream::DeflateBlock(void) {

    // initialize the gzip header
    char* buffer = Resources.CompressedBlock;
    memset(buffer, 0, 18);
    buffer[0]  = Constants::GZIP_ID1;
    buffer[1]  = Constants::GZIP_ID2;
    buffer[2]  = Constants::CM_DEFLATE;
    buffer[3]  = Constants::FLG_FEXTRA;
    buffer[9]  = Constants::OS_UNKNOWN;
    buffer[10] = Constants::BGZF_XLEN;
    buffer[12] = Constants::BGZF_ID1;
    buffer[13] = Constants::BGZF_ID2;
    buffer[14] = Constants::BGZF_LEN;

    // set compression level
    const int compressionLevel = ( IsWriteCompressed ? Z_DEFAULT_COMPRESSION : 0 );

    // loop to retry for blocks that do not compress enough
    int inputLength = BlockOffset;
    size_t compressedLength = 0;
    const unsigned int bufferSize = Constants::BGZF_MAX_BLOCK_SIZE;

    while ( true ) {

        // initialize zstream values
        z_stream zs;
        zs.zalloc    = NULL;
        zs.zfree     = NULL;
        zs.next_in   = (Bytef*)Resources.UncompressedBlock;
        zs.avail_in  = inputLength;
        zs.next_out  = (Bytef*)&buffer[Constants::BGZF_BLOCK_HEADER_LENGTH];
        zs.avail_out = bufferSize -
                       Constants::BGZF_BLOCK_HEADER_LENGTH -
                       Constants::BGZF_BLOCK_FOOTER_LENGTH;

        // initialize the zlib compression algorithm
        int status = deflateInit2(&zs,
                                  compressionLevel,
                                  Z_DEFLATED,
                                  Constants::GZIP_WINDOW_BITS,
                                  Constants::Z_DEFAULT_MEM_LEVEL,
                                  Z_DEFAULT_STRATEGY);
        if ( status != Z_OK )
            throw BamException("BgzfStream::DeflateBlock", "zlib deflateInit2 failed");

        // compress the data
        status = deflate(&zs, Z_FINISH);

        // if not at stream end
        if ( status != Z_STREAM_END ) {

            deflateEnd(&zs);

            // if error status
            if ( status != Z_OK )
                throw BamException("BgzfStream::DeflateBlock", "zlib deflate failed");

            // not enough space available in buffer
            // try to reduce the input length & re-start loop
            inputLength -= 1024;
            if ( inputLength <= 0 )
                throw BamException("BgzfStream::DeflateBlock", "input reduction failed");
            continue;
        }

        // finalize the compression routine
        status = deflateEnd(&zs);
        if ( status != Z_OK )
            throw BamException("BgzfStream::DeflateBlock", "zlib deflateEnd failed");

        // update compressedLength
        compressedLength = zs.total_out +
                           Constants::BGZF_BLOCK_HEADER_LENGTH +
                           Constants::BGZF_BLOCK_FOOTER_LENGTH;
        if ( compressedLength > Constants::BGZF_MAX_BLOCK_SIZE )
            throw BamException("BgzfStream::DeflateBlock", "deflate overflow");

        // quit while loop
        break;
    }

    // store the compressed length
    BamTools::PackUnsignedShort(&buffer[16], static_cast<uint16_t>(compressedLength - 1));

    // store the CRC32 checksum
    uint32_t crc = crc32(0, NULL, 0);
    crc = crc32(crc, (Bytef*)Resources.UncompressedBlock, inputLength);
    BamTools::PackUnsignedInt(&buffer[compressedLength - 8], crc);
    BamTools::PackUnsignedInt(&buffer[compressedLength - 4], inputLength);

    // ensure that we have less than a block of data left
    int remaining = BlockOffset - inputLength;
    if ( remaining > 0 ) {
        if ( remaining > inputLength )
            throw BamException("BgzfStream::DeflateBlock", "after deflate, remainder too large");
        memcpy(Resources.UncompressedBlock, Resources.UncompressedBlock + inputLength, remaining);
    }

    // update block data & return compressedlength
    BlockOffset = remaining;
    return compressedLength;
}

// flushes the data in the BGZF block
void BgzfStream::FlushBlock(void) {

    // flush all of the remaining blocks
    while ( BlockOffset > 0 ) {

        // compress the data block
        const size_t blockLength = DeflateBlock();

        // flush the data to our output stream
        const size_t numBytesWritten = fwrite(Resources.CompressedBlock, 1, blockLength, Resources.Stream);
        if ( numBytesWritten != blockLength ) {
            stringstream s("");
            s << "expected to write " << blockLength
              << " bytes during flushing, but wrote " << numBytesWritten;
            throw BamException("BgzfStream::FlushBlock", s.str());
        }

        // update block data
        BlockAddress += blockLength;
    }
}

// decompresses the current block
size_t BgzfStream::InflateBlock(const size_t& blockLength) {

    // setup zlib stream object
    z_stream zs;
    zs.zalloc    = NULL;
    zs.zfree     = NULL;
    zs.next_in   = (Bytef*)Resources.CompressedBlock + 18;
    zs.avail_in  = blockLength - 16;
    zs.next_out  = (Bytef*)Resources.UncompressedBlock;
    zs.avail_out = Constants::BGZF_DEFAULT_BLOCK_SIZE;

    // initialize
    int status = inflateInit2(&zs, Constants::GZIP_WINDOW_BITS);
    if ( status != Z_OK )
        throw BamException("BgzfStream::InflateBlock", "zlib inflateInit failed");

    // decompress
    status = inflate(&zs, Z_FINISH);
    if ( status != Z_STREAM_END ) {
        inflateEnd(&zs);
        throw BamException("BgzfStream::InflateBlock", "zlib inflate failed");
    }

    // finalize
    status = inflateEnd(&zs);
    if ( status != Z_OK ) {
        inflateEnd(&zs);
        throw BamException("BgzfStream::InflateBlock", "zlib inflateEnd failed");
    }

    // return result
    return zs.total_out;
}

// opens the BGZF file for reading (mode is either "rb" for reading, or "wb" for writing)
void BgzfStream::Open(const string& filename, const char* mode) {

    // make sure we're starting with fresh state
    if ( IsOpen )
        Close();

    // determine open mode
    if ( strcmp(mode, "rb") == 0 )
        IsWriteOnly = false;
    else if ( strcmp(mode, "wb") == 0)
        IsWriteOnly = true;
    else {
        const string message = string("unknown file mode: ") + mode;
        throw BamException("BgzfStream::Open", message);
    }

    // open BGZF stream on a file
    if ( (filename != "stdin") && (filename != "stdout") && (filename != "-"))
        Resources.Stream = fopen(filename.c_str(), mode);

    // open BGZF stream on stdin
    else if ( (filename == "stdin" || filename == "-") && (strcmp(mode, "rb") == 0 ) )
        Resources.Stream = freopen(NULL, mode, stdin);

    // open BGZF stream on stdout
    else if ( (filename == "stdout" || filename == "-") && (strcmp(mode, "wb") == 0) )
        Resources.Stream = freopen(NULL, mode, stdout);

    // ensure valid Stream
    if ( !Resources.Stream ) {
        const string message = string("unable to open file: ") + filename;
        throw BamException("BgzfStream::Open", message);
    }

    // set flag
    IsOpen = true;
}

// reads BGZF data into a byte buffer
size_t BgzfStream::Read(char* data, const size_t dataLength) {

    // if stream not open for reading (or empty request)
    if ( !IsOpen || IsWriteOnly || dataLength == 0 )
        return 0;

    // read blocks as needed until desired data length is retrieved
    size_t numBytesRead = 0;
    while ( numBytesRead < dataLength ) {

        // determine bytes available in current block
        int bytesAvailable = BlockLength - BlockOffset;

        // read (and decompress) next block if needed
        if ( bytesAvailable <= 0 ) {
            ReadBlock();
            bytesAvailable = BlockLength - BlockOffset;
            if ( bytesAvailable <= 0 )
                break;
        }

        // copy data from uncompressed source buffer into data destination buffer
        const size_t copyLength = min( (dataLength-numBytesRead), (size_t)bytesAvailable );
        memcpy(data, Resources.UncompressedBlock + BlockOffset, copyLength);

        // update counters
        BlockOffset  += copyLength;
        data         += copyLength;
        numBytesRead += copyLength;
    }

    // update block data
    if ( BlockOffset == BlockLength ) {
        BlockAddress = ftell64(Resources.Stream);
        BlockOffset  = 0;
        BlockLength  = 0;
    }

    // return actual number of bytes read
    return numBytesRead;
}

// reads a BGZF block
void BgzfStream::ReadBlock(void) {

    // store block start
    int64_t blockAddress = ftell64(Resources.Stream);

    // read block header from file
    char header[Constants::BGZF_BLOCK_HEADER_LENGTH];
    size_t count = fread(header, 1, Constants::BGZF_BLOCK_HEADER_LENGTH, Resources.Stream);

    // if block header empty, set marker & skip rest of method
    if ( count == 0 ) {
        BlockLength = 0;
        return;
    }

    // if block header invalid size
    if ( count != sizeof(header) )
        throw BamException("BgzfStream::ReadBlock", "invalid block header size");

    // validate block header contents
    if ( !BgzfStream::CheckBlockHeader(header) )
        throw BamException("BgzfStream::ReadBlock", "invalid block header contents");

    // copy header contents to compressed buffer
    const size_t blockLength = BamTools::UnpackUnsignedShort(&header[16]) + 1;
    memcpy(Resources.CompressedBlock, header, Constants::BGZF_BLOCK_HEADER_LENGTH);

    // read remainder of block
    const size_t remaining = blockLength - Constants::BGZF_BLOCK_HEADER_LENGTH;
    count = fread(&Resources.CompressedBlock[Constants::BGZF_BLOCK_HEADER_LENGTH], 1, remaining, Resources.Stream);
    if ( count != remaining )
        throw BamException("BgzfStream::ReadBlock", "could not read data from block");

    // decompress block data
    count = InflateBlock(blockLength);

    // update block metadata
    if ( BlockLength != 0 )
        BlockOffset = 0;
    BlockAddress = blockAddress;
    BlockLength  = count;
}

// seek to position in BGZF file
void BgzfStream::Seek(const int64_t& position) {

    // determine adjusted offset & address
    int     blockOffset  = (position & 0xFFFF);
    int64_t blockAddress = (position >> 16) & 0xFFFFFFFFFFFFLL;

    // attempt seek in file
    if ( fseek64(Resources.Stream, blockAddress, SEEK_SET) != 0 ) {
        stringstream s("");
        s << "unable to seek to position: " << position;
        throw BamException("BgzfStream::Seek", s.str());
    }

    // if successful, update block metadata
    BlockLength  = 0;
    BlockAddress = blockAddress;
    BlockOffset  = blockOffset;
}

void BgzfStream::SetWriteCompressed(bool ok) {
    IsWriteCompressed = ok;
}

// get file position in BGZF file
int64_t BgzfStream::Tell(void) const {
    if ( !IsOpen )
        return 0;
    return ( (BlockAddress << 16) | (BlockOffset & 0xFFFF) );
}

// writes the supplied data into the BGZF buffer
size_t BgzfStream::Write(const char* data, const size_t dataLength) {

    // skip if file not open for writing
    if ( !IsOpen || !IsWriteOnly )
        return false;

    // write blocks as needed til all data is written
    size_t numBytesWritten = 0;
    const char* input = data;
    const size_t blockLength = Constants::BGZF_DEFAULT_BLOCK_SIZE;
    while ( numBytesWritten < dataLength ) {

        // copy data contents to uncompressed output buffer
        const size_t copyLength = min(blockLength - BlockOffset, dataLength - numBytesWritten);
        char* buffer = Resources.UncompressedBlock;
        memcpy(buffer + BlockOffset, input, copyLength);

        // update counters
        BlockOffset     += copyLength;
        input           += copyLength;
        numBytesWritten += copyLength;

        // flush (& compress) output buffer when full
        if ( BlockOffset == blockLength )
            FlushBlock();
    }

    // return actual number of bytes written
    return numBytesWritten;
}
