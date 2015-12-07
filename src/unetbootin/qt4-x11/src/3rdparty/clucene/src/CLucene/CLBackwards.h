/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#ifndef _clucene_backwards_h
#define _clucene_backwards_h

//In light of the recent major changes to clucene,
//this file should help to maintain some backwards compatibility
//include it after including StdHeader.h
//
//Note: I haven't tested this much, so please send me your changes

//dirent is one of the most major changes that won't easily port.
//you can use the TCHAR copying macros, STRCPY_TtoA and STRCPY_AtoT
//to copy between different character types.

//If you use stringPrintF, you will need to add the target string
//size parameter, because it is using _sntprintf... Change this if you
//want, but _sntprintf is much safer

#define char_t TCHAR
#define uchar_t TCHAR
#define l_byte_t byte_t

//#define stringSpn _tcsspn //not used in clucene anymore
#define stringCSpn _tcscspn
#define stringLength _tcslen
//#define stringToInteger _ttoi //not used in clucene anymore, use integer w/ base
#define stringFind _tcsstr
#define stringFindChar _tcschr
#define stringCompare _tcscmp
#define stringNCopy _tcsncpy
#define stringCopy _tcscpy
#define stringCat _tcscat
//#define stringToken _tcstok //not used in clucene anymore
#define stringPrintF _sntprintf //you will have errors, because now we used printf w/ bufferlen count
#define printFormatted _tprintf

//conversion functions
#define integerToString _i64tot
#define stringToIntegerBase _tcstoi64
#define stringToFloat _tcstod

//file find structures
#define Cmd_Stat fileStat
#define Struct_Stat fileStat
#define stringICompare _tcsicmp
#define stringNCompare _tcsncmp
#define stringDifference _tcscmp

//character conversion functions
#define isSpace _istspace
#define isDigit _istwdigit
#define isAlNum _istwalnum
#define toLower _totlower
#define stringUpper _tcsupr
//#define stringLower _tcslwr //not used in clucene anymore

#define _THROWX(y) _THROWT(y)
#define _THROWC(y) _THROWA(y)

//file naming stuff - remember we have changed all names to file naming lower case
#define fileRename _rename
#define fileFullName(abs,rel) _realpath(rel,abs)
#define makeDirectory _tmkdir
#define unlinkFile _unlink

//no longer supported definitions
#ifdef _UNICODE
	#define TO_CHAR_T STRDUP_AtoT
	#define _cout wcout
	#define _cin wcin
    #define _cerr wcerr
#else
	#define TO_CHAR_T STRDUP_WtoT
	#define _cout cout
	#define _cin cin
	#define _cerr cerr
#endif

//some headers that used to be automatically included:
#include "CLucene/util/dirent.h" //if we have dirent, then the native one will be used

#endif
