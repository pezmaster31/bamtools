// ***************************************************************************
// TcpSocket_p.h (c) 2011 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// ---------------------------------------------------------------------------
// Last modified: 7 December 2011 (DB)
// ---------------------------------------------------------------------------
// Provides basic TCP I/O interface
// ***************************************************************************

#ifndef TCPSOCKET_P_H
#define TCPSOCKET_P_H

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
#include "api/IBamIODevice.h"
#include "api/internal/io/HostInfo_p.h"
#include "api/internal/io/RollingBuffer_p.h"

namespace BamTools {
namespace Internal {

class BamHttp;
class TcpSocketEngine;

class TcpSocket
{

    // enums
public:
    enum SocketError
    {
        NoError = -2,
        UnknownSocketError = -1,
        ConnectionRefusedError = 0,
        RemoteHostClosedError,
        HostNotFoundError,
        SocketAccessError,
        SocketResourceError,
        SocketTimeoutError,
        NetworkError,
        UnsupportedSocketOperationError
    };

    enum SocketState
    {
        UnconnectedState = 0,
        ConnectedState
    };

    // ctor & dtor
public:
    TcpSocket();
    ~TcpSocket();

    // TcpSocket interface
public:
    // connection methods
    bool ConnectToHost(const std::string& hostName,
                       const uint16_t port,  // Connect("host", 80)
                       IBamIODevice::OpenMode mode = IBamIODevice::ReadOnly);
    bool ConnectToHost(const std::string& hostName,
                       const std::string& port,  // Connect("host", "80")
                       IBamIODevice::OpenMode mode = IBamIODevice::ReadOnly);
    void DisconnectFromHost();
    bool IsConnected() const;

    // I/O methods
    std::size_t BufferBytesAvailable() const;
    bool CanReadLine() const;
    void ClearBuffer();  // force buffer to clear (not a 'flush', just a 'discard')
    int64_t Read(char* data, const unsigned int numBytes);
    std::string ReadLine(int64_t max = 0);
    int64_t ReadLine(char* dest, std::size_t max);
    bool WaitForReadLine();
    int64_t Write(const char* data, const unsigned int numBytes);

    // connection values
    std::string GetHostName() const;
    //        HostAddress GetLocalAddress() const;
    //        uint16_t    GetLocalPort() const;
    HostAddress GetRemoteAddress() const;
    uint16_t GetRemotePort() const;

    // connection status
    TcpSocket::SocketError GetError() const;
    TcpSocket::SocketState GetState() const;
    std::string GetErrorString() const;

    // internal methods
private:
    bool ConnectImpl(const HostInfo& hostInfo, const std::string& port,
                     IBamIODevice::OpenMode mode);
    bool InitializeSocketEngine(HostAddress::NetworkProtocol protocol);
    int64_t ReadFromSocket();
    void ResetSocketEngine();

    // data members
private:
    IBamIODevice::OpenMode m_mode;

    std::string m_hostName;
    //        uint16_t    m_localPort;
    uint16_t m_remotePort;
    //        HostAddress m_localAddress;
    HostAddress m_remoteAddress;

    TcpSocketEngine* m_engine;
    int m_cachedSocketDescriptor;

    RollingBuffer m_readBuffer;

    TcpSocket::SocketError m_error;
    TcpSocket::SocketState m_state;
    std::string m_errorString;

    friend class BamHttp;
};

}  // namespace Internal
}  // namespace BamTools

#endif  // TCPSOCKET_P_H
