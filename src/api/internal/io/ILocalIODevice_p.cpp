// ***************************************************************************
// ILocalIODevice_p.cpp (c) 2011 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// ---------------------------------------------------------------------------
// Last modified: 27 July 2012 (DB)
// ---------------------------------------------------------------------------
// Provides shared behavior for files & pipes
// ***************************************************************************

#include "api/internal/io/ILocalIODevice_p.h"
using namespace BamTools;
using namespace BamTools::Internal;

#include <cstdio>

ILocalIODevice::ILocalIODevice()
    : IBamIODevice()
    , m_stream(0)
{}

ILocalIODevice::~ILocalIODevice()
{
    Close();
}

void ILocalIODevice::Close()
{

    // skip if not open
    if (!IsOpen()) return;

    // flush & close FILE*
    fflush(m_stream);
    fclose(m_stream);
    m_stream = 0;

    // reset other device state
    m_mode = IBamIODevice::NotOpen;
}

int64_t ILocalIODevice::Read(char* data, const unsigned int numBytes)
{
    BT_ASSERT_X(m_stream, "ILocalIODevice::Read: trying to read from null stream");
    BT_ASSERT_X((m_mode & IBamIODevice::ReadOnly),
                "ILocalIODevice::Read: device not in read-able mode");
    return static_cast<int64_t>(fread(data, sizeof(char), numBytes, m_stream));
}

int64_t ILocalIODevice::Tell() const
{
    BT_ASSERT_X(m_stream, "ILocalIODevice::Tell: trying to get file position fromnull stream");
    return ftell64(m_stream);
}

int64_t ILocalIODevice::Write(const char* data, const unsigned int numBytes)
{
    BT_ASSERT_X(m_stream, "ILocalIODevice::Write: tryint to write to null stream");
    BT_ASSERT_X((m_mode & IBamIODevice::WriteOnly),
                "ILocalIODevice::Write: device not in write-able mode");
    return static_cast<int64_t>(fwrite(data, sizeof(char), numBytes, m_stream));
}
