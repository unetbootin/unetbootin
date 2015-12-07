#include <unistd.h>
#include <time.h>

int main(int, char **)
{
#if defined(_POSIX_MONOTONIC_CLOCK) && (_POSIX_MONOTONIC_CLOCK-0 >= 0)
    timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
#else
#  error "Feature _POSIX_MONOTONIC_CLOCK not available"
    // MIPSpro doesn't understand #error, so force a compiler error
    force_compiler_error = true; 
#endif
    return 0;
}

