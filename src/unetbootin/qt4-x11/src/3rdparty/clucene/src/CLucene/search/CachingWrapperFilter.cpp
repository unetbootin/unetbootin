/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#include "CLucene/StdHeader.h"
#include "CachingWrapperFilter.h"

CL_NS_DEF(search)
CL_NS_USE(index)
CL_NS_USE(util)

AbstractCachingFilter::AbstractCachingFilter():
	cache(false,true)
{
}
AbstractCachingFilter::AbstractCachingFilter(const AbstractCachingFilter& copy):
	cache(false,true)
{
}
AbstractCachingFilter::~AbstractCachingFilter(){
}
AbstractCachingFilter::BitSetHolder::BitSetHolder(CL_NS(util)::BitSet* bits, bool deleteBs){
	this->bits = bits;
	this->deleteBs = deleteBs;
}
AbstractCachingFilter::BitSetHolder::~BitSetHolder(){
	if ( deleteBs )
		_CLDELETE(bits);
}


BitSet* AbstractCachingFilter::bits(IndexReader* reader){
	SCOPED_LOCK_MUTEX(cache.THIS_LOCK)
	BitSetHolder* cached = cache.get(reader);
	if ( cached != NULL )
		return cached->bits;
	BitSet* bs = doBits(reader);
	BitSetHolder* bsh = _CLNEW BitSetHolder(bs, doShouldDeleteBitSet(bs));
	cache.put(reader,bsh);
	return bs;
}
void AbstractCachingFilter::closeCallback(CL_NS(index)::IndexReader* reader, void*){
	SCOPED_LOCK_MUTEX(cache.THIS_LOCK)
	cache.remove(reader);
}




CachingWrapperFilter::CachingWrapperFilter(Filter* filter, bool deleteFilter){
	this->filter = filter;
	this->deleteFilter = deleteFilter;
}
CachingWrapperFilter::CachingWrapperFilter(const CachingWrapperFilter& copy):
	AbstractCachingFilter(copy)
{
	this->filter = copy.filter->clone();
	this->deleteFilter = true;
}
Filter* CachingWrapperFilter::clone() const{
	return _CLNEW CachingWrapperFilter(*this);
}
TCHAR* CachingWrapperFilter::toString(){
	TCHAR* fs = filter->toString();
	int len = _tcslen(fs)+23;
	TCHAR* ret = _CL_NEWARRAY(TCHAR,len);
	_sntprintf(ret,len,_T("CachingWrapperFilter(%s)"),fs);
	_CLDELETE_CARRAY(fs);
	return ret;
}
BitSet* CachingWrapperFilter::doBits(IndexReader* reader){
	return filter->bits(reader);
}
bool CachingWrapperFilter::doShouldDeleteBitSet( CL_NS(util)::BitSet* bits ){
	return filter->shouldDeleteBitSet(bits);
}
CachingWrapperFilter::~CachingWrapperFilter(){
	if ( deleteFilter ){
		_CLDELETE(filter);
	}else
		filter=NULL;
}

CL_NS_END
