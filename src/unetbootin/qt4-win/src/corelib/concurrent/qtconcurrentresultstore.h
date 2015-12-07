/****************************************************************************
**
** Copyright (C) 2005-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#ifndef QTCONCURRENT_RESULTSTORE_H
#define QTCONCURRENT_RESULTSTORE_H

#include <QtCore/qglobal.h>

#ifndef QT_NO_QFUTURE

#include <QtCore/qmap.h>
#include <QtCore/qdebug.h>

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

QT_MODULE(Core)

/*
    ResultStore stores indexed results. Results can be added and retrieved
    either individually batched in a QVector. Retriveing results and checking
    which indexes are in the store can be done either by iterating or by random
    accees. In addition results kan be removed from the front of the store,
    either individually or in batches.
*/

#ifndef qdoc

namespace QtConcurrent {

class ResultItem
{
public:
    ResultItem(const void *_result, int _count) : m_count(_count), result(_result) { } // contruct with vector of results
    ResultItem(const void *_result) : m_count(0), result(_result) { } // construct with result
    ResultItem() : m_count(0), result(0) { }
    bool isValid() const { return result != 0; }
    bool isVector() const { return m_count != 0; }
    int count() const { return (m_count == 0) ?  1 : m_count; }
    int m_count;          // result is either a pointer to a result or to a vector of results,
    const void *result; // if count is 0 it's a result, otherwise it's a vector.
};

class Q_CORE_EXPORT ResultIteratorBase
{
public:
    ResultIteratorBase();
    ResultIteratorBase(QMap<int, ResultItem>::const_iterator _mapIterator, int _vectorIndex = 0);
    int vectorIndex() const;
    int resultIndex() const;

    ResultIteratorBase operator++();
    int batchSize() const;
    void batchedAdvance();
    bool operator==(const ResultIteratorBase &other) const;
    bool operator!=(const ResultIteratorBase &other) const;
    bool isVector() const;
    bool canIncrementVectorIndex() const;
protected:
    QMap<int, ResultItem>::const_iterator mapIterator;
    int m_vectorIndex;
};

template <typename T>
class  ResultIterator : public ResultIteratorBase
{
public:
    ResultIterator(const ResultIteratorBase &base)
    : ResultIteratorBase(base) { }

    const T &value() const
    {
        return *pointer();
    }

    const T *pointer() const
    {
        if (mapIterator.value().isVector())
            return &(reinterpret_cast<const QVector<T> *>(mapIterator.value().result)->at(m_vectorIndex));
        else
            return reinterpret_cast<const T *>(mapIterator.value().result);
    }
};

class Q_CORE_EXPORT ResultStoreBase
{
public:
    ResultStoreBase();
    void setFilterMode(bool enable);
    bool filterMode() const;
    int addResult(int index, const void *result);
    int addResults(int index, const void *results, int vectorSize, int logicalCount);
    ResultIteratorBase begin() const;
    ResultIteratorBase end() const;
    bool hasNextResult() const;
    ResultIteratorBase resultAt(int index) const;
    bool contains(int index) const;
    int count() const;
    virtual ~ResultStoreBase() { };

protected:
    int insertResultItem(int index, ResultItem &resultItem);
    void insertResultItemIfValid(int index, ResultItem &resultItem);
    void syncPendingResults();
    void syncResultCount();
    int updateInsertIndex(int index, int _count);

    QMap<int, ResultItem> m_results;
    int insertIndex;     // The index where the next results(s) will be inserted.
    int resultCount;     // The number of consecutive results stored, starting at index 0.

    bool m_filterMode;
    QMap<int, ResultItem> pendingResults;
    int filteredResults;
    
};

template <typename T>
class ResultStore : public ResultStoreBase
{
public:
    ResultStore() { }

    ResultStore(const ResultStoreBase &base)
    : ResultStoreBase(base) { }

    int addResult(int index, const T  *result)
    {
        if (result == 0)
            return ResultStoreBase::addResult(index, result);
        else
            return ResultStoreBase::addResult(index, new T(*result));
    }

    int addResults(int index, const QVector<T> *results)
    {
        return ResultStoreBase::addResults(index, new QVector<T>(*results), results->count(), results->count());
    }

    int addResults(int index, const QVector<T> *results, int totalCount)
    {
        return ResultStoreBase::addResults(index, new QVector<T>(*results), results->count(), totalCount);
    }

    int addCanceledResult(int index)
    {
        return addResult(index, 0);
    }

    int addCanceledResults(int index, int _count)
    {
        QVector<T> empty;
        return addResults(index, &empty, _count);
    }

    ResultIterator<T> begin() const
    {
        return static_cast<ResultIterator<T> >(ResultStoreBase::begin());
    }

    ResultIterator<T> end() const
    {
        return static_cast<ResultIterator<T> >(ResultStoreBase::end());
    }

    ResultIterator<T> resultAt(int index) const
    {
        return static_cast<ResultIterator<T> >(ResultStoreBase::resultAt(index));
    }

    void clear()
    {
        QMap<int, ResultItem>::const_iterator mapIterator = m_results.begin();
        while (mapIterator != m_results.end()) {
            if (mapIterator.value().isVector())
                delete reinterpret_cast<const QVector<T> *>(mapIterator.value().result);
            else
                delete reinterpret_cast<const T *>(mapIterator.value().result);
            ++mapIterator;
        }
        resultCount = 0;
        m_results.clear();
    }

    ~ResultStore()
    {
        clear();
    }

};

} // namespace QtConcurrent

#endif //qdoc

QT_END_NAMESPACE
QT_END_HEADER

#endif // QT_NO_CONCURRENT

#endif
