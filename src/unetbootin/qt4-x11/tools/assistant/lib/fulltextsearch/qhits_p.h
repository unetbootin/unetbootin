/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QCLucene library and is distributable under
** the terms of the LGPL license as specified in the license.txt file.
**
****************************************************************************/

#ifndef QHITS_P_H
#define QHITS_P_H

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

#include "qsort_p.h"
#include "qquery_p.h"
#include "qhits_p.h"
#include "qfilter_p.h"
#include "qdocument_p.h"
#include "qsearchable_p.h"
#include "qclucene_global_p.h"

#include <QtCore/QSharedDataPointer>
#include <QtCore/QSharedData>

CL_NS_DEF(search)
    class Hits;
CL_NS_END
CL_NS_USE(search)

QT_BEGIN_NAMESPACE

class QCLuceneSearcher;

class QHELP_EXPORT QCLuceneHitsPrivate : public QSharedData
{
public:
    QCLuceneHitsPrivate();
    QCLuceneHitsPrivate(const QCLuceneHitsPrivate &other);

    ~QCLuceneHitsPrivate();

    Hits *hits;
    bool deleteCLuceneHits;

private:
    QCLuceneHitsPrivate &operator=(const QCLuceneHitsPrivate &other);
};

class QHELP_EXPORT QCLuceneHits
{
public:
    QCLuceneHits(const QCLuceneSearcher &searcher, const QCLuceneQuery &query, 
        const QCLuceneFilter &filter);
    QCLuceneHits(const QCLuceneSearcher &searcher, const QCLuceneQuery &query, 
        const QCLuceneFilter &filter, const QCLuceneSort &sort);
    virtual ~QCLuceneHits();

    QCLuceneDocument document(const qint32 index);
    qint32 length() const;
	qint32 id (const qint32 index);
	qreal score(const qint32 index);

protected:
    friend class QCLuceneSearcher;
    QSharedDataPointer<QCLuceneHitsPrivate> d;
};

QT_END_NAMESPACE

#endif  // QHITS_P_H
