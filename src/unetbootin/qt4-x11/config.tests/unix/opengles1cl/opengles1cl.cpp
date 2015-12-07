#include <GLES/gl.h>
#include <GLES/egl.h>

int main(int, char **)
{
    GLfixed a = 0;
    eglInitialize(0, 0, 0);
    glColor4x(a, a, a, a);
    glClear(GL_COLOR_BUFFER_BIT);

    return 0;
}
