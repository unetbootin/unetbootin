#include <ft2build.h>
#include FT_FREETYPE_H

#if ((FREETYPE_MAJOR*10000 + FREETYPE_MINOR*100 + FREETYPE_PATCH) < 20103)
#  error "This version of freetype is too old."
#endif

int main(int, char **)
{
    FT_Face face;
    face = 0;
    return 0;
}
