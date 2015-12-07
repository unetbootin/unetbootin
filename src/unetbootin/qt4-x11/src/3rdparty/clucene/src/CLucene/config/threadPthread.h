/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
//NOTE: do not include this file directly, it is included from lucene internally.

#ifndef lucene_config_threadPthread_h
#define lucene_config_threadPthread_h

#include <pthread.h>

CL_NS_DEF(util)

///a posix implementation of the lock mutex
///todo: we need a spinlock implemenation for usage in reference counting
class mutex_pthread
{
private:
	pthread_mutex_t mtx;

public:
	mutex_pthread(const mutex_pthread& clone);
	mutex_pthread();
	~mutex_pthread();
	void lock();
	void unlock();
	
private:
	#ifndef _CL_HAVE_PTHREAD_MUTEX_RECURSIVE
	pthread_t lockOwner;
	unsigned int lockCount;
	#endif
};

#define _LUCENE_SLEEP(x) usleep(x*1000) //_LUCENE_SLEEP should be in millis, usleep is in micros
#define _LUCENE_THREADMUTEX CL_NS(util)::mutex_pthread
#define _LUCENE_CURRTHREADID pthread_self()
#define _LUCENE_THREADID_TYPE pthread_t

class CLuceneThreadIdCompare
{
public:
	enum
	{	// parameters for hash table
		bucket_size = 4,	// 0 < bucket_size
		min_buckets = 8
	};	// min_buckets = 2 ^^ N, 0 < N

	bool operator()( pthread_t t1, pthread_t t2 ) const{
		return t1 < t2;
	}
};


CL_NS_END

#endif //lucene_config_threadPthread_h
