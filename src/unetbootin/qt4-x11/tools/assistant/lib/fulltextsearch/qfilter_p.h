/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QCLucene library and is distributable under
** the terms of the LGPL license as specified in the license.txt file.
**
****************************************************************************/

#ifndef QFilter_P_H
#define QFilter_P_H

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

#include <QtCore/QSharedData>
#include <QtCore/QSharedDataPointer>

CL_NS_DEF(search)
    class Filter;
CL_NS_END
CL_NS_USE(search)

QT_BEGIN_NAMESPACE

class QCLuceneHits;
class QCLuceneSearcher;

class QHELP_EXPORT QCLuceneFilterPrivate : public QSharedData
{
public:
    QCLuceneFilterPrivate();
    QCLuceneFilterPrivate(const QCLuceneFilterPrivate &other);

    ~QCLuceneFilterPrivate ();

    Filter *filter;
    bool deleteCLuceneFilter;

private:
    QCLuceneFilterPrivate &operator=(const QCLuceneFilterPrivate &other);
};

class QHELP_EXPORT QCLuceneFilter
{
    QCLuceneFilter();
    virtual ~QCLuceneFilter();

protected:
    friend class QCLuceneHits;
    friend class QCLuceneSearcher;
    QSharedDataPointer<QCLuceneFilterPrivate> d;
};

QT_END_NAMESPACE

#endif  // QFilter_P_H
