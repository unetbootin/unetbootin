/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#ifndef _lucene_util_StringIntern_H
#define _lucene_util_StringIntern_H

#if defined(_LUCENE_PRAGMA_ONCE)
# pragma once
#endif

#include "VoidMap.h"
CL_NS_DEF(util)
typedef CL_NS(util)::CLHashMap<const TCHAR*,int,CL_NS(util)::Compare::TChar,CL_NS(util)::Equals::TChar,CL_NS(util)::Deletor::tcArray, CL_NS(util)::Deletor::DummyInt32 > __wcsintrntype;
typedef CL_NS(util)::CLHashMap<const char*,int,CL_NS(util)::Compare::Char,CL_NS(util)::Equals::Char,CL_NS(util)::Deletor::acArray, CL_NS(util)::Deletor::DummyInt32 > __strintrntype;

  /** Functions for intern'ing strings. This
  * is a process of pooling strings thus using less memory,
  * and furthermore allows intern'd strings to be directly
  * compared:
  * string1==string2, rather than _tcscmp(string1,string2)
  */
  class CLStringIntern{
	  static __wcsintrntype stringPool;
	  static __strintrntype stringaPool;
	  STATIC_DEFINE_MUTEX(THIS_LOCK)
  public:
	/** 
	* Internalise the specified string.
	* \return Returns a pointer to the internalised string
	*/
	static const char* internA(const char* str CL_FILELINEPARAM);
	/** 
	* Uninternalise the specified string. Decreases
	* the reference count and frees the string if 
	* reference count is zero
	* \returns true if string was destroyed, otherwise false
	*/
	static bool uninternA(const char* str);

	/** 
	* Internalise the specified string.
	* \return Returns a pointer to the internalised string
	*/
	static const TCHAR* intern(const TCHAR* str CL_FILELINEPARAM);
	
	/** 
	* Uninternalise the specified string. Decreases
	* the reference count and frees the string if 
	* reference count is zero
	* \returns true if string was destroyed, otherwise false
	*/
	static bool unintern(const TCHAR* str);

    static void shutdown();
  };

CL_NS_END
#endif
