/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#ifndef _lucene_debug_lucenebase_
#define _lucene_debug_lucenebase_

#ifdef _LUCENE_PRAGMA_ONCE
# pragma once
#endif

CL_NS_DEF(debug)

//Lucenebase is the superclass of all clucene objects. It provides
//memory debugging tracking and/or reference counting
class LuceneBase{
public:
#ifdef LUCENE_ENABLE_MEMLEAKTRACKING
	static void* operator new (size_t size);
	static void operator delete (void *p);
	int32_t __cl_initnum; ///< The order that the object was created at. This is then used to do a lookup in the objects list

	static void* operator new (size_t size, char const * file, int32_t line);
	static void operator delete (void *p, char const * file, int32_t line);

	static void* __cl_voidpadd(void* data, const char* file, int line, size_t size); ///<add arbitary data to the lucenbase_list and returns the same data
	static void __cl_voidpremove(const void* data, const char* file, int line);///<remove arbitary data to the lucenbase_list
	static void __cl_unregister(const void* obj); ///<un register object from the mem leak and ref count system

	static int32_t __cl_GetUnclosedObjectsCount();  ///< gets the number of unclosed objects
	static const char* __cl_GetUnclosedObject(int32_t item);  ///< get the name of the nth unclosed object
	static char* __cl_GetUnclosedObjects();  ///< get a string with the names of the unclosed objects
	static void __cl_PrintUnclosedObjects(); ///< print unclosed objects to the stdout
  
  	///This will clear memory relating to refcounting
	///other tools can be used to more accurately identify
	///memory leaks. This should only be called just
	///before closing, and after retrieving the
	///unclosed object list
  	static void __cl_ClearMemory();

#endif //LUCENE_ENABLE_MEMLEAKTRACKING

	int __cl_refcount;
	LuceneBase(){
		__cl_refcount=1;
	}
	inline int __cl_getref(){
		return __cl_refcount;
	}
	inline int __cl_addref(){
		__cl_refcount++;
		return __cl_refcount;
	}
	inline int __cl_decref(){
		__cl_refcount--;
		return __cl_refcount;
	}
    virtual ~LuceneBase(){};
};

class LuceneVoidBase{
	public:
	#ifdef _DEBUG
		//a compile time check to make sure that _CLDELETE and _CLDECDELETE is being
		//used correctly.
		int dummy__see_mem_h_for_details; 
	#endif
        virtual ~LuceneVoidBase(){};
};

CL_NS_END
#endif //_lucene_debug_lucenebase_
