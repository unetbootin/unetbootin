#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/shape.h>

int main(int, char **)
{
    XShapeEvent shapeevent;
    shapeevent.type = 0;
    return 0;    
}
