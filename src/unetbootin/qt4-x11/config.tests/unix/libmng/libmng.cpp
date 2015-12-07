#include <libmng.h>

int main(int, char **)
{
    mng_handle hMNG;
    mng_cleanup(&hMNG);

#if MNG_VERSION_MAJOR < 1 || (MNG_VERSION_MAJOR == 1 && MNG_VERSION_MINOR == 0 && MNG_VERSION_RELEASE < 9)
#error System libmng version is less than 1.0.9; using built-in version instead.
#endif

    return 0;
}
