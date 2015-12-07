#include <X11/Xlib.h>
#include <X11/extensions/Xfixes.h>

#if XFIXES_MAJOR < 2
#  error "Required Xfixes version 2.0 not found."
#endif

int main(int, char **)
{
    XFixesSelectionNotifyEvent event;
    event.type = 0;
    return 0; 
}

