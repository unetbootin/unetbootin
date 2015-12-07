/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#include "CLucene/StdHeader.h"
#include "PhraseQuery.h"

#include "SearchHeader.h"
#include "Scorer.h"
#include "BooleanQuery.h"
#include "TermQuery.h"

#include "CLucene/index/Term.h"
#include "CLucene/index/Terms.h"
#include "CLucene/index/IndexReader.h"

#include "CLucene/util/StringBuffer.h"
#include "CLucene/util/VoidList.h"
#include "CLucene/util/Arrays.h"

#include "ExactPhraseScorer.h"
#include "SloppyPhraseScorer.h"

CL_NS_USE(index)
CL_NS_USE(util)
CL_NS_DEF(search)

  PhraseQuery::PhraseQuery():
	terms(false)
  {
  //Func - Constructor
  //Pre  - true
  //Post - An empty PhraseQuery has been created
  
      slop   = 0;

	  field = NULL;
  }
  PhraseQuery::PhraseQuery(const PhraseQuery& clone):
	Query(clone), terms(false)
  {
      slop  = clone.slop;
	  field = clone.field;
	  int32_t size=clone.positions.size();
	  { //msvc6 scope fix
		  for ( int32_t i=0;i<size;i++ ){
			  int32_t n = clone.positions[i];
			  this->positions.push_back( n );
		  }
	  }
	  size=clone.terms.size();
	  { //msvc6 scope fix
		  for ( int32_t i=0;i<size;i++ ){
			  this->terms.push_back( _CL_POINTER(clone.terms[i]));
		  }
	  }
  }
  Query* PhraseQuery::clone() const{
	  return _CLNEW PhraseQuery(*this);
  }
  bool PhraseQuery::equals(CL_NS(search)::Query *other) const{
	  if (!(other->instanceOf(PhraseQuery::getClassName())))
            return false;

    PhraseQuery* pq = (PhraseQuery*)other;
    bool ret = (this->getBoost() == pq->getBoost())
      && (this->slop == pq->slop);
	
		if ( ret ){
			CLListEquals<CL_NS(index)::Term,Term::Equals,
				const CL_NS(util)::CLVector<CL_NS(index)::Term*>,
				const CL_NS(util)::CLVector<CL_NS(index)::Term*> > comp;
			ret = comp.equals(&this->terms,&pq->terms);
		}
	
		if ( ret ){
			CLListEquals<int32_t,Equals::Int32,
				const CL_NS(util)::CLVector<int32_t,CL_NS(util)::Deletor::DummyInt32>,
				const CL_NS(util)::CLVector<int32_t,CL_NS(util)::Deletor::DummyInt32> > comp;
			ret = comp.equals(&this->positions,&pq->positions);
		}
		return ret;
  }


  PhraseQuery::~PhraseQuery(){
  //Func - Destructor
  //Pre  - true
  //Post 0 The instance has been destroyed
      
	  //Iterate through all the terms
	  for (uint32_t i = 0; i < terms.size(); i++){
        _CLLDECDELETE(terms[i]);
      }
	  positions.clear();
  }

  size_t PhraseQuery::hashCode() const {
		//todo: do cachedHashCode, and invalidate on add/remove clause
		size_t ret = Similarity::floatToByte(getBoost()) ^ Similarity::floatToByte(slop);
		
		{ //msvc6 scope fix
			for ( int32_t i=0;terms.size();i++ )
				ret = 31 * ret + terms[i]->hashCode();
		}
		{ //msvc6 scope fix
			for ( int32_t i=0;positions.size();i++ )
				ret = 31 * ret + positions[i];
		}
		return ret;
	}

  const TCHAR* PhraseQuery::getClassName(){
    return _T("PhraseQuery");
  }
  const TCHAR* PhraseQuery::getQueryName() const{
  //Func - Returns the string "PhraseQuery"
  //Pre  - true
  //Post - The string "PhraseQuery" has been returned
    return getClassName();
  }

  
	/**
	* Adds a term to the end of the query phrase.
	* The relative position of the term is the one immediately after the last term added.
	*/
	void PhraseQuery::add(Term* term) {
		CND_PRECONDITION(term != NULL,"term is NULL");

		int32_t position = 0;

		if(positions.size() > 0)
			position = (positions[positions.size()-1]) + 1;
		
		add(term, position);
	}

	void PhraseQuery::add(Term* term, int32_t position) {
	//Func - Adds a term to the end of the query phrase. 
	//Pre  - term != NULL 
	//Post - The term has been added if its field matches the field of the PhraseQuery
	//       and true is returned otherwise false is returned
		CND_PRECONDITION(term != NULL,"term is NULL");

		if (terms.size() == 0)
			field = term->field();
		else{
			//Check if the field of the _CLNEW term matches the field of the PhraseQuery
			//can use != because fields are interned
			if ( term->field() != field){
				//return false;
				TCHAR buf[200];
				_sntprintf(buf,200,_T("All phrase terms must be in the same field: %s"),term->field());
				_CLTHROWT(CL_ERR_IllegalArgument,buf);
			}
		}
		//Store the _CLNEW term
		terms.push_back(_CL_POINTER(term));

		positions.push_back(position);
	}

	void PhraseQuery::getPositions(Array<int32_t>& result) const{
		result.length = positions.size();
		result.values = _CL_NEWARRAY(int32_t,result.length);
		for(int32_t i = 0; i < result.length; i++){
			result.values[i] = positions[i];
		}
	}
	int32_t* PhraseQuery::getPositions() const{
	    CND_WARNING(false,"getPositions() is deprecated")

        Array<int32_t> arr;
        getPositions(arr);
        return arr.values;
	}
  
  Weight* PhraseQuery::_createWeight(Searcher* searcher) {
    if (terms.size() == 1) {			  // optimize one-term case
      Term* term = terms[0];
      Query* termQuery = _CLNEW TermQuery(term);
      termQuery->setBoost(getBoost());
      Weight* ret = termQuery->_createWeight(searcher);
	  _CLDELETE(termQuery);
	  return ret;
    }
    return _CLNEW PhraseWeight(searcher,this);
  }


  Term** PhraseQuery::getTerms() const{
  //Func - added by search highlighter
  //Pre  -
  //Post -

	  //Let size contain the number of terms
      int32_t size = terms.size();
      Term** ret = _CL_NEWARRAY(Term*,size+1);
       
	  CND_CONDITION(ret != NULL,"Could not allocated memory for ret");

	  //Iterate through terms and copy each pointer to ret
	  for ( int32_t i=0;i<size;i++ ){
          ret[i] = terms[i];
     }
     ret[size] = NULL;
     return ret;
  }

  TCHAR* PhraseQuery::toString(const TCHAR* f) const{
  //Func - Prints a user-readable version of this query. 
  //Pre  - f != NULL
  //Post - The query string has been returned

      if ( terms.size()== 0 )
		  return NULL;

      StringBuffer buffer;
      if ( f==NULL || _tcscmp(field,f)!=0) {
          buffer.append(field);
          buffer.append( _T(":"));
      }

      buffer.append( _T("\"") );

      Term *T = NULL;

	  //iterate through all terms
      for (uint32_t i = 0; i < terms.size(); i++) {
		  //Get the i-th term
		  T = terms[i];

		  //Ensure T is a valid Term
          CND_CONDITION(T !=NULL,"T is NULL");

          buffer.append( T->text() );
		  //Check if i is at the end of terms
		  if (i != terms.size()-1){
              buffer.append(_T(" "));
              }
          }

      buffer.append( _T("\"") );

      if (slop != 0) {
          buffer.append(_T("~"));
          buffer.appendFloat(slop,0);
          }

	  //Check if there is an other boost factor than 1.0
      if (getBoost() != 1.0f) {
          buffer.append(_T("^"));
          buffer.appendFloat( getBoost(),1 );
          }

	  //return the query string
	  return buffer.toString();
  }







  
 PhraseQuery::PhraseWeight::PhraseWeight(Searcher* searcher, PhraseQuery* _this) {
   this->_this=_this;
   this->value = 0;
   this->idf = 0;
   this->queryNorm = 0;
   this->queryWeight = 0;
   this->searcher = searcher;
 }

 TCHAR* PhraseQuery::PhraseWeight::toString() { 
	return STRDUP_TtoT(_T("weight(PhraseQuery)"));
 }
 PhraseQuery::PhraseWeight::~PhraseWeight(){
 }

 
 Query* PhraseQuery::PhraseWeight::getQuery() { return _this; }
 qreal PhraseQuery::PhraseWeight::getValue() { return value; }

 qreal PhraseQuery::PhraseWeight::sumOfSquaredWeights(){
   idf = _this->getSimilarity(searcher)->idf(&_this->terms, searcher);
   queryWeight = idf * _this->getBoost();    // compute query weight
   return queryWeight * queryWeight;         // square it
 }

 void PhraseQuery::PhraseWeight::normalize(qreal queryNorm) {
   this->queryNorm = queryNorm;
   queryWeight *= queryNorm;                   // normalize query weight
   value = queryWeight * idf;                  // idf for document 
 }

  Scorer* PhraseQuery::PhraseWeight::scorer(IndexReader* reader)  {
  //Func -
  //Pre  -
  //Post -

	  //Get the length of terms
      int32_t tpsLength = _this->terms.size();

	  //optimize zero-term case
      if (tpsLength == 0)			  
          return NULL;
    
    TermPositions** tps = _CL_NEWARRAY(TermPositions*,tpsLength+1);

	//Check if tps has been allocated properly
    CND_CONDITION(tps != NULL,"Could not allocate memory for tps");

    TermPositions* p = NULL;

	//Iterate through all terms
	int32_t size = _this->terms.size();
    for (int32_t i = 0; i < size; i++) {
        //Get the termPostitions for the i-th term
        p = reader->termPositions(_this->terms[i]);
      
		//Check if p is valid
		if (p == NULL) {
			//Delete previous retrieved termPositions
			while (--i >= 0){
				_CLVDELETE(tps[i]);  //todo: not a clucene object... should be
			}
            _CLDELETE_ARRAY(tps); 
            return NULL;
        }

        //Store p at i in tps
        tps[i] = p;
    }
	tps[tpsLength] = NULL;

    Scorer* ret = NULL;

    Array<int32_t> positions;
	_this->getPositions(positions);
	int32_t slop = _this->getSlop();
	if ( slop != 0)
		 // optimize exact case
		 //todo: need to pass these: this, tps, 
         ret = _CLNEW SloppyPhraseScorer(this,tps,positions.values,
								_this->getSimilarity(searcher), 
								slop, reader->norms(_this->field));
	else
	    ret = _CLNEW ExactPhraseScorer(this, tps, positions.values, 
									_this->getSimilarity(searcher),
                                    reader->norms(_this->field));
	positions.deleteArray();

    CND_CONDITION(ret != NULL,"Could not allocate memory for ret");

	//tps can be deleted safely. SloppyPhraseScorer or ExactPhraseScorer will take care
	//of its values

    _CLDELETE_ARRAY(tps);
    return ret;
  }

 void PhraseQuery::PhraseWeight::explain(IndexReader* reader, int32_t doc, Explanation* result){
   TCHAR descbuf[LUCENE_SEARCH_EXPLANATION_DESC_LEN+1];
   TCHAR* tmp;
   
   tmp = getQuery()->toString();
   _sntprintf(descbuf,LUCENE_SEARCH_EXPLANATION_DESC_LEN,_T("weight(%s in %d), product of:"),
	   tmp,doc);
   _CLDELETE_CARRAY(tmp);
   result->setDescription(descbuf);
   
   StringBuffer docFreqs;
   StringBuffer query;
   query.appendChar('\"');
   for (uint32_t i = 0; i < _this->terms.size(); i++) {
     if (i != 0) {
       docFreqs.appendChar(' ');
       query.appendChar(' ');
     }

     Term* term = _this->terms[i];

     docFreqs.append(term->text());
     docFreqs.appendChar('=');
     docFreqs.appendInt(searcher->docFreq(term));

     query.append(term->text());
   }
   query.appendChar('\"');

   _sntprintf(descbuf,LUCENE_SEARCH_EXPLANATION_DESC_LEN,
	   _T("idf(%s: %s)"),_this->field,docFreqs.getBuffer());
   Explanation* idfExpl = _CLNEW Explanation(idf, descbuf);
   
   // explain query weight
   Explanation* queryExpl = _CLNEW Explanation;
   tmp = getQuery()->toString();
   _sntprintf(descbuf,LUCENE_SEARCH_EXPLANATION_DESC_LEN,
		_T("queryWeight(%s), product of:"),tmp);
   _CLDELETE_CARRAY(tmp);
   queryExpl->setDescription(descbuf);

   Explanation* boostExpl = _CLNEW Explanation(_this->getBoost(), _T("boost"));
   if (_this->getBoost() != 1.0f)
     queryExpl->addDetail(boostExpl);
   queryExpl->addDetail(idfExpl);
   
   Explanation* queryNormExpl = _CLNEW Explanation(queryNorm,_T("queryNorm"));
   queryExpl->addDetail(queryNormExpl);

   queryExpl->setValue(boostExpl->getValue() *
                      idfExpl->getValue() *
                      queryNormExpl->getValue());

   result->addDetail(queryExpl);
   
   // explain field weight
   Explanation* fieldExpl = _CLNEW Explanation;
    _sntprintf(descbuf,LUCENE_SEARCH_EXPLANATION_DESC_LEN,
		_T("fieldWeight(%s:%s in %d), product of:"),
		_this->field,query.getBuffer(),doc);
   fieldExpl->setDescription(descbuf);

   
   Explanation* tfExpl = _CLNEW Explanation;
   scorer(reader)->explain(doc, tfExpl);
   fieldExpl->addDetail(tfExpl);
   fieldExpl->addDetail(idfExpl);

   Explanation* fieldNormExpl = _CLNEW Explanation();
   uint8_t* fieldNorms = reader->norms(_this->field);
   qreal fieldNorm =
     fieldNorms!=NULL ? Similarity::decodeNorm(fieldNorms[doc]) : 0.0f;
   fieldNormExpl->setValue(fieldNorm);

   
    _sntprintf(descbuf,LUCENE_SEARCH_EXPLANATION_DESC_LEN,
		_T("fieldNorm(field=%s, doc=%d)"),_this->field,doc);
   fieldNormExpl->setDescription(descbuf);
   fieldExpl->addDetail(fieldNormExpl);
   
   fieldExpl->setValue(tfExpl->getValue() *
                      idfExpl->getValue() *
                      fieldNormExpl->getValue());
   
   result->addDetail(fieldExpl);

   // combine them
   result->setValue(queryExpl->getValue() * fieldExpl->getValue());

   if (queryExpl->getValue() == 1.0f){
     result->set(*fieldExpl);
     _CLDELETE(fieldExpl);
   }
 }


CL_NS_END
