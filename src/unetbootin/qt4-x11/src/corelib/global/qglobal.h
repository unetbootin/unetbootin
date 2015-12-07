/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QGLOBAL_H
#define QGLOBAL_H

#include <stddef.h>

#define QT_VERSION_STR "4.5.0"
/*
   QT_VERSION is (major << 16) + (minor << 8) + patch.
*/
#define QT_VERSION 0x040500
/*
   can be used like #if (QT_VERSION >= QT_VERSION_CHECK(4, 4, 0))
*/
#define QT_VERSION_CHECK(major, minor, patch) ((major<<16)|(minor<<8)|(patch))

#define QT_PACKAGEDATE_STR "2009-02-25"

#define QT_PACKAGE_TAG "g05dab8490c8fe4c603c85122133b79672da85ce3"

#if !defined(QT_BUILD_MOC)
#include <QtCore/qconfig.h>
#endif

#ifdef __cplusplus

#ifndef QT_NAMESPACE /* user namespace */

# define QT_PREPEND_NAMESPACE(name) ::name
# define QT_USE_NAMESPACE
# define QT_BEGIN_NAMESPACE
# define QT_END_NAMESPACE
# define QT_BEGIN_INCLUDE_NAMESPACE
# define QT_END_INCLUDE_NAMESPACE
# define QT_BEGIN_MOC_NAMESPACE
# define QT_END_MOC_NAMESPACE
# define QT_FORWARD_DECLARE_CLASS(name) class name;
# define QT_FORWARD_DECLARE_STRUCT(name) struct name;
# define QT_MANGLE_NAMESPACE(name) name

#else /* user namespace */

# define QT_PREPEND_NAMESPACE(name) ::QT_NAMESPACE::name
# define QT_USE_NAMESPACE using namespace ::QT_NAMESPACE;
# define QT_BEGIN_NAMESPACE namespace QT_NAMESPACE {
# define QT_END_NAMESPACE }
# define QT_BEGIN_INCLUDE_NAMESPACE }
# define QT_END_INCLUDE_NAMESPACE namespace QT_NAMESPACE {
# define QT_BEGIN_MOC_NAMESPACE QT_USE_NAMESPACE
# define QT_END_MOC_NAMESPACE
# define QT_FORWARD_DECLARE_CLASS(name) \
    QT_BEGIN_NAMESPACE class name; QT_END_NAMESPACE \
    using QT_PREPEND_NAMESPACE(name);

# define QT_FORWARD_DECLARE_STRUCT(name) \
    QT_BEGIN_NAMESPACE struct name; QT_END_NAMESPACE \
    using QT_PREPEND_NAMESPACE(name);

# define QT_MANGLE_NAMESPACE0(x) x
# define QT_MANGLE_NAMESPACE1(a, b) a##_##b
# define QT_MANGLE_NAMESPACE2(a, b) QT_MANGLE_NAMESPACE1(a,b)
# define QT_MANGLE_NAMESPACE(name) QT_MANGLE_NAMESPACE2( \
        QT_MANGLE_NAMESPACE0(name), QT_MANGLE_NAMESPACE0(QT_NAMESPACE))

namespace QT_NAMESPACE {}

# ifndef QT_BOOTSTRAPPED
# ifndef QT_NO_USING_NAMESPACE
   /*
    This expands to a "using QT_NAMESPACE" also in _header files_.
    It is the only way the feature can be used without too much
    pain, but if people _really_ do not want it they can add
    DEFINES += QT_NO_USING_NAMESPACE to theur .pro files.
    */
   QT_USE_NAMESPACE
# endif
# endif

#endif /* user namespace */

#else /* __cplusplus */

# define QT_BEGIN_NAMESPACE
# define QT_END_NAMESPACE
# define QT_USE_NAMESPACE
# define QT_BEGIN_INCLUDE_NAMESPACE
# define QT_END_INCLUDE_NAMESPACE

#endif /* __cplusplus */

#if defined(Q_OS_MAC) && !defined(Q_CC_INTEL)
#define QT_BEGIN_HEADER extern "C++" {
#define QT_END_HEADER }
#define QT_BEGIN_INCLUDE_HEADER }
#define QT_END_INCLUDE_HEADER extern "C++" {
#else
#define QT_BEGIN_HEADER
#define QT_END_HEADER
#define QT_BEGIN_INCLUDE_HEADER
#define QT_END_INCLUDE_HEADER extern "C++"
#endif

/*
   The operating system, must be one of: (Q_OS_x)

     DARWIN   - Darwin OS (synonym for Q_OS_MAC)
     MSDOS    - MS-DOS and Windows
     OS2      - OS/2
     OS2EMX   - XFree86 on OS/2 (not PM)
     WIN32    - Win32 (Windows 95/98/ME and Windows NT/2000/XP)
     WINCE    - WinCE (Windows CE 5.0)
     CYGWIN   - Cygwin
     SOLARIS  - Sun Solaris
     HPUX     - HP-UX
     ULTRIX   - DEC Ultrix
     LINUX    - Linux
     FREEBSD  - FreeBSD
     NETBSD   - NetBSD
     OPENBSD  - OpenBSD
     BSDI     - BSD/OS
     IRIX     - SGI Irix
     OSF      - HP Tru64 UNIX
     SCO      - SCO OpenServer 5
     UNIXWARE - UnixWare 7, Open UNIX 8
     AIX      - AIX
     HURD     - GNU Hurd
     DGUX     - DG/UX
     RELIANT  - Reliant UNIX
     DYNIX    - DYNIX/ptx
     QNX      - QNX
     QNX6     - QNX RTP 6.1
     LYNX     - LynxOS
     BSD4     - Any BSD 4.4 system
     UNIX     - Any UNIX BSD/SYSV system
*/

#if defined(__APPLE__) && (defined(__GNUC__) || defined(__xlC__) || defined(__xlc__))
#  define Q_OS_DARWIN
#  define Q_OS_BSD4
#  ifdef __LP64__
#    define Q_OS_DARWIN64
#  else
#    define Q_OS_DARWIN32
#  endif
#elif defined(__CYGWIN__)
#  define Q_OS_CYGWIN
#elif defined(MSDOS) || defined(_MSDOS)
#  define Q_OS_MSDOS
#elif defined(__OS2__)
#  if defined(__EMX__)
#    define Q_OS_OS2EMX
#  else
#    define Q_OS_OS2
#  endif
#elif !defined(SAG_COM) && (defined(WIN64) || defined(_WIN64) || defined(__WIN64__))
#  define Q_OS_WIN32
#  define Q_OS_WIN64
#elif !defined(SAG_COM) && (defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__))
#  if defined(WINCE) || defined(_WIN32_WCE)
#    define Q_OS_WINCE
#  else
#    define Q_OS_WIN32
#  endif
#elif defined(__MWERKS__) && defined(__INTEL__)
#  define Q_OS_WIN32
#elif defined(__sun) || defined(sun)
#  define Q_OS_SOLARIS
#elif defined(hpux) || defined(__hpux)
#  define Q_OS_HPUX
#elif defined(__ultrix) || defined(ultrix)
#  define Q_OS_ULTRIX
#elif defined(sinix)
#  define Q_OS_RELIANT
#elif defined(__linux__) || defined(__linux)
#  define Q_OS_LINUX
#elif defined(__FreeBSD__) || defined(__DragonFly__)
#  define Q_OS_FREEBSD
#  define Q_OS_BSD4
#elif defined(__NetBSD__)
#  define Q_OS_NETBSD
#  define Q_OS_BSD4
#elif defined(__OpenBSD__)
#  define Q_OS_OPENBSD
#  define Q_OS_BSD4
#elif defined(__bsdi__)
#  define Q_OS_BSDI
#  define Q_OS_BSD4
#elif defined(__sgi)
#  define Q_OS_IRIX
#elif defined(__osf__)
#  define Q_OS_OSF
#elif defined(_AIX)
#  define Q_OS_AIX
#elif defined(__Lynx__)
#  define Q_OS_LYNX
#elif defined(__GNU__)
#  define Q_OS_HURD
#elif defined(__DGUX__)
#  define Q_OS_DGUX
#elif defined(__QNXNTO__)
#  define Q_OS_QNX6
#elif defined(__QNX__)
#  define Q_OS_QNX
#elif defined(_SEQUENT_)
#  define Q_OS_DYNIX
#elif defined(_SCO_DS) /* SCO OpenServer 5 + GCC */
#  define Q_OS_SCO
#elif defined(__USLC__) /* all SCO platforms + UDK or OUDK */
#  define Q_OS_UNIXWARE
#elif defined(__svr4__) && defined(i386) /* Open UNIX 8 + GCC */
#  define Q_OS_UNIXWARE
#elif defined(__INTEGRITY)
#  define Q_OS_INTEGRITY
#elif defined(__MAKEDEPEND__)
#else
#  error "Qt has not been ported to this OS - talk to qt-bugs@trolltech.com"
#endif

#if defined(Q_OS_WIN32) || defined(Q_OS_WIN64) || defined(Q_OS_WINCE)
#  define Q_OS_WIN
#endif

#if defined(Q_OS_DARWIN)
#  define Q_OS_MAC /* Q_OS_MAC is mostly for compatibility, but also more clear */
#  define Q_OS_MACX /* Q_OS_MACX is only for compatibility.*/
#  if defined(Q_OS_DARWIN64)
#     define Q_OS_MAC64
#  elif defined(Q_OS_DARWIN32)
#     define Q_OS_MAC32
#  endif
#endif

#ifdef AUTODETECT_COCOA
#  ifdef Q_OS_MAC64
#    define QT_MAC_USE_COCOA 1
#  endif
#endif

#if defined(Q_OS_MSDOS) || defined(Q_OS_OS2) || defined(Q_OS_WIN)
#  undef Q_OS_UNIX
#elif !defined(Q_OS_UNIX)
#  define Q_OS_UNIX
#endif

#if defined(Q_OS_DARWIN) && !defined(QT_LARGEFILE_SUPPORT)
#  define QT_LARGEFILE_SUPPORT 64
#endif

#ifdef Q_OS_DARWIN
#  ifdef MAC_OS_X_VERSION_MIN_REQUIRED
#    undef MAC_OS_X_VERSION_MIN_REQUIRED
#  endif
#  define MAC_OS_X_VERSION_MIN_REQUIRED MAC_OS_X_VERSION_10_3
#  include <AvailabilityMacros.h>
#  if !defined(MAC_OS_X_VERSION_10_3)
#     define MAC_OS_X_VERSION_10_3 MAC_OS_X_VERSION_10_2 + 1
#  endif
#  if !defined(MAC_OS_X_VERSION_10_4)
#       define MAC_OS_X_VERSION_10_4 MAC_OS_X_VERSION_10_3 + 1
#  endif
#  if !defined(MAC_OS_X_VERSION_10_5)
#       define MAC_OS_X_VERSION_10_5 MAC_OS_X_VERSION_10_4 + 1
#  endif
#  if !defined(MAC_OS_X_VERSION_10_6)
#       define MAC_OS_X_VERSION_10_6 MAC_OS_X_VERSION_10_5 + 1
#  endif
#  if (MAC_OS_X_VERSION_MAX_ALLOWED == MAC_OS_X_VERSION_10_6)
#    warning "Support for this version of Mac OS X is still preliminary"
#  endif
#  if (MAC_OS_X_VERSION_MAX_ALLOWED > MAC_OS_X_VERSION_10_6)
#    error "This version of Mac OS X is unsupported"
#  endif
#endif

#ifdef QT_MAC_USE_COCOA
#define QT_MAC_NO_QUICKDRAW 1
#endif

#ifdef __LSB_VERSION__
#  if __LSB_VERSION__ < 40
#    error "This version of the Linux Standard Base is unsupported"
#  endif
#ifndef QT_LINUXBASE
#  define QT_LINUXBASE
#endif
#endif

/*
   The compiler, must be one of: (Q_CC_x)

     SYM      - Digital Mars C/C++ (used to be Symantec C++)
     MWERKS   - Metrowerks CodeWarrior
     MSVC     - Microsoft Visual C/C++, Intel C++ for Windows
     BOR      - Borland/Turbo C++
     WAT      - Watcom C++
     GNU      - GNU C++
     COMEAU   - Comeau C++
     EDG      - Edison Design Group C++
     OC       - CenterLine C++
     SUN      - Forte Developer, or Sun Studio C++
     MIPS     - MIPSpro C++
     DEC      - DEC C++
     HPACC    - HP aC++
     USLC     - SCO OUDK and UDK
     CDS      - Reliant C++
     KAI      - KAI C++
     INTEL    - Intel C++ for Linux, Intel C++ for Windows
     HIGHC    - MetaWare High C/C++
     PGI      - Portland Group C++
     GHS      - Green Hills Optimizing C++ Compilers
     RVCT     - ARM Realview Compiler Suite


   Should be sorted most to least authoritative.
*/

#if defined(__ghs)
#  define Q_OUTOFLINE_TEMPLATE inline
#endif

/* Symantec C++ is now Digital Mars */
#if defined(__DMC__) || defined(__SC__)
#  define Q_CC_SYM
/* "explicit" semantics implemented in 8.1e but keyword recognized since 7.5 */
#  if defined(__SC__) && __SC__ < 0x750
#    define Q_NO_EXPLICIT_KEYWORD
#  endif
#  define Q_NO_USING_KEYWORD

#elif defined(__MWERKS__)
#  define Q_CC_MWERKS
/* "explicit" recognized since 4.0d1 */

#elif defined(_MSC_VER)
#  define Q_CC_MSVC
/* proper support of bool for _MSC_VER >= 1100 */
#  define Q_CANNOT_DELETE_CONSTANT
#  define Q_OUTOFLINE_TEMPLATE inline
#  define Q_NO_TEMPLATE_FRIENDS
#  define QT_NO_PARTIAL_TEMPLATE_SPECIALIZATION
/* Visual C++.Net issues for _MSC_VER >= 1300 */
#  if _MSC_VER >= 1300
#    define Q_CC_MSVC_NET
#    if _MSC_VER < 1310 || (defined(Q_OS_WIN64) && defined(_M_IA64))
#      define Q_TYPENAME
#    else
#      undef QT_NO_PARTIAL_TEMPLATE_SPECIALIZATION
#    endif
#  else
#    define Q_NO_USING_KEYWORD
#    define QT_NO_MEMBER_TEMPLATES
#  endif
#  if _MSC_VER < 1310
#     define QT_NO_QOBJECT_CHECK
#     define Q_TYPENAME
#     define QT_NO_TEMPLATE_TEMPLATE_PARAMETERS
#  endif
/* Intel C++ disguising as Visual C++: the `using' keyword avoids warnings */
#  if defined(__INTEL_COMPILER)
#    define Q_CC_INTEL
#  endif
/* x64 does not support mmx intrinsics on windows */
#  if (defined(Q_OS_WIN64) && defined(_M_X64))
#    undef QT_HAVE_SSE
#    undef QT_HAVE_SSE2
#    undef QT_HAVE_MMX
#    undef QT_HAVE_3DNOW
#  endif


#elif defined(__BORLANDC__) || defined(__TURBOC__)
#  define Q_CC_BOR
#  define Q_INLINE_TEMPLATE
#  if __BORLANDC__ < 0x502
#    define Q_NO_BOOL_TYPE
#    define Q_NO_EXPLICIT_KEYWORD
#  endif
#  define Q_NO_USING_KEYWORD

#elif defined(__WATCOMC__)
#  define Q_CC_WAT
#  if defined(Q_OS_QNX4)
/* compiler flags */
#    define Q_TYPENAME
#    define Q_NO_BOOL_TYPE
#    define Q_CANNOT_DELETE_CONSTANT
#    define mutable
/* ??? */
#    define Q_BROKEN_TEMPLATE_SPECIALIZATION
/* no template classes in QVariant */
#    define QT_NO_TEMPLATE_VARIANT
/* Wcc does not fill in functions needed by valuelists, maps, and
   valuestacks implicitly */
#    define Q_FULL_TEMPLATE_INSTANTIATION
/* can we just compare the structures? */
#    define Q_FULL_TEMPLATE_INSTANTIATION_MEMCMP
/* these are not useful to our customers */
#    define QT_NO_QWS_MULTIPROCESS
#    define QT_NO_QWS_CURSOR
#  endif

#elif defined(__CC_ARM)
#  define Q_CC_RVCT

#elif defined(__GNUC__)
#  define Q_CC_GNU
#  define Q_C_CALLBACKS
#  if defined(__MINGW32__)
#    define Q_CC_MINGW
#  endif
#  if defined(__INTEL_COMPILER)
/* Intel C++ also masquerades as GCC 3.2.0 */
#    define Q_CC_INTEL
#    define Q_NO_TEMPLATE_FRIENDS
#  endif
#  ifdef __APPLE__
#    define Q_NO_DEPRECATED_CONSTRUCTORS
#  endif
#  if __GNUC__ == 2 && __GNUC_MINOR__ <= 7
#    define Q_FULL_TEMPLATE_INSTANTIATION
#  endif
/* GCC 2.95 knows "using" but does not support it correctly */
#  if __GNUC__ == 2 && __GNUC_MINOR__ <= 95
#    define Q_NO_USING_KEYWORD
#    define QT_NO_STL_WCHAR
#  endif
/* GCC 3.1 and GCC 3.2 wrongly define _SB_CTYPE_MACROS on HP-UX */
#  if defined(Q_OS_HPUX) && __GNUC__ == 3 && __GNUC_MINOR__ >= 1
#    define Q_WRONG_SB_CTYPE_MACROS
#  endif
/* GCC <= 3.3 cannot handle template friends */
#  if __GNUC__ < 3 || (__GNUC__ == 3 && __GNUC_MINOR__ <= 3)
#    define Q_NO_TEMPLATE_FRIENDS
#  endif
/* Apple's GCC 3.1 chokes on our streaming qDebug() */
#  if defined(Q_OS_DARWIN) && __GNUC__ == 3 && (__GNUC_MINOR__ >= 1 && __GNUC_MINOR__ < 3)
#    define Q_BROKEN_DEBUG_STREAM
#  endif
#  if (defined(Q_CC_GNU) || defined(Q_CC_INTEL)) && !defined(QT_MOC_CPP)
#    define Q_PACKED __attribute__ ((__packed__))
#    define Q_NO_PACKED_REFERENCE
#    ifndef __ARM_EABI__
#      define QT_NO_ARM_EABI
#    endif
#  endif

/* IBM compiler versions are a bit messy. There are actually two products:
   the C product, and the C++ product. The C++ compiler is always packaged
   with the latest version of the C compiler. Version numbers do not always
   match. This little table (I'm not sure it's accurate) should be helpful:

   C++ product                C product

   C Set 3.1                  C Compiler 3.0
   ...                        ...
   C++ Compiler 3.6.6         C Compiler 4.3
   ...                        ...
   Visual Age C++ 4.0         ...
   ...                        ...
   Visual Age C++ 5.0         C Compiler 5.0
   ...                        ...
   Visual Age C++ 6.0         C Compiler 6.0

   Now:
   __xlC__    is the version of the C compiler in hexadecimal notation
              is only an approximation of the C++ compiler version
   __IBMCPP__ is the version of the C++ compiler in decimal notation
              but it is not defined on older compilers like C Set 3.1 */
#elif defined(__xlC__)
#  define Q_CC_XLC
#  define Q_FULL_TEMPLATE_INSTANTIATION
#  if __xlC__ < 0x400
#    define Q_NO_BOOL_TYPE
#    define Q_NO_EXPLICIT_KEYWORD
#    define Q_NO_USING_KEYWORD
#    define Q_TYPENAME
#    define Q_OUTOFLINE_TEMPLATE inline
#    define Q_BROKEN_TEMPLATE_SPECIALIZATION
#    define Q_CANNOT_DELETE_CONSTANT
#  endif

/* Older versions of DEC C++ do not define __EDG__ or __EDG - observed
   on DEC C++ V5.5-004. New versions do define  __EDG__ - observed on
   Compaq C++ V6.3-002.
   This compiler is different enough from other EDG compilers to handle
   it separately anyway. */
#elif defined(__DECCXX) || defined(__DECC)
#  define Q_CC_DEC
/* Compaq C++ V6 compilers are EDG-based but I'm not sure about older
   DEC C++ V5 compilers. */
#  if defined(__EDG__)
#    define Q_CC_EDG
#  endif
/* Compaq have disabled EDG's _BOOL macro and use _BOOL_EXISTS instead
   - observed on Compaq C++ V6.3-002.
   In any case versions prior to Compaq C++ V6.0-005 do not have bool. */
#  if !defined(_BOOL_EXISTS)
#    define Q_NO_BOOL_TYPE
#  endif
/* Spurious (?) error messages observed on Compaq C++ V6.5-014. */
#  define Q_NO_USING_KEYWORD
/* Apply to all versions prior to Compaq C++ V6.0-000 - observed on
   DEC C++ V5.5-004. */
#  if __DECCXX_VER < 60060000
#    define Q_TYPENAME
#    define Q_BROKEN_TEMPLATE_SPECIALIZATION
#    define Q_CANNOT_DELETE_CONSTANT
#  endif
/* avoid undefined symbol problems with out-of-line template members */
#  define Q_OUTOFLINE_TEMPLATE inline

/* The Portland Group C++ compiler is based on EDG and does define __EDG__
   but the C compiler does not */
#elif defined(__PGI)
#  define Q_CC_PGI
#  if defined(__EDG__)
#    define Q_CC_EDG
#  endif

/* Compilers with EDG front end are similar. To detect them we test:
   __EDG documented by SGI, observed on MIPSpro 7.3.1.1 and KAI C++ 4.0b
   __EDG__ documented in EDG online docs, observed on Compaq C++ V6.3-002
   and PGI C++ 5.2-4 */
#elif !defined(Q_OS_HPUX) && (defined(__EDG) || defined(__EDG__))
#  define Q_CC_EDG
/* From the EDG documentation (does not seem to apply to Compaq C++):
   _BOOL
        Defined in C++ mode when bool is a keyword. The name of this
        predefined macro is specified by a configuration flag. _BOOL
        is the default.
   __BOOL_DEFINED
        Defined in Microsoft C++ mode when bool is a keyword. */
#  if !defined(_BOOL) && !defined(__BOOL_DEFINED)
#    define Q_NO_BOOL_TYPE
#  endif

/* The Comeau compiler is based on EDG and does define __EDG__ */
#  if defined(__COMO__)
#    define Q_CC_COMEAU
#    define Q_C_CALLBACKS

/* The `using' keyword was introduced to avoid KAI C++ warnings
   but it's now causing KAI C++ errors instead. The standard is
   unclear about the use of this keyword, and in practice every
   compiler is using its own set of rules. Forget it. */
#  elif defined(__KCC)
#    define Q_CC_KAI
#    define Q_NO_USING_KEYWORD

/* Using the `using' keyword avoids Intel C++ for Linux warnings */
#  elif defined(__INTEL_COMPILER)
#    define Q_CC_INTEL

/* Uses CFront, make sure to read the manual how to tweak templates. */
#  elif defined(__ghs)
#    define Q_CC_GHS

/* The UnixWare 7 UDK compiler is based on EDG and does define __EDG__ */
#  elif defined(__USLC__) && defined(__SCO_VERSION__)
#    define Q_CC_USLC
/* The latest UDK 7.1.1b does not need this, but previous versions do */
#    if !defined(__SCO_VERSION__) || (__SCO_VERSION__ < 302200010)
#      define Q_OUTOFLINE_TEMPLATE inline
#    endif
#    define Q_NO_USING_KEYWORD /* ### check "using" status */

/* Never tested! */
#  elif defined(CENTERLINE_CLPP) || defined(OBJECTCENTER)
#    define Q_CC_OC
#    define Q_NO_USING_KEYWORD

/* CDS++ defines __EDG__ although this is not documented in the Reliant
   documentation. It also follows conventions like _BOOL and this documented */
#  elif defined(sinix)
#    define Q_CC_CDS
#    define Q_NO_USING_KEYWORD

/* The MIPSpro compiler defines __EDG */
#  elif defined(__sgi)
#    define Q_CC_MIPS
#    define Q_NO_USING_KEYWORD /* ### check "using" status */
#    define Q_NO_TEMPLATE_FRIENDS
#    if defined(_COMPILER_VERSION) && (_COMPILER_VERSION >= 740)
#      define Q_OUTOFLINE_TEMPLATE inline
#      pragma set woff 3624,3625,3649 /* turn off some harmless warnings */
#    endif
#  endif

/* Never tested! */
#elif defined(__HIGHC__)
#  define Q_CC_HIGHC

#elif defined(__SUNPRO_CC) || defined(__SUNPRO_C)
#  define Q_CC_SUN
/* 5.0 compiler or better
    'bool' is enabled by default but can be disabled using -features=nobool
    in which case _BOOL is not defined
        this is the default in 4.2 compatibility mode triggered by -compat=4 */
#  if __SUNPRO_CC >= 0x500
#    if __SUNPRO_CC < 0x570
#      define QT_NO_TEMPLATE_TEMPLATE_PARAMETERS
#    endif
#    if !defined(_BOOL)
#      define Q_NO_BOOL_TYPE
#    endif
#    if defined(__SUNPRO_CC_COMPAT) && (__SUNPRO_CC_COMPAT <= 4)
#      define Q_NO_USING_KEYWORD
#    endif
#    define Q_C_CALLBACKS
/* 4.2 compiler or older */
#  else
#    define Q_NO_BOOL_TYPE
#    define Q_NO_EXPLICIT_KEYWORD
#    define Q_NO_USING_KEYWORD
#  endif

/* CDS++ does not seem to define __EDG__ or __EDG according to Reliant
   documentation but nevertheless uses EDG conventions like _BOOL */
#elif defined(sinix)
#  define Q_CC_EDG
#  define Q_CC_CDS
#  if !defined(_BOOL)
#    define Q_NO_BOOL_TYPE
#  endif
#  define Q_BROKEN_TEMPLATE_SPECIALIZATION

#elif defined(Q_OS_HPUX)
/* __HP_aCC was not defined in first aCC releases */
#  if defined(__HP_aCC) || __cplusplus >= 199707L
#    define Q_NO_TEMPLATE_FRIENDS
#    define Q_CC_HPACC
#    ifdef QT_ARCH_PARISC
#      define QT_NO_TEMPLATE_TEMPLATE_PARAMETERS
#    endif
#  else
#    define Q_CC_HP
#    define Q_NO_BOOL_TYPE
#    define Q_FULL_TEMPLATE_INSTANTIATION
#    define Q_BROKEN_TEMPLATE_SPECIALIZATION
#    define Q_NO_EXPLICIT_KEYWORD
#  endif
#  define Q_NO_USING_KEYWORD /* ### check "using" status */

#else
#  error "Qt has not been tested with this compiler - talk to qt-bugs@trolltech.com"
#endif

#ifndef Q_PACKED
#  define Q_PACKED
#  undef Q_NO_PACKED_REFERENCE
#endif

#ifndef Q_CONSTRUCTOR_FUNCTION
# define Q_CONSTRUCTOR_FUNCTION0(AFUNC) \
   static const int AFUNC ## __init_variable__ = AFUNC();
# define Q_CONSTRUCTOR_FUNCTION(AFUNC) Q_CONSTRUCTOR_FUNCTION0(AFUNC)
#endif

#ifndef Q_DESTRUCTOR_FUNCTION
# define Q_DESTRUCTOR_FUNCTION0(AFUNC) \
    class AFUNC ## __dest_class__ { \
    public: \
       inline AFUNC ## __dest_class__() { } \
       inline ~ AFUNC ## __dest_class__() { AFUNC(); } \
    } AFUNC ## __dest_instance__;
# define Q_DESTRUCTOR_FUNCTION(AFUNC) Q_DESTRUCTOR_FUNCTION0(AFUNC)
#endif

#ifndef Q_REQUIRED_RESULT
#  if defined(Q_CC_GNU) && !defined(Q_CC_INTEL) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 1))
#    define Q_REQUIRED_RESULT __attribute__ ((warn_unused_result))
#  else
#    define Q_REQUIRED_RESULT
#  endif
#endif

#ifndef Q_COMPILER_MANGLES_RETURN_TYPE
#  if defined(Q_CC_MSVC)
#    define Q_COMPILER_MANGLES_RETURN_TYPE
#  endif
#endif

/*
   The window system, must be one of: (Q_WS_x)

     MACX     - Mac OS X
     MAC9     - Mac OS 9
     QWS      - Qt for Embedded Linux
     WIN32    - Windows
     X11      - X Window System
     PM       - unsupported
     WIN16    - unsupported
*/

#if defined(Q_OS_MSDOS)
#  define Q_WS_WIN16
#  error "Qt requires Win32 and does not work with Windows 3.x"
#elif defined(_WIN32_X11_)
#  define Q_WS_X11
#elif defined(Q_OS_WIN32)
#  define Q_WS_WIN32
#  if defined(Q_OS_WIN64)
#    define Q_WS_WIN64
#  endif
#elif defined(Q_OS_WINCE)
#  define Q_WS_WIN32
#elif defined(Q_OS_OS2)
#  define Q_WS_PM
#  error "Qt does not work with OS/2 Presentation Manager or Workplace Shell"
#elif defined(Q_OS_UNIX)
#  if defined(Q_OS_MAC) && !defined(__USE_WS_X11__) && !defined(Q_WS_QWS)
#    define Q_WS_MAC
#    define Q_WS_MACX
#    if defined(Q_OS_MAC64)
#      define Q_WS_MAC64
#    elif defined(Q_OS_MAC32)
#      define Q_WS_MAC32
#    endif
#  elif !defined(Q_WS_QWS)
#    define Q_WS_X11
#  endif
#endif

#if defined(Q_WS_WIN16) || defined(Q_WS_WIN32)
#  define Q_WS_WIN
#endif

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

/*
   Size-dependent types (architechture-dependent byte order)

   Make sure to update QMetaType when changing these typedefs
*/

typedef signed char qint8;         /* 8 bit signed */
typedef unsigned char quint8;      /* 8 bit unsigned */
typedef short qint16;              /* 16 bit signed */
typedef unsigned short quint16;    /* 16 bit unsigned */
typedef int qint32;                /* 32 bit signed */
typedef unsigned int quint32;      /* 32 bit unsigned */
#if defined(Q_OS_WIN) && !defined(Q_CC_GNU)
#  define Q_INT64_C(c) c ## i64    /* signed 64 bit constant */
#  define Q_UINT64_C(c) c ## ui64   /* unsigned 64 bit constant */
typedef __int64 qint64;            /* 64 bit signed */
typedef unsigned __int64 quint64;  /* 64 bit unsigned */
#else
#  define Q_INT64_C(c) static_cast<long long>(c ## LL)     /* signed 64 bit constant */
#  define Q_UINT64_C(c) static_cast<unsigned long long>(c ## ULL) /* unsigned 64 bit constant */
typedef long long qint64;           /* 64 bit signed */
typedef unsigned long long quint64; /* 64 bit unsigned */
#endif

typedef qint64 qlonglong;
typedef quint64 qulonglong;

#ifndef QT_POINTER_SIZE
#  if defined(Q_OS_WIN64)
#   define QT_POINTER_SIZE 8
#  elif defined(Q_OS_WIN32) || defined(Q_OS_WINCE)
#   define QT_POINTER_SIZE 4
#  endif
#endif

#define Q_INIT_RESOURCE(name) \
    do { extern int QT_MANGLE_NAMESPACE(qInitResources_ ## name) ();       \
        QT_MANGLE_NAMESPACE(qInitResources_ ## name) (); } while (0)
#define Q_CLEANUP_RESOURCE(name) \
    do { extern int QT_MANGLE_NAMESPACE(qCleanupResources_ ## name) ();    \
        QT_MANGLE_NAMESPACE(qCleanupResources_ ## name) (); } while (0)

#if defined(__cplusplus)

/*
  quintptr and qptrdiff is guaranteed to be the same size as a pointer, i.e.

      sizeof(void *) == sizeof(quintptr)
      && sizeof(void *) == sizeof(qptrdiff)
*/
template <int> class QUintForSize    { private: typedef void    Type; };
template <>    class QUintForSize<4> { public:  typedef quint32 Type; };
template <>    class QUintForSize<8> { public:  typedef quint64 Type; };
template <typename T> class QUintForType : public QUintForSize<sizeof(T)> { };
typedef QUintForType<void *>::Type quintptr;

template <int> class QIntForSize    { private: typedef void   Type; };
template <>    class QIntForSize<4> { public:  typedef qint32 Type; };
template <>    class QIntForSize<8> { public:  typedef qint64 Type; };
template <typename T> class QIntForType : public QIntForSize<sizeof(T)> { };
typedef QIntForType<void *>::Type qptrdiff;

/*
   Useful type definitions for Qt
*/

QT_BEGIN_INCLUDE_NAMESPACE
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;
QT_END_INCLUDE_NAMESPACE

#if defined(Q_NO_BOOL_TYPE)
#error "Compiler doesn't support the bool type"
#endif

/*
   Constant bool values
*/

#ifndef QT_LINUXBASE /* the LSB defines TRUE and FALSE for us */
#  ifndef TRUE
#   define TRUE true
#   define FALSE false
#  endif
#endif

/*
   Proper for-scoping in VC++6 and MIPSpro CC
*/
#ifndef QT_NO_KEYWORDS
#  if (defined(Q_CC_MSVC) && !defined(Q_CC_MSVC_NET) && !defined(Q_CC_INTEL)) || defined(Q_CC_MIPS) || (defined(Q_CC_HPACC) && defined(__ia64))
#    define for if(0){}else for
#  endif
#endif

/*
   Workaround for static const members on MSVC++.
*/

#if defined(Q_CC_MSVC)
#  define QT_STATIC_CONST static
#  define QT_STATIC_CONST_IMPL
#else
#  define QT_STATIC_CONST static const
#  define QT_STATIC_CONST_IMPL const
#endif

/*
   Warnings and errors when using deprecated methods
*/
#if defined(Q_MOC_RUN)
#  define Q_DECL_DEPRECATED Q_DECL_DEPRECATED
#elif (defined(Q_CC_GNU) && !defined(Q_CC_INTEL) && (__GNUC__ - 0 > 3 || (__GNUC__ - 0 == 3 && __GNUC_MINOR__ - 0 >= 2))) || defined(Q_CC_RVCT)
#  define Q_DECL_DEPRECATED __attribute__ ((__deprecated__))
#elif defined(Q_CC_MSVC) && (_MSC_VER >= 1300)
#  define Q_DECL_DEPRECATED __declspec(deprecated)
#  if defined (Q_CC_INTEL)
#    define Q_DECL_VARIABLE_DEPRECATED
#  else
#  endif
#else
#  define Q_DECL_DEPRECATED
#endif
#ifndef Q_DECL_VARIABLE_DEPRECATED
#  define Q_DECL_VARIABLE_DEPRECATED Q_DECL_DEPRECATED
#endif
#ifndef Q_DECL_CONSTRUCTOR_DEPRECATED
#  if defined(Q_MOC_RUN)
#    define Q_DECL_CONSTRUCTOR_DEPRECATED Q_DECL_CONSTRUCTOR_DEPRECATED
#  elif defined(Q_NO_DEPRECATED_CONSTRUCTORS)
#    define Q_DECL_CONSTRUCTOR_DEPRECATED
#  else
#    define Q_DECL_CONSTRUCTOR_DEPRECATED Q_DECL_DEPRECATED
#  endif
#endif

#if defined(QT_NO_DEPRECATED)
/* disable Qt3 support as well */
#  undef QT3_SUPPORT_WARNINGS
#  undef QT3_SUPPORT
#  undef QT_DEPRECATED
#  undef QT_DEPRECATED_VARIABLE
#  undef QT_DEPRECATED_CONSTRUCTOR
#elif defined(QT_DEPRECATED_WARNINGS)
/* enable Qt3 support warnings as well */
#  undef QT3_SUPPORT_WARNINGS
#  define QT3_SUPPORT_WARNINGS
#  undef QT_DEPRECATED
#  define QT_DEPRECATED Q_DECL_DEPRECATED
#  undef QT_DEPRECATED_VARIABLE
#  define QT_DEPRECATED_VARIABLE Q_DECL_VARIABLE_DEPRECATED
#  undef QT_DEPRECATED_CONSTRUCTOR
#  define QT_DEPRECATED_CONSTRUCTOR explicit Q_DECL_CONSTRUCTOR_DEPRECATED
#else
#  undef QT_DEPRECATED
#  define QT_DEPRECATED
#  undef QT_DEPRECATED_VARIABLE
#  define QT_DEPRECATED_VARIABLE
#  undef QT_DEPRECATED_CONSTRUCTOR
#  define QT_DEPRECATED_CONSTRUCTOR
#endif

#if defined(QT3_SUPPORT_WARNINGS)
#  if !defined(QT_COMPAT_WARNINGS) /* also enable compat */
#    define QT_COMPAT_WARNINGS
#  endif
#  undef QT3_SUPPORT
#  define QT3_SUPPORT Q_DECL_DEPRECATED
#  undef QT3_SUPPORT_VARIABLE
#  define QT3_SUPPORT_VARIABLE Q_DECL_VARIABLE_DEPRECATED
#  undef QT3_SUPPORT_CONSTRUCTOR
#  define QT3_SUPPORT_CONSTRUCTOR explicit Q_DECL_CONSTRUCTOR_DEPRECATED
#elif defined(QT3_SUPPORT) /* define back to nothing */
#  if !defined(QT_COMPAT) /* also enable qt3 support */
#    define QT_COMPAT
#  endif
#  undef QT3_SUPPORT
#  define QT3_SUPPORT
#  undef QT3_SUPPORT_VARIABLE
#  define QT3_SUPPORT_VARIABLE
#  undef QT3_SUPPORT_CONSTRUCTOR
#  define QT3_SUPPORT_CONSTRUCTOR explicit
#endif

/* moc compats (signals/slots) */
#ifndef QT_MOC_COMPAT
#  if defined(QT3_SUPPORT)
#    define QT_MOC_COMPAT QT3_SUPPORT
#  else
#    define QT_MOC_COMPAT
#  endif
#else
#  undef QT_MOC_COMPAT
#  define QT_MOC_COMPAT
#endif

#ifdef QT_ASCII_CAST_WARNINGS
#  define QT_ASCII_CAST_WARN Q_DECL_DEPRECATED
#  if defined(Q_CC_GNU) && __GNUC__ < 4
     /* gcc < 4 doesn't like Q_DECL_DEPRECATED in front of constructors */
#    define QT_ASCII_CAST_WARN_CONSTRUCTOR
#  else
#    define QT_ASCII_CAST_WARN_CONSTRUCTOR Q_DECL_CONSTRUCTOR_DEPRECATED
#  endif
#else
#  define QT_ASCII_CAST_WARN
#  define QT_ASCII_CAST_WARN_CONSTRUCTOR
#endif

#if defined(__i386__) || defined(_WIN32) || defined(_WIN32_WCE)
#  if defined(Q_CC_GNU)
#if !defined(Q_CC_INTEL) && ((100*(__GNUC__ - 0) + 10*(__GNUC_MINOR__ - 0) + __GNUC_PATCHLEVEL__) >= 332)
#    define QT_FASTCALL __attribute__((regparm(3)))
#else
#    define QT_FASTCALL
#endif
#  elif defined(Q_CC_MSVC) && (_MSC_VER > 1300 || defined(Q_CC_INTEL))
#    define QT_FASTCALL __fastcall
#  else
#     define QT_FASTCALL
#  endif
#else
#  define QT_FASTCALL
#endif

typedef int QNoImplicitBoolCast;

#if defined(QT_ARCH_ARM) || defined(QT_ARCH_ARMV6) || defined(QT_ARCH_AVR32) || (defined(QT_ARCH_MIPS) && (defined(Q_WS_QWS) || defined(Q_OS_WINCE))) || defined(QT_ARCH_SH) || defined(QT_ARCH_SH4A)
#define QT_NO_FPU
#endif

#if defined(QT_COORD_TYPE)
typedef QT_COORD_TYPE qreal;
#elif defined(QT_NO_FPU) || defined(QT_ARCH_ARM) || defined(QT_ARCH_WINDOWSCE)
typedef float qreal;
#else
typedef double qreal;
#endif

/*
   Utility macros and inline functions
*/

template <typename T>
inline T qAbs(const T &t) { return t >= 0 ? t : -t; }

inline int qRound(qreal d)
{ return d >= 0.0 ? int(d + 0.5) : int(d - int(d-1) + 0.5) + int(d-1); }

inline qint64 qRound64(qreal d)
{ return d >= 0.0 ? qint64(d + 0.5) : qint64(d - qint64(d-1) + 0.5) + qint64(d-1); }

template <typename T>
inline const T &qMin(const T &a, const T &b) { if (a < b) return a; return b; }
template <typename T>
inline const T &qMax(const T &a, const T &b) { if (a < b) return b; return a; }
template <typename T>
inline const T &qBound(const T &min, const T &val, const T &max)
{ return qMax(min, qMin(max, val)); }

#ifdef QT3_SUPPORT
typedef qint8 Q_INT8;
typedef quint8 Q_UINT8;
typedef qint16 Q_INT16;
typedef quint16 Q_UINT16;
typedef qint32 Q_INT32;
typedef quint32 Q_UINT32;
typedef qint64 Q_INT64;
typedef quint64 Q_UINT64;

typedef qint64 Q_LLONG;
typedef quint64 Q_ULLONG;
#if defined(Q_OS_WIN64)
typedef __int64 Q_LONG;             /* word up to 64 bit signed */
typedef unsigned __int64 Q_ULONG;   /* word up to 64 bit unsigned */
#else
typedef long Q_LONG;                /* word up to 64 bit signed */
typedef unsigned long Q_ULONG;      /* word up to 64 bit unsigned */
#endif

#  define QABS(a) qAbs(a)
#  define QMAX(a, b) qMax((a), (b))
#  define QMIN(a, b) qMin((a), (b))
#endif

/*
   Data stream functions are provided by many classes (defined in qdatastream.h)
*/

class QDataStream;

#ifndef QT_BUILD_KEY
#define QT_BUILD_KEY "unspecified"
#endif

#if defined(Q_WS_MAC)
#  ifndef QMAC_QMENUBAR_NO_EVENT
#    define QMAC_QMENUBAR_NO_EVENT
#  endif
#endif

#if !defined(Q_WS_QWS) && !defined(QT_NO_COP)
#  define QT_NO_COP
#endif

# include <QtCore/qfeatures.h>

#define QT_SUPPORTS(FEATURE) (!defined(QT_NO_##FEATURE))

#ifndef Q_DECL_EXPORT
#  ifdef Q_OS_WIN
#    define Q_DECL_EXPORT __declspec(dllexport)
#  elif defined(QT_VISIBILITY_AVAILABLE)
#    define Q_DECL_EXPORT __attribute__((visibility("default")))
#  endif
#  ifndef Q_DECL_EXPORT
#    define Q_DECL_EXPORT
#  endif
#endif
#ifndef Q_DECL_IMPORT
#  if defined(Q_OS_WIN)
#    define Q_DECL_IMPORT __declspec(dllimport)
#  else
#    define Q_DECL_IMPORT
#  endif
#endif

/*
   Create Qt DLL if QT_DLL is defined (Windows only)
*/

#if defined(Q_OS_WIN)
#  if defined(QT_NODLL)
#    undef QT_MAKEDLL
#    undef QT_DLL
#  elif defined(QT_MAKEDLL)        /* create a Qt DLL library */
#    if defined(QT_DLL)
#      undef QT_DLL
#    endif
#    if defined(QT_BUILD_CORE_LIB)
#      define Q_CORE_EXPORT Q_DECL_EXPORT
#    else
#      define Q_CORE_EXPORT Q_DECL_IMPORT
#    endif
#    if defined(QT_BUILD_GUI_LIB)
#      define Q_GUI_EXPORT Q_DECL_EXPORT
#    else
#      define Q_GUI_EXPORT Q_DECL_IMPORT
#    endif
#    if defined(QT_BUILD_SQL_LIB)
#      define Q_SQL_EXPORT Q_DECL_EXPORT
#    else
#      define Q_SQL_EXPORT Q_DECL_IMPORT
#    endif
#    if defined(QT_BUILD_NETWORK_LIB)
#      define Q_NETWORK_EXPORT Q_DECL_EXPORT
#    else
#      define Q_NETWORK_EXPORT Q_DECL_IMPORT
#    endif
#    if defined(QT_BUILD_SVG_LIB)
#      define Q_SVG_EXPORT Q_DECL_EXPORT
#    else
#      define Q_SVG_EXPORT Q_DECL_IMPORT
#    endif
#    if defined(QT_BUILD_OPENGL_LIB)
#      define Q_OPENGL_EXPORT Q_DECL_EXPORT
#    else
#      define Q_OPENGL_EXPORT Q_DECL_IMPORT
#    endif
#    if defined(QT_BUILD_XML_LIB)
#      define Q_XML_EXPORT Q_DECL_EXPORT
#    else
#      define Q_XML_EXPORT Q_DECL_IMPORT
#    endif
#    if defined(QT_BUILD_XMLPATTERNS_LIB)
#      define Q_XMLPATTERNS_EXPORT Q_DECL_EXPORT
#    else
#      define Q_XMLPATTERNS_EXPORT Q_DECL_IMPORT
#    endif
#    if defined(QT_BUILD_SCRIPT_LIB)
#      define Q_SCRIPT_EXPORT Q_DECL_EXPORT
#    else
#      define Q_SCRIPT_EXPORT Q_DECL_IMPORT
#    endif
#    if defined(QT_BUILD_SCRIPTTOOLS_LIB)
#      define Q_SCRIPTTOOLS_EXPORT Q_DECL_EXPORT
#    else
#      define Q_SCRIPTTOOLS_EXPORT Q_DECL_IMPORT
#    endif
#    if defined(QT_BUILD_CANVAS_LIB)
#      define Q_CANVAS_EXPORT Q_DECL_EXPORT
#    else
#      define Q_CANVAS_EXPORT Q_DECL_IMPORT
#    endif
#    if defined(QT_BUILD_COMPAT_LIB)
#      define Q_COMPAT_EXPORT Q_DECL_EXPORT
#    else
#      define Q_COMPAT_EXPORT Q_DECL_IMPORT
#    endif
#    define Q_TEMPLATEDLL
#  elif defined(QT_DLL) /* use a Qt DLL library */
#    define Q_CORE_EXPORT Q_DECL_IMPORT
#    define Q_GUI_EXPORT Q_DECL_IMPORT
#    define Q_SQL_EXPORT Q_DECL_IMPORT
#    define Q_NETWORK_EXPORT Q_DECL_IMPORT
#    define Q_SVG_EXPORT Q_DECL_IMPORT
#    define Q_CANVAS_EXPORT Q_DECL_IMPORT
#    define Q_OPENGL_EXPORT Q_DECL_IMPORT
#    define Q_XML_EXPORT Q_DECL_IMPORT
#    define Q_XMLPATTERNS_EXPORT Q_DECL_IMPORT
#    define Q_SCRIPT_EXPORT Q_DECL_IMPORT
#    define Q_SCRIPTTOOLS_EXPORT Q_DECL_IMPORT
#    define Q_COMPAT_EXPORT Q_DECL_IMPORT
#    define Q_TEMPLATEDLL
#  endif
#  define Q_NO_DECLARED_NOT_DEFINED
#else
#  if defined(Q_OS_LINUX) && defined(Q_CC_BOR)
#    define Q_TEMPLATEDLL
#    define Q_NO_DECLARED_NOT_DEFINED
#  endif
#  undef QT_MAKEDLL /* ignore these for other platforms */
#  undef QT_DLL
#endif

#if !defined(Q_CORE_EXPORT)
#  if defined(QT_SHARED)
#    define Q_CORE_EXPORT Q_DECL_EXPORT
#    define Q_GUI_EXPORT Q_DECL_EXPORT
#    define Q_SQL_EXPORT Q_DECL_EXPORT
#    define Q_NETWORK_EXPORT Q_DECL_EXPORT
#    define Q_SVG_EXPORT Q_DECL_EXPORT
#    define Q_OPENGL_EXPORT Q_DECL_EXPORT
#    define Q_XML_EXPORT Q_DECL_EXPORT
#    define Q_XMLPATTERNS_EXPORT Q_DECL_EXPORT
#    define Q_SCRIPT_EXPORT Q_DECL_EXPORT
#    define Q_SCRIPTTOOLS_EXPORT Q_DECL_EXPORT
#    define Q_COMPAT_EXPORT Q_DECL_EXPORT
#  else
#    define Q_CORE_EXPORT
#    define Q_GUI_EXPORT
#    define Q_SQL_EXPORT
#    define Q_NETWORK_EXPORT
#    define Q_SVG_EXPORT
#    define Q_OPENGL_EXPORT
#    define Q_XML_EXPORT
#    define Q_XMLPATTERNS_EXPORT
#    define Q_SCRIPT_EXPORT
#    define Q_SCRIPTTOOLS_EXPORT
#    define Q_COMPAT_EXPORT
#  endif
#endif

// Functions marked as Q_GUI_EXPORT_INLINE were exported and inlined by mistake.
// Compilers like MinGW complain that the import attribute is ignored.
#if defined(Q_CC_MINGW)
#    if defined(QT_BUILD_CORE_LIB)
#      define Q_CORE_EXPORT_INLINE Q_CORE_EXPORT inline
#    else
#      define Q_CORE_EXPORT_INLINE inline
#    endif
#    if defined(QT_BUILD_GUI_LIB)
#      define Q_GUI_EXPORT_INLINE Q_GUI_EXPORT inline
#    else
#      define Q_GUI_EXPORT_INLINE inline
#    endif
#else
#    define Q_CORE_EXPORT_INLINE Q_CORE_EXPORT inline
#    define Q_GUI_EXPORT_INLINE Q_GUI_EXPORT inline
#endif

/*
   No, this is not an evil backdoor. QT_BUILD_INTERNAL just exports more symbols
   for Trolltech's internal unit tests. If you want slower loading times and more
   symbols that can vanish from version to version, feel free to define QT_BUILD_INTERNAL.
*/
#if defined(QT_BUILD_INTERNAL) && defined(Q_OS_WIN) && defined(QT_MAKEDLL)
#    define Q_AUTOTEST_EXPORT Q_DECL_EXPORT
#elif defined(QT_BUILD_INTERNAL) && defined(Q_OS_WIN) && defined(QT_DLL)
#    define Q_AUTOTEST_EXPORT Q_DECL_IMPORT
#elif defined(QT_BUILD_INTERNAL) && !defined(Q_OS_WIN) && defined(QT_SHARED)
#    define Q_AUTOTEST_EXPORT Q_DECL_EXPORT
#else
#    define Q_AUTOTEST_EXPORT
#endif

/*
   System information
*/

class QString;
class Q_CORE_EXPORT QSysInfo {
public:
    enum Sizes {
        WordSize = (sizeof(void *)<<3)
    };

#if defined(QT_BUILD_QMAKE)
    enum Endian {
        BigEndian,
        LittleEndian
    };
    /* needed to bootstrap qmake */
    static const int ByteOrder;
#elif defined(Q_BYTE_ORDER)
    enum Endian {
        BigEndian,
        LittleEndian

#  ifdef qdoc
        , ByteOrder = <platform-dependent>
#  elif Q_BYTE_ORDER == Q_BIG_ENDIAN
        , ByteOrder = BigEndian
#  elif Q_BYTE_ORDER == Q_LITTLE_ENDIAN
        , ByteOrder = LittleEndian
#  else
#    error "Undefined byte order"
#  endif
    };
#else
#  error "Qt not configured correctly, please run configure"
#endif
#if defined(Q_WS_WIN) || defined(Q_OS_CYGWIN)
    enum WinVersion {
        WV_32s      = 0x0001,
        WV_95       = 0x0002,
        WV_98       = 0x0003,
        WV_Me       = 0x0004,
        WV_DOS_based= 0x000f,

        /* codenames */
        WV_NT       = 0x0010,
        WV_2000     = 0x0020,
        WV_XP       = 0x0030,
        WV_2003     = 0x0040,
        WV_VISTA    = 0x0080,
        WV_NT_based = 0x00f0,

        /* version numbers */
        WV_4_0      = WV_NT,
        WV_5_0      = WV_2000,
        WV_5_1      = WV_XP,
        WV_5_2      = WV_2003,
        WV_6_0      = WV_VISTA,

        WV_CE       = 0x0100,
        WV_CENET    = 0x0200,
        WV_CE_5     = 0x0300,
        WV_CE_6     = 0x0400,
        WV_CE_based = 0x0f00
    };
    static const WinVersion WindowsVersion;
    static WinVersion windowsVersion();

#endif
#ifdef Q_OS_MAC
    enum MacVersion {
        MV_Unknown = 0x0000,

        /* version */
        MV_9 = 0x0001,
        MV_10_0 = 0x0002,
        MV_10_1 = 0x0003,
        MV_10_2 = 0x0004,
        MV_10_3 = 0x0005,
        MV_10_4 = 0x0006,
        MV_10_5 = 0x0007,

        /* codenames */
        MV_CHEETAH = MV_10_0,
        MV_PUMA = MV_10_1,
        MV_JAGUAR = MV_10_2,
        MV_PANTHER = MV_10_3,
        MV_TIGER = MV_10_4,
        MV_LEOPARD = MV_10_5
    };
    static const MacVersion MacintoshVersion;
#endif
};

Q_CORE_EXPORT const char *qVersion();
Q_CORE_EXPORT bool qSharedBuild();

#if defined(Q_OS_MAC)
inline int qMacVersion() { return QSysInfo::MacintoshVersion; }
#endif

#ifdef QT3_SUPPORT
inline QT3_SUPPORT bool qSysInfo(int *wordSize, bool *bigEndian)
{
    *wordSize = QSysInfo::WordSize;
    *bigEndian = (QSysInfo::ByteOrder == QSysInfo::BigEndian);
    return true;
}
#endif

#if defined(Q_WS_WIN) || defined(Q_OS_CYGWIN)
#if defined(QT3_SUPPORT)
inline QT3_SUPPORT bool qt_winUnicode() { return !(QSysInfo::WindowsVersion & QSysInfo::WV_DOS_based); }
inline QT3_SUPPORT int qWinVersion() { return QSysInfo::WindowsVersion; }
#endif

#ifdef Q_OS_WINCE
#define QT_WA(uni, ansi) uni
#define QT_WA_INLINE(uni, ansi) (uni)
#elif defined(UNICODE)
#define QT_WA(uni, ansi) if (!(QSysInfo::windowsVersion() & QSysInfo::WV_DOS_based)) { uni } else { ansi }

#define QT_WA_INLINE(uni, ansi) (!(QSysInfo::windowsVersion() & QSysInfo::WV_DOS_based) ? uni : ansi)
#else
#define QT_WA(uni, ansi) ansi
#define QT_WA_INLINE(uni, ansi) ansi
#endif
#endif /* Q_WS_WIN */

#ifndef Q_OUTOFLINE_TEMPLATE
#  define Q_OUTOFLINE_TEMPLATE
#endif
#ifndef Q_INLINE_TEMPLATE
#  define Q_INLINE_TEMPLATE inline
#endif

#ifndef Q_TYPENAME
#  define Q_TYPENAME typename
#endif

/*
   Avoid "unused parameter" warnings
*/

#if defined(Q_CC_INTEL) && !defined(Q_OS_WIN)
template <typename T>
inline void qUnused(T &x) { (void)x; }
#  define Q_UNUSED(x) qUnused(x);
#else
#  define Q_UNUSED(x) (void)x;
#endif

/*
   Debugging and error handling
*/

#if !defined(QT_NO_DEBUG) && !defined(QT_DEBUG)
#  define QT_DEBUG
#endif

#ifndef qPrintable
#  define qPrintable(string) (string).toLocal8Bit().constData()
#endif

Q_CORE_EXPORT void qDebug(const char *, ...) /* print debug message */
#if defined(Q_CC_GNU) && !defined(__INSURE__)
    __attribute__ ((format (printf, 1, 2)))
#endif
;

Q_CORE_EXPORT void qWarning(const char *, ...) /* print warning message */
#if defined(Q_CC_GNU) && !defined(__INSURE__)
    __attribute__ ((format (printf, 1, 2)))
#endif
;

class QString;
Q_CORE_EXPORT QString qt_error_string(int errorCode = -1);
Q_CORE_EXPORT void qCritical(const char *, ...) /* print critical message */
#if defined(Q_CC_GNU) && !defined(__INSURE__)
    __attribute__ ((format (printf, 1, 2)))
#endif
;
Q_CORE_EXPORT void qFatal(const char *, ...) /* print fatal message and exit */
#if defined(Q_CC_GNU) && !defined(__INSURE__)
    __attribute__ ((format (printf, 1, 2)))
#endif
;

#ifdef QT3_SUPPORT
Q_CORE_EXPORT QT3_SUPPORT void qSystemWarning(const char *msg, int code = -1);
#endif /* QT3_SUPPORT */
Q_CORE_EXPORT void qErrnoWarning(int code, const char *msg, ...);
Q_CORE_EXPORT void qErrnoWarning(const char *msg, ...);

#if (defined(QT_NO_DEBUG_OUTPUT) || defined(QT_NO_TEXTSTREAM)) && !defined(QT_NO_DEBUG_STREAM)
#define QT_NO_DEBUG_STREAM
#endif

/*
  Forward declarations only.

  In order to use the qDebug() stream, you must #include<QDebug>
*/
class QDebug;
class QNoDebug;
#ifndef QT_NO_DEBUG_STREAM
Q_CORE_EXPORT_INLINE QDebug qDebug();
Q_CORE_EXPORT_INLINE QDebug qWarning();
Q_CORE_EXPORT_INLINE QDebug qCritical();
#else
inline QNoDebug qDebug();
#endif

#define QT_NO_QDEBUG_MACRO if(1); else qDebug
#ifdef QT_NO_DEBUG_OUTPUT
#  define qDebug QT_NO_QDEBUG_MACRO
#endif
#define QT_NO_QWARNING_MACRO if(1); else qWarning
#ifdef QT_NO_WARNING_OUTPUT
#  define qWarning QT_NO_QWARNING_MACRO
#endif


inline void qt_noop() {}

Q_CORE_EXPORT void qt_assert(const char *assertion, const char *file, int line);

#if !defined(Q_ASSERT)
#  ifndef QT_NO_DEBUG
#    define Q_ASSERT(cond) ((!(cond)) ? qt_assert(#cond,__FILE__,__LINE__) : qt_noop())
#  else
#    define Q_ASSERT(cond) qt_noop()
#  endif
#endif

#if defined(QT_NO_DEBUG) && !defined(QT_PAINT_DEBUG)
#define QT_NO_PAINT_DEBUG
#endif

Q_CORE_EXPORT void qt_assert_x(const char *where, const char *what, const char *file, int line);

#if !defined(Q_ASSERT_X)
#  ifndef QT_NO_DEBUG
#    define Q_ASSERT_X(cond, where, what) ((!(cond)) ? qt_assert_x(where, what,__FILE__,__LINE__) : qt_noop())
#  else
#    define Q_ASSERT_X(cond, where, what) qt_noop()
#  endif
#endif

Q_CORE_EXPORT void qt_check_pointer(const char *, int);

#ifndef QT_NO_DEBUG
#  define Q_CHECK_PTR(p) do {if(!(p))qt_check_pointer(__FILE__,__LINE__);} while (0)
#else
#  define Q_CHECK_PTR(p)
#endif

#if (defined(Q_CC_GNU) && !defined(Q_OS_SOLARIS)) || defined(Q_CC_HPACC)
#  define Q_FUNC_INFO __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
    /* MSVC 2002 doesn't have __FUNCSIG__ nor can it handle QT_STRINGIFY. */
#  if _MSC_VER <= 1300
#      define Q_FUNC_INFO __FILE__ "(line number unavailable)"
#  else
#      define Q_FUNC_INFO __FUNCSIG__
#  endif
#else
#   if defined(Q_OS_SOLARIS) || defined(Q_CC_XLC)
#      define Q_FUNC_INFO __FILE__ "(line number unavailable)"
#   else
        /* These two macros makes it possible to turn the builtin line expander into a
         * string literal. */
#       define QT_STRINGIFY2(x) #x
#       define QT_STRINGIFY(x) QT_STRINGIFY2(x)
#       define Q_FUNC_INFO __FILE__ ":" QT_STRINGIFY(__LINE__)
#   endif
    /* The MIPSpro compiler postpones macro expansion, and therefore macros must be in scope
     * when being used. */
#   if !defined(Q_CC_MIPS)
#       undef QT_STRINGIFY2
#       undef QT_STRINGIFY
#   endif
#endif

enum QtMsgType { QtDebugMsg, QtWarningMsg, QtCriticalMsg, QtFatalMsg, QtSystemMsg = QtCriticalMsg };

Q_CORE_EXPORT void qt_message_output(QtMsgType, const char *buf);

typedef void (*QtMsgHandler)(QtMsgType, const char *);
Q_CORE_EXPORT QtMsgHandler qInstallMsgHandler(QtMsgHandler);

#ifdef QT3_SUPPORT
inline QT3_SUPPORT void qSuppressObsoleteWarnings(bool = true) {}
inline QT3_SUPPORT void qObsolete(const char *, const char * = 0, const char * = 0) {}
#endif

#if defined(QT_NO_THREAD)

template <typename T>
class QGlobalStatic
{
public:
    T *pointer;
    inline QGlobalStatic(T *p) : pointer(p) { }
    inline ~QGlobalStatic() { pointer = 0; }
};

#define Q_GLOBAL_STATIC(TYPE, NAME)                              \
    static TYPE *NAME()                                          \
    {                                                            \
        static TYPE this_##NAME;                                 \
        static QGlobalStatic<TYPE > global_##NAME(&this_##NAME); \
        return global_##NAME.pointer;                            \
    }

#define Q_GLOBAL_STATIC_WITH_ARGS(TYPE, NAME, ARGS)              \
    static TYPE *NAME()                                          \
    {                                                            \
        static TYPE this_##NAME ARGS;                            \
        static QGlobalStatic<TYPE > global_##NAME(&this_##NAME); \
        return global_##NAME.pointer;                            \
    }

#define Q_GLOBAL_STATIC_WITH_INITIALIZER(TYPE, NAME, INITIALIZER) \
    static TYPE *NAME()                                           \
    {                                                             \
        static TYPE this_##NAME;                                  \
        static QGlobalStatic<TYPE > global_##NAME(0);             \
        if (!global_##NAME.pointer) {                             \
            TYPE *x = global_##NAME.pointer = &this_##NAME;       \
            INITIALIZER;                                          \
        }                                                         \
        return global_##NAME.pointer;                             \
    }

#else

// forward declaration, since qatomic.h needs qglobal.h
template <typename T> class QBasicAtomicPointer;

// POD for Q_GLOBAL_STATIC
template <typename T>
class QGlobalStatic
{
public:
    QBasicAtomicPointer<T> pointer;
    bool destroyed;
};

// Created as a function-local static to delete a QGlobalStatic<T>
template <typename T>
class QGlobalStaticDeleter
{
public:
    QGlobalStatic<T> &globalStatic;
    QGlobalStaticDeleter(QGlobalStatic<T> &_globalStatic)
        : globalStatic(_globalStatic)
    { }

    inline ~QGlobalStaticDeleter()
    {
        delete globalStatic.pointer;
        globalStatic.pointer = 0;
        globalStatic.destroyed = true;
    }
};

#define Q_GLOBAL_STATIC_INIT(TYPE, NAME)                              \
    static QGlobalStatic<TYPE > this_##NAME = { Q_BASIC_ATOMIC_INITIALIZER(0), false }

#define Q_GLOBAL_STATIC(TYPE, NAME)                                     \
    Q_GLOBAL_STATIC_INIT(TYPE, NAME);                                   \
    static TYPE *NAME()                                                 \
    {                                                                   \
        if (!this_##NAME.pointer && !this_##NAME.destroyed) {           \
            TYPE *x = new TYPE;                                         \
            if (!this_##NAME.pointer.testAndSetOrdered(0, x))           \
                delete x;                                               \
            else                                                        \
                static QGlobalStaticDeleter<TYPE > cleanup(this_##NAME); \
        }                                                               \
        return this_##NAME.pointer;                                     \
    }

#define Q_GLOBAL_STATIC_WITH_ARGS(TYPE, NAME, ARGS)                     \
    Q_GLOBAL_STATIC_INIT(TYPE, NAME);                                   \
    static TYPE *NAME()                                                 \
    {                                                                   \
        if (!this_##NAME.pointer && !this_##NAME.destroyed) {           \
            TYPE *x = new TYPE ARGS;                                    \
            if (!this_##NAME.pointer.testAndSetOrdered(0, x))           \
                delete x;                                               \
            else                                                        \
                static QGlobalStaticDeleter<TYPE > cleanup(this_##NAME); \
        }                                                               \
        return this_##NAME.pointer;                                     \
    }

#define Q_GLOBAL_STATIC_WITH_INITIALIZER(TYPE, NAME, INITIALIZER)       \
    Q_GLOBAL_STATIC_INIT(TYPE, NAME);                                   \
    static TYPE *NAME()                                                 \
    {                                                                   \
        if (!this_##NAME.pointer && !this_##NAME.destroyed) {           \
            TYPE *x = new TYPE;                                         \
            INITIALIZER;                                                \
            if (!this_##NAME.pointer.testAndSetOrdered(0, x))           \
                delete x;                                               \
            else                                                        \
                static QGlobalStaticDeleter<TYPE > cleanup(this_##NAME); \
        }                                                               \
        return this_##NAME.pointer;                                     \
    }

#endif

class QBool
{
    bool b;

public:
    inline explicit QBool(bool B) : b(B) {}
    inline operator const void *() const
    { return b ? static_cast<const void *>(this) : static_cast<const void *>(0); }
};

inline bool operator==(QBool b1, bool b2) { return !b1 == !b2; }
inline bool operator==(bool b1, QBool b2) { return !b1 == !b2; }
inline bool operator==(QBool b1, QBool b2) { return !b1 == !b2; }
inline bool operator!=(QBool b1, bool b2) { return !b1 != !b2; }
inline bool operator!=(bool b1, QBool b2) { return !b1 != !b2; }
inline bool operator!=(QBool b1, QBool b2) { return !b1 != !b2; }

static inline bool qFuzzyCompare(double p1, double p2)
{
    return (qAbs(p1 - p2) <= 0.000000000001 * qMin(qAbs(p1), qAbs(p2)));
}

static inline bool qFuzzyCompare(float p1, float p2)
{
    return (qAbs(p1 - p2) <= 0.00001f * qMin(qAbs(p1), qAbs(p2)));
}

/*
   This function tests a double for a null value. It doesn't
   check whether the actual value is 0 or close to 0, but whether
   it is binary 0.
*/
static inline bool qIsNull(double d)
{
    union U {
        double d;
        quint64 u;
    };
    U val;
    val.d = d;
    return val.u == quint64(0);
}

/*
   This function tests a float for a null value. It doesn't
   check whether the actual value is 0 or close to 0, but whether
   it is binary 0.
*/
static inline bool qIsNull(float f)
{
    union U {
        float f;
        quint32 u;
    };
    U val;
    val.f = f;
    return val.u == 0u;
}

/*
   Compilers which follow outdated template instantiation rules
   require a class to have a comparison operator to exist when
   a QList of this type is instantiated. It's not actually
   used in the list, though. Hence the dummy implementation.
   Just in case other code relies on it we better trigger a warning
   mandating a real implementation.
*/

#ifdef Q_FULL_TEMPLATE_INSTANTIATION
#  define Q_DUMMY_COMPARISON_OPERATOR(C) \
    bool operator==(const C&) const { \
        qWarning(#C"::operator==(const "#C"&) was called"); \
        return false; \
    }
#else
#  define Q_DUMMY_COMPARISON_OPERATOR(C)
#endif


/*
   QTypeInfo     - type trait functionality
   qIsDetached   - data sharing functionality
*/

#ifndef QT_NO_PARTIAL_TEMPLATE_SPECIALIZATION

/*
  The catch-all template.
*/

template <typename T> inline bool qIsDetached(T &) { return true; }

template <typename T>
class QTypeInfo
{
public:
    enum {
        isPointer = false,
        isComplex = true,
        isStatic = true,
        isLarge = (sizeof(T)>sizeof(void*)),
        isDummy = false
    };
};

template <typename T>
class QTypeInfo<T*>
{
public:
    enum {
        isPointer = true,
        isComplex = false,
        isStatic = false,
        isLarge = false,
        isDummy = false
    };
};

#else

template <typename T> char QTypeInfoHelper(T*(*)());
void* QTypeInfoHelper(...);

template <typename T> inline bool qIsDetached(T &) { return true; }

template <typename T>
class QTypeInfo
{
public:
    enum {
        isPointer = (1 == sizeof(QTypeInfoHelper((T(*)())0))),
        isComplex = !isPointer,
        isStatic = !isPointer,
        isLarge = (sizeof(T)>sizeof(void*)),
        isDummy = false
    };
};

#endif /* QT_NO_PARTIAL_TEMPLATE_SPECIALIZATION */

/*
   Specialize a specific type with:

     Q_DECLARE_TYPEINFO(type, flags);

   where 'type' is the name of the type to specialize and 'flags' is
   logically-OR'ed combination of the flags below.
*/
enum { /* TYPEINFO flags */
    Q_COMPLEX_TYPE = 0,
    Q_PRIMITIVE_TYPE = 0x1,
    Q_STATIC_TYPE = 0,
    Q_MOVABLE_TYPE = 0x2,
    Q_DUMMY_TYPE = 0x4
};

#define Q_DECLARE_TYPEINFO(TYPE, FLAGS) \
template <> \
class QTypeInfo<TYPE> \
{ \
public: \
    enum { \
        isComplex = (((FLAGS) & Q_PRIMITIVE_TYPE) == 0), \
        isStatic = (((FLAGS) & (Q_MOVABLE_TYPE | Q_PRIMITIVE_TYPE)) == 0), \
        isLarge = (sizeof(TYPE)>sizeof(void*)), \
        isPointer = false, \
        isDummy = (((FLAGS) & Q_DUMMY_TYPE) != 0) \
    }; \
    static inline const char *name() { return #TYPE; } \
}

/*
   Specialize a shared type with:

     Q_DECLARE_SHARED(type);

   where 'type' is the name of the type to specialize.  NOTE: shared
   types must declare a 'bool isDetached(void) const;' member for this
   to work.
*/
#if defined Q_CC_MSVC && _MSC_VER < 1300
template <typename T>
inline void qSwap_helper(T &value1, T &value2, T*)
{
    T t = value1;
    value1 = value2;
    value2 = t;
}
#define Q_DECLARE_SHARED(TYPE)                                          \
template <> inline bool qIsDetached<TYPE>(TYPE &t) { return t.isDetached(); } \
template <> inline void qSwap_helper<TYPE>(TYPE &value1, TYPE &value2, TYPE*) \
{ \
    const TYPE::DataPtr t = value1.data_ptr(); \
    value1.data_ptr() = value2.data_ptr(); \
    value2.data_ptr() = t; \
}
#else
#define Q_DECLARE_SHARED(TYPE)                                          \
template <> inline bool qIsDetached<TYPE>(TYPE &t) { return t.isDetached(); } \
template <typename T> inline void qSwap(T &, T &); \
template <> inline void qSwap<TYPE>(TYPE &value1, TYPE &value2) \
{ \
    const TYPE::DataPtr t = value1.data_ptr(); \
    value1.data_ptr() = value2.data_ptr(); \
    value2.data_ptr() = t; \
}
#endif

/*
   QTypeInfo primitive specializations
*/
Q_DECLARE_TYPEINFO(bool, Q_PRIMITIVE_TYPE);
Q_DECLARE_TYPEINFO(char, Q_PRIMITIVE_TYPE);
Q_DECLARE_TYPEINFO(signed char, Q_PRIMITIVE_TYPE);
Q_DECLARE_TYPEINFO(uchar, Q_PRIMITIVE_TYPE);
Q_DECLARE_TYPEINFO(short, Q_PRIMITIVE_TYPE);
Q_DECLARE_TYPEINFO(ushort, Q_PRIMITIVE_TYPE);
Q_DECLARE_TYPEINFO(int, Q_PRIMITIVE_TYPE);
Q_DECLARE_TYPEINFO(uint, Q_PRIMITIVE_TYPE);
Q_DECLARE_TYPEINFO(long, Q_PRIMITIVE_TYPE);
Q_DECLARE_TYPEINFO(ulong, Q_PRIMITIVE_TYPE);
Q_DECLARE_TYPEINFO(qint64, Q_PRIMITIVE_TYPE);
Q_DECLARE_TYPEINFO(quint64, Q_PRIMITIVE_TYPE);
Q_DECLARE_TYPEINFO(float, Q_PRIMITIVE_TYPE);
Q_DECLARE_TYPEINFO(double, Q_PRIMITIVE_TYPE);
#ifndef Q_OS_DARWIN
Q_DECLARE_TYPEINFO(long double, Q_PRIMITIVE_TYPE);
#endif

/*
   These functions make it possible to use standard C++ functions with
   a similar name from Qt header files (especially template classes).
*/
Q_CORE_EXPORT void *qMalloc(size_t size);
Q_CORE_EXPORT void qFree(void *ptr);
Q_CORE_EXPORT void *qRealloc(void *ptr, size_t size);
Q_CORE_EXPORT void *qMemCopy(void *dest, const void *src, size_t n);
Q_CORE_EXPORT void *qMemSet(void *dest, int c, size_t n);


/*
   Avoid some particularly useless warnings from some stupid compilers.
   To get ALL C++ compiler warnings, define QT_CC_WARNINGS or comment out
   the line "#define QT_NO_WARNINGS".
*/
#if !defined(QT_CC_WARNINGS)
#  define QT_NO_WARNINGS
#endif
#if defined(QT_NO_WARNINGS)
#  if defined(Q_CC_MSVC)
#    pragma warning(disable: 4251) /* class 'A' needs to have dll interface for to be used by clients of class 'B'. */
#    pragma warning(disable: 4244) /* 'conversion' conversion from 'type1' to 'type2', possible loss of data */
#    pragma warning(disable: 4275) /* non - DLL-interface classkey 'identifier' used as base for DLL-interface classkey 'identifier' */
#    pragma warning(disable: 4514) /* unreferenced inline/local function has been removed */
#    pragma warning(disable: 4800) /* 'type' : forcing value to bool 'true' or 'false' (performance warning) */
#    pragma warning(disable: 4097) /* typedef-name 'identifier1' used as synonym for class-name 'identifier2' */
#    pragma warning(disable: 4706) /* assignment within conditional expression */
#    pragma warning(disable: 4786) /* truncating debug info after 255 characters */
#    pragma warning(disable: 4660) /* template-class specialization 'identifier' is already instantiated */
#    pragma warning(disable: 4355) /* 'this' : used in base member initializer list */
#    pragma warning(disable: 4231) /* nonstandard extension used : 'extern' before template explicit instantiation */
#    pragma warning(disable: 4710) /* function not inlined */
#    pragma warning(disable: 4530) /* C++ exception handler used, but unwind semantics are not enabled. Specify -GX */
#    if _MSC_VER < 1300
#      pragma warning(disable: 4284) /* return type for 'type1::operator ->' is 'type2 *' */
                                     /* (ie; not a UDT or reference to a UDT.  Will produce errors if applied using infix notation) */
#    endif
#  elif defined(Q_CC_BOR)
#    pragma option -w-inl
#    pragma option -w-aus
#    pragma warn -inl
#    pragma warn -pia
#    pragma warn -ccc
#    pragma warn -rch
#    pragma warn -sig
#  endif
#endif

class Q_CORE_EXPORT QFlag
{
    int i;
public:
    inline QFlag(int i);
    inline operator int() const { return i; }
};

inline QFlag::QFlag(int ai) : i(ai) {}

class Q_CORE_EXPORT QIncompatibleFlag
{
    int i;
public:
    inline explicit QIncompatibleFlag(int i);
    inline operator int() const { return i; }
};

inline QIncompatibleFlag::QIncompatibleFlag(int ai) : i(ai) {}


#ifndef Q_NO_TYPESAFE_FLAGS

template<typename Enum>
class QFlags
{
    typedef void **Zero;
    int i;
public:
    typedef Enum enum_type;
    inline QFlags(const QFlags &f) : i(f.i) {}
    inline QFlags(Enum f) : i(f) {}
    inline QFlags(Zero = 0) : i(0) {}
    inline QFlags(QFlag f) : i(f) {}

    inline QFlags &operator=(const QFlags &f) { i = f.i; return *this; }
    inline QFlags &operator&=(int mask) { i &= mask; return *this; }
    inline QFlags &operator&=(uint mask) { i &= mask; return *this; }
    inline QFlags &operator|=(QFlags f) { i |= f.i; return *this; }
    inline QFlags &operator|=(Enum f) { i |= f; return *this; }
    inline QFlags &operator^=(QFlags f) { i ^= f.i; return *this; }
    inline QFlags &operator^=(Enum f) { i ^= f; return *this; }

    inline operator int() const { return i; }

    inline QFlags operator|(QFlags f) const { QFlags g; g.i = i | f.i; return g; }
    inline QFlags operator|(Enum f) const { QFlags g; g.i = i | f; return g; }
    inline QFlags operator^(QFlags f) const { QFlags g; g.i = i ^ f.i; return g; }
    inline QFlags operator^(Enum f) const { QFlags g; g.i = i ^ f; return g; }
    inline QFlags operator&(int mask) const { QFlags g; g.i = i & mask; return g; }
    inline QFlags operator&(uint mask) const { QFlags g; g.i = i & mask; return g; }
    inline QFlags operator&(Enum f) const { QFlags g; g.i = i & f; return g; }
    inline QFlags operator~() const { QFlags g; g.i = ~i; return g; }

    inline bool operator!() const { return !i; }

    inline bool testFlag(Enum f) const { return (i & f) == f; }
};

#define Q_DECLARE_FLAGS(Flags, Enum)\
typedef QFlags<Enum> Flags;

#if defined Q_CC_MSVC && _MSC_VER < 1300
# define Q_DECLARE_INCOMPATIBLE_FLAGS(Flags)
#else
# define Q_DECLARE_INCOMPATIBLE_FLAGS(Flags) \
inline QIncompatibleFlag operator|(Flags::enum_type f1, int f2) \
{ return QIncompatibleFlag(int(f1) | f2); }
#endif

#define Q_DECLARE_OPERATORS_FOR_FLAGS(Flags) \
inline QFlags<Flags::enum_type> operator|(Flags::enum_type f1, Flags::enum_type f2) \
{ return QFlags<Flags::enum_type>(f1) | f2; } \
inline QFlags<Flags::enum_type> operator|(Flags::enum_type f1, QFlags<Flags::enum_type> f2) \
{ return f2 | f1; } Q_DECLARE_INCOMPATIBLE_FLAGS(Flags)


#else /* Q_NO_TYPESAFE_FLAGS */

#define Q_DECLARE_FLAGS(Flags, Enum)\
typedef uint Flags;
#define Q_DECLARE_OPERATORS_FOR_FLAGS(Flags)

#endif /* Q_NO_TYPESAFE_FLAGS */

#if defined(Q_CC_GNU) && !defined(Q_CC_INTEL)
/* make use of typeof-extension */
template <typename T>
class QForeachContainer {
public:
    inline QForeachContainer(const T& t) : c(t), brk(0), i(c.begin()), e(c.end()) { }
    const T c;
    int brk;
    typename T::const_iterator i, e;
};

#define Q_FOREACH(variable, container)                                \
for (QForeachContainer<__typeof__(container)> _container_(container); \
     !_container_.brk && _container_.i != _container_.e;              \
     __extension__  ({ ++_container_.brk; ++_container_.i; }))                       \
    for (variable = *_container_.i;; __extension__ ({--_container_.brk; break;}))

#else

struct QForeachContainerBase {};

template <typename T>
class QForeachContainer : public QForeachContainerBase {
public:
    inline QForeachContainer(const T& t): c(t), brk(0), i(c.begin()), e(c.end()){};
    const T c;
    mutable int brk;
    mutable typename T::const_iterator i, e;
    inline bool condition() const { return (!brk++ && i != e); }
};

template <typename T> inline T *qForeachPointer(const T &) { return 0; }

template <typename T> inline QForeachContainer<T> qForeachContainerNew(const T& t)
{ return QForeachContainer<T>(t); }

template <typename T>
inline const QForeachContainer<T> *qForeachContainer(const QForeachContainerBase *base, const T *)
{ return static_cast<const QForeachContainer<T> *>(base); }

#if (defined(Q_CC_MSVC) && !defined(Q_CC_MSVC_NET) && !defined(Q_CC_INTEL)) || defined(Q_CC_MIPS)
/*
   Proper for-scoping in VC++6 and MIPSpro CC
*/
#  define Q_FOREACH(variable,container)                                                             \
    if(0){}else                                                                                     \
    for (const QForeachContainerBase &_container_ = qForeachContainerNew(container);                \
         qForeachContainer(&_container_, true ? 0 : qForeachPointer(container))->condition();       \
         ++qForeachContainer(&_container_, true ? 0 : qForeachPointer(container))->i)               \
        for (variable = *qForeachContainer(&_container_, true ? 0 : qForeachPointer(container))->i; \
             qForeachContainer(&_container_, true ? 0 : qForeachPointer(container))->brk;           \
             --qForeachContainer(&_container_, true ? 0 : qForeachPointer(container))->brk)

#else
#  define Q_FOREACH(variable, container) \
    for (const QForeachContainerBase &_container_ = qForeachContainerNew(container); \
         qForeachContainer(&_container_, true ? 0 : qForeachPointer(container))->condition();       \
         ++qForeachContainer(&_container_, true ? 0 : qForeachPointer(container))->i)               \
        for (variable = *qForeachContainer(&_container_, true ? 0 : qForeachPointer(container))->i; \
             qForeachContainer(&_container_, true ? 0 : qForeachPointer(container))->brk;           \
             --qForeachContainer(&_container_, true ? 0 : qForeachPointer(container))->brk)
#endif // MSVC6 || MIPSpro

#endif

#define Q_FOREVER for(;;)
#ifndef QT_NO_KEYWORDS
#  ifndef foreach
#    define foreach Q_FOREACH
#  endif
#  ifndef forever
#    define forever Q_FOREVER
#  endif
#endif

#if 0
/* tell gcc to use its built-in methods for some common functions */
#if defined(QT_NO_DEBUG) && defined(Q_CC_GNU)
#  define qMemCopy __builtin_memcpy
#  define qMemSet __builtin_memset
#endif
#endif

#define Q_DECLARE_PRIVATE(Class) \
    inline Class##Private* d_func() { return reinterpret_cast<Class##Private *>(d_ptr); } \
    inline const Class##Private* d_func() const { return reinterpret_cast<const Class##Private *>(d_ptr); } \
    friend class Class##Private;

#define Q_DECLARE_PRIVATE_D(Dptr, Class) \
    inline Class##Private* d_func() { return reinterpret_cast<Class##Private *>(Dptr); } \
    inline const Class##Private* d_func() const { return reinterpret_cast<const Class##Private *>(Dptr); } \
    friend class Class##Private;

#define Q_DECLARE_PUBLIC(Class)                                    \
    inline Class* q_func() { return static_cast<Class *>(q_ptr); } \
    inline const Class* q_func() const { return static_cast<const Class *>(q_ptr); } \
    friend class Class;

#define Q_D(Class) Class##Private * const d = d_func()
#define Q_Q(Class) Class * const q = q_func()

#define QT_TR_NOOP(x) (x)
#define QT_TR_NOOP_UTF8(x) (x)
#define QT_TRANSLATE_NOOP(scope, x) (x)
#define QT_TRANSLATE_NOOP_UTF8(scope, x) (x)
#define QT_TRANSLATE_NOOP3(scope, x, comment) {x, comment}
#define QT_TRANSLATE_NOOP3_UTF8(scope, x, comment) {x, comment}
#define QDOC_PROPERTY(text)

/*
   When RTTI is not available, define this macro to force any uses of
   dynamic_cast to cause a compile failure.
*/

#ifdef QT_NO_DYNAMIC_CAST
#  define dynamic_cast QT_PREPEND_NAMESPACE(qt_dynamic_cast_check)

  template<typename T, typename X>
  T qt_dynamic_cast_check(X, T* = 0)
  { return T::dynamic_cast_will_always_fail_because_rtti_is_disabled; }
#endif

/*
   Some classes do not permit copies to be made of an object. These
   classes contains a private copy constructor and assignment
   operator to disable copying (the compiler gives an error message).
*/

#if !defined(Q_NO_DECLARED_NOT_DEFINED) || !defined(QT_MAKEDLL)
# define Q_DISABLE_COPY(Class) \
     Class(const Class &); \
     Class &operator=(const Class &);
#else
# define Q_DISABLE_COPY(Class)
#endif

class QByteArray;
Q_CORE_EXPORT QByteArray qgetenv(const char *varName);
Q_CORE_EXPORT bool qputenv(const char *varName, const QByteArray& value);

inline int qIntCast(double f) { return int(f); }
inline int qIntCast(float f) { return int(f); }

/*
  Reentrant versions of basic rand() functions for random number generation
*/
Q_CORE_EXPORT void qsrand(uint seed);
Q_CORE_EXPORT int qrand();

/*
   Compat functions that were generated by configure
*/
#ifdef QT3_SUPPORT
#ifndef QT_PRODUCT_LICENSEE
#  define QT_PRODUCT_LICENSEE QLibraryInfo::licensee()
#endif
#ifndef QT_PRODUCT_LICENSE
#  define QT_PRODUCT_LICENSE QLibraryInfo::licensedProducts()
#endif
QT3_SUPPORT Q_CORE_EXPORT const char *qInstallPath();
QT3_SUPPORT Q_CORE_EXPORT const char *qInstallPathDocs();
QT3_SUPPORT Q_CORE_EXPORT const char *qInstallPathHeaders();
QT3_SUPPORT Q_CORE_EXPORT const char *qInstallPathLibs();
QT3_SUPPORT Q_CORE_EXPORT const char *qInstallPathBins();
QT3_SUPPORT Q_CORE_EXPORT const char *qInstallPathPlugins();
QT3_SUPPORT Q_CORE_EXPORT const char *qInstallPathData();
QT3_SUPPORT Q_CORE_EXPORT const char *qInstallPathTranslations();
QT3_SUPPORT Q_CORE_EXPORT const char *qInstallPathSysconf();
#endif

/*
   This gives us the possibility to check which modules the user can
   use. These are purely compile time checks and will generate no code.
*/

/* Qt modules */
#define QT_MODULE_CORE                 0x00001
#define QT_MODULE_GUI                  0x00002
#define QT_MODULE_NETWORK              0x00004
#define QT_MODULE_OPENGL               0x00008
#define QT_MODULE_SQL                  0x00010
#define QT_MODULE_XML                  0x00020
#define QT_MODULE_QT3SUPPORTLIGHT      0x00040
#define QT_MODULE_QT3SUPPORT           0x00080
#define QT_MODULE_SVG                  0x00100
#define QT_MODULE_ACTIVEQT             0x00200
#define QT_MODULE_GRAPHICSVIEW         0x00400
#define QT_MODULE_SCRIPT               0x00800
#define QT_MODULE_XMLPATTERNS          0x01000
#define QT_MODULE_HELP                 0x02000
#define QT_MODULE_TEST                 0x04000
#define QT_MODULE_DBUS                 0x08000
#define QT_MODULE_SCRIPTTOOLS          0x10000

/* Qt editions */
#define QT_EDITION_CONSOLE      (QT_MODULE_CORE \
                                 | QT_MODULE_NETWORK \
                                 | QT_MODULE_SQL \
                                 | QT_MODULE_SCRIPT \
                                 | QT_MODULE_XML \
                                 | QT_MODULE_XMLPATTERNS \
                                 | QT_MODULE_TEST \
                                 | QT_MODULE_DBUS)
#define QT_EDITION_DESKTOPLIGHT (QT_MODULE_CORE \
                                 | QT_MODULE_GUI \
                                 | QT_MODULE_QT3SUPPORTLIGHT \
                                 | QT_MODULE_TEST \
                                 | QT_MODULE_DBUS)
#define QT_EDITION_OPENSOURCE   (QT_MODULE_CORE \
                                 | QT_MODULE_GUI \
                                 | QT_MODULE_NETWORK \
                                 | QT_MODULE_OPENGL \
                                 | QT_MODULE_SQL \
                                 | QT_MODULE_XML \
                                 | QT_MODULE_XMLPATTERNS \
                                 | QT_MODULE_SCRIPT \
                                 | QT_MODULE_SCRIPTTOOLS \
                                 | QT_MODULE_QT3SUPPORTLIGHT \
                                 | QT_MODULE_QT3SUPPORT \
                                 | QT_MODULE_SVG \
                                 | QT_MODULE_GRAPHICSVIEW \
                                 | QT_MODULE_HELP \
                                 | QT_MODULE_TEST \
                                 | QT_MODULE_DBUS)
#define QT_EDITION_DESKTOP      (QT_EDITION_OPENSOURCE \
                                 | QT_MODULE_ACTIVEQT)
#define QT_EDITION_UNIVERSAL    QT_EDITION_DESKTOP
#define QT_EDITION_ACADEMIC     QT_EDITION_DESKTOP
#define QT_EDITION_EDUCATIONAL  QT_EDITION_DESKTOP
#define QT_EDITION_EVALUATION   QT_EDITION_DESKTOP

/* Determine which modules can be used */
#ifndef QT_EDITION
#  ifdef QT_BUILD_QMAKE
#    define QT_EDITION QT_EDITION_DESKTOP
#  else
#    error "Qt not configured correctly, please run configure"
#  endif
#endif

#define QT_LICENSED_MODULE(x) \
    enum QtValidLicenseFor##x##Module { Licensed##x = true };

#if (QT_EDITION & QT_MODULE_CORE)
QT_LICENSED_MODULE(Core)
#endif
#if (QT_EDITION & QT_MODULE_GUI)
QT_LICENSED_MODULE(Gui)
#endif
#if (QT_EDITION & QT_MODULE_NETWORK)
QT_LICENSED_MODULE(Network)
#endif
#if (QT_EDITION & QT_MODULE_OPENGL)
QT_LICENSED_MODULE(OpenGL)
#endif
#if (QT_EDITION & QT_MODULE_SQL)
QT_LICENSED_MODULE(Sql)
#endif
#if (QT_EDITION & QT_MODULE_XML)
QT_LICENSED_MODULE(Xml)
#endif
#if (QT_EDITION & QT_MODULE_XMLPATTERNS)
QT_LICENSED_MODULE(XmlPatterns)
#endif
#if (QT_EDITION & QT_MODULE_HELP)
QT_LICENSED_MODULE(Help)
#endif
#if (QT_EDITION & QT_MODULE_SCRIPT) || defined(QT_BUILD_QMAKE)
QT_LICENSED_MODULE(Script)
#endif
#if (QT_EDITION & QT_MODULE_SCRIPTTOOLS)
QT_LICENSED_MODULE(ScriptTools)
#endif
#if (QT_EDITION & QT_MODULE_QT3SUPPORTLIGHT)
QT_LICENSED_MODULE(Qt3SupportLight)
#endif
#if (QT_EDITION & QT_MODULE_QT3SUPPORT)
QT_LICENSED_MODULE(Qt3Support)
#endif
#if (QT_EDITION & QT_MODULE_SVG)
QT_LICENSED_MODULE(Svg)
#endif
#if (QT_EDITION & QT_MODULE_ACTIVEQT)
QT_LICENSED_MODULE(ActiveQt)
#endif
#if (QT_EDITION & QT_MODULE_TEST)
QT_LICENSED_MODULE(Test)
#endif
#if (QT_EDITION & QT_MODULE_DBUS)
QT_LICENSED_MODULE(DBus)
#endif

#define QT_MODULE(x) \
    typedef QtValidLicenseFor##x##Module Qt##x##Module;

#ifdef QT_NO_CONCURRENT
#  define QT_NO_QFUTURE
#endif

/*
    Turn off certain features for compilers that have problems parsing
    the code.
*/
#if (defined(Q_CC_HPACC) && defined(QT_ARCH_PARISC)) \
    || defined(Q_CC_MIPS) \
    || defined(Q_CC_XLC)
// HP aCC A.03.*, MIPSpro, and xlC cannot handle
// the template function declarations for the QtConcurrent functions
#  define QT_NO_QFUTURE
#  define QT_NO_CONCURRENT
#endif

// MSVC 6.0, MSVC .NET 2002, and old versions of Sun CC can`t handle the map(), etc templates,
// but the QFuture class compiles.
#if (defined(Q_CC_MSVC) && _MSC_VER <= 1300) || (defined (__SUNPRO_CC) && __SUNPRO_CC <= 0x590)
#  define QT_NO_CONCURRENT
#endif

// Mingw uses a gcc 3 version which has problems with some of the
// map/filter overloads. So does IRIX and Solaris.
#if (defined(Q_OS_IRIX) || defined(Q_CC_MINGW) || defined (Q_OS_SOLARIS)) && (__GNUC__ < 4)
#  define QT_NO_CONCURRENT_MAP
#  define QT_NO_CONCURRENT_FILTER
#endif

QT_END_NAMESPACE
QT_END_HEADER

#endif /* __cplusplus */

#endif /* QGLOBAL_H */
