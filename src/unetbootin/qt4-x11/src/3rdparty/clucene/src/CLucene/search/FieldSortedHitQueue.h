/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#ifndef _lucene_search_FieldSortedHitQueue_
#define _lucene_search_FieldSortedHitQueue_

#if defined(_LUCENE_PRAGMA_ONCE)
# pragma once
#endif

#include "FieldCache.h"
#include "Sort.h"
#include "FieldDocSortedHitQueue.h"
#include "SearchHeader.h"
#include "FieldCacheImpl.h"
#include "CLucene/util/PriorityQueue.h"

CL_NS_DEF(search)


/**
 * Expert: A hit queue for sorting by hits by terms in more than one field.
 * Uses <code>FieldCache.DEFAULT</code> for maintaining internal term lookup tables.
 *
 * @see Searchable#search(Query,Filter,int32_t,Sort)
 * @see FieldCache
 */
class FieldSortedHitQueue: public CL_NS(util)::PriorityQueue<FieldDoc*, 
	CL_NS(util)::Deletor::Object<FieldDoc> > {

    ///the type that is stored in the field cache. can't use a typedef because
    ///the decorated name would become too long
    class hitqueueCacheReaderType: public CL_NS(util)::CLHashMap<FieldCacheImpl::FileEntry*,
	    ScoreDocComparator*, 
        FieldCacheImpl::FileEntry::Compare,
        FieldCacheImpl::FileEntry::Equals,
	    CL_NS(util)::Deletor::Object<FieldCacheImpl::FileEntry>,
	    CL_NS(util)::Deletor::Object<ScoreDocComparator> >{

    public:
	    hitqueueCacheReaderType(bool deleteValue){
		    setDeleteKey(true);
		    setDeleteValue(deleteValue);
	    }
	    ~hitqueueCacheReaderType(){
            clear();
	    }

    };

public: //todo: remove this and below after close callback is implemented
	//note: typename gets too long if using cacheReaderType as a typename
	typedef CL_NS(util)::CLHashMap<CL_NS(index)::IndexReader*, 
		hitqueueCacheReaderType*, 
		CL_NS(util)::Compare::Void<CL_NS(index)::IndexReader>,
		CL_NS(util)::Equals::Void<CL_NS(index)::IndexReader>,
		CL_NS(util)::Deletor::Object<CL_NS(index)::IndexReader>, 
		CL_NS(util)::Deletor::Object<hitqueueCacheReaderType> > hitqueueCacheType; 

	/** Internal cache of comparators. Similar to FieldCache, only
	*  caches comparators instead of term values. 
	*/
	static hitqueueCacheType Comparators;
private:
	
	/** Returns a comparator if it is in the cache.*/
	static ScoreDocComparator* lookup (CL_NS(index)::IndexReader* reader, const TCHAR* field, int32_t type, SortComparatorSource* factory);
	
	/** Stores a comparator into the cache. 
		returns the valid ScoreDocComparator.
	*/
	static void store (CL_NS(index)::IndexReader* reader, const TCHAR* field, int32_t type, SortComparatorSource* factory, ScoreDocComparator* value);

  
  //todo: Locale locale, not implemented yet
  static ScoreDocComparator* getCachedComparator (CL_NS(index)::IndexReader* reader, 
  	const TCHAR* fieldname, int32_t type, SortComparatorSource* factory);
  	
  	
  /**
   * Returns a comparator for sorting hits according to a field containing integers.
   * @param reader  Index to use.
   * @param fieldname  Field containg integer values.
   * @return  Comparator for sorting hits.
   * @throws IOException If an error occurs reading the index.
   */
  static ScoreDocComparator* comparatorInt (CL_NS(index)::IndexReader* reader, const TCHAR* fieldname);

  /**
   * Returns a comparator for sorting hits according to a field containing floats.
   * @param reader  Index to use.
   * @param fieldname  Field containg float values.
   * @return  Comparator for sorting hits.
   * @throws IOException If an error occurs reading the index.
   */
  static ScoreDocComparator* comparatorFloat (CL_NS(index)::IndexReader* reader, const TCHAR* fieldname);

  /**
   * Returns a comparator for sorting hits according to a field containing strings.
   * @param reader  Index to use.
   * @param fieldname  Field containg string values.
   * @return  Comparator for sorting hits.
   * @throws IOException If an error occurs reading the index.
   */
  static ScoreDocComparator* comparatorString (CL_NS(index)::IndexReader* reader, const TCHAR* fieldname);


  //todo:
  /**
   * Returns a comparator for sorting hits according to a field containing strings.
   * @param reader  Index to use.
   * @param fieldname  Field containg string values.
   * @return  Comparator for sorting hits.
   * @throws IOException If an error occurs reading the index.
   
  static ScoreDocComparator* comparatorStringLocale (IndexReader* reader, TCHAR* fieldname, Locale locale){
    Collator collator = Collator.getInstance (locale);
    TCHAR* field = fieldname.intern();
    TCHAR** index = FieldCache.DEFAULT.getStrings (reader, field);
    return _CLNEW ScoreDocComparator() {

      public int32_t compare (ScoreDoc i, ScoreDoc j) {
        return collator.compare (index[i.doc], index[j.doc]);
      }

      public Comparable sortValue (ScoreDoc i) {
        return index[i.doc];
      }

      public int32_t sortType() {
        return SortField.STRING;
      }
    };
  }*/

  /**
   * Returns a comparator for sorting hits according to values in the given field.
   * The terms in the field are looked at to determine whether they contain integers,
   * floats or strings.  Once the type is determined, one of the other static methods
   * in this class is called to get the comparator.
   * @param reader  Index to use.
   * @param fieldname  Field containg values.
   * @return  Comparator for sorting hits.
   * @throws IOException If an error occurs reading the index.
   */
  static ScoreDocComparator* comparatorAuto (CL_NS(index)::IndexReader* reader, const TCHAR* fieldname);


protected:
  /** Stores a comparator corresponding to each field being sorted by */
  ScoreDocComparator** comparators;
  int32_t comparatorsLen;
  
  /** Stores the sort criteria being used. */
  SortField** fields;
  int32_t fieldsLen;

  /** Stores the maximum score value encountered, for normalizing.
   *  we only care about scores greater than 1.0 - if all the scores
   *  are less than 1.0, we don't have to normalize. */
  qreal maxscore;

  /**
   * Returns whether <code>a</code> is less relevant than <code>b</code>.
   * @param a ScoreDoc
   * @param b ScoreDoc
   * @return <code>true</code> if document <code>a</code> should be sorted after document <code>b</code>.
   */
  bool lessThan (FieldDoc* docA, FieldDoc* docB);
public:

  /**
   * Creates a hit queue sorted by the given list of fields.
   * @param reader  Index to use.
   * @param fields Field names, in priority order (highest priority first).  Cannot be <code>null</code> or empty.
   * @param size  The number of hits to retain.  Must be greater than zero.
   * @throws IOException
   */
	FieldSortedHitQueue (CL_NS(index)::IndexReader* reader, SortField** fields, int32_t size);

    ~FieldSortedHitQueue();

	/**
	* Callback for when IndexReader closes. This causes
	* any Comparators to be removed for the specified reader.
	*/
	static void closeCallback(CL_NS(index)::IndexReader* reader, void* param);
		
  /**
   * Given a FieldDoc object, stores the values used
   * to sort the given document.  These values are not the raw
   * values out of the index, but the internal representation
   * of them.  This is so the given search hit can be collated
   * by a MultiSearcher with other search hits.
   * @param  doc  The FieldDoc to store sort values into.
   * @return  The same FieldDoc passed in.
   * @see Searchable#search(Query,Filter,int32_t,Sort)
   */
	FieldDoc* fillFields (FieldDoc* doc) const;

	void setFields (SortField** fields){
		this->fields = fields;
	}

  	/** Returns the SortFields being used by this hit queue. */
	SortField** getFields() {
	return fields;
	}
};


CL_NS_END
#endif
