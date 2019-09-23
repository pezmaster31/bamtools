// ***************************************************************************
// TcpSocketEngine_p.h (c) 2011 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// ---------------------------------------------------------------------------
// Last modified: 8 December 2011 (DB)
// ---------------------------------------------------------------------------
// Provides low-level implementation of TCP I/O
// ***************************************************************************

#ifndef TCPSOCKETENGINE_P_H
#define TCPSOCKETENGINE_P_H

//  -------------
//  W A R N I N G
//  -------------
//
// This file is not part of the BamTools API.  It exists purely as an
// implementation detail. This header file may change from version to version
// without notice, or even be removed.
//
// We mean it.

#include "api/internal/io/HostAddress_p.h"
#include "api/internal/io/TcpSocket_p.h"

#ifdef _WIN32
#include "api/internal/io/NetWin_p.h"
#endif

#include <cstddef>

namespace BamTools {
namespace Internal {

class TcpSocketEngine
{

    // ctors & dtor
public:
    TcpSocketEngine();
    ~TcpSocketEngine();

    // TcpSocketEngine interface
public:
    // connection-related methods
    void Close();
    bool Connect(const HostAddress& address, const uint16_t port);
    bool Initialize(HostAddress::NetworkProtocol protocol);
    bool IsValid() const;

    // IO-related methods
    int64_t NumBytesAvailable() const;
    int64_t Read(char* dest, std::size_t max);
    int64_t Write(const char* data, std::size_t length);

    bool WaitForRead(int msec, bool* timedOut);
    bool WaitForWrite(int msec, bool* timedOut);

    // query connection state
    //        HostAddress GetLocalAddress() const;
    //        uint16_t GetLocalPort() const;
    HostAddress GetRemoteAddress() const;
    uint16_t GetRemotePort() const;

    int GetSocketDescriptor() const;
    TcpSocket::SocketError GetSocketError();
    TcpSocket::SocketState GetSocketState();

    std::string GetErrorString() const;

    // platform-dependent internal methods
    // provided in the corresponding TcpSocketEngine_<OS>_p.cpp
private:
    void nativeClose();
    bool nativeConnect(const HostAddress& address, const uint16_t port);
    bool nativeCreateSocket(HostAddress::NetworkProtocol protocol);
    void nativeDisconnect();
    int64_t nativeNumBytesAvailable() const;
    int64_t nativeRead(char* dest, std::size_t max);
    int nativeSelect(int msecs, bool isRead) const;
    int64_t nativeWrite(const char* data, std::size_t length);

    // data members
private:
    int m_socketDescriptor;

    //        HostAddress m_localAddress;
    HostAddress m_remoteAddress;
    //        uint16_t m_localPort;
    uint16_t m_remotePort;

    TcpSocket::SocketError m_socketError;
    TcpSocket::SocketState m_socketState;
    std::string m_errorString;

#ifdef _WIN32
    WindowsSockInit m_win;
#endif
};

}  // namespace Internal
}  // namespace BamTools

#endif  // TCPSOCKETENGINE_P_H
