/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtScript module of the Qt Toolkit.
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

#include "qscriptvalueiterator.h"

#ifndef QT_NO_SCRIPT

#include "qscriptvalueiterator_p.h"
#include "qscriptvalueiteratorimpl_p.h"
#include "qscriptengine_p.h"
#include "qscriptcontext_p.h"
#include "qscriptvalueimpl_p.h"
#include "qscriptmember_p.h"
#include "qscriptobject_p.h"

QT_BEGIN_NAMESPACE

/*!
  \since 4.3
  \class QScriptValueIterator

  \brief The QScriptValueIterator class provides a Java-style iterator for QScriptValue.

  \ingroup script
  \mainclass

  The QScriptValueIterator constructor takes a QScriptValue as
  argument.  After construction, the iterator is located at the very
  beginning of the sequence of properties. Here's how to iterate over
  all the properties of a QScriptValue:

  \snippet doc/src/snippets/code/src_script_qscriptvalueiterator.cpp 0

  The next() advances the iterator. The name(), value() and flags()
  functions return the name, value and flags of the last item that was
  jumped over.

  If you want to remove properties as you iterate over the
  QScriptValue, use remove(). If you want to modify the value of a
  property, use setValue().

  Note that QScriptValueIterator only iterates over the QScriptValue's
  own properties; i.e. it does not follow the prototype chain. You can
  use a loop like this to follow the prototype chain:

  \snippet doc/src/snippets/code/src_script_qscriptvalueiterator.cpp 1

  Note that QScriptValueIterator will not automatically skip over
  properties that have the QScriptValue::SkipInEnumeration flag set;
  that flag only affects iteration in script code.  If you want, you
  can skip over such properties with code like the following:

  \snippet doc/src/snippets/code/src_script_qscriptvalueiterator.cpp 2

  \sa QScriptValue::property()
*/

/*!
  \internal
*/
QScriptValueIteratorPrivate::QScriptValueIteratorPrivate()
{
}

/*!
  \internal
*/
QScriptValueIteratorPrivate::~QScriptValueIteratorPrivate()
{
}

/*!
  Constructs an iterator for traversing \a object. The iterator is
  set to be at the front of the sequence of properties (before the
  first property).
*/
QScriptValueIterator::QScriptValueIterator(const QScriptValue &object)
{
    QScriptValueImpl val = QScriptValuePrivate::valueOf(object);
    if (!val.isObject()) {
        d_ptr = 0;
    } else {
        d_ptr = new QScriptValueIteratorPrivate();
        d_ptr->it = new QScriptValueIteratorImpl(val);
    }
}

/*!
  Destroys the iterator.
*/
QScriptValueIterator::~QScriptValueIterator()
{
    if (d_ptr) {
        delete d_ptr->it;
        delete d_ptr;
        d_ptr = 0;
    }
}

/*!
  Returns true if there is at least one item ahead of the iterator
  (i.e. the iterator is \e not at the back of the property sequence);
  otherwise returns false.

  \sa next(), hasPrevious()
*/
bool QScriptValueIterator::hasNext() const
{
    Q_D(const QScriptValueIterator);
    return (d && d->it->hasNext());
}

/*!
  Advances the iterator by one position.

  Calling this function on an iterator located at the back of the
  container leads to undefined results.

  \sa hasNext(), previous(), name()
*/
void QScriptValueIterator::next()
{
    Q_D(QScriptValueIterator);
    if (d)
        d->it->next();
}

/*!
  Returns true if there is at least one item behind the iterator
  (i.e. the iterator is \e not at the front of the property sequence);
  otherwise returns false.

  \sa previous(), hasNext()
*/
bool QScriptValueIterator::hasPrevious() const
{
    Q_D(const QScriptValueIterator);
    return (d && d->it->hasPrevious());
}

/*!
  Moves the iterator back by one position.

  Calling this function on an iterator located at the front of the
  container leads to undefined results.

  \sa hasPrevious(), next(), name()
*/
void QScriptValueIterator::previous()
{
    Q_D(QScriptValueIterator);
    if (d)
        d->it->previous();
}

/*!
  Moves the iterator to the front of the QScriptValue (before the
  first property).

  \sa toBack(), next()
*/
void QScriptValueIterator::toFront()
{
    Q_D(QScriptValueIterator);
    if (d)
        d->it->toFront();
}

/*!
  Moves the iterator to the back of the QScriptValue (after the
  last property).

  \sa toFront(), previous()
*/
void QScriptValueIterator::toBack()
{
    Q_D(QScriptValueIterator);
    if (d)
        d->it->toBack();
}

/*!
  Returns the name of the last property that was jumped over using
  next() or previous().

  \sa value(), flags()
*/
QString QScriptValueIterator::name() const
{
    Q_D(const QScriptValueIterator);
    if (!d)
        return QString();
    return d->it->name();
}

/*!
  \since 4.4

  Returns the name of the last property that was jumped over using
  next() or previous().
*/
QScriptString QScriptValueIterator::scriptName() const
{
    Q_D(const QScriptValueIterator);
    if (!d)
        return QScriptString();
    QScriptEnginePrivate *eng = d->it->object().engine();
    return eng->internedString(d->it->nameId());
}

/*!
  Returns the value of the last property that was jumped over using
  next() or previous().

  \sa setValue(), name()
*/
QScriptValue QScriptValueIterator::value() const
{
    Q_D(const QScriptValueIterator);
    if (!d)
        return QScriptValue();
    QScriptEnginePrivate *eng = d->it->object().engine();
    return eng->toPublic(d->it->value());
}

/*!
  Sets the \a value of the last property that was jumped over using
  next() or previous().

  \sa value(), name()
*/
void QScriptValueIterator::setValue(const QScriptValue &value)
{
    Q_D(const QScriptValueIterator);
    if (d) {
        QScriptEnginePrivate *eng = d->it->object().engine();
        d->it->setValue(eng->toImpl(value));
    }
}

/*!
  Returns the flags of the last property that was jumped over using
  next() or previous().

  \sa value()
*/
QScriptValue::PropertyFlags QScriptValueIterator::flags() const
{
    Q_D(const QScriptValueIterator);
    if (!d)
        return 0;
    return QScriptValue::PropertyFlags(d->it->flags() & ~QScript::Member::InternalRange);
}

/*!
  Removes the last property that was jumped over using next()
  or previous().

  \sa setValue()
*/
void QScriptValueIterator::remove()
{
    Q_D(const QScriptValueIterator);
    if (d)
        d->it->remove();
}

/*!
  Makes the iterator operate on \a object. The iterator is set to be
  at the front of the sequence of properties (before the first
  property).
*/
QScriptValueIterator& QScriptValueIterator::operator=(QScriptValue &object)
{
    if (d_ptr) {
        delete d_ptr->it;
        d_ptr = 0;
    }
    QScriptValueImpl val = QScriptValuePrivate::valueOf(object);
    if (val.isObject()) {
        d_ptr = new QScriptValueIteratorPrivate();
        d_ptr->it = new QScriptValueIteratorImpl(val);
    }
    return *this;
}

QT_END_NAMESPACE

#endif // QT_NO_SCRIPT
