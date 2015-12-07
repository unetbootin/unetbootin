#include <mmintrin.h>

int main(int, char**)
{
    _mm_unpackhi_pi16(_mm_setzero_si64(), _mm_setzero_si64());
    return 0;
}
