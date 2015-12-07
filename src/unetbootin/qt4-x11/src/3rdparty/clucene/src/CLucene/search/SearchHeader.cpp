/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#include "CLucene/StdHeader.h"
#include "SearchHeader.h"
#include "BooleanQuery.h"
#include "FieldDocSortedHitQueue.h"

CL_NS_USE(index)
CL_NS_DEF(search)

CL_NS(document)::Document* Searchable::doc(const int32_t i){
    CL_NS(document)::Document* ret = _CLNEW CL_NS(document)::Document;
    if (!doc(i,ret) )
        _CLDELETE(ret);
    return ret;
}

//static
Query* Query::mergeBooleanQueries(Query** queries) {
    CL_NS(util)::CLVector<BooleanClause*> allClauses;
    int32_t i = 0;
    while ( queries[i] != NULL ){
		BooleanQuery* bq = (BooleanQuery*)queries[i];
		
		int32_t size = bq->getClauseCount();
		BooleanClause** clauses = _CL_NEWARRAY(BooleanClause*, size);
		bq->getClauses(clauses);
		
		for (int32_t j = 0;j<size;++j ){
			allClauses.push_back(clauses[j]);
			j++;
		}
		_CLDELETE_ARRAY(clauses);
		i++;
    }

    BooleanQuery* result = _CLNEW BooleanQuery();
    CL_NS(util)::CLVector<BooleanClause*>::iterator itr = allClauses.begin();
    while (itr != allClauses.end() ) {
		result->add(*itr);
    }
    return result;
}

Query::Query(const Query& clone):boost(clone.boost){
		//constructor
}
Weight* Query::_createWeight(Searcher* searcher){
	_CLTHROWA(CL_ERR_UnsupportedOperation,"UnsupportedOperationException: Query::_createWeight");
}

Query::Query():
   boost(1.0f)
{
	//constructor
}
Query::~Query(){
}

/** Expert: called to re-write queries into primitive queries. */
Query* Query::rewrite(CL_NS(index)::IndexReader* reader){
   return this;
}

Query* Query::combine(Query** queries){
   _CLTHROWA(CL_ERR_UnsupportedOperation,"UnsupportedOperationException: Query::combine");
}
Similarity* Query::getSimilarity(Searcher* searcher) {
   return searcher->getSimilarity();
}
bool Query::instanceOf(const TCHAR* other) const{
   const TCHAR* t = getQueryName();
	if ( t==other || _tcscmp( t, other )==0 )
		return true;
	else
		return false;
}
TCHAR* Query::toString() const{
   return toString(LUCENE_BLANK_STRING);
}

void Query::setBoost(qreal b) { boost = b; }

qreal Query::getBoost() const { return boost; }

Weight* Query::weight(Searcher* searcher){
    Query* query = searcher->rewrite(this);
    Weight* weight = query->_createWeight(searcher);
    qreal sum = weight->sumOfSquaredWeights();
    qreal norm = getSimilarity(searcher)->queryNorm(sum);
    weight->normalize(norm);
    return weight;
}

TopFieldDocs::TopFieldDocs (int32_t totalHits, FieldDoc** fieldDocs, int32_t scoreDocsLen, SortField** fields):
 TopDocs (totalHits, NULL, scoreDocsLen)
{
	this->fields = fields;
	this->fieldDocs = fieldDocs;
	this->scoreDocs = _CL_NEWARRAY(ScoreDoc,scoreDocsLen);
	for (int32_t i=0;i<scoreDocsLen;i++ )
		this->scoreDocs[i] = this->fieldDocs[i]->scoreDoc;
}
TopFieldDocs::~TopFieldDocs(){
	if ( fieldDocs ){
		for (int32_t i=0;i<scoreDocsLength;i++)
			_CLDELETE(fieldDocs[i]);
		_CLDELETE_ARRAY(fieldDocs);
	}
	if ( fields != NULL ){
       for ( int i=0;fields[i]!=NULL;i++ )
           _CLDELETE(fields[i]);
       _CLDELETE_ARRAY(fields);
    }
}

TopDocs::TopDocs(const int32_t th, ScoreDoc*sds, int32_t scoreDocsLen):
    totalHits(th),
	scoreDocsLength(scoreDocsLen),
	scoreDocs(sds)
{
//Func - Constructor
//Pre  - sds may or may not be NULL
//       sdLength >= 0
//Post - The instance has been created

}

TopDocs::~TopDocs(){
//Func - Destructor
//Pre  - true
//Post - The instance has been destroyed

	_CLDELETE_ARRAY(scoreDocs);
}

CL_NS_END
