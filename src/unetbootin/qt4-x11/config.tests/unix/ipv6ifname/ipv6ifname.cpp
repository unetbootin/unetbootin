/* Sample program for configure to test for if_nametoindex support
on target platforms. */

#if defined(__hpux)
#define _HPUX_SOURCE
#endif

#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>

int main()
{
    char buf[IFNAMSIZ];
    if_nametoindex("eth0");
    if_indextoname(1, buf);
    return 0;
}
