/* Sample program for configure to test for if_nametoindex support
on target platforms. */

#if defined(__hpux)
#define _HPUX_SOURCE
#endif

#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#include <ifaddrs.h>

int main()
{
    ifaddrs *list;
    getifaddrs(&list);
    freeifaddrs(list);
    return 0;
}
