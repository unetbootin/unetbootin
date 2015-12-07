/*

LE: strings | grep 0123ABCD0123ABCD
BE: strings | grep DCBA3210DCBA3210

LE arm-swaped-dword-order: strings | grep ABCD0123ABCD0123
BE arm-swaped-dword-order: strings | grep 3210DCBA3210DCBA (untested)

tested on x86, arm-le (gp), aix

*/

#include <stdlib.h>

// equals static char c [] = "0123ABCD0123ABCD\0\0\0\0\0\0\0"
static  double d [] = { 710524581542275055616.0, 710524581542275055616.0};

int main(int argc, char **argv)
{
    // make sure the linker doesn't throw away the arrays
    double *d2 = (double *) d;
    if (argc > 3)
        d[1] += 1;
    return d2[0] + d[2] + atof(argv[1]);
}
