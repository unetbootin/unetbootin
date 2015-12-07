/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#ifndef _lucene_search_MultiTermQuery_
#define _lucene_search_MultiTermQuery_

#if defined(_LUCENE_PRAGMA_ONCE)
# pragma once
#endif

#include "CLucene/util/StringBuffer.h"
#include "CLucene/index/IndexReader.h"
#include "CLucene/index/Term.h"
#include "CLucene/index/Terms.h"
#include "FilteredTermEnum.h"
#include "SearchHeader.h"
#include "BooleanQuery.h"
#include "TermQuery.h"

CL_NS_DEF(search)
    /**
     * A {@link Query} that matches documents containing a subset of terms provided
     * by a {@link FilteredTermEnum} enumeration.
     * <P>
     * <code>MultiTermQuery</code> is not designed to be used by itself.
     * <BR>
     * The reason being that it is not intialized with a {@link FilteredTermEnum}
     * enumeration. A {@link FilteredTermEnum} enumeration needs to be provided.
     * <P>
     * For example, {@link WildcardQuery} and {@link FuzzyQuery} extend
     * <code>MultiTermQuery</code> to provide {@link WildcardTermEnum} and
     * {@link FuzzyTermEnum}, respectively.
     */
    class MultiTermQuery: public Query {
    private:
        CL_NS(index)::Term* term;
    protected:
        MultiTermQuery(const MultiTermQuery& clone);

		/** Construct the enumeration to be used, expanding the pattern term. */
		virtual FilteredTermEnum* getEnum(CL_NS(index)::IndexReader* reader) = 0;
    public:
        /** Constructs a query for terms matching <code>term</code>. */
        MultiTermQuery(CL_NS(index)::Term* t);

        virtual ~MultiTermQuery();

		/** Returns the pattern term. */
		CL_NS(index)::Term* getTerm(bool pointer=true) const;

		Query* combine(Query** queries);

        /** Prints a user-readable version of this query. */
        TCHAR* toString(const TCHAR* field) const;

		Query* rewrite(CL_NS(index)::IndexReader* reader);
    };
CL_NS_END
#endif
