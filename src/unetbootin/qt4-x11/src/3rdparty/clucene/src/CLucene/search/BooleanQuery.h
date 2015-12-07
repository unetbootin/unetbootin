/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#ifndef _lucene_search_BooleanQuery_
#define _lucene_search_BooleanQuery_

#if defined(_LUCENE_PRAGMA_ONCE)
# pragma once
#endif

#include "ConjunctionScorer.h"
#include "CLucene/index/IndexReader.h"
#include "CLucene/util/StringBuffer.h"
#include "SearchHeader.h"
#include "BooleanClause.h"
#include "BooleanScorer.h"
#include "Scorer.h"

CL_NS_DEF(search)

	
    // A Query that matches documents matching boolean combinations of other
    // queries, typically {@link TermQuery}s or {@link PhraseQuery}s.
	class BooleanQuery:public Query {
	public:
		typedef CL_NS(util)::CLVector<BooleanClause*,CL_NS(util)::Deletor::Object<BooleanClause> > ClausesType;
	private:
		BooleanQuery::ClausesType clauses;
		static size_t maxClauseCount;

		class BooleanWeight: public Weight {
		private:
			Searcher* searcher;
			CL_NS(util)::CLVector<Weight*,CL_NS(util)::Deletor::Object<Weight> > weights;
			ClausesType* clauses;
			BooleanQuery* parentQuery;
		public:
			BooleanWeight(Searcher* searcher, 
				CL_NS(util)::CLVector<BooleanClause*,CL_NS(util)::Deletor::Object<BooleanClause> >* clauses, 
				BooleanQuery* parentQuery);
			~BooleanWeight();
			Query* getQuery();
			qreal getValue();
			qreal sumOfSquaredWeights();
			void normalize(qreal norm);
			Scorer* scorer(CL_NS(index)::IndexReader* reader);
			void explain(CL_NS(index)::IndexReader* reader, int32_t doc, Explanation* ret);
		};//booleanweight

    protected:
      Weight* _createWeight(Searcher* searcher) {
         return _CLNEW BooleanWeight(searcher,&clauses,this);
      }
      BooleanQuery(const BooleanQuery& clone);
	public:
    /** Constructs an empty boolean query. */
        BooleanQuery();
        
        ~BooleanQuery();
        
        const TCHAR* getQueryName() const;
		static const TCHAR* getClassName();
        
         /** Return the maximum number of clauses permitted, 1024 by default.
            * Attempts to add more than the permitted number of clauses cause {@link
            * TooManyClauses} to be thrown.*/
         static size_t getMaxClauseCount();

         /** Set the maximum number of clauses permitted. */
         static void setMaxClauseCount(size_t maxClauseCount);

       /** Adds a clause to a boolean query.  Clauses may be:
		* <ul>
		* <li><code>required</code> which means that documents which <i>do not</i>
		* match this sub-query will <i>not</i> match the boolean query;
		* <li><code>prohibited</code> which means that documents which <i>do</i>
		* match this sub-query will <i>not</i> match the boolean query; or
		* <li>neither, in which case matched documents are neither prohibited from
		* nor required to match the sub-query. However, a document must match at
		* least 1 sub-query to match the boolean query.
		* </ul>
		* It is an error to specify a clause as both <code>required</code> and
		* <code>prohibited</code>.
		*
		* @see #getMaxClauseCount()
		*/
		void add(Query* query, const bool required, const bool prohibited){
			add(query,false,required,prohibited);
		}
		void add(Query* query, const bool deleteQuery, const bool required, const bool prohibited);
		
		/** Copies the clauses of this query into the array.
		* The array must be at least as long as getClauseCount()
		* If you want to use the clauses, make sure you null terminate it.
		*/
		void getClauses(BooleanClause** clauses) const;
		
		///@deprecated
		_CL_DEPRECATED( getClauses(clauses) ) BooleanClause** getClauses() const;
		
	    /**
	    * Give client code access to clauses.size() so we know how
	    * large the array returned by getClauses is. 
	    */
	    size_t getClauseCount() const;

		/** Adds a clause to a boolean query.
		* @see #getMaxClauseCount()
		*/
		void add(BooleanClause* clause);

		Query* rewrite(CL_NS(index)::IndexReader* reader);
		Query* clone() const;
		bool equals(Query* o) const;
			
	  	/** Prints a user-readable version of this query. */
		TCHAR* toString(const TCHAR* field) const;
		/** Returns a hash code value for this object.*/
		size_t hashCode() const;
    };

CL_NS_END
#endif
