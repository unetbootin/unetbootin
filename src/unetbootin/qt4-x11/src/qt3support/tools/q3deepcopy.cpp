/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt3Support module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
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

    \snippet doc/src/snippets/code/src_qt3support_tools_q3deepcopy.cpp 0

    Q3DeepCopy can be used several ways to ensure that an object
    references unique, unshared data. In the example below, \c s1, \c
    s2 and \c s5 share data, while neither \c s3 nor \c s4 share data.
    \snippet doc/src/snippets/code/src_qt3support_tools_q3deepcopy.cpp 1

    In the example below, \c s1, \c s2 and \c s5 share data, and \c s3
    and \c s4 share data.
    \snippet doc/src/snippets/code/src_qt3support_tools_q3deepcopy.cpp 2

    Q3DeepCopy can also provide safety in multithreaded applications
    that use shared classes. In the example below, the variable \c
    global_string is used safely since the data contained in \c
    global_string is always a deep copy. This ensures that all threads
    get a unique copy of the data, and that any assignments to \c
    global_string will result in a deep copy.

    \snippet doc/src/snippets/code/src_qt3support_tools_q3deepcopy.cpp 3

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
