/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
//NOTE: do not include this file directly, it is included from lucene internally.

#ifndef lucene_config_threadCSection_h
#define lucene_config_threadCSection_h
#if defined(_LUCENE_PRAGMA_ONCE)
# pragma once
#endif

CL_NS_DEF(util)

   #if !defined(LUCENE_USE_WINDOWS_H) && !defined(_WINDOWS_) && !defined(__MINGW32__)
      //we have not explicity included windows.h and windows.h has
      //not been included (check _WINDOWS_), then we must define
      //our own definitions to the thread locking functions:
      struct CRITICAL_SECTION
      {
         struct critical_section_debug * DebugInfo;
         long LockCount;
         long RecursionCount;
         void * OwningThread;
         void * LockSemaphore;
      #if defined(_WIN64)
         unsigned __int64 SpinCount;
      #else
         unsigned long SpinCount;
      #endif
      };
   #endif

   ///a windows implementation of the lock mutex
   ///todo: boost has a InterlockedExchange way of locking too. More backwards compatible/faster???
	class mutex_win32
	{
	private:
		CRITICAL_SECTION mtx;
	public:
		mutex_win32(const mutex_win32& clone);
		mutex_win32();
		~mutex_win32();
		void lock();
		void unlock();
	};
   
	class CLuceneThreadIdCompare
	{
	public:
			
		enum
		{	// parameters for hash table
			bucket_size = 4,	// 0 < bucket_size
			min_buckets = 8
		};	// min_buckets = 2 ^^ N, 0 < N

		bool operator()( DWORD t1, DWORD t2 ) const{
			return t1 < t2;
		}
	};
	
	#define _LUCENE_SLEEP(x) Sleep(x)
	#define _LUCENE_THREADMUTEX CL_NS(util)::mutex_win32
	#define _LUCENE_CURRTHREADID GetCurrentThreadId()
	#define _LUCENE_THREADID_TYPE DWORD
CL_NS_END

#endif //lucene_config_threadCSection_h
