// ***************************************************************************
// BamException_p.h (c) 2011 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// ---------------------------------------------------------------------------
// Last modified: 6 October 2011 (DB)
// ---------------------------------------------------------------------------
// Provides a basic exception class for BamTools internals
// ***************************************************************************

#ifndef BAMEXCEPTION_P_H
#define BAMEXCEPTION_P_H

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

#include <exception>
#include <string>

namespace BamTools {
namespace Internal {

class API_NO_EXPORT BamException : public std::exception
{

public:
    BamException(const std::string& where, const std::string& message)
        : std::exception()
        , m_errorString(where + SEPARATOR + message)
    {}

    ~BamException() throw() {}

    const char* what() const throw()
    {
        return m_errorString.c_str();
    }

private:
    std::string m_errorString;
    static const std::string SEPARATOR;
};

}  // namespace Internal
}  // namespace BamTools

#endif  // BAMEXCEPTION_P_H
