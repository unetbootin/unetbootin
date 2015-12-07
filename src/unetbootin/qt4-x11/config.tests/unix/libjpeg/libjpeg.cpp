#include <sys/types.h>
#include <stdio.h>
extern "C" {
#include <jpeglib.h>
}

int main(int, char **)
{
    j_compress_ptr cinfo;
    jpeg_create_compress(cinfo);
    return 0;
}
