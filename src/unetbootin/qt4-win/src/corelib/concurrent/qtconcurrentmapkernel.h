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

#ifndef QTCONCURRENT_MAPKERNEL_H
#define QTCONCURRENT_MAPKERNEL_H

#include <QtCore/qglobal.h>

#ifndef QT_NO_CONCURRENT

#include <QtCore/qtconcurrentiteratekernel.h>
#include <QtCore/qtconcurrentreducekernel.h>

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

QT_MODULE(Core)

#ifndef qdoc
namespace QtConcurrent {

// map kernel, works with both parallel-for and parallel-while
template <typename Iterator, typename MapFunctor>
class MapKernel : public IterateKernel<Iterator, void>
{
    MapFunctor map;
public:
    typedef void ReturnType;
    MapKernel(Iterator begin, Iterator end, MapFunctor _map)
        : IterateKernel<Iterator, void>(begin, end), map(_map)
    { }

    bool runIteration(Iterator it, int, void *)
    {
        map(*it);
        return false;
    }

    bool runIterations(Iterator sequenceBeginIterator, int beginIndex, int endIndex, void *)
    {
        int currentIndex = beginIndex;
        while (currentIndex != endIndex) {
            runIteration((sequenceBeginIterator + currentIndex), currentIndex, 0);
            ++currentIndex;
        }
        return false;
    }
};

template <typename ReducedResultType,
          typename Iterator,
          typename MapFunctor,
          typename ReduceFunctor,
          typename Reducer = ReduceKernel<ReduceFunctor,
                                          ReducedResultType,
                                          typename MapFunctor::result_type> >
class MappedReducedKernel : public IterateKernel<Iterator, ReducedResultType>
{
    ReducedResultType reducedResult;
    MapFunctor map;
    ReduceFunctor reduce;
    Reducer reducer;
public:
    typedef ReducedResultType ReturnType;
    MappedReducedKernel(Iterator begin, Iterator end, MapFunctor _map, ReduceFunctor _reduce, ReduceOptions reduceOptions)
        : IterateKernel<Iterator, ReducedResultType>(begin, end), reducedResult(), map(_map), reduce(_reduce), reducer(reduceOptions)
    { }

    MappedReducedKernel(ReducedResultType initialValue,
                     MapFunctor _map,
                     ReduceFunctor _reduce)
        : reducedResult(initialValue), map(_map), reduce(_reduce)
    { }

    bool runIteration(Iterator it, int index, ReducedResultType *)
    {
        IntermediateResults<typename MapFunctor::result_type> results;
        results.begin = index;
        results.end = index + 1;

        results.vector.append(map(*it));
        reducer.runReduce(reduce, reducedResult, results);
        return false;
    }

    bool runIterations(Iterator sequenceBeginIterator, int begin, int end, ReducedResultType *)
    {
        IntermediateResults<typename MapFunctor::result_type> results;
        results.begin = begin;
        results.end = end;
        results.vector.reserve(end - begin);

        int currentIndex = begin;
        while (currentIndex != end) {
            results.vector.append(map(*(sequenceBeginIterator + currentIndex)));
            ++currentIndex;
        }

        reducer.runReduce(reduce, reducedResult, results);
        return false;
    }

    void finish()
    {
        reducer.finish(reduce, reducedResult);
    }

    bool shouldThrottleThread()
    {
        return IterateKernel<Iterator, ReducedResultType>::shouldThrottleThread() || reducer.shouldThrottle();
    }

    bool shouldStartThread()
    {
        return IterateKernel<Iterator, ReducedResultType>::shouldStartThread() && reducer.shouldStartThread();
    }

    typedef ReducedResultType ResultType;
    ReducedResultType *result()
    {
        return &reducedResult;
    }
};

template <typename Iterator, typename MapFunctor>
class MappedEachKernel : public IterateKernel<Iterator, typename MapFunctor::result_type>
{
    MapFunctor map;
    typedef typename MapFunctor::result_type T;
public:
    typedef T ReturnType;
    typedef T ResultType;

    MappedEachKernel(Iterator begin, Iterator end, MapFunctor _map)
        : IterateKernel<Iterator, T>(begin, end), map(_map) { }

    bool runIteration(Iterator it, int,  T *result)
    {
        *result = map(*it);
        return true;
    }

    bool runIterations(Iterator sequenceBeginIterator, int begin, int end, T *results)
    {
        int currentIndex = begin;
        while (currentIndex != end) {
            runIteration(sequenceBeginIterator + currentIndex, currentIndex,  results + (currentIndex - begin));
            ++currentIndex;
        }
        return true;
    }
};

template <typename Iterator, typename Functor>
inline ThreadEngineStarter<void> startMap(Iterator begin, Iterator end, Functor functor)
{
    return startThreadEngine(new MapKernel<Iterator, Functor>(begin, end, functor));
}

template <typename T, typename Iterator, typename Functor>
inline ThreadEngineStarter<T> startMapped(Iterator begin, Iterator end, Functor functor)
{
    return startThreadEngine(new MappedEachKernel<Iterator, Functor>(begin, end, functor));
}

/*
    The SequnceHolder class is used to hold a reference to the
    sequence we are working on.
*/
template <typename Sequence, typename Base, typename Functor>
struct SequenceHolder1 : public Base
{
    SequenceHolder1(const Sequence &_sequence, Functor functor)
        : Base(_sequence.constBegin(), _sequence.constEnd(), functor), sequence(_sequence)
    { }

    Sequence sequence;

    void finish()
    {
        Base::finish();
        // Clear the sequence to make sure all temporaries are destroyed
        // before finished is signaled.
        sequence = Sequence();
    }
};

template <typename T, typename Sequence, typename Functor>
inline ThreadEngineStarter<T> startMapped(const Sequence &sequence, Functor functor)
{
    typedef SequenceHolder1<Sequence,
                            MappedEachKernel<typename Sequence::const_iterator , Functor>, Functor>
                            SequenceHolderType;

    return startThreadEngine(new SequenceHolderType(sequence, functor));
}

template <typename IntermediateType, typename ResultType, typename Sequence, typename MapFunctor, typename ReduceFunctor>
inline ThreadEngineStarter<ResultType> startMappedReduced(const Sequence & sequence,
                                                           MapFunctor mapFunctor, ReduceFunctor reduceFunctor,
                                                           ReduceOptions options)
{
    typedef typename Sequence::const_iterator Iterator;
    typedef ReduceKernel<ReduceFunctor, ResultType, IntermediateType> Reducer;
    typedef MappedReducedKernel<ResultType, Iterator, MapFunctor, ReduceFunctor, Reducer> MappedReduceType;
    typedef SequenceHolder2<Sequence, MappedReduceType, MapFunctor, ReduceFunctor> SequenceHolderType;
    return startThreadEngine(new SequenceHolderType(sequence, mapFunctor, reduceFunctor, options));
}

template <typename IntermediateType, typename ResultType, typename Iterator, typename MapFunctor, typename ReduceFunctor>
inline ThreadEngineStarter<ResultType> startMappedReduced(Iterator begin, Iterator end,
                                                           MapFunctor mapFunctor, ReduceFunctor reduceFunctor,
                                                           ReduceOptions options)
{
    typedef ReduceKernel<ReduceFunctor, ResultType, IntermediateType> Reducer;
    typedef MappedReducedKernel<ResultType, Iterator, MapFunctor, ReduceFunctor, Reducer> MappedReduceType;
    return startThreadEngine(new MappedReduceType(begin, end, mapFunctor, reduceFunctor, options));
}

} // namespace QtConcurrent

#endif //qdoc

QT_END_NAMESPACE
QT_END_HEADER

#endif // QT_NO_CONCURRENT

#endif
