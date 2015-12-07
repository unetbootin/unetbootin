#include <X11/Xlib.h>
#include <X11/extensions/Xrender.h>

#if RENDER_MAJOR == 0 && RENDER_MINOR < 5
#  error "Required Xrender version 0.6 not found."
#else
int main(int, char **)
{
    XRenderPictFormat *format;
    format = 0;
    return 0;
}
#endif
