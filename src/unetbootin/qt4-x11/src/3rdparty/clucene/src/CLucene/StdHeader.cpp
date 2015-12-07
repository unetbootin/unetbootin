/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
*
* Changes are Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
------------------------------------------------------------------------------*/
#include "CLucene/StdHeader.h"
#include "CLucene/util/Misc.h"

#include "CLucene/search/Sort.h"
#include "CLucene/search/Similarity.h"
#include "CLucene/search/FieldCache.h"
#include "CLucene/search/FieldSortedHitQueue.h"

#if defined(_CLCOMPILER_MSVC) && defined(_DEBUG)
#   define CRTDBG_MAP_ALLOC
#   include <stdlib.h>
#   include <crtdbg.h>
#endif

CL_NS_USE(util)

TCHAR* _LUCENE_BLANK_STRING = _T("");
char* _LUCENE_BLANK_ASTRING = "";

#ifndef Q_CC_MIPS
#if defined(_LUCENE_THREADMUTEX_USINGDEFAULT)
#    if defined(_LUCENE_PRAGMA_WARNINGS)
#        pragma message ("==================Using clunky thread mutex!!!==================")
#    else
#       if !defined(Q_OS_SOLARIS)
#           warning "==================Using clunky thread mutex!!!=================="
#       endif
#    endif
#endif

#if defined(_ASCII)
#    if defined(_LUCENE_PRAGMA_WARNINGS) 
#        pragma message ("==================Using ascii mode!!!==================")
#    else
#        if !defined(Q_OS_SOLARIS)	
#            warning "==================Using ascii mode!!!=================="
#        endif
#    endif
#endif

//This causes confusion, because CLucene doesn't really need hashed maps/sets. My experience with the
//hash maps on linux are that there are no significant improvements in using them (infact it adversely
//affected performance... therefore we'll just silently ignore
/*#if defined(LUCENE_DISABLE_HASHING)
#    if defined(_LUCENE_PRAGMA_WARNINGS)
#        pragma message ("==================Hashing not available or is disabled! CLucene may run slower than optimal ==================")
#    else
#        if !defined(Q_OS_SOLARIS)
#            warning "==================Hashing not available or is disabled! CLucene may run slower than optimal =================="
#        endif
#    endif
#endif*/
#endif

//clears all static memory. do not attempt to do anything else
//in clucene after calling this function
void _lucene_shutdown(){
    CL_NS(search)::FieldSortedHitQueue::Comparators.clear();
	_CLDELETE(CL_NS(search)::Sort::RELEVANCE);
	_CLDELETE(CL_NS(search)::Sort::INDEXORDER);
	_CLDELETE(CL_NS(search)::ScoreDocComparator::INDEXORDER);
	_CLDELETE(CL_NS(search)::ScoreDocComparator::RELEVANCE);
	_CLDELETE(CL_NS(search)::SortField::FIELD_SCORE);
	_CLDELETE(CL_NS(search)::SortField::FIELD_DOC);
	_CLDELETE(CL_NS(search)::FieldCache::DEFAULT);

	_CLLDELETE(CL_NS(search)::Similarity::getDefault());

    CL_NS(util)::CLStringIntern::shutdown();
}

void CLDebugBreak(){
	//can be used for debug breaking...
#if defined(_CLCOMPILER_MSVC) && defined(_DEBUG)
	_CrtDbgBreak();
#else
	int i=0; //a line to put breakpoint on
#endif
}

//these are functions that lucene uses which
//are not replacement functions
char* lucenestrdup(const char* v CL_FILELINEPARAM){
    size_t len = strlen(v);
    char* ret = new char[len+1];
    strncpy(ret,v,len+1);
#if defined(LUCENE_ENABLE_MEMLEAKTRACKING)
#   if defined(LUCENE_ENABLE_FILELINEINFO)
		CL_NS(debug)::LuceneBase::__cl_voidpadd((void*)ret,file,line,len);
#   else
		CL_NS(debug)::LuceneBase::__cl_voidpadd((void*)ret,__FILE__,__LINE__,len);
#   endif
#endif
    return ret;
}

#ifdef _UCS2
wchar_t* lucenewcsdup(const wchar_t* v CL_FILELINEPARAM){
    size_t len = _tcslen(v);
    wchar_t* ret = new wchar_t[len+1];
    _tcsncpy(ret,v,len+1);
#if defined(LUCENE_ENABLE_MEMLEAKTRACKING)
#   if defined(LUCENE_ENABLE_FILELINEINFO)
		CL_NS(debug)::LuceneBase::__cl_voidpadd((void*)ret,file,line,len);
#   else
		CL_NS(debug)::LuceneBase::__cl_voidpadd((void*)ret,__FILE__,__LINE__,len);
#   endif
#endif
    return ret;
}
#endif //ucs2


//ok, these are the exceptions, but these never
//exist on non-msvc platform, so lets put it here
#ifndef _CL_HAVE_FILELENGTH
int64_t lucene_filelength(int filehandle)
{
    struct fileStat info;
    if (fileHandleStat(filehandle, &info) == -1)
 	 _CLTHROWA( CL_ERR_IO,"fileStat error" );
    return info.st_size;
}
#endif
