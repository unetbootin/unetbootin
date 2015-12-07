#include <X11/Xlib.h>
#include <X11/XKBlib.h>

int main(int, char **)
{
    Display *display = 0;

    int opcode = -1;
    int xkbEventBase = -1;
    int xkbErrorBase = -1;
    int xkblibMajor = XkbMajorVersion;
    int xkblibMinor = XkbMinorVersion;
    XkbQueryExtension(display, &opcode, &xkbEventBase, &xkbErrorBase, &xkblibMajor, &xkblibMinor);

    int keycode = 0;
    unsigned int state = 0;
    KeySym keySym;
    unsigned int consumedModifiers;
    XkbLookupKeySym(display, keycode, state, &consumedModifiers, &keySym);

    XkbDescPtr xkbDesc = XkbGetMap(display, XkbAllClientInfoMask, XkbUseCoreKbd);
    int w = XkbKeyGroupsWidth(xkbDesc, keycode);
    keySym = XkbKeySym(xkbDesc, keycode, w-1);
    XkbFreeClientMap(xkbDesc, XkbAllClientInfoMask, true);

    state = XkbPCF_GrabsUseXKBStateMask;
    (void) XkbSetPerClientControls(display, state, &state);

    return 0;
}
