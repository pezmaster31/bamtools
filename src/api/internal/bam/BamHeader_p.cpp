// ***************************************************************************
// BamHeader_p.cpp (c) 2010 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// ---------------------------------------------------------------------------
// Last modified: 18 November 2012 (DB)
// ---------------------------------------------------------------------------
// Provides the basic functionality for handling BAM headers.
// ***************************************************************************

#include "api/internal/bam/BamHeader_p.h"
#include "api/BamAux.h"
#include "api/BamConstants.h"
#include "api/internal/io/BgzfStream_p.h"
#include "api/internal/utils/BamException_p.h"
using namespace BamTools;
using namespace BamTools::Internal;

#include <cstddef>
#include <cstdlib>
#include <cstring>

// ------------------------
// static utility methods
// ------------------------

static inline bool isValidMagicNumber(const char* buffer)
{
    return (strncmp(buffer, Constants::BAM_HEADER_MAGIC, Constants::BAM_HEADER_MAGIC_LENGTH) == 0);
}

// --------------------------
// BamHeader implementation
// --------------------------

// ctor
BamHeader::BamHeader() {}

// dtor
BamHeader::~BamHeader() {}

// reads magic number from BGZF stream, returns true if valid
void BamHeader::CheckMagicNumber(BgzfStream* stream)
{

    // try to read magic number
    char buffer[Constants::BAM_HEADER_MAGIC_LENGTH];
    const std::size_t numBytesRead = stream->Read(buffer, Constants::BAM_HEADER_MAGIC_LENGTH);
    if (numBytesRead != (int)Constants::BAM_HEADER_MAGIC_LENGTH)
        throw BamException("BamHeader::CheckMagicNumber", "could not read magic number");

    // validate magic number
    if (!isValidMagicNumber(buffer))
        throw BamException("BamHeader::CheckMagicNumber", "invalid magic number");
}

// clear SamHeader data
void BamHeader::Clear()
{
    m_header.Clear();
}

// return true if SamHeader data is valid
bool BamHeader::IsValid() const
{
    return m_header.IsValid();
}

// load BAM header ('magic number' and SAM header text) from BGZF stream
void BamHeader::Load(BgzfStream* stream)
{

    // read & check magic number
    CheckMagicNumber(stream);

    // read header (length, then actual text)
    uint32_t length(0);
    ReadHeaderLength(stream, length);
    ReadHeaderText(stream, length);
}

// reads SAM header text length from BGZF stream, stores it in @length
void BamHeader::ReadHeaderLength(BgzfStream* stream, uint32_t& length)
{

    // read BAM header text length
    char buffer[sizeof(uint32_t)];
    const std::size_t numBytesRead = stream->Read(buffer, sizeof(uint32_t));
    if (numBytesRead != sizeof(uint32_t))
        throw BamException("BamHeader::ReadHeaderLength", "could not read header length");

    // convert char buffer to length
    length = BamTools::UnpackUnsignedInt(buffer);
    if (BamTools::SystemIsBigEndian()) BamTools::SwapEndian_32(length);
}

// reads SAM header text from BGZF stream, stores in SamHeader object
void BamHeader::ReadHeaderText(BgzfStream* stream, const uint32_t& length)
{

    // read header text
    char* headerText = (char*)calloc(length + 1, 1);
    const std::size_t bytesRead = stream->Read(headerText, length);

    // if error reading, clean up buffer & throw
    if (bytesRead != length) {
        free(headerText);
        throw BamException("BamHeader::ReadHeaderText", "could not read header text");
    }

    // otherwise, text was read OK
    // store & cleanup
    m_header.SetHeaderText(static_cast<std::string>((const char*)headerText));
    free(headerText);
}

// returns const-reference to SamHeader data object
const SamHeader& BamHeader::ToConstSamHeader() const
{
    return m_header;
}

// returns *copy* of SamHeader data object
SamHeader BamHeader::ToSamHeader() const
{
    return m_header;
}

// returns SAM-formatted string of header data
std::string BamHeader::ToString() const
{
    return m_header.ToString();
}
