#include <emmintrin.h>
#if defined(__GNUC__) && __GNUC__ < 4 && __GNUC_MINOR__ < 3
#error GCC < 3.2 is known to create internal compiler errors with our MMX code
#endif

int main(int, char**)
{
    __m128i a = _mm_setzero_si128();
    _mm_maskmoveu_si128(a, _mm_setzero_si128(), 0);
    return 0;
}
