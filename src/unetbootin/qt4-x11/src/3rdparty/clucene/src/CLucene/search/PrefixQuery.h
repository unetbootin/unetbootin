/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#ifndef _lucene_search_PrefixQuery
#define _lucene_search_PrefixQuery
#if defined(_LUCENE_PRAGMA_ONCE)
# pragma once
#endif

#include "CLucene/index/Term.h"
#include "CLucene/index/Terms.h"
#include "CLucene/index/IndexReader.h"
#include "SearchHeader.h"
#include "BooleanQuery.h"
#include "TermQuery.h"
#include "CLucene/util/StringBuffer.h"

CL_NS_DEF(search) 
	//PrefixQuery is a Query that matches documents containing terms with a specified prefix.

	class PrefixQuery: public Query {
	private:
		CL_NS(index)::Term* prefix;
	protected:
		PrefixQuery(const PrefixQuery& clone);
	public:

		//Constructor. Constructs a query for terms starting with prefix
		PrefixQuery(CL_NS(index)::Term* Prefix);

		//Destructor
		~PrefixQuery();

		//Returns the name "PrefixQuery"
		const TCHAR* getQueryName() const;
		static const TCHAR* getClassName();

		/** Returns the prefix of this query. */
		CL_NS(index)::Term* getPrefix(bool pointer=true);

		Query* combine(Query** queries);
		Query* rewrite(CL_NS(index)::IndexReader* reader);
		Query* clone() const;
		bool equals(Query * other) const;

		//Creates a user-readable version of this query and returns it as as string
		TCHAR* toString(const TCHAR* field) const;

		size_t hashCode() const;
	};
	
	
    class PrefixFilter: public Filter 
    {
    private:
    	CL_NS(index)::Term* prefix;
    protected:
    	PrefixFilter( const PrefixFilter& copy );
    	
    public:
    	PrefixFilter(CL_NS(index)::Term* prefix);
    	~PrefixFilter();
    
    	/** Returns a BitSet with true for documents which should be permitted in
    	search results, and false for those that should not. */
    	CL_NS(util)::BitSet* bits( CL_NS(index)::IndexReader* reader );
    	
    	Filter* clone() const;
    	TCHAR* toString();
    };
CL_NS_END
#endif
