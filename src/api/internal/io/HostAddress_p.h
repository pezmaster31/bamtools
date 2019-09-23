// ***************************************************************************
// HostAddress_p.h (c) 2011 Derek Barnett
// Marth Lab, Department of Biology, Boston College
// ---------------------------------------------------------------------------
// Last modified: 10 November 2011 (DB)
// ---------------------------------------------------------------------------
// Provides a generic IP address container
// ***************************************************************************

#ifndef HOSTADDRESS_P_H
#define HOSTADDRESS_P_H

//  -------------
//  W A R N I N G
//  -------------
//
// This file is not part of the BamTools API.  It exists purely as an
// implementation detail. This header file may change from version to version
// without notice, or even be removed.
//
// We mean it.

#include <cstddef>
#include <cstring>
#include <string>
#include "api/api_global.h"

namespace BamTools {
namespace Internal {

struct IPv6Address
{

    // ctor
    IPv6Address()
    {
        std::memset(&data, 0, sizeof(uint8_t) * 16);
    }

    // data access (no bounds checking)
    uint8_t& operator[](std::size_t index)
    {
        return data[index];
    }

    uint8_t operator[](std::size_t index) const
    {
        return data[index];
    }

    // data
    uint8_t data[16];
};

class HostAddress
{

    // enums
public:
    enum NetworkProtocol
    {
        UnknownNetworkProtocol = -1,
        IPv4Protocol = 0,
        IPv6Protocol
    };

    // ctors & dtor
public:
    HostAddress();
    explicit HostAddress(const uint32_t ip4Address);
    explicit HostAddress(const uint8_t* ip6Address);
    explicit HostAddress(const IPv6Address& ip6Address);
    explicit HostAddress(const std::string& address);

    // HostAddress interface
public:
    void Clear();
    bool HasIPAddress() const;  // returns whether string address could be converted to IP address
    bool IsNull() const;

    uint32_t GetIPv4Address() const;
    IPv6Address GetIPv6Address() const;
    std::string GetIPString() const;
    HostAddress::NetworkProtocol GetProtocol() const;

    void SetAddress(const uint32_t ip4Address);
    void SetAddress(const uint8_t* ip6Address);
    void SetAddress(const IPv6Address& ip6Address);
    void SetAddress(const std::string& address);

    // HostAddress comparison operators
public:
    bool operator==(const HostAddress& other) const;
    bool operator!=(const HostAddress& other) const
    {
        return !(operator==(other));
    }
    bool operator<(const HostAddress& other) const;

    // internal methods
private:
    bool ParseAddress();

    // data members
private:
    HostAddress::NetworkProtocol m_protocol;
    uint32_t m_ip4Address;
    IPv6Address m_ip6Address;
    std::string m_ipString;
    bool m_hasIpAddress;  // true until string passed in, then signifies whether string was an IP
};

}  // namespace Internal
}  // namespace BamTools

#endif  // HOSTADDRESS_P_H
