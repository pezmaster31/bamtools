// ***************************************************************************
// HttpHeader_p.h (c) 2011 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// ---------------------------------------------------------------------------
// Last modified: 13 January 2012 (DB)
// ---------------------------------------------------------------------------
// Provides a generic interface for parsing/generating HTTP headers, along
// with specialized request & response header types
// ***************************************************************************

#ifndef HTTP_HEADER_P_H
#define HTTP_HEADER_P_H

#include "api/api_global.h"

//  -------------
//  W A R N I N G
//  -------------
//
// This file is not part of the BamTools API.  It exists purely as an
// implementation detail. This header file may change from version to version
// without notice, or even be removed.
//
// We mean it.

#include <map>
#include <string>

namespace BamTools {
namespace Internal {

class API_NO_EXPORT HttpHeader
{

    // ctors & dtor
public:
    HttpHeader();
    HttpHeader(const std::string& s);
    virtual ~HttpHeader() {}

    // HttpHeader interface
public:
    // header field=>value access
    bool ContainsKey(const std::string& key) const;
    std::string GetValue(const std::string& key);
    void RemoveField(const std::string& key);
    void SetField(const std::string& key, const std::string& value);

    // get formatted header string
    virtual std::string ToString() const;

    // query HTTP version used
    int GetMajorVersion() const;
    int GetMinorVersion() const;

    // see if header was parsed OK
    bool IsValid() const;

    // internal methods
protected:
    void Parse(const std::string& s);
    virtual bool ParseLine(const std::string& line, int lineNumber);
    void SetValid(bool ok);
    void SetVersion(int major, int minor);

    // data members
private:
    std::map<std::string, std::string> m_fields;

    bool m_isValid;  // should usually be true, only false if error processing a header line
    int m_majorVersion;
    int m_minorVersion;
};

class API_NO_EXPORT HttpRequestHeader : public HttpHeader
{

    // ctor & dtor
public:
    HttpRequestHeader(const std::string& method,    // "GET", "HEAD", ...
                      const std::string& resource,  // filename
                      int majorVersion = 1,         // version info
                      int minorVersion = 1);

    // HttpRequestHeader interface
public:
    std::string GetMethod() const;
    std::string GetResource() const;

    // HttpHeader implementation
public:
    std::string ToString() const;

protected:
    bool ParseLine(const std::string& line, int lineNumber);

    // data members
private:
    std::string m_method;
    std::string m_resource;
};

class API_NO_EXPORT HttpResponseHeader : public HttpHeader
{

    // ctor & dtor
public:
    HttpResponseHeader(const int statusCode,                       // 200, 404, etc
                       const std::string& reason = std::string(),  // 'reason phrase' for code
                       int majorVersion = 1,                       // version info
                       int minorVersion = 1);
    HttpResponseHeader(const std::string& s);

    // HttpRequestHeader interface
public:
    std::string GetReason() const;
    int GetStatusCode() const;

    // HttpHeader implementation
public:
    std::string ToString() const;

protected:
    bool ParseLine(const std::string& line, int lineNumber);

    // data members
private:
    int m_statusCode;
    std::string m_reason;
};

}  // namespace Internal
}  // namespace BamTools

#endif  // HTTP_HEADER_P_H
