// ***************************************************************************
// BamHttp_p.h (c) 2011 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// ---------------------------------------------------------------------------
// Last modified: 10 November 2011 (DB)
// ---------------------------------------------------------------------------
// Provides reading/writing of BAM files on HTTP server
// ***************************************************************************

#ifndef BAMHTTP_P_H
#define BAMHTTP_P_H

//  -------------
//  W A R N I N G
//  -------------
//
// This file is not part of the BamTools API.  It exists purely as an
// implementation detail. This header file may change from version to version
// without notice, or even be removed.
//
// We mean it.

#include <string>
#include "api/IBamIODevice.h"

namespace BamTools {
namespace Internal {

class HttpRequestHeader;
class HttpResponseHeader;
class TcpSocket;

class BamHttp : public IBamIODevice
{

    // ctor & dtor
public:
    BamHttp(const std::string& url);
    ~BamHttp();

    // IBamIODevice implementation
public:
    void Close();
    bool IsOpen() const;
    bool IsRandomAccess() const;
    bool Open(const IBamIODevice::OpenMode mode);
    int64_t Read(char* data, const unsigned int numBytes);
    bool Seek(const int64_t& position, const int origin = SEEK_SET);
    int64_t Tell() const;
    int64_t Write(const char* data, const unsigned int numBytes);

    // internal methods
private:
    void ClearResponse();
    bool ConnectSocket();
    void DisconnectSocket();
    bool EnsureSocketConnection();
    void ParseUrl(const std::string& url);
    int64_t ReadFromSocket(char* data, const unsigned int numBytes);
    bool ReceiveResponse();
    bool SendGetRequest(const size_t numBytes = 0x10000);
    bool SendHeadRequest();
    int64_t WriteToSocket(const char* data, const unsigned int numBytes);

    // data members
private:
    // our main socket
    TcpSocket* m_socket;

    // our connection data
    std::string m_hostname;
    std::string m_port;
    std::string m_filename;

    // our last (active) request & response info
    HttpRequestHeader* m_request;
    HttpResponseHeader* m_response;

    // internal state flags
    bool m_isUrlParsed;

    // file position
    int64_t m_filePosition;
    int64_t m_fileEndPosition;
    int64_t m_rangeEndPosition;
};

}  // namespace Internal
}  // namespace BamTools

#endif  // BAMHTTP_P_H
