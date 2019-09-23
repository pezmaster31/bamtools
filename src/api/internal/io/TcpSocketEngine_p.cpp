// ***************************************************************************
// TcpSocketEngine_p.cpp (c) 2011 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// ---------------------------------------------------------------------------
// Last modified: 8 December 2011 (DB)
// ---------------------------------------------------------------------------
// Provides low-level implementation of TCP I/O
// ***************************************************************************

// N.B. - this file contains the top-level, platform-independent logic. "Native" methods
//        are called as needed from the TcpSocketEngine_<X>.cpp files. Selection of the proper
//        native method file should have been handled at build-time by CMake.

#include "api/internal/io/TcpSocketEngine_p.h"
#include "api/internal/io/HostInfo_p.h"

#include <cstddef>

using namespace BamTools;
using namespace BamTools::Internal;

TcpSocketEngine::TcpSocketEngine()
    : m_socketDescriptor(-1)
    //    , m_localPort(0)
    , m_remotePort(0)
    , m_socketError(TcpSocket::UnknownSocketError)
    , m_socketState(TcpSocket::UnconnectedState)
{}

TcpSocketEngine::~TcpSocketEngine()
{
    Close();
}

void TcpSocketEngine::Close()
{

    // close socket if we have valid FD
    if (m_socketDescriptor != -1) {
        nativeClose();
        m_socketDescriptor = -1;
    }

    // reset state
    m_socketState = TcpSocket::UnconnectedState;
    //    m_localAddress.Clear();
    m_remoteAddress.Clear();
    //    m_localPort = 0;
    m_remotePort = 0;
}

bool TcpSocketEngine::Connect(const HostAddress& address, const uint16_t port)
{

    // return failure if invalid FD or already connected
    if (!IsValid() || (m_socketState == TcpSocket::ConnectedState)) {
        // TODO: set error string
        return false;
    }

    // attempt to connect to host address on requested port
    if (!nativeConnect(address, port)) {
        // TODO: set error string
        return false;
    }

    // if successful, store remote host address port & return success
    // TODO: (later) fetch proxied remote & local host/port  here
    m_remoteAddress = address;
    m_remotePort = port;
    return true;
}

std::string TcpSocketEngine::GetErrorString() const
{
    return m_errorString;
}

//HostAddress TcpSocketEngine::GetLocalAddress() const {
//    return m_localAddress;
//}

//uint16_t TcpSocketEngine::GetLocalPort() const {
//    return m_localPort;
//}

HostAddress TcpSocketEngine::GetRemoteAddress() const
{
    return m_remoteAddress;
}

uint16_t TcpSocketEngine::GetRemotePort() const
{
    return m_remotePort;
}

int TcpSocketEngine::GetSocketDescriptor() const
{
    return m_socketDescriptor;
}

TcpSocket::SocketError TcpSocketEngine::GetSocketError()
{
    return m_socketError;
}

TcpSocket::SocketState TcpSocketEngine::GetSocketState()
{
    return m_socketState;
}

bool TcpSocketEngine::Initialize(HostAddress::NetworkProtocol protocol)
{

    // close current socket if we have one open
    if (IsValid()) Close();

    // attempt to create new socket
    return nativeCreateSocket(protocol);
}

bool TcpSocketEngine::IsValid() const
{
    return (m_socketDescriptor != -1);
}

int64_t TcpSocketEngine::NumBytesAvailable() const
{

    // return 0 if socket FD is invalid
    if (!IsValid()) {
        // TODO: set error string
        return -1;
    }

    // otherwise check socket to see how much is ready
    return nativeNumBytesAvailable();
}

int64_t TcpSocketEngine::Read(char* dest, std::size_t max)
{

    // return failure if can't read
    if (!IsValid() || (m_socketState != TcpSocket::ConnectedState)) return -1;

    // otherwise return number of bytes read
    return nativeRead(dest, max);
}

bool TcpSocketEngine::WaitForRead(int msec, bool* timedOut)
{

    // reset timedOut flag
    *timedOut = false;

    // need to wait for our socket to be ready to read
    const int ret = nativeSelect(msec, true);

    // if timed out
    if (ret == 0) {
        *timedOut = true;
        m_socketError = TcpSocket::SocketTimeoutError;
        m_errorString = "socket timed out";
    }

    // return if any sockets available for reading
    return (ret > 0);
}

bool TcpSocketEngine::WaitForWrite(int msec, bool* timedOut)
{

    // reset timedOut flag
    *timedOut = false;

    // need to wait for our socket to be ready to write
    const int ret = nativeSelect(msec, false);

    // if timed out
    if (ret == 0) {
        *timedOut = true;
        m_socketError = TcpSocket::SocketTimeoutError;
        m_errorString = "socket timed out";
    }

    // return if any sockets available for reading
    return (ret > 0);
}

int64_t TcpSocketEngine::Write(const char* data, std::size_t length)
{

    // return failure if can't write
    if (!IsValid() || (m_socketState != TcpSocket::ConnectedState)) {
        // TODO: set error string
        return -1;
    }

    // otherwise return number of bytes written
    return nativeWrite(data, length);
}
