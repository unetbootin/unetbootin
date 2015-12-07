#include <GLES/gl.h>
#include <GLES/egl.h>

int main(int, char **)
{
    GLfloat a = 1.0f;
    eglInitialize(0, 0, 0);
    glColor4f(a, a, a, a);
    glClear(GL_COLOR_BUFFER_BIT);

    return 0;
}
