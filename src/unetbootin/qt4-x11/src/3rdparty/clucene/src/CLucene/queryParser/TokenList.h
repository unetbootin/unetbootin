/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#ifndef _lucene_queryParser_TokenList_
#define _lucene_queryParser_TokenList_

#if defined(_LUCENE_PRAGMA_ONCE)
# pragma once
#endif

#include "CLucene/util/VoidList.h"
#include "QueryToken.h"
CL_NS_DEF(queryParser)

	// Represents a list of the tokens.
	class TokenList:LUCENE_BASE
	{
	private:
		CL_NS(util)::CLVector<QueryToken*> tokens; //todo:,CL_NS(util)::Deletor::Object<QueryToken>
    public:
		TokenList();
		~TokenList();

		void add(QueryToken* token);

		void push(QueryToken* token);

		QueryToken* peek();

		QueryToken* extract();

		int32_t count() const;
	};
CL_NS_END
#endif
