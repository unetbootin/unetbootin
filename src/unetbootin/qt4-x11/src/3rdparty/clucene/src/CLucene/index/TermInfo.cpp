/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
*
* Changes are Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
------------------------------------------------------------------------------*/

#include "CLucene/StdHeader.h"
#include "TermInfo.h"

CL_NS_DEF(index)

TermInfo::TermInfo()
{
    set(0, 0, 0, 0);
}

TermInfo::~TermInfo()
{
}

TermInfo::TermInfo(int32_t df, int64_t fp, int64_t pp)
{
    set(df, fp, pp, 0);
}

TermInfo::TermInfo(const TermInfo* ti)
{
    if (ti)
        set(ti);
}

void TermInfo::set(const TermInfo* ti)
{
    if (ti)
        set(ti->docFreq, ti->freqPointer, ti->proxPointer, ti->skipOffset);
}

void TermInfo::set(int32_t df, int64_t fp, int64_t pp, int32_t so)
{
    CND_PRECONDITION(df >= 0, "df contains negative number");
    CND_PRECONDITION(fp >= 0, "fp contains negative number");
    CND_PRECONDITION(pp >= 0, "pp contains negative number");

    docFreq = df;
    freqPointer = fp;
    proxPointer = pp;
    skipOffset  = so;
}

CL_NS_END
