/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
*
* Changes are Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
------------------------------------------------------------------------------*/
#if !defined(_lucene_COMPILER_ACC)
#define _lucene_COMPILER_ACC

// It is internal CLucene header - DO NOT include it directly
#if !defined(_SUPPRESS_MAKE_BASED_CONFIG)
#if defined(_BUILD_FOR_QT_)
#include "fulltextsearch/qclucene-config_p.h"
#else
#include "CLucene/clucene-config.h" //make clucene-config.h file
#endif
#endif

#if defined(_ASCII)
#undef _UCS2
#elif defined(_UCS2)
//
#else
#define CL_CHARSET_GUESS
#endif

//dont allow FS_MMAP if mmap is not available
#if defined(LUCENE_FS_MMAP) && !defined(_CL_HAVE_MMAP)
#error "LUCENE_FS_MMAP is defined and MMap doesn't appear to be available"
#endif

#ifdef _CL_HAVE_NO_FUNCTION_TRY_BLOCKS
#undef  _LUCENE_DISABLE_EXCEPTIONS
#define _LUCENE_DISABLE_EXCEPTIONS
    
#error "this is bad if you made it here... your compiler seems not to have try/catch blocks."
#error "maybe you could implement an alternative solution for us? :)"
#endif

#ifndef _CL_HAVE_NAMESPACES
#define DISABLE_NAMESPACE
#endif

#define LUCENE_DISABLE_HASHING //we could enable this, but so far test show that the hashing is slower :(

//define the file functions
#define fileSeek lseek
#define fileSize _filelength
#define fileStat stat
#define fileHandleStat fstat
#ifdef _CL_HAVE_TELL
#define fileTell tell
#else
//ftell (and probably soon ftell64) are POSIX standard functions, but tell and
//tell64 are not, so we define fileTell in terms of fileSeek.
#define fileTell(fhandle) fileSeek(fhandle, 0, SEEK_CUR)
#endif

//this is needed early on so that CL_MAX_PATH can be correctly determined
//in the StdHeader.h. This was earlier causing problems with macosx.
//:: crash was due to realpath() that expects an output arguments that
//has at least the size of PATH_MAX (even if the result has a lower size)
#include <limits.h>
    
#ifndef _CL_HAVE_WCHAR_T
    typedef unsigned short wchar_t;
#endif

#if defined(__CYGWIN__)
//cygwin seems to incorrectly define that it has wprintf???
#undef _CL_HAVE_WPRINTF
#elif defined(__MINGW32__)
#    ifndef _CL_HAVE_WINDOWS_H
#    define _CL_HAVE_WINDOWS_H
#    endif
#endif


///////////////////////////////////////////////////////////////////////////////
//end _lucene_COMPILER_ACC
#elif !defined(_lucene_COMPILER_ACC2)
#define _lucene_COMPILER_ACC2
//second inclusion

    //types
    #if defined(_CL_HAVE_SYS_TYPES_H)
        #include <sys/types.h>
    #endif
	#if defined(_CL_HAVE_INTTYPES_H)
		#include <inttypes.h>
	#elif defined(_CL_HAVE_STDINT_H)
		#include <stdint.h>
	#else
		#if _CL_SIZEOF_UNSIGNED_LONG_LONG==8
			typedef unsigned long long uint64_t;
			typedef long long int64_t;
		#elif _CL_SIZEOF_UNSIGNED_LONG==8
			typedef unsigned long uint64_t;
			typedef long int64_t;
		#else
			#error I do not know what to use for a uint64_t.
		#endif
		
		/* Give us an unsigned 32-bit data type. */
		#if _CL_SIZEOF_UNSIGNED_LONG==4
			typedef unsigned long uint32_t;
			typedef long int32_t;
		#elif _CL_SIZEOF_UNSIGNED_INT==4
			typedef unsigned int uint32_t;
			typedef int int32_t;
		#else
			#error I do not know what to use for a uint32_t.
		#endif

		/* An unsigned 8-bit data type */
		#if _CL_SIZEOF_UNSIGNED_CHAR==1
			typedef unsigned char uint8_t;
		#else
			#error I do not know what to use for a uint8_t.
		#endif
	#endif
    
    //second chance to fix default settings
	//this must be defined later, otherwise it messes up
	//the standard libraries
	#if !defined(__MINGW32__)
	    #define _close ::close
	    #define _read ::read
	#endif

  	//now that int64_t is defined, we can define this...
	#ifndef _CL_HAVE_FILELENGTH
	    #undef fileSize
	    #define fileSize lucene_filelength
	    int64_t lucene_filelength(int handle);
	#endif

#elif !defined(_lucene_COMPILER_ACC3)
#define _lucene_COMPILER_ACC3
    //third inclusion

	#if !defined(__MINGW32__)
	    //define replacements
	    #define O_RANDOM 0
	    #undef O_BINARY
	    #define O_BINARY 0
	    #define _S_IREAD  0444
	    #define _S_IWRITE 0333  // write and execute permissions

	    //some functions that are needed - not charset dependent and not tchar type functions
	    #define _open open
	    #define _write write
	    #define _snprintf snprintf

	    //clucene uses ascii for filename interactions
	    #define _realpath(rel,abs) realpath(rel,abs)
	    #define _mkdir(x) mkdir(x,0777)
	    #define _unlink unlink
	#else
	    #define _realpath(rel,abs) _fullpath(abs,rel,CL_MAX_PATH)
	#endif
	//also required by mingw
	#define _rename rename
#endif
