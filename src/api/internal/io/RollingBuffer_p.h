#ifndef ROLLINGBUFFER_P_H
#define ROLLINGBUFFER_P_H

#include "api/api_global.h"
#include "api/internal/io/ByteArray_p.h"
#include <deque>
#include <string>

namespace BamTools {
namespace Internal {

class RollingBuffer {

    // ctors & dtor
    public:
        RollingBuffer(size_t growth); // inits buffer, new byte arrays will try to be of size @growth
        ~RollingBuffer(void);         // dtor

    // RollingBuffer interface
    public:
        size_t BlockSize(void) const;          // returns current buffer size
        bool CanReadLine(void) const;          // checks buffer for carriage return
        void Chop(size_t n);                   // frees @n bytes from end of buffer
        void Clear(void);                      // clears entire buffer structure
        void Free(size_t n);                   // frees @n bytes from front of buffer
        size_t IndexOf(char c) const;          // checks buffer for @c
        bool IsEmpty(void) const;              // returns whether buffer contains data
        size_t Read(char* dest, size_t max);   // returns up to @maxLen bytes into @dest, returns exactly how many bytes were read from buffer
        size_t ReadLine(char* dest, size_t max);
        std::string ReadLine(size_t max = 0);

        const char* ReadPointer(void) const;   // returns a C-fxn compatible char* to byte data
        char* Reserve(size_t n);               // ensures that buffer contains space for @n incoming bytes, returns write-able char*
        size_t Size(void) const;               // returns current number of bytes stored in buffer
        void Write(const char* src, size_t n); // reserves space for @n bytes, then appends contents of @src to buffer

    // data members
    private:
        size_t m_head;                // index into current data (next char)
        size_t m_tail;                // index into last data position
        size_t m_tailBufferIndex;     // m_data::size() - 1
        size_t m_totalBufferSize;     // total buffer size
        size_t m_bufferGrowth;        // new buffers are typically initialized with this size
        std::deque<ByteArray> m_data; // basic 'buffer of buffers'
};

} // namespace Internal
} // namespace BamTools

#endif // ROLLINGBUFFER_P_H
