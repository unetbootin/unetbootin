/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#ifndef _lucene_search_SearchHeader_
#define _lucene_search_SearchHeader_

#if defined(_LUCENE_PRAGMA_ONCE)
# pragma once
#endif

#include "CLucene/index/IndexReader.h"
#include "CLucene/index/Term.h"
#include "Filter.h"
#include "CLucene/document/Document.h"
#include "Sort.h"
#include "CLucene/util/VoidList.h"
#include "Explanation.h"
#include "Similarity.h"

CL_NS_DEF(search)

	//predefine classes
	class Scorer;
	class Query;
	class Hits;
	class Sort;
	class FieldDoc;
	class TopFieldDocs;
   
   /** Expert: Returned by low-level search implementations.
	* @see TopDocs */
	struct ScoreDoc {
		/** Expert: A hit document's number.
		* @see Searcher#doc(int32_t)
		*/
		int32_t doc;

		/** Expert: The score of this document for the query. */
		qreal score;
	};

	/** Expert: Returned by low-level search implementations.
	* @see Searcher#search(Query,Filter,int32_t) */
	class TopDocs:LUCENE_BASE {
	public:
		/** Expert: The total number of hits for the query.
		 * @see Hits#length()
		*/
		int32_t totalHits;

		/** Expert: The top hits for the query. */
		ScoreDoc* scoreDocs;
		int32_t scoreDocsLength;

		/** Expert: Constructs a TopDocs. TopDocs takes ownership of the ScoreDoc array*/
		TopDocs(const int32_t th, ScoreDoc* sds, int32_t scoreDocsLength);
		~TopDocs();
	};

    // Lower-level search API.
    // @see Searcher#search(Query,HitCollector)
	class HitCollector: LUCENE_BASE {
    public:
      /** Called once for every non-zero scoring document, with the document number
      * and its score.
      *
      * <P>If, for example, an application wished to collect all of the hits for a
      * query in a BitSet, then it might:<pre>
      *   Searcher searcher = new IndexSearcher(indexReader);
      *   final BitSet bits = new BitSet(indexReader.maxDoc());
      *   searcher.search(query, new HitCollector() {
      *       public void collect(int32_t doc, float score) {
      *         bits.set(doc);
      *       }
      *     });
      * </pre>
      *
      * <p>Note: This is called in an inner search loop.  For good search
      * performance, implementations of this method should not call
      * {@link Searcher#doc(int32_t)} or
      * {@link IndexReader#document(int32_t)} on every
      * document number encountered.  Doing so can slow searches by an order
      * of magnitude or more.
      * <p>Note: The <code>score</code> passed to this method is a raw score.
      * In other words, the score will not necessarily be a float whose value is
      * between 0 and 1.
      */
      virtual void collect(const int32_t doc, const qreal score) = 0;
		virtual ~HitCollector(){}
    };

   /** Expert: Calculate query weights and build query scorers.
   *
   * <p>A Weight is constructed by a query, given a Searcher ({@link
   * Query#_createWeight(Searcher)}).  The {@link #sumOfSquaredWeights()} method
   * is then called on the top-level query to compute the query normalization
   * factor (@link Similarity#queryNorm(qreal)}).  This factor is then passed to
   * {@link #normalize(qreal)}.  At this point the weighting is complete and a
   * scorer may be constructed by calling {@link #scorer(IndexReader)}.
   */
	class Weight: LUCENE_BASE {
    public:
		virtual ~Weight(){
		};

      /** The query that this concerns. */
      virtual Query* getQuery() = 0;

      /** The weight for this query. */
      virtual qreal getValue() = 0;

      /** The sum of squared weights of contained query clauses. */
      virtual qreal sumOfSquaredWeights() = 0;

      /** Assigns the query normalization factor to this. */
      virtual void normalize(qreal norm) = 0;

      /** Constructs a scorer for this. */
      virtual Scorer* scorer(CL_NS(index)::IndexReader* reader) = 0;

      /** An explanation of the score computation for the named document. */
      virtual void explain(CL_NS(index)::IndexReader* reader, int32_t doc, Explanation* ret) = 0;

      virtual TCHAR* toString(){
         return STRDUP_TtoT(_T("Weight"));
      }
   };

   class HitDoc:LUCENE_BASE {
    public:
		qreal score;
		int32_t id;
		CL_NS(document)::Document* doc;
		
		HitDoc* next;					  // in doubly-linked cache
		HitDoc* prev;					  // in doubly-linked cache
		
		HitDoc(const qreal s, const int32_t i);
		~HitDoc();
    };



    // A ranked list of documents, used to hold search results. 
   class Hits:LUCENE_BASE {
    private:
		Query* query;
		Searcher* searcher;
		Filter* filter;
		const Sort* sort;

		size_t _length;				  // the total number of hits
		CL_NS(util)::CLVector<HitDoc*, CL_NS(util)::Deletor::Object<HitDoc> > hitDocs;	  // cache of hits retrieved

		HitDoc* first;				  // head of LRU cache
		HitDoc* last;				  // tail of LRU cache
		int32_t numDocs;			  // number cached
		int32_t maxDocs;			  // max to cache

    public:
		Hits(Searcher* s, Query* q, Filter* f, const Sort* sort=NULL);
		~Hits();

		/** Returns the total number of hits available in this set. */
		int32_t length() const;
	    
		/** Returns the stored fields of the n<sup>th</sup> document in this set.
		<p>Documents are cached, so that repeated requests for the same element may
		return the same Document object. 
		*
		* @memory Memory belongs to the hits object. Don't delete the return value.
		*/
		CL_NS(document)::Document& doc(const int32_t n);
	      
		/** Returns the id for the nth document in this set. */
		int32_t id (const int32_t n);
	    
		/** Returns the score for the nth document in this set. */
		qreal score(const int32_t n);
	      
	private:
		// Tries to add new documents to hitDocs.
		// Ensures that the hit numbered <code>_min</code> has been retrieved.
		void getMoreDocs(const size_t _min);
	    
		HitDoc* getHitDoc(const size_t n);
	    
		void addToFront(HitDoc* hitDoc);
	    
		void remove(const HitDoc* hitDoc);

  };

   /** The interface for search implementations.
   *
   * <p>Implementations provide search over a single index, over multiple
   * indices, and over indices on remote servers.
   */
   class Searchable: LUCENE_BASE {
   public:
   	virtual ~Searchable(){
	}

      /** Lower-level search API.
      *
      * <p>{@link HitCollector#collect(int32_t,qreal)} is called for every non-zero
      * scoring document.
      *
      * <p>Applications should only use this if they need <i>all</i> of the
      * matching documents.  The high-level search API ({@link
      * Searcher#search(Query*)}) is usually more efficient, as it skips
      * non-high-scoring hits.
      *
      * @param query to match documents
      * @param filter if non-null, a bitset used to eliminate some documents
      * @param results to receive hits
      */
      virtual void _search(Query* query, Filter* filter, HitCollector* results) = 0;

      /** Frees resources associated with this Searcher.
      * Be careful not to call this method while you are still using objects
      * like {@link Hits}.
      */
      virtual void close() = 0;

      /** Expert: Returns the number of documents containing <code>term</code>.
      * Called by search code to compute term weights.
      * @see IndexReader#docFreq(Term).
      */
      virtual int32_t docFreq(const CL_NS(index)::Term* term) const = 0;

      /** Expert: Returns one greater than the largest possible document number.
      * Called by search code to compute term weights.
      * @see IndexReader#maxDoc().
      */
      virtual int32_t maxDoc() const = 0;

      /** Expert: Low-level search implementation.  Finds the top <code>n</code>
      * hits for <code>query</code>, applying <code>filter</code> if non-null.
      *
      * <p>Called by {@link Hits}.
      *
      * <p>Applications should usually call {@link Searcher#search(Query*)} or
      * {@link Searcher#search(Query*,Filter*)} instead.
      */
      virtual TopDocs* _search(Query* query, Filter* filter, const int32_t n) = 0;

      /** Expert: Returns the stored fields of document <code>i</code>.
      * Called by {@link HitCollector} implementations.
      * @see IndexReader#document(int32_t).
      */
      virtual bool doc(int32_t i, CL_NS(document)::Document* d) = 0;
      _CL_DEPRECATED( doc(i, document) ) CL_NS(document)::Document* doc(const int32_t i);

      /** Expert: called to re-write queries into primitive queries. */
      virtual Query* rewrite(Query* query) = 0;

      /** Returns an Explanation that describes how <code>doc</code> scored against
      * <code>query</code>.
      *
      * <p>This is intended to be used in developing Similarity implementations,
      * and, for good performance, should not be displayed with every hit.
      * Computing an explanation is as expensive as executing the query over the
      * entire index.
      */
      virtual void explain(Query* query, int32_t doc, Explanation* ret) = 0;

      /** Expert: Low-level search implementation with arbitrary sorting.  Finds
      * the top <code>n</code> hits for <code>query</code>, applying
      * <code>filter</code> if non-null, and sorting the hits by the criteria in
      * <code>sort</code>.
      *
      * <p>Applications should usually call {@link
      * Searcher#search(Query,Filter,Sort)} instead.
      */
	  	virtual TopFieldDocs* _search(Query* query, Filter* filter, const int32_t n, const Sort* sort) = 0;
   };



	/** An abstract base class for search implementations.
	* Implements some common utility methods.
	*/
	class Searcher:public Searchable {
	private:
		/** The Similarity implementation used by this searcher. */
		Similarity* similarity;

		public:
		Searcher(){
			similarity = Similarity::getDefault();
		}
		virtual ~Searcher(){
		}

		// Returns the documents matching <code>query</code>.
		Hits* search(Query* query) {
			return search(query, (Filter*)NULL );
		}

		// Returns the documents matching <code>query</code> and
		//	<code>filter</code>. 
		Hits* search(Query* query, Filter* filter) {
			return _CLNEW Hits(this, query, filter);
		}

		/** Returns documents matching <code>query</code> sorted by
		* <code>sort</code>.
		*/
		Hits* search(Query* query, const Sort* sort){
			return _CLNEW Hits(this, query, NULL, sort);
		}

		/** Returns documents matching <code>query</code> and <code>filter</code>,
			* sorted by <code>sort</code>.
			*/
		Hits* search(Query* query, Filter* filter, const Sort* sort){
			return _CLNEW Hits(this, query, filter, sort);
		}

		/** Lower-level search API.
		*
		* <p>{@link HitCollector#collect(int32_t	,qreal)} is called for every non-zero
		* scoring document.
		*
		* <p>Applications should only use this if they need <i>all</i> of the
		* matching documents.  The high-level search API ({@link
		* Searcher#search(Query*)}) is usually more efficient, as it skips
		* non-high-scoring hits.
		* <p>Note: The <code>score</code> passed to this method is a raw score.
		* In other words, the score will not necessarily be a float whose value is
		* between 0 and 1.
		*/
		void _search(Query* query, HitCollector* results) {
			Searchable::_search(query, NULL, results);
		}

		/** Expert: Set the Similarity implementation used by this Searcher.
		*
		* @see Similarity#setDefault(Similarity)
		*/
		void setSimilarity(Similarity* similarity) {
			this->similarity = similarity;
		}

		/** Expert: Return the Similarity implementation used by this Searcher.
		*
		* <p>This defaults to the current value of {@link Similarity#getDefault()}.
		*/
		Similarity* getSimilarity(){
			return this->similarity;
		}
	};

	/** The abstract base class for queries.
    <p>Instantiable subclasses are:
    <ul>
    <li> {@link TermQuery}
    <li> {@link MultiTermQuery}
    <li> {@link BooleanQuery}
    <li> {@link WildcardQuery}
    <li> {@link PhraseQuery}
    <li> {@link PrefixQuery}
    <li> {@link PhrasePrefixQuery}
    <li> {@link FuzzyQuery}
    <li> {@link RangeQuery}
    <li> {@link spans.SpanQuery}
    </ul>
    <p>A parser for queries is contained in:
    <ul>
    <li>{@link queryParser.QueryParser QueryParser}
    </ul>
	*/
	class Query :LUCENE_BASE {
	private:
		// query boost factor
		qreal boost;
	protected:
		Query(const Query& clone);
	public:
		Query();
		virtual ~Query();

		/** Sets the boost for this query clause to <code>b</code>.  Documents
		* matching this clause will (in addition to the normal weightings) have
		* their score multiplied by <code>b</code>.
		*/
		void setBoost(qreal b);

		/** Gets the boost for this clause.  Documents matching
		* this clause will (in addition to the normal weightings) have their score
		* multiplied by <code>b</code>.   The boost is 1.0 by default.
		*/
		qreal getBoost() const;

        /** Expert: Constructs an initializes a Weight for a top-level query. */
        Weight* weight(Searcher* searcher);

        /** Expert: called to re-write queries into primitive queries. */
        virtual Query* rewrite(CL_NS(index)::IndexReader* reader);
        
        /** Expert: called when re-writing queries under MultiSearcher.
         *
         * <p>Only implemented by derived queries, with no
         * {@link #_createWeight(Searcher)} implementatation.
         */
         virtual Query* combine(Query** queries);

        /** Expert: merges the clauses of a set of BooleanQuery's into a single
         * BooleanQuery.
         *
         *<p>A utility for use by {@link #combine(Query[])} implementations.
	     */
        static Query* mergeBooleanQueries(Query** queries);

        /** Expert: Returns the Similarity implementation to be used for this query.
        * Subclasses may override this method to specify their own Similarity
        * implementation, perhaps one that delegates through that of the Searcher.
        * By default the Searcher's Similarity implementation is returned.*/
        Similarity* getSimilarity(Searcher* searcher);
        
        /** Returns a clone of this query. */
        virtual Query* clone() const = 0;
        virtual const TCHAR* getQueryName() const = 0;
        bool instanceOf(const TCHAR* other) const;
        
        /** Prints a query to a string, with <code>field</code> as the default field
        * for terms.  <p>The representation used is one that is readable by
        * {@link queryParser.QueryParser QueryParser}
        * (although, if the query was created by the parser, the printed
        * representation may not be exactly what was parsed).
        */
        virtual TCHAR* toString(const TCHAR* field) const = 0;
        
        virtual bool equals(Query* other) const = 0;
        virtual size_t hashCode() const = 0;
        
        /** Prints a query to a string. */
        TCHAR* toString() const;

		
		/** Expert: Constructs an appropriate Weight implementation for this query.
		*
		* <p>Only implemented by primitive queries, which re-write to themselves.
		* <i>This is an Internal function</i>
		*/
		virtual Weight* _createWeight(Searcher* searcher);
		
	};


CL_NS_END
#endif
