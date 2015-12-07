/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
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
    \class QPointer
    \brief The QPointer class is a template class that provides guarded pointers to QObjects.

    \ingroup objectmodel
    \mainclass

    A guarded pointer, QPointer<T>, behaves like a normal C++
    pointer \c{T *}, except that it is automatically set to 0 when the
    referenced object is destroyed (unlike normal C++ pointers, which
    become "dangling pointers" in such cases). \c T must be a
    subclass of QObject.

    Guarded pointers are useful whenever you need to store a pointer
    to a QObject that is owned by someone else, and therefore might be
    destroyed while you still hold a reference to it. You can safely
    test the pointer for validity.

    Example:

    \snippet doc/src/snippets/pointer/pointer.cpp 0
    \dots
    \snippet doc/src/snippets/pointer/pointer.cpp 1
    \snippet doc/src/snippets/pointer/pointer.cpp 2

    If the QLabel is deleted in the meantime, the \c label variable
    will hold 0 instead of an invalid address, and the last line will
    never be executed.

    The functions and operators available with a QPointer are the
    same as those available with a normal unguarded pointer, except
    the pointer arithmetic operators (\c{+}, \c{-}, \c{++}, and
    \c{--}), which are normally used only with arrays of objects.

    Use QPointers like normal pointers and you will not need to read
    this class documentation.

    For creating guarded pointers, you can construct or assign to them
    from a T* or from another guarded pointer of the same type. You
    can compare them with each other using operator==() and
    operator!=(), or test for 0 with isNull(). You can dereference
    them using either the \c *x or the \c x->member notation.

    A guarded pointer will automatically cast to a \c T *, so you can
    freely mix guarded and unguarded pointers. This means that if you
    have a QPointer<QWidget>, you can pass it to a function that
    requires a QWidget *. For this reason, it is of little value to
    declare functions to take a QPointer as a parameter; just use
    normal pointers. Use a QPointer when you are storing a pointer
    over time.

    Note that class \c T must inherit QObject, or a compilation or
    link error will result.

    \sa QObject, QObjectCleanupHandler
*/

/*!
    \fn QPointer::QPointer()

    Constructs a 0 guarded pointer.

    \sa isNull()
*/

/*!
    \fn QPointer::QPointer(T* p)

    Constructs a guarded pointer that points to same object that \a p
    points to.
*/

/*!
    \fn QPointer::QPointer(const QPointer<T> &p)

    Copies one guarded pointer from another. The constructed guarded
    pointer points to the same object that \a p points to (which may
    be 0).
*/

/*!
    \fn QPointer::~QPointer()

    Destroys the guarded pointer. Just like a normal pointer,
    destroying a guarded pointer does \e not destroy the object being
    pointed to.
*/

/*!
    \fn QPointer<T>& QPointer::operator=(const QPointer<T> &p)

    Assignment operator. This guarded pointer will now point to the
    same object that \a p points to.
*/

/*!
    \fn QPointer<T> & QPointer::operator=(T* p)

    \overload

    Assignment operator. This guarded pointer will now point to the
    same object that \a p points to.
*/

/*!
    \fn T* QPointer::data() const
    \since 4.4

    Returns the pointer to the object being guarded.
*/

/*!
    \fn bool QPointer::isNull() const

    Returns \c true if the referenced object has been destroyed or if
    there is no referenced object; otherwise returns false.
*/

/*!
    \fn T* QPointer::operator->() const

    Overloaded arrow operator; implements pointer semantics. Just use
    this operator as you would with a normal C++ pointer.
*/

/*!
    \fn T& QPointer::operator*() const

    Dereference operator; implements pointer semantics. Just use this
    operator as you would with a normal C++ pointer.
*/

/*!
    \fn QPointer::operator T*() const

    Cast operator; implements pointer semantics. Because of this
    function you can pass a QPointer\<T\> to a function where a T*
    is required.
*/

/*!
    \fn bool operator==(const T *o, const QPointer<T> &p)

    Equality operator. Returns true if \a o and the guarded
    pointer \a p are pointing to the same object, otherwise
    returns false.

*/
/*!
    \fn bool operator==(const QPointer<T> &p, const T *o)

    Equality operator. Returns true if \a o and the guarded
    pointer \a p are pointing to the same object, otherwise
    returns false.

*/
/*!
    \fn bool operator==(T *o, const QPointer<T> &p)

    Equality operator. Returns true if \a o and the guarded
    pointer \a p are pointing to the same object, otherwise
    returns false.

*/
/*!
    \fn bool operator==(const QPointer<T> &p, T *o)

    Equality operator. Returns true if \a o and the guarded
    pointer \a p are pointing to the same object, otherwise
    returns false.

*/
/*!
    \fn bool operator==(const QPointer<T> &p1, const QPointer<T> &p2)

    Equality operator. Returns true if the guarded pointers \a p1 and \a p2
    are pointing to the same object, otherwise
    returns false.

*/


/*!
    \fn bool operator!=(const T *o, const QPointer<T> &p)

    Inequality operator. Returns true if \a o and the guarded
    pointer \a p are not pointing to the same object, otherwise
    returns false.
*/
/*!
    \fn bool operator!=(const QPointer<T> &p, const T *o)

    Inequality operator. Returns true if \a o and the guarded
    pointer \a p are not pointing to the same object, otherwise
    returns false.
*/
/*!
    \fn bool operator!=(T *o, const QPointer<T> &p)

    Inequality operator. Returns true if \a o and the guarded
    pointer \a p are not pointing to the same object, otherwise
    returns false.
*/
/*!
    \fn bool operator!=(const QPointer<T> &p, T *o)

    Inequality operator. Returns true if \a o and the guarded
    pointer \a p are not pointing to the same object, otherwise
    returns false.
*/
/*!
    \fn bool operator!=(const QPointer<T> &p1, const QPointer<T> &p2)

    Inequality operator. Returns true if  the guarded pointers \a p1 and
    \a p2 are not pointing to the same object, otherwise
    returns false.
*/
