/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#include "CLucene/StdHeader.h"
#include "StandardFilter.h"

#include "../AnalysisHeader.h"
#include "../Analyzers.h"
#include "StandardTokenizerConstants.h"
#include "CLucene/util/StringBuffer.h"

CL_NS_USE(analysis)
CL_NS_USE(util)
CL_NS_DEF2(analysis,standard)

  StandardFilter::StandardFilter(TokenStream* in, bool deleteTokenStream):
    TokenFilter(in, deleteTokenStream)
  {
  }

  StandardFilter::~StandardFilter(){
  }

  bool StandardFilter::next(Token* t) {
    if (!input->next(t))
      return false;

    TCHAR* text = t->_termText;
    const int32_t textLength = t->termTextLength();
    const TCHAR* type = t->type();

    if ( type == tokenImage[APOSTROPHE] && //we can compare the type directy since the type should always come from the tokenImage
		( textLength >= 2 && _tcsicmp(text+textLength-2, _T("'s"))==0  ) )
    {
      // remove 's
      text[textLength-2]=0; 
	  t->resetTermTextLen();

      return true;

    } else if ( type == tokenImage[ACRONYM] ) {		  // remove dots
		int32_t j = 0;
		for ( int32_t i=0;i<textLength;i++ ){
			if ( text[i] != '.' )
				text[j++]=text[i];
		}
		text[j]=0;
      return true;

    } else {
      return true;
    }
  }

CL_NS_END2
