#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>

#if RANDR_MAJOR != 1 || RANDR_MINOR < 1
#  error "Requried Xrandr version 1.1 not found."
#endif

int main(int, char **)
{
    XRRScreenSize *size;
    size = 0;
    return 0;
}
