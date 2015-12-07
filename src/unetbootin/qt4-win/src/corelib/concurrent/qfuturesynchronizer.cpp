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

/*! \class QFutureSynchronizer
    \since 4.4

    \brief The QFutureSynchronizer class is a convenience class that simplifies
    QFuture synchronization.
    
    QFutureSynchronizer is a template class that simplifies synchronization of
    one or more QFuture objects. Futures are added using the addFuture() or
    setFuture() functions. The futures() function returns a list of futures.
    Use clearFutures() to remove all futures from the QFutureSynchronizer.
    
    The waitForFinished() function waits for all futures to finish.
    The destructor of QFutureSynchronizer calls waitForFinished(), providing
    an easy way to ensure that all futures have finished before returning from
    a function:
    
    \snippet doc/src/snippets/code/src.corelib.concurrent.qfuturesynchronizer.cpp 0
    
    The behavior of waitForFinished() can be changed using the
    setCancelOnWait() function. Calling setCancelOnWait(true) will cause
    waitForFinished() to cancel all futures before waiting for them to finish.
    You can query the status of the cancel-on-wait feature using the
    cancelOnWait() function.
    
    \sa QFuture, QFutureWatcher, {threads.html#qtconcurrent-intro}{Qt Concurrent}
*/

/*!
    \fn QFutureSynchronizer::QFutureSynchronizer()

    Constructs a QFutureSynchronizer.
*/

/*!
    \fn QFutureSynchronizer::QFutureSynchronizer(const QFuture<T> &future)

    Constructs a QFutureSynchronizer and begins watching \a future by calling
    addFuture().
    
    \sa addFuture()
*/

/*!
    \fn QFutureSynchronizer::~QFutureSynchronizer()
    
    Calls waitForFinished() function to ensure that all futures have finished
    before destroying this QFutureSynchronizer.
    
    \sa waitForFinished()
*/

/*!
    \fn void QFutureSynchronizer::setFuture(const QFuture<T> &future)
    
    Sets \a future to be the only future managed by this QFutureSynchronizer.
    This is a convenience function that calls waitForFinished(),
    then clearFutures(), and finally passes \a future to addFuture().
    
    \sa addFuture(), waitForFinished(), clearFutures()
*/

/*!
    \fn void QFutureSynchronizer::addFuture(const QFuture<T> &future)

    Adds \a future to the list of managed futures.
    
    \sa futures()
*/

/*!
    \fn void QFutureSynchronizer::waitForFinished()

    Waits for all futures to finish. If cancelOnWait() returns true, each
    future is canceled before waiting for them to finish.
    
    \sa cancelOnWait(), setCancelOnWait()
*/

/*!
    \fn void QFutureSynchronizer::clearFutures()

    Removes all managed futures from this QFutureSynchronizer.
    
    \sa addFuture(), setFuture()   
*/

/*!
    \fn QList<QFuture<T> > QFutureSynchronizer::futures() const

    Returns a list of all managed futures.
    
    \sa addFuture(), setFuture()
*/

/*!
    \fn void QFutureSynchronizer::setCancelOnWait(bool enabled)
    
    Enables or disables the cancel-on-wait feature based on the \a enabled
    argument. If \a enabled is true, the waitForFinished() function will cancel
    all futures before waiting for them to finish.

    \sa waitForFinished()
*/

/*!
    \fn bool QFutureSynchronizer::cancelOnWait() const

    Returns true if the cancel-on-wait feature is enabled; otherwise returns
    false. If cancel-on-wait is enabled, the waitForFinished() function will
    cancel all futures before waiting for them to finish.

    \sa waitForFinished()
*/
