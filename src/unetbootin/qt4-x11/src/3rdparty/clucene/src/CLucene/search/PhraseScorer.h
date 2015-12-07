/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#ifndef _lucene_search_PhraseScorer_
#define _lucene_search_PhraseScorer_

#if defined(_LUCENE_PRAGMA_ONCE)
# pragma once
#endif

#include "PhraseQueue.h"
#include "PhrasePositions.h"
#include "Scorer.h"
#include "Similarity.h"

CL_NS_DEF(search)

	class PhraseScorer: public Scorer {
	private:
		Weight* weight;
		qreal freq;
		bool firstTime;
		bool more;

	protected:
		uint8_t* norms;
		qreal value;

		PhraseQueue* pq;        //is used to order the list point to by first and last
		PhrasePositions* first; //Points to the first in the list of PhrasePositions
		PhrasePositions* last;  //Points to the last in the list of PhrasePositions

	public:
		//Constructor
		PhraseScorer(Weight* weight, CL_NS(index)::TermPositions** tps, 
		int32_t* positions, Similarity* similarity, uint8_t* norms);
		virtual ~PhraseScorer();

		int32_t doc() const { return first->doc; }
		bool next();
		qreal score();
		bool skipTo(int32_t target);


		void explain(int32_t doc, Explanation* ret);
		TCHAR* toString();
	protected:
		virtual qreal phraseFreq() =0;

        //Transfers the PhrasePositions from the PhraseQueue pq to
        //the PhrasePositions list with first as its first element
		void pqToList();

		//Moves first to the end of the list
		void firstToLast();
	private:
		bool doNext();
		void init();
		void sort();
	};
CL_NS_END
#endif
