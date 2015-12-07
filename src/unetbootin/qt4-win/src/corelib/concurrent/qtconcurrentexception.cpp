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

#include "qtconcurrentexception.h"

#ifndef QT_NO_QFUTURE
#ifndef QT_NO_EXCEPTIONS

QT_BEGIN_NAMESPACE

/*! 
    \class QtConcurrent::Exception
    \brief The Exception class provides a base class for exceptions that can transferred across threads.
    \since 4.4

    Qt Concurrent supports throwing and catching exceptions across thread
    boundaries, provided that the exception inherit from QtConcurrent::Exception
    and implement two helper functions:

    \snippet doc/src/snippets/code/src.corelib.concurrent.qtconcurrentexception.cpp 0

    QtConcurrent::Exception subclasses must be thrown by value and
    caught by reference:

    \snippet doc/src/snippets/code/src.corelib.concurrent.qtconcurrentexception.cpp 1
*/

/*!
    \fn QtConcurrent::Exception::raise() const 
    In your QtConcurrent::Exception subclass, reimplement raise() like this:
    
    \snippet doc/src/snippets/code/src.corelib.concurrent.qtconcurrentexception.cpp 2
*/

/*!
    \fn QtConcurrent::Exception::clone() const
    In your QtConcurrent::Exception subclass, reimplement clone() like this:
    
    \snippet doc/src/snippets/code/src.corelib.concurrent.qtconcurrentexception.cpp 3
*/

/*! 
    \class QtConcurrent::UnhandledException

    \brief The UnhandledException class represents an unhandled exception in a worker thread.
    \since 4.4

    If a worker thread throws an exception that is not a subclass of QtConcurrent::Exception,
    the Qt Concurrent functions will throw a QtConcurrent::UnhandledException
    on the receiver thread side.

    Inheriting from this class is not supported.
*/

/*!
    \fn QtConcurrent::UnhandledException::raise() const
    \internal
*/

/*!
    \fn QtConcurrent::UnhandledException::clone() const
    \internal
*/

namespace QtConcurrent
{

void Exception::raise() const
{
    Exception e = *this;
    throw e;
}

Exception *Exception::clone() const
{
    return new Exception(*this);
}

void UnhandledException::raise() const
{
    UnhandledException e = *this;
    throw e;
}

Exception *UnhandledException::clone() const
{
    return new UnhandledException(*this);
}

#ifndef qdoc

namespace internal {

class Base
{
public:
    Base(Exception *exception)
    : exception(exception), refCount(1), hasThrown(false) { }
    ~Base() { delete exception; }

    Exception *exception;
    QAtomicInt refCount;
    bool hasThrown;
};

ExceptionHolder::ExceptionHolder(Exception *exception)
: base(new Base(exception)) {}

ExceptionHolder::ExceptionHolder(const ExceptionHolder &other)
: base(other.base)
{
    base->refCount.ref();
}

void ExceptionHolder::operator=(const ExceptionHolder &other)
{
    if (base == other.base)
        return;

    if (base->refCount.deref() == false)
        delete base;

    base = other.base;
    base->refCount.ref();
}

ExceptionHolder::~ExceptionHolder()
{
    if (base->refCount.deref() == 0)
        delete base;
}

Exception *ExceptionHolder::exception() const
{
    return base->exception;
}

void ExceptionStore::setException(const Exception &e)
{
    if (hasException() == false)
        exceptionHolder = ExceptionHolder(e.clone());
}

bool ExceptionStore::hasException() const
{
    return (exceptionHolder.exception() != 0);
}

ExceptionHolder ExceptionStore::exception()
{
    return exceptionHolder;
}

void ExceptionStore::throwPossibleException()
{
    /* On win32-g++, with GCC 3.4.2 std::uncaught_exception() isn't reliable. */
    if (hasException()
#ifndef Q_CC_MINGW
        && std::uncaught_exception() == false
#endif
            ) {
        exceptionHolder.base->hasThrown = true;
        exceptionHolder.exception()->raise();
    }
}

bool ExceptionStore::hasThrown() const { return exceptionHolder.base->hasThrown; }

} // namespace internal

#endif //qdoc

} // namespace QtConcurrent

QT_END_NAMESPACE

#endif // QT_NO_EXCEPTIONS
#endif // QT_NO_CONCURRENT
