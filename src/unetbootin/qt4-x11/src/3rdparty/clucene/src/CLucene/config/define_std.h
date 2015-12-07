/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#ifndef lucene_define_std
#define lucene_define_std
//define a standard list of defines. 
//These defines represents a fairly complete compiler.
//Of course it is preferable to use the autoconf generated
//list, but then not all systems can do this :)

//we support long files - 64 bit file functions
#define _LARGE_FILES

//support namespaces
#define _CL_HAVE_NAMESPACES

//support try/catch blocks
#define _CL_HAVE_FUNCTION_TRY_BLOCKS

//the normal headers
#define _CL_STDC_HEADERS
#define _CL_HAVE_STDARG_H
#define _CL_HAVE_ALGORITHM
#define _CL_HAVE_FUNCTIONAL
#define _CL_HAVE_MATH_H
#define _CL_HAVE_STL
#define _CL_HAVE_HASH_MAP
#define _CL_HAVE_HASH_SET
#define _CL_HAVE_MAP
#define _CL_HAVE_SET
#define _CL_HAVE_LIST
#define _CL_HAVE_VECTOR
#define _CL_HAVE_STDEXCEPT
#define _CL_HAVE_ERRNO_H
#define _CL_HAVE_SYS_STAT_H
#define _CL_HAVE_FCNTL_H

//character & std tchar support
#define _CL_HAVE_TCHAR_H
#ifdef _UCS2
	#define _CL_HAVE_WCTYPE_H

	#define _CL_HAVE_WCSCPY
	#define _CL_HAVE_WCSNCPY
	#define _CL_HAVE_WCSCAT
	#define _CL_HAVE_WCSCHR
	#define _CL_HAVE_WCSSTR
	#define _CL_HAVE_WCSLEN
	#define _CL_HAVE_WCSCMP
	#define _CL_HAVE_WCSNCMP
	#define _CL_HAVE_WCSCSPN
#else
	#define _CL_HAVE_CTYPE_H
#endif

//already have the normal structures
#define _CL_HAVE_FLOAT_T
#define _CL_HAVE_INTPTR_T

//system dependant:
#define _CL_HAVE_STRING_H //could be HAVE_STRINGS_H && HAVE_STRCHR
#define _CL_HAVE_SYS_TIMEB_H
#define _CL_HAVE_TIME_H

#if defined (_WIN32) || defined (__WIN32) || defined (WIN32) || defined (__WIN32__)
    #define _CL_HAVE_IO_H
    #define _CL_HAVE_DIRECT_H
    #define _CL_HAVE_WINDOWS_H
#else
    #define _CL_HAVE_UNISTD_H
#endif

////////////////////////////////////////////////
//now for individual functions. some compilers
//miss these, so must individually define what
//we have
////////////////////////////////////////////////

//string functions
#define _CL_HAVE_STRLWR
#define _CL_HAVE_WCSLWR
#define _CL_HAVE_WCSCASECMP
#define _CL_HAVE_STRCASECMP

//formatting functions
#define _CL_HAVE_SNWPRINTF
#define _CL_HAVE_VSNWPRINTF
#define _CL_HAVE_WPRINTF
#define _CL_HAVE_SNPRINTF
#define _CL_HAVE_PRINTF


//conversion functions
#define _CL_HAVE_STRTOLL
#define _CL_HAVE_WCSTOLL
#define _CL_HAVE_WCSTOD
#define _CL_HAVE_LLTOA
#define _CL_HAVE_LLTOW
#define _CL_HAVE_INTPTR_T

//these ones are not standard (msvc)
//so you will probably need to undefine
//if you are not using msvc
#define _CL_HAVE_FILELENGTH


#endif
