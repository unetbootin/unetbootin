/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#if !defined(lucene_compiler_h)
#define lucene_compiler_h

#if defined(_MBCS) || defined(_ASCII)
#undef _ASCII
#undef _UCS2
#define _ASCII
#elif defined(_UNICODE)
#define _UCS2
#elif !defined(_UCS2)
#define _UCS2
#endif

//msvc needs unicode define so that it uses unicode library
#ifdef _UCS2
#undef _UNICODE
#define _UNICODE
#undef _ASCII
#else
#undef _UNICODE
#undef _UCS2
#endif


////////////////////////////////////////////////////////////////////
//  Figure out what compiler we are using
////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && !defined(__MWERKS__) && !defined (__COMO__) 
#define _CLCOMPILER_MSVC _MSC_VER
#endif

#if defined(__GNUC__) || defined(__SUNPRO_CC) || defined(__xlC__) || defined(__sgi) && defined(__EDG__)
#include "CLucene/config/CompilerGcc.h"

#elif defined(_CLCOMPILER_MSVC)
/* Microsoft Visual C++ */
#include "CLucene/config/CompilerMsvc.h"

#elif defined (__BORLANDC__)
#include "CLucene/config/CompilerBcb.h"

#elif defined (__HP_aCC)
#include "CLucene/config/CompilerAcc.h"

#else
    //Unable to identify the compiler, issue error diagnostic. 
    //Edit <CLucene/config/LuceneMycomp.h> to set STLport up for your compiler.
    //Uncomment this next line
#error "Unable to identify the compiler, issue error diagnostic. Edit <CLucene/config/CompilerMycomp.h> to set Lucene up for your compiler."
#include "CLucene/config/LuceneMycomp.h"
#endif /* end of compiler choice */
////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////
//   Now include platform specific definitions
////////////////////////////////////////////////////////////////////

/* Operating system recognition (basic) */
#if defined (__unix) || defined (__linux__) || defined (__QNX__) || defined (_AIX) || defined (__NetBSD__) || defined (__OpenBSD__) || defined (__Lynx__) || defined(hpux) || defined(__hpux)
#undef  _UNIX
#define _UNIX 1
#include "CLucene/config/PlatformUnix.h"

#elif defined(macintosh) || defined (_MAC) || defined(__APPLE__)
#undef _MAC
#define _MAC  1
#include "CLucene/config/PlatformMac.h"

#elif defined (_WIN32) || defined (__WIN32) || defined (WIN32) || defined (__WIN32__)
#undef _WIN32
#define _WIN32
#include "CLucene/config/PlatformWin32.h"

#elif defined (__WIN16) || defined (WIN16) || defined (_WIN16)
#undef _WIN16
#define _WIN16
#error "CLucene has not been tested on this platform. Please send a report to the lucene administrators if you are able to successfully compile"
#else
#error "CLucene could not identify the platform."
#endif /* platforms */



////////////////////////////////////////////////////////////////////
//   Now we take all that we have learnt, and define some things
////////////////////////////////////////////////////////////////////

//lets just say that we can always do unicode! :)
#ifdef CL_CHARSET_GUESS
#define _UCS2
#endif

#if defined(_ASCII)
#undef _UCS2
#elif defined(_UCS2)
#undef _ASCII
#endif

#ifndef  _LUCENE_NO_NEW_STYLE_CASTS
#define __CONST_CAST(typ,var) const_cast<typ>(var)
#define __REINTERPRET_CAST(typ,var) reinterpret_cast<typ>(var)
#else
#define __CONST_CAST(typ,var) ((typ)(var))
#define __REINTERPRET_CAST,var) ((typ)(var))
#endif

#ifndef _CL_DEPRECATE_TEXT
#define _CL_DEPRECATE_TEXT(_Text)
#endif
#define _CL_DEPRECATED(_NewItem) _CL_DEPRECATE_TEXT("This function or variable has been superceded by newer library or operating system functionality. Consider using" #_NewItem "instead. See online help for details.")


//cnd-debug exit command
#ifndef debugFatalExit
#define debugFatalExit(ret) exit(ret)
#endif

#ifndef _CL_ILONG
#define _CL_ILONG(x)       x ## L
#endif
#ifndef _ILONGLONG
#define _ILONGLONG(x)   x ## LL
#endif

//define whats the values of item intergers *should* be. we can check this in a test
#define LUCENE_INT64_MAX_SHOULDBE _ILONGLONG(0x7FFFFFFFFFFFFFFF)
#define LUCENE_INT32_MAX_SHOULDBE 0x7FFFFFFFL
#define LUCENE_UINT8_MAX_SHOULDBE 0xff

//maximum path length. only used for buffers that use fullpath.
//anything else should use a dynamic length.
#if defined(CL_MAX_PATH)
//do nothing...
#elif defined(PATH_MAX)
#define CL_MAX_PATH PATH_MAX
#elif defined(MAX_PATH)
#define CL_MAX_PATH MAX_PATH
#elif defined(_MAX_PATH)
#define CL_MAX_PATH _MAX_PATH
#else
 #error "CL_MAX_PATH could not be determined"
#endif

//this is the max filename... for now its just the same,
//but this could change, so we use a different name
#define CL_MAX_NAME CL_MAX_PATH
//this used to be CL_MAX_NAME * 32, but as Alex Hudson points out, this could come to be 128kb.
//the above logic for CL_MAX_NAME should be correct enough to handle all file names
#define CL_MAX_DIR CL_MAX_PATH

#ifdef _LARGE_FILES
#define LUCENE_MAX_FILELENGTH LUCENE_INT64_MAX_SHOULDBE
#else
#define LUCENE_MAX_FILELENGTH LUCENE_INT32_MAX_SHOULDBE
#endif

//use the LUCENE_STATIC_CONSTANT_SYNTAX to determine LUCENE_STATIC_CONSTANT
#ifndef LUCENE_STATIC_CONSTANT
	//autoconf is not properly detecting the correct method for this, and since there's no real big
	//harm in always using an enum, we'll probably just make this the default.
	/*#if LUCENE_STATIC_CONSTANT_SYNTAX == 1
		#define LUCENE_STATIC_CONSTANT(type, assignment) static const type assignment
	#elif LUCENE_STATIC_CONSTANT_SYNTAX == 2*/
		#define LUCENE_STATIC_CONSTANT(type, assignment) enum { assignment }
	/*#else
		#error "LUCENE_STATIC_CONSTANT not defined, and/or LUCENE_STATIC_CONSTANT_SYNTAX is not defined to a valid value"
	#endif*/
#endif

//end of lucene_compiler_h
#elif !defined(lucene_compiler_h2)
#define lucene_compiler_h2
//here we include the compiler header again, this gives the header a
//second chance at including stuff, after the main inclusions are complete

#if defined (__GNUC__) || defined(__SUNPRO_CC) || defined(__xlC__) || defined(__sgi)  && defined(__EDG__)
#include "CLucene/config/CompilerGcc.h"
    
#elif defined(_CLCOMPILER_MSVC)
/* Microsoft Visual C++ */
#include "CLucene/config/CompilerMsvc.h"
        
#elif defined __BORLANDC__
#include "CLucene/config/CompilerBcb.h"

#elif defined (__HP_aCC)
#include "CLucene/config/CompilerAcc.h"
        
#else
//Unable to identify the compiler, issue error diagnostic. 
//Edit <CLucene/config/LuceneMycomp.h> to set STLport up for your compiler.
//Uncomment this next line
#error "Unable to identify the compiler, issue error diagnostic. Edit <CLucene/config/CompilerMycomp.h> to set Lucene up for your compiler."
#include "CLucene/config/LuceneMycomp.h"
#endif /* end of compiler choice */

#ifndef _CL_HAVE_FLOAT_T
//#ifdef _CL_HAVE_LONG_DOUBLE
// long double's are not working (reported by Mark Ashworth on Solaris 64)
//   typedef long double float_t;    /* `float' expressions are evaluated as `long double'.  */
//#else
//  we are going to use qreal now
//  typedef double float_t;
//#endif
#endif

/*todo: but need to define SIZEOF_VOID_P #if (SIZEOF_VOID_P > 4 && SIZEOF_VOID_P <= 8)
#ifndef _CL_HAVE_INTPTR_T
     typedef int64_t intptr_t;
#endif
#elif (SIZEOF_VOID_P > 2 && SIZEOF_VOID_P <= 4)
#    ifndef _CL_HAVE_INTPTR_T
        typedef int32_t intptr_t;
#    endif
#else
#error "void * is either >8 bytes or <= 2.  In either case, I am confused."
#endif*/

#ifndef _CL_HAVE_INTPTR_T
    typedef int intptr_t;
#endif
    
//end of lucene_compiler_h2
#elif !defined(lucene_compiler_h3)
#define lucene_compiler_h3
//here we include the compiler header again, this gives the header a
//third chance at including stuff, after the main inclusions are complete
 
#if defined (__GNUC__ ) || defined(__SUNPRO_CC) || defined(__xlC__) || defined(__sgi) && defined(__EDG__)
#include "CLucene/config/CompilerGcc.h"
    
#elif defined(_CLCOMPILER_MSVC)
/* Microsoft Visual C++ */
#include "CLucene/config/CompilerMsvc.h"

#elif defined __BORLANDC__
#include "CLucene/config/CompilerBcb.h"
        
#elif defined (__HP_aCC)
#include "CLucene/config/CompilerAcc.h"

#else
//Unable to identify the compiler, issue error diagnostic. 
//Edit <CLucene/config/LuceneMycomp.h> to set STLport up for your compiler.
//Uncomment this next line
#error "Unable to identify the compiler, issue error diagnostic. Edit <CLucene/config/CompilerMycomp.h> to set Lucene up for your compiler."
#include "CLucene/config/LuceneMycomp.h"
#endif /* end of compiler choice */
   
#endif
