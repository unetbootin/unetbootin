/* Sample program for configure to test for broken X11 headers that
confuse gcc 2.95 and better on target platforms such as Solaris.
*/

#include <X11/Xlib.h>
#include <X11/ICE/ICElib.h>

int main()
{
    return 0;
}
