#include "libpq-fe.h"

int main(int, char **)
{
    PQescapeBytea(0, 0, 0);
    PQunescapeBytea(0, 0);
    return 0;
}
