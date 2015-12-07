/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtScriptTools module of the Qt Toolkit.
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

#include "qscriptdebuggervalue_p.h"

#include <QtScript/qscriptvalue.h>
#include <QtScript/qscriptengine.h>
#include <QtCore/qdatastream.h>
#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

/*!
  \since 4.5
  \class QScriptDebuggerValue
  \internal

  \brief The QScriptDebuggerValue class represents a script value.
*/

class QScriptDebuggerValuePrivate
{
public:
    QScriptDebuggerValuePrivate();
    ~QScriptDebuggerValuePrivate();

    QScriptDebuggerValue::ValueType type;
    union {
        bool booleanValue;
        QString *stringValue;
        double numberValue;
        qint64 objectId;
    };

    QBasicAtomicInt ref;
};

QScriptDebuggerValuePrivate::QScriptDebuggerValuePrivate()
{
    ref = 0;
}

QScriptDebuggerValuePrivate::~QScriptDebuggerValuePrivate()
{
    if (type == QScriptDebuggerValue::StringValue)
        delete stringValue;
}

QScriptDebuggerValue::QScriptDebuggerValue()
    : d_ptr(0)
{
}

QScriptDebuggerValue::QScriptDebuggerValue(const QScriptValue &value)
    : d_ptr(0)
{
    if (value.isValid()) {
        d_ptr = new QScriptDebuggerValuePrivate;
        if (value.isUndefined())
            d_ptr->type = UndefinedValue;
        else if (value.isNull())
            d_ptr->type = NullValue;
        else if (value.isNumber()) {
            d_ptr->type = NumberValue;
            d_ptr->numberValue = value.toNumber();
        } else if (value.isBoolean()) {
            d_ptr->type = BooleanValue;
            d_ptr->booleanValue = value.toBoolean();
        } else if (value.isString()) {
            d_ptr->type = StringValue;
            d_ptr->stringValue = new QString(value.toString());
        } else {
            Q_ASSERT(value.isObject());
            d_ptr->type = ObjectValue;
            d_ptr->objectId = value.objectId();
        }
        d_ptr->ref.ref();
    }
}

QScriptDebuggerValue::QScriptDebuggerValue(double value)
    : d_ptr(new QScriptDebuggerValuePrivate)
{
    d_ptr->type = NumberValue;
    d_ptr->numberValue = value;
    d_ptr->ref.ref();
}

QScriptDebuggerValue::QScriptDebuggerValue(bool value)
    : d_ptr(new QScriptDebuggerValuePrivate)
{
    d_ptr->type = BooleanValue;
    d_ptr->booleanValue = value;
    d_ptr->ref.ref();
}

QScriptDebuggerValue::QScriptDebuggerValue(const QString &value)
    : d_ptr(new QScriptDebuggerValuePrivate)
{
    d_ptr->type = StringValue;
    d_ptr->stringValue = new QString(value);
    d_ptr->ref.ref();
}

QScriptDebuggerValue::QScriptDebuggerValue(qint64 objectId)
    : d_ptr(new QScriptDebuggerValuePrivate)
{
    d_ptr->type = ObjectValue;
    d_ptr->objectId = objectId;
    d_ptr->ref.ref();
}

QScriptDebuggerValue::QScriptDebuggerValue(ValueType type)
    : d_ptr(new QScriptDebuggerValuePrivate)
{
    d_ptr->type = type;
    d_ptr->ref.ref();
}

QScriptDebuggerValue::QScriptDebuggerValue(const QScriptDebuggerValue &other)
    : d_ptr(other.d_ptr)
{
    if (d_ptr)
        d_ptr->ref.ref();
}

QScriptDebuggerValue::~QScriptDebuggerValue()
{
    if (d_ptr && !d_ptr->ref.deref()) {
        delete d_ptr;
        d_ptr = 0;
    }
}

QScriptDebuggerValue &QScriptDebuggerValue::operator=(const QScriptDebuggerValue &other)
{
    if (d_ptr == other.d_ptr)
        return *this;
    if (d_ptr && !d_ptr->ref.deref())
        delete d_ptr;
    d_ptr = other.d_ptr;
    if (d_ptr)
        d_ptr->ref.ref();
    return *this;
}

/*!
  Returns the type of this value.
*/
QScriptDebuggerValue::ValueType QScriptDebuggerValue::type() const
{
    Q_D(const QScriptDebuggerValue);
    if (!d)
        return NoValue;
    return d->type;
}

/*!
  Returns this value as a number.
*/
double QScriptDebuggerValue::numberValue() const
{
    Q_D(const QScriptDebuggerValue);
    if (!d)
        return 0;
    Q_ASSERT(d->type == NumberValue);
    return d->numberValue;
}

/*!
  Returns this value as a boolean.
*/
bool QScriptDebuggerValue::booleanValue() const
{
    Q_D(const QScriptDebuggerValue);
    if (!d)
        return false;
    Q_ASSERT(d->type == BooleanValue);
    return d->booleanValue;
}

/*!
  Returns this value as a string.
*/
QString QScriptDebuggerValue::stringValue() const
{
    Q_D(const QScriptDebuggerValue);
    if (!d)
        return QString();
    Q_ASSERT(d->type == StringValue);
    return *d->stringValue;
}

/*!
  Returns this value as an object ID.
*/
qint64 QScriptDebuggerValue::objectId() const
{
    Q_D(const QScriptDebuggerValue);
    if (!d)
        return -1;
    Q_ASSERT(d->type == ObjectValue);
    return d->objectId;
}

/*!
  Converts this QScriptDebuggerValue to a QScriptValue in the
  given \a engine and returns the resulting value.
*/
QScriptValue QScriptDebuggerValue::toScriptValue(QScriptEngine *engine) const
{
    Q_D(const QScriptDebuggerValue);
    if (!d)
        return QScriptValue();
    switch (d->type) {
    case NoValue:
        return QScriptValue();
    case UndefinedValue:
        return engine->undefinedValue();
    case NullValue:
        return engine->nullValue();
    case BooleanValue:
        return QScriptValue(engine, d->booleanValue);
    case StringValue:
        return QScriptValue(engine, *d->stringValue);
    case NumberValue:
        return QScriptValue(engine, d->numberValue);
    case ObjectValue:
        return engine->objectById(d->objectId);
    }
    return QScriptValue();
}

/*!
  Returns a string representation of this value.
*/
QString QScriptDebuggerValue::toString() const
{
    Q_D(const QScriptDebuggerValue);
    if (!d)
        return QString();
    switch (d->type) {
    case NoValue:
        return QString();
    case UndefinedValue:
        return QString::fromLatin1("undefined");
    case NullValue:
        return QString::fromLatin1("null");
    case BooleanValue:
        if (d->booleanValue)
            return QString::fromLatin1("true");
        else
            return QString::fromLatin1("false");
    case StringValue:
        return *d->stringValue;
    case NumberValue:
        return QString::number(d->numberValue); // ### qScriptNumberToString()
    case ObjectValue:
        return QString::fromLatin1("[object Object]");
    }
    return QString();
}

/*!
  Returns true if this QScriptDebuggerValue is equal to the \a other
  value, otherwise returns false.
*/
bool QScriptDebuggerValue::operator==(const QScriptDebuggerValue &other) const
{
    Q_D(const QScriptDebuggerValue);
    const QScriptDebuggerValuePrivate *od = other.d_func();
    if (d == od)
        return true;
    if (!d || !od)
        return false;
    if (d->type != od->type)
        return false;
    switch (d->type) {
    case NoValue:
    case UndefinedValue:
    case NullValue:
        return true;
    case BooleanValue:
        return d->booleanValue == od->booleanValue;
    case StringValue:
        return *d->stringValue == *od->stringValue;
    case NumberValue:
        return d->numberValue == od->numberValue;
    case ObjectValue:
        return d->objectId == od->objectId;
    }
    return false;
}

/*!
  Returns true if this QScriptDebuggerValue is not equal to the \a
  other value, otherwise returns false.
*/
bool QScriptDebuggerValue::operator!=(const QScriptDebuggerValue &other) const
{
    return !(*this == other);
}

/*!
  \fn QDataStream &operator<<(QDataStream &stream, const QScriptDebuggerValue &value)
  \relates QScriptDebuggerValue

  Writes the given \a value to the specified \a stream.
*/
QDataStream &operator<<(QDataStream &out, const QScriptDebuggerValue &value)
{
    out << (quint32)value.type();
    switch (value.type()) {
    case QScriptDebuggerValue::NoValue:
    case QScriptDebuggerValue::UndefinedValue:
    case QScriptDebuggerValue::NullValue:
        break;
    case QScriptDebuggerValue::BooleanValue:
        out << value.booleanValue();
        break;
    case QScriptDebuggerValue::StringValue:
        out << value.stringValue();
        break;
    case QScriptDebuggerValue::NumberValue:
        out << value.numberValue();
        break;
    case QScriptDebuggerValue::ObjectValue:
        out << value.objectId();
        break;
    }
    return out;
}

/*!
  \fn QDataStream &operator>>(QDataStream &stream, QScriptDebuggerValue &value)
  \relates QScriptDebuggerValue

  Reads a QScriptDebuggerValue from the specified \a stream into the
  given \a value.
*/
QDataStream &operator>>(QDataStream &in, QScriptDebuggerValue &value)
{
    quint32 type;
    in >> type;
    switch (QScriptDebuggerValue::ValueType(type)) {
    case QScriptDebuggerValue::UndefinedValue:
    case QScriptDebuggerValue::NullValue:
        value = QScriptDebuggerValue(QScriptDebuggerValue::ValueType(type));
        break;
    case QScriptDebuggerValue::BooleanValue: {
        bool b;
        in >> b;
        value = QScriptDebuggerValue(b);
    }   break;
    case QScriptDebuggerValue::StringValue: {
        QString s;
        in >> s;
        value = QScriptDebuggerValue(s);
    }   break;
    case QScriptDebuggerValue::NumberValue: {
        double d;
        in >> d;
        value = QScriptDebuggerValue(d);
    }   break;
    case QScriptDebuggerValue::ObjectValue: {
        qint64 id;
        in >> id;
        value = QScriptDebuggerValue(id);
    }   break;
    case QScriptDebuggerValue::NoValue:
    default:
        value = QScriptDebuggerValue();
        break;
    }
    return in;
}

QT_END_NAMESPACE
