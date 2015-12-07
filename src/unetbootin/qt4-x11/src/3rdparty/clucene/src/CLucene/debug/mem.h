/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#ifndef _lucene_debug_mem_h
#define _lucene_debug_mem_h

#if defined(_LUCENE_PRAGMA_ONCE)
# pragma once
#endif

#include "lucenebase.h"

//Macro for creating new objects
#if defined(LUCENE_ENABLE_MEMLEAKTRACKING)
   #define _CLNEW new(__FILE__, __LINE__)
   #define LUCENE_BASE public CL_NS(debug)::LuceneBase
#elif defined(LUCENE_ENABLE_REFCOUNT)
   #define _CLNEW new
   #define LUCENE_BASE public CL_NS(debug)::LuceneBase
#else
   #define _CLNEW new
   #define LUCENE_BASE public CL_NS(debug)::LuceneVoidBase
   #define LUCENE_BASE_CHECK(obj) (obj)->dummy__see_mem_h_for_details
#endif
#define _CL_POINTER(x) (x==NULL?NULL:(x->__cl_addref()>=0?x:x)) //return a add-ref'd object
#define LUCENE_REFBASE public CL_NS(debug)::LuceneBase //this is the base of classes who *always* need refcounting

#if defined(_DEBUG)
  #if !defined(LUCENE_BASE_CHECK)
		#define LUCENE_BASE_CHECK(x)
	#endif
#else
	#undef LUCENE_BASE_CHECK
	#define LUCENE_BASE_CHECK(x)
#endif

//Macro for creating new arrays
#ifdef LUCENE_ENABLE_MEMLEAKTRACKING
	#define _CL_NEWARRAY(type,size) (type*)CL_NS(debug)::LuceneBase::__cl_voidpadd(new type[(size_t)size],__FILE__,__LINE__,(size_t)size);
	#define _CLDELETE_ARRAY(x) if (x!=NULL){CL_NS(debug)::LuceneBase::__cl_voidpremove((const void*)x,__FILE__,__LINE__); delete [] x; x=NULL;}
	#define _CLDELETE_LARRAY(x) if (x!=NULL){CL_NS(debug)::LuceneBase::__cl_voidpremove((const void*)x,__FILE__,__LINE__);delete [] x;}
	#ifndef _CLDELETE_CARRAY
		#define _CLDELETE_CARRAY(x) if (x!=NULL){CL_NS(debug)::LuceneBase::__cl_voidpremove((const void*)x,__FILE__,__LINE__);delete [] x; x=NULL;}
		#define _CLDELETE_LCARRAY(x) if (x!=NULL){CL_NS(debug)::LuceneBase::__cl_voidpremove((const void*)x,__FILE__,__LINE__);delete [] x;}
	#endif
#else
	#define _CL_NEWARRAY(type,size) new type[size]
	#define _CLDELETE_ARRAY(x) if (x!=NULL){delete [] x; x=NULL;}
	#define _CLDELETE_LARRAY(x) if (x!=NULL){delete [] x;}
	#ifndef _CLDELETE_CARRAY
		#define _CLDELETE_CARRAY(x) if (x!=NULL){delete [] x; x=NULL;}
		#define _CLDELETE_LCARRAY(x) if (x!=NULL){delete [] x;}
	#endif
#endif
//a shortcut for deleting a carray and all its contents
#define _CLDELETE_CARRAY_ALL(x) {if ( x!=NULL ){ for(int xcda=0;x[xcda]!=NULL;xcda++)_CLDELETE_CARRAY(x[xcda]);}_CLDELETE_ARRAY(x)};
#define _CLDELETE_ARRAY_ALL(x) {if ( x!=NULL ){ for(int xcda=0;x[xcda]!=NULL;xcda++)_CLDELETE(x[xcda]);}_CLDELETE_ARRAY(x)};
#ifndef _CLDELETE_CaARRAY
		#define _CLDELETE_CaARRAY _CLDELETE_CARRAY
		#define _CLDELETE_LCaARRAY _CLDELETE_LCARRAY
#endif

//Macro for deleting
#ifdef LUCENE_ENABLE_REFCOUNT
	#define _CLDELETE(x) if (x!=NULL){ CND_PRECONDITION((x)->__cl_refcount>=0,"__cl_refcount was < 0"); if ((x)->__cl_decref() <= 0)delete x; x=NULL; }
	#define _CLLDELETE(x) if (x!=NULL){ CND_PRECONDITION((x)->__cl_refcount>=0,"__cl_refcount was < 0"); if ((x)->__cl_decref() <= 0)delete x; }
#else
	#define _CLDELETE(x) if (x!=NULL){ LUCENE_BASE_CHECK(x); delete x; x=NULL; }
	#define _CLLDELETE(x) if (x!=NULL){ LUCENE_BASE_CHECK(x); delete x; }
#endif

//_CLDECDELETE deletes objects which are *always* refcounted
#define _CLDECDELETE(x) if (x!=NULL){ CND_PRECONDITION((x)->__cl_refcount>=0,"__cl_refcount was < 0"); if ((x)->__cl_decref() <= 0)delete x; x=NULL; }
#define _CLLDECDELETE(x) if (x!=NULL){ CND_PRECONDITION((x)->__cl_refcount>=0,"__cl_refcount was < 0"); if ((x)->__cl_decref() <= 0)delete x; }

//_VDelete should be used for deleting non-clucene objects.
//when using reference counting, _CLDELETE casts the object
//into a LuceneBase*.
#define _CLVDELETE(x) if(x!=NULL){delete x; x=NULL;}

template<typename T>
class Array: LUCENE_BASE{
public:
	T* values;
	size_t length;

	void deleteAll(){
		for (size_t i=0;i<length;i++)
			_CLDELETE(values[i]);
		_CLDELETE_ARRAY(values);
	}
	void deleteArray(){
		_CLDELETE_ARRAY(values);
	}

	Array(){
		values = NULL;
		length = 0;
	}
	Array(T* values, size_t length){
		this->values = values;
		this->length = length;
	}
	Array(size_t length){
		this->values = _CL_NEWARRAY(T,length);
		this->length = length;
	}
	~Array(){}

	const T operator[](size_t _Pos) const
	{
		if (length <= _Pos){
			_CLTHROWA(CL_ERR_IllegalArgument,"vector subscript out of range");
		}
		return (*(values + _Pos));
	}
	T operator[](size_t _Pos)
	{
		if (length <= _Pos){
			_CLTHROWA(CL_ERR_IllegalArgument,"vector subscript out of range");
		}
		return (*(values + _Pos));
	}

};

#endif //_lucene_debug_lucenebase_
