/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#ifndef _lucene_search_Filter_
#define _lucene_search_Filter_

#if defined(_LUCENE_PRAGMA_ONCE)
# pragma once
#endif

#include "CLucene/index/IndexReader.h"
#include "CLucene/util/BitSet.h"

CL_NS_DEF(search)
  // Abstract base class providing a mechanism to restrict searches to a subset
  // of an index.
  class Filter: LUCENE_BASE {
  public:
    virtual ~Filter(){
	}
	
	virtual Filter* clone() const = 0;

    /**
    * Returns a BitSet with true for documents which should be permitted in
    * search results, and false for those that should not.
    * MEMORY: read shouldDeleteBitSet
    */
    virtual CL_NS(util)::BitSet* bits(CL_NS(index)::IndexReader* reader)=0;
    
    /**
    * Because of the problem of cached bitsets with the CachingWrapperFilter,
    * CLucene has no way of knowing whether to delete the bitset returned from bits().
    * To properly clean memory from bits(), pass the bitset to this function. The
    * Filter should be deleted if this function returns true.
    */
	virtual bool shouldDeleteBitSet(const CL_NS(util)::BitSet* bs) const{ return true; }

	//Creates a user-readable version of this query and returns it as as string
	virtual TCHAR* toString()=0;
  };
CL_NS_END
#endif
