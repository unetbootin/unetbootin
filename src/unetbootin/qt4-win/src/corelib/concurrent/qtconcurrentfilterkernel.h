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

#ifndef QTCONCURRENT_FILTERKERNEL_H
#define QTCONCURRENT_FILTERKERNEL_H

#include <QtCore/qglobal.h>

#ifndef QT_NO_CONCURRENT

#include <QtCore/qtconcurrentiteratekernel.h>
#include <QtCore/qtconcurrentmapkernel.h>
#include <QtCore/qtconcurrentreducekernel.h>

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

QT_MODULE(Core)

#ifndef qdoc

namespace QtConcurrent {

// Implementation of filter
template <typename Sequence, typename KeepFunctor, typename ReduceFunctor>
class FilterKernel : public IterateKernel<typename Sequence::const_iterator, void>
{
    typedef ReduceKernel<ReduceFunctor, Sequence, typename Sequence::value_type> Reducer;
    typedef IterateKernel<typename Sequence::const_iterator, void> IterateKernelType;
    typedef typename ReduceFunctor::result_type T;

    Sequence reducedResult;
    Sequence &sequence;
    KeepFunctor keep;
    ReduceFunctor reduce;
    Reducer reducer;

public:
    FilterKernel(Sequence &_sequence, KeepFunctor _keep, ReduceFunctor _reduce)
        : IterateKernelType(_sequence.constBegin(), _sequence.constEnd()), reducedResult(),
          sequence(_sequence),
          keep(_keep),
          reduce(_reduce),
          reducer(OrderedReduce)
    { }

    bool runIteration(typename Sequence::const_iterator it, int index, T *)
    {
        IntermediateResults<typename Sequence::value_type> results;
        results.begin = index;
        results.end = index + 1;

            if (keep(*it))
                results.vector.append(*it);

            reducer.runReduce(reduce, reducedResult, results);
            return false;
    }

    bool runIterations(typename Sequence::const_iterator sequenceBeginIterator, int begin, int end, T *)
    {
        IntermediateResults<typename Sequence::value_type> results;
        results.begin = begin;
        results.end = end;
        results.vector.reserve(end - begin);

        int currentIndex = begin;
        while (currentIndex != end) {
            typename Sequence::const_iterator currentIterator =
                sequenceBeginIterator + currentIndex;
            if (keep(*currentIterator))
                results.vector.append(*currentIterator);
            ++currentIndex;
        }

        reducer.runReduce(reduce, reducedResult, results);
        return false;
    }

    void finish()
    {
        reducer.finish(reduce, reducedResult);
        sequence = reducedResult;
    }

    inline bool shouldThrottleThread()
    {
        return IterateKernelType::shouldThrottleThread() || reducer.shouldThrottle();
    }

    inline bool shouldStartThread()
    {
        return IterateKernelType::shouldStartThread() && reducer.shouldStartThread();
    }

    typedef void ReturnType;
    typedef void ResultType;
};

// Implementation of filter-reduce
template <typename ReducedResultType,
          typename Iterator,
          typename KeepFunctor,
          typename ReduceFunctor,
          typename Reducer = ReduceKernel<ReduceFunctor,
                                          ReducedResultType,
                                          typename Iterator::value_type> >
class FilteredReducedKernel : public IterateKernel<Iterator, ReducedResultType>
{
    ReducedResultType reducedResult;
    KeepFunctor keep;
    ReduceFunctor reduce;
    Reducer reducer;
    typedef IterateKernel<Iterator, ReducedResultType> IterateKernelType;

public:
    FilteredReducedKernel(Iterator begin,
                          Iterator end,
                          KeepFunctor _keep,
                          ReduceFunctor _reduce,
                          ReduceOptions reduceOption)
        : IterateKernelType(begin, end), reducedResult(), keep(_keep), reduce(_reduce), reducer(reduceOption)
    { }

#if 0
    FilteredReducedKernel(ReducedResultType initialValue,
                          KeepFunctor keep,
                          ReduceFunctor reduce,
                          ReduceOption reduceOption)
        : reducedResult(initialValue), keep(keep), reduce(reduce), reducer(reduceOption)
    { }
#endif

    bool runIteration(Iterator it, int index, ReducedResultType *)
    {
        IntermediateResults<typename Iterator::value_type> results;
        results.begin = index;
        results.end = index + 1;

        if (keep(*it))
            results.vector.append(*it);

        reducer.runReduce(reduce, reducedResult, results);
        return false;
    }

    bool runIterations(Iterator sequenceBeginIterator, int begin, int end, ReducedResultType *)
    {
        IntermediateResults<typename Iterator::value_type> results;
        results.begin = begin;
        results.end = end;
        results.vector.reserve(end - begin);

        int currentIndex = begin;
        while (currentIndex != end) {
            Iterator currentIterator = sequenceBeginIterator + currentIndex;
            if (keep(*currentIterator))
                results.vector.append(*currentIterator);
            ++currentIndex;
        }

        reducer.runReduce(reduce, reducedResult, results);
        return false;
    }

    void finish()
    {
        reducer.finish(reduce, reducedResult);
    }

    inline bool shouldThrottleThread()
    {
        return IterateKernelType::shouldThrottleThread() || reducer.shouldThrottle();
    }

    inline bool shouldStartThread()
    {
        return IterateKernelType::shouldStartThread() && reducer.shouldStartThread();
    }

    typedef ReducedResultType ReturnType;
    typedef ReducedResultType ResultType;
    ReducedResultType *result()
    {
        return &reducedResult;
    }
};

// Implementation of filter that reports individual results via QFutureInterface
template <typename Iterator, typename KeepFunctor>
class FilteredEachKernel : public IterateKernel<Iterator, typename Iterator::value_type>
{
    typedef typename Iterator::value_type T;
    typedef IterateKernel<Iterator, T> IterateKernelType;

    KeepFunctor keep;

public:
    typedef T ReturnType;
    typedef T ResultType;

    FilteredEachKernel(Iterator begin, Iterator end, KeepFunctor _keep)
        : IterateKernelType(begin, end), keep(_keep)
    { }

    void start()
    {
        if (this->futureInterface)
            this->futureInterface->setFilterMode(true);
        IterateKernelType::start();
    }

    bool runIteration(Iterator it, int index, T *)
    {
        if (keep(*it))
            this->reportResult(&(*it), index);
        else
            this->reportResult(0, index);
        return false;
    }

    bool runIterations(Iterator sequenceBeginIterator, int begin, int end, T *)
    {
        const int count = end - begin;
        IntermediateResults<typename Iterator::value_type> results;
        results.begin = begin;
        results.end = end;
        results.vector.reserve(count);

        int currentIndex = begin;
        while (currentIndex != end) {
            Iterator currentIterator = sequenceBeginIterator + currentIndex;
            if (keep(*currentIterator))
                results.vector.append(*currentIterator);
            ++currentIndex;
        }

        this->reportResults(results.vector, begin, count);
        return false;
    }
};

template <typename Iterator, typename KeepFunctor>
inline
ThreadEngineStarter<typename Iterator::value_type>
startFiltered(Iterator begin, Iterator end, KeepFunctor functor)
{
    return startThreadEngine(new FilteredEachKernel<Iterator, KeepFunctor>(begin, end, functor));
}

template <typename Sequence, typename KeepFunctor>
inline ThreadEngineStarter<typename Sequence::value_type>
startFiltered(const Sequence &sequence, KeepFunctor functor)
{
    typedef SequenceHolder1<Sequence,
                            FilteredEachKernel<typename Sequence::const_iterator, KeepFunctor>,
                            KeepFunctor>
        SequenceHolderType;
        return startThreadEngine(new SequenceHolderType(sequence, functor));
}

template <typename ResultType, typename Sequence, typename MapFunctor, typename ReduceFunctor>
inline ThreadEngineStarter<ResultType> startFilteredReduced(const Sequence & sequence,
                                                           MapFunctor mapFunctor, ReduceFunctor reduceFunctor,
                                                           ReduceOptions options)
{
    typedef typename Sequence::const_iterator Iterator;
    typedef ReduceKernel<ReduceFunctor, ResultType, typename Iterator::value_type> Reducer;
    typedef FilteredReducedKernel<ResultType, Iterator, MapFunctor, ReduceFunctor, Reducer> FilteredReduceType;
    typedef SequenceHolder2<Sequence, FilteredReduceType, MapFunctor, ReduceFunctor> SequenceHolderType;
    return startThreadEngine(new SequenceHolderType(sequence, mapFunctor, reduceFunctor, options));
}


template <typename ResultType, typename Iterator, typename MapFunctor, typename ReduceFunctor>
inline ThreadEngineStarter<ResultType> startFilteredReduced(Iterator begin, Iterator end,
                                                           MapFunctor mapFunctor, ReduceFunctor reduceFunctor,
                                                           ReduceOptions options)
{
    typedef ReduceKernel<ReduceFunctor, ResultType, typename Iterator::value_type> Reducer;
    typedef FilteredReducedKernel<ResultType, Iterator, MapFunctor, ReduceFunctor, Reducer> FilteredReduceType;
    return startThreadEngine(new FilteredReduceType(begin, end, mapFunctor, reduceFunctor, options));
}


} // namespace QtConcurrent

#endif // qdoc

QT_END_NAMESPACE
QT_END_HEADER

#endif // QT_NO_CONCURRENT

#endif
