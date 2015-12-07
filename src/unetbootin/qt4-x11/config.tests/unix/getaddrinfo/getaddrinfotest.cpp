/* Sample program for configure to test for getaddrinfo on the unix
     platform. we check for all structures and functions required. */

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

int main()
{
    addrinfo *res = 0;
    if (getaddrinfo("foo", 0, 0, &res) == 0)
        freeaddrinfo(res);
    gai_strerror(0);
    
    return 0;
}
