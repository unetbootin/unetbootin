/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
*
* Changes are Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
------------------------------------------------------------------------------*/
#include "CLucene/StdHeader.h"
#include "MultiFieldQueryParser.h"
#include "CLucene/analysis/AnalysisHeader.h"
#include "CLucene/search/BooleanQuery.h"
#include "CLucene/search/PhraseQuery.h"
#include "CLucene/search/SearchHeader.h"
#include "QueryParser.h"

CL_NS_USE(index)
CL_NS_USE(util)
CL_NS_USE(search)
CL_NS_USE(analysis)

CL_NS_DEF(queryParser)

MultiFieldQueryParser::MultiFieldQueryParser(const TCHAR** fields, CL_NS(analysis)::Analyzer* a, BoostMap* boosts):
	QueryParser(NULL,a)
{
	this->fields = fields;
    this->boosts = boosts;
}
MultiFieldQueryParser::~MultiFieldQueryParser(){
}

//static 
Query* MultiFieldQueryParser::parse(const TCHAR* query, const TCHAR** fields, Analyzer* analyzer)
{
    BooleanQuery* bQuery = _CLNEW BooleanQuery();
    int32_t i = 0;
    while ( fields[i] != NULL ){
		   Query* q = QueryParser::parse(query, fields[i], analyzer);
			bQuery->add(q, true, false, false);

        i++;
    }
    return bQuery;
}

//static 
Query* MultiFieldQueryParser::parse(const TCHAR* query, const TCHAR** fields, const uint8_t* flags, Analyzer* analyzer)
{
    BooleanQuery* bQuery = _CLNEW BooleanQuery();
    int32_t i = 0;
    while ( fields[i] != NULL )
    {
		Query* q = QueryParser::parse(query, fields[i], analyzer);
        uint8_t flag = flags[i];
        switch (flag)
        {
			case MultiFieldQueryParser::REQUIRED_FIELD:
                bQuery->add(q, true, true, false);
                break;
            case MultiFieldQueryParser::PROHIBITED_FIELD:
                bQuery->add(q, true, false, true);
                break;
            default:
                bQuery->add(q, true, false, false);
                break;
        }

        i++;
    }
    return bQuery;
}


Query* MultiFieldQueryParser::GetFieldQuery(const TCHAR* field, TCHAR* queryText, int32_t slop){
	if (field == NULL) {
		CL_NS_STD(vector)<BooleanClause*> clauses;
		for (int i = 0; fields[i]!=NULL; ++i) {
			Query* q = QueryParser::GetFieldQuery(fields[i], queryText);
			if (q != NULL) {
                //If the user passes a map of boosts
                 if (boosts != NULL) {
                     //Get the boost from the map and apply them
                     BoostMap::const_iterator itr = boosts->find(fields[i]);
                     if (itr != boosts->end()) {
                         q->setBoost(itr->second);
                     }
                 }
				if (q->getQueryName() == PhraseQuery::getClassName()) {
					((PhraseQuery*)q)->setSlop(slop);
				}
				//if (q instanceof MultiPhraseQuery) {
				//	((MultiPhraseQuery) q).setSlop(slop);
				//}
				q = QueryAddedCallback(fields[i], q);
				if ( q )
					clauses.push_back(_CLNEW BooleanClause(q, true, false,false));
			}
		}
		if (clauses.size() == 0)  // happens for stopwords
			return NULL;
		Query* q = QueryParser::GetBooleanQuery(clauses);
        return q;
	}else{
		Query* q = QueryParser::GetFieldQuery(field, queryText);
		if ( q )
			q = QueryAddedCallback(field,q);
		return q;
	}
}


Query* MultiFieldQueryParser::GetFieldQuery(const TCHAR* field, TCHAR* queryText){
	return GetFieldQuery(field, queryText, 0);
}


CL_NS(search)::Query* MultiFieldQueryParser::GetFuzzyQuery(const TCHAR* field, TCHAR* termStr){
	if (field == NULL) {
		CL_NS_STD(vector)<BooleanClause*> clauses;
		for (int i = 0; fields[i]!=NULL; ++i) {
			Query* q = QueryParser::GetFuzzyQuery(fields[i], termStr); //todo: , minSimilarity
			if ( q ){
				q = QueryAddedCallback(fields[i], q);
				if ( q ){
					clauses.push_back(_CLNEW BooleanClause(q,true,false,false) );
				}
			}
		}
		return QueryParser::GetBooleanQuery(clauses);
	}else{
		Query* q = QueryParser::GetFuzzyQuery(field, termStr);//todo: , minSimilarity
		if ( q )
			q = QueryAddedCallback(field,q);
		return q;
	}
}

Query* MultiFieldQueryParser::GetPrefixQuery(const TCHAR* field, TCHAR* termStr){
	if (field == NULL) {
		CL_NS_STD(vector)<BooleanClause*> clauses;
		for (int i = 0; fields[i]!=NULL; ++i) {
			Query* q = QueryParser::GetPrefixQuery(fields[i], termStr);
			if ( q ){
				q = QueryAddedCallback(fields[i],q);
				if ( q ){
					clauses.push_back(_CLNEW BooleanClause(q,true,false,false));
				}
			}
		}
		return QueryParser::GetBooleanQuery(clauses);
	}else{
		Query* q = QueryParser::GetPrefixQuery(field, termStr);
		if ( q )
			q = QueryAddedCallback(field,q);
		return q;
	}
}

Query* MultiFieldQueryParser::GetWildcardQuery(const TCHAR* field, TCHAR* termStr){
	if (field == NULL) {
		CL_NS_STD(vector)<BooleanClause*> clauses;
		for (int i = 0; fields[i]!=NULL; ++i) {
			Query* q = QueryParser::GetWildcardQuery(fields[i], termStr);
			if ( q ){
				q = QueryAddedCallback(fields[i],q);
				if ( q ){
					clauses.push_back(_CLNEW BooleanClause(q,true,false,false));
				}
			}
		}
		return QueryParser::GetBooleanQuery(clauses);
	}else{
		Query* q = QueryParser::GetWildcardQuery(field, termStr);
		if ( q )
			q = QueryAddedCallback(field,q);
		return q;
	}
}


Query* MultiFieldQueryParser::GetRangeQuery(const TCHAR* field, TCHAR* part1, TCHAR* part2, bool inclusive){
	if (field == NULL) {
		CL_NS_STD(vector)<BooleanClause*> clauses;
		for (int i = 0; fields[i]!=NULL; ++i) {
			Query* q = QueryParser::GetRangeQuery(fields[i], part1, part2, inclusive);
			if ( q ){
				q = QueryAddedCallback(fields[i],q);
				if ( q ){
					clauses.push_back(_CLNEW BooleanClause(q,true,false,false));
				}
			}
		}
		return QueryParser::GetBooleanQuery(clauses);
	}else{
		Query* q = QueryParser::GetRangeQuery(field, part1, part2, inclusive);
		if ( q )
			q = QueryAddedCallback(field,q);
		return q;
	}
}


CL_NS_END
