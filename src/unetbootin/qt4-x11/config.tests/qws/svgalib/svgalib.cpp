#include <vga.h>
#include <vgagl.h>

int main(int, char **)
{
    int mode = vga_getdefaultmode();
    gl_setcontextvga(mode);

    return 0;
}
