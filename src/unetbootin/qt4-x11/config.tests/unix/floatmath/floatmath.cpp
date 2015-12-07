#include <math.h>

int main(int argc, char **argv)
{
    float c = ceilf(1.3f);
    float f = floorf(1.7f);
    float s = sinf(3.8);
    float t = cosf(7.3);
    float u = sqrtf(8.4);
    float l = logf(9.2);

    if (c == 1.0f && f == 2.0f && s == 3.0f && t == 4.0f && u == 5.0f && l == 6.0f)
        return 0;
    else
        return 1;
}

