#ifndef BYTEARRAY_P_H
#define BYTEARRAY_P_H

#include "api/api_global.h"
#include <string>
#include <vector>

namespace BamTools {
namespace Internal {

// provides a wrapper around a byte vector
class ByteArray {

    // ctors & dtor
    public:
        ByteArray(void);
        ByteArray(const std::string& value);
        ByteArray(const std::vector<char>& value);
        ByteArray(const char* value, size_t n);
        ByteArray(const ByteArray& other);
        ~ByteArray(void);

        ByteArray& operator=(const ByteArray& other);

    // ByteArray interface
    public:

        // data access
        const char* ConstData(void) const;
        char* Data(void);
        const char& operator[](size_t i) const;
        char& operator[](size_t i);

        // byte array manipulation
        void Clear(void);
        size_t IndexOf(const char c, const size_t from = 0, const size_t to = 0) const;
        ByteArray& Remove(size_t from, size_t n);
        void Resize(size_t n);
        size_t Size(void) const;
        void Squeeze(void);

    // data members
    private:
        std::vector<char> m_data;
};

} // namespace Internal
} // namespace BamTools

#endif // BYTEARRAY_P_H
