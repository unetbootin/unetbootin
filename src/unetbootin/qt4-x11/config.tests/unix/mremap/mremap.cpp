#include <unistd.h>
#include <sys/mman.h>

int main(int, char **)
{
    (void) ::mremap(static_cast<void *>(0), size_t(0), size_t(42), MREMAP_MAYMOVE);

    return 0;
}

