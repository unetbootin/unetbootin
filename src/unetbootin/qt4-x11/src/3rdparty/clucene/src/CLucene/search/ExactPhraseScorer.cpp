/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#include "CLucene/StdHeader.h"
#include "ExactPhraseScorer.h"

#include "PhraseScorer.h"
#include "CLucene/index/Terms.h"

CL_NS_USE(index)
CL_NS_DEF(search)

	ExactPhraseScorer::ExactPhraseScorer(Weight* weight, TermPositions** tps, 
		int32_t* positions, Similarity* similarity, uint8_t* norms):
		PhraseScorer(weight, tps, positions, similarity, norms){
	//Func - Constructor
	//Pre  - tps != NULL
	//       tpsLength >= 0
	//       n != NULL
	//Post - Instance has been created

		CND_PRECONDITION(tps != NULL,"tps is NULL");
		CND_PRECONDITION(tps[0] != NULL,"tps is NULL");
		//CND_PRECONDITION(n != NULL,"n is NULL") =this is checked already in PhraseScorer

	}

	qreal ExactPhraseScorer::phraseFreq(){
	//Func - Returns the freqency of the phrase
	//Pre  - first != NULL
	//       last  != NULL
	//       pq    != NULL
	//       size of the PhraseQueue pq is 0
	//Post - The frequency of the phrase has been returned

		CND_PRECONDITION(first != NULL,"first is NULL");
		CND_PRECONDITION(last  != NULL,"last is NULL");
		CND_PRECONDITION(pq    != NULL,"pq is NULL");
		CND_PRECONDITION(pq->size()==0,"pq is not empty");

		//build pq from list

		//Add the nodes of the list of PhrasePositions and store them
		//into the PhraseQueue pq so it can used to build
		//a list of sorted nodes
		for (PhrasePositions* pp = first; pp != NULL; pp = pp->_next) {
			//Read the first TermPosition of the current PhrasePositions pp
			pp->firstPosition();
			//Store the current PhrasePositions pp into the PhraseQueue pq
			pq->put(pp);
		}
		//pqToList requires that first and last be NULL when it's called.  
		//This is done at the beginning of pqToList()
		//In this case, the nodes of the linked list are referenced by pq (see
		//above loop), so we can clear our pointers to the head and tail of the
		//linked list without fear of leaking the nodes.
		
		//rebuild list from pq
		pqToList();

		//Initialize freq at 0
		int32_t freq = 0;

		//find position with all terms
		do {
			//scan forward in first
			while (first->position < last->position){
				do{
					if (!first->nextPosition()){
						return (qreal)freq;
					}
				} while (first->position < last->position);
				//Make the current first node the last node in the list
				firstToLast();
			}
			//all equal: a match has been found
			freq++;
		} while (last->nextPosition());

		return (qreal)freq;
	}
CL_NS_END
