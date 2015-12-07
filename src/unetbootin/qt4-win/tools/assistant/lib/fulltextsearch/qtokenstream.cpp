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