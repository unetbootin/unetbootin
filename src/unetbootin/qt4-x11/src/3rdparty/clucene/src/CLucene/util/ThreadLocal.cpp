/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#include "CLucene/StdHeader.h"
#include "CLucene/LuceneThreads.h"
#include "ThreadLocal.h"

CL_NS_DEF(util)

DEFINE_MUTEX(ThreadLocalBase::ThreadLocalBase_THIS_LOCK)

ThreadLocalBase::ShutdownHooksType ThreadLocalBase::shutdownHooks(false);
ThreadLocalBase::ThreadLocalsType ThreadLocalBase::threadLocals(false,false);

ThreadLocalBase::ThreadLocalBase(){
}
ThreadLocalBase::~ThreadLocalBase(){
}

void ThreadLocalBase::UnregisterCurrentThread(){
	_LUCENE_THREADID_TYPE id = _LUCENE_CURRTHREADID;
	SCOPED_LOCK_MUTEX(ThreadLocalBase_THIS_LOCK)
	
	ThreadLocalsType::iterator itr = threadLocals.lower_bound(id);
	ThreadLocalsType::iterator end = threadLocals.upper_bound(id);
	while ( itr != end ){
		itr->second->setNull();
		++itr;
	}
}
void ThreadLocalBase::shutdown(){
	SCOPED_LOCK_MUTEX(ThreadLocalBase_THIS_LOCK)
	
	ThreadLocalsType::iterator itr = threadLocals.begin();
	while ( itr != threadLocals.end() ){
		itr->second->setNull();
		++itr;
	}

	ShutdownHooksType::iterator itr2 = shutdownHooks.begin();
	while ( itr2 != shutdownHooks.end() ){
		ShutdownHook* hook = *itr2;
		hook(false);
	}
}
void ThreadLocalBase::registerShutdownHook(ShutdownHook* hook){
	SCOPED_LOCK_MUTEX(ThreadLocalBase_THIS_LOCK)
	shutdownHooks.insert(hook);
}


CL_NS_END
