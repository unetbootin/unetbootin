/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#ifndef _lucene_search_FilteredTermEnum_
#define _lucene_search_FilteredTermEnum_

#if defined(_LUCENE_PRAGMA_ONCE)
# pragma once
#endif

#include "CLucene/index/Term.h"
#include "CLucene/index/Terms.h"

CL_NS_DEF(search)
  //FilteredTermEnum is an abstract class for enumerating a subset of all terms. 
  //
  //Term enumerations are always ordered by term->compareTo().  Each term in
  //the enumeration is greater than all that precede it. 
  
  class FilteredTermEnum: public CL_NS(index)::TermEnum {
  public:
      //Constructor
      FilteredTermEnum();
	  //Destructor
      virtual ~FilteredTermEnum();
        
      //Equality measure on the term
      virtual qreal difference() = 0;

      //Returns the docFreq of the current Term in the enumeration.
      int32_t docFreq() const ;
        
      //Increments the enumeration to the next element
      bool next() ;
        
      //Returns a pointer to the current Term in the enumeration.
      CL_NS(index)::Term* term();
      CL_NS(index)::Term* term(bool pointer);
        
      //Closes the enumeration to further activity, freeing resources.
      void close();

    protected:
      //Equality compare on the term */
      virtual bool termCompare(CL_NS(index)::Term* term) = 0;
        
      //Indiciates the end of the enumeration has been reached
      virtual bool endEnum() = 0;
        
      void setEnum(CL_NS(index)::TermEnum* actualEnum) ;
    
    private:
        CL_NS(index)::Term* currentTerm;
        CL_NS(index)::TermEnum* actualEnum;
        
    };
CL_NS_END
#endif
