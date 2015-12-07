/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#ifndef _lucene_search_SloppyPhraseScorer_
#define _lucene_search_SloppyPhraseScorer_
#if defined(_LUCENE_PRAGMA_ONCE)
# pragma once
#endif

#include "PhraseScorer.h"
#include "CLucene/index/Terms.h"


CL_NS_DEF(search)
	class SloppyPhraseScorer: public PhraseScorer {
	private:
		int32_t slop;

	public:
		SloppyPhraseScorer(Weight* weight, CL_NS(index)::TermPositions** tps, 
			int32_t* positions, Similarity* similarity, 
			int32_t slop, uint8_t* norms);
		~SloppyPhraseScorer(){
		}

	protected:
		qreal phraseFreq();
	};
CL_NS_END
#endif

