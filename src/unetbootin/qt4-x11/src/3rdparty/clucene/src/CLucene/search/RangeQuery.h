/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#ifndef _lucene_search_RangeQuery_
#define _lucene_search_RangeQuery_
#if defined(_LUCENE_PRAGMA_ONCE)
# pragma once
#endif

#include "SearchHeader.h"
#include "Scorer.h"
#include "TermQuery.h"

#include "CLucene/index/Term.h"
#include "CLucene/index/Terms.h"

#include "CLucene/util/StringBuffer.h"


CL_NS_DEF(search)
    /** Constructs a query selecting all terms greater than
     * <code>lowerTerm</code> but less than <code>upperTerm</code>.
     * There must be at least one term and either term may be null,
     * in which case there is no bound on that side, but if there are
     * two terms, both terms <b>must</b> be for the same field.
     */
	class RangeQuery: public Query
    {
    private: 
        CL_NS(index)::Term* lowerTerm;
        CL_NS(index)::Term* upperTerm;
        bool inclusive;
	protected:
		RangeQuery(const RangeQuery& clone);
		
	public:
        // Constructs a query selecting all terms greater than 
        // <code>lowerTerm</code> but less than <code>upperTerm</code>.
        // There must be at least one term and either term may be NULL--
        // in which case there is no bound on that side, but if there are 
        // two term, both terms <b>must</b> be for the same field.
		RangeQuery(CL_NS(index)::Term* LowerTerm, CL_NS(index)::Term* UpperTerm, const bool Inclusive);
		~RangeQuery();

		const TCHAR* getQueryName() const;
		static const TCHAR* getClassName();

		Query* rewrite(CL_NS(index)::IndexReader* reader);

		Query* combine(Query** queries);
        
        // Prints a user-readable version of this query. 
        TCHAR* toString(const TCHAR* field) const;
        
		Query* clone() const;

		bool equals(Query * other) const;

		CL_NS(index)::Term* getLowerTerm(bool pointer=true) const;
		CL_NS(index)::Term* getUpperTerm(bool pointer=true) const;
		bool isInclusive() const;
        const TCHAR* getField() const;

		size_t hashCode() const;
    };

CL_NS_END
#endif
