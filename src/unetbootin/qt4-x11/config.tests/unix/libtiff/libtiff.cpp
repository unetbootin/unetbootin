#include <tiffio.h>

#if !defined(TIFF_VERSION)
#    error "Required libtiff not found"
#elif TIFF_VERSION < 42
#    error "unsupported tiff version"
#endif

int main(int, char **)
{
    tdata_t buffer = _TIFFmalloc(128);
    _TIFFfree(buffer);

    // some libtiff implementations where TIFF_VERSION >= 42 do not
    // have TIFFReadRGBAImageOriented(), so let's check for it
    TIFFReadRGBAImageOriented(0, 0, 0, 0, 0, 0);

    return 0;
}
