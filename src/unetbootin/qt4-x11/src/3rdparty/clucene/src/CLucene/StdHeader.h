/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
*
* Distributable under the terms of either the Apache License (Version 2.0) or
* the GNU Lesser General Public License, as specified in the COPYING file.
*
* Changes are Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
------------------------------------------------------------------------------*/
#ifndef lucene_stdheader_h
#define lucene_stdheader_h

#if defined(OVERRIDE_DEFAULT_CLCONFIG)
 #include "AltCLConfig.h"
#else
 #include "CLucene/CLConfig.h"
#endif

//first inclusion of compiler.h (it will be called again later)
#include "CLucene/config/compiler.h"

extern void _lucene_shutdown();
extern int _lucene_counter_break; //can set a watch on this
#if defined(LUCENE_ENABLE_MEMLEAKTRACKING)
	extern bool _lucene_disable_debuglogging; //if LUCENE_ENABLE_CONSTRUCTOR_LOG is on, dont do log if this is true
#endif

////////////////////////////////////////////////////////
//   default includes
////////////////////////////////////////////////////////
#ifndef LUCENE_DISABLE_INCLUDES

#include <stdio.h>

#if defined(_CL_STDC_HEADERS)
 #include <stdlib.h>
 #include <stddef.h>
#else
 #if defined(_CL_HAVE_STDLIB_H)
  #include <stdlib.h>
 #endif
#endif

#if defined(_CL_HAVE_STRING_H)
 #if !defined(_CL_STDC_HEADERS) && defined(_CL_HAVE_MEMORY_H)
  #include <memory.h>
 #endif
 #include <string.h>
#elif defined(_CL_HAVE_STRINGS_H)
 //note: as a side note, strtok is not thread-safe.. so be careful where you use it!
 #error "strtok replacement for BSD has not been implemented"
 #include <strings.h>
 #if !defined(_CL_HAVE_STRCHR)
  #define strchr index
  #define strrchr rindex
 #endif
#endif

#if defined(_CL_HAVE_UNISTD_H)
 #include <unistd.h>
#elif defined(_CL_HAVE_IO_H) && defined(_CL_HAVE_DIRECT_H)
 #include <io.h>
 #include <direct.h>
#else
 #error "Neither unistd.h or (io.h & direct.h) were available"
#endif

#ifndef _CL_DISABLE_NATIVE_EXCEPTIONS
 #ifdef _CL_HAVE_STDEXCEPT
   #include <stdexcept>
 #else
  #error "CLucene can't compile with exception handling on because <stdexcept> header is not available"
 #endif
#endif

#if defined(_CL_STAT_MACROS_BROKEN)
 #error "Haven't implemented STAT_MACROS_BROKEN fix yet"
#elif defined(_CL_HAVE_SYS_STAT_H)
 #include <sys/stat.h>
#else
 #error "Haven't implemented platforms with no sys/stat.h"
#endif

#if defined(_CL_HAVE_STDARG_H)
 #include <stdarg.h>
#else
 #error "CLucene can compile, but some extras may not work"
#endif

#if defined(_CL_HAVE_MATH_H)
 #include <math.h>
#else
 #error "CLucene can't compile without <math.h>"
#endif

#if defined(_CL_HAVE_MAP)
 #include <map>
#else
 #error "CLucene can't compile without the map header"
#endif

#if defined(_CL_HAVE_LIST)
 #include <list>
#else
 #error "CLucene can't compile without the list header"
#endif

#if defined(_CL_HAVE_SET)
 #include <set>
#else
 #error "CLucene can't compile without the set header"
#endif

#if defined(_CL_HAVE_VECTOR)
 #include <vector>
#else
 #error "CLucene can't compile without the vector header"
#endif

#if !defined(LUCENE_DISABLE_HASHING) && defined(_CL_HAVE_HASH_MAP) && defined(_CL_HAVE_HASH_SET)
	//hashing is all or nothing!
	#include <hash_map>
	#include <hash_set>
#elif !defined(LUCENE_DISABLE_HASHING) && defined(_CL_HAVE_EXT_HASH_MAP) && defined(_CL_HAVE_EXT_HASH_SET)
	#include <ext/hash_map>
	#include <ext/hash_set>
#elif !defined(LUCENE_DISABLE_HASHING)
	#define LUCENE_DISABLE_HASHING
#endif
#if !defined(LUCENE_DISABLE_HASHING) && !defined(CL_NS_HASHING)
	#define CL_NS_HASHING(func) std::func
#endif

#if defined(_CL_HAVE_ALGORITHM)
# include <algorithm>
#else
# error "Can't compile clucene without <algorithm>"
#endif

#if defined(_CL_HAVE_FUNCTIONAL)
# include <functional>
#else
# error "Can't compile clucene without <functional>"
#endif

#if !defined(_CL_HAVE_PRINTF)
 #error "CLucene can't compile without printf, replacements have not been implemented"
#endif

#if !defined(_CL_HAVE_SNPRINTF) && !defined(_CL_HAVE__SNPRINTF)
 #error "CLucene can't compile without snprintf, replacements have not been implemented"
#elif !defined(_CL_HAVE__SNPRINTF)&& defined(_CL_HAVE_SVNPRINTF)
 #define _snprintf snprintf
#endif

#if defined(_UCS2)
 #if defined(_CL_HAVE_WCHAR_H)
    #include <wchar.h>
 #else
	//actually the repl_wchar.h replacements header will
	//always be included. It replaces some functions
	//that are missing in some wchar.h headers.
 #endif
#endif

#if defined(_UCS2) && defined(_CL_HAVE_WCTYPE_H)
 #include <wctype.h>
#elif defined(_ASCII) && defined(_CL_HAVE_CTYPE_H)
 #include <ctype.h>
 #undef  LUCENE_USE_INTERNAL_CHAR_FUNCTIONS
#elif defined(_UCS2)
 //must be in _UCS2 to use internal char functions
 #undef  LUCENE_USE_INTERNAL_CHAR_FUNCTIONS
 #define LUCENE_USE_INTERNAL_CHAR_FUNCTIONS
#else
 #error "Cannot compile in _ASCII without ctype.h"
#endif

//always include replacement, some missing tchar defines
#include "CLucene/config/repl_tchar.h"

#if defined(_CL_HAVE_ERRNO_H)
 #include <errno.h>
#else
 #error "Haven't implemented platforms with no errno.h"
#endif

#if defined(_CL_HAVE_FCNTL_H)
 #include <fcntl.h>
#else
 #error "Haven't implemented platforms with no fcntl.h"
#endif

#if defined(_CL_HAVE_WINDOWS_H)
 #include <windows.h>
#endif

#endif //LUCENE_DISABLE_INCLUDES
//
////////////////////////////////////////////////////////

//second inclusion of compiler.h
//this gives CompilerXXX.h a chance to include other headers
#include "CLucene/config/compiler.h"
//
////////////////////////////////////////////////////////


////////////////////////////////////////////////////////
// Character functions.
// Here we decide whose character functions to use
////////////////////////////////////////////////////////
#if defined(LUCENE_USE_INTERNAL_CHAR_FUNCTIONS)
    #define stringCaseFold cl_tcscasefold
	#define stringCaseFoldCmp cl_tcscasefoldcmp

	#undef _istspace
	#undef _istdigit
	#undef _istalnum
	#undef _istalpha
	#undef _totlower
	#undef _totupper
    #define _istalnum cl_isalnum
    #define _istalpha cl_isletter
    #define _istspace cl_isspace
    #define _istdigit cl_isdigit
    #define _totlower cl_tolower
    #define _totupper cl_toupper

    //here are some functions to help deal with utf8/ucs2 conversions
    //lets let the user decide what mb functions to use... we provide pure utf8 ones no matter what.
    /*#undef _mbtowc
    #undef _mbstowcs
    #undef _wctomb
    #undef _wcstombs
    #define _mbtowc lucene_mbstowc
    #define _mbsstowcs lucene_mbstowcs
    #define _wctomb lucene_wcto_mb
    #define _wcstombs lucene_wcstombs*/
#else
    //we are using native functions
	//here are some functions to help deal with utf8/ucs2 conversions
	/*#define _mbtowc mbtowc
	#define _wctomb wctomb
	#define _mbstowcs mbstowcs
	#define _wcstombs wcstombs*/

    //we are using native character functions
    #if defined(_ASCII)
        #undef _istspace
        #undef _istdigit
        #undef _istalnum
        #undef _istalpha
        #undef _totlower
        #undef _totupper
        #define _istspace(x) isspace((unsigned char)x)
        #define _istdigit(x) isdigit((unsigned char)x)
        #define _istalnum(x) isalnum((unsigned char)x)
        #define _istalpha(x) isalpha((unsigned char)x)
        #define _totlower(x) tolower((unsigned char)x)
        #define _totupper(x) toupper((unsigned char)x)
    #endif
#endif

//the methods contained in gunichartables.h
typedef unsigned long  clunichar;
bool cl_isletter(clunichar c);
bool cl_isalnum(clunichar c);
bool cl_isdigit(clunichar c);
bool cl_isspace (clunichar c);
TCHAR cl_tolower (TCHAR c);
TCHAR cl_toupper (TCHAR c);

int cl_tcscasefoldcmp(const TCHAR * dst, const TCHAR * src);
TCHAR* cl_tcscasefold( TCHAR * str, int len=-1 );

//we provide utf8 conversion functions
size_t lucene_utf8towc  (wchar_t *ret, const char *s, size_t n);
size_t lucene_utf8towcs(wchar_t *,    const char *,  size_t maxslen);
size_t lucene_wctoutf8  (char * ret,   const wchar_t  str);
size_t lucene_wcstoutf8 (char *,       const wchar_t *, size_t maxslen);
size_t lucene_utf8charlen(const char *p);

///a blank string...
extern TCHAR* _LUCENE_BLANK_STRING;
#define LUCENE_BLANK_STRING _LUCENE_BLANK_STRING
extern char* _LUCENE_BLANK_ASTRING;
#define LUCENE_BLANK_ASTRING _LUCENE_BLANK_ASTRING

/* Converts a string into a form that is independent of case. The
 * result will not correspond to any particular case, but can be
 * compared for equality or ordered with the results of calling
 * stringCaseFold() on other strings.
 *
 * If we did not define this elsewhere, then just convert to lower case
 */
#ifndef stringCaseFold
    #define stringCaseFold _tcslwr
#endif
/* Compares 2 strings using case folding (if available)
 * If we did not define this elsewhere, then just compare
 * using normal method
 */
#ifndef stringCaseFoldCmp
    #define stringCaseFoldCmp _tcsicmp
#endif

//now that all the character routines are completed, include the
//wchar.h replacements.
#include "CLucene/config/repl_wchar.h" //always include replacements

//a replacement for _tcsdup. This uses new TCHAR[] instead of malloc, so that we can use delete[] to free
#if defined(LUCENE_ENABLE_FILELINEINFO)
	#define CL_FILELINE ,__FILE__,__LINE__
	#define CL_FILELINEREF ,file,line ///<for passing the reference along to another function
	#define CL_FILELINEREF2 ,file,line ///<for passing the reference along to another function
	#define CL_FILELINEPARAM ,char* file,int line
#else
	#define CL_FILELINE
	#define CL_FILELINEREF
	#define CL_FILELINEREF2 ,NULL,-1
	#define CL_FILELINEPARAM
#endif

char* lucenestrdup(const char* v CL_FILELINEPARAM);
#if defined(_UCS2)
    wchar_t* lucenewcsdup(const wchar_t* v CL_FILELINEPARAM);
    #define stringDuplicate(x) lucenewcsdup(x CL_FILELINE) //don't change this... uses [] instead of malloc
#else
    #define stringDuplicate(x) lucenestrdup(x CL_FILELINE) //don't change this... uses [] instead of malloc
#endif

#define STRCPY_AtoA(target,src,len) strncpy(target,src,len)
#define STRDUP_AtoA(x) lucenestrdup(x CL_FILELINE)

#if defined(_UCS2)
	#define STRDUP_WtoW(x) lucenewcsdup(x CL_FILELINE)
    #define STRDUP_TtoT STRDUP_WtoW
    #define STRDUP_WtoT STRDUP_WtoW
    #define STRDUP_TtoW STRDUP_WtoW

    #define STRDUP_AtoW(x) CL_NS(util)::Misc::_charToWide(x CL_FILELINE)
	#define STRDUP_AtoT STRDUP_AtoW

    #define STRDUP_WtoA(x) CL_NS(util)::Misc::_wideToChar(x CL_FILELINE)
	#define STRDUP_TtoA STRDUP_WtoA

    #define STRCPY_WtoW(target,src,len) _tcsncpy(target,src,len)
	#define STRCPY_TtoW STRCPY_WtoW
	#define STRCPY_WtoT STRCPY_WtoW
	#define STRCPY_TtoT STRCPY_WtoW

    #define STRCPY_AtoW(target,src,len) CL_NS(util)::Misc::_cpycharToWide(src,target,len)
    #define STRCPY_AtoT STRCPY_AtoW

    #define STRCPY_WtoA(target,src,len) CL_NS(util)::Misc::_cpywideToChar(src,target,len)
    #define STRCPY_TtoA STRCPY_WtoA
#else
    #define STRDUP_AtoT STRDUP_AtoA
    #define STRDUP_TtoA STRDUP_AtoA
    #define STRDUP_TtoT STRDUP_AtoA

    #define STRDUP_WtoT(x) xxxxxxxxxxxxxxx //not possible
    #define STRCPY_WtoT(target,src,len) xxxxxxxxxxxxxxx //not possible

    #define STRCPY_AtoT STRCPY_AtoA
    #define STRCPY_TtoA STRCPY_AtoA
    #define STRCPY_TtoT STRCPY_AtoA
#endif

//
////////////////////////////////////////////////////////

////////////////////////////////////////////////////////
//namespace helper
////////////////////////////////////////////////////////
#if defined(_LUCENE_DONTIMPLEMENT_NS_MACROS)

#elif !defined(DISABLE_NAMESPACE)
//
//  W A R N I N G
//  -------------
//
// adjustments here, need to be done in
// QTDIR/src/tools/assistant/lib/fulltextsearch/qclucene_global.h as well
//
#   ifdef QT_NAMESPACE
#       define CL_NS_DEF(sub) namespace QT_NAMESPACE { namespace lucene{ namespace sub{
#       define CL_NS_DEF2(sub,sub2) namespace QT_NAMESPACE { namespace lucene{ namespace sub{ namespace sub2 {

#       define CL_NS_END }}}
#       define CL_NS_END2 }}}}

#       define CL_NS_USE(sub) using namespace QT_NAMESPACE::lucene::sub;
#       define CL_NS_USE2(sub,sub2) using namespace QT_NAMESPACE::lucene::sub::sub2;

#       define CL_NS(sub) QT_NAMESPACE::lucene::sub
#       define CL_NS2(sub,sub2) QT_NAMESPACE::lucene::sub::sub2
#   else
#       define CL_NS_DEF(sub) namespace lucene{ namespace sub{
#       define CL_NS_DEF2(sub,sub2) namespace lucene{ namespace sub{ namespace sub2 {

#       define CL_NS_END }}
#       define CL_NS_END2 }}}

#       define CL_NS_USE(sub) using namespace lucene::sub;
#       define CL_NS_USE2(sub,sub2) using namespace lucene::sub::sub2;

#       define CL_NS(sub) lucene::sub
#       define CL_NS2(sub,sub2) lucene::sub::sub2
#   endif
#else
#   define CL_NS_DEF(sub)
#   define CL_NS_DEF2(sub, sub2)
#   define CL_NS_END
#   define CL_NS_END2
#   define CL_NS_USE(sub)
#   define CL_NS_USE2(sub,sub2)
#   define CL_NS(sub)
#   define CL_NS2(sub,sub2)
#endif

#if defined(LUCENE_NO_STDC_NAMESPACE)
   //todo: haven't actually tested this on a non-stdc compliant compiler
   #define CL_NS_STD(func) ::func
#else
   #define CL_NS_STD(func) std::func
#endif
//
////////////////////////////////////////////////////////

//
void CLDebugBreak(); //define a debugbreak function


////////////////////////////////////////////////////////////////
//   These are defines and functions used throughout clucene
////////////////////////////////////////////////////////////////
#undef _T //remove any previously defined _T - required for ppc os
#if defined(_UCS2)
  #define _T(x)       L ## x
#else
  #define _T(x)       x
#endif

//third inclusion of compiler.h
//this gives CompilerXXX.h a chance to fix any of the
//default settings
#include "CLucene/config/compiler.h"

#if defined _MSC_VER && (_MSC_VER < 1300)
# define LUCENE_NO_STDC_NAMESPACE
#endif

//use std namespace
#ifndef LUCENE_NO_STDC_NAMESPACE
using namespace std;
#endif


////////////////////////////////////////////////////////
//misc shortcut defines
////////////////////////////////////////////////////////

//include the headers that we need practically everywhere
#include "CLucene/debug/error.h" //all delete/creation/mem debugging code
#include "CLucene/debug/condition.h" //conditional debugging (like assert)
#include "CLucene/debug/mem.h" //all delete/creation/mem debugging code
#include "LuceneThreads.h" //lucene threading support

#include "CLucene/util/Misc.h"
#include "CLucene/util/Equators.h"
#include "CLucene/util/VoidList.h"
#include "CLucene/util/VoidMap.h"


CL_NS_DEF(util)
typedef CL_NS(util)::CLVector<TCHAR*> StringArray;
typedef CL_NS(util)::CLVector<TCHAR*, CL_NS(util)::Deletor::tcArray > StringArrayWithDeletor;
typedef CL_NS(util)::CLVector<const TCHAR*> StringArrayConst;
typedef CL_NS(util)::CLVector<const TCHAR*, CL_NS(util)::Deletor::tcArray > StringArrayConstWithDeletor;

typedef CL_NS(util)::CLVector<char*> AStringArray;
typedef CL_NS(util)::CLVector<char*, CL_NS(util)::Deletor::acArray > AStringArrayWithDeletor;
typedef CL_NS(util)::CLVector<const char*> AStringArrayConst;
typedef CL_NS(util)::CLVector<const char*, CL_NS(util)::Deletor::acArray > AStringArrayConstWithDeletor;
CL_NS_END

//
////////////////////////////////////////////////////////

#endif // STDHEADER_H
