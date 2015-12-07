/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#ifndef _lucene_search_Scorer_
#define _lucene_search_Scorer_

#if defined(_LUCENE_PRAGMA_ONCE)
# pragma once
#endif

#include "Similarity.h"
#include "SearchHeader.h"
#include "Explanation.h"

CL_NS_DEF(search)
   /** Expert: Implements scoring for a class of queries. */
class Scorer: LUCENE_BASE {
   private:
      Similarity* similarity;
   protected:
      /** Constructs a Scorer. */
      Scorer(Similarity* similarity) {
         this->similarity = similarity;
      }
	public:
		virtual ~Scorer(){
		}

      /** Returns the Similarity implementation used by this scorer. */
      Similarity* getSimilarity()  const{
         return this->similarity;
      }

      /** Scores all documents and passes them to a collector. */
      void score(HitCollector* hc) {
         while (next()) {
            hc->collect(doc(), score());
         }
      }

      /** Advance to the next document matching the query.  Returns true iff there
         * is another match. */
      virtual bool next() = 0;

      /** Returns the current document number.  Initially invalid, until {@link
         * #next()} is called the first time. */
      virtual int32_t doc() const = 0;

      /** Returns the score of the current document.  Initially invalid, until
         * {@link #next()} is called the first time. */
      virtual qreal score() = 0;

      /** Skips to the first match beyond the current whose document number is
         * greater than or equal to <i>target</i>. <p>Returns true iff there is such
         * a match.  <p>Behaves as if written: <pre>
         *   boolean skipTo(int32_t target) {
         *     do {
         *       if (!next())
         * 	     return false;
         *     } while (target > doc());
         *     return true;
         *   }
         * </pre>
         * Most implementations are considerably more efficient than that.
         */
      virtual bool skipTo(int32_t target) = 0;

      /** Returns an explanation of the score for <code>doc</code>. */
      virtual void explain(int32_t doc, Explanation* ret) = 0;

	  
      /** Returns an string which explains the object */
      virtual TCHAR* toString() = 0;

    };
CL_NS_END
#endif
