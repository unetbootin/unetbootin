/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QCLucene library and is distributable under
** the terms of the LGPL license as specified in the license.txt file.
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
