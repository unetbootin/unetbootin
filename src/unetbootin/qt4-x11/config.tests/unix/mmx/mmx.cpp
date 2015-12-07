#include <mmintrin.h>
#if defined(__GNUC__) && __GNUC__ < 4 && __GNUC_MINOR__ < 3
#error GCC < 3.2 is known to create internal compiler errors with our MMX code
#endif

int main(int, char**)
{
    _mm_empty();
    return 0;
}
