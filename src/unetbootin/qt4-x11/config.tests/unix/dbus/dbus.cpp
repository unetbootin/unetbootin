#define DBUS_API_SUBJECT_TO_CHANGE
#include <dbus/dbus.h>

#if DBUS_MAJOR_PROTOCOL_VERSION < 1
#error Needs at least dbus version 1
#endif

int main(int, char **)
{
    dbus_shutdown();
    return 0;
}
