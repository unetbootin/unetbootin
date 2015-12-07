/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#ifndef _lucene_search_FieldDocSortedHitQueue_
#define _lucene_search_FieldDocSortedHitQueue_

#if defined(_LUCENE_PRAGMA_ONCE)
# pragma once
#endif

#include "Sort.h"
#include "CLucene/util/PriorityQueue.h"

CL_NS_DEF(search)

/**
 * Expert: A ScoreDoc which also contains information about
 * how to sort the referenced document.  In addition to the
 * document number and score, this object contains an array
 * of values for the document from the field(s) used to sort.
 * For example, if the sort criteria was to sort by fields
 * "a", "b" then "c", the <code>fields</code> object array
 * will have three elements, corresponding respectively to
 * the term values for the document in fields "a", "b" and "c".
 * The class of each element in the array will be either
 * Integer, Float or String depending on the type of values
 * in the terms of each field.
 *
 * @see ScoreDoc
 * @see TopFieldDocs
 */
class FieldDoc: LUCENE_BASE {
public:
	//FieldDoc did inherit from ScoreDoc, but now we make the scoredoc a member
	struct ScoreDoc scoreDoc;

	/** Expert: The values which are used to sort the referenced document.
	 * The order of these will match the original sort criteria given by a
	 * Sort object.  Each Object will be either an Integer, Float or String,
	 * depending on the type of values in the terms of the original field.
	 * @see Sort
	 * @see Searchable#search(Query,Filter,int32_t,Sort)
	 */
	CL_NS(util)::Comparable** fields;

	/** Expert: Creates one of these objects with empty sort information. */
	FieldDoc (int32_t doc, qreal score);
	/** Expert: Creates one of these objects with the given sort information. */
	FieldDoc (int32_t doc, qreal score, CL_NS(util)::Comparable** fields);
    ~FieldDoc();
};

/**
 * Expert: Collects sorted results from Searchable's and collates them.
 * The elements put into this queue must be of type FieldDoc.
 */
class FieldDocSortedHitQueue: 
	public CL_NS(util)::PriorityQueue<FieldDoc*,CL_NS(util)::Deletor::Object<FieldDoc> > 
{
private:
    DEFINE_MUTEX(THIS_LOCK)

	// this cannot contain AUTO fields - any AUTO fields should
	// have been resolved by the time this class is used.
	SortField** fields;
	int32_t fieldsLen;
	
	void _countsize(){
		fieldsLen=0;
		while(fields[fieldsLen]!=NULL)
			fieldsLen++;
	}

	// used in the case where the fields are sorted by locale
	// based strings
	//todo: not implemented in clucene because locales has not been implemented
	//Collator[] collators; //volatile 

public:
	/**
	 * Creates a hit queue sorted by the given list of fields.
	 * @param fields Field names, in priority order (highest priority first).
	 * @param size  The number of hits to retain.  Must be greater than zero.
	 */
	FieldDocSortedHitQueue (SortField** fields, int32_t size);
    ~FieldDocSortedHitQueue();


	/**
	 * Allows redefinition of sort fields if they are <code>NULL</code>.
	 * This is to handle the case using ParallelMultiSearcher where the
	 * original list contains AUTO and we don't know the actual sort
	 * type until the values come back.  The fields can only be set once.
	 * This method is thread safe.
	 * @param fields
	 */
	void setFields (SortField** fields);

	/** Returns the fields being used to sort. */
	SortField** getFields() {
		return fields;
	}

	/** Returns an array of collators, possibly <code>NULL</code>.  The collators
	 * correspond to any SortFields which were given a specific locale.
	 * @param fields Array of sort fields.
	 * @return Array, possibly <code>NULL</code>.
	 
	private Collator[] hasCollators (SortField[] fields) {
		if (fields == NULL) return NULL;
		Collator[] ret = new Collator[fields.length];
		for (int32_t i=0; i<fields.length; ++i) {
			Locale locale = fields[i].getLocale();
			if (locale != NULL)
				ret[i] = Collator.getInstance (locale);
		}
		return ret;
	}*/

protected:
	/**
	 * Returns whether <code>a</code> is less relevant than <code>b</code>.
	 * @param a FieldDoc
	 * @param b FieldDoc
	 * @return <code>true</code> if document <code>a</code> should be sorted after document <code>b</code>.
	 */
	bool lessThan (FieldDoc* docA, FieldDoc* docB);
};


/**
* Expert: Returned by low-level sorted search implementations.
*
* @see Searchable#search(Query,Filter,int32_t,Sort)
*/
class TopFieldDocs: public TopDocs {
public:
	/// The fields which were used to sort results by.
	SortField** fields;

	FieldDoc** fieldDocs;

   /** Creates one of these objects.
   * @param totalHits  Total number of hits for the query.
   * @param fieldDocs  The top hits for the query.
   * @param scoreDocs  The top hits for the query.
   * @param scoreDocsLen  Length of fieldDocs and scoreDocs
   * @param fields     The sort criteria used to find the top hits.
   */
  TopFieldDocs (int32_t totalHits, FieldDoc** fieldDocs, int32_t scoreDocsLen, SortField** fields);
	~TopFieldDocs();
};

CL_NS_END
#endif

