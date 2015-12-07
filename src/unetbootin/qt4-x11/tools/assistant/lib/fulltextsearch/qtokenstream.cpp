/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QCLucene library and is distributable under
** the terms of the LGPL license as specified in the license.txt file.
**
****************************************************************************/

#include "qtokenstream_p.h"

#include <CLucene.h>
#include <CLucene/analysis/AnalysisHeader.h>

QT_BEGIN_NAMESPACE

QCLuceneTokenStreamPrivate::QCLuceneTokenStreamPrivate()
    : QSharedData()
{
    tokenStream = 0;
    deleteCLuceneTokenStream = true;
}

QCLuceneTokenStreamPrivate::QCLuceneTokenStreamPrivate(const QCLuceneTokenStreamPrivate &other)
    : QSharedData()
{
    tokenStream = _CL_POINTER(other.tokenStream);
}

QCLuceneTokenStreamPrivate::~QCLuceneTokenStreamPrivate()
{
    if (deleteCLuceneTokenStream)
        _CLDECDELETE(tokenStream);
}


QCLuceneTokenStream::QCLuceneTokenStream()
    : d(new QCLuceneTokenStreamPrivate())
{
    // nothing todo
}

QCLuceneTokenStream::~QCLuceneTokenStream()
{
    // nothing todo
}

void QCLuceneTokenStream::close()
{
    d->tokenStream->close();
}

bool QCLuceneTokenStream::next(QCLuceneToken &token)
{
    return d->tokenStream->next(token.d->token);   
}

QT_END_NAMESPACE