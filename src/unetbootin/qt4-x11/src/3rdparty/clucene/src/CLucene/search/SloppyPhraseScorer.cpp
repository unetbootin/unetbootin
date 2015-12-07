/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#include "CLucene/StdHeader.h"
#include "SloppyPhraseScorer.h"

#include "PhraseScorer.h"
#include "CLucene/index/Terms.h"

CL_NS_USE(index)
CL_NS_DEF(search)

  SloppyPhraseScorer::SloppyPhraseScorer(Weight* weight, CL_NS(index)::TermPositions** tps, 
			int32_t* positions, Similarity* similarity, 
			int32_t slop, uint8_t* norms):
      PhraseScorer(weight,tps,positions,similarity,norms){
  //Func - Constructor
  //Pre  - tps != NULL 
  //       tpsLength >= 0
  //       n != NULL
  //Post - Instance has been created

      CND_PRECONDITION(tps != NULL, "tps is NULL");
      //CND_PRECONDITION(n != NULL, _T("n is NULL")) = checked in PhraseScorer;

      this->slop = slop;
  }

  qreal SloppyPhraseScorer::phraseFreq() {
  //Func - Returns the freqency of the phrase
  //Pre  - first != NULL
  //       last  != NULL
  //       pq    != NULL
  //Post - The frequency of the phrase has been returned

	  CND_PRECONDITION(first != NULL,"first is NULL");
	  CND_PRECONDITION(last  != NULL,"last is NULL");
	  CND_PRECONDITION(pq    != NULL,"pq is NULL");

	  //Clear the PhraseQueue pq;
      pq->clear();

      int32_t end = 0;

	  //declare iterator
      PhrasePositions* pp = NULL;

      // build pq from list

	  //Sort the list of PhrasePositions using pq
      for (pp = first; pp != NULL; pp = pp->_next) {
		  //Read the first TermPosition of the current PhrasePositions pp
          pp->firstPosition();
		  //Check if the position of the pp is bigger than end
		  if (pp->position > end){
              end = pp->position;
		      }
          //Store the current PhrasePositions pp into the PhraseQueue pp
          pq->put(pp);		
          }

     qreal freq = 0.0f;
     
	 bool done = false;
     
	 do {
		 //Pop a PhrasePositions pp from the PhraseQueue pp
         pp = pq->pop();
         //Get start position
         int32_t start = pp->position;
		 //Get next position
		 int32_t next = pq->top()->position;

         for (int32_t pos = start; pos <= next; pos = pp->position) {
             //advance pp to min window
             start = pos;				  
             
             if (!pp->nextPosition()) {
				 //ran out of a term -- done
                 done = true;
                 break;
                 }
             }
         
         //Calculate matchLength
		 int32_t matchLength = end - start;
         //Check if matchLength is smaller than slop
         if (matchLength <= slop){
             // penalize longer matches
             freq += 1.0 / (matchLength + 1);	  
             }

		 if (pp->position > end){
             end = pp->position;
             }
         
         //restore pq
		 pq->put(pp);				  
	 }while (!done);

     return freq;
  }
CL_NS_END
