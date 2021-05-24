// ***************************************************************************
// ByteArray_p.h (c) 2011 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// ---------------------------------------------------------------------------
// Last modified: 10 November 2011 (DB)
// ---------------------------------------------------------------------------
// Provides a dynamic, variable-length byte buffer
// ***************************************************************************

#ifndef BYTEARRAY_P_H
#define BYTEARRAY_P_H

#include "api/api_global.h"

//  -------------
//  W A R N I N G
//  -------------
//
// This file is not part of the BamTools API.  It exists purely as an
// implementation detail. This header file may change from version to version
// without notice, or even be removed.
//
// We mean it.

#include <cstddef>
#include <string>
#include <vector>

namespace BamTools {
namespace Internal {

// provides a wrapper around a byte vector
class API_NO_EXPORT ByteArray
{

    // ctors & dtor
public:
    ByteArray();
    ByteArray(const std::string& value);
    ByteArray(const std::vector<char>& value);
    ByteArray(const char* value, std::size_t n);

    // ByteArray interface
public:
    // data access
    const char* ConstData() const;
    char* Data();
    const char& operator[](std::size_t i) const;
    char& operator[](std::size_t i);

    // byte array manipulation
    void Clear();
    std::size_t IndexOf(const char c, const std::size_t from = 0, const std::size_t to = 0) const;
    ByteArray& Remove(std::size_t from, std::size_t n);
    void Resize(std::size_t n);
    std::size_t Size() const;
    void Squeeze();

    // data members
private:
    std::vector<char> m_data;
};

}  // namespace Internal
}  // namespace BamTools

#endif  // BYTEARRAY_P_H
