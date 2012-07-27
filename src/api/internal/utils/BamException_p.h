
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

#define EX_SEPARATOR ": " //Use this instead of a static const std::string class member which creates nasty link dependencies
namespace BamTools {
namespace Internal {

class BamException : public std::exception {

    public:
        inline BamException(const std::string& where, const std::string& message)
            : std::exception()
            , m_errorString(where + std::string(EX_SEPARATOR) + message)
        { }

        inline ~BamException(void) throw() { }

        inline const char* what(void) const throw() {
            return m_errorString.c_str();
        }

    private:
        std::string m_errorString;
};

} // namespace Internal
} // namespace BamTools

#endif // BAMEXCEPTION_P_H
