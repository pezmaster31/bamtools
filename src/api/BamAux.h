// ***************************************************************************
// BamAux.h (c) 2009 Derek Barnett, Michael Str�mberg
// Marth Lab, Department of Biology, Boston College
// ---------------------------------------------------------------------------
// Last modified: 25 October 2011 (DB)
// ---------------------------------------------------------------------------
// Provides data structures & utility methods that are used throughout the API.
// ***************************************************************************

#ifndef BAMAUX_H
#define BAMAUX_H

#include <cstddef>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "api/api_global.h"

/*! \file BamAux.h

    Provides data structures & utility methods that are used throughout the API.
*/

/*! \namespace BamTools
    \brief Contains all BamTools classes & methods.

    The BamTools API contained in this namespace contains classes and methods
    for reading, writing, and manipulating BAM alignment files.
*/
namespace BamTools {

// ----------------------------------------------------------------
// CigarOp

/*! \struct BamTools::CigarOp
    \brief Represents a CIGAR alignment operation.

    \sa \samSpecURL for more details on using CIGAR operations.
*/
struct API_EXPORT CigarOp
{

    char Type;        //!< CIGAR operation type (MIDNSHPX=)
    uint32_t Length;  //!< CIGAR operation length (number of bases)

    //! constructor
    CigarOp(const char type = '\0', const uint32_t& length = 0)
        : Type(type)
        , Length(length)
    {}
};

// ----------------------------------------------------------------
// RefData

/*! \struct BamTools::RefData
    \brief Represents a reference sequence entry
*/
struct API_EXPORT RefData
{

    std::string RefName;  //!< name of reference sequence
    int32_t RefLength;    //!< length of reference sequence

    //! constructor
    RefData(const std::string& name = std::string(), const int32_t& length = 0)
        : RefName(name)
        , RefLength(length)
    {}
};

//! convenience typedef for vector of RefData entries
typedef std::vector<RefData> RefVector;

// ----------------------------------------------------------------
// BamRegion

/*! \struct BamTools::BamRegion
    \brief Represents a sequential genomic region

    Allowed to span multiple (sequential) references.

    \warning BamRegion now represents a zero-based, HALF-OPEN interval.
    In previous versions of BamTools (0.x & 1.x) all intervals were treated
    as zero-based, CLOSED.
*/
struct API_EXPORT BamRegion
{

    int LeftRefID;      //!< reference ID for region's left boundary
    int LeftPosition;   //!< position for region's left boundary
    int RightRefID;     //!< reference ID for region's right boundary
    int RightPosition;  //!< position for region's right boundary

    //! constructor
    BamRegion(const int& leftID = -1, const int& leftPos = -1, const int& rightID = -1,
              const int& rightPos = -1)
        : LeftRefID(leftID)
        , LeftPosition(leftPos)
        , RightRefID(rightID)
        , RightPosition(rightPos)
    {}

    //! Clears region boundaries
    void clear()
    {
        LeftRefID = -1;
        LeftPosition = -1;
        RightRefID = -1;
        RightPosition = -1;
    }

    //! Returns true if region has a left boundary
    bool isLeftBoundSpecified() const
    {
        return (LeftRefID >= 0 && LeftPosition >= 0);
    }

    //! Returns true if region boundaries are not defined
    bool isNull() const
    {
        return (!isLeftBoundSpecified() && !isRightBoundSpecified());
    }

    //! Returns true if region has a right boundary
    bool isRightBoundSpecified() const
    {
        return (RightRefID >= 0 && RightPosition >= 1);
    }
};

struct API_EXPORT CustomHeaderTag
{
    std::string TagName;
    std::string TagValue;
};

// ----------------------------------------------------------------
// General utility methods

/*! \fn bool FileExists(const std::string& filename)
    \brief returns true if the file exists
*/
inline bool FileExists(const std::string& filename)
{
    std::ifstream f(filename.c_str(), std::ifstream::in);
    return !f.fail();
}

/*! \fn void SwapEndian_16(uint16_t& x)
    \brief swaps endianness of unsigned 16-bit integer, in place
*/
inline void SwapEndian_16(uint16_t& x)
{
    x = ((x & 0xFF00) >> 8) | ((x & 0x00FF) << 8);
}

/*! \fn void SwapEndian_16(int16_t& x)
    \brief swaps endianness of signed 16-bit integer, in place
*/
inline void SwapEndian_16(int16_t& x)
{
    uint16_t val = x;
    SwapEndian_16(val);
    x = val;
}

/*! \fn void SwapEndian_32(uint32_t& x)
    \brief swaps endianness of unsigned 32-bit integer, in place
*/
inline void SwapEndian_32(uint32_t& x)
{
    x = ((x & 0xFF000000) >> 24) | ((x & 0x00FF0000) >> 8) | ((x & 0x0000FF00) << 8) |
        ((x & 0x000000FF) << 24);
}

/*! \fn void SwapEndian_32(int32_t& x)
    \brief swaps endianness of signed 32-bit integer, in place
*/
inline void SwapEndian_32(int32_t& x)
{
    uint32_t val = x;
    SwapEndian_32(val);
    x = val;
}

/*! \fn void SwapEndian_64(uint64_t& x)
    \brief swaps endianness of unsigned 64-bit integer, in place
*/
inline void SwapEndian_64(uint64_t& x)
{
    x = ((x & 0xFF00000000000000ull) >> 56) | ((x & 0x00FF000000000000ull) >> 40) |
        ((x & 0x0000FF0000000000ull) >> 24) | ((x & 0x000000FF00000000ull) >> 8) |
        ((x & 0x00000000FF000000ull) << 8) | ((x & 0x0000000000FF0000ull) << 24) |
        ((x & 0x000000000000FF00ull) << 40) | ((x & 0x00000000000000FFull) << 56);
}

/*! \fn void SwapEndian_64(int64_t& x)
    \brief swaps endianness of signed 64-bit integer, in place
*/
inline void SwapEndian_64(int64_t& x)
{
    uint64_t val = x;
    SwapEndian_64(val);
    x = val;
}

/*! \fn void SwapEndian_16p(char* data)
    \brief swaps endianness of the next 2 bytes in a buffer, in place
*/
inline void SwapEndian_16p(char* data)
{
    uint16_t value;
    std::memcpy(&value, data, sizeof(uint16_t));
    SwapEndian_16(value);
    std::memcpy(data, &value, sizeof(uint16_t));
}

/*! \fn void SwapEndian_32p(char* data)
    \brief swaps endianness of the next 4 bytes in a buffer, in place
*/
inline void SwapEndian_32p(char* data)
{
    uint32_t value;
    std::memcpy(&value, data, sizeof(uint32_t));
    SwapEndian_32(value);
    std::memcpy(data, &value, sizeof(uint32_t));
}

/*! \fn void SwapEndian_64p(char* data)
    \brief swaps endianness of the next 8 bytes in a buffer, in place
*/
inline void SwapEndian_64p(char* data)
{
    uint64_t value;
    std::memcpy(&value, data, sizeof(uint64_t));
    SwapEndian_64(value);
    std::memcpy(data, &value, sizeof(uint64_t));
}

/*! \fn bool SystemIsBigEndian()
    \brief checks host architecture's byte order
    \return \c true if system uses big-endian ordering
*/
inline bool SystemIsBigEndian()
{
    const uint16_t one = 0x0001;
    return ((*(char*)&one) == 0);
}

/*! \fn void PackUnsignedInt(char* buffer, unsigned int value)
    \brief stores unsigned integer value in a byte buffer

    \param[out] buffer destination buffer
    \param[in]  value  value to 'pack' in buffer
*/
inline void PackUnsignedInt(char* buffer, unsigned int value)
{
    buffer[0] = (char)value;
    buffer[1] = (char)(value >> 8);
    buffer[2] = (char)(value >> 16);
    buffer[3] = (char)(value >> 24);
}

/*! \fn void PackUnsignedShort(char* buffer, unsigned short value)
    \brief stores unsigned short integer value in a byte buffer

    \param[out] buffer destination buffer
    \param[in]  value  value to 'pack' in buffer
*/
inline void PackUnsignedShort(char* buffer, unsigned short value)
{
    buffer[0] = (char)value;
    buffer[1] = (char)(value >> 8);
}

/*! \fn double UnpackDouble(const char* buffer)
    \brief reads a double value from byte buffer

    \param[in] buffer source byte buffer
    \return the (double) value read from the buffer
*/
inline double UnpackDouble(const char* buffer)
{
    double result;
    std::memcpy(&result, buffer, sizeof(double));
    return result;
}

/*! \fn double UnpackFloat(const char* buffer)
    \brief reads a float value from byte buffer

    \param[in] buffer source byte buffer
    \return the (float) value read from the buffer
*/
inline float UnpackFloat(const char* buffer)
{
    float result;
    std::memcpy(&result, buffer, sizeof(float));
    return result;
}

/*! \fn signed int UnpackSignedInt(const char* buffer)
    \brief reads a signed integer value from byte buffer

    \param[in] buffer source byte buffer
    \return the (signed int) value read from the buffer
*/
inline signed int UnpackSignedInt(const char* buffer)
{
    signed int result;
    std::memcpy(&result, buffer, sizeof(signed int));
    return result;
}

/*! \fn signed short UnpackSignedShort(const char* buffer)
    \brief reads a signed short integer value from byte buffer

    \param[in] buffer source byte buffer
    \return the (signed short) value read from the buffer
*/
inline signed short UnpackSignedShort(const char* buffer)
{
    signed short result;
    std::memcpy(&result, buffer, sizeof(signed short));
    return result;
}

/*! \fn unsigned int UnpackUnsignedInt(const char* buffer)
    \brief reads an unsigned integer value from byte buffer

    \param[in] buffer source byte buffer
    \return the (unsigned int) value read from the buffer
*/
inline unsigned int UnpackUnsignedInt(const char* buffer)
{
    unsigned int result;
    std::memcpy(&result, buffer, sizeof(unsigned int));
    return result;
}

/*! \fn unsigned short UnpackUnsignedShort(const char* buffer)
    \brief reads an unsigned short integer value from byte buffer

    \param[in] buffer source byte buffer
    \return the (unsigned short) value read from the buffer
*/
inline unsigned short UnpackUnsignedShort(const char* buffer)
{
    unsigned short result;
    std::memcpy(&result, buffer, sizeof(unsigned short));

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    // no further operations required
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    SwapEndian_16(result);
#else
#error "Unsupported hardware"
#endif

    return result;
}

// ----------------------------------------------------------------
// 'internal' helper structs

/*! \struct RaiiBuffer
    \internal
*/
struct API_NO_EXPORT RaiiBuffer
{

    // data members
    char* Buffer;
    const std::size_t NumBytes;

    // ctor & dtor
    RaiiBuffer(const std::size_t n)
        : Buffer(new char[n]())
        , NumBytes(n)
    {}

    ~RaiiBuffer()
    {
        delete[] Buffer;
    }

    // add'l methods
    void Clear()
    {
        std::memset(Buffer, 0, NumBytes);
    }
};

}  // namespace BamTools

#endif  // BAMAUX_H
