// ***************************************************************************
// HttpHeader_p.cpp (c) 2011 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// ---------------------------------------------------------------------------
// Last modified: 13 January 2012 (DB)
// ---------------------------------------------------------------------------
// Provides a generic interface for parsing/generating HTTP headers, along
// with specialized request & response header types
// ***************************************************************************

#include "api/internal/io/HttpHeader_p.h"
using namespace BamTools;
using namespace BamTools::Internal;

#include <cstddef>
#include <cstdlib>
#include <sstream>
#include <vector>

namespace BamTools {

// -----------
// constants
// -----------

namespace Constants {

static const char CAR_RET_CHAR = '\r';
static const char COLON_CHAR = ':';
static const char DOT_CHAR = '.';
static const char NEWLINE_CHAR = '\n';
static const char SPACE_CHAR = ' ';

static const std::string FIELD_NEWLINE = "\r\n";
static const std::string FIELD_SEPARATOR = ": ";
static const std::string HTTP_STRING = "HTTP/";

}  // namespace Constants

// ------------------------
// static utility methods
// ------------------------

namespace Internal {

static inline bool IsSpace(const char c)
{
    const int n = static_cast<int>(c);
    return (n == 0 || (n <= 13 && n >= 9));
}

// split on hitting single char delim
static std::vector<std::string> Split(const std::string& source, const char delim)
{
    std::stringstream ss(source);
    std::string field;
    std::vector<std::string> fields;
    while (std::getline(ss, field, delim))
        fields.push_back(field);
    return fields;
}

static std::string Trim(const std::string& source)
{

    // skip if empty string
    if (source.empty()) return source;

    // fetch string data
    const char* s = source.data();  // ignoring null-term on purpose
    const std::size_t size = source.size();
    std::size_t start = 0;
    std::size_t end = size - 1;

    // skip if no spaces at start or end
    if (!IsSpace(s[start]) && !IsSpace(s[end])) return source;

    // remove leading whitespace
    while ((start != end) && IsSpace(s[start]))
        ++start;

    // remove trailing whitespace
    if (start <= end) {
        while (end && IsSpace(s[end]))
            --end;
    }

    // return result
    return std::string(s + start, (end - start) + 1);
}

}  // namespace Internal
}  // namespace BamTools

// ---------------------------
// HttpHeader implementation
// ---------------------------

HttpHeader::HttpHeader()
    : m_isValid(true)
    , m_majorVersion(1)
    , m_minorVersion(1)
{}

HttpHeader::HttpHeader(const std::string& s)
    : m_isValid(true)
    , m_majorVersion(1)
    , m_minorVersion(1)
{
    Parse(s);
}

bool HttpHeader::ContainsKey(const std::string& key) const
{
    return (m_fields.find(key) != m_fields.end());
}

int HttpHeader::GetMajorVersion() const
{
    return m_majorVersion;
}

int HttpHeader::GetMinorVersion() const
{
    return m_minorVersion;
}

std::string HttpHeader::GetValue(const std::string& key)
{
    if (ContainsKey(key))
        return m_fields[key];
    else
        return std::string();
}

bool HttpHeader::IsValid() const
{
    return m_isValid;
}

void HttpHeader::Parse(const std::string& s)
{

    // trim whitespace from input string
    const std::string trimmed = Trim(s);

    // split into list of header lines
    std::vector<std::string> rawFields = Split(trimmed, Constants::NEWLINE_CHAR);

    // prep our 'cleaned' fields container
    std::vector<std::string> cleanFields;
    cleanFields.reserve(rawFields.size());

    // remove any empty fields and clean any trailing windows-style carriage returns ('\r')
    std::vector<std::string>::iterator rawFieldIter = rawFields.begin();
    std::vector<std::string>::iterator rawFieldEnd = rawFields.end();
    for (; rawFieldIter != rawFieldEnd; ++rawFieldIter) {
        std::string& field = (*rawFieldIter);

        // skip empty fields
        if (field.empty()) continue;

        // remove carriage returns
        const std::size_t fieldSize = field.size();
        if (field[fieldSize - 1] == Constants::CAR_RET_CHAR) field.resize(fieldSize - 1);

        // store cleaned field
        cleanFields.push_back(field);
    }

    // skip add'l processing if nothing here
    if (cleanFields.empty()) return;

    // parse header lines
    int lineNumber = 0;
    std::vector<std::string>::const_iterator fieldIter = cleanFields.begin();
    std::vector<std::string>::const_iterator fieldEnd = cleanFields.end();
    for (; fieldIter != fieldEnd; ++fieldIter, ++lineNumber) {
        if (!ParseLine((*fieldIter), lineNumber)) {
            m_isValid = false;
            return;
        }
    }
}

bool HttpHeader::ParseLine(const std::string& line, int)
{

    // find colon position, return failure if not found
    const std::size_t colonFound = line.find(Constants::COLON_CHAR);
    if (colonFound == std::string::npos) return false;

    // store key/value (without leading/trailing whitespace) & return success
    const std::string key = Trim(line.substr(0, colonFound));
    const std::string value = Trim(line.substr(colonFound + 1));
    m_fields[key] = value;
    return true;
}

void HttpHeader::RemoveField(const std::string& key)
{
    m_fields.erase(key);
}

void HttpHeader::SetField(const std::string& key, const std::string& value)
{
    m_fields[key] = value;
}

void HttpHeader::SetValid(bool ok)
{
    m_isValid = ok;
}

void HttpHeader::SetVersion(int major, int minor)
{
    m_majorVersion = major;
    m_minorVersion = minor;
}

std::string HttpHeader::ToString() const
{
    std::string result;
    if (m_isValid) {
        std::map<std::string, std::string>::const_iterator fieldIter = m_fields.begin();
        std::map<std::string, std::string>::const_iterator fieldEnd = m_fields.end();
        for (; fieldIter != fieldEnd; ++fieldIter) {
            const std::string& key = (*fieldIter).first;
            const std::string& value = (*fieldIter).second;
            const std::string& line =
                key + Constants::FIELD_SEPARATOR + value + Constants::FIELD_NEWLINE;
            result += line;
        }
    }
    return result;
}

// ----------------------------------
// HttpRequestHeader implementation
// ----------------------------------

HttpRequestHeader::HttpRequestHeader(const std::string& method, const std::string& resource,
                                     int majorVersion, int minorVersion)
    : HttpHeader()
    , m_method(method)
    , m_resource(resource)
{
    SetVersion(majorVersion, minorVersion);
}

std::string HttpRequestHeader::GetMethod() const
{
    return m_method;
}

std::string HttpRequestHeader::GetResource() const
{
    return m_resource;
}

bool HttpRequestHeader::ParseLine(const std::string& line, int lineNumber)
{

    // if not 'request line', just let base class parse
    if (lineNumber != 0) return HttpHeader::ParseLine(line, lineNumber);

    // fail if empty line
    if (line.empty()) return false;

    // walk through request line, storing positions
    //    GET /path/to/resource HTTP/1.1
    //    ^  ^^                ^^
    const std::size_t foundMethod =
        line.find_first_not_of(Constants::SPACE_CHAR);  // skip any leading whitespace
    if (foundMethod == std::string::npos) return false;
    const std::size_t foundFirstSpace = line.find(Constants::SPACE_CHAR, foundMethod + 1);
    if (foundFirstSpace == std::string::npos) return false;
    const std::size_t foundResource =
        line.find_first_not_of(Constants::SPACE_CHAR, foundFirstSpace + 1);
    if (foundResource == std::string::npos) return false;
    const std::size_t foundSecondSpace = line.find(Constants::SPACE_CHAR, foundResource + 1);
    if (foundSecondSpace == std::string::npos) return false;
    const std::size_t foundVersion =
        line.find_first_not_of(Constants::SPACE_CHAR, foundSecondSpace + 1);
    if (foundVersion == std::string::npos) return false;

    // parse out method & resource
    m_method = line.substr(foundMethod, foundFirstSpace - foundMethod);
    m_resource = line.substr(foundResource, foundSecondSpace - foundResource);

    // parse out version numbers
    const std::string temp = line.substr(foundVersion);
    if ((temp.find(Constants::HTTP_STRING) != 0) || (temp.size() != 8)) return false;
    const int major = static_cast<int>(temp.at(5) - '0');
    const int minor = static_cast<int>(temp.at(7) - '0');
    SetVersion(major, minor);

    // if we get here, return success
    return true;
}

std::string HttpRequestHeader::ToString() const
{
    std::stringstream request;
    request << m_method << Constants::SPACE_CHAR << m_resource << Constants::SPACE_CHAR
            << Constants::HTTP_STRING << GetMajorVersion() << Constants::DOT_CHAR
            << GetMinorVersion() << Constants::FIELD_NEWLINE << HttpHeader::ToString()
            << Constants::FIELD_NEWLINE;
    return request.str();
}

// -----------------------------------
// HttpResponseHeader implementation
// -----------------------------------

HttpResponseHeader::HttpResponseHeader(const int statusCode, const std::string& reason,
                                       int majorVersion, int minorVersion)

    : HttpHeader()
    , m_statusCode(statusCode)
    , m_reason(reason)
{
    SetVersion(majorVersion, minorVersion);
}

HttpResponseHeader::HttpResponseHeader(const std::string& s)
    : HttpHeader()
    , m_statusCode(0)
{
    Parse(s);
}

std::string HttpResponseHeader::GetReason() const
{
    return m_reason;
}

int HttpResponseHeader::GetStatusCode() const
{
    return m_statusCode;
}

bool HttpResponseHeader::ParseLine(const std::string& line, int lineNumber)
{

    // if not 'status line', just let base class
    if (lineNumber != 0) return HttpHeader::ParseLine(line, lineNumber);

    // fail if empty line
    if (line.empty()) return false;

    // walk through status line, storing positions
    //    HTTP/1.1 200 OK
    //    ^       ^^  ^^

    const std::size_t foundVersion =
        line.find_first_not_of(Constants::SPACE_CHAR);  // skip any leading whitespace
    if (foundVersion == std::string::npos) return false;
    const std::size_t foundFirstSpace = line.find(Constants::SPACE_CHAR, foundVersion + 1);
    if (foundFirstSpace == std::string::npos) return false;
    const std::size_t foundStatusCode =
        line.find_first_not_of(Constants::SPACE_CHAR, foundFirstSpace + 1);
    if (foundStatusCode == std::string::npos) return false;
    const std::size_t foundSecondSpace = line.find(Constants::SPACE_CHAR, foundStatusCode + 1);
    if (foundSecondSpace == std::string::npos) return false;
    const std::size_t foundReason =
        line.find_first_not_of(Constants::SPACE_CHAR, foundSecondSpace + 1);
    if (foundReason == std::string::npos) return false;

    // parse version numbers
    std::string temp = line.substr(foundVersion, foundFirstSpace - foundVersion);
    if ((temp.find(Constants::HTTP_STRING) != 0) || (temp.size() != 8)) return false;
    const int major = static_cast<int>(temp.at(5) - '0');
    const int minor = static_cast<int>(temp.at(7) - '0');
    SetVersion(major, minor);

    // parse status code
    temp = line.substr(foundStatusCode, foundSecondSpace - foundStatusCode);
    if (temp.size() != 3) return false;
    m_statusCode = std::atoi(temp.c_str());

    // reason phrase should be everything else left
    m_reason = line.substr(foundReason);

    // if we get here, return success
    return true;
}

std::string HttpResponseHeader::ToString() const
{
    std::stringstream response;
    response << Constants::HTTP_STRING << GetMajorVersion() << Constants::DOT_CHAR
             << GetMinorVersion() << Constants::SPACE_CHAR << m_statusCode << Constants::SPACE_CHAR
             << m_reason << Constants::FIELD_NEWLINE << HttpHeader::ToString()
             << Constants::FIELD_NEWLINE;
    return response.str();
}
