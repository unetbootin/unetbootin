/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#ifndef _lucene_util_Misc_H
#define _lucene_util_Misc_H

#if defined(_LUCENE_PRAGMA_ONCE)
# pragma once
#endif

CL_NS_DEF(util)


  /** A class containing various functions. 
  */
  class Misc{
  public:
    static uint64_t currentTimeMillis();
//    static const TCHAR* replace_all( const TCHAR* val, const TCHAR* srch, const TCHAR* repl );
    static bool dir_Exists(const char* path);
	static int64_t file_Size(const char* path);

    static size_t ahashCode(const char* str);
	  static size_t ahashCode(const char* str, size_t len);

    static TCHAR* join ( const TCHAR* a, const TCHAR* b, const TCHAR* c=NULL, const TCHAR* d=NULL,const TCHAR* e=NULL,const TCHAR* f=NULL );
    static char* ajoin ( const char* a, const char* b, const char* c=NULL, const char* d=NULL,const char* e=NULL,const char* f=NULL );

    static bool priv_isDotDir( const TCHAR* name );
	//Creates a filename by concatenating Segment with ext and x
	static char* segmentname(const char* segment, const char* ext, const int32_t x=-1 );
	//Creates a filename in buffer by concatenating Segment with ext and x
	static void segmentname(char* buffer,int32_t bufferLen, const char* Segment, const char* ext, const int32_t x=-1);

   /**
   * Compares two strings, character by character, and returns the
   * first position where the two strings differ from one another.
   *
   * @param s1 The first string to compare
   * @param s1Len The length of the first string to compare
   * @param s2 The second string to compare
   * @param s2Len The length of the second string to compare
   * @return The first position where the two strings differ.
   */
	static int32_t stringDifference(const TCHAR* s1, const int32_t s1Len, const TCHAR* s2, const int32_t s2Len);

#ifdef _UCS2
    static size_t whashCode(const wchar_t* str);
		static size_t whashCode(const wchar_t* str, size_t len);
		#define thashCode whashCode
	  
    static char* _wideToChar(const wchar_t* s CL_FILELINEPARAM);
    static wchar_t* _charToWide(const char* s CL_FILELINEPARAM);

    static void _cpycharToWide(const char* s, wchar_t* d, size_t len);
    static void _cpywideToChar(const wchar_t* s, char* d, size_t len);
#else
	#define thashCode ahashCode
#endif
  };

CL_NS_END
#endif
