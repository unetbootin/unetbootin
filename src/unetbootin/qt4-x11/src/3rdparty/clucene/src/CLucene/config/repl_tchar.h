/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#ifndef _CL_HAVE_TCHAR_H
#if defined(_UCS2)
    #define TCHAR wchar_t
    
    //note: descriptions with * in front have replacement functions
    
    //formatting functions
    #define _sntprintf swprintf //* make a formatted a string
    #define _tprintf wprintf //* print a formatted string
    
    //this one has no replacement functions yet, but it is only used in the tests
    #define _vsntprintf vsnwprintf //* print a formatted string using variable arguments
    
    //we are using the internal functions of the compiler here
    //if LUCENE_USE_INTERNAL_CHAR_FUNCTIONS is defined, thesse
    //will be replaced by internal functions
    #define _istalnum iswalnum //* alpha/numeric char check
    #define _istalpha iswalpha //* alpha char check
    #define _istspace iswspace //* space char check
    #define _istdigit iswdigit //* digit char check
    #define _totlower towlower //* convert char to lower case
    #define _totupper towupper //* convert char to lower case
    #define _tcslwr wcslwr //* convert string to lower case
    
    //these are the string handling functions
    //we may need to create wide-character/multi-byte replacements for these
    #define _tcscpy wcscpy //copy a string to another string
    #define _tcsncpy wcsncpy //copy a specified amount of one string to another string.
    #define _tcscat wcscat //copy a string onto the end of the other string
    #define _tcschr wcschr //find location of one character
    #define _tcsstr wcsstr //find location of a string
    #define _tcslen wcslen //get length of a string
    #define _tcscmp wcscmp //case sensitive compare two strings
    #define _tcsncmp wcsncmp //case sensitive compare two strings
    #define _tcscspn wcscspn //location of any of a set of character in a string
    
    #ifdef _CL_HAVE_WCSICMP
        #define _tcsicmp wcsicmp //* case insensitive compare two string
    #else
        #define _tcsicmp wcscasecmp //* case insensitive compare two string
    #endif
    
    //conversion functions
    #define _tcstod wcstod //convert a string to a double
    #ifdef _PA_RISC
        #define _tcstoi64 __wcstoll //* convers a string to an 64bit bit integer
    #else
        #define _tcstoi64 wcstoll //* convers a string to an 64bit bit integer
    #endif
    #define _i64tot lltow //* converts a 64 bit integer to a string (with base)
    
#else //if defined(_ASCII)
    #define TCHAR char
    
    //formatting functions
    #define _sntprintf snprintf    
    #define _tprintf printf
    #define _vsntprintf vsnprintf 
    
    //we are using the internal functions of the compiler here
    //if LUCENE_USE_INTERNAL_CHAR_FUNCTIONS is defined, thesse
    //will be replaced by internal functions
    #define _istalnum isalnum
    #define _istalpha isalpha
    #define _istspace isspace
    #define _istdigit isdigit
    #define _totlower tolower
    #define _totupper toupper
    #define _tcslwr strlwr
    
    //these are the string handling functions
    #define _tcscpy strcpy
    #define _tcsncpy strncpy
    #define _tcscat strcat
    #define _tcschr strchr
    #define _tcsstr strstr
    #define _tcslen strlen
    #define _tcscmp strcmp
    #define _tcsncmp strncmp
    #define _tcsicmp strcasecmp
    #define _tcscspn strcspn
    
    //converstion methods
    #define _tcstod strtod
    #define _tcstoi64 strtoll
    #define _i64tot lltoa
#endif
#else //HAVE_TCHAR_H
 #include <tchar.h>
 
 //some tchar headers miss these...
 #ifndef _tcstoi64
 	#if defined(_UCS2)
		#define _tcstoi64 wcstoll //* convers a string to an 64bit bit integer
	#else
		#define _tcstoi64 strtoll
	#endif
 #endif
 
#endif //HAVE_TCHAR_H
