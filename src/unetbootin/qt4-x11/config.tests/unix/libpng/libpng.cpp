#include <png.h>

#if !defined(PNG_LIBPNG_VER) || PNG_LIBPNG_VER < 10017
#  error "Required libpng version 1.0.17 not found."
#endif

int main(int, char **)
{
    png_structp png_ptr;
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,0,0,0);
    return 0;
}
