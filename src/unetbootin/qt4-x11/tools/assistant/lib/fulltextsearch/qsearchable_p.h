/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QCLucene library and is distributable under
** the terms of the LGPL license as specified in the license.txt file.
**
****************************************************************************/

#ifndef QSEARCHABLE_P_H
#define QSEARCHABLE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API. It exists for the convenience
// of the help generator tools. This header file may change from version
// to version without notice, or even be removed.
//
// We mean it.
//

#include "qhits_p.h"
#include "qsort_p.h"
#include "qquery_p.h"
#include "qfilter_p.h"
#include "qdocument_p.h"
#include "qindexreader_p.h"
#include "qclucene_global_p.h"

#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QSharedDataPointer>
#include <QtCore/QSharedData>

CL_NS_DEF(search)
    class Searcher;
CL_NS_END
CL_NS_USE(search)

QT_BEGIN_NAMESPACE

class QCLuceneHits;
class QCLuceneSearcher;
class QCLuceneIndexSearcher;
class QCLuceneMultiSearcher;

class QHELP_EXPORT QCLuceneSearchablePrivate : public QSharedData
{
public:
    QCLuceneSearchablePrivate();
    QCLuceneSearchablePrivate(const QCLuceneSearchablePrivate &other);

    ~QCLuceneSearchablePrivate();

    Searcher *searchable;
    bool deleteCLuceneSearchable;

private:
    QCLuceneSearchablePrivate &operator=(const QCLuceneSearchablePrivate &other);
};

class QHELP_EXPORT QCLuceneSearchable
{
public:
    virtual ~QCLuceneSearchable();

protected:
    friend class QCLuceneSearcher;
    friend class QCLuceneIndexSearcher;
    friend class QCLuceneMultiSearcher;
    QSharedDataPointer<QCLuceneSearchablePrivate> d;

private:
    QCLuceneSearchable();
};

class QHELP_EXPORT QCLuceneSearcher : public QCLuceneSearchable
{
public:
    QCLuceneSearcher();
    virtual ~QCLuceneSearcher();

    QCLuceneHits search(const QCLuceneQuery &query);
    QCLuceneHits search(const QCLuceneQuery &query, const QCLuceneFilter &filter);
    QCLuceneHits search(const QCLuceneQuery &query, const QCLuceneSort &sort);
    QCLuceneHits search(const QCLuceneQuery &query, const QCLuceneFilter &filter, 
        const QCLuceneSort &sort);

protected:
    friend class QCLuceneHits;
};

class QHELP_EXPORT QCLuceneIndexSearcher : public QCLuceneSearcher
{
public:
    QCLuceneIndexSearcher(const QString &path);
    QCLuceneIndexSearcher(const QCLuceneIndexReader &reader);
    ~QCLuceneIndexSearcher();

    void close();
    qint32 maxDoc() const;
    QCLuceneIndexReader getReader();
    bool doc(qint32 i, QCLuceneDocument &document);

private:
    QCLuceneIndexReader reader;
};

class QHELP_EXPORT QCLuceneMultiSearcher : public QCLuceneSearcher
{
public:
    QCLuceneMultiSearcher(const QList<QCLuceneSearchable> searchables);
    ~QCLuceneMultiSearcher();

    void close();
    qint32 maxDoc() const;
    qint32 subDoc(qint32 index) const;
    qint32 subSearcher(qint32 index) const;
    qint32 searcherIndex(qint32 index) const;
    bool doc(qint32 i, QCLuceneDocument &document);
};

QT_END_NAMESPACE

#endif  // QSEARCHABLE_P_H
