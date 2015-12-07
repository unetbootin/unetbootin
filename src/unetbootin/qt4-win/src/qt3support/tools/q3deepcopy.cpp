/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt3Support module of the Qt Toolkit.
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

#include "q3deepcopy.h"

QT_BEGIN_NAMESPACE

/*!
    \class Q3DeepCopy qdeepcopy.h
    \brief The Q3DeepCopy class is a template class which ensures that
    implicitly shared and explicitly shared classes reference unique
    data.

    \reentrant

    \compat

    Normally, shared copies reference the same data to optimize memory
    use and for maximum speed. In the example below, \c s1, \c s2, \c
    s3, \c s4 and \c s5 share data.

    \snippet doc/src/snippets/code/src.qt3support.tools.q3deepcopy.cpp 0

    Q3DeepCopy can be used several ways to ensure that an object
    references unique, unshared data. In the example below, \c s1, \c
    s2 and \c s5 share data, while neither \c s3 nor \c s4 share data.
    \snippet doc/src/snippets/code/src.qt3support.tools.q3deepcopy.cpp 1

    In the example below, \c s1, \c s2 and \c s5 share data, and \c s3
    and \c s4 share data.
    \snippet doc/src/snippets/code/src.qt3support.tools.q3deepcopy.cpp 2

    Q3DeepCopy can also provide safety in multithreaded applications
    that use shared classes. In the example below, the variable \c
    global_string is used safely since the data contained in \c
    global_string is always a deep copy. This ensures that all threads
    get a unique copy of the data, and that any assignments to \c
    global_string will result in a deep copy.

    \snippet doc/src/snippets/code/src.qt3support.tools.q3deepcopy.cpp 3

    \warning It is the application developer's responsibility to
    protect the object shared across multiple threads.

    The examples above use QString, which is an implicitly shared
    class. The behavior of Q3DeepCopy is the same when using explicitly
    shared classes like QByteArray.

    Currently, Q3DeepCopy works with the following classes:
    \list
    \i QMemArray (including subclasses like QByteArray and QCString)
    \i QMap
    \i QString
    \i QValueList (including subclasses like QStringList and QValueStack)
    \i QValueVector
    \endlist

    \sa \link threads.html Thread Support in Qt \endlink
*/

/*!
    \fn Q3DeepCopy::Q3DeepCopy()

    Constructs an empty instance of type \e T.
*/

/*!
    \fn Q3DeepCopy::Q3DeepCopy( const T &t )

    Constructs a deep copy of \a t.
*/

/*!
    \fn Q3DeepCopy<T>& Q3DeepCopy::operator=( const T &t )

    Assigns a deep copy of \a t.
*/

/*!
    \fn Q3DeepCopy::operator T ()

    Returns a deep copy of the encapsulated data.
*/

QT_END_NAMESPACE
