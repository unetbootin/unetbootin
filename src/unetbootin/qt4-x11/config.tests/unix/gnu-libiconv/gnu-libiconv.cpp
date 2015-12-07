#if defined(__sgi)
#error "iconv not supported on IRIX"
#else
#include <iconv.h>

int main(int, char **)
{
    iconv_t x = iconv_open("", "");

    const char *inp;
    char *outp;
    size_t inbytes, outbytes;
    iconv(x, &inp, &inbytes, &outp, &outbytes);

    iconv_close(x);

    return 0;
}
#endif
