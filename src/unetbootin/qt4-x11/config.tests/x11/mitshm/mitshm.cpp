#ifdef Q_OS_HPUX
#error "MITSHM not supported on HP-UX."
#else
#include <X11/Xlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <X11/extensions/XShm.h>

int main(int, char **)
{
    Display *dpy = 0;
    int minor;
    int major;
    int pixmaps;
    if (dpy && XShmQueryVersion(dpy, &major, &minor, &pixmaps)) {
        minor = 0;
        major = 0;
        pixmaps = 0;
    }
    return 0;
}
#endif
