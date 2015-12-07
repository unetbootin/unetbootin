#include <sys/inotify.h>

int main()
{
    inotify_init();
    inotify_add_watch(0, "foobar", IN_ACCESS);
    inotify_rm_watch(0, 1);
    return 0;
}
