/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtSql module of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License versions 2.0 or 3.0 as published by the Free Software
** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file.  Alternatively you may (at
** your option) use any later version of the GNU General Public
** License if such license has been publicly approved by Trolltech ASA
** (or its successors, if any) and the KDE Free Qt Foundation. In
** addition, as a special exception, Trolltech gives you certain
** additional rights. These rights are described in the Trolltech GPL
** Exception version 1.2, which can be found at
** http://www.trolltech.com/products/qt/gplexception/ and in the file
** GPL_EXCEPTION.txt in this package.
**
** Please review the following information to ensure GNU General
** Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/. If
** you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** In addition, as a special exception, Trolltech, as the sole
** copyright holder for Qt Designer, grants users of the Qt/Eclipse
** Integration plug-in the right for the Qt/Eclipse Integration to
** link to functionality provided by Qt Designer and its related
** libraries.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not expressly
** granted herein.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "private/qsqlcachedresult_p.h"

#include <qvariant.h>
#include <qdatetime.h>
#include <qvector.h>

QT_BEGIN_NAMESPACE

/*
   QSqlCachedResult is a convenience class for databases that only allow
   forward only fetching. It will cache all the results so we can iterate
   backwards over the results again.

   All you need to do is to inherit from QSqlCachedResult and reimplement
   gotoNext(). gotoNext() will have a reference to the internal cache and
   will give you an index where you can start filling in your data. Special
   case: If the user actually wants a forward-only query, idx will be -1
   to indicate that we are not interested in the actual values.
*/

static const uint initial_cache_size = 128;

class QSqlCachedResultPrivate
{
public:
    QSqlCachedResultPrivate();
    bool canSeek(int i) const;
    inline int cacheCount() const;
    void init(int count, bool fo);
    void cleanup();
    int nextIndex();
    void revertLast();

    QSqlCachedResult::ValueCache cache;
    int rowCacheEnd;
    int colCount;
    bool forwardOnly;
    bool atEnd;
};

QSqlCachedResultPrivate::QSqlCachedResultPrivate():
    rowCacheEnd(0), colCount(0), forwardOnly(false), atEnd(false)
{
}

void QSqlCachedResultPrivate::cleanup()
{
    cache.clear();
    forwardOnly = false;
    atEnd = false;
    colCount = 0;
    rowCacheEnd = 0;
}

void QSqlCachedResultPrivate::init(int count, bool fo)
{
    Q_ASSERT(count);
    cleanup();
    forwardOnly = fo;
    colCount = count;
    if (fo) {
        cache.resize(count);
        rowCacheEnd = count;
    } else {
        cache.resize(initial_cache_size * count);
    }
}

int QSqlCachedResultPrivate::nextIndex()
{
    if (forwardOnly)
        return 0;
    int newIdx = rowCacheEnd;
    if (newIdx + colCount > cache.size())
        cache.resize(qMin(cache.size() * 2, cache.size() + 10000));
    rowCacheEnd += colCount;

    return newIdx;
}

bool QSqlCachedResultPrivate::canSeek(int i) const
{
    if (forwardOnly || i < 0)
        return false;
    return rowCacheEnd >= (i + 1) * colCount;
}

void QSqlCachedResultPrivate::revertLast()
{
    if (forwardOnly)
        return;
    rowCacheEnd -= colCount;
}

inline int QSqlCachedResultPrivate::cacheCount() const
{
    Q_ASSERT(!forwardOnly);
    Q_ASSERT(colCount);
    return rowCacheEnd / colCount;
}

//////////////

QSqlCachedResult::QSqlCachedResult(const QSqlDriver * db): QSqlResult (db)
{
    d = new QSqlCachedResultPrivate();
}

QSqlCachedResult::~QSqlCachedResult()
{
    delete d;
}

void QSqlCachedResult::init(int colCount)
{
    d->init(colCount, isForwardOnly());
}

bool QSqlCachedResult::fetch(int i)
{
    if ((!isActive()) || (i < 0))
        return false;
    if (at() == i)
        return true;
    if (d->forwardOnly) {
        // speed hack - do not copy values if not needed
        if (at() > i || at() == QSql::AfterLastRow)
            return false;
        while(at() < i - 1) {
            if (!gotoNext(d->cache, -1))
                return false;
            setAt(at() + 1);
        }
        if (!gotoNext(d->cache, 0))
            return false;
        setAt(at() + 1);
        return true;
    }
    if (d->canSeek(i)) {
        setAt(i);
        return true;
    }
    if (d->rowCacheEnd > 0)
        setAt(d->cacheCount());
    while (at() < i + 1) {
        if (!cacheNext())
            return false;
    }
    setAt(i);

    return true;
}

bool QSqlCachedResult::fetchNext()
{
    if (d->canSeek(at() + 1)) {
        setAt(at() + 1);
        return true;
    }
    return cacheNext();
}

bool QSqlCachedResult::fetchPrevious()
{
    return fetch(at() - 1);
}

bool QSqlCachedResult::fetchFirst()
{
    if (d->forwardOnly && at() != QSql::BeforeFirstRow) {
        return false;
    }
    if (d->canSeek(0)) {
        setAt(0);
        return true;
    }
    return cacheNext();
}

bool QSqlCachedResult::fetchLast()
{
    if (d->atEnd) {
        if (d->forwardOnly)
            return false;
        else
            return fetch(d->cacheCount() - 1);
    }

    int i = at();
    while (fetchNext())
        ++i; /* brute force */
    if (d->forwardOnly && at() == QSql::AfterLastRow) {
        setAt(i);
        return true;
    } else {
        return fetch(i);
    }
}

QVariant QSqlCachedResult::data(int i)
{
    int idx = d->forwardOnly ? i : at() * d->colCount + i;
    if (i >= d->colCount || i < 0 || at() < 0 || idx >= d->rowCacheEnd)
        return QVariant();

    return d->cache.at(idx);
}

bool QSqlCachedResult::isNull(int i)
{
    int idx = d->forwardOnly ? i : at() * d->colCount + i;
    if (i > d->colCount || i < 0 || at() < 0 || idx >= d->rowCacheEnd)
        return true;

    return d->cache.at(idx).isNull();
}

void QSqlCachedResult::cleanup()
{
    setAt(QSql::BeforeFirstRow);
    setActive(false);
    d->cleanup();
}

void QSqlCachedResult::clearValues()
{
    setAt(QSql::BeforeFirstRow);
    d->rowCacheEnd = 0;
    d->atEnd = false;
}

bool QSqlCachedResult::cacheNext()
{
    if (d->atEnd)
        return false;

    if (!gotoNext(d->cache, d->nextIndex())) {
        d->revertLast();
        d->atEnd = true;
        return false;
    }
    setAt(at() + 1);
    return true;
}

int QSqlCachedResult::colCount() const
{
    return d->colCount;
}

QSqlCachedResult::ValueCache &QSqlCachedResult::cache()
{
    return d->cache;
}

QT_END_NAMESPACE
