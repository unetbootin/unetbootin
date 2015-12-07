/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#ifndef _lucene_search_ExactPhraseScorer_
#define _lucene_search_ExactPhraseScorer_

#if defined(_LUCENE_PRAGMA_ONCE)
# pragma once
#endif

#include "PhraseScorer.h"
#include "CLucene/index/Terms.h"

CL_NS_DEF(search)

  class ExactPhraseScorer: public PhraseScorer {
    public:
    ExactPhraseScorer(Weight* weight, CL_NS(index)::TermPositions** tps, int32_t* positions, 
       Similarity* similarity, uint8_t* norms );
       
	~ExactPhraseScorer(){
	}
    protected:
      //Returns the exact freqency of the phrase
      qreal phraseFreq();
    };
CL_NS_END
#endif
