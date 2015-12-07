/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#ifndef _lucene_search_DateFilter_
#define _lucene_search_DateFilter_

#if defined(_LUCENE_PRAGMA_ONCE)
# pragma once
#endif

#include "CLucene/document/DateField.h"
#include "CLucene/index/Term.h"
#include "CLucene/index/Terms.h"
#include "CLucene/index/IndexReader.h"
#include "CLucene/util/BitSet.h"
#include "Filter.h"

CL_NS_DEF(search)
  /**
 * A Filter that restricts search results to a range of time.
 *
 * <p>For this to work, documents must have been indexed with a
 * {@link DateField}.
 */
  class DateFilter: public Filter {
  private:
	CL_NS(index)::Term* start;
    CL_NS(index)::Term* end;

  protected:
    DateFilter(const DateFilter& copy);
  public:
    ~DateFilter();

    /** Constructs a filter for field <code>f</code> matching times between
      <code>from</code> and <code>to</code>. */
    DateFilter(const TCHAR* f, int64_t from, int64_t to);

    /** Constructs a filter for field <code>f</code> matching times before
      <code>time</code>. */
    static DateFilter* Before(const TCHAR* field, int64_t time) ;

    /** Constructs a filter for field <code>f</code> matching times after
      <code>time</code>. */
    static DateFilter* After(const TCHAR* field, int64_t time) ;

    /** Returns a BitSet with true for documents which should be permitted in
      search results, and false for those that should not. */
	  CL_NS(util)::BitSet* bits(CL_NS(index)::IndexReader* reader) ;

	Filter* clone() const;
	
	TCHAR* toString();
  };
CL_NS_END
#endif
