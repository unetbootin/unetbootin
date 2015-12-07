#include <cups/cups.h>

int main(int, char **)
{
    cups_dest_t *d;
    cupsGetDests(&d);
    return 0;
}
