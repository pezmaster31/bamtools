// ***************************************************************************
// BamPipe_p.cpp (c) 2011 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// ---------------------------------------------------------------------------
// Last modified: 18 October 2012 (DB)
// ---------------------------------------------------------------------------
// Provides BAM pipe-specific IO behavior
// ***************************************************************************

#include "api/internal/io/BamPipe_p.h"
using namespace BamTools;
using namespace BamTools::Internal;

#include <cstdio>
#include <iostream>

BamPipe::BamPipe()
    : ILocalIODevice()
{}

BamPipe::~BamPipe() {}

bool BamPipe::IsRandomAccess() const
{
    return false;
}

bool BamPipe::Open(const IBamIODevice::OpenMode mode)
{

    // make sure we're starting with a fresh pipe
    Close();

    // open stdin/stdout depending on requested openmode
#if defined(SYSTEM_NODEJS) && SYSTEM_NODEJS == 1
    if (mode == IBamIODevice::ReadOnly)
        m_stream = stdin;
    else if (mode == IBamIODevice::WriteOnly)
        m_stream = stdout;
#else
    if (mode == IBamIODevice::ReadOnly)
        m_stream = freopen(0, "rb", stdin);
    else if (mode == IBamIODevice::WriteOnly)
        m_stream = freopen(0, "wb", stdout);
#endif  // SYSTEM_NODEJS

    else {
        const std::string errorType =
            std::string((mode == IBamIODevice::ReadWrite) ? "unsupported" : "unknown");
        const std::string message = errorType + " open mode requested";
        SetErrorString("BamPipe::Open", message);
        return false;
    }

    // check that we obtained a valid FILE*
    if (m_stream == 0) {
        const std::string message_base = std::string("could not open handle on ");
        const std::string message =
            message_base + ((mode == IBamIODevice::ReadOnly) ? "stdin" : "stdout");
        SetErrorString("BamPipe::Open", message);
        return false;
    }

    // store current IO mode & return success
    m_mode = mode;
    return true;
}

bool BamPipe::Seek(const int64_t&, const int)
{
    SetErrorString("BamPipe::Seek", "random access not allowed in FIFO pipe");
    return false;
}
