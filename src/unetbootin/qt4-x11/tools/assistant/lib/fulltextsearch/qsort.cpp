/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QCLucene library and is distributable under
** the terms of the LGPL license as specified in the license.txt file.
**
****************************************************************************/

#include "qsort_p.h"
#include "qclucene_global_p.h"

#include <CLucene.h>
#include <CLucene/search/Sort.h>

QT_BEGIN_NAMESPACE

QCLuceneSortPrivate::QCLuceneSortPrivate()
    : QSharedData()
{
    sort = 0;
    deleteCLuceneSort = true;
}

QCLuceneSortPrivate::QCLuceneSortPrivate (const QCLuceneSortPrivate &other)
    : QSharedData()
{
    sort = _CL_POINTER(other.sort);
}

QCLuceneSortPrivate::~QCLuceneSortPrivate()
{
    if (deleteCLuceneSort)
        _CLDECDELETE(sort);
}


QCLuceneSort::QCLuceneSort()
    : d(new QCLuceneSortPrivate())
{
    d->sort = new lucene::search::Sort();
}

QCLuceneSort::QCLuceneSort(const QStringList &fieldNames)
    : d(new QCLuceneSortPrivate())
{
    d->sort = new lucene::search::Sort();
    setSort(fieldNames);
}

QCLuceneSort::QCLuceneSort(const QString &field, bool reverse)
    : d(new QCLuceneSortPrivate())
{
    d->sort = new lucene::search::Sort();
    setSort(field, reverse);
}

QCLuceneSort::~QCLuceneSort()
{
    // nothing todo
}

QString QCLuceneSort::toString() const
{
    return TCharToQString(d->sort->toString());
}

void QCLuceneSort::setSort(const QStringList &fieldNames)
{
    TCHAR **nameArray = new TCHAR*[fieldNames.count()];
    for (int i = 0; i < fieldNames.count(); ++i)
        nameArray[i] = QStringToTChar(fieldNames.at(i));

    d->sort->setSort((const TCHAR**)nameArray);

    for (int i = 0; i < fieldNames.count(); ++i)
        delete [] nameArray[i];
    delete nameArray;
}

void QCLuceneSort::setSort(const QString &field, bool reverse)
{
    TCHAR *name = QStringToTChar(field);
    d->sort->setSort(name, reverse);
    delete [] name;
}

QT_END_NAMESPACE
