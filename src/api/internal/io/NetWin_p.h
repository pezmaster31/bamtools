#ifndef NETWIN_P_H
#define NETWIN_P_H

#ifdef _WIN32 // <-- source files only include the proper Net*_p.h, but this is a double-check

#include <winsock2.h>  // <-- should bring 'windows.h' along with it
#include <Ws2tcpip.h>

#ifndef   BT_SOCKLEN_T
#  define BT_SOCKLEN_T int
#endif

#ifdef _MSC_VER
#  pragma comment(lib, "ws2_32.lib")
#endif

namespace BamTools {
namespace Internal {

// use RAII to ensure WSA is en
class WindowsSockInit {
    public:
        WindowsSockInit(void) {
            WSAData wsadata;
            WSAStartup(MAKEWORD(2,2), &wsadata); // catch error ?
        }

        ~WindowsSockInit(void) {
            WSACleanup();
        }
};

} // namespace Internal
} // namespace BamTools

#endif // _WIN32

#endif // NETWIN_P_H

