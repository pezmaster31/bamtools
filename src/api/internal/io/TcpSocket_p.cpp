// ***************************************************************************
// TcpSocket_p.cpp (c) 2011 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// ---------------------------------------------------------------------------
// Last modified: 25 October 2011 (DB)
// ---------------------------------------------------------------------------
// Provides generic TCP socket (buffered) I/O
// ***************************************************************************

#include "api/internal/io/TcpSocket_p.h"
#include "api/internal/io/TcpSocketEngine_p.h"
using namespace BamTools;
using namespace BamTools::Internal;

#include <sstream>
#include <vector>
using namespace std;

// ------------------------------------
// static utility methods & constants
// ------------------------------------

namespace BamTools {
namespace Internal {

// constants
static const size_t DEFAULT_BUFFER_SIZE = 0x8000;

} // namespace Internal
} // namespace BamTools

// --------------------------
// TcpSocket implementation
// --------------------------

TcpSocket::TcpSocket(void)
    : m_mode(IBamIODevice::NotOpen)
//    , m_localPort(0)
    , m_remotePort(0)
    , m_engine(0)
    , m_cachedSocketDescriptor(-1)
    , m_readBuffer(DEFAULT_BUFFER_SIZE)
    , m_error(TcpSocket::UnknownSocketError)
    , m_state(TcpSocket::UnconnectedState)
{ }

TcpSocket::~TcpSocket(void) {
    if ( m_state == TcpSocket::ConnectedState )
        DisconnectFromHost();
}

size_t TcpSocket::BufferBytesAvailable(void) const {
    return m_readBuffer.Size();
}

bool TcpSocket::CanReadLine(void) const {
    return m_readBuffer.CanReadLine();
}

void TcpSocket::ClearBuffer(void) {
    m_readBuffer.Clear();
}

bool TcpSocket::ConnectImpl(const HostInfo& hostInfo,
                            const std::string& port,
                            IBamIODevice::OpenMode mode)
{
    // skip if we're already connected
    if ( m_state == TcpSocket::ConnectedState ) {
        m_error = TcpSocket::SocketResourceError;
        return false;
    }

    // reset socket state
    m_mode =  mode;
    m_hostName = hostInfo.HostName();
    m_state = TcpSocket::UnconnectedState;
    m_error = TcpSocket::UnknownSocketError;
//    m_localPort  = 0;
    m_remotePort = 0;
//    m_localAddress.Clear();
    m_remoteAddress.Clear();
    m_readBuffer.Clear();

    // fetch candidate addresses for requested host
    vector<HostAddress> addresses = hostInfo.Addresses();
    if ( addresses.empty() ) {
        m_error = TcpSocket::HostNotFoundError;
        return false;
    }

    // convert port string to integer
    stringstream ss(port);
    uint16_t portNumber(0);
    ss >> portNumber;

    // iterate through adddresses
    vector<HostAddress>::const_iterator addrIter = addresses.begin();
    vector<HostAddress>::const_iterator addrEnd  = addresses.end();
    for ( ; addrIter != addrEnd; ++addrIter) {
        const HostAddress& addr = (*addrIter);

        // try to initialize socket engine with this address
        if ( !InitializeSocketEngine(addr.GetProtocol()) ) {
            // failure to initialize is OK here
            // we'll just try the next available address
            continue;
        }

        // attempt actual connection
        if ( m_engine->Connect(addr, portNumber) ) {

            // if connection successful, update our state & return true
            m_mode = mode;
//            m_localAddress  = m_engine->GetLocalAddress();
//            m_localPort     = m_engine->GetLocalPort();
            m_remoteAddress = m_engine->GetRemoteAddress();
            m_remotePort    = m_engine->GetRemotePort();
            m_cachedSocketDescriptor = m_engine->GetSocketDescriptor();
            m_state = TcpSocket::ConnectedState;
            return true;
        }
    }

    // if we get here, no connection could be made
    m_error = TcpSocket::HostNotFoundError;
    return false;
}

bool TcpSocket::ConnectToHost(const string& hostName,
                              uint16_t port,
                              IBamIODevice::OpenMode mode)
{
    stringstream ss("");
    ss << port;
    return ConnectToHost(hostName, ss.str(), mode);

}

bool TcpSocket::ConnectToHost(const string& hostName,
                              const string& port,
                              IBamIODevice::OpenMode mode)
{
    // create new address object with requested host name
    HostAddress hostAddress;
    hostAddress.SetAddress(hostName);

    HostInfo info;
    // if host name was IP address ("x.x.x.x" or IPv6 format)
    // otherwise host name was 'plain-text' ("www.foo.bar")
    // we need to look up IP address(es)
    if ( hostAddress.HasIPAddress() )
        info.SetAddresses( vector<HostAddress>(1, hostAddress) );
    else
        info = HostInfo::Lookup(hostName, port);

    // attempt connection on requested port
    return ConnectImpl(info, port, mode);
}

void TcpSocket::DisconnectFromHost(void) {

    // close socket engine & delete
    if ( m_state == TcpSocket::ConnectedState )
        ResetSocketEngine();

    // reset connection state
//    m_localPort = 0;
    m_remotePort = 0;
//    m_localAddress.Clear();
    m_remoteAddress.Clear();
    m_hostName.clear();
    m_cachedSocketDescriptor = -1;
}

TcpSocket::SocketError TcpSocket::GetError(void) const {
    return m_error;
}

std::string TcpSocket::GetErrorString(void) const {
    return m_errorString;
}

std::string TcpSocket::GetHostName(void) const {
    return m_hostName;
}

//HostAddress TcpSocket::GetLocalAddress(void) const {
//    return m_localAddress;
//}

//uint16_t TcpSocket::GetLocalPort(void) const {
//    return m_localPort;
//}

HostAddress TcpSocket::GetRemoteAddress(void) const {
    return m_remoteAddress;
}

uint16_t TcpSocket::GetRemotePort(void) const {
    return m_remotePort;
}

TcpSocket::SocketState TcpSocket::GetState(void) const {
    return m_state;
}

bool TcpSocket::InitializeSocketEngine(HostAddress::NetworkProtocol protocol) {
    ResetSocketEngine();
    m_engine = new TcpSocketEngine;
    return m_engine->Initialize(protocol);
}

bool TcpSocket::IsConnected(void) const {
    if ( m_engine == 0 )
        return false;
    return ( m_engine->IsValid() && (m_state == TcpSocket::ConnectedState) );
}

// may be read in a look until desired data amount has been read
// returns: number of bytes read, or -1 if error
int64_t TcpSocket::Read(char* data, const unsigned int numBytes) {

    // if we have data in buffer, just return it
    if ( !m_readBuffer.IsEmpty() ) {
        const size_t bytesRead = m_readBuffer.Read(data, numBytes);
        return static_cast<int64_t>(bytesRead);
    }

    // otherwise, we'll need to fetch data from socket
    // first make sure we have a valid socket engine
    if ( m_engine == 0 ) {
        // TODO: set error string/state?
        return -1;
    }

    // fetch data from socket, return 0 for success, -1 for failure
    // since this should be called in a loop, we'll pull the actual bytes on next iteration
    return ( ReadFromSocket() ? 0 : -1 );
}

bool TcpSocket::ReadFromSocket(void) {

    // wait for ready read
    bool timedOut;
    bool isReadyRead = m_engine->WaitForRead(5000, &timedOut);

    // if not ready
    if ( !isReadyRead ) {

        // if we simply timed out
        if ( timedOut ) {
            // TODO: set error string
            return false;
        }

        // otherwise, there was an error
        else {
            // TODO: set error string
            return false;
        }
    }

    // #########################################################################
    // clean this up - smells funky, but it's a key step so it has to be right
    // #########################################################################
    // get number of bytes available from socket
    // (if 0, still try to read some data so we don't trigger any OS event behavior
    //  that respond to repeated access to a remote closed socket)
    int64_t bytesToRead = m_engine->NumBytesAvailable();
    if ( bytesToRead < 0 )
        return false;
    else if ( bytesToRead == 0 )
        bytesToRead = 4096;

    // make space in buffer & read from socket
    char* buffer = m_readBuffer.Reserve(bytesToRead);
    int64_t numBytesRead = m_engine->Read(buffer, bytesToRead);

    // (Qt uses -2 for no data, not error)
    // squeeze buffer back down & return success
    if ( numBytesRead == -2 ) {
        m_readBuffer.Chop(bytesToRead);
        return true;
    }
    // #########################################################################

    // check for any socket engine errors
    if ( !m_engine->IsValid() ) {
        // TODO: set error string
        ResetSocketEngine();
        return false;
    }

    // return success
    return true;
}

string TcpSocket::ReadLine(void) {
    if ( m_readBuffer.CanReadLine() )
        return m_readBuffer.ReadLine();
    return string();
}

void TcpSocket::ResetSocketEngine(void) {

    // shut down socket engine
    if ( m_engine ) {
        m_engine->Close();
        delete m_engine;
        m_engine = 0;
    }

    // reset our state & cached socket handle
    m_state = TcpSocket::UnconnectedState;
    m_cachedSocketDescriptor = -1;
}

int64_t TcpSocket::Write(const char* data, const unsigned int numBytes) {

    // single-shot attempt at write (not buffered, just try to shove the data through socket)
    // this method purely exists to send 'small' HTTP requests/FTP commands from client to server

    int64_t bytesWritten(0);

    // wait for our socket to be write-able
    bool timedOut;
    bool isReadyWrite = m_engine->WaitForWrite(3000, &timedOut);
    if ( isReadyWrite )
        bytesWritten = m_engine->Write(data, numBytes);
    else {
        // timeout is OK (with current setup), we'll just return 0 & try again
        // but we need to report if engine encountered some other error
        if ( !timedOut ) {
            // TODO: set error string
            bytesWritten = -1;
        }
    }

    // return actual number of bytes written to socket
    return bytesWritten;
}
