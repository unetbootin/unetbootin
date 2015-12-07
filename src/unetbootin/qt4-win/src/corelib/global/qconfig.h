/* Everything */


/* License information */
#define QT_PRODUCT_LICENSEE "Open Source"
#define QT_PRODUCT_LICENSE "OpenSource"

// Qt Edition
#ifndef QT_EDITION
#  define QT_EDITION QT_EDITION_OPENSOURCE
#endif

#if (defined(_DEBUG) || defined(DEBUG))
# if (defined(WIN64) || defined(_WIN64) || defined(__WIN64__))
#  define QT_BUILD_KEY "Windows x64 mingw debug full-config"
# else
#  define QT_BUILD_KEY "Windows mingw debug full-config"
# endif
#else
# if (defined(WIN64) || defined(_WIN64) || defined(__WIN64__))
#  define QT_BUILD_KEY "Windows x64 mingw release full-config"
# else
#  define QT_BUILD_KEY "Windows mingw release full-config"
# endif
#endif

/* Machine byte-order */
#define Q_BIG_ENDIAN 4321
#define Q_LITTLE_ENDIAN 1234
#define Q_BYTE_ORDER Q_LITTLE_ENDIAN

// Compile time features
#define QT_ARCH_WINDOWS
#if defined(QT_BUILTIN_GIF_READER) && defined(QT_NO_BUILTIN_GIF_READER)
# undef QT_BUILTIN_GIF_READER
#elif !defined(QT_BUILTIN_GIF_READER)
# define QT_BUILTIN_GIF_READER 1
#endif

#if defined(QT_NO_DBUS) && defined(QT_DBUS)
# undef QT_NO_DBUS
#elif !defined(QT_NO_DBUS)
# define QT_NO_DBUS
#endif

#if defined(QT_NO_DIRECT3D) && defined(QT_DIRECT3D)
# undef QT_NO_DIRECT3D
#elif !defined(QT_NO_DIRECT3D)
# define QT_NO_DIRECT3D
#endif

#if defined(QT_NO_OPENSSL) && defined(QT_OPENSSL)
# undef QT_NO_OPENSSL
#elif !defined(QT_NO_OPENSSL)
# define QT_NO_OPENSSL
#endif

#if defined(QT_NO_PHONON) && defined(QT_PHONON)
# undef QT_NO_PHONON
#elif !defined(QT_NO_PHONON)
# define QT_NO_PHONON
#endif

#if defined(QT_NO_STYLE_WINDOWSCE) && defined(QT_STYLE_WINDOWSCE)
# undef QT_NO_STYLE_WINDOWSCE
#elif !defined(QT_NO_STYLE_WINDOWSCE)
# define QT_NO_STYLE_WINDOWSCE
#endif

#if defined(QT_NO_STYLE_WINDOWSMOBILE) && defined(QT_STYLE_WINDOWSMOBILE)
# undef QT_NO_STYLE_WINDOWSMOBILE
#elif !defined(QT_NO_STYLE_WINDOWSMOBILE)
# define QT_NO_STYLE_WINDOWSMOBILE
#endif

