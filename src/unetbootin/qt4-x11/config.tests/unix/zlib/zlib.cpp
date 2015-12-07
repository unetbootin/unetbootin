#include <zlib.h>

int main(int, char **)
{
    z_streamp stream;
    stream = 0;
    const char *ver = zlibVersion();
    ver = 0;
    // compress2 was added in zlib version 1.0.8
    int res = compress2(0, 0, 0, 0, 1);
    res = 0;
    return 0;
}
