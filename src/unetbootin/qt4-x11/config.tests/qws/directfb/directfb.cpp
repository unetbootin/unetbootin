#include <directfb.h>

int main(int, char **)
{
    DFBResult result = DFB_OK;
    result = DirectFBInit(0, 0);

    return (result == DFB_OK);
}
