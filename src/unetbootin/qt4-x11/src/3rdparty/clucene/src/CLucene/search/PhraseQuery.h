/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#ifndef _lucene_search_PhraseQuery_
#define _lucene_search_PhraseQuery_

#if defined(_LUCENE_PRAGMA_ONCE)
# pragma once
#endif

#include "SearchHeader.h"
#include "Scorer.h"
#include "BooleanQuery.h"
#include "TermQuery.h"

#include "CLucene/index/Term.h"
#include "CLucene/index/Terms.h"
#include "CLucene/index/IndexReader.h"

#include "CLucene/util/StringBuffer.h"
#include "CLucene/util/VoidList.h"

#include "ExactPhraseScorer.h"
#include "SloppyPhraseScorer.h"

CL_NS_DEF(search)
	// A Query that matches documents containing a particular sequence of terms.
	// This may be combined with other terms with a {@link BooleanQuery}.
	class PhraseQuery: public Query {
	private:
		CL_NS(util)::CLVector<int32_t,CL_NS(util)::Deletor::DummyInt32> positions;
		int32_t slop;

		const TCHAR* field;
		CL_NS(util)::CLVector<CL_NS(index)::Term*> terms;

    
    	class PhraseWeight: public Weight {
    	private:
    		Searcher* searcher;
    		qreal value;
    		qreal idf;
    		qreal queryNorm;
    		qreal queryWeight;
    		PhraseQuery* _this;
    	public:
    		PhraseWeight(Searcher* searcher, PhraseQuery* _this);
    		~PhraseWeight();
    		TCHAR* toString();
    
    		Query* getQuery();
    		qreal getValue();
    
    		qreal sumOfSquaredWeights();
    		void normalize(qreal queryNorm);
    		Scorer* scorer(CL_NS(index)::IndexReader* reader);
    		void explain(CL_NS(index)::IndexReader* reader, int32_t doc, Explanation* ret);
    		TCHAR* toString(TCHAR* f);
    		bool equals(PhraseWeight* o);
    	};
    	friend class PhraseWeight;
	protected:
		Weight* _createWeight(Searcher* searcher);
		PhraseQuery(const PhraseQuery& clone);
	public: 
        //Constructor
        PhraseQuery();

		//Destructor
		~PhraseQuery();

        //Returns the string "PhraseQuery"
        const TCHAR* getQueryName() const;
        static const TCHAR* getClassName();
       
        //Sets the number of other words permitted between words in query phrase.
        //If zero, then this is an exact phrase search.  For larger values this works
        //like a WITHIN or NEAR operator.
        //
        //The slop is in fact an edit-distance, where the units correspond to
        //moves of terms in the query phrase out of position.  For example, to switch
        //the order of two words requires two moves (the first move places the words
        //atop one another), so to permit re-orderings of phrases, the slop must be
        //at least two.
        //
        //More exact matches are scored higher than sloppier matches, thus search
        //results are sorted by exactness.
        //
        //The slop is zero by default, requiring exact matches.
        void setSlop(const int32_t s) { slop = s; }
        
        //Returns the slop.  See setSlop(). 
        int32_t getSlop() const { return slop; }
        
        //Adds a term to the end of the query phrase. 
        void add(CL_NS(index)::Term* term);
		void add(CL_NS(index)::Term* term, int32_t position);


        
		//Returns the sum of squared weights 
        qreal sumOfSquaredWeights(Searcher* searcher);
        
		//Normalizes the Weight
        void normalize(const qreal norm);
        
        Scorer* scorer(CL_NS(index)::IndexReader* reader);
        
        //added by search highlighter
        CL_NS(index)::Term** getTerms() const;
		_CL_DEPRECATED( deleteDocuments ) int32_t* getPositions() const; ///@deprecated. use getPositions(Array<int32_t>& result)
		void getPositions(Array<int32_t>& result) const;
        const TCHAR* getFieldName() const{ return field; }
 
        //Prints a user-readable version of this query. 
        TCHAR* toString(const TCHAR* f) const;

		Query* clone() const;
		bool equals(CL_NS(search)::Query *) const;
		
		size_t hashCode() const;
	};
CL_NS_END
#endif
