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

/*!
    \namespace QtConcurrent
    \since 4.4
    \brief The QtConcurrent namespace provides high-level APIs that make it
    possible to write multi-threaded programs without using low-level
    threading primitives.

    See the \l {threads.html#qtconcurrent-intro}{Qt Concurrent} section in the \l{threads.html}{threading} documentation.
    
    \ingroup thread
*/

/*!
    \namespace QtConcurrent::internal
    \internal

    \brief The QtConcurrent::internal namespace contains QtConcurrent
    implementation details.

    \ingroup thread
*/

/*!
    \enum QtConcurrent::ReduceOption
    This enum specifies the order of which results from the map or filter 
    function are passed to the reduce function.

    \value UnorderedReduce Reduction is done in an arbitrary order.
    \value OrderedReduce Reduction is done in the order of the
    original sequence.
    \value SequentialReduce Reduction is done sequentally: only one
    thread will enter the reduce function at a time (parallel reduction
    might be supported in a future version of Qt Concurrent.)
*/

/*!
    \headerfile <QtConcurrentMap>
    \title Concurrent Map and Map-Reduce

    \brief The <QtConcurrentMap> header provides concurrent Map and MapReduce.

    These functions are a part of the \l {threads.html#qtconcurrent-intro}{Qt Concurrent} framework.

    The QtConcurrent::map(), QtConcurrent::mapped() and
    QtConcurrent::mappedReduced() functions run computations in parallel on
    the items in a sequence such as a QList or a QVector. QtConcurrent::map()
    modifies a sequence in-place, QtConcurrent::mapped() returns a new
    sequence containing the modified content, and QtConcurrent::mappedReduced()
    returns a single result.

    \section1 Concurrent Map

    QtConcurrent::mapped() takes an input sequence and a map function. This map
    function is then called for each item in the sequence, and a new sequence
    containing the return values from the map function is returned.

    The map function must be of the form:

    \snippet doc/src/snippets/code/src.corelib.concurrent.qtconcurrentmap.cpp 0

    T and U can be any type (and they can even be the same type), but T must
    match the type stored in the sequence. The function returns the modified
    or \e mapped content.

    This example shows how to apply a scale function to all the items
    in a sequence:

    \snippet doc/src/snippets/code/src.corelib.concurrent.qtconcurrentmap.cpp 1

    The results of the map are made available through QFuture.  See the
    QFuture and QFutureWatcher documentation for more information on how to
    use QFuture in your applications.

    If you want to modify a sequence in-place, use QtConcurrent::map(). The
    map function must then be of the form:

    \snippet doc/src/snippets/code/src.corelib.concurrent.qtconcurrentmap.cpp 2

    Note that the return value and return type of the map function are not
    used.

    Using QtConcurrent::map() is similar to using QtConcurrent::mapped():

    \snippet doc/src/snippets/code/src.corelib.concurrent.qtconcurrentmap.cpp 3

    Since the sequence is modified in place, QtConcurrent::map() does not
    return any results via QFuture. However, you can still use QFuture and
    QFutureWatcher to monitor the status of the map.

    \section1 Concurrent Map-Reduce

    QtConcurrent::mappedReduced() is similar to QtConcurrent::mapped(), but
    instead of returning a sequence with the new results, the results are
    combined into a single value using a reduce function.

    The reduce function must be of the form:

    \snippet doc/src/snippets/code/src.corelib.concurrent.qtconcurrentmap.cpp 4

    T is the type of the final result, U is the return type of the map
    function. Note that the return value and return type of the reduce
    function are not used.

    Call QtConcurrent::mappedReduced() like this:

    \snippet doc/src/snippets/code/src.corelib.concurrent.qtconcurrentmap.cpp 5

    The reduce function will be called once for each result returned by the map
    function, and should merge the \e{intermediate} into the \e{result}
    variable.  QtConcurrent::mappedReduced() guarantees that only one thread
    will call reduce at a time, so using a mutex to lock the result variable
    is not neccesary. The QtConcurrent::ReduceOptions enum provides a way to
    control the order in which the reduction is done. If
    QtConcurrent::UnorderedReduce is used (the default), the order is
    undefined, while QtConcurrent::OrderedReduce ensures that the reduction
    is done in the order of the original sequence.

    \section1 Additional API Features

    \section2 Using Iterators instead of Sequence

    Each of the above functions has a variant that takes an iterator range
    instead of a sequence. You use them in the same way as the sequence
    variants:

    \snippet doc/src/snippets/code/src.corelib.concurrent.qtconcurrentmap.cpp 6

    \section2 Blocking Variants

    Each of the above functions has a blocking variant that returns
    the final result instead of a QFuture. You use them in the same
    way as the asynchronous variants.

    \snippet doc/src/snippets/code/src.corelib.concurrent.qtconcurrentmap.cpp 7

    Note that the result types above are not QFuture objects, but real result
    types (in this case, QList<QImage> and QImage).

    \section2 Using Member Functions

    QtConcurrent::map(), QtConcurrent::mapped(), and
    QtConcurrent::mappedReduced() accept pointers to member functions.
    The member function class type must match the type stored in the sequence:

    \snippet doc/src/snippets/code/src.corelib.concurrent.qtconcurrentmap.cpp 8

    Note that when using QtConcurrent::mappedReduced(), you can mix the use of
    normal and member functions freely:

    \snippet doc/src/snippets/code/src.corelib.concurrent.qtconcurrentmap.cpp 9

    \section2 Using Function Objects
    
    QtConcurrent::map(), QtConcurrent::mapped(), and
    QtConcurrent::mappedReduced() accept function objects, which can be used to
    add state to a function call. The result_type typedef must define the 
    result type of the function call operator:

    \snippet doc/src/snippets/code/src.corelib.concurrent.qtconcurrentmap.cpp 

    \section2 Using Bound Function Arguments

    Note that Qt does not provide support for bound functions. This is
    provided by 3rd party libraries like
    \l{http://www.boost.org/libs/bind/bind.html}{Boost} or
    \l{http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2005/n1836.pdf}{C++
    TR1 Library Extensions}.

    If you want to use a map function that takes more than one argument you can
    use boost::bind() or std::tr1::bind() to transform it onto a function that
    takes one argument.

    As an example, we'll use QImage::scaledToWidth():

    \snippet doc/src/snippets/code/src.corelib.concurrent.qtconcurrentmap.cpp 10

    scaledToWidth takes three arguments (including the "this" pointer) and
    can't be used with QtConcurrent::mapped() directly, because
    QtConcurrent::mapped() expects a function that takes one argument. To use
    QImage::scaledToWidth() with QtConcurrent::mapped() we have to provide a
    value for the \e{width} and the \e{transformation mode}:

    \snippet doc/src/snippets/code/src.corelib.concurrent.qtconcurrentmap.cpp 11

    The return value from boost::bind() is a function object (functor) with
    the following signature:

    \snippet doc/src/snippets/code/src.corelib.concurrent.qtconcurrentmap.cpp 12

    This matches what QtConcurrent::mapped() expects, and the complete example
    becomes:

    \snippet doc/src/snippets/code/src.corelib.concurrent.qtconcurrentmap.cpp 13
*/

/*!
    \fn QFuture<void> QtConcurrent::map(Sequence &sequence, MapFunction function)
    \relates <QtConcurrentMap>

    Calls \a function once for each item in \a sequence. The \a function is
    passed a reference to the item, so that any modifications done to the item
    will appear in \a sequence.
*/

/*!
    \fn QFuture<void> QtConcurrent::map(Iterator begin, Iterator end, MapFunction function)
    \relates <QtConcurrentMap>

    Calls \a function once for each item from \a begin to \a end. The
    \a function is passed a reference to the item, so that any modifications
    done to the item will appear in the sequence which the iterators belong to.
*/

/*!
    \fn QFuture<T> QtConcurrent::mapped(const Sequence &sequence, MapFunction function)
    \relates <QtConcurrentMap>

    Calls \a function once for each item in \a sequence and returns a future
    with each mapped item as a result. You can use QFuture::const_iterator or
    QFutureIterator to iterate through the results.
*/

/*!
    \fn QFuture<T> QtConcurrent::mapped(ConstIterator begin, ConstIterator end, MapFunction function)
    \relates <QtConcurrentMap>

    Calls \a function once for each item from \a begin to \a end and returns a
    future with each mapped item as a result. You can use
    QFuture::const_iterator or QFutureIterator to iterate through the results.
*/

/*!
    \fn QFuture<T> QtConcurrent::mappedReduced(const Sequence &sequence,
    MapFunction mapFunction, ReduceFunction reduceFunction,
    QtConcurrent::ReduceOptions reduceOptions)

    \relates <QtConcurrentMap>

    Calls \a mapFunction once for each item in \a sequence. The return value of
    each \a mapFunction is passed to \a reduceFunction.

    Note that while \a mapFunction is called concurrently, only one thread at a
    time will call \a reduceFunction. The order in which \a reduceFunction is
    called is determined by \a reduceOptions.
*/

/*!
    \fn QFuture<T> QtConcurrent::mappedReduced(ConstIterator begin,
    ConstIterator end, MapFunction mapFunction, ReduceFunction reduceFunction,
    QtConcurrent::ReduceOptions reduceOptions)

    \relates <QtConcurrentMap>

    Calls \a mapFunction once for each item from \a begin to \a end. The return
    value of each \a mapFunction is passed to \a reduceFunction.

    Note that while \a mapFunction is called concurrently, only one thread at a
    time will call \a reduceFunction. The order in which \a reduceFunction is
    called is undefined.
*/
