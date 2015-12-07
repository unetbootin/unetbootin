/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#include "CLucene/StdHeader.h"

#ifndef _CL_DISABLE_MULTITHREADING
CL_NS_DEF(util)


mutexGuard::mutexGuard(const mutexGuard& clone){
	//no autoclone
	mrMutex = NULL;
}
mutexGuard::mutexGuard( _LUCENE_THREADMUTEX& rMutex ) :
	mrMutex(&rMutex)
{
	mrMutex->lock();
}
mutexGuard::~mutexGuard()
{
	mrMutex->unlock();
}

#if defined(_LUCENE_DONTIMPLEMENT_THREADMUTEX)
	//do nothing
	#if defined(_LUCENE_PRAGMA_WARNINGS)
	 #pragma message ("==================Not implementing any thread mutex==================")
	#else
	 #warning "==================Not implementing any thread mutex=================="
	#endif



#elif defined(_CL_HAVE_WIN32_THREADS)
	#include "CLucene/config/threadCSection.h"

   #if !defined(LUCENE_USE_WINDOWS_H) && !defined(_WINDOWS_)
      //we have not explicity included windows.h and windows.h has
      //not been included (check _WINDOWS_), then we must define
      //our own definitions to the thread locking functions:
      extern "C" __declspec(dllimport) void __stdcall InitializeCriticalSection(CRITICAL_SECTION *);
      extern "C" __declspec(dllimport) void __stdcall EnterCriticalSection(CRITICAL_SECTION *);
      extern "C" __declspec(dllimport) void __stdcall LeaveCriticalSection(CRITICAL_SECTION *);
      extern "C" __declspec(dllimport) void __stdcall DeleteCriticalSection(CRITICAL_SECTION *);
	  extern "C" __declspec(dllimport) unsigned long __stdcall GetCurrentThreadId();
   #endif
	
	mutex_win32::mutex_win32(const mutex_win32& clone){
		InitializeCriticalSection(&mtx); 
	}
	mutex_win32::mutex_win32()
	{ 
		InitializeCriticalSection(&mtx); 
	}
	
	mutex_win32::~mutex_win32()
	{ 
		DeleteCriticalSection(&mtx); 
	}
	
	void mutex_win32::lock()
	{ 
		EnterCriticalSection(&mtx); 
	}
	
	void mutex_win32::unlock()
	{ 
		LeaveCriticalSection(&mtx); 
	}



#elif defined(_CL_HAVE_PTHREAD)
	#include "CLucene/config/threadPthread.h"
		
	#ifdef _CL_HAVE_PTHREAD_MUTEX_RECURSIVE
		bool mutex_pthread_attr_initd=false;
		pthread_mutexattr_t mutex_pthread_attr;
	#endif
	
	#ifdef _CL__CND_DEBUG
		#define _CLPTHREAD_CHECK(c,m) CND_PRECONDITION(c==0,m)
	#else
		#define _CLPTHREAD_CHECK(c,m) c;
	#endif

	mutex_pthread::mutex_pthread(const mutex_pthread& clone){
		#ifdef _CL_HAVE_PTHREAD_MUTEX_RECURSIVE
		_CLPTHREAD_CHECK(pthread_mutex_init(&mtx, &mutex_pthread_attr), "mutex_pthread(clone) constructor failed")
		#else
	  	#if defined(__hpux) && defined(_DECTHREADS_)
			_CLPTHREAD_CHECK(pthread_mutex_init(&mtx, pthread_mutexattr_default), "mutex_pthread(clone) constructor failed")
		#else
			_CLPTHREAD_CHECK(pthread_mutex_init(&mtx, 0), "mutex_pthread(clone) constructor failed")
		#endif
		lockCount=0;
		lockOwner=0;
		#endif
	}
	mutex_pthread::mutex_pthread()
	{ 
		#ifdef _CL_HAVE_PTHREAD_MUTEX_RECURSIVE
	  	if ( mutex_pthread_attr_initd == false ){
	  		pthread_mutexattr_init(&mutex_pthread_attr);
		  	pthread_mutexattr_settype(&mutex_pthread_attr, PTHREAD_MUTEX_RECURSIVE);
		  	mutex_pthread_attr_initd = true;
		}
		_CLPTHREAD_CHECK(pthread_mutex_init(&mtx, &mutex_pthread_attr), "mutex_pthread(clone) constructor failed")
		#else
	  	#if defined(__hpux) && defined(_DECTHREADS_)
			_CLPTHREAD_CHECK(pthread_mutex_init(&mtx, pthread_mutexattr_default), "mutex_pthread(clone) constructor failed")
		#else
			_CLPTHREAD_CHECK(pthread_mutex_init(&mtx, 0), "mutex_pthread(clone) constructor failed")
		#endif
		lockCount=0;
		lockOwner=0;
		#endif
	}
	
	mutex_pthread::~mutex_pthread()
	{ 
		_CLPTHREAD_CHECK(pthread_mutex_destroy(&mtx), "~mutex_pthread destructor failed") 
	}
	
	void mutex_pthread::lock()
	{ 
		#ifndef _CL_HAVE_PTHREAD_MUTEX_RECURSIVE
		pthread_t currentThread = pthread_self();
		if( pthread_equal( lockOwner, currentThread ) ) {
			++lockCount;
		} else {
			_CLPTHREAD_CHECK(pthread_mutex_lock(&mtx), "mutex_pthread::lock")
			lockOwner = currentThread;
			lockCount = 1;
		}
		#else
		_CLPTHREAD_CHECK(pthread_mutex_lock(&mtx), "mutex_pthread::lock")
		#endif
	}
	
	void mutex_pthread::unlock()
	{ 
		#ifndef _CL_HAVE_PTHREAD_MUTEX_RECURSIVE
		--lockCount;
		if( lockCount == 0 )
		{
			lockOwner = 0;
			_CLPTHREAD_CHECK(pthread_mutex_unlock(&mtx), "mutex_pthread::unlock")
		}
		#else
		_CLPTHREAD_CHECK(pthread_mutex_unlock(&mtx), "mutex_pthread::unlock")
		#endif
	}

#endif //thread impl choice


CL_NS_END
#endif //!_CL_DISABLE_MULTITHREADING
