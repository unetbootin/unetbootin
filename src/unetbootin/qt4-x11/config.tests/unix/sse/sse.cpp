#include <xmmintrin.h>
#if defined(__GNUC__) && __GNUC__ < 4 && __GNUC_MINOR__ < 3
#error GCC < 3.2 is known to create internal compiler errors with our MMX code
#endif

int main(int, char**)
{
    __m64 a = _mm_setzero_si64();
    a = _mm_shuffle_pi16(a, 0);
    return _m_to_int(a);
}
