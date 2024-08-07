// ***************************************************************************
// BamWriter_p.cpp (c) 2010 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// ---------------------------------------------------------------------------
// Last modified: 18 November 2012 (DB)
// ---------------------------------------------------------------------------
// Provides the basic functionality for producing BAM files
// ***************************************************************************

#include "api/internal/bam/BamWriter_p.h"
#include "api/BamAlignment.h"
#include "api/BamConstants.h"
#include "api/IBamIODevice.h"
#include "api/internal/utils/BamException_p.h"
using namespace BamTools;
using namespace BamTools::Internal;

#include <cstddef>
#include <cstdlib>
#include <cstring>

// ctor
BamWriterPrivate::BamWriterPrivate()
    : m_isBigEndian(BamTools::SystemIsBigEndian())
{}

// dtor
BamWriterPrivate::~BamWriterPrivate()
{
    Close();
}

// calculates minimum bin for a BAM alignment interval [begin, end)
uint32_t BamWriterPrivate::CalculateMinimumBin(const int begin, int end) const
{
    --end;
    if ((begin >> 14) == (end >> 14)) {
        return 4681 + (begin >> 14);
    }
    if ((begin >> 17) == (end >> 17)) {
        return 585 + (begin >> 17);
    }
    if ((begin >> 20) == (end >> 20)) {
        return 73 + (begin >> 20);
    }
    if ((begin >> 23) == (end >> 23)) {
        return 9 + (begin >> 23);
    }
    if ((begin >> 26) == (end >> 26)) {
        return 1 + (begin >> 26);
    }
    return 0;
}

// closes the alignment archive
void BamWriterPrivate::Close()
{

    // skip if file not open
    if (!IsOpen()) {
        return;
    }

    // close output stream
    try {
        m_stream.Close();
    } catch (const BamException& e) {
        m_errorString = e.what();
    }
}

// creates a cigar string from the supplied alignment
void BamWriterPrivate::CreatePackedCigar(const std::vector<CigarOp>& cigarOperations,
                                         std::string& packedCigar)
{

    // initialize
    const std::size_t numCigarOperations = cigarOperations.size();
    packedCigar.resize(numCigarOperations * Constants::BAM_SIZEOF_INT);

    // pack the cigar data into the string
    unsigned int* pPackedCigar = (unsigned int*)packedCigar.data();

    // iterate over cigar operations
    std::vector<CigarOp>::const_iterator coIter = cigarOperations.begin();
    std::vector<CigarOp>::const_iterator coEnd = cigarOperations.end();
    for (; coIter != coEnd; ++coIter) {

        // store op in packedCigar
        uint8_t cigarOp;
        switch (coIter->Type) {
            case (Constants::BAM_CIGAR_MATCH_CHAR):
                cigarOp = Constants::BAM_CIGAR_MATCH;
                break;
            case (Constants::BAM_CIGAR_INS_CHAR):
                cigarOp = Constants::BAM_CIGAR_INS;
                break;
            case (Constants::BAM_CIGAR_DEL_CHAR):
                cigarOp = Constants::BAM_CIGAR_DEL;
                break;
            case (Constants::BAM_CIGAR_REFSKIP_CHAR):
                cigarOp = Constants::BAM_CIGAR_REFSKIP;
                break;
            case (Constants::BAM_CIGAR_SOFTCLIP_CHAR):
                cigarOp = Constants::BAM_CIGAR_SOFTCLIP;
                break;
            case (Constants::BAM_CIGAR_HARDCLIP_CHAR):
                cigarOp = Constants::BAM_CIGAR_HARDCLIP;
                break;
            case (Constants::BAM_CIGAR_PAD_CHAR):
                cigarOp = Constants::BAM_CIGAR_PAD;
                break;
            case (Constants::BAM_CIGAR_SEQMATCH_CHAR):
                cigarOp = Constants::BAM_CIGAR_SEQMATCH;
                break;
            case (Constants::BAM_CIGAR_MISMATCH_CHAR):
                cigarOp = Constants::BAM_CIGAR_MISMATCH;
                break;
            default:
                const std::string message =
                    std::string("invalid CIGAR operation type") + coIter->Type;
                throw BamException("BamWriter::CreatePackedCigar", message);
        }

        *pPackedCigar = coIter->Length << Constants::BAM_CIGAR_SHIFT | cigarOp;
        pPackedCigar++;
    }
}

// encodes the supplied query sequence into 4-bit notation
void BamWriterPrivate::EncodeQuerySequence(const std::string& query, std::string& encodedQuery)
{

    // prepare the encoded query string
    const std::size_t queryLength = query.size();
    const std::size_t encodedQueryLength = static_cast<std::size_t>((queryLength + 1) / 2);
    encodedQuery.resize(encodedQueryLength);
    char* pEncodedQuery = (char*)encodedQuery.data();
    const char* pQuery = (const char*)query.data();

    // walk through original query sequence, encoding its bases
    unsigned char nucleotideCode;
    bool useHighWord = true;
    while (*pQuery) {
        switch (*pQuery) {
            case (Constants::BAM_DNA_EQUAL):
                nucleotideCode = Constants::BAM_BASECODE_EQUAL;
                break;
            case (Constants::BAM_DNA_A):
                nucleotideCode = Constants::BAM_BASECODE_A;
                break;
            case (Constants::BAM_DNA_C):
                nucleotideCode = Constants::BAM_BASECODE_C;
                break;
            case (Constants::BAM_DNA_M):
                nucleotideCode = Constants::BAM_BASECODE_M;
                break;
            case (Constants::BAM_DNA_G):
                nucleotideCode = Constants::BAM_BASECODE_G;
                break;
            case (Constants::BAM_DNA_R):
                nucleotideCode = Constants::BAM_BASECODE_R;
                break;
            case (Constants::BAM_DNA_S):
                nucleotideCode = Constants::BAM_BASECODE_S;
                break;
            case (Constants::BAM_DNA_V):
                nucleotideCode = Constants::BAM_BASECODE_V;
                break;
            case (Constants::BAM_DNA_T):
                nucleotideCode = Constants::BAM_BASECODE_T;
                break;
            case (Constants::BAM_DNA_W):
                nucleotideCode = Constants::BAM_BASECODE_W;
                break;
            case (Constants::BAM_DNA_Y):
                nucleotideCode = Constants::BAM_BASECODE_Y;
                break;
            case (Constants::BAM_DNA_H):
                nucleotideCode = Constants::BAM_BASECODE_H;
                break;
            case (Constants::BAM_DNA_K):
                nucleotideCode = Constants::BAM_BASECODE_K;
                break;
            case (Constants::BAM_DNA_D):
                nucleotideCode = Constants::BAM_BASECODE_D;
                break;
            case (Constants::BAM_DNA_B):
                nucleotideCode = Constants::BAM_BASECODE_B;
                break;
            case (Constants::BAM_DNA_N):
                nucleotideCode = Constants::BAM_BASECODE_N;
                break;
            default:
                const std::string message = std::string("invalid base: ") + *pQuery;
                throw BamException("BamWriter::EncodeQuerySequence", message);
        }

        // pack the nucleotide code
        if (useHighWord) {
            *pEncodedQuery = nucleotideCode << 4;
            useHighWord = false;
        } else {
            *pEncodedQuery |= nucleotideCode;
            ++pEncodedQuery;
            useHighWord = true;
        }

        // increment the query position
        ++pQuery;
    }
}

// returns a description of the last error that occurred
std::string BamWriterPrivate::GetErrorString() const
{
    return m_errorString;
}

// returns whether BAM file is open for writing or not
bool BamWriterPrivate::IsOpen() const
{
    return m_stream.IsOpen();
}

// opens the alignment archive
bool BamWriterPrivate::Open(const std::string& filename, const std::string& samHeaderText,
                            const RefVector& referenceSequences)
{
    try {

        // open the BGZF file for writing
        m_stream.Open(filename, IBamIODevice::WriteOnly);

        // write BAM file 'metadata' components
        WriteMagicNumber();
        WriteSamHeaderText(samHeaderText);
        WriteReferences(referenceSequences);

        // return success
        return true;

    } catch (const BamException& e) {
        m_errorString = e.what();
        return false;
    }
}

// saves the alignment to the alignment archive
bool BamWriterPrivate::SaveAlignment(const BamAlignment& al)
{

    try {

        // if BamAlignment contains only the core data and a raw char data buffer
        // (as a result of BamReader::GetNextAlignmentCore())
        if (al.SupportData.HasCoreOnly) {
            WriteCoreAlignment(al);

            // otherwise, BamAlignment should contain character in the standard fields: Name, QueryBases, etc
            // (resulting from BamReader::GetNextAlignment() *OR* being generated directly by client code)
        } else {
            WriteAlignment(al);
        }

        // if we get here, everything OK
        return true;

    } catch (const BamException& e) {
        m_errorString = e.what();
        return false;
    }
}

void BamWriterPrivate::SetWriteCompressed(bool ok)
{
    // modifying compression is not allowed if BAM file is open
    if (!IsOpen()) {
        m_stream.SetWriteCompressed(ok);
    }
}

void BamWriterPrivate::WriteAlignment(const BamAlignment& al)
{

    // calculate char lengths
    const unsigned int nameLength = al.Name.size() + 1;
    const unsigned int numCigarOperations = al.CigarData.size();
    const unsigned int queryLength = ((al.QueryBases == "*") ? 0 : al.QueryBases.size());
    const unsigned int tagDataLength = al.TagData.size();

    // no way to tell if alignment's bin is already defined (there is no default, invalid value)
    // so we'll go ahead calculate its bin ID before storing
    const uint32_t alignmentBin = CalculateMinimumBin(al.Position, al.GetEndPosition());

    // create our packed cigar string
    std::string packedCigar;
    CreatePackedCigar(al.CigarData, packedCigar);
    const unsigned int packedCigarLength = packedCigar.size();

    // encode the query
    unsigned int encodedQueryLength = 0;
    std::string encodedQuery;
    if (queryLength > 0) {
        EncodeQuerySequence(al.QueryBases, encodedQuery);
        encodedQueryLength = encodedQuery.size();
    }

    // write the block size
    const unsigned int dataBlockSize = nameLength + packedCigarLength + encodedQueryLength +
                                       queryLength +  // here referring to quality length
                                       tagDataLength;
    unsigned int blockSize = Constants::BAM_CORE_SIZE + dataBlockSize;
    if (numCigarOperations >= 65536) {
        blockSize += 16;
    }
    if (m_isBigEndian) {
        BamTools::SwapEndian_32(blockSize);
    }
    m_stream.Write((char*)&blockSize, Constants::BAM_SIZEOF_INT);

    // assign the BAM core data
    uint32_t buffer[Constants::BAM_CORE_BUFFER_SIZE];
    buffer[0] = al.RefID;
    buffer[1] = al.Position;
    buffer[2] = (alignmentBin << 16) | (al.MapQuality << 8) | nameLength;
    buffer[3] = (al.AlignmentFlag << 16) | (numCigarOperations < 65536 ? numCigarOperations : 2);
    buffer[4] = queryLength;
    buffer[5] = al.MateRefID;
    buffer[6] = al.MatePosition;
    buffer[7] = al.InsertSize;

    // swap BAM core endian-ness, if necessary
    if (m_isBigEndian) {
        for (int i = 0; i < 8; ++i) {
            BamTools::SwapEndian_32(buffer[i]);
        }
    }

    // write the BAM core
    m_stream.Write((char*)&buffer, Constants::BAM_CORE_SIZE);

    // write the query name
    m_stream.Write(al.Name.c_str(), nameLength);

    // write the packed cigar
    if (numCigarOperations < 65536) {
        if (m_isBigEndian) {
            char* cigarData = new char[packedCigarLength]();
            std::memcpy(cigarData, packedCigar.data(), packedCigarLength);
            if (m_isBigEndian) {
                for (size_t i = 0; i < packedCigarLength; i += sizeof(uint32_t)) {
                    BamTools::SwapEndian_32p(&cigarData[i]);
                }
            }
            m_stream.Write(cigarData, packedCigarLength);
            delete[] cigarData;  // TODO: cleanup on Write exception thrown?
        } else {
            m_stream.Write(packedCigar.data(), packedCigarLength);
        }
    } else {
        unsigned int cigar[2];
        cigar[0] = queryLength << 4 | 4;
        cigar[1] = (al.GetEndPosition() - al.Position) << 4 | 3;
        if (m_isBigEndian) {
            BamTools::SwapEndian_32(cigar[0]);
            BamTools::SwapEndian_32(cigar[1]);
        }
        m_stream.Write((char*)cigar, 8);
    }

    if (queryLength > 0) {

        // write the encoded query sequence
        m_stream.Write(encodedQuery.data(), encodedQueryLength);

        // write the base qualities
        char* pBaseQualities = new char[queryLength]();
        if (al.Qualities.empty() || (al.Qualities.size() == 1 && al.Qualities[0] == '*') ||
            al.Qualities[0] == (char)0xFF) {
            std::memset(pBaseQualities, 0xFF,
                        queryLength);  // if missing or '*', fill with invalid qual
        } else {
            for (std::size_t i = 0; i < queryLength; ++i) {
                pBaseQualities[i] =
                    al.Qualities.at(i) - 33;  // FASTQ ASCII -> phred score conversion
            }
        }
        m_stream.Write(pBaseQualities, queryLength);
        delete[] pBaseQualities;
    }

    // write the tag data
    if (m_isBigEndian) {

        char* tagData = new char[tagDataLength]();
        std::memcpy(tagData, al.TagData.data(), tagDataLength);

        std::size_t i = 0;
        while (i < tagDataLength) {

            i += Constants::BAM_TAG_TAGSIZE;  // skip tag chars (e.g. "RG", "NM", etc.)
            const char type = tagData[i];     // get tag type at position i
            ++i;

            switch (type) {

                case (Constants::BAM_TAG_TYPE_ASCII):
                case (Constants::BAM_TAG_TYPE_INT8):
                case (Constants::BAM_TAG_TYPE_UINT8):
                    ++i;
                    break;

                case (Constants::BAM_TAG_TYPE_INT16):
                case (Constants::BAM_TAG_TYPE_UINT16):
                    BamTools::SwapEndian_16p(&tagData[i]);
                    i += sizeof(uint16_t);
                    break;

                case (Constants::BAM_TAG_TYPE_FLOAT):
                case (Constants::BAM_TAG_TYPE_INT32):
                case (Constants::BAM_TAG_TYPE_UINT32):
                    BamTools::SwapEndian_32p(&tagData[i]);
                    i += sizeof(uint32_t);
                    break;

                case (Constants::BAM_TAG_TYPE_HEX):
                case (Constants::BAM_TAG_TYPE_STRING):
                    // no endian swapping necessary for hex-string/string data
                    while (tagData[i]) {
                        ++i;
                    }
                    // increment one more for null terminator
                    ++i;
                    break;

                case (Constants::BAM_TAG_TYPE_ARRAY):

                {
                    // read array type
                    const char arrayType = tagData[i];
                    ++i;

                    // swap endian-ness of number of elements in place, then retrieve for loop
                    BamTools::SwapEndian_32p(&tagData[i]);
                    int32_t numElements;
                    std::memcpy(&numElements, &tagData[i], sizeof(uint32_t));
                    i += sizeof(uint32_t);

                    // swap endian-ness of array elements
                    for (int j = 0; j < numElements; ++j) {
                        switch (arrayType) {
                            case (Constants::BAM_TAG_TYPE_INT8):
                            case (Constants::BAM_TAG_TYPE_UINT8):
                                // no endian-swapping necessary
                                ++i;
                                break;
                            case (Constants::BAM_TAG_TYPE_INT16):
                            case (Constants::BAM_TAG_TYPE_UINT16):
                                BamTools::SwapEndian_16p(&tagData[i]);
                                i += sizeof(uint16_t);
                                break;
                            case (Constants::BAM_TAG_TYPE_FLOAT):
                            case (Constants::BAM_TAG_TYPE_INT32):
                            case (Constants::BAM_TAG_TYPE_UINT32):
                                BamTools::SwapEndian_32p(&tagData[i]);
                                i += sizeof(uint32_t);
                                break;
                            default:
                                delete[] tagData;
                                const std::string message =
                                    std::string("invalid binary array type: ") + arrayType;
                                throw BamException("BamWriter::SaveAlignment", message);
                        }
                    }

                    break;
                }

                default:
                    delete[] tagData;
                    const std::string message = std::string("invalid tag type: ") + type;
                    throw BamException("BamWriter::SaveAlignment", message);
            }
        }

        m_stream.Write(tagData, tagDataLength);
        delete[] tagData;  // TODO: cleanup on Write exception thrown?
    } else {
        m_stream.Write(al.TagData.data(), tagDataLength);
    }

    if (numCigarOperations >= 65536) {
        m_stream.Write("CGBI", 4);
        if (m_isBigEndian) {
            unsigned int cigar_len_buf = numCigarOperations;
            BamTools::SwapEndian_32(cigar_len_buf);
            m_stream.Write((char*)&cigar_len_buf, 4);

            char* cigarData = new char[packedCigarLength]();
            std::memcpy(cigarData, packedCigar.data(), packedCigarLength);
            if (m_isBigEndian) {
                for (size_t i = 0; i < packedCigarLength; i += sizeof(uint32_t)) {
                    BamTools::SwapEndian_32p(&cigarData[i]);
                }
            }
            m_stream.Write(cigarData, packedCigarLength);
            delete[] cigarData;  // TODO: cleanup on Write exception thrown?
        } else {
            m_stream.Write((char*)&numCigarOperations, 4);
            m_stream.Write(packedCigar.data(), packedCigarLength);
        }
    }
}

void BamWriterPrivate::WriteCoreAlignment(const BamAlignment& al)
{

    // write the block size
    unsigned int blockSize = al.SupportData.BlockLength;
    if (al.SupportData.NumCigarOperations >= 65536) {
        blockSize += 16;
    }
    if (m_isBigEndian) {
        BamTools::SwapEndian_32(blockSize);
    }
    m_stream.Write((char*)&blockSize, Constants::BAM_SIZEOF_INT);

    // re-calculate bin (in case BamAlignment's position has been previously modified)
    const uint32_t alignmentBin = CalculateMinimumBin(al.Position, al.GetEndPosition());

    // assign the BAM core data
    uint32_t buffer[Constants::BAM_CORE_BUFFER_SIZE];
    buffer[0] = al.RefID;
    buffer[1] = al.Position;
    buffer[2] = (alignmentBin << 16) | (al.MapQuality << 8) | al.SupportData.QueryNameLength;
    buffer[3] = (al.AlignmentFlag << 16) |
                (al.SupportData.NumCigarOperations < 65536 ? al.SupportData.NumCigarOperations : 2);
    buffer[4] = al.SupportData.QuerySequenceLength;
    buffer[5] = al.MateRefID;
    buffer[6] = al.MatePosition;
    buffer[7] = al.InsertSize;

    // swap BAM core endian-ness, if necessary
    if (m_isBigEndian) {
        for (int i = 0; i < 8; ++i) {
            BamTools::SwapEndian_32(buffer[i]);
        }
    }

    // write the BAM core
    m_stream.Write((char*)&buffer, Constants::BAM_CORE_SIZE);

    // write the raw char data
    if (al.SupportData.NumCigarOperations < 65536) {
        m_stream.Write((char*)al.SupportData.AllCharData.data(),
                       al.SupportData.BlockLength - Constants::BAM_CORE_SIZE);
    } else {
        const char* data = al.SupportData.AllCharData.c_str();
        const unsigned data_len = al.SupportData.BlockLength - Constants::BAM_CORE_SIZE;
        const unsigned cigar_offset = al.SupportData.QueryNameLength;
        const unsigned seq_offset = cigar_offset + al.SupportData.NumCigarOperations * 4;
        unsigned fake_cigar[2];
        fake_cigar[0] = al.SupportData.QuerySequenceLength << 4 | 4;
        fake_cigar[1] = (al.GetEndPosition() - al.Position) << 4 | 3;
        m_stream.Write(data, al.SupportData.QueryNameLength);
        if (m_isBigEndian) {
            BamTools::SwapEndian_32(fake_cigar[0]);
            BamTools::SwapEndian_32(fake_cigar[1]);
        }
        m_stream.Write((char*)&fake_cigar, 8);
        m_stream.Write(data + seq_offset, data_len - seq_offset);
        m_stream.Write("CGBI", 4);
        if (m_isBigEndian) {
            unsigned cigar_len_buf = al.SupportData.NumCigarOperations;
            BamTools::SwapEndian_32(cigar_len_buf);
            m_stream.Write((char*)&cigar_len_buf, 4);
        } else {
            m_stream.Write((char*)&al.SupportData.NumCigarOperations, 4);
        }
        m_stream.Write(data + cigar_offset, al.SupportData.NumCigarOperations * 4);
    }
}

void BamWriterPrivate::WriteMagicNumber()
{
    // write BAM file 'magic number'
    m_stream.Write(Constants::BAM_HEADER_MAGIC, Constants::BAM_HEADER_MAGIC_LENGTH);
}

void BamWriterPrivate::WriteReferences(const BamTools::RefVector& referenceSequences)
{

    // write the number of reference sequences
    uint32_t numReferenceSequences = referenceSequences.size();
    if (m_isBigEndian) {
        BamTools::SwapEndian_32(numReferenceSequences);
    }
    m_stream.Write((char*)&numReferenceSequences, Constants::BAM_SIZEOF_INT);

    // foreach reference sequence
    RefVector::const_iterator rsIter = referenceSequences.begin();
    RefVector::const_iterator rsEnd = referenceSequences.end();
    for (; rsIter != rsEnd; ++rsIter) {

        // write the reference sequence name length (+1 for terminator)
        const uint32_t actualNameLen = rsIter->RefName.size() + 1;
        uint32_t maybeSwappedNameLen = actualNameLen;
        if (m_isBigEndian) {
            BamTools::SwapEndian_32(maybeSwappedNameLen);
        }
        m_stream.Write((char*)&maybeSwappedNameLen, Constants::BAM_SIZEOF_INT);

        // write the reference sequence name
        m_stream.Write(rsIter->RefName.c_str(), actualNameLen);

        // write the reference sequence length
        int32_t referenceLength = rsIter->RefLength;
        if (m_isBigEndian) {
            BamTools::SwapEndian_32(referenceLength);
        }
        m_stream.Write((char*)&referenceLength, Constants::BAM_SIZEOF_INT);
    }
}

void BamWriterPrivate::WriteSamHeaderText(const std::string& samHeaderText)
{

    // write the SAM header  text length
    const uint32_t actualHeaderLen = samHeaderText.size();
    uint32_t maybeSwappedHeaderLen = samHeaderText.size();
    if (m_isBigEndian) {
        BamTools::SwapEndian_32(maybeSwappedHeaderLen);
    }
    m_stream.Write((char*)&maybeSwappedHeaderLen, Constants::BAM_SIZEOF_INT);

    // write the SAM header text
    if (actualHeaderLen > 0) {
        m_stream.Write(samHeaderText.data(), actualHeaderLen);
    }
}
