// ***************************************************************************
// IBamIODevice.h (c) 2011 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// ---------------------------------------------------------------------------
// Last modified: 10 November 2011 (DB)
// ---------------------------------------------------------------------------
// Base class for all BAM I/O devices (e.g. local file, pipe, HTTP, FTP, etc.)
//
// Derived classes should provide protocol-specific implementations for
// reading/writing plain bytes, as well as other I/O-related behaviors.
//
// Since IBamIODevices may be defined in client code, the internal
// BamExceptions are NOT allowed to be thrown from devices, including the
// built-in ones. This keeps a consistent interface at the BgzfStream for
// handling any device type. Use the error string for relaying error messages.
// ***************************************************************************

#ifndef IBAMIODEVICE_H
#define IBAMIODEVICE_H

#include <cstdio>
#include <string>
#include "api/api_global.h"

namespace BamTools {

class API_EXPORT IBamIODevice
{

    // enums
public:
    enum OpenMode
    {
        NotOpen = 0x0000,
        ReadOnly = 0x0001,
        WriteOnly = 0x0002,
        ReadWrite = ReadOnly | WriteOnly
    };

    // ctor & dtor
public:
    virtual ~IBamIODevice() {}

    // IBamIODevice interface
public:
    // TODO: add seek(pos, *from*)

    // pure virtuals
    virtual void Close() = 0;
    virtual bool IsRandomAccess() const = 0;
    virtual bool Open(const OpenMode mode) = 0;
    virtual int64_t Read(char* data, const unsigned int numBytes) = 0;
    virtual bool Seek(const int64_t& position, const int origin = SEEK_SET) = 0;
    virtual int64_t Tell() const = 0;
    virtual int64_t Write(const char* data, const unsigned int numBytes) = 0;

    // default implementation provided
    virtual std::string GetErrorString();
    virtual bool IsOpen() const;
    virtual OpenMode Mode() const;

    // internal methods
protected:
    IBamIODevice();  // hidden ctor
    void SetErrorString(const std::string& where, const std::string& what);

    // data members
protected:
    OpenMode m_mode;
    std::string m_errorString;
};

inline IBamIODevice::IBamIODevice()
    : m_mode(IBamIODevice::NotOpen)
{}

inline std::string IBamIODevice::GetErrorString()
{
    return m_errorString;
}

inline bool IBamIODevice::IsOpen() const
{
    return (m_mode != IBamIODevice::NotOpen);
}

inline IBamIODevice::OpenMode IBamIODevice::Mode() const
{
    return m_mode;
}

inline void IBamIODevice::SetErrorString(const std::string& where, const std::string& what)
{
    static const std::string SEPARATOR = ": ";
    m_errorString = where + SEPARATOR + what;
}

}  // namespace BamTools

#endif  // IBAMIODEVICE_H
