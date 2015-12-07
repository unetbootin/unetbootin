/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
*
* Changes are Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
------------------------------------------------------------------------------*/
#ifndef _lucene_util_ThreadLocal_H
#define _lucene_util_ThreadLocal_H

#if defined(_LUCENE_PRAGMA_ONCE)
# pragma once
#endif

#include "CLucene/util/VoidMap.h"

CL_NS_DEF(util)

class ThreadLocalBase: LUCENE_BASE{
public:
	/** 
	* A hook called when CLucene is starting or shutting down, 
	* this can be used for setting up and tearing down static
	* variables
	*/
	typedef void ShutdownHook(bool startup);

protected:
	STATIC_DEFINE_MUTEX(ThreadLocalBase_THIS_LOCK)
	typedef CL_NS(util)::CLMultiMap<_LUCENE_THREADID_TYPE, ThreadLocalBase*, 
			CL_NS(util)::CLuceneThreadIdCompare, 
			CL_NS(util)::Deletor::ConstNullVal<_LUCENE_THREADID_TYPE>, 
			CL_NS(util)::Deletor::ConstNullVal<ThreadLocalBase*> > ThreadLocalsType;
	static ThreadLocalsType threadLocals;
	//todo: using http://en.wikipedia.org/wiki/Thread-local_storage#Pthreads_implementation
	//would work better... but lots of testing would be needed first...
	typedef CL_NS(util)::CLSetList<ShutdownHook*, 
			CL_NS(util)::Compare::Void<ShutdownHook>, 
			CL_NS(util)::Deletor::ConstNullVal<ShutdownHook*> > ShutdownHooksType;
	static ShutdownHooksType shutdownHooks;

	ThreadLocalBase();
public:
	virtual ~ThreadLocalBase();

	/**
	* Call this function to clear the local thread data for this
	* ThreadLocal. Calling set(NULL) does the same thing, except
	* this function is virtual and can be called without knowing
	* the template.
	*/
	virtual void setNull() = 0;

	/**
	* If you want to clean up thread specific memory, then you should
	* make sure this thread is called when the thread is not going to be used
	* again. This will clean up threadlocal data which can contain quite a lot
	* of data, so if you are creating lots of new threads, then it is a good idea
	* to use this function, otherwise there will be many memory leaks.
	*/
	static void UnregisterCurrentThread();

	/**
	* Call this function to shutdown CLucene
	*/
	static void shutdown();

	/**
	* Add this function to the shutdown hook list. This function will be called
	* when CLucene is shutdown.
	*/
	static void registerShutdownHook(ShutdownHook* hook);
};

template<typename T,typename _deletor>
class ThreadLocal: public ThreadLocalBase{
	typedef CL_NS(util)::CLSet<_LUCENE_THREADID_TYPE, T, 
			CL_NS(util)::CLuceneThreadIdCompare, 
			CL_NS(util)::Deletor::ConstNullVal<_LUCENE_THREADID_TYPE>, 
			_deletor > LocalsType;
	LocalsType locals;
	DEFINE_MUTEX(locals_LOCK)
public:
	ThreadLocal();
	~ThreadLocal();
	T get();
	void setNull();
	void set(T t);
};

template<typename T,typename _deletor>
ThreadLocal<T,_deletor>::ThreadLocal():
	locals(false,true)
{
	//add this object to the base's list of threadlocals to be
	//notified in case of UnregisterThread()
	_LUCENE_THREADID_TYPE id = _LUCENE_CURRTHREADID;
	SCOPED_LOCK_MUTEX(ThreadLocalBase_THIS_LOCK)
	threadLocals.insert( CL_NS_STD(pair)<const _LUCENE_THREADID_TYPE, ThreadLocalBase*>(id, this) );
}

template<typename T,typename _deletor>
ThreadLocal<T,_deletor>::~ThreadLocal(){
	//remove this object to the base's list of threadlocals
	_LUCENE_THREADID_TYPE id = _LUCENE_CURRTHREADID;
	SCOPED_LOCK_MUTEX(ThreadLocalBase_THIS_LOCK)
	
	//remove all the thread local data for this object
	locals.clear();

	//remove this object from the ThreadLocalBase threadLocal list
	ThreadLocalsType::iterator itr = threadLocals.lower_bound(id);
	ThreadLocalsType::iterator end = threadLocals.upper_bound(id);
	while ( itr != end ){
		if ( itr->second == this){
			threadLocals.erase(itr);
			break;
		}
		++itr;
	}
}

template<typename T,typename _deletor>
T ThreadLocal<T,_deletor>::get(){
	return locals.get(_LUCENE_CURRTHREADID);
}

template<typename T,typename _deletor>
void ThreadLocal<T,_deletor>::setNull(){
	set(NULL);
}

template<typename T,typename _deletor>
void ThreadLocal<T,_deletor>::set(T t){
	_LUCENE_THREADID_TYPE id = _LUCENE_CURRTHREADID;
	locals.remove(id);
	if ( t != NULL )
		locals.insert( CL_NS_STD(pair)<const _LUCENE_THREADID_TYPE,T>(id, t) );
}

CL_NS_END
#endif
