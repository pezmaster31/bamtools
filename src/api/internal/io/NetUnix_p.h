#ifndef NETUNIX_P_H
#define NETUNIX_P_H

#ifndef _WIN32 // <-- source files only include the proper Net*_p.h, but this is a double-check

#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>

#ifndef   BT_SOCKLEN_T
#  define BT_SOCKLEN_T socklen_t
#endif

namespace BamTools {
namespace Internal {

} // namespace Internal
} // namespace BamTools

#endif // _WIN32
#endif // NETUNIX_P_H
