#include <X11/Xlib.h>
#include <X11/Xcursor/Xcursor.h>

#if !defined(XCURSOR_LIB_MAJOR)
#  define XCURSOR_LIB_MAJOR XCURSOR_MAJOR
#endif
#if !defined(XCURSOR_LIB_MINOR)
#  define XCURSOR_LIB_MINOR XCURSOR_MINOR
#endif

#if XCURSOR_LIB_MAJOR == 1 && XCURSOR_LIB_MINOR >= 0
#  define XCURSOR_FOUND
#else
#  define
#  error "Required Xcursor version 1.0 not found."
#endif

int main(int, char **)
{
    XcursorImage *image;
    image = 0;
    XcursorCursors *cursors;
    cursors = 0;
    return 0;
}
