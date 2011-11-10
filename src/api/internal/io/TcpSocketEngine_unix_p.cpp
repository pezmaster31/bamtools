// ***************************************************************************
// TcpSocketEngine_unix_p.cpp (c) 2011 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// ---------------------------------------------------------------------------
// Last modified: 10 November 2011 (DB)
// ---------------------------------------------------------------------------
// Provides low-level implementation of TCP I/O for all UNIX-like systems
// ***************************************************************************

#include "api/internal/io/TcpSocketEngine_p.h"
#include "api/internal/io/NetUnix_p.h"
using namespace BamTools;
using namespace BamTools::Internal;

#include <cerrno>
#include <ctime>
#include <iostream>
using namespace std;

// ------------------------
// static utility methods
// ------------------------

namespace BamTools {
namespace Internal {

//static inline
//void getPortAndAddress(const sockaddr* s, uint16_t& port, HostAddress& address) {

//    // IPv6
//    if (s->sa_family == AF_INET6) {
//        sockaddr_in6* ip6 = (sockaddr_in6*)s;
//        port = ntohs(ip6->sin6_port);
//        IPv6Address tmp;
//        memcpy(&tmp.data, &(ip6->sin6_addr.s6_addr), sizeof(tmp));
//        address.SetAddress(tmp);
//        return;
//    }

//    // IPv4
//    if ( s->sa_family == AF_INET ) {
//        sockaddr_in* ip4 = (sockaddr_in*)s;
//        port = ntohl(ip4->sin_port);
//        address.SetAddress( ntohl(ip4->sin_addr.s_addr) );
//        return;
//    }

//    // should be unreachable
//    BT_ASSERT_X(false, "TcpSocketEngine::getPortAndAddress() : unknown network protocol ");
//}

} // namespace Internal
} // namespace BamTools

// --------------------------------
// TcpSocketEngine implementation
// --------------------------------

void TcpSocketEngine::nativeClose(void) {
    close(m_socketDescriptor);
}

bool TcpSocketEngine::nativeConnect(const HostAddress& address, const uint16_t port) {

    // setup connection parameters from address/port
    sockaddr_in  sockAddrIPv4;
    sockaddr_in6 sockAddrIPv6;
    sockaddr*    sockAddrPtr  = 0;
    BT_SOCKLEN_T sockAddrSize = 0;

    // IPv6
    if ( address.GetProtocol() == HostAddress::IPv6Protocol ) {

        memset(&sockAddrIPv6, 0, sizeof(sockAddrIPv6));
        sockAddrIPv6.sin6_family = AF_INET6;
        sockAddrIPv6.sin6_port   = htons(port);

        IPv6Address ip6 = address.GetIPv6Address();
        memcpy(&sockAddrIPv6.sin6_addr.s6_addr, &ip6, sizeof(ip6));

        sockAddrSize = sizeof(sockAddrIPv6);
        sockAddrPtr  = (sockaddr*)&sockAddrIPv6;
    }

    // IPv4
    else if ( address.GetProtocol() == HostAddress::IPv4Protocol ) {

        memset(&sockAddrIPv4, 0, sizeof(sockAddrIPv4));
        sockAddrIPv4.sin_family      = AF_INET;
        sockAddrIPv4.sin_port        = htons(port);
        sockAddrIPv4.sin_addr.s_addr = htonl(address.GetIPv4Address());

        sockAddrSize = sizeof(sockAddrIPv4);
        sockAddrPtr  = (sockaddr*)&sockAddrIPv4;
    }

    // unknown (should be unreachable)
    else BT_ASSERT_X(false, "TcpSocketEngine::nativeConnect() : unknown network protocol");

    // attempt connection
    int connectResult = connect(m_socketDescriptor, sockAddrPtr, sockAddrSize);

    // if hit error
    if ( connectResult == -1 ) {

        // see what error was encountered
        switch ( errno ) {

            case EISCONN:
                m_socketState = TcpSocket::ConnectedState;
                break;
            case ECONNREFUSED:
            case EINVAL:
                m_socketError = TcpSocket::ConnectionRefusedError;
                m_socketState = TcpSocket::UnconnectedState;
                m_errorString = "connection refused";
                break;
            case ETIMEDOUT:
                m_socketError = TcpSocket::NetworkError;
                m_errorString = "connection timed out";
                break;
            case EHOSTUNREACH:
                m_socketError = TcpSocket::NetworkError;
                m_socketState = TcpSocket::UnconnectedState;
                m_errorString = "host unreachable";
                break;
            case ENETUNREACH:
                m_socketError = TcpSocket::NetworkError;
                m_socketState = TcpSocket::UnconnectedState;
                m_errorString = "network unreachable";
                break;
            case EADDRINUSE:
                m_socketError = TcpSocket::NetworkError;
                m_errorString = "address already in use";
                break;
            case EACCES:
            case EPERM:
                m_socketError = TcpSocket::SocketAccessError;
                m_socketState = TcpSocket::UnconnectedState;
                m_errorString = "permission denied";
            case EAFNOSUPPORT:
            case EBADF:
            case EFAULT:
            case ENOTSOCK:
                m_socketState = TcpSocket::UnconnectedState;
            default:
                break;
        }

        if ( m_socketState != TcpSocket::ConnectedState )
            return false;
    }

    // otherwise, we should be good
    // update state & return success
    m_socketState = TcpSocket::ConnectedState;
    return true;
}

bool TcpSocketEngine::nativeCreateSocket(HostAddress::NetworkProtocol protocol) {

    // get protocol value for requested protocol type
    const int protocolNum = ( (protocol == HostAddress::IPv6Protocol) ? AF_INET6
                                                                      : AF_INET );

    // attempt to create socket
    int socketFd = socket(protocolNum, SOCK_STREAM, IPPROTO_TCP);

    // if we fetched an invalid socket descriptor
    if ( socketFd <= 0 ) {

        // see what error we got
        switch ( errno ) {
            case EPROTONOSUPPORT:
            case EAFNOSUPPORT:
            case EINVAL:
                m_socketError = TcpSocket::UnsupportedSocketOperationError;
                m_errorString = "protocol not supported";
                break;
            case ENFILE:
            case EMFILE:
            case ENOBUFS:
            case ENOMEM:
                m_socketError = TcpSocket::SocketResourceError;
                m_errorString = "out of resources";
                break;
            case EACCES:
                m_socketError = TcpSocket::SocketAccessError;
                m_errorString = "permission denied";
                break;
            default:
                break;
        }

        // return failure
        return false;
    }

    // otherwise, store our socket FD & return success
    m_socketDescriptor = socketFd;
    return true;
}

//bool TcpSocketEngine::nativeFetchConnectionParameters(void) {

//    // reset addresses/ports
////    m_localAddress.Clear();
//    m_remoteAddress.Clear();
////    m_localPort  = 0;
//    m_remotePort = 0;

//    // skip (return failure) if invalid socket FD
//    if ( m_socketDescriptor == -1 )
//        return false;

//    sockaddr sa;
//    BT_SOCKLEN_T sockAddrSize = sizeof(sa);

//    // fetch local address info
//    memset(&sa, 0, sizeof(sa));
//    if ( getsockname(m_socketDescriptor, &sa, &sockAddrSize) == 0 )
//        getPortAndAddress(&sa, m_localPort, m_localAddress);
//    else if ( errno == EBADF ) {
//        m_socketError = TcpSocket::UnsupportedSocketOperationError;
//        m_errorString = "invalid socket descriptor";
//        return false;
//    }

//    // fetch remote address
//    if ( getpeername(m_socketDescriptor, &sa, &sockAddrSize) == 0 )
//        getPortAndAddress(&sa, m_remotePort, m_remoteAddress);

//    // return success
//    return true;
//}

int64_t TcpSocketEngine::nativeNumBytesAvailable(void) const {

    // fetch number of bytes, return 0 on error
    int numBytes(0);
    if ( ioctl(m_socketDescriptor, FIONREAD, (char*)&numBytes) < 0 )
        return -1;
    return static_cast<int64_t>(numBytes);
}

int64_t TcpSocketEngine::nativeRead(char* dest, size_t max) {

    if ( !IsValid() )
        return -1;

    ssize_t ret = read(m_socketDescriptor, dest, max);
    if ( ret < 0 ) {
        ret = -1;
        switch ( errno ) {
            case EAGAIN :
                // No data was available for reading
                ret = -2;
                break;
            case ECONNRESET :
                ret = 0;
                break;
            default:
                break;
        }
    }
    return static_cast<int64_t>(ret);
}

// negative value for msecs will block (forever) until ready
int TcpSocketEngine::nativeSelect(int msecs, bool isRead) const {

    // set up FD set
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(m_socketDescriptor, &fds);

    // setup our timeout
    timeval tv;
    tv.tv_sec  = msecs / 1000;
    tv.tv_usec = (msecs % 1000) * 1000;

    // do 'select'
    int ret;
    if ( isRead )
        ret = select(m_socketDescriptor + 1, &fds, 0, 0, (msecs < 0 ? 0 : &tv));
    else
        ret = select(m_socketDescriptor + 1, 0, &fds, 0, (msecs < 0 ? 0 : &tv));
    return ret;
}

int64_t TcpSocketEngine::nativeWrite(const char* data, size_t length) {

    ssize_t writtenBytes = write(m_socketDescriptor, data, length);
    if ( writtenBytes < 0 ) {
        switch (errno) {
            case EPIPE:
            case ECONNRESET:
                writtenBytes = -1;
                m_socketError = TcpSocket::RemoteHostClosedError;
                m_errorString = "remote host closed connection";
                Close();
                break;
            case EAGAIN:
                writtenBytes = 0;
                break;
            default:
                break;
        }
    }
    return static_cast<int64_t>(writtenBytes);
}
