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

#ifndef QTCONCURRENT_FILTER_H
#define QTCONCURRENT_FILTER_H

#include <QtCore/qglobal.h>

#ifndef QT_NO_CONCURRENT

#include <QtCore/qtconcurrentfilterkernel.h>
#include <QtCore/qtconcurrentfunctionwrappers.h>

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

QT_MODULE(Core)

#ifdef qdoc

namespace QtConcurrent {

    QFuture<void> filter(Sequence &sequence, FilterFunction filterFunction);

    template <typename T>
    QFuture<T> filtered(const Sequence &sequence, FilterFunction filterFunction);
    template <typename T>
    QFuture<T> filtered(ConstIterator begin, ConstIterator end, FilterFunction filterFunction);

    template <typename T>
    QFuture<T> filteredReduced(const Sequence &sequence,
                               FilterFunction filterFunction,
                               ReduceFunction reduceFunction,
                               QtConcurrent::ReduceOptions reduceOptions = UnorderedReduce | SequentialReduce);
    template <typename T>
    QFuture<T> filteredReduced(ConstIterator begin,
                               ConstIterator end,
                               FilterFunction filterFunction,
                               ReduceFunction reduceFunction,
                               QtConcurrent::ReduceOptions reduceOptions = UnorderedReduce | SequentialReduce);

    void blockingFilter(Sequence &sequence, FilterFunction filterFunction);

    template <typename Sequence>
    Sequence blockingFiltered(const Sequence &sequence, FilterFunction filterFunction);
    template <typename Sequence>
    Sequence blockingFiltered(ConstIterator begin, ConstIterator end, FilterFunction filterFunction);

    template <typename T>
    T blockingFilteredReduced(const Sequence &sequence,
                              FilterFunction filterFunction,
                              ReduceFunction reduceFunction,
                              QtConcurrent::ReduceOptions reduceOptions = UnorderedReduce | SequentialReduce);
    template <typename T>
    T blockingFilteredReduced(ConstIterator begin,
                              ConstIterator end,
                              FilterFunction filterFunction,
                              ReduceFunction reduceFunction,
                              QtConcurrent::ReduceOptions reduceOptions = UnorderedReduce | SequentialReduce);

} // namespace QtConcurrent

#else

namespace QtConcurrent {

template <typename Sequence, typename KeepFunctor, typename T, typename C, typename U>
ThreadEngineStarter<void> filterInternal(Sequence &sequence, KeepFunctor keep, T (C::*reduce)(U))
{
    typedef MemberFunctionWrapper1<T, C, U> ReduceFunctor;
    typedef typename Sequence::const_iterator Iterator;
    typedef FilterKernel<Sequence, KeepFunctor, ReduceFunctor> KernelType;
    return startThreadEngine(new KernelType(sequence, keep, reduce));
}

// filter() on sequences
template <typename Sequence, typename KeepFunctor>
QFuture<void> filter(Sequence &sequence, KeepFunctor keep)
{
    return filterInternal(sequence, keep, &Sequence::append);
}

template <typename Sequence, typename T>
QFuture<void> filter(Sequence &sequence, bool (keep)(T))
{
    return filterInternal(sequence, FunctionWrapper1<bool, T>(keep), &Sequence::append);
}

template <typename Sequence, typename C>
QFuture<void> filter(Sequence &sequence, bool (C::*keep)() const)
{
    return filterInternal(sequence, ConstMemberFunctionWrapper<bool, C>(keep), &Sequence::append);
}

// filteredReduced() on sequences
template <typename ResultType, typename Sequence, typename KeepFunctor, typename ReduceFunctor>
QFuture<ResultType> filteredReduced(const Sequence &sequence,
                                    KeepFunctor keep,
                                    ReduceFunctor reduce,
                                    ReduceOptions options = ReduceOptions(UnorderedReduce | SequentialReduce))
{
    return startFilteredReduced<ResultType>(sequence, keep, reduce, options);
 }

template <typename ResultType, typename Sequence, typename T, typename ReduceFunctor>
QFuture<ResultType> filteredReduced(const Sequence &sequence,
                                    bool (filter)(T),
                                    ReduceFunctor reduce,
                                    ReduceOptions options = ReduceOptions(UnorderedReduce | SequentialReduce))
{
    return filteredReduced<ResultType>(sequence,
                                       FunctionWrapper1<bool, T>(filter),
                                       reduce,
                                       options);
}

template <typename ResultType, typename Sequence, typename C, typename ReduceFunctor>
QFuture<ResultType> filteredReduced(const Sequence &sequence,
                                    bool (C::*filter)() const,
                                    ReduceFunctor reduce,
                                    ReduceOptions options = ReduceOptions(UnorderedReduce | SequentialReduce))
{
    return filteredReduced<ResultType>(sequence,
                                       ConstMemberFunctionWrapper<bool, C>(filter),
                                       reduce,
                                       options);
}

template <typename Sequence, typename KeepFunctor, typename T, typename U, typename V>
QFuture<U> filteredReduced(const Sequence &sequence,
                           KeepFunctor keep,
                           T (reduce)(U &, V),
                           ReduceOptions options = ReduceOptions(UnorderedReduce | SequentialReduce))
{
    return filteredReduced<U>(sequence,
                              keep,
                              FunctionWrapper2<T, U &, V>(reduce),
                              options);
}

template <typename Sequence, typename KeepFunctor, typename T, typename C, typename U>
QFuture<C> filteredReduced(const Sequence &sequence,
                           KeepFunctor keep,
                           T (C::*reduce)(U),
                           ReduceOptions options = ReduceOptions(UnorderedReduce | SequentialReduce))
{
    return filteredReduced<C>(sequence,
                              keep,
                              MemberFunctionWrapper1<T, C, U>(reduce),
                              options);
}

template <typename Sequence, typename T, typename U, typename V, typename W>
QFuture<V> filteredReduced(const Sequence &sequence,
                           bool (keep)(T),
                           U (reduce)(V &, W),
                           ReduceOptions options = ReduceOptions(UnorderedReduce | SequentialReduce))
{
    return filteredReduced<V>(sequence,
                              FunctionWrapper1<bool, T>(keep),
                              FunctionWrapper2<U, V &, W>(reduce),
                              options);
}

template <typename Sequence, typename C, typename T, typename U, typename V>
QFuture<U> filteredReduced(const Sequence &sequence,
                           bool (C::*keep)() const,
                           T (reduce)(U &, V),
                           ReduceOptions options = ReduceOptions(UnorderedReduce | SequentialReduce))
{
    return filteredReduced<U>(sequence,
                              ConstMemberFunctionWrapper<bool, C>(keep),
                              FunctionWrapper2<T, U &, V>(reduce),
                              options);
}

template <typename Sequence, typename T, typename U, typename C, typename V>
QFuture<C> filteredReduced(const Sequence &sequence,
                           bool (keep)(T),
                           U (C::*reduce)(V),
                           ReduceOptions options = ReduceOptions(UnorderedReduce | SequentialReduce))
{
    return filteredReduced<C>(sequence,
                              FunctionWrapper1<bool, T>(keep),
                              MemberFunctionWrapper1<U, C, V>(reduce),
                              options);
}

template <typename Sequence, typename C, typename T, typename D, typename U>
QFuture<D> filteredReduced(const Sequence &sequence,
                           bool (C::*keep)() const,
                           T (D::*reduce)(U),
                           ReduceOptions options = ReduceOptions(UnorderedReduce | SequentialReduce))
{
    return filteredReduced<D>(sequence,
                              ConstMemberFunctionWrapper<bool, C>(keep),
                              MemberFunctionWrapper1<T, D, U>(reduce),
                              options);
}

// filteredReduced() on iterators
template <typename ResultType, typename Iterator, typename KeepFunctor, typename ReduceFunctor>
QFuture<ResultType> filteredReduced(Iterator begin,
                                    Iterator end,
                                    KeepFunctor keep,
                                    ReduceFunctor reduce,
                                    ReduceOptions options = ReduceOptions(UnorderedReduce | SequentialReduce))
{
   return startFilteredReduced<ResultType>(begin, end, keep, reduce, options);
}

template <typename ResultType, typename Iterator, typename T, typename ReduceFunctor>
QFuture<ResultType> filteredReduced(Iterator begin,
                                    Iterator end,
                                    bool (filter)(T),
                                    ReduceFunctor reduce,
                                    ReduceOptions options = ReduceOptions(UnorderedReduce | SequentialReduce))
{
    return filteredReduced<ResultType>(begin,
                                       end,
                                       FunctionWrapper1<bool, T>(filter),
                                       reduce,
                                       options);
}

template <typename ResultType, typename Iterator, typename C, typename ReduceFunctor>
QFuture<ResultType> filteredReduced(Iterator begin,
                                    Iterator end,
                                    bool (C::*filter)() const,
                                    ReduceFunctor reduce,
                                    ReduceOptions options = ReduceOptions(UnorderedReduce | SequentialReduce))
{
    return filteredReduced<ResultType>(begin,
                                       end,
                                       ConstMemberFunctionWrapper<bool, C>(filter),
                                       reduce,
                                       options);
}

template <typename Iterator, typename KeepFunctor, typename T, typename U, typename V>
QFuture<U> filteredReduced(Iterator begin,
                           Iterator end,
                           KeepFunctor keep,
                           T (reduce)(U &, V),
                           ReduceOptions options = ReduceOptions(UnorderedReduce | SequentialReduce))
{
    return filteredReduced<U>(begin,
                              end,
                              keep,
                              FunctionWrapper2<T, U &, V>(reduce),
                              options);
}

template <typename Iterator, typename KeepFunctor, typename T, typename C, typename U>
QFuture<C> filteredReduced(Iterator begin,
                           Iterator end,
                           KeepFunctor keep,
                           T (C::*reduce)(U),
                           ReduceOptions options = ReduceOptions(UnorderedReduce | SequentialReduce))
{
    return filteredReduced<C>(begin,
                              end,
                              keep,
                              MemberFunctionWrapper1<T, C, U>(reduce),
                              options);
}

template <typename Iterator, typename T, typename U, typename V, typename W>
QFuture<V> filteredReduced(Iterator begin,
                           Iterator end,
                           bool (keep)(T),
                           U (reduce)(V &, W),
                           ReduceOptions options = ReduceOptions(UnorderedReduce | SequentialReduce))
{
    return filteredReduced<V>(begin,
                              end,
                              FunctionWrapper1<bool, T>(keep),
                              FunctionWrapper2<U, V &, W>(reduce),
                              options);
}

template <typename Iterator, typename C, typename T, typename U, typename V>
QFuture<U> filteredReduced(Iterator begin,
                           Iterator end,
                           bool (C::*keep)() const,
                           T (reduce)(U &, V),
                           ReduceOptions options = ReduceOptions(UnorderedReduce | SequentialReduce))
{
    return filteredReduced<U>(begin,
                              end,
                              ConstMemberFunctionWrapper<bool, C>(keep),
                              FunctionWrapper2<T, U &, V>(reduce),
                              options);
}

template <typename Iterator, typename T, typename U, typename C, typename V>
QFuture<C> filteredReduced(Iterator begin,
                           Iterator end,
                           bool (keep)(T),
                           U (C::*reduce)(V),
                           ReduceOptions options = ReduceOptions(UnorderedReduce | SequentialReduce))
{
    return filteredReduced<C>(begin,
                              end,
                              FunctionWrapper1<bool, T>(keep),
                              MemberFunctionWrapper1<U, C, V>(reduce),
                              options);
}

template <typename Iterator, typename C, typename T, typename D, typename U>
QFuture<D> filteredReduced(Iterator begin,
                           Iterator end,
                           bool (C::*keep)() const,
                           T (D::*reduce)(U),
                           ReduceOptions options = ReduceOptions(UnorderedReduce | SequentialReduce))
{
    return filteredReduced<D>(begin,
                              end,
                              ConstMemberFunctionWrapper<bool, C>(keep),
                              MemberFunctionWrapper1<T, D, U>(reduce),
                              options);
}


// filtered() on sequences
template <typename Sequence, typename KeepFunctor>
QFuture<typename Sequence::value_type> filtered(const Sequence &sequence, KeepFunctor keep)
{
    return startFiltered(sequence, keep);
}

template <typename Sequence, typename T>
QFuture<T> filtered(const Sequence &sequence, bool (keep)(T))
{
    return startFiltered(sequence, FunctionWrapper1<bool, T>(keep));
}

template <typename Sequence, typename C>
QFuture<C> filtered(const Sequence &sequence, bool (C::*keep)() const)
{
    return startFiltered(sequence, ConstMemberFunctionWrapper<bool, C>(keep));
}

// filtered() on iterators
template <typename Iterator, typename KeepFunctor>
QFuture<typename Iterator::value_type> filtered(Iterator begin, Iterator end, KeepFunctor keep)
{
    return startFiltered(begin, end, keep);
}

template <typename Iterator, typename T>
QFuture<typename Iterator::value_type> filtered(Iterator begin, Iterator end, bool (keep)(T))
{
    return startFiltered(begin, end, FunctionWrapper1<bool, T>(keep));
}

template <typename Iterator, typename C>
QFuture<typename Iterator::value_type> filtered(Iterator begin,
                                                Iterator end,
                                                bool (C::*keep)() const)
{
    return startFiltered(begin, end, ConstMemberFunctionWrapper<bool, C>(keep));
}


// blocking filter() on sequences
template <typename Sequence, typename KeepFunctor>
void blockingFilter(Sequence &sequence, KeepFunctor keep)
{
    filterInternal(sequence, keep, &Sequence::append).startBlocking();
}

template <typename Sequence, typename T>
void blockingFilter(Sequence &sequence, bool (keep)(T))
{
    filterInternal(sequence, FunctionWrapper1<bool, T>(keep), &Sequence::append)
        .startBlocking();
}

template <typename Sequence, typename C>
void blockingFilter(Sequence &sequence, bool (C::*keep)() const)
{
    filterInternal(sequence,
                   ConstMemberFunctionWrapper<bool, C>(keep),
                   &Sequence::append)
        .startBlocking();
}

// blocking filteredReduced() on sequences
template <typename ResultType, typename Sequence, typename KeepFunctor, typename ReduceFunctor>
ResultType blockingFilteredReduced(const Sequence &sequence,
                                   KeepFunctor keep,
                                   ReduceFunctor reduce,
                                   ReduceOptions options = ReduceOptions(UnorderedReduce | SequentialReduce))
{
    return startFilteredReduced<ResultType>(sequence, keep, reduce, options)
        .startBlocking();
}

template <typename ResultType, typename Sequence, typename T, typename ReduceFunctor>
ResultType blockingFilteredReduced(const Sequence &sequence,
                                   bool (filter)(T),
                                   ReduceFunctor reduce,
                                   ReduceOptions options = ReduceOptions(UnorderedReduce | SequentialReduce))
{
    return blockingFilteredReduced<ResultType>
        (sequence,
         FunctionWrapper1<bool, T>(filter),
         reduce,
         options);
}

template <typename ResultType, typename Sequence, typename C, typename ReduceFunctor>
ResultType blockingFilteredReduced(const Sequence &sequence,
                                   bool (C::*filter)() const,
                                   ReduceFunctor reduce,
                                   ReduceOptions options = ReduceOptions(UnorderedReduce | SequentialReduce))
{
    return blockingFilteredReduced<ResultType>
        (sequence,
         ConstMemberFunctionWrapper<bool, C>(filter),
         reduce,
         options);
}

template <typename Sequence, typename KeepFunctor, typename T, typename U, typename V>
U blockingFilteredReduced(const Sequence &sequence,
                          KeepFunctor keep,
                          T (reduce)(U &, V),
                          ReduceOptions options = ReduceOptions(UnorderedReduce | SequentialReduce))
{
    return blockingFilteredReduced<U>
        (sequence,
         keep,
         FunctionWrapper2<T, U &, V>(reduce),
         options);
}

template <typename Sequence, typename KeepFunctor, typename T, typename C, typename U>
C blockingFilteredReduced(const Sequence &sequence,
                          KeepFunctor keep,
                          T (C::*reduce)(U),
                          ReduceOptions options = ReduceOptions(UnorderedReduce | SequentialReduce))
{
    return blockingFilteredReduced<C>
        (sequence,
         keep,
         MemberFunctionWrapper1<T, C, U>(reduce),
         options);
}

template <typename Sequence, typename T, typename U, typename V, typename W>
V blockingFilteredReduced(const Sequence &sequence,
                          bool (keep)(T),
                          U (reduce)(V &, W),
                          ReduceOptions options = ReduceOptions(UnorderedReduce | SequentialReduce))
{
    return blockingFilteredReduced<V>
        (sequence,
         FunctionWrapper1<bool, T>(keep),
         FunctionWrapper2<U, V &, W>(reduce),
         options);
}

template <typename Sequence, typename C, typename T, typename U, typename V>
U blockingFilteredReduced(const Sequence &sequence,
                          bool (C::*keep)() const,
                          T (reduce)(U &, V),
                          ReduceOptions options = ReduceOptions(UnorderedReduce | SequentialReduce))
{
    return blockingFilteredReduced<U>
        (sequence,
         ConstMemberFunctionWrapper<bool, C>(keep),
         FunctionWrapper2<T, U &, V>(reduce),
         options);
}

template <typename Sequence, typename T, typename U, typename C, typename V>
C blockingFilteredReduced(const Sequence &sequence,
                          bool (keep)(T),
                          U (C::*reduce)(V),
                          ReduceOptions options = ReduceOptions(UnorderedReduce | SequentialReduce))
{
    return blockingFilteredReduced<C>
        (sequence,
         FunctionWrapper1<bool, T>(keep),
         MemberFunctionWrapper1<U, C, V>(reduce),
         options);
}

template <typename Sequence, typename C, typename T, typename D, typename U>
D blockingFilteredReduced(const Sequence &sequence,
                          bool (C::*keep)() const,
                          T (D::*reduce)(U),
                          ReduceOptions options = ReduceOptions(UnorderedReduce | SequentialReduce))
{
    return blockingFilteredReduced<D>
        (sequence,
         ConstMemberFunctionWrapper<bool, C>(keep),
         MemberFunctionWrapper1<T, D, U>(reduce),
         options);
}

// blocking filteredReduced() on iterators
template <typename ResultType, typename Iterator, typename KeepFunctor, typename ReduceFunctor>
ResultType blockingFilteredReduced(Iterator begin,
                                   Iterator end,
                                   KeepFunctor keep,
                                   ReduceFunctor reduce,
                                   ReduceOptions options = ReduceOptions(UnorderedReduce | SequentialReduce))
{
    return startFilteredReduced<ResultType>(begin, end, keep, reduce, options)
        .startBlocking();
}

template <typename ResultType, typename Iterator, typename T, typename ReduceFunctor>
ResultType blockingFilteredReduced(Iterator begin,
                                   Iterator end,
                                   bool (filter)(T),
                                   ReduceFunctor reduce,
                                   ReduceOptions options = ReduceOptions(UnorderedReduce | SequentialReduce))
{
    return blockingFilteredReduced<ResultType>
        (begin,
         end,
         FunctionWrapper1<bool, T>(filter),
         reduce,
         options);
}

template <typename ResultType, typename Iterator, typename C, typename ReduceFunctor>
ResultType blockingFilteredReduced(Iterator begin,
                                   Iterator end,
                                   bool (C::*filter)() const,
                                   ReduceFunctor reduce,
                                   ReduceOptions options = ReduceOptions(UnorderedReduce | SequentialReduce))
{
    return blockingFilteredReduced<ResultType>
        (begin,
         end,
         ConstMemberFunctionWrapper<bool, C>(filter),
         reduce,
         options);
}

template <typename Iterator, typename KeepFunctor, typename T, typename U, typename V>
U blockingFilteredReduced(Iterator begin,
                          Iterator end,
                          KeepFunctor keep,
                          T (reduce)(U &, V),
                          ReduceOptions options = ReduceOptions(UnorderedReduce | SequentialReduce))
{
    return blockingFilteredReduced<U>
        (begin,
         end,
         keep,
         FunctionWrapper2<T, U &, V>(reduce),
         options);
}

template <typename Iterator, typename KeepFunctor, typename T, typename C, typename U>
C blockingFilteredReduced(Iterator begin,
                          Iterator end,
                          KeepFunctor keep,
                          T (C::*reduce)(U),
                          ReduceOptions options = ReduceOptions(UnorderedReduce | SequentialReduce))
{
    return blockingFilteredReduced<C>
        (begin,
         end,
         keep,
         MemberFunctionWrapper1<T, C, U>(reduce),
         options);
}

template <typename Iterator, typename T, typename U, typename V, typename W>
V blockingFilteredReduced(Iterator begin,
                          Iterator end,
                          bool (keep)(T),
                          U (reduce)(V &, W),
                          ReduceOptions options = ReduceOptions(UnorderedReduce | SequentialReduce))
{
    return blockingFilteredReduced<V>
        (begin,
         end,
         FunctionWrapper1<bool, T>(keep),
         FunctionWrapper2<U, V &, W>(reduce),
         options);
}

template <typename Iterator, typename C, typename T, typename U, typename V>
U blockingFilteredReduced(Iterator begin,
                          Iterator end,
                          bool (C::*keep)() const,
                          T (reduce)(U &, V),
                          ReduceOptions options = ReduceOptions(UnorderedReduce | SequentialReduce))
{
    return blockingFilteredReduced<U>
        (begin,
         end,
         ConstMemberFunctionWrapper<bool, C>(keep),
         FunctionWrapper2<T, U &, V>(reduce),
         options);
}

template <typename Iterator, typename T, typename U, typename C, typename V>
C blockingFilteredReduced(Iterator begin,
                          Iterator end,
                          bool (keep)(T),
                          U (C::*reduce)(V),
                          ReduceOptions options = ReduceOptions(UnorderedReduce | SequentialReduce))
{
    return blockingFilteredReduced<C>
        (begin,
         end,
         FunctionWrapper1<bool, T>(keep),
         MemberFunctionWrapper1<U, C, V>(reduce),
         options);
}

template <typename Iterator, typename C, typename T, typename D, typename U>
D blockingFilteredReduced(Iterator begin,
                          Iterator end,
                          bool (C::*keep)() const,
                          T (D::*reduce)(U),
                          ReduceOptions options = ReduceOptions(UnorderedReduce | SequentialReduce))
{
    return blockingFilteredReduced<D>
        (begin,
         end,
         ConstMemberFunctionWrapper<bool, C>(keep),
         MemberFunctionWrapper1<T, D, U>(reduce),
         options);
}

// blocking filtered() on sequences
template <typename Sequence, typename KeepFunctor>
Sequence blockingFiltered(const Sequence &sequence, KeepFunctor keep)
{
    return blockingFilteredReduced(sequence, keep, &Sequence::append, OrderedReduce);
}

template <typename Sequence, typename T>
Sequence blockingFiltered(const Sequence &sequence, bool (keep)(T))
{
    return blockingFilteredReduced(sequence, keep, &Sequence::append, OrderedReduce);
}

template <typename Sequence, typename C>
Sequence blockingFiltered(const Sequence &sequence, bool (C::*filter)() const)
{
    return blockingFilteredReduced(sequence,
                                   filter,
                                   &Sequence::append,
                                   OrderedReduce);
}

// blocking filtered() on iterators
template <typename OutputSequence, typename Iterator, typename KeepFunctor>
OutputSequence blockingFiltered(Iterator begin, Iterator end, KeepFunctor keep)
{
    return blockingFilteredReduced(begin,
                                   end,
                                   keep,
                                   &OutputSequence::append,
                                   OrderedReduce);
}

template <typename OutputSequence, typename Iterator, typename T>
OutputSequence blockingFiltered(Iterator begin, Iterator end, bool (keep)(T))
{
    return blockingFilteredReduced(begin,
                                   end,
                                   keep,
                                   &OutputSequence::append,
                                   OrderedReduce);
}

template <typename OutputSequence, typename Iterator, typename C>
OutputSequence blockingFiltered(Iterator begin, Iterator end, bool (C::*filter)() const)
{
    return blockingFilteredReduced(begin,
                                   end,
                                   filter,
                                   &OutputSequence::append,
                                   OrderedReduce);
}

} // namespace QtConcurrent

#endif // qdoc

QT_END_NAMESPACE
QT_END_HEADER

#endif // QT_NO_CONCURRENT

#endif
