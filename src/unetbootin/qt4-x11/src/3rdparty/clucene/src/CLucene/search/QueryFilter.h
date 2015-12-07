/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#ifndef _lucene_search_QueryFilter_
#define _lucene_search_QueryFilter_

#include "CLucene/util/BitSet.h"
#include "CLucene/index/IndexReader.h"
#include "SearchHeader.h"
#include "CachingWrapperFilter.h"

CL_NS_DEF(search)

class QueryFilter: public Filter
{
private:
	Query* query;

	class QFHitCollector: public HitCollector{
		CL_NS(util)::BitSet* bits;
	public:    
		QFHitCollector(CL_NS(util)::BitSet* bits);
		void collect(const int32_t doc, const qreal score);
    };

protected:
	QueryFilter( const QueryFilter& copy );
public:
	QueryFilter( const Query* query );
	
	~QueryFilter();
	
	CL_NS(util)::BitSet* bits( CL_NS(index)::IndexReader* reader );
	
	Filter *clone() const;
	
	TCHAR *toString();
};

CL_NS_END
#endif
