// ***************************************************************************
// ByteArray_p.cpp (c) 2011 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// ---------------------------------------------------------------------------
// Last modified: 10 November 2011 (DB)
// ---------------------------------------------------------------------------
// Provides a dynamic, variable-length byte buffer
// ***************************************************************************

#include "api/internal/io/ByteArray_p.h"
using namespace BamTools;
using namespace BamTools::Internal;

#include <cstddef>
#include <cstdlib>
#include <cstring>

// --------------------------
// ByteArray implementation
// --------------------------

ByteArray::ByteArray()
    : m_data()
{}

ByteArray::ByteArray(const std::string& value)
    : m_data(value.begin(), value.end())
{}

ByteArray::ByteArray(const std::vector<char>& value)
    : m_data(value)
{}

ByteArray::ByteArray(const char* value, std::size_t n)
{
    const std::string s(value, n);
    m_data.assign(s.begin(), s.end());
}

void ByteArray::Clear()
{
    m_data.clear();
}

const char* ByteArray::ConstData() const
{
    return &m_data[0];
}

char* ByteArray::Data()
{
    return &m_data[0];
}

const char& ByteArray::operator[](std::size_t i) const
{
    return m_data[i];
}

char& ByteArray::operator[](std::size_t i)
{
    return m_data[i];
}

std::size_t ByteArray::IndexOf(const char c, const std::size_t from, const std::size_t to) const
{
    const std::size_t size = ((to == 0) ? m_data.size() : to);
    for (std::size_t i = from; i < size; ++i) {
        if (m_data.at(i) == c) return i;
    }
    return m_data.size();
}

ByteArray& ByteArray::Remove(std::size_t from, std::size_t n)
{

    // if 'from' outside range, just return
    const std::size_t originalSize = m_data.size();
    if (from >= originalSize) return *this;

    // if asked to clip from 'from' to end (or beyond), simply resize
    if (from + n >= originalSize) Resize(from);

    // otherwise, shift data & resize
    else {
        memmove(&m_data[from], &m_data[from + n], (originalSize - from - n));
        Resize(originalSize - n);
    }

    // return reference to modified byte array
    return *this;
}

void ByteArray::Resize(std::size_t n)
{
    m_data.resize(n, 0);
}

std::size_t ByteArray::Size() const
{
    return m_data.size();
}

void ByteArray::Squeeze()
{
    std::vector<char> t(m_data);
    t.swap(m_data);
}
