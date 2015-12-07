/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#ifndef _lucene_analysis_standard_StandardTokenizerConstants
#define _lucene_analysis_standard_StandardTokenizerConstants

#if defined(_LUCENE_PRAGMA_ONCE)
# pragma once
#endif

CL_NS_DEF2(analysis,standard)
  enum TokenTypes {
    _EOF,
    UNKNOWN,
    ALPHANUM,
    APOSTROPHE,
    ACRONYM,
    COMPANY,
    EMAIL,
    HOST,
    NUM,
    CJK
  };

  const static TCHAR* tokenImage[] = {
    _T("<EOF>"),
    _T("<UNKNOWN>"),
    _T("<ALPHANUM>"),
    _T("<APOSTROPHE>"),
    _T("<ACRONYM>"),
    _T("<COMPANY>"),
    _T("<EMAIL>"),
    _T("<HOST>"),
    _T("<NUM>"),
    _T("<CJK>")
  };

CL_NS_END2
#endif
