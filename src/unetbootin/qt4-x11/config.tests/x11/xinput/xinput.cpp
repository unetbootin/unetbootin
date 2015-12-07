#ifdef Q_OS_SOLARIS
#error "Not supported."
#else

#include <X11/Xlib.h>
#include <X11/extensions/XInput.h>

#ifdef Q_OS_IRIX
#  include <wacom.h>
#endif

int main(int, char **)
{
    XDeviceButtonEvent *event;
    event = 0;
    return 0;
}
#endif
