/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#ifndef _lucene_queryParser_QueryParser_
#define _lucene_queryParser_QueryParser_

#if defined(_LUCENE_PRAGMA_ONCE)
# pragma once
#endif

#include "CLucene/analysis/AnalysisHeader.h"
#include "CLucene/util/Reader.h"
#include "CLucene/search/SearchHeader.h"
#include "CLucene/index/Term.h"

#include "TokenList.h"
#include "QueryToken.h"
#include "QueryParserBase.h"
#include "Lexer.h"

CL_NS_DEF(queryParser)

/**
* @brief CLucene's default query parser.
*
* <p>It's a query parser.
* The only method that clients should need to call is Parse().
* The syntax for query const TCHAR*s is as follows:
* A Query is a series of clauses. A clause may be prefixed by:</p>
* <ul>
*	<li>a plus (+) or a minus (-) sign, indicating that the 
*	clause is required or prohibited respectively; or</li>
*	<li>a term followed by a colon, indicating the field to be searched.
*	This enables one to construct queries which search multiple fields.</li>
*	</ul>
*	<p>
*	A clause may be either:</p>
*	<ul>
*	<li>a term, indicating all the documents that contain this term; or</li>
*	<li>a nested query, enclosed in parentheses. Note that this may be 
*	used with a +/- prefix to require any of a set of terms.</li>
*	</ul>
*	<p>
* Thus, in BNF, the query grammar is:</p>
*	<code>
*	Query  ::= ( Clause )*
*	Clause ::= ["+", "-"] [&lt;TERM&gt; ":"] ( &lt;TERM&gt; | "(" Query ")" )
*	</code>
*	<p>
*	Examples of appropriately formatted queries can be found in the test cases.
*	</p>
*/
class QueryParser : public QueryParserBase
{
private:
	const TCHAR* field;
	TokenList* tokens;
public:
	/**
	* Initializes a new instance of the QueryParser class with a specified field and
	* analyzer values.
	*/
	QueryParser(const TCHAR* field, CL_NS(analysis)::Analyzer* analyzer);
	~QueryParser();

	/**
	* Returns a parsed Query instance.
	* Note: this call is not threadsafe, either use a seperate QueryParser for each thread, or use a thread lock
	* <param name="query">The query value to be parsed.</param>
	* <returns>A parsed Query instance.</returns>
	*/
	virtual CL_NS(search)::Query* parse(const TCHAR* query);

	/**
	* Returns a parsed Query instance.
	* Note: this call is not threadsafe, either use a seperate QueryParser for each thread, or use a thread lock
	* <param name="reader">The TextReader value to be parsed.</param>
	* <returns>A parsed Query instance.</returns>
	*/
	virtual CL_NS(search)::Query* parse(CL_NS(util)::Reader* reader);

	/**
	* Returns a new instance of the Query class with a specified query, field and
	* analyzer values.
	*/
	static CL_NS(search)::Query* parse(const TCHAR* query, const TCHAR* field, CL_NS(analysis)::Analyzer* analyzer);

	CL_NS(analysis)::Analyzer* getAnalyzer() { return analyzer; }

	/**
	* @return Returns the field.
	*/
	const TCHAR* getField() { return field; }

	//deprecated functions
	_CL_DEPRECATED( setLowercaseExpandedTerms ) void  setLowercaseWildcardTerms(bool lowercaseWildcardTerms){ setLowercaseExpandedTerms(lowercaseWildcardTerms);  }
	_CL_DEPRECATED( getLowercaseExpandedTerms ) bool  getLowercaseWildcardTerms() const { return getLowercaseExpandedTerms(); }
protected:
	//these functions may be defined under certain compilation conditions.
	//note that this functionality is deprecated, you should create your own queryparser
	//if you want to remove this functionality...it will be removed... be warned!
#ifdef NO_PREFIX_QUERY
	virtual CL_NS(search)::Query* GetPrefixQuery(const TCHAR* field,const TCHAR* termStr){ return NULL; }
#endif
#ifdef NO_FUZZY_QUERY
    virtual CL_NS(search)::Query* GetFuzzyQuery(const TCHAR* field,const TCHAR* termStr){ return NULL; }
#endif
#ifdef NO_RANGE_QUERY
	virtual CL_NS(search)::Query* GetRangeQuery(const TCHAR* field, const TCHAR* part1, const TCHAR* part2, bool inclusive) { return NULL; }
#endif
#ifdef NO_WILDCARD_QUERY
	virtual CL_NS(search)::Query* GetWildcardQuery(const TCHAR* field, TCHAR* termStr) { return NULL; }
#endif
private:
	/**
	* matches for CONJUNCTION
	* CONJUNCTION ::= <AND> | <OR>
	*/
	int32_t MatchConjunction();

	/**
	* matches for MODIFIER
	* MODIFIER ::= <PLUS> | <MINUS> | <NOT>
	*/
	int32_t MatchModifier();

	/**
	* matches for QUERY
	* QUERY ::= [MODIFIER] CLAUSE (<CONJUNCTION> [MODIFIER] CLAUSE)*
	*/
	CL_NS(search)::Query* MatchQuery(const TCHAR* field);

	/**
	* matches for CLAUSE
	* CLAUSE ::= [TERM <COLON>] ( TERM | (<LPAREN> QUERY <RPAREN>))
	*/
	CL_NS(search)::Query* MatchClause(const TCHAR* field);

    /**
	* matches for TERM
	* TERM ::= TERM | PREFIXTERM | WILDTERM | NUMBER
	*			[ <FUZZY> ] [ <CARAT> <NUMBER> [<FUZZY>]]
	*
	*			| (<RANGEIN> | <RANGEEX>) [<CARAT> <NUMBER>]
	*			| <QUOTED> [SLOP] [<CARAT> <NUMBER>]
	*/
	CL_NS(search)::Query* MatchTerm(const TCHAR* field);

	/**
	* matches for QueryToken of the specified type and returns it
	* otherwise Exception throws
	*/
	QueryToken* MatchQueryToken(QueryToken::Types expectedType);

	/**
	* Extracts the first token from the Tokenlist tokenlist
    * and destroys it
    */
	void ExtractAndDeleteToken(void);
};
CL_NS_END
#endif
