#include <unistd.h>
#include <time.h>

int main(int, char **)
{
#if defined(_POSIX_TIMERS) && (_POSIX_TIMERS > 0)
    timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
#else
#  error "Feature _POSIX_TIMERS not available"
    // MIPSpro doesn't understand #error, so force a compiler error
    force_compiler_error = true;
#endif
    return 0;
}

