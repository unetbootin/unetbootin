/* Sample program for configure to test IPv6 support on target
platforms. We check for the required IPv6 data structures. */

#if defined(__hpux)
#define _HPUX_SOURCE
#endif

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main()
{
    sockaddr_in6 tmp;
    sockaddr_storage tmp2;
    (void)tmp.sin6_addr.s6_addr;
    (void)tmp.sin6_port;
    (void)tmp.sin6_family;
    (void)tmp.sin6_scope_id;
    (void)tmp2;

    return 0;
}
