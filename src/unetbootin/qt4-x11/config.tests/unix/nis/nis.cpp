#include <sys/types.h>
#include <rpc/rpc.h>
#include <rpcsvc/ypclnt.h>
#include <rpcsvc/yp_prot.h>

int main(int, char **)
{
    char *d;
    yp_get_default_domain(&d);
    return 0;
}
