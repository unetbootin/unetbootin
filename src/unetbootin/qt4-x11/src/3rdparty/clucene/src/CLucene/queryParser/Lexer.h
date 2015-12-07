/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
*
* Changes are Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
------------------------------------------------------------------------------*/
#ifndef _lucene_queryParser_Lexer_
#define _lucene_queryParser_Lexer_

#if defined(_LUCENE_PRAGMA_ONCE)
# pragma once
#endif

#include "CLucene/util/FastCharStream.h"
#include "CLucene/util/Reader.h"
#include "CLucene/util/StringBuffer.h"

#include "TokenList.h"

CL_NS_DEF(queryParser)
class QueryParserBase;
	// A simple Lexer that is used by QueryParser.
	class Lexer:LUCENE_BASE
	{
	private:
		CL_NS(util)::FastCharStream* reader;
		QueryParserBase* queryparser; //holds the queryparser so that we can do callbacks
		bool delSR;  //Indicates if the reader must be deleted or not

    public:
		// Initializes a new instance of the Lexer class with the specified
		// query to lex.
		Lexer(QueryParserBase* queryparser, const TCHAR* query);

		// Initializes a new instance of the Lexer class with the specified
		// TextReader to lex.
		Lexer(QueryParserBase* queryparser, CL_NS(util)::Reader* source);

		//Breaks the input stream onto the tokens list tokens
		void Lex(TokenList *tokens);
		
		~Lexer();

	private:
		bool GetNextToken(QueryToken* token);

		// Reads an integer number. buf should quite large, probably as large as a field should ever be
		void ReadIntegerNumber(const TCHAR ch, TCHAR* buf, int buflen);

		// Reads an inclusive range like [some words]
		bool ReadInclusiveRange(const TCHAR prev, QueryToken* token);

		// Reads an exclusive range like {some words}
		bool ReadExclusiveRange(const TCHAR prev, QueryToken* token);

		// Reads quoted string like "something else"
		bool ReadQuoted(const TCHAR prev, QueryToken* token);

		bool ReadTerm(const TCHAR prev, QueryToken* token);

        //reads an escaped character into the buf. Buf requires at least 3 characters
		bool ReadEscape(const TCHAR prev, TCHAR* buf);
	};
CL_NS_END
#endif
