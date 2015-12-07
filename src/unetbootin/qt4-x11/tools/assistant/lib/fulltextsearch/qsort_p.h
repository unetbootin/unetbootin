/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QCLucene library and is distributable under
** the terms of the LGPL license as specified in the license.txt file.
**
****************************************************************************/

#ifndef QSORT_P_H
#define QSORT_P_H

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

#include "qclucene_global_p.h"

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QSharedDataPointer>
#include <QtCore/QSharedData>

CL_NS_DEF(search)
    class Sort;
CL_NS_END
CL_NS_USE(search)

QT_BEGIN_NAMESPACE

class QCLuceneHits;
class QCLuceneField;

class QHELP_EXPORT QCLuceneSortPrivate : public QSharedData
{
public:
    QCLuceneSortPrivate();
    QCLuceneSortPrivate (const QCLuceneSortPrivate &other);

    ~QCLuceneSortPrivate();

    Sort *sort;
    bool deleteCLuceneSort;

private:
    QCLuceneSortPrivate &operator=(const QCLuceneSortPrivate &other);
};

class QHELP_EXPORT QCLuceneSort
{
public:
	QCLuceneSort();
	QCLuceneSort(const QStringList &fieldNames);
    QCLuceneSort(const QString &field, bool reverse = false);

    virtual ~QCLuceneSort();

    QString toString() const;
    void setSort(const QStringList &fieldNames);
	void setSort(const QString &field, bool reverse = false);

protected:
    friend class QCLuceneHits;
    QSharedDataPointer<QCLuceneSortPrivate> d;
};

QT_END_NAMESPACE

#endif  // QSORT_P_H
