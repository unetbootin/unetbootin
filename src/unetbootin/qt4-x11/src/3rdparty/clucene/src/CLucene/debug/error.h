/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#ifndef _lucene_debug_error_
#define _lucene_debug_error_

#if defined(_LUCENE_PRAGMA_ONCE)
# pragma once
#endif

#define CL_ERR_UNKNOWN -1
#define CL_ERR_IO 1
#define CL_ERR_NullPointer 2
#define CL_ERR_Runtime 3
#define CL_ERR_IllegalArgument 4
#define CL_ERR_Parse 5
#define CL_ERR_TokenMgr 6
#define CL_ERR_UnsupportedOperation 7
#define CL_ERR_InvalidState 8
#define CL_ERR_IndexOutOfBounds 9
#define CL_ERR_TooManyClauses 10
#define CL_ERR_RAMTransaction 11
#define CL_ERR_InvalidCast 12
#define CL_ERR_IllegalState 13



////////////////////////////////////////////////////////
//error try/throw/catch definitions
////////////////////////////////////////////////////////
#ifdef _CL_DISABLE_NATIVE_EXCEPTIONS
 /*#define try _jpr_Try
 #define _CLCATCH _jpr_Catch
 #define _CLFINALLY(x) xxxx
 #define _CLTHROWA(y) _jpr_Throw
 #define _THROWA_DEL(y) _jpr_Throw
 #define _RETHROW(x) _jpr_Throw
 #define _CLTHROWT(y) _jpr_Throw

 #define _THROWS ,_jpr_Throws*/
#else
  class CLuceneError
  {
  	int error_number;
	char* _awhat;
	TCHAR* _twhat;
  public:
    CLuceneError(const CLuceneError& clone);
    CLuceneError(int num, const char* str, bool ownstr);
#ifdef _UCS2
    CLuceneError(int num, const TCHAR* str, bool ownstr);
#endif
  	int number(){return error_number;}
	char* what();
	TCHAR* twhat();
	~CLuceneError() throw();
  };
	
 //#define _THROWS //does nothing
 #define _CLFINALLY(x) catch(...){ x; throw; } x //note: code x is not run if return is called
 #define _CLTHROWA(number, str) throw CLuceneError(number, str,false)
 #define _CLTHROWT(number, str) throw CLuceneError(number, str,false)
 #define _CLTHROWA_DEL(number, str) throw CLuceneError(number, str,true) //throw a string ensures the value is deleted
 #define _CLTHROWT_DEL(number, str) throw CLuceneError(number, str,true) //throw a string ensures the value is deleted


#endif //_LUCENE_DISABLE_EXCEPTIONS
//
////////////////////////////////////////////////////////

#endif
