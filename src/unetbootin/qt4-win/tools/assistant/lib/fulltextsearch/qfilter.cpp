/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QCLucene library and is distributable under
** the terms of the LGPL license as specified in the license.txt file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "qfilter_p.h"

#include <CLucene.h>
#include <CLucene/search/Filter.h>

QT_BEGIN_NAMESPACE

QCLuceneFilterPrivate::QCLuceneFilterPrivate()
    : QSharedData()
{
    filter = 0;
    deleteCLuceneFilter = true;
}

QCLuceneFilterPrivate::QCLuceneFilterPrivate(const QCLuceneFilterPrivate &other)
    : QSharedData()
{
    filter = _CL_POINTER(other.filter);
}

QCLuceneFilterPrivate::~QCLuceneFilterPrivate ()
{
    if (deleteCLuceneFilter)
        _CLDECDELETE(filter);
}


QCLuceneFilter::QCLuceneFilter()
    : d(new QCLuceneFilterPrivate())
{
    // nothing todo
}

QCLuceneFilter::~QCLuceneFilter()
{
    // nothing todo
}

QT_END_NAMESPACE
