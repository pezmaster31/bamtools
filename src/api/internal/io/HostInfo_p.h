#ifndef HOSTINFO_P_H
#define HOSTINFO_P_H

#include "api/internal/io/HostAddress_p.h"
#include <string>
#include <vector>

namespace BamTools {
namespace Internal {

class HostInfo {

    public:
        enum ErrorType { NoError = 0
                       , HostNotFound
                       , UnknownError
                       };

    // ctors & dtor
    public:
        HostInfo(void);
        HostInfo(const HostInfo& other);
        ~HostInfo(void);

    // HostInfo interface
    public:
        std::string HostName(void) const;
        void SetHostName(const std::string& name);

        std::vector<HostAddress> Addresses(void) const;
        void SetAddresses(const std::vector<HostAddress>& addresses);

        HostInfo::ErrorType GetError(void) const;
        std::string GetErrorString(void) const;

    // internal methods
    private:
        void SetError(const HostInfo::ErrorType error);
        void SetErrorString(const std::string& errorString);

    // static methods
    public:
        static HostInfo Lookup(const std::string& hostname,
                               const std::string& port);

    // data members
    private:
        std::string m_hostName;
        std::vector<HostAddress> m_addresses;
        HostInfo::ErrorType m_error;
        std::string m_errorString;
};

} // namespace Internal
} // namespace BamTools

#endif // HOSTINFO_P_H
