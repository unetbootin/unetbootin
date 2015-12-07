/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#include "CLucene/StdHeader.h"
#include "BooleanQuery.h"

#include "BooleanClause.h"
#include "CLucene/index/IndexReader.h"
#include "CLucene/util/StringBuffer.h"
#include "CLucene/util/Arrays.h"
#include "SearchHeader.h"
#include "BooleanScorer.h"
#include "Scorer.h"

CL_NS_USE(index)
CL_NS_USE(util)
CL_NS_DEF(search)

	BooleanQuery::BooleanQuery():
		clauses(true)
	{
    }

	BooleanQuery::BooleanQuery(const BooleanQuery& clone):
		Query(clone)
	{
		for ( uint32_t i=0;i<clone.clauses.size();i++ ){
			BooleanClause* clause = clone.clauses[i]->clone();
			clause->deleteQuery=true;
			add(clause);
		}
	}

    BooleanQuery::~BooleanQuery(){
		clauses.clear();
    }

	size_t BooleanQuery::hashCode() const {
		//todo: do cachedHashCode, and invalidate on add/remove clause
		size_t ret = 0;
		for (uint32_t i = 0 ; i < clauses.size(); i++) {
			BooleanClause* c = clauses[i];
			ret = 31 * ret + c->hashCode();
		}
		ret = ret ^ Similarity::floatToByte(getBoost());
		return ret;
	}

    const TCHAR* BooleanQuery::getQueryName() const{
      return getClassName();
    }
	const TCHAR* BooleanQuery::getClassName(){
      return _T("BooleanQuery");
    }

   /**
   * Default value is 1024.  Use <code>org.apache.lucene.maxClauseCount</code>
   * system property to override.
   */
   size_t BooleanQuery::maxClauseCount = LUCENE_BOOLEANQUERY_MAXCLAUSECOUNT;
   size_t BooleanQuery::getMaxClauseCount(){
      return maxClauseCount;
   }

   void BooleanQuery::setMaxClauseCount(size_t maxClauseCount){
	   BooleanQuery::maxClauseCount = maxClauseCount;
   }

  void BooleanQuery::add(Query* query, const bool deleteQuery, const bool required, const bool prohibited) {
		BooleanClause* bc = _CLNEW BooleanClause(query,deleteQuery,required, prohibited);
		try{
			add(bc);
		}catch(...){
			_CLDELETE(bc);
			throw;
		}
  }

  void BooleanQuery::add(BooleanClause* clause) {
    if (clauses.size() >= getMaxClauseCount())
      _CLTHROWA(CL_ERR_TooManyClauses,"Too Many Clauses");

    clauses.push_back(clause);
  }


  size_t BooleanQuery::getClauseCount() const {
    return (int32_t) clauses.size();
  }

  TCHAR* BooleanQuery::toString(const TCHAR* field) const{
    StringBuffer buffer;
    if (getBoost() != 1.0) {
      buffer.append(_T("("));
    }

    for (uint32_t i = 0 ; i < clauses.size(); i++) {
      BooleanClause* c = clauses[i];
      if (c->prohibited)
        buffer.append(_T("-"));
      else if (c->required)
        buffer.append(_T("+"));

      if ( c->query->instanceOf(BooleanQuery::getClassName()) ) {	  // wrap sub-bools in parens
        buffer.append(_T("("));

        TCHAR* buf = c->query->toString(field);
        buffer.append(buf);
        _CLDELETE_CARRAY( buf );

        buffer.append(_T(")"));
      } else {
        TCHAR* buf = c->query->toString(field);
        buffer.append(buf);
        _CLDELETE_CARRAY( buf );
      }
      if (i != clauses.size()-1)
        buffer.append(_T(" "));

      if (getBoost() != 1.0) {
         buffer.append(_T(")^"));
         buffer.appendFloat(getBoost(),1);
      }
    }
    return buffer.toString();
  }




    BooleanClause** BooleanQuery::getClauses() const
	{
		CND_MESSAGE(false, "Warning: BooleanQuery::getClauses() is deprecated")
		BooleanClause** ret = _CL_NEWARRAY(BooleanClause*, clauses.size()+1);
		getClauses(ret);
		return ret;
	}
	
	void BooleanQuery::getClauses(BooleanClause** ret) const
	{
		size_t size=clauses.size();
		for ( uint32_t i=0;i<size;i++ )
			ret[i] = clauses[i];
	}
	  Query* BooleanQuery::rewrite(IndexReader* reader) {
         if (clauses.size() == 1) {                    // optimize 1-clause queries
            BooleanClause* c = clauses[0];
            if (!c->prohibited) {			  // just return clause
				Query* query = c->query->rewrite(reader);    // rewrite first

				//if the query doesn't actually get re-written,
				//then return a clone (because the BooleanQuery
				//will register different to the returned query.
				if ( query == c->query )
					query = query->clone();

				if (getBoost() != 1.0f) {                 // incorporate boost
					query->setBoost(getBoost() * query->getBoost());
				}

				return query;
            }
         }

         BooleanQuery* clone = NULL;                    // recursively rewrite
		 for (uint32_t i = 0 ; i < clauses.size(); i++) {
            BooleanClause* c = clauses[i];
            Query* query = c->query->rewrite(reader);
            if (query != c->query) {                     // clause rewrote: must clone
               if (clone == NULL)
                  clone = (BooleanQuery*)this->clone();
			   //todo: check if delete query should be on...
			   //in fact we should try and get rid of these
			   //for compatibility sake
               clone->clauses.set (i, _CLNEW BooleanClause(query, true, c->required, c->prohibited));
            }
         }
         if (clone != NULL) {
			 return clone;                               // some clauses rewrote
         } else
            return this;                                // no clauses rewrote
      }


      Query* BooleanQuery::clone()  const{
		 BooleanQuery* clone = _CLNEW BooleanQuery(*this);
         return clone;
      }

      /** Returns true iff <code>o</code> is equal to this. */
      bool BooleanQuery::equals(Query* o)const {
         if (!(o->instanceOf(BooleanQuery::getClassName())))
            return false;
         const BooleanQuery* other = (BooleanQuery*)o;

		 bool ret = (this->getBoost() == other->getBoost());
		 if ( ret ){
			 CLListEquals<BooleanClause,BooleanClause::Compare, const ClausesType, const ClausesType> comp;
			 ret = comp.equals(&this->clauses,&other->clauses);
		 }
		return ret;
      }

	qreal BooleanQuery::BooleanWeight::getValue() { return parentQuery->getBoost(); }
	Query* BooleanQuery::BooleanWeight::getQuery() { return (Query*)parentQuery; }





	BooleanQuery::BooleanWeight::BooleanWeight(Searcher* searcher, 
		CLVector<BooleanClause*,Deletor::Object<BooleanClause> >* clauses, BooleanQuery* parentQuery)
	{
		this->searcher = searcher;
		this->parentQuery = parentQuery;
		this->clauses = clauses;
		for (uint32_t i = 0 ; i < clauses->size(); i++) {
			weights.push_back((*clauses)[i]->query->_createWeight(searcher));
		}
	}
	BooleanQuery::BooleanWeight::~BooleanWeight(){
		this->weights.clear();
	}

    qreal BooleanQuery::BooleanWeight::sumOfSquaredWeights() {
      qreal sum = 0.0f;
      for (uint32_t i = 0 ; i < weights.size(); i++) {
        BooleanClause* c = (*clauses)[i];
        Weight* w = weights[i];
        if (!c->prohibited)
          sum += w->sumOfSquaredWeights();         // sum sub weights
      }
      sum *= parentQuery->getBoost() * parentQuery->getBoost();             // boost each sub-weight
      return sum ;
    }

    void BooleanQuery::BooleanWeight::normalize(qreal norm) {
      norm *= parentQuery->getBoost();                         // incorporate boost
      for (uint32_t i = 0 ; i < weights.size(); i++) {
        BooleanClause* c = (*clauses)[i];
        Weight* w = weights[i];
        if (!c->prohibited)
          w->normalize(norm);
      }
    }

    Scorer* BooleanQuery::BooleanWeight::scorer(IndexReader* reader){
      // First see if the (faster) ConjunctionScorer will work.  This can be
      // used when all clauses are required.  Also, at this point a
      // BooleanScorer cannot be embedded in a ConjunctionScorer, as the hits
      // from a BooleanScorer are not always sorted by document number (sigh)
      // and hence BooleanScorer cannot implement skipTo() correctly, which is
      // required by ConjunctionScorer.
      bool allRequired = true;
      bool noneBoolean = true;
	  { //msvc6 scope fix
		  for (uint32_t i = 0 ; i < weights.size(); i++) {
			BooleanClause* c = (*clauses)[i];
			if (!c->required)
			  allRequired = false;
			if (c->query->instanceOf(BooleanQuery::getClassName()))
			  noneBoolean = false;
		  }
	  }

      if (allRequired && noneBoolean) {           // ConjunctionScorer is okay
        ConjunctionScorer* result =
          _CLNEW ConjunctionScorer(parentQuery->getSimilarity(searcher));
        for (uint32_t i = 0 ; i < weights.size(); i++) {
          Weight* w = weights[i];
          Scorer* subScorer = w->scorer(reader);
          if (subScorer == NULL)
            return NULL;
          result->add(subScorer);
        }
        return result;
      }

      // Use good-old BooleanScorer instead.
      BooleanScorer* result = _CLNEW BooleanScorer(parentQuery->getSimilarity(searcher));

	  { //msvc6 scope fix
		  for (uint32_t i = 0 ; i < weights.size(); i++) {
			BooleanClause* c = (*clauses)[i];
			Weight* w = weights[i];
			Scorer* subScorer = w->scorer(reader);
			if (subScorer != NULL)
			  result->add(subScorer, c->required, c->prohibited);
			else if (c->required)
			  return NULL;
		  }
	  }

      return result;
    }

	void BooleanQuery::BooleanWeight::explain(IndexReader* reader, int32_t doc, Explanation* result){
      int32_t coord = 0;
      int32_t maxCoord = 0;
      qreal sum = 0.0f;
      Explanation* sumExpl = _CLNEW Explanation;
      for (uint32_t i = 0 ; i < weights.size(); i++) {
        BooleanClause* c = (*clauses)[i];
        Weight* w = weights[i];
        Explanation* e = _CLNEW Explanation;
        w->explain(reader, doc, e);
        if (!c->prohibited) 
           maxCoord++;
        if (e->getValue() > 0) {
          if (!c->prohibited) {
            sumExpl->addDetail(e);
            sum += e->getValue();
            coord++;
            e = NULL; //prevent e from being deleted
          } else {
            //we want to return something else...
            _CLDELETE(sumExpl);
            result->setValue(0.0f);
            result->setDescription(_T("match prohibited"));
            return;
          }
        } else if (c->required) {
            _CLDELETE(sumExpl);
            result->setValue(0.0f);
            result->setDescription(_T("match prohibited"));
            return;
        }
        
        _CLDELETE(e);
      }
      sumExpl->setValue(sum);

      if (coord == 1){                               // only one clause matched
		  Explanation* tmp = sumExpl;
		  sumExpl = sumExpl->getDetail(0)->clone();          // eliminate wrapper
		  _CLDELETE(tmp);
      }

      sumExpl->setDescription(_T("sum of:"));
	  qreal coordFactor = parentQuery->getSimilarity(searcher)->coord(coord, maxCoord);
	  if (coordFactor == 1.0f){                      // coord is no-op
        result->set(*sumExpl);                       // eliminate wrapper
		_CLDELETE(sumExpl);
	  } else {
        result->setDescription( _T("product of:"));
        result->addDetail(sumExpl);

        StringBuffer explbuf;
        explbuf.append(_T("coord("));
        explbuf.appendInt(coord);
        explbuf.append(_T("/"));
        explbuf.appendInt(maxCoord);
        explbuf.append(_T(")"));
        result->addDetail(_CLNEW Explanation(coordFactor, explbuf.getBuffer()));
        result->setValue(sum*coordFactor);
      }
    }


CL_NS_END
