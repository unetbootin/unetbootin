/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#include "CLucene/StdHeader.h"
#include "StandardAnalyzer.h"

#include "CLucene/util/VoidMap.h"
#include "CLucene/util/Reader.h"
#include "CLucene/analysis/AnalysisHeader.h"
#include "CLucene/analysis/Analyzers.h"
#include "StandardFilter.h"
#include "StandardTokenizer.h"

CL_NS_USE(util)
CL_NS_USE(analysis)

CL_NS_DEF2(analysis,standard)

	StandardAnalyzer::StandardAnalyzer():
		stopSet(false)
	{
      StopFilter::fillStopTable( &stopSet,CL_NS(analysis)::StopAnalyzer::ENGLISH_STOP_WORDS);
	}

	StandardAnalyzer::StandardAnalyzer( const TCHAR** stopWords):
		stopSet(false)
	{
		StopFilter::fillStopTable( &stopSet,stopWords );
	}

	StandardAnalyzer::~StandardAnalyzer(){
	}


	TokenStream* StandardAnalyzer::tokenStream(const TCHAR* fieldName, Reader* reader) 
	{
		TokenStream* ret = _CLNEW StandardTokenizer(reader);
		ret = _CLNEW StandardFilter(ret,true);
		ret = _CLNEW LowerCaseFilter(ret,true);
		ret = _CLNEW StopFilter(ret,true, &stopSet);
		return ret;
	}
CL_NS_END2
