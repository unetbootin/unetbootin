#include <openssl/opensslv.h>

#if !defined(OPENSSL_VERSION_NUMBER) || OPENSSL_VERSION_NUMBER-0 < 0x0090700fL
#  error "OpenSSL >= 0.9.7 is required"
#endif

int main()
{
}
