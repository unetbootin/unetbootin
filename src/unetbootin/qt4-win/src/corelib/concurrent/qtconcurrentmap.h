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

#ifndef QTCONCURRENT_MAP_H
#define QTCONCURRENT_MAP_H

#include <QtCore/qglobal.h>

#ifndef QT_NO_CONCURRENT

#include <QtCore/qtconcurrentmapkernel.h>
#include <QtCore/qtconcurrentreducekernel.h>
#include <QtCore/qtconcurrentfunctionwrappers.h>
#include <QtCore/qstringlist.h>

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

QT_MODULE(Core)

#ifdef qdoc

namespace QtConcurrent {

    QFuture<void> map(Sequence &sequence, MapFunction function);
    QFuture<void> map(Iterator begin, Iterator end, MapFunction function);

    template <typename T>
    QFuture<T> mapped(const Sequence &sequence, MapFunction function);
    template <typename T>
    QFuture<T> mapped(ConstIterator begin, ConstIterator end, MapFunction function);

    template <typename T>
    QFuture<T> mappedReduced(const Sequence &sequence,
                             MapFunction function,
                             ReduceFunction function,
                             QtConcurrent::ReduceOptions options = UnorderedReduce | SequentialReduce);
    template <typename T>
    QFuture<T> mappedReduced(ConstIterator begin,
                             ConstIterator end,
                             MapFunction function,
                             ReduceFunction function,
                             QtConcurrent::ReduceOptions options = UnorderedReduce | SequentialReduce);

    void blockingMap(Sequence &sequence, MapFunction function);
    void blockingMap(Iterator begin, Iterator end, MapFunction function);

    template <typename T>
    T blockingMapped(const Sequence &sequence, MapFunction function);
    template <typename T>
    T blockingMapped(ConstIterator begin, ConstIterator end, MapFunction function);

    template <typename T>
    T blockingMappedReduced(const Sequence &sequence,
                            MapFunction function,
                            ReduceFunction function,
                            QtConcurrent::ReduceOptions options = UnorderedReduce | SequentialReduce);
    template <typename T>
    T blockingMappedReduced(ConstIterator begin,
                            ConstIterator end,
                            MapFunction function,
                            ReduceFunction function,
                            QtConcurrent::ReduceOptions options = UnorderedReduce | SequentialReduce);

} // namespace QtConcurrent

#else

namespace QtConcurrent {

// map() on sequences
template <typename Sequence, typename MapFunctor>
QFuture<void> map(Sequence &sequence, MapFunctor map)
{
    return startMap(sequence.begin(), sequence.end(), map);
}

template <typename Sequence, typename T, typename U>
QFuture<void> map(Sequence &sequence, T (map)(U))
{
    return startMap(sequence.begin(), sequence.end(), FunctionWrapper1<T, U>(map));
}

template <typename Sequence, typename T, typename C>
QFuture<void> map(Sequence &sequence, T (C::*map)())
{
    return startMap(sequence.begin(), sequence.end(), MemberFunctionWrapper<T, C>(map));
}

// map() on iterators
template <typename Iterator, typename MapFunctor>
QFuture<void> map(Iterator begin, Iterator end, MapFunctor map)
{
    return startMap(begin, end, map);
}

template <typename Iterator, typename T, typename U>
QFuture<void> map(Iterator begin, Iterator end, T (map)(U))
{
    return startMap(begin, end, FunctionWrapper1<T, U>(map));
}

template <typename Iterator, typename T, typename C>
QFuture<void> map(Iterator begin, Iterator end, T (C::*map)())
{
    return startMap(begin, end, MemberFunctionWrapper<T, C>(map));
}

// mappedReduced() for sequences.
template <typename ResultType, typename Sequence, typename MapFunctor, typename ReduceFunctor>
QFuture<ResultType> mappedReduced(const Sequence &sequence,
                                  MapFunctor map,
                                  ReduceFunctor reduce,
                                  ReduceOptions options = ReduceOptions(UnorderedReduce | SequentialReduce))
{
    return startMappedReduced<typename MapFunctor::result_type, ResultType>
    (sequence, map, reduce, options);
}

template <typename Sequence, typename MapFunctor, typename T, typename U, typename V>
QFuture<U> mappedReduced(const Sequence &sequence,
                         MapFunctor map,
                         T (reduce)(U &, V),
                         ReduceOptions options = ReduceOptions(UnorderedReduce | SequentialReduce))
{
    return startMappedReduced<typename MapFunctor::result_type, U>
    (sequence, map, FunctionWrapper2<T, U &, V>(reduce), options);
}

template <typename Sequence, typename MapFunctor, typename T, typename C, typename U>
QFuture<C> mappedReduced(const Sequence &sequence,
                         MapFunctor map,
                         T (C::*reduce)(U),
                         ReduceOptions options = ReduceOptions(UnorderedReduce | SequentialReduce))
{
    return startMappedReduced<typename MapFunctor::result_type, C>
    (sequence, map, MemberFunctionWrapper1<T, C, U>(reduce), options);
}

template <typename ResultType, typename Sequence, typename T, typename U, typename ReduceFunctor>
QFuture<ResultType> mappedReduced(const Sequence &sequence,
                                  T (map)(U),
                                  ReduceFunctor reduce,
                                  ReduceOptions options = ReduceOptions(UnorderedReduce | SequentialReduce))
{
    return startMappedReduced<T, ResultType>
    (sequence, FunctionWrapper1<T, U>(map), reduce, options);
}

template <typename ResultType, typename Sequence, typename T, typename C, typename ReduceFunctor>
QFuture<ResultType> mappedReduced(const Sequence &sequence,
                                  T (C::*map)() const,
                                  ReduceFunctor reduce,
                                  ReduceOptions options = ReduceOptions(UnorderedReduce | SequentialReduce))
{
    return startMappedReduced<T, ResultType>
    (sequence, ConstMemberFunctionWrapper<T, C>(map), reduce, options);
}

template <typename Sequence, typename T, typename U, typename V, typename W, typename X>
QFuture<W> mappedReduced(const Sequence &sequence,
                         T (map)(U),
                         V (reduce)(W &, X),
                         ReduceOptions options = ReduceOptions(UnorderedReduce | SequentialReduce))
{
    return startMappedReduced<T, W>
    (sequence, FunctionWrapper1<T, U>(map), FunctionWrapper2<V, W &, X>(reduce), options);
}

template <typename Sequence, typename T, typename C, typename U, typename V, typename W>
QFuture<V> mappedReduced(const Sequence &sequence,
                         T (C::*map)() const,
                         U (reduce)(V &, W),
                         ReduceOptions options = ReduceOptions(UnorderedReduce | SequentialReduce))
{
    return startMappedReduced<T, V> (sequence, ConstMemberFunctionWrapper<T, C>(map),
                                    FunctionWrapper2<U, V &, W>(reduce), options);
}

template <typename Sequence, typename T, typename U, typename V, typename C, typename W>
QFuture<C> mappedReduced(const Sequence &sequence,
                         T (map)(U),
                         V (C::*reduce)(W),
                         ReduceOptions options = ReduceOptions(UnorderedReduce | SequentialReduce))
{
    return startMappedReduced<T, C> (sequence, FunctionWrapper1<T, U>(map),
                                     MemberFunctionWrapper1<V, C, W>(reduce), options);
}

template <typename Sequence, typename T, typename C, typename U,typename D, typename V>
QFuture<D> mappedReduced(const Sequence &sequence,
                         T (C::*map)() const,
                         U (D::*reduce)(V),
                         ReduceOptions options = ReduceOptions(UnorderedReduce | SequentialReduce))
{
    return startMappedReduced<T, D>(sequence, ConstMemberFunctionWrapper<T, C>(map),
                                    MemberFunctionWrapper1<U, D, V>(reduce), options);
}

// mappedReduced() for iterators
template <typename ResultType, typename Iterator, typename MapFunctor, typename ReduceFunctor>
QFuture<ResultType> mappedReduced(Iterator begin,
                                  Iterator end,
                                  MapFunctor map,
                                  ReduceFunctor reduce,
                                  ReduceOptions options = ReduceOptions(UnorderedReduce | SequentialReduce))
{
    return startMappedReduced<ResultType, typename MapFunctor::result_type>
    (begin, end, map, reduce, options);
}

template <typename Iterator, typename MapFunctor, typename T, typename U, typename V>
QFuture<U> mappedReduced(Iterator begin,
                         Iterator end,
                         MapFunctor map,
                         T (reduce)(U &, V),
                         ReduceOptions options = ReduceOptions(UnorderedReduce | SequentialReduce))
{
    return startMappedReduced<typename MapFunctor::result_type, U>
    (begin, end, map, FunctionWrapper2<T, U &, V>(reduce), options);
}

template <typename Iterator, typename MapFunctor, typename T, typename C, typename U>
QFuture<C> mappedReduced(Iterator begin,
                         Iterator end,
                         MapFunctor map,
                         T (C::*reduce)(U),
                         ReduceOptions options = ReduceOptions(UnorderedReduce | SequentialReduce))
{
    return startMappedReduced<typename MapFunctor::result_type, C>
    (begin, end, map, MemberFunctionWrapper1<T, C, U>(reduce), options);
}

template <typename ResultType, typename Iterator, typename T, typename U, typename ReduceFunctor>
QFuture<ResultType> mappedReduced(Iterator begin,
                                  Iterator end,
                                  T (map)(U),
                                  ReduceFunctor reduce,
                                  ReduceOptions options = ReduceOptions(UnorderedReduce | SequentialReduce))
{
    return startMappedReduced<T, ResultType>
    (begin, end, FunctionWrapper1<T, U>(map), reduce, options);
}

template <typename ResultType, typename Iterator, typename T, typename C, typename ReduceFunctor>
QFuture<ResultType> mappedReduced(Iterator begin,
                                  Iterator end,
                                  T (C::*map)() const,
                                  ReduceFunctor reduce,
                                  ReduceOptions options = ReduceOptions(UnorderedReduce | SequentialReduce))
{
    return startMappedReduced<T, ResultType>
    (begin, end, ConstMemberFunctionWrapper<T, C>(map), reduce, options);
}

template <typename Iterator, typename T, typename U, typename V, typename W, typename X>
QFuture<W> mappedReduced(Iterator begin,
                         Iterator end,
                         T (map)(U),
                         V (reduce)(W &, X),
                         ReduceOptions options = ReduceOptions(UnorderedReduce | SequentialReduce))
{
    return startMappedReduced<T, W>
    (begin, end, FunctionWrapper1<T, U>(map), FunctionWrapper2<V, W &, X>(reduce), options);
}

template <typename Iterator, typename T, typename C, typename U, typename V, typename W>
QFuture<V> mappedReduced(Iterator begin,
                         Iterator end,
                         T (C::*map)() const,
                         U (reduce)(V &, W),
                         ReduceOptions options = ReduceOptions(UnorderedReduce | SequentialReduce))
{
    return startMappedReduced<T, V>(begin, end, ConstMemberFunctionWrapper<T, C>(map),
                                   FunctionWrapper2<U, V &, W>(reduce), options);
}

template <typename Iterator, typename T, typename U, typename V, typename C, typename W>
QFuture<C> mappedReduced(Iterator begin,
                         Iterator end,
                         T (map)(U),
                         V (C::*reduce)(W),
                         ReduceOptions options = ReduceOptions(UnorderedReduce | SequentialReduce))
{
    return startMappedReduced<T, C>
    (begin, end, FunctionWrapper1<T, U>(map), MemberFunctionWrapper1<V, C, W>(reduce), options);
}

template <typename Iterator, typename T, typename C, typename U,typename D, typename V>
QFuture<D> mappedReduced(Iterator begin,
                         Iterator end,
                         T (C::*map)() const,
                         U (D::*reduce)(V),
                         ReduceOptions options = ReduceOptions(UnorderedReduce | SequentialReduce))
{
    return startMappedReduced<T, D>(begin, end, ConstMemberFunctionWrapper<T, C>(map),
                                    MemberFunctionWrapper1<U, D, V>(reduce), options);
}

// mapped() for sequences
template <typename Sequence, typename MapFunctor>
QFuture<typename MapFunctor::result_type> mapped(const Sequence &sequence, MapFunctor map)
{
    return startMapped<typename MapFunctor::result_type>(sequence, map);
}

template <typename Sequence, typename T, typename U>
QFuture<T> mapped(const Sequence &sequence, T (map)(U))
{
   return startMapped<T>(sequence, FunctionWrapper1<T, U>(map));
}

template <typename Sequence, typename T, typename C>
QFuture<T> mapped(const Sequence &sequence, T (C::*map)() const)
{
    return startMapped<T>(sequence, ConstMemberFunctionWrapper<T, C>(map));
}

// mapped() for iterator ranges.
template <typename Iterator, typename MapFunctor>
QFuture<typename MapFunctor::result_type> mapped(Iterator begin, Iterator end, MapFunctor map)
{
    return startMapped<Q_TYPENAME MapFunctor::result_type>(begin, end, map);
}

template <typename Iterator, typename T, typename U>
QFuture<T> mapped(Iterator begin, Iterator end, T (map)(U))
{
    return startMapped<T>(begin, end, FunctionWrapper1<T, U>(map));
}

template <typename Iterator, typename T, typename C>
QFuture<T> mapped(Iterator begin, Iterator end, T (C::*map)() const)
{
    return startMapped<T>(begin, end, ConstMemberFunctionWrapper<T, C>(map));
}


template <typename Sequence, typename MapFunctor>
void blockingMap(Sequence &sequence, MapFunctor map)
{
    startMap(sequence.begin(), sequence.end(), map).startBlocking();
}

template <typename Sequence, typename T, typename U>
void blockingMap(Sequence &sequence, T (map)(U))
{
    startMap(sequence.begin(), sequence.end(), QtConcurrent::FunctionWrapper1<T, U>(map)).startBlocking();
}

template <typename Sequence, typename T, typename C>
void blockingMap(Sequence &sequence, T (C::*map)())
{
    startMap(sequence.begin(), sequence.end(), QtConcurrent::MemberFunctionWrapper<T, C>(map)).startBlocking();
}

template <typename Iterator, typename MapFunctor>
void blockingMap(Iterator begin, Iterator end, MapFunctor map)
{
    startMap(begin, end, map).startBlocking();
}

template <typename Iterator, typename T, typename U>
void blockingMap(Iterator begin, Iterator end, T (map)(U))
{
    startMap(begin, end, QtConcurrent::FunctionWrapper1<T, U>(map)).startBlocking();
}

template <typename Iterator, typename T, typename C>
void blockingMap(Iterator begin, Iterator end, T (C::*map)())
{
    startMap(begin, end, QtConcurrent::MemberFunctionWrapper<T, C>(map)).startBlocking();
}

template <typename ResultType, typename Sequence, typename MapFunctor, typename ReduceFunctor>
ResultType blockingMappedReduced(const Sequence &sequence,
                                 MapFunctor map,
                                 ReduceFunctor reduce,
                                 ReduceOptions options = ReduceOptions(UnorderedReduce | SequentialReduce))
{
    return QtConcurrent::startMappedReduced<typename MapFunctor::result_type, ResultType>
    (sequence, map, reduce, options).startBlocking();
}

template <typename Sequence, typename MapFunctor, typename T, typename U, typename V>
U blockingMappedReduced(const Sequence &sequence,
                        MapFunctor map,
                        T (reduce)(U &, V),
                        QtConcurrent::ReduceOptions options = QtConcurrent::ReduceOptions(QtConcurrent::UnorderedReduce | QtConcurrent::SequentialReduce))
{
    return QtConcurrent::startMappedReduced<typename MapFunctor::result_type, U>
        (sequence,
         map,
         QtConcurrent::FunctionWrapper2<T, U &, V>(reduce),
         options)
        .startBlocking();
}

template <typename Sequence, typename MapFunctor, typename T, typename C, typename U>
C blockingMappedReduced(const Sequence &sequence,
                        MapFunctor map,
                        T (C::*reduce)(U),
                        QtConcurrent::ReduceOptions options = QtConcurrent::ReduceOptions(QtConcurrent::UnorderedReduce | QtConcurrent::SequentialReduce))
{
    return QtConcurrent::startMappedReduced<typename MapFunctor::result_type, C>
        (sequence,
         map,
         QtConcurrent::MemberFunctionWrapper1<T, C, U>(reduce),
         options)
        .startBlocking();
}

template <typename ResultType, typename Sequence, typename T, typename U, typename ReduceFunctor>
ResultType blockingMappedReduced(const Sequence &sequence,
                                 T (map)(U),
                                 ReduceFunctor reduce,
                                 QtConcurrent::ReduceOptions options = QtConcurrent::ReduceOptions(QtConcurrent::UnorderedReduce | QtConcurrent::SequentialReduce))
{
    return QtConcurrent::startMappedReduced<T, ResultType>
        (sequence,
         QtConcurrent::FunctionWrapper1<T, U>(map),
         reduce,
         options)
        .startBlocking();
}

template <typename ResultType, typename Sequence, typename T, typename C, typename ReduceFunctor>
ResultType blockingMappedReduced(const Sequence &sequence,
                                 T (C::*map)() const,
                                 ReduceFunctor reduce,
                                 QtConcurrent::ReduceOptions options = QtConcurrent::ReduceOptions(QtConcurrent::UnorderedReduce | QtConcurrent::SequentialReduce))
{
    return QtConcurrent::startMappedReduced<T, ResultType>
        (sequence,
         QtConcurrent::ConstMemberFunctionWrapper<T, C>(map),
         reduce,
         options)
        .startBlocking();
}

template <typename Sequence, typename T, typename U, typename V, typename W, typename X>
W blockingMappedReduced(const Sequence &sequence,
                        T (map)(U),
                        V (reduce)(W &, X),
                        QtConcurrent::ReduceOptions options = QtConcurrent::ReduceOptions(QtConcurrent::UnorderedReduce | QtConcurrent::SequentialReduce))
{
    return QtConcurrent::startMappedReduced<T, W>
        (sequence,
         QtConcurrent::FunctionWrapper1<T, U>(map),
         QtConcurrent::FunctionWrapper2<V, W &, X>(reduce),
         options)
        .startBlocking();
}

template <typename Sequence, typename T, typename C, typename U, typename V, typename W>
V blockingMappedReduced(const Sequence &sequence,
                        T (C::*map)() const,
                        U (reduce)(V &, W),
                        QtConcurrent::ReduceOptions options = QtConcurrent::ReduceOptions(QtConcurrent::UnorderedReduce | QtConcurrent::SequentialReduce))
{
    return QtConcurrent::startMappedReduced<T, V>
        (sequence,
         QtConcurrent::ConstMemberFunctionWrapper<T, C>(map),
         QtConcurrent::FunctionWrapper2<U, V &, W>(reduce),
         options)
        .startBlocking();
}

template <typename Sequence, typename T, typename U, typename V, typename C, typename W>
C blockingMappedReduced(const Sequence &sequence,
                        T (map)(U),
                        V (C::*reduce)(W),
                        QtConcurrent::ReduceOptions options = QtConcurrent::ReduceOptions(QtConcurrent::UnorderedReduce | QtConcurrent::SequentialReduce))
{
    return QtConcurrent::startMappedReduced<T, C>
        (sequence,
         QtConcurrent::FunctionWrapper1<T, U>(map),
         QtConcurrent::MemberFunctionWrapper1<V, C, W>(reduce),
         options)
        .startBlocking();
}

template <typename Sequence, typename T, typename C, typename U,typename D, typename V>
D blockingMappedReduced(const Sequence &sequence,
                        T (C::*map)() const,
                        U (D::*reduce)(V),
                        QtConcurrent::ReduceOptions options = QtConcurrent::ReduceOptions(QtConcurrent::UnorderedReduce | QtConcurrent::SequentialReduce))
{
    return QtConcurrent::startMappedReduced<T, D>
        (sequence,
         QtConcurrent::ConstMemberFunctionWrapper<T, C>(map),
         QtConcurrent::MemberFunctionWrapper1<U, D, V>(reduce),
         options)
        .startBlocking();
}

template <typename ResultType, typename Iterator, typename MapFunctor, typename ReduceFunctor>
ResultType blockingMappedReduced(Iterator begin,
                                 Iterator end,
                                 MapFunctor map,
                                 ReduceFunctor reduce,
                                 QtConcurrent::ReduceOptions options = QtConcurrent::ReduceOptions(QtConcurrent::UnorderedReduce | QtConcurrent::SequentialReduce))
{
    return QtConcurrent::startMappedReduced<typename MapFunctor::result_type, ResultType>
        (begin, end, map, reduce, options).startBlocking();
}

template <typename Iterator, typename MapFunctor, typename T, typename U, typename V>
U blockingMappedReduced(Iterator begin,
                        Iterator end,
                        MapFunctor map,
                        T (reduce)(U &, V),
                        QtConcurrent::ReduceOptions options = QtConcurrent::ReduceOptions(QtConcurrent::UnorderedReduce | QtConcurrent::SequentialReduce))
{
    return QtConcurrent::startMappedReduced<typename MapFunctor::result_type, U>
        (begin,
         end,
         map,
         QtConcurrent::FunctionWrapper2<T, U &, V>(reduce),
         options)
        .startBlocking();
}

template <typename Iterator, typename MapFunctor, typename T, typename C, typename U>
C blockingMappedReduced(Iterator begin,
                        Iterator end,
                        MapFunctor map,
                        T (C::*reduce)(U),
                        QtConcurrent::ReduceOptions options = QtConcurrent::ReduceOptions(QtConcurrent::UnorderedReduce | QtConcurrent::SequentialReduce))
{
    return QtConcurrent::startMappedReduced<typename MapFunctor::result_type, C>
        (begin,
         end,
         map,
         QtConcurrent::MemberFunctionWrapper1<T, C, U>(reduce),
         options)
        .startBlocking();
}

template <typename ResultType, typename Iterator, typename T, typename U, typename ReduceFunctor>
ResultType blockingMappedReduced(Iterator begin,
                                 Iterator end,
                                 T (map)(U),
                                 ReduceFunctor reduce,
                                 QtConcurrent::ReduceOptions options = QtConcurrent::ReduceOptions(QtConcurrent::UnorderedReduce | QtConcurrent::SequentialReduce))
{
    return QtConcurrent::startMappedReduced<T, ResultType>
        (begin,
         end,
         QtConcurrent::FunctionWrapper1<T, U>(map),
         reduce,
         options)
        .startBlocking();
}

template <typename ResultType, typename Iterator, typename T, typename C, typename ReduceFunctor>
ResultType blockingMappedReduced(Iterator begin,
                                 Iterator end,
                                 T (C::*map)() const,
                                 ReduceFunctor reduce,
                                 QtConcurrent::ReduceOptions options = QtConcurrent::ReduceOptions(QtConcurrent::UnorderedReduce | QtConcurrent::SequentialReduce))
{
    return QtConcurrent::startMappedReduced<T, ResultType>
        (begin,
         end,
         QtConcurrent::ConstMemberFunctionWrapper<T, C>(map),
         reduce,
         options)
        .startBlocking();
}

template <typename Iterator, typename T, typename U, typename V, typename W, typename X>
W blockingMappedReduced(Iterator begin,
                        Iterator end,
                        T (map)(U),
                        V (reduce)(W &, X),
                        QtConcurrent::ReduceOptions options = QtConcurrent::ReduceOptions(QtConcurrent::UnorderedReduce | QtConcurrent::SequentialReduce))
{
    return QtConcurrent::startMappedReduced<T, W>
        (begin,
         end,
         QtConcurrent::FunctionWrapper1<T, U>(map),
         QtConcurrent::FunctionWrapper2<V, W &, X>(reduce),
         options)
        .startBlocking();
}

template <typename Iterator, typename T, typename C, typename U, typename V, typename W>
V blockingMappedReduced(Iterator begin,
                        Iterator end,
                        T (C::*map)() const,
                        U (reduce)(V &, W),
                        QtConcurrent::ReduceOptions options = QtConcurrent::ReduceOptions(QtConcurrent::UnorderedReduce | QtConcurrent::SequentialReduce))
{
    return QtConcurrent::startMappedReduced<T, V>
        (begin,
         end,
         QtConcurrent::ConstMemberFunctionWrapper<T, C>(map),
         QtConcurrent::FunctionWrapper2<U, V &, W>(reduce),
         options)
        .startBlocking();
}

template <typename Iterator, typename T, typename U, typename V, typename C, typename W>
C blockingMappedReduced(Iterator begin,
                        Iterator end,
                        T (map)(U),
                        V (C::*reduce)(W),
                        QtConcurrent::ReduceOptions options = QtConcurrent::ReduceOptions(QtConcurrent::UnorderedReduce | QtConcurrent::SequentialReduce))
{
    return QtConcurrent::startMappedReduced<T, C>
        (begin,
         end,
         QtConcurrent::FunctionWrapper1<T, U>(map),
         QtConcurrent::MemberFunctionWrapper1<V, C, W>(reduce),
         options)
        .startBlocking();
}

template <typename Iterator, typename T, typename C, typename U,typename D, typename V>
D blockingMappedReduced(Iterator begin,
                        Iterator end,
                        T (C::*map)() const,
                        U (D::*reduce)(V),
                        QtConcurrent::ReduceOptions options = QtConcurrent::ReduceOptions(QtConcurrent::UnorderedReduce | QtConcurrent::SequentialReduce))
{
    return QtConcurrent::startMappedReduced<T, D>
        (begin,
         end,
         QtConcurrent::ConstMemberFunctionWrapper<T, C>(map),
         QtConcurrent::MemberFunctionWrapper1<U, D, V>(reduce),
         options)
        .startBlocking();
}

// mapped() for sequences
template <typename OutputSequence, typename InputSequence, typename MapFunctor>
OutputSequence blockingMapped(const InputSequence &sequence, MapFunctor map)
{
    return blockingMappedReduced(sequence, map, &OutputSequence::append,
                                 QtConcurrent::OrderedReduce);
}

template <typename OutputSequence, typename InputSequence, typename T, typename U>
OutputSequence blockingMapped(const InputSequence &sequence, T (map)(U))
{
    return blockingMappedReduced(sequence, map, &OutputSequence::append,
                                 QtConcurrent::OrderedReduce);
}

template <typename OutputSequence, typename InputSequence, typename T, typename C>
OutputSequence blockingMapped(const InputSequence &sequence, T (C::*map)() const)
{
    return blockingMappedReduced(sequence, map, &OutputSequence::append,
                                 QtConcurrent::OrderedReduce);
}

#ifndef QT_NO_TEMPLATE_TEMPLATE_PARAMETERS

// overloads for changing the container value type:
template <template <typename> class Sequence, typename MapFunctor, typename T>
Sequence<typename MapFunctor::result_type> blockingMapped(const Sequence<T> &sequence, MapFunctor map)
{
    typedef Sequence<typename MapFunctor::result_type> OutputSequence;
    return blockingMappedReduced(sequence, map, &OutputSequence::append,
                                               QtConcurrent::OrderedReduce);
}

template <template <typename> class Sequence, typename T, typename U, typename V>
Sequence<U> blockingMapped(const Sequence<T> &sequence, U (map)(V))
{
    typedef Sequence<U> OutputSequence;
    return blockingMappedReduced(sequence, map, &OutputSequence::append,
                                               QtConcurrent::OrderedReduce);
}

template <template <typename> class Sequence, typename T, typename U, typename C>
Sequence<U> blockingMapped(const Sequence<T> &sequence, U (C::*map)() const)
{
    typedef Sequence<U> OutputSequence;
    return blockingMappedReduced(sequence, map, &OutputSequence::append,
                                               QtConcurrent::OrderedReduce);
}

#endif // QT_NO_TEMPLATE_TEMPLATE_PARAMETER

// overloads for changing the container value type from a QStringList:
template <typename MapFunctor>
QList<typename MapFunctor::result_type> blockingMapped(const QStringList &sequence, MapFunctor map)
{
    typedef QList<typename MapFunctor::result_type> OutputSequence;
    return blockingMappedReduced(sequence, map, &OutputSequence::append,
                                 QtConcurrent::OrderedReduce);
}

template <typename U, typename V>
QList<U> blockingMapped(const QStringList &sequence, U (map)(V))
{
    typedef QList<U> OutputSequence;
    return blockingMappedReduced(sequence, map, &OutputSequence::append,
                                 QtConcurrent::OrderedReduce);
}

template <typename U, typename C>
QList<U> blockingMapped(const QStringList &sequence, U (C::*map)() const)
{
    typedef QList<U> OutputSequence;
    return blockingMappedReduced(sequence, map, &OutputSequence::append,
                                 QtConcurrent::OrderedReduce);
}

// mapped()  for iterator ranges
template <typename Sequence, typename Iterator, typename MapFunctor>
Sequence blockingMapped(Iterator begin, Iterator end, MapFunctor map)
{
    return blockingMappedReduced(begin, end, map, &Sequence::append,
                                 QtConcurrent::OrderedReduce);
}

template <typename Sequence, typename Iterator, typename T, typename U>
Sequence blockingMapped(Iterator begin, Iterator end, T (map)(U))
{
    return blockingMappedReduced(begin, end, map, &Sequence::append,
                                 QtConcurrent::OrderedReduce);
}

template <typename Sequence, typename Iterator, typename T, typename C>
Sequence blockingMapped(Iterator begin, Iterator end, T (C::*map)() const)
{
    return blockingMappedReduced(begin, end, map, &Sequence::append,
                                 QtConcurrent::OrderedReduce);
}

} // namespace QtConcurrent

#endif // qdoc

QT_END_NAMESPACE
QT_END_HEADER

#endif // QT_NO_CONCURRENT

#endif
