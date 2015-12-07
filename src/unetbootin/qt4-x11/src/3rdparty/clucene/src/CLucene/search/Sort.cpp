/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#include "CLucene/StdHeader.h"
#include "Sort.h"
#include "Compare.h"

CL_NS_USE(util)
CL_NS_DEF(search)


		
  /** Represents sorting by document score (relevancy). */
  SortField* SortField::FIELD_SCORE = _CLNEW SortField (NULL, DOCSCORE,false);

  /** Represents sorting by document number (index order). */
  SortField* SortField::FIELD_DOC = _CLNEW SortField (NULL, DOC,false);

  
	/** Represents sorting by computed relevance. Using this sort criteria
	 * returns the same results as calling {@link Searcher#search(Query) Searcher#search()}
	 * without a sort criteria, only with slightly more overhead. */
   Sort* Sort::RELEVANCE = _CLNEW Sort();

	/** Represents sorting by index order. */
   Sort* Sort::INDEXORDER = _CLNEW Sort (SortField::FIELD_DOC);




   /** Creates a sort by terms in the given field where the type of term value
   * is determined dynamically ({@link #AUTO AUTO}).
   * @param field Name of field to sort by, cannot be <code>null</code>.
   */
  SortField::SortField (const TCHAR* field) {
     this->type = AUTO;
     this->reverse = false;
     this->field = CLStringIntern::intern(field  CL_FILELINE);
	 this->factory = NULL;
  }

  /** Creates a sort, possibly in reverse, by terms in the given field where
   * the type of term value is determined dynamically ({@link #AUTO AUTO}).
   * @param field Name of field to sort by, cannot be <code>null</code>.
   * @param reverse True if natural order should be reversed.
   
  SortField::SortField (const TCHAR* field, bool reverse) {
    this->field = CLStringIntern::intern(field  CL_FILELINE);
    this->reverse = reverse;
     this->type = AUTO;
	 this->factory = NULL;
  }*/


  /** Creates a sort, possibly in reverse, by terms in the given field with the
   * type of term values explicitly given.
   * @param field  Name of field to sort by.  Can be <code>null</code> if
   *               <code>type</code> is SCORE or DOC.
   * @param type   Type of values in the terms.
   * @param reverse True if natural order should be reversed (default=false).
   */
  SortField::SortField (const TCHAR* field, int32_t type, bool reverse) {
    this->field = (field != NULL) ? CLStringIntern::intern(field  CL_FILELINE) : field;
    this->type = type;
    this->reverse = reverse;
	 this->factory = NULL;
  }
  
  SortField::SortField(const SortField& clone){
    this->field = (clone.field != NULL) ? CLStringIntern::intern(clone.field  CL_FILELINE) : clone.field;
    this->type = clone.type;
    this->reverse = clone.reverse;
	 this->factory = clone.factory;
  }
  SortField* SortField::clone() const{
   return _CLNEW SortField(*this); 
  }

  /** Creates a sort by terms in the given field sorted
   * according to the given locale.
   * @param field  Name of field to sort by, cannot be <code>null</code>.
   * @param locale Locale of values in the field.
   */
  /*SortField::SortField (TCHAR* field, Locale* locale) {
    this->field = (field != NULL) ? CLStringIntern::intern(field): field;
    this->type = STRING;
    this->locale = locale;
  }*/

  /** Creates a sort, possibly in reverse, by terms in the given field sorted
   * according to the given locale.
   * @param field  Name of field to sort by, cannot be <code>null</code>.
   * @param locale Locale of values in the field.
   */
  /*SortField::SortField (TCHAR* field, Locale* locale, bool reverse) {
     this->field = (field != NULL) ? CLStringIntern::intern(field): field;
    this->type = STRING;
    this->locale = locale;
    this->reverse = reverse;
  }*/


  /** Creates a sort, possibly in reverse, with a custom comparison function.
   * @param field Name of field to sort by; cannot be <code>null</code>.
   * @param comparator Returns a comparator for sorting hits.
   * @param reverse True if natural order should be reversed (default=false).
   */
  SortField::SortField (const TCHAR* field, SortComparatorSource* comparator, bool reverse) {
    this->field = (field != NULL) ? CLStringIntern::intern(field  CL_FILELINE): field;
    this->type = CUSTOM;
    this->reverse = reverse;
    this->factory = comparator;
  }

  SortField::~SortField(){
	  CLStringIntern::unintern(field);
  }
  
  TCHAR* SortField::toString() const {
	CL_NS(util)::StringBuffer buffer;
    switch (type) {
      case DOCSCORE: buffer.append(_T("<score>"));
                  break;

      case DOC: buffer.append(_T("<doc>"));
                break;

      case CUSTOM: buffer.append (_T("<custom:\""));
         buffer.append( field );
         buffer.append( _T("\": "));
         buffer.append(factory->getName());
         buffer.append(_T(">"));
         break;

      default: buffer.append( _T("\""));
            buffer.append( field );
            buffer.append( _T("\"") );
               break;
    }

    //if (locale != null) buffer.append ("("+locale+")"); todo:
    if (reverse) buffer.appendChar('!');

    return buffer.toString();
  }













   /** Sorts by computed relevance.  This is the same sort criteria as
	 * calling {@link Searcher#search(Query) Searcher#search()} without a sort criteria, only with
	 * slightly more overhead. */
	Sort::Sort() {
		fields=NULL;
		SortField** fields=_CL_NEWARRAY(SortField*,3);
		fields[0]=SortField::FIELD_SCORE;
		fields[1]=SortField::FIELD_DOC;
		fields[2]=NULL;
		setSort (fields);
        _CLDELETE_ARRAY(fields);
	}

	Sort::~Sort(){
		clear();
	}
	void Sort::clear(){
		if ( fields != NULL ){
			int32_t i=0;
			while ( fields[i] != NULL ){
				if ( fields[i] != SortField::FIELD_SCORE &&
					 fields[i] != SortField::FIELD_DOC ){
					_CLDELETE(fields[i]);
				}
				i++;
			}
			_CLDELETE_ARRAY(fields);
		}
	}

	/** Sorts possibly in reverse by the terms in <code>field</code> then by
	 * index order (document number). The type of value in <code>field</code> is determined
	 * automatically.
	 * @see SortField#AUTO
	 */
	Sort::Sort (const TCHAR* field, bool reverse) {
		this->fields=NULL;
		setSort (field, reverse);
	}


	/** Sorts in succession by the terms in each field.
	 * The type of value in <code>field</code> is determined
	 * automatically.
	 * @see SortField#AUTO
	 */
	Sort::Sort (const TCHAR** fields) {
		this->fields=NULL;
		setSort (fields);
	}


	/** Sorts by the criteria in the given SortField. */
	Sort::Sort (SortField* field) {
		this->fields=NULL;
		setSort (field);
	}


	/** Sorts in succession by the criteria in each SortField. */
	Sort::Sort (SortField** fields) {
		this->fields=NULL;
		setSort (fields);
	}


	/** Sets the sort to the terms in <code>field</code> possibly in reverse,
	 * then by index order (document number). */
	void Sort::setSort (const TCHAR* field, bool reverse) {
		clear();
		fields = _CL_NEWARRAY(SortField*,3);
		fields[0] = _CLNEW SortField (field, SortField::AUTO, reverse);
		fields[1] = SortField::FIELD_DOC;
		fields[2] = NULL;
	}


	/** Sets the sort to the terms in each field in succession. */
	void Sort::setSort (const TCHAR** fieldnames) {
		clear();

		int32_t n = 0;
		while ( fieldnames[n] != NULL )
		   n++;

		fields = _CL_NEWARRAY(SortField*,n+1);
		for (int32_t i = 0; i < n; ++i) {
			fields[i] = _CLNEW SortField (fieldnames[i], SortField::AUTO,false);
		}
		fields[n]=NULL;
	}


	/** Sets the sort to the given criteria. */
	void Sort::setSort (SortField* field) {
		clear();

        this->fields = _CL_NEWARRAY(SortField*,2);
		this->fields[0] = field;
		this->fields[1] = NULL;
	}


	/** Sets the sort to the given criteria in succession. */
	void Sort::setSort (SortField** fields) {
		clear();
        
        int n=0;
		while ( fields[n] != NULL )
		   n++;
        this->fields = _CL_NEWARRAY(SortField*,n+1);
        for (int i=0;i<n+1;i++)
            this->fields[i]=fields[i];
	}

	TCHAR* Sort::toString() const {
		CL_NS(util)::StringBuffer buffer;

		int32_t i = 0;
		while ( fields[i] != NULL ){
			if (i>0)
				buffer.appendChar(',');

			const TCHAR* p = fields[i]->toString();
			buffer.append(p);
			_CLDELETE_CARRAY(p);
			  
			i++;
		}

		return buffer.toString();
	}





  ScoreDocComparator* ScoreDocComparator::INDEXORDER = _CLNEW ScoreDocComparators::IndexOrder;
  ScoreDocComparator* ScoreDocComparator::RELEVANCE = _CLNEW ScoreDocComparators::Relevance;

  ScoreDocComparator::~ScoreDocComparator(){
  }


class ScoreDocComparatorImpl: public ScoreDocComparator{
    Comparable** cachedValues;
	FieldCacheAuto* fca;
	int32_t cachedValuesLen;
public:
	ScoreDocComparatorImpl(FieldCacheAuto* fca){
		this->fca = fca;
		if ( fca->contentType != FieldCacheAuto::COMPARABLE_ARRAY )
		_CLTHROWA(CL_ERR_InvalidCast,"Invalid field cache auto type");
		this->cachedValues = fca->comparableArray;
		this->cachedValuesLen = fca->contentLen;
	}
	~ScoreDocComparatorImpl(){
	}
	int32_t compare (struct ScoreDoc* i, struct ScoreDoc* j){
		CND_PRECONDITION(i->doc >= 0 && i->doc < cachedValuesLen, "i->doc out of range")
		CND_PRECONDITION(j->doc >= 0 && j->doc < cachedValuesLen, "j->doc out of range")
		return cachedValues[i->doc]->compareTo (cachedValues[j->doc]);
	}

	CL_NS(util)::Comparable* sortValue (struct ScoreDoc* i){
		CND_PRECONDITION(i->doc >= 0 && i->doc < cachedValuesLen, "i->doc out of range")
		return cachedValues[i->doc];
	}

	int32_t sortType(){
		return SortField::CUSTOM;
	}
};
    
ScoreDocComparator* SortComparator::newComparator (CL_NS(index)::IndexReader* reader, const TCHAR* fieldname){
	return _CLNEW ScoreDocComparatorImpl(FieldCache::DEFAULT->getCustom (reader, fieldname, this));
}
SortComparator::SortComparator(){
}
SortComparator::~SortComparator(){
}


CL_NS_END
