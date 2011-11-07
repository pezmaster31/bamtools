#ifndef TCPSOCKETENGINE_P_H
#define TCPSOCKETENGINE_P_H

#include "api/internal/io/HostAddress_p.h"
#include "api/internal/io/TcpSocket_p.h"

namespace BamTools {
namespace Internal {

struct TcpSocketEngine {

    // ctors & dtor
    public:
        TcpSocketEngine(void);
        TcpSocketEngine(const TcpSocketEngine& other);
        ~TcpSocketEngine(void);

    // TcpSocketEngine interface
    public:

        // connection-related methods
        void Close(void);
        bool Connect(const HostAddress& address, const uint16_t port);
        bool Initialize(HostAddress::NetworkProtocol protocol);
        bool IsValid(void) const;

        // IO-related methods
        int64_t NumBytesAvailable(void) const;
        int64_t Read(char* dest, size_t max);
        int64_t Write(const char* data, size_t length);

        bool WaitForRead(int msec, bool* timedOut);
        bool WaitForWrite(int msec, bool* timedOut);

        // query connection state
//        HostAddress GetLocalAddress(void) const;
//        uint16_t GetLocalPort(void) const;
        HostAddress GetRemoteAddress(void) const;
        uint16_t    GetRemotePort(void) const;

        int GetSocketDescriptor(void) const;
        TcpSocket::SocketError GetSocketError(void);
        TcpSocket::SocketState GetSocketState(void);

        std::string GetErrorString(void) const;

    // platform-dependent internal methods
    // provided in the corresponding TcpSocketEngine_<OS>_p.cpp
    private:
        void    nativeClose(void);
        bool    nativeConnect(const HostAddress& address, const uint16_t port);
        bool    nativeCreateSocket(HostAddress::NetworkProtocol protocol);
        void    nativeDisconnect(void);
        bool    nativeFetchConnectionParameters(void);
        int64_t  nativeNumBytesAvailable(void) const;
        int64_t nativeRead(char* dest, size_t max);
        int     nativeSelect(int msecs, bool isRead) const;
        int64_t nativeWrite(const char* data, size_t length);

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
};

} // namespace Internal
} // namespace BamTools

#endif // TCPSOCKETENGINE_P_H
