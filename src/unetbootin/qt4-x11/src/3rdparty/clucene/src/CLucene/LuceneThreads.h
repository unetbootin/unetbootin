/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#ifndef _LuceneThreads_h
#define  _LuceneThreads_h
#if defined(_LUCENE_PRAGMA_ONCE)
# pragma once
#endif

#if defined(_CL_DISABLE_MULTITHREADING)
	#define SCOPED_LOCK_MUTEX(theMutex)
	#define DEFINE_MUTEX(x)
	#define STATIC_DEFINE_MUTEX(x)
	#define _LUCENE_SLEEP(x)
	#define _LUCENE_CURRTHREADID 1
	#define _LUCENE_THREADID_TYPE char
	
	CL_NS_DEF(util)
	class CLuceneThreadIdCompare
	{
	public:
		enum
		{	// parameters for hash table
			bucket_size = 4,	// 0 < bucket_size
			min_buckets = 8
		};	// min_buckets = 2 ^^ N, 0 < N

		bool operator()( char t1, char t2 ) const{
			return t1 < t2;
		}
	};
    CL_NS_END
#else

	#if defined(_LUCENE_DONTIMPLEMENT_THREADMUTEX)
		//do nothing
	#elif defined(_CL_HAVE_PTHREAD)
		#include "CLucene/config/threadPthread.h"
	#elif defined(_CL_HAVE_WIN32_THREADS) || defined(_CLCOMPILER_MSVC) || defined(__MINGW32__) //note that mingw32 could have pthreads, so put this after.
		#if !defined(_CL_HAVE_WIN32_THREADS)
		#define _CL_HAVE_WIN32_THREADS
		#endif
		#include "CLucene/config/threadCSection.h"
	#else
		#error A valid thread library was not found
	#endif //mutex types
	
	CL_NS_DEF(util)
	/** @internal */
	class mutexGuard
	{
	private:
		_LUCENE_THREADMUTEX* mrMutex;
		mutexGuard(const mutexGuard& clone);
	public:
		mutexGuard( _LUCENE_THREADMUTEX& rMutex );
		~mutexGuard();
	};
	CL_NS_END
	
	#define SCOPED_LOCK_MUTEX(theMutex) 	CL_NS(util)::mutexGuard theMutexGuard(theMutex);
	#define DEFINE_MUTEX(theMutex) 			_LUCENE_THREADMUTEX theMutex;
	#define STATIC_DEFINE_MUTEX(theMutex) 	static _LUCENE_THREADMUTEX theMutex;

#endif //_CL_DISABLE_MULTITHREADING



#endif
