/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#include "CLucene/StdHeader.h"
#include "TermQuery.h"

#include "SearchHeader.h"
#include "Scorer.h"
#include "CLucene/index/Term.h"
#include "TermScorer.h"
#include "CLucene/index/IndexReader.h"
#include "CLucene/util/StringBuffer.h"
#include "CLucene/index/Terms.h"

CL_NS_USE(index)
CL_NS_DEF(search)


	/** Constructs a query for the term <code>t</code>. */
	TermQuery::TermQuery(Term* t):
		term( _CL_POINTER(t) )
	{
	}
	TermQuery::TermQuery(const TermQuery& clone):
  		Query(clone){
		this->term=_CL_POINTER(clone.term);
	}
	TermQuery::~TermQuery(){
	    _CLDECDELETE(term);
	}

	Query* TermQuery::clone() const{
		return _CLNEW TermQuery(*this);
	}

	const TCHAR* TermQuery::getClassName(){
		return _T("TermQuery");
	}
	const TCHAR* TermQuery::getQueryName() const{
		return getClassName();
	}
	size_t TermQuery::hashCode() const {
		return Similarity::floatToByte(getBoost()) ^ term->hashCode();
	}
    
      
	//added by search highlighter
	Term* TermQuery::getTerm(bool pointer) const
	{
		if ( pointer )
			return _CL_POINTER(term);
		else
			return term;
	}
    
    
	/** Prints a user-readable version of this query. */
	TCHAR* TermQuery::toString(const TCHAR* field) const{
		CL_NS(util)::StringBuffer buffer;
		if ( field==NULL || _tcscmp(term->field(),field)!= 0 ) {
			buffer.append(term->field());
			buffer.append(_T(":"));
		}
		buffer.append(term->text());
		if (getBoost() != 1.0f) {
			buffer.append(_T("^"));
			buffer.appendFloat( getBoost(),1 );
		}
		return buffer.toString();
	}

   /** Returns true iff <code>o</code> is equal to this. */
   bool TermQuery::equals(Query* other) const {
	   if (!(other->instanceOf(TermQuery::getClassName())))
            return false;

	   TermQuery* tq = (TermQuery*)other;
      return (this->getBoost() == tq->getBoost())
         && this->term->equals(tq->term);
   }


   TermQuery::TermWeight::TermWeight(Searcher* searcher, TermQuery* _this, Term* _term) {
		this->_this = _this;
		this->_term = _term;
		this->searcher = searcher;
		value=0;
		idf=0;
		queryNorm=0;
		queryWeight=0;
	}
   TermQuery::TermWeight::~TermWeight(){
   }

    //return a *new* string describing this object
	TCHAR* TermQuery::TermWeight::toString() { 
		int32_t size=_tcslen(_this->getQueryName()) + 10;
		TCHAR* tmp = _CL_NEWARRAY(TCHAR, size);//_tcslen(weight())
		_sntprintf(tmp,size,_T("weight(%s)"),_this->getQueryName());
		return tmp;
	}

	qreal TermQuery::TermWeight::sumOfSquaredWeights() {
		idf = _this->getSimilarity(searcher)->idf(_term, searcher); // compute idf
		queryWeight = idf * _this->getBoost();             // compute query weight
		return queryWeight * queryWeight;           // square it
	}

	void TermQuery::TermWeight::normalize(qreal queryNorm) {
		this->queryNorm = queryNorm;
		queryWeight *= queryNorm;                   // normalize query weight
		value = queryWeight * idf;                  // idf for document 
	}

	Scorer* TermQuery::TermWeight::scorer(IndexReader* reader) {
		TermDocs* termDocs = reader->termDocs(_term);
		    
		if (termDocs == NULL)
			return NULL;
		    
		return _CLNEW TermScorer(this, termDocs, _this->getSimilarity(searcher),
								reader->norms(_term->field()));
	}

	void TermQuery::TermWeight::explain(IndexReader* reader, int32_t doc, Explanation* result){
		TCHAR buf[LUCENE_SEARCH_EXPLANATION_DESC_LEN];
        TCHAR* tmp;

        tmp = getQuery()->toString();
		_sntprintf(buf,LUCENE_SEARCH_EXPLANATION_DESC_LEN,
			_T("weight(%s in %d), product of:"),tmp,doc);
        _CLDELETE_CARRAY(tmp);
		result->setDescription(buf);

		_sntprintf(buf,LUCENE_SEARCH_EXPLANATION_DESC_LEN,
			_T("idf(docFreq=%d)"), searcher->docFreq(_term) );
		Explanation* idfExpl = _CLNEW Explanation(idf, buf);

		// explain query weight
		Explanation* queryExpl = _CLNEW Explanation();
        tmp = getQuery()->toString();
		_sntprintf(buf,LUCENE_SEARCH_EXPLANATION_DESC_LEN,
			_T("queryWeight(%s), product of:"), tmp);
        _CLDELETE_CARRAY(tmp);
		queryExpl->setDescription(buf);

		Explanation* boostExpl = _CLNEW Explanation(_this->getBoost(), _T("boost"));
		if (_this->getBoost() != 1.0f)
			queryExpl->addDetail(boostExpl);
        else
            _CLDELETE(boostExpl);

		queryExpl->addDetail(idfExpl->clone());

		Explanation* queryNormExpl = _CLNEW Explanation(queryNorm,_T("queryNorm"));
		queryExpl->addDetail(queryNormExpl);
		    
		queryExpl->setValue(_this->getBoost()* // always 1.0
							idfExpl->getValue() *
							queryNormExpl->getValue());
		    
		// explain field weight
		const TCHAR* field = _term->field();
		Explanation* fieldExpl = _CLNEW Explanation();

        tmp = _term->toString();
		_sntprintf(buf,LUCENE_SEARCH_EXPLANATION_DESC_LEN,
			_T("fieldWeight(%s in %d), product of:"),tmp,doc);
        _CLDELETE_CARRAY(tmp);
		fieldExpl->setDescription(buf);

        Scorer* sc = scorer(reader);
		Explanation* tfExpl = _CLNEW Explanation;
		sc->explain(doc, tfExpl);
        _CLDELETE(sc);
		fieldExpl->addDetail(tfExpl);
		fieldExpl->addDetail(idfExpl);

		Explanation* fieldNormExpl = _CLNEW Explanation();
		uint8_t* fieldNorms = reader->norms(field);
		qreal fieldNorm =
			fieldNorms!=NULL ? Similarity::decodeNorm(fieldNorms[doc]) : 0.0f;
		fieldNormExpl->setValue(fieldNorm);

		_sntprintf(buf,LUCENE_SEARCH_EXPLANATION_DESC_LEN,
			_T("fieldNorm(field=%s, doc=%d)"),field,doc);
		fieldNormExpl->setDescription(buf);
		fieldExpl->addDetail(fieldNormExpl);

		fieldExpl->setValue(tfExpl->getValue() *
							idfExpl->getValue() *
							fieldNormExpl->getValue());

        /*if (queryExpl->getValue() == 1.0f){
			_CLDELETE(result);
            return fieldExpl;
        }else{*/
		    result->addDetail(queryExpl);
		    result->addDetail(fieldExpl);

		    // combine them
		    result->setValue(queryExpl->getValue() * fieldExpl->getValue());
        //}
	}
	
	Weight* TermQuery::_createWeight(Searcher* searcher) {
        return _CLNEW TermWeight(searcher,this,term);
    }
CL_NS_END

