/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#ifndef _lucene_repl_wchar_h
#define _lucene_repl_wchar_h

#ifdef _UCS2

#ifndef _CL_HAVE_WCSCPY
	//copy a string to another string
	#error wcscpy is not defined, and a licensed replacement has not been written yet
#endif

#ifndef _CL_HAVE_WCSNCPY
	//copy a specified amount of one string to another string.
	#error wcsncpy is not defined, and a licensed replacement has not been written yet
#endif

#ifndef _CL_HAVE_WCSCAT
	//copy a string onto the end of the other string
	#error wcscat is not defined, and a licensed replacement has not been written yet
#endif

#ifndef _CL_HAVE_WCSCHR
	//find location of one character
	#error wcschr is not defined, and a licensed replacement has not been written yet
#endif

#ifndef _CL_HAVE_WCSSTR
	//find location of a string
	#error wcspy is not defined, and a licensed replacement has not been written yet
#endif

#ifndef _CL_HAVE_WCSLEN
	//get length of a string
	#error wcslen is not defined, and a licensed replacement has not been written yet
#endif

#ifndef _CL_HAVE_WCSCMP
	//case sensitive compare two strings
	#error wcscmp is not defined, and a licensed replacement has not been written yet
#endif

#ifndef _CL_HAVE_WCSNCMP
	//case sensitive compare two strings of a specified length
	#error wcsncmp is not defined, and a licensed replacement has not been written yet
#endif

#ifndef _CL_HAVE_WCSCSPN
	//Return the length of the maximum initial segment
	//of WCS which contains only wide-characters not in REJECT.
	#error wcscspn is not defined, and a licensed replacement has not been written yet
#endif

#endif //_UCS2

//string function replacements
#if defined(LUCENE_USE_INTERNAL_CHAR_FUNCTIONS) || (defined(_UCS2) && !defined(_CL_HAVE_WCSCASECMP)) || (defined(_ASCII) && !defined(_CL_HAVE_STRCASECMP))
    int lucene_tcscasecmp(const TCHAR *, const TCHAR *);
    #undef _tcsicmp
    #define _tcsicmp lucene_tcscasecmp
#endif
#if defined(LUCENE_USE_INTERNAL_CHAR_FUNCTIONS) || (defined(_UCS2) && !defined(_CL_HAVE_WCSLWR)) || (defined(_ASCII) && !defined(_CL_HAVE_STRLWR))
    TCHAR* lucene_tcslwr( TCHAR* str );
    #undef _tcslwr
    #define _tcslwr lucene_tcslwr
#endif

//conversion functions
#if (defined(_ASCII) && !defined(_CL_HAVE_LLTOA)) || (defined(_UCS2) && !defined(_CL_HAVE_LLTOW))
    TCHAR* lucene_i64tot( int64_t value, TCHAR* str, int radix);
    #undef _i64tot
    #define _i64tot lucene_i64tot
#endif
#if (defined(_UCS2) && !defined(_CL_HAVE_WCSTOLL)) || (defined(_ASCII) && !defined(_CL_HAVE_STRTOLL))
	int64_t lucene_tcstoi64(const TCHAR* str, TCHAR**end, int radix);
    #undef _tcstoi64
    #define _tcstoi64 lucene_tcstoi64
#endif
#if defined(_UCS2) && !defined(_CL_HAVE_WCSTOD)
    double lucene_tcstod(const TCHAR *value, TCHAR **end);
    #undef _tcstod
    #define _tcstod lucene_tcstod
#endif

//printf functions
#if defined(_UCS2) && (!defined(_CL_HAVE_SNWPRINTF) || defined(_CL_HAVE_SWPRINTF_BUG) )
    #undef _sntprintf
    #define _sntprintf lucene_snwprintf
    int lucene_snwprintf(wchar_t* strbuf, size_t count, const wchar_t * format, ...);
    
    #ifndef __CL_INCLUDE_TPRINTF
    	#define __CL_INCLUDE_TPRINTF
    #endif
#endif
#if defined(_UCS2) && !defined(_CL_HAVE_WPRINTF)
    #undef _tprintf
    #define _tprintf lucene_wprintf
    void lucene_wprintf(const wchar_t * format, ...);
    
    #ifndef __CL_INCLUDE_TPRINTF
    	#define __CL_INCLUDE_TPRINTF
    #endif
#endif
#if defined(_UCS2) && (!defined(_CL_HAVE_VSNWPRINTF) || defined(_CL_HAVE_SWPRINTF_BUG) )
    #undef _vsntprintf
    #define _vsntprintf lucene_vsnwprintf
    int lucene_vsnwprintf(wchar_t * strbuf, size_t count, const wchar_t * format, va_list& ap);

    #ifndef __CL_INCLUDE_TPRINTF
    	#define __CL_INCLUDE_TPRINTF
    #endif
#endif

//todo: if _CL_HAVE_SNPRINTF_BUG fails(snprintf overflow),we should use our own
//function. but we don't have it currently, and our functions are dubious anyway...

#endif //end of _lucene_repl_wchar_h
