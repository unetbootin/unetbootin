/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#include "CLucene/StdHeader.h"

#include "SearchHeader.h"
#include "CLucene/document/Document.h"
#include "CLucene/index/IndexReader.h"
#include "Filter.h"
#include "CLucene/search/SearchHeader.h"

CL_NS_USE(document)
CL_NS_USE(util)
CL_NS_USE(index)

CL_NS_DEF(search)

	HitDoc::HitDoc(const qreal s, const int32_t i)
	{
	//Func - Constructor
	//Pre  - true
	//Post - The instance has been created

		next  = NULL;
		prev  = NULL;
		doc   = NULL;
		score = s;
		id    = i;
	}

	HitDoc::~HitDoc(){
	//Func - Destructor
	//Pre  - true
	//Post - The instance has been destroyed

		_CLDELETE(doc);
	}


	Hits::Hits(Searcher* s, Query* q, Filter* f, const Sort* _sort):
		query(q), searcher(s), filter(f), sort(_sort)
	{
	//Func - Constructor
	//Pre  - s contains a valid reference to a searcher s
	//       q contains a valid reference to a Query
	//       f is NULL or contains a pointer to a filter
	//Post - The instance has been created

		_length  = 0;
		first   = NULL;
		last    = NULL;
		numDocs = 0;
		maxDocs = 200;

		//retrieve 100 initially
		getMoreDocs(50);
	}

	Hits::~Hits(){

	}
	int32_t Hits::length() const {
		return _length;
	}

	Document& Hits::doc(const int32_t n){
		HitDoc* hitDoc = getHitDoc(n);

		// Update LRU cache of documents
		remove(hitDoc);				  // remove from list, if there
		addToFront(hitDoc);				  // add to front of list
		if (numDocs > maxDocs) {			  // if cache is full
			HitDoc* oldLast = last;
			remove(last);				  // flush last

			_CLDELETE( oldLast->doc );
			oldLast->doc = NULL;
		}

		if (hitDoc->doc == NULL){
			hitDoc->doc = _CLNEW Document;
			searcher->doc(hitDoc->id, hitDoc->doc);	  // cache miss: read document
		}

		return *hitDoc->doc;
	}

	int32_t Hits::id (const int32_t n){
		return getHitDoc(n)->id;
	}

	qreal Hits::score(const int32_t n){
		return getHitDoc(n)->score;
	}

	void Hits::getMoreDocs(const size_t m){
		size_t _min = m;
		{
			size_t nHits = hitDocs.size();
			if ( nHits > _min)
				_min = nHits;
		}

		size_t n = _min * 2;				  // double # retrieved
		TopDocs* topDocs = NULL;
		if ( sort==NULL )
			topDocs = (TopDocs*)((Searchable*)searcher)->_search(query, filter, n);
		else
			topDocs = (TopDocs*)((Searchable*)searcher)->_search(query, filter, n, sort);
		_length = topDocs->totalHits;
		ScoreDoc* scoreDocs = topDocs->scoreDocs;
		int32_t scoreDocsLength = topDocs->scoreDocsLength;

		qreal scoreNorm = 1.0f;
		//Check that scoreDocs is a valid pointer before using it
		if (scoreDocs != NULL){
			if (_length > 0 && scoreDocs[0].score > 1.0f){
				scoreNorm = 1.0f / scoreDocs[0].score;
			}

			int32_t end = scoreDocsLength < _length ? scoreDocsLength : _length;
			for (int32_t i = hitDocs.size(); i < end; i++) {
				hitDocs.push_back(_CLNEW HitDoc(scoreDocs[i].score*scoreNorm, scoreDocs[i].doc));
			}
		}

		_CLDELETE(topDocs);
	}

	HitDoc* Hits::getHitDoc(const size_t n){
		if (n >= _length){
		    TCHAR buf[100];
            _sntprintf(buf, 100,_T("Not a valid hit number: %d"),n);
			_CLTHROWT(CL_ERR_IndexOutOfBounds, buf );
		}
		if (n >= hitDocs.size())
			getMoreDocs(n);

		return hitDocs[n];
	}

	void Hits::addToFront(HitDoc* hitDoc) {  // insert at front of cache
		if (first == NULL)
			last = hitDoc;
		else
			first->prev = hitDoc;

		hitDoc->next = first;
		first = hitDoc;
		hitDoc->prev = NULL;

		numDocs++;
	}

	void Hits::remove(const HitDoc* hitDoc) {	  // remove from cache
		if (hitDoc->doc == NULL)			  // it's not in the list
			return;					  // abort

		if (hitDoc->next == NULL)
			last = hitDoc->prev;
		else
			hitDoc->next->prev = hitDoc->prev;

		if (hitDoc->prev == NULL)
			first = hitDoc->next;
		else
			hitDoc->prev->next = hitDoc->next;

		numDocs--;
	}
CL_NS_END
