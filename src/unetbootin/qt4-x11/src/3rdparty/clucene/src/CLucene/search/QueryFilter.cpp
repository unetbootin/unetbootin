/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#include "CLucene/StdHeader.h"
#include "QueryFilter.h"
#include "IndexSearcher.h"

CL_NS_DEF(search)
CL_NS_USE(util)
CL_NS_USE(index)


QueryFilter::QueryFilter( const Query* query )
{
	this->query = query->clone();
}


QueryFilter::~QueryFilter()
{
	_CLDELETE( query );
}


QueryFilter::QueryFilter( const QueryFilter& copy )
{
	this->query = copy.query->clone();
}


Filter* QueryFilter::clone() const {
	return _CLNEW QueryFilter(*this );
}


TCHAR* QueryFilter::toString()
{
	TCHAR* qt = query->toString();
	size_t len = _tcslen(qt) + 14;
	TCHAR* ret = _CL_NEWARRAY( TCHAR, len );
	ret[0] = 0;
	_sntprintf( ret, len, _T("QueryFilter(%s)"), qt );
	_CLDELETE_CARRAY(qt);
	return ret;
}


/** Returns a BitSet with true for documents which should be permitted in
search results, and false for those that should not. */
BitSet* QueryFilter::bits( IndexReader* reader )
{
    BitSet* bits = _CLNEW BitSet(reader->maxDoc());

	IndexSearcher s(reader);
	QFHitCollector hc(bits);
	s._search(query, NULL, &hc);
    return bits;
}


QueryFilter::QFHitCollector::QFHitCollector(CL_NS(util)::BitSet* bits){
	this->bits = bits;
}

void QueryFilter::QFHitCollector::collect(const int32_t doc, const qreal score) {
	bits->set(doc);  // set bit for hit
}


CL_NS_END
