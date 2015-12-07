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

#include "qscriptvalue.h"

#ifndef QT_NO_SCRIPT

#include "qscriptvalue_p.h"
#include "qscriptengine_p.h"
#include "qscriptvalueimpl_p.h"
#include "qscriptcontext_p.h"
#include "qscriptmember_p.h"
#include "qscriptobject_p.h"
#include "qscriptclass.h"
#include "qscriptclass_p.h"

#include <QtCore/QDateTime>
#include <QtCore/QRegExp>

QT_BEGIN_NAMESPACE

/*!
  \since 4.3
  \class QScriptValue

  \brief The QScriptValue class acts as a container for the Qt Script data types.

  \ingroup script
  \mainclass

  QScriptValue supports the types defined in the \l{ECMA-262}
  standard: The primitive types, which are Undefined, Null, Boolean,
  Number, and String; and the Object type. Additionally, Qt Script
  has built-in support for QVariant, QObject and QMetaObject.

  For the object-based types (including Date and RegExp), use the
  newT() functions in QScriptEngine (e.g. QScriptEngine::newObject())
  to create a QScriptValue of the desired type. For the primitive types,
  use one of the QScriptValue constructor overloads.

  The methods named isT() (e.g. isBool(), isUndefined()) can be
  used to test if a value is of a certain type. The methods named
  toT() (e.g. toBool(), toString()) can be used to convert a
  QScriptValue to another type. You can also use the generic
  qscriptvalue_cast() function.

  Object values have zero or more properties which are themselves
  QScriptValues. Use setProperty() to set a property of an object, and
  call property() to retrieve the value of a property.

  \snippet doc/src/snippets/code/src_script_qscriptvalue.cpp 0

  Each property can have a set of attributes; these are specified as
  the third (optional) argument to setProperty(). The attributes of a
  property can be queried by calling the propertyFlags() function. The
  following code snippet creates a property that cannot be modified by
  script code:

  \snippet doc/src/snippets/code/src_script_qscriptvalue.cpp 1

  If you want to iterate over the properties of a script object, use
  the QScriptValueIterator class.

  Object values have an internal \c{prototype} property, which can be
  accessed with prototype() and setPrototype(). Properties added to a
  prototype are shared by all objects having that prototype; this is
  referred to as prototype-based inheritance. In practice, it means
  that (by default) the property() function will automatically attempt
  to look up look the property in the prototype() (and in the
  prototype of the prototype(), and so on), if the object itself does
  not have the requested property. Note that this prototype-based
  lookup is not performed by setProperty(); setProperty() will always
  create the property in the script object itself.  For more
  information, see the \l{QtScript} documentation.

  Function objects (objects for which isFunction() returns true) can
  be invoked by calling call(). Constructor functions can be used to
  construct new objects by calling construct().

  Use equals(), strictlyEquals() and lessThan() to compare a QScriptValue
  to another.

  Object values can have custom data associated with them; see the
  setData() and data() functions. By default, this data is not
  accessible to scripts; it can be used to store any data you want to
  associate with the script object. Typically this is used by custom
  class objects (see QScriptClass) to store a C++ type that contains
  the "native" object data.

  Note that a QScriptValue for which isObject() is true only carries a
  reference to an actual object; copying the QScriptValue will only
  copy the object reference, not the object itself. If you want to
  clone an object (i.e. copy an object's properties to another
  object), you can do so with the help of a \c{for-in} statement in
  script code, or QScriptValueIterator in C++.

  \sa QScriptEngine, QScriptValueIterator
*/

/*!
    \enum QScriptValue::SpecialValue

    This enum is used to specify a single-valued type.

    \value UndefinedValue An undefined value.

    \value NullValue A null value.
*/

/*!
    \enum QScriptValue::PropertyFlag

    This enum describes the attributes of a property.

    \value ReadOnly The property is read-only. Attempts by Qt Script code to write to the property will be ignored.

    \value Undeletable Attempts by Qt Script code to \c{delete} the property will be ignored.

    \value SkipInEnumeration The property is not to be enumerated by a \c{for-in} enumeration.

    \value PropertyGetter The property is defined by a function which will be called to get the property value.

    \value PropertySetter The property is defined by a function which will be called to set the property value.

    \value QObjectMember This flag is used to indicate that an existing property is a QObject member (a property or method).

    \value KeepExistingFlags This value is used to indicate to setProperty() that the property's flags should be left unchanged. If the property doesn't exist, the default flags (0) will be used.

    \value UserRange Flags in this range are not used by Qt Script, and can be used for custom purposes.
*/

/*!
    \enum QScriptValue::ResolveFlag

    This enum specifies how to look up a property of an object.

    \value ResolveLocal Only check the object's own properties.

    \value ResolvePrototype Check the object's own properties first, then search the prototype chain. This is the default.

    \value ResolveScope Check the object's own properties first, then search the scope chain.

    \value ResolveFull Check the object's own properties first, then search the prototype chain, and finally search the scope chain.
*/

/*!
  Constructs an invalid QScriptValue.
*/
QScriptValue::QScriptValue()
    : d_ptr(0)
{
}

/*!
  Destroys this QScriptValue.
*/
QScriptValue::~QScriptValue()
{
    if (d_ptr && !d_ptr->ref.deref()) {
        if (engine()) {
            QScriptEnginePrivate::get(engine())->unregisterValue(d_ptr);
        } else {
            delete d_ptr;
        }
        d_ptr = 0;
    }
}

/*!
  Constructs a new QScriptValue that is a copy of \a other.

  Note that if \a other is an object (i.e., isObject() would return
  true), then only a reference to the underlying object is copied into
  the new script value (i.e., the object itself is not copied).
*/
QScriptValue::QScriptValue(const QScriptValue &other)
    : d_ptr(other.d_ptr)
{
    if (d_ptr)
        d_ptr->ref.ref();
}

/*!
  \obsolete 

  Constructs a new QScriptValue with the special \a value and
  registers it with the script \a engine.
*/
QScriptValue::QScriptValue(QScriptEngine *engine, QScriptValue::SpecialValue value)
{
    if (engine) {
        QScriptEnginePrivate *eng_p = QScriptEnginePrivate::get(engine);
        d_ptr = eng_p->registerValue(QScriptValueImpl(value));
        d_ptr->ref.ref();
    } else {
        d_ptr = 0;
    }
}

/*!
  \obsolete

  \fn QScriptValue::QScriptValue(QScriptEngine *engine, bool value)

  Constructs a new QScriptValue with the boolean \a value and
  registers it with the script \a engine.
*/
QScriptValue::QScriptValue(QScriptEngine *engine, bool val)
{
    if (engine) {
        QScriptEnginePrivate *eng_p = QScriptEnginePrivate::get(engine);
        d_ptr = eng_p->registerValue(QScriptValueImpl(val));
        d_ptr->ref.ref();
    } else {
        d_ptr = 0;
    }
}

/*!
  \fn QScriptValue::QScriptValue(QScriptEngine *engine, int value)
  \obsolete

  Constructs a new QScriptValue with the integer \a value and
  registers it with the script \a engine.
*/
QScriptValue::QScriptValue(QScriptEngine *engine, int val)
{
    if (engine) {
        QScriptEnginePrivate *eng_p = QScriptEnginePrivate::get(engine);
        d_ptr = eng_p->registerValue(QScriptValueImpl(val));
        d_ptr->ref.ref();
    } else {
        d_ptr = 0;
    }
}

/*!
  \fn QScriptValue::QScriptValue(QScriptEngine *engine, uint value)
  \obsolete

  Constructs a new QScriptValue with the unsigned integer \a value and
  registers it with the script \a engine.
 */
QScriptValue::QScriptValue(QScriptEngine *engine, uint val)
{
    if (engine) {
        QScriptEnginePrivate *eng_p = QScriptEnginePrivate::get(engine);
        d_ptr = eng_p->registerValue(QScriptValueImpl(val));
        d_ptr->ref.ref();
    } else {
        d_ptr = 0;
    }
}

/*!
  \fn QScriptValue::QScriptValue(QScriptEngine *engine, qsreal value)
  \obsolete

  Constructs a new QScriptValue with the qsreal \a value and
  registers it with the script \a engine.
*/
QScriptValue::QScriptValue(QScriptEngine *engine, qsreal val)
{
    if (engine) {
        QScriptEnginePrivate *eng_p = QScriptEnginePrivate::get(engine);
        d_ptr = eng_p->registerValue(QScriptValueImpl(val));
        d_ptr->ref.ref();
    } else {
        d_ptr = 0;
    }
}

/*!
  \fn QScriptValue::QScriptValue(QScriptEngine *engine, const QString &value)
  \obsolete

  Constructs a new QScriptValue with the string \a value and
  registers it with the script \a engine.
*/
QScriptValue::QScriptValue(QScriptEngine *engine, const QString &val)
{
    if (engine) {
        QScriptValueImpl v;
        QScriptEnginePrivate *eng_p = QScriptEnginePrivate::get(engine);
        eng_p->newString(&v, val);
        d_ptr = eng_p->registerValue(v);
        d_ptr->ref.ref();
    } else {
        d_ptr = 0;
    }
}

/*!
  \fn QScriptValue::QScriptValue(QScriptEngine *engine, const char *value)
  \obsolete

  Constructs a new QScriptValue with the string \a value and
  registers it with the script \a engine.
*/

#ifndef QT_NO_CAST_FROM_ASCII
QScriptValue::QScriptValue(QScriptEngine *engine, const char *val)
{
    if (engine) {
        QScriptValueImpl v;
        QScriptEnginePrivate *eng_p = QScriptEnginePrivate::get(engine);
        eng_p->newString(&v, QString::fromAscii(val));
        d_ptr = eng_p->registerValue(v);
        d_ptr->ref.ref();
    } else {
        d_ptr = 0;
    }
}
#endif

/*!
  \since 4.5

  Constructs a new QScriptValue with a special \a value.
*/
QScriptValue::QScriptValue(SpecialValue value)
    : d_ptr(new QScriptValuePrivate)
{
    d_ptr->value = QScriptValueImpl(value);
    d_ptr->ref.ref();
}

/*!
  \since 4.5

  Constructs a new QScriptValue with a boolean \a value.
*/
QScriptValue::QScriptValue(bool value)
    : d_ptr(new QScriptValuePrivate)
{
    d_ptr->value = QScriptValueImpl(value);
    d_ptr->ref.ref();
}

/*!
  \since 4.5

  Constructs a new QScriptValue with a number \a value.
*/
QScriptValue::QScriptValue(int value)
    : d_ptr(new QScriptValuePrivate)
{
    d_ptr->value = QScriptValueImpl(value);
    d_ptr->ref.ref();
}

/*!
  \since 4.5

  Constructs a new QScriptValue with a number \a value.
*/
QScriptValue::QScriptValue(uint value)
    : d_ptr(new QScriptValuePrivate)
{
    d_ptr->value = QScriptValueImpl(value);
    d_ptr->ref.ref();
}

/*!
  \since 4.5

  Constructs a new QScriptValue with a number \a value.
*/
QScriptValue::QScriptValue(qsreal value)
    : d_ptr(new QScriptValuePrivate)
{
    d_ptr->value = QScriptValueImpl(value);
    d_ptr->ref.ref();
}

/*!
  \since 4.5

  Constructs a new QScriptValue with a string \a value.
*/
QScriptValue::QScriptValue(const QString &value)
    : d_ptr(new QScriptValuePrivate)
{
    d_ptr->value.m_type = QScript::LazyStringType;
    d_ptr->value.m_lazy_string_value = new QString(value);
    d_ptr->ref.ref();
}

/*!
  \since 4.5

  Constructs a new QScriptValue with a string \a value.
*/
QScriptValue::QScriptValue(const QLatin1String &value)
    : d_ptr(new QScriptValuePrivate)
{
    d_ptr->value.m_type = QScript::LazyStringType;
    d_ptr->value.m_lazy_string_value = new QString(value);
    d_ptr->ref.ref();
}

/*!
  \since 4.5

  Constructs a new QScriptValue with a string \a value.
*/

#ifndef QT_NO_CAST_FROM_ASCII
QScriptValue::QScriptValue(const char *value)
    : d_ptr(new QScriptValuePrivate)
{
    d_ptr->value.m_type = QScript::LazyStringType;
    d_ptr->value.m_lazy_string_value = new QString(QString::fromAscii(value));
    d_ptr->ref.ref();
}
#endif

/*!
  Assigns the \a other value to this QScriptValue.

  Note that if \a other is an object (isObject() returns true),
  only a reference to the underlying object will be assigned;
  the object itself will not be copied.
*/
QScriptValue &QScriptValue::operator=(const QScriptValue &other)
{
    if (d_ptr == other.d_ptr)
        return *this;
    if (d_ptr && !d_ptr->ref.deref()) {
        if (engine()) {
            QScriptEnginePrivate::get(engine())->unregisterValue(d_ptr);
        } else {
            delete d_ptr;
        }
    }
    d_ptr = other.d_ptr;
    if (d_ptr)
        d_ptr->ref.ref();
    return *this;
}

/*!
  Returns true if this QScriptValue is an object of the Error class;
  otherwise returns false.

  \sa QScriptContext::throwError()
*/
bool QScriptValue::isError() const
{
    Q_D(const QScriptValue);
    return d && d->value.isError();
}

/*!
  Returns true if this QScriptValue is an object of the Array class;
  otherwise returns false.

  \sa QScriptEngine::newArray()
*/
bool QScriptValue::isArray() const
{
    Q_D(const QScriptValue);
    return d && d->value.isArray();
}

/*!
  Returns true if this QScriptValue is an object of the Date class;
  otherwise returns false.

  \sa QScriptEngine::newDate()
*/
bool QScriptValue::isDate() const
{
    Q_D(const QScriptValue);
    return d && d->value.isDate();
}

/*!
  Returns true if this QScriptValue is an object of the RegExp class;
  otherwise returns false.

  \sa QScriptEngine::newRegExp()
*/
bool QScriptValue::isRegExp() const
{
    Q_D(const QScriptValue);
    return d && d->value.isRegExp();
}

/*!
  If this QScriptValue is an object, returns the internal prototype
  (\c{__proto__} property) of this object; otherwise returns an
  invalid QScriptValue.

  \sa setPrototype(), isObject()
*/
QScriptValue QScriptValue::prototype() const
{
    Q_D(const QScriptValue);
    if (!d || !d->value.isObject())
        return QScriptValue();
    QScriptEnginePrivate *eng = QScriptEnginePrivate::get(engine());
    return eng->toPublic(d->value.prototype());
}

/*!
  If this QScriptValue is an object, sets the internal prototype
  (\c{__proto__} property) of this object to be \a prototype;
  otherwise does nothing.

  The internal prototype should not be confused with the public
  property with name "prototype"; the public prototype is usually
  only set on functions that act as constructors.

  \sa prototype(), isObject()
*/
void QScriptValue::setPrototype(const QScriptValue &prototype)
{
    Q_D(QScriptValue);
    if (!d || !d->value.isObject())
        return;
    if (prototype.isValid() && prototype.engine()
        && (prototype.engine() != engine())) {
        qWarning("QScriptValue::setPrototype() failed: "
                 "cannot set a prototype created in "
                 "a different engine");
        return;
    }
    QScriptValueImpl was = d->value.prototype();
    d->value.setPrototype(d->value.engine()->toImpl(prototype));
    if (d->value.detectedCycle()) {
        qWarning("QScriptValue::setPrototype() failed: "
                 "cyclic prototype value");
        d->value.setPrototype(was);
    }
}

/*!
  Returns the scope object of this QScriptValue. This function is only
  relevant for function objects. The scope determines how variables are
  resolved when the function is invoked.
*/
QScriptValue QScriptValue::scope() const
{
    Q_D(const QScriptValue);
    if (!d || !d->value.isObject())
        return QScriptValue();
    QScriptEnginePrivate *eng = QScriptEnginePrivate::get(engine());
    return eng->toPublic(d->value.scope());
}

/*!
  Sets the \a scope object of this QScriptValue. This function is only
  relevant for function objects. Changing the scope is useful when creating
  closures; see \l{Nested Functions and the Scope Chain}.
*/
void QScriptValue::setScope(const QScriptValue &scope)
{
    Q_D(QScriptValue);
    if (!d || !d->value.isObject())
        return;
    if (scope.isValid() && scope.engine()
        && (scope.engine() != engine())) {
        qWarning("QScriptValue::setScope() failed: "
                 "cannot set a scope object created in "
                 "a different engine");
        return;
    }
    d->value.setScope(d->value.engine()->toImpl(scope));
}

/*!
  Returns true if this QScriptValue is an instance of
  \a other; otherwise returns false.

  This QScriptValue is considered to be an instance of \a other if
  \a other is a function and the value of the \c{prototype}
  property of \a other is in the prototype chain of this
  QScriptValue.
*/
bool QScriptValue::instanceOf(const QScriptValue &other) const
{
    Q_D(const QScriptValue);
    if (!isObject() || !other.isObject())
        return false;
    if (other.engine() != engine()) {
        qWarning("QScriptValue::instanceof: "
                 "cannot perform operation on a value created in "
                 "a different engine");
        return false;
    }
    return d->value.engine()->toImpl(*this)
        .instanceOf(d->value.engine()->toImpl(other));
}

/*!
  Returns true if this QScriptValue is less than \a other, otherwise
  returns false.  The comparison follows the behavior described in
  \l{ECMA-262} section 11.8.5, "The Abstract Relational Comparison
  Algorithm".

  Note that if this QScriptValue or the \a other value are objects,
  calling this function has side effects on the script engine, since
  the engine will call the object's valueOf() function (and possibly
  toString()) in an attempt to convert the object to a primitive value
  (possibly resulting in an uncaught script exception).

  \sa equals()
*/
bool QScriptValue::lessThan(const QScriptValue &other) const
{
    if (!isValid() || !other.isValid())
        return false;
    if (other.engine() && engine() && (other.engine() != engine())) {
        qWarning("QScriptValue::lessThan: "
                 "cannot compare to a value created in "
                 "a different engine");
        return false;
    }
    return QScriptEnginePrivate::lessThan(QScriptValuePrivate::valueOf(*this),
                                          QScriptValuePrivate::valueOf(other));
}

/*!
  Returns true if this QScriptValue is equal to \a other, otherwise
  returns false. The comparison follows the behavior described in
  \l{ECMA-262} section 11.9.3, "The Abstract Equality Comparison
  Algorithm".

  This function can return true even if the type of this QScriptValue
  is different from the type of the \a other value; i.e. the
  comparison is not strict.  For example, comparing the number 9 to
  the string "9" returns true; comparing an undefined value to a null
  value returns true; comparing a \c{Number} object whose primitive
  value is 6 to a \c{String} object whose primitive value is "6"
  returns true; and comparing the number 1 to the boolean value
  \c{true} returns true. If you want to perform a comparison
  without such implicit value conversion, use strictlyEquals().

  Note that if this QScriptValue or the \a other value are objects,
  calling this function has side effects on the script engine, since
  the engine will call the object's valueOf() function (and possibly
  toString()) in an attempt to convert the object to a primitive value
  (possibly resulting in an uncaught script exception).

  \sa strictlyEquals(), lessThan()
*/
bool QScriptValue::equals(const QScriptValue &other) const
{
    if (!isValid() || !other.isValid())
        return isValid() == other.isValid();
    if (other.engine() && engine() && (other.engine() != engine())) {
        qWarning("QScriptValue::equals: "
                 "cannot compare to a value created in "
                 "a different engine");
        return false;
    }
    return QScriptEnginePrivate::equals(QScriptValuePrivate::valueOf(*this),
                                        QScriptValuePrivate::valueOf(other));
}

/*!
  Returns true if this QScriptValue is equal to \a other using strict
  comparison (no conversion), otherwise returns false. The comparison
  follows the behavior described in \l{ECMA-262} section 11.9.6, "The
  Strict Equality Comparison Algorithm".

  If the type of this QScriptValue is different from the type of the
  \a other value, this function returns false. If the types are equal,
  the result depends on the type, as shown in the following table:

    \table
    \header \o Type \o Result
    \row    \o Undefined  \o true
    \row    \o Null       \o true
    \row    \o Boolean    \o true if both values are true, false otherwise
    \row    \o Number     \o false if either value is NaN (Not-a-Number); true if values are equal, false otherwise
    \row    \o String     \o true if both values are exactly the same sequence of characters, false otherwise
    \row    \o Object     \o true if both values refer to the same object, false otherwise
    \endtable
  
  \sa equals()
*/
bool QScriptValue::strictlyEquals(const QScriptValue &other) const
{
    if (!isValid() || !other.isValid())
        return isValid() == other.isValid();
    if (other.engine() && engine() && (other.engine() != engine())) {
        qWarning("QScriptValue::strictlyEquals: "
                 "cannot compare to a value created in "
                 "a different engine");
        return false;
    }
    return QScriptEnginePrivate::strictlyEquals(QScriptValuePrivate::valueOf(*this),
                                                QScriptValuePrivate::valueOf(other));
}

/*!
  Returns the string value of this QScriptValue, as defined in
  \l{ECMA-262} section 9.8, "ToString".

  Note that if this QScriptValue is an object, calling this function
  has side effects on the script engine, since the engine will call
  the object's toString() function (and possibly valueOf()) in an
  attempt to convert the object to a primitive value (possibly
  resulting in an uncaught script exception).

  \sa isString()
*/
QString QScriptValue::toString() const
{
    Q_D(const QScriptValue);
    if (!d)
        return QString();
    return d->value.toString();
}

/*!
  Returns the number value of this QScriptValue, as defined in
  \l{ECMA-262} section 9.3, "ToNumber".

  Note that if this QScriptValue is an object, calling this function
  has side effects on the script engine, since the engine will call
  the object's valueOf() function (and possibly toString()) in an
  attempt to convert the object to a primitive value (possibly
  resulting in an uncaught script exception).

  \sa isNumber(), toInteger(), toInt32(), toUInt32(), toUInt16()
*/
qsreal QScriptValue::toNumber() const
{
    Q_D(const QScriptValue);
    if (!d)
        return 0;
    return d->value.toNumber();
}

/*!
  \obsolete

  Use toBool() instead.
*/
bool QScriptValue::toBoolean() const
{
    Q_D(const QScriptValue);
    if (!d)
        return false;
    return d->value.toBoolean();
}

/*!
  \since 4.5

  Returns the boolean value of this QScriptValue, using the conversion
  rules described in \l{ECMA-262} section 9.2, "ToBoolean".

  Note that if this QScriptValue is an object, calling this function
  has side effects on the script engine, since the engine will call
  the object's valueOf() function (and possibly toString()) in an
  attempt to convert the object to a primitive value (possibly
  resulting in an uncaught script exception).

  \sa isBool()
*/
bool QScriptValue::toBool() const
{
    Q_D(const QScriptValue);
    if (!d)
        return false;
    return d->value.toBoolean();
}

/*!
  Returns the signed 32-bit integer value of this QScriptValue, using
  the conversion rules described in \l{ECMA-262} section 9.5, "ToInt32".

  Note that if this QScriptValue is an object, calling this function
  has side effects on the script engine, since the engine will call
  the object's valueOf() function (and possibly toString()) in an
  attempt to convert the object to a primitive value (possibly
  resulting in an uncaught script exception).

  \sa toNumber(), toUInt32()
*/
qint32 QScriptValue::toInt32() const
{
    Q_D(const QScriptValue);
    if (!d)
        return 0;
    return d->value.toInt32();
}

/*!
  Returns the unsigned 32-bit integer value of this QScriptValue, using
  the conversion rules described in \l{ECMA-262} section 9.6, "ToUint32".

  Note that if this QScriptValue is an object, calling this function
  has side effects on the script engine, since the engine will call
  the object's valueOf() function (and possibly toString()) in an
  attempt to convert the object to a primitive value (possibly
  resulting in an uncaught script exception).

  \sa toNumber(), toInt32()
*/
quint32 QScriptValue::toUInt32() const
{
    Q_D(const QScriptValue);
    if (!d)
        return 0;
    return d->value.toUInt32();
}

/*!
  Returns the unsigned 16-bit integer value of this QScriptValue, using
  the conversion rules described in \l{ECMA-262} section 9.7, "ToUint16".

  Note that if this QScriptValue is an object, calling this function
  has side effects on the script engine, since the engine will call
  the object's valueOf() function (and possibly toString()) in an
  attempt to convert the object to a primitive value (possibly
  resulting in an uncaught script exception).

  \sa toNumber()
*/
quint16 QScriptValue::toUInt16() const
{
    Q_D(const QScriptValue);
    if (!d)
        return 0;
    return d->value.toUInt16();
}

/*!
  Returns the integer value of this QScriptValue, using the conversion
  rules described in \l{ECMA-262} section 9.4, "ToInteger".

  Note that if this QScriptValue is an object, calling this function
  has side effects on the script engine, since the engine will call
  the object's valueOf() function (and possibly toString()) in an
  attempt to convert the object to a primitive value (possibly
  resulting in an uncaught script exception).

  \sa toNumber()
*/
qsreal QScriptValue::toInteger() const
{
    Q_D(const QScriptValue);
    if (!d)
        return 0;
    return d->value.toInteger();
}

/*!
  Returns the QVariant value of this QScriptValue, if it can be
  converted to a QVariant; otherwise returns an invalid QVariant.
  The conversion is performed according to the following table:

    \table
    \header \o Input Type \o Result
    \row    \o Undefined  \o An invalid QVariant.
    \row    \o Null       \o An invalid QVariant.
    \row    \o Boolean    \o A QVariant containing the value of the boolean.
    \row    \o Number     \o A QVariant containing the value of the number.
    \row    \o String     \o A QVariant containing the value of the string.
    \row    \o QVariant Object \o The result is the QVariant value of the object (no conversion).
    \row    \o QObject Object \o A QVariant containing a pointer to the QObject.
    \row    \o Date Object \o A QVariant containing the date value (toDateTime()).
    \row    \o RegExp Object \o A QVariant containing the regular expression value (toRegExp()).
    \row    \o Object     \o If the value is primitive, then the result is converted to a QVariant according to the above rules; otherwise, an invalid QVariant is returned.
    \endtable

  \sa isVariant()
*/
QVariant QScriptValue::toVariant() const
{
    Q_D(const QScriptValue);
    if (!d)
        return QVariant();
    return d->value.toVariant();
}

/*!
  \obsolete

  This function is obsolete; use QScriptEngine::toObject() instead.
*/
QScriptValue QScriptValue::toObject() const
{
    Q_D(const QScriptValue);
    if (!d)
        return QScriptValue();
    QScriptEnginePrivate *eng = QScriptEnginePrivate::get(engine());
    if (!eng)
        return QScriptValue();
    return eng->toPublic(eng->toObject(d->value));
}

/*!
  Returns a QDateTime representation of this value, in local time.
  If this QScriptValue is not a date, or the value of the date is NaN
  (Not-a-Number), an invalid QDateTime is returned.

  \sa isDate()
*/
QDateTime QScriptValue::toDateTime() const
{
    Q_D(const QScriptValue);
    if (!d)
        return QDateTime();
    return d->value.toDateTime();
}

#ifndef QT_NO_REGEXP
/*!
  Returns the QRegExp representation of this value.
  If this QScriptValue is not a regular expression, an empty
  QRegExp is returned.

  \sa isRegExp()
*/
QRegExp QScriptValue::toRegExp() const
{
    Q_D(const QScriptValue);
    if (!d)
        return QRegExp();
    return d->value.toRegExp();
}
#endif // QT_NO_REGEXP

/*!
  If this QScriptValue is a QObject, returns the QObject pointer
  that the QScriptValue represents; otherwise, returns 0.

  If the QObject that this QScriptValue wraps has been deleted,
  this function returns 0 (i.e. it is possible for toQObject()
  to return 0 even when isQObject() returns true).

  \sa isQObject()
*/
QObject *QScriptValue::toQObject() const
{
    Q_D(const QScriptValue);
    if (!d)
        return 0;
    return d->value.toQObject();
}

/*!
  If this QScriptValue is a QMetaObject, returns the QMetaObject pointer
  that the QScriptValue represents; otherwise, returns 0.

  \sa isQMetaObject()
*/
const QMetaObject *QScriptValue::toQMetaObject() const
{
    Q_D(const QScriptValue);
    if (!d)
        return 0;
    return d->value.toQMetaObject();
}

/*!
  Sets the value of this QScriptValue's property with the given \a name to
  the given \a value.

  If this QScriptValue is not an object, this function does nothing.

  If this QScriptValue does not already have a property with name \a name,
  a new property is created; the given \a flags then specify how this
  property may be accessed by script code.

  If \a value is invalid, the property is removed.

  If the property is implemented using a setter function (i.e. has the
  PropertySetter flag set), calling setProperty() has side-effects on
  the script engine, since the setter function will be called with the
  given \a value as argument (possibly resulting in an uncaught script
  exception).

  Note that you cannot specify custom getter or setter functions for
  built-in properties, such as the \c{length} property of Array objects
  or meta properties of QObject objects.

  \sa property()
*/
void QScriptValue::setProperty(const QString &name, const QScriptValue &value,
                               const PropertyFlags &flags)
{
    Q_D(QScriptValue);
    if (!d || !d->value.isObject())
        return;
    if (value.engine() && (value.engine() != engine())) {
        qWarning("QScriptValue::setProperty(%s) failed: "
                 "cannot set value created in a different engine",
                 qPrintable(name));
        return;
    }
    d->value.setProperty(name, d->value.engine()->toImpl(value), flags);
}

/*!
  Returns the value of this QScriptValue's property with the given \a name,
  using the given \a mode to resolve the property.

  If no such property exists, an invalid QScriptValue is returned.

  If the property is implemented using a getter function (i.e. has the
  PropertyGetter flag set), calling property() has side-effects on the
  script engine, since the getter function will be called (possibly
  resulting in an uncaught script exception). If an exception
  occurred, property() returns the value that was thrown (typically
  an \c{Error} object).

  \sa setProperty(), propertyFlags(), QScriptValueIterator
*/
QScriptValue QScriptValue::property(const QString &name,
                                    const ResolveFlags &mode) const
{
    Q_D(const QScriptValue);
    if (!d || !d->value.isObject())
        return QScriptValue();
    QScriptEnginePrivate *eng = QScriptEnginePrivate::get(engine());
    return eng->toPublic(d->value.property(name, mode));
}

/*!
  \overload

  Returns the property at the given \a arrayIndex, using the given \a
  mode to resolve the property.

  This function is provided for convenience and performance when
  working with array objects.

  If this QScriptValue is not an Array object, this function behaves
  as if property() was called with the string representation of \a
  arrayIndex.
*/
QScriptValue QScriptValue::property(quint32 arrayIndex,
                                    const ResolveFlags &mode) const
{
    Q_D(const QScriptValue);
    if (!d || !d->value.isObject())
        return QScriptValue();
    QScriptEnginePrivate *eng = QScriptEnginePrivate::get(engine());
    return eng->toPublic(d->value.property(arrayIndex, mode));
}

/*!
  \overload

  Sets the property at the given \a arrayIndex to the given \a value.

  This function is provided for convenience and performance when
  working with array objects.

  If this QScriptValue is not an Array object, this function behaves
  as if setProperty() was called with the string representation of \a
  arrayIndex.
*/
void QScriptValue::setProperty(quint32 arrayIndex, const QScriptValue &value,
                               const PropertyFlags &flags)
{
    Q_D(QScriptValue);
    if (!d || !d->value.isObject())
        return;
    if (value.engine() && (value.engine() != engine())) {
        qWarning("QScriptValue::setProperty() failed: "
                 "cannot set value created in a different engine");
        return;
    }
    d->value.setProperty(arrayIndex, d->value.engine()->toImpl(value), flags);
}

/*! 
  \since 4.4

  Returns the value of this QScriptValue's property with the given \a name,
  using the given \a mode to resolve the property.

  This overload of property() is useful when you need to look up the
  same property repeatedly, since the lookup can be performed faster
  when the name is represented as an interned string.

  \sa QScriptEngine::toStringHandle(), setProperty()
*/
QScriptValue QScriptValue::property(const QScriptString &name,
                                    const ResolveFlags &mode) const
{
    Q_D(const QScriptValue);
    if (!d || !d->value.isObject())
        return QScriptValue();
    if (!name.isValid())
        return QScriptValue();
    QScriptStringPrivate *s = QScriptStringPrivate::get(name);
    QScriptEnginePrivate *eng = QScriptEnginePrivate::get(engine());
    return eng->toPublic(d->value.property(s->nameId, mode));
}

/*!
  \since 4.4

  Sets the value of this QScriptValue's property with the given \a
  name to the given \a value. The given \a flags specify how this
  property may be accessed by script code.

  This overload of setProperty() is useful when you need to set the
  same property repeatedly, since the operation can be performed
  faster when the name is represented as an interned string.

  \sa QScriptEngine::toStringHandle()
*/
void QScriptValue::setProperty(const QScriptString &name,
                               const QScriptValue &value,
                               const PropertyFlags &flags)
{
    Q_D(QScriptValue);
    if (!d || !d->value.isObject() || !name.isValid())
        return;
    if (value.engine() && (value.engine() != engine())) {
        qWarning("QScriptValue::setProperty() failed: "
                 "cannot set value created in a different engine");
        return;
    }
    QScriptStringPrivate *s = QScriptStringPrivate::get(name);
    d->value.setProperty(s->nameId, d->value.engine()->toImpl(value), flags);
}

/*!
  Returns the flags of the property with the given \a name, using the
  given \a mode to resolve the property.

  \sa property()
*/
QScriptValue::PropertyFlags QScriptValue::propertyFlags(const QString &name,
                                                        const ResolveFlags &mode) const
{
    Q_D(const QScriptValue);
    if (!d)
        return 0;
    return d->value.propertyFlags(name, mode);
}

/*!
  \since 4.4

  Returns the flags of the property with the given \a name, using the
  given \a mode to resolve the property.

  \sa property()
*/
QScriptValue::PropertyFlags QScriptValue::propertyFlags(const QScriptString &name,
                                                        const ResolveFlags &mode) const
{
    Q_D(const QScriptValue);
    if (!d)
        return 0;
    if (!name.isValid())
        return 0;
    QScriptStringPrivate *s = QScriptStringPrivate::get(name);
    return d->value.propertyFlags(s->nameId, mode);
}

/*!
  Calls this QScriptValue as a function, using \a thisObject as
  the `this' object in the function call, and passing \a args
  as arguments to the function. Returns the value returned from
  the function.

  If this QScriptValue is not a function, call() does nothing
  and returns an invalid QScriptValue.

  Note that if \a thisObject is not an object, the global object
  (see \l{QScriptEngine::globalObject()}) will be used as the
  `this' object.

  Calling call() can cause an exception to occur in the script engine;
  in that case, call() returns the value that was thrown (typically an
  \c{Error} object). You can call
  QScriptEngine::hasUncaughtException() to determine if an exception
  occurred.

  \snippet doc/src/snippets/code/src_script_qscriptvalue.cpp 2

  \sa construct()
*/
QScriptValue QScriptValue::call(const QScriptValue &thisObject,
                                const QScriptValueList &args)
{
    Q_D(QScriptValue);
    if (!d || !d->value.isObject())
        return QScriptValue();
    if (isFunction() && thisObject.isValid() && thisObject.engine() &&
        engine() && (thisObject.engine() != engine())) {
        qWarning("QScriptValue::call() failed: "
                 "cannot call function with thisObject created in "
                 "a different engine");
        return QScriptValue();
    }
    QScriptEnginePrivate *eng = QScriptEnginePrivate::get(engine());
    return eng->toPublic(d->value.call(eng->toImpl(thisObject),
                                       eng->toImplList(args)));
}

/*!
  Calls this QScriptValue as a function, using \a thisObject as
  the `this' object in the function call, and passing \a arguments
  as arguments to the function. Returns the value returned from
  the function.

  If this QScriptValue is not a function, call() does nothing
  and returns an invalid QScriptValue.

  \a arguments can be an arguments object, an array, null or
  undefined; any other type will cause a TypeError to be thrown.

  Note that if \a thisObject is not an object, the global object
  (see \l{QScriptEngine::globalObject()}) will be used as the
  `this' object.

  One common usage of this function is to forward native function
  calls to another function:

  \snippet doc/src/snippets/code/src_script_qscriptvalue.cpp 3

  \sa construct(), QScriptContext::argumentsObject()
*/
QScriptValue QScriptValue::call(const QScriptValue &thisObject,
                                const QScriptValue &arguments)
{
    Q_D(QScriptValue);
    if (!d || !d->value.isObject())
        return QScriptValue();
    if (isFunction() && thisObject.isValid() && thisObject.engine()
        && (thisObject.engine() != engine())) {
        qWarning("QScriptValue::call() failed: "
                 "cannot call function with thisObject created in "
                 "a different engine");
        return QScriptValue();
    }
    QScriptEnginePrivate *eng = QScriptEnginePrivate::get(engine());
    return eng->toPublic(d->value.call(eng->toImpl(thisObject),
                                       eng->toImpl(arguments)));
}

/*!
  Creates a new \c{Object} and calls this QScriptValue as a
  constructor, using the created object as the `this' object and
  passing \a args as arguments. If the return value from the
  constructor call is an object, then that object is returned;
  otherwise the default constructed object is returned.

  If this QScriptValue is not a function, construct() does nothing
  and returns an invalid QScriptValue.

  Calling construct() can cause an exception to occur in the script
  engine; in that case, construct() returns the value that was thrown
  (typically an \c{Error} object). You can call
  QScriptEngine::hasUncaughtException() to determine if an exception
  occurred.

  \sa call(), QScriptEngine::newObject()
*/
QScriptValue QScriptValue::construct(const QScriptValueList &args)
{
    Q_D(QScriptValue);
    if (!d || !d->value.isObject())
        return QScriptValue();
    QScriptEnginePrivate *eng = QScriptEnginePrivate::get(engine());
    return eng->toPublic(d->value.construct(eng->toImplList(args)));
}

/*!
  Creates a new \c{Object} and calls this QScriptValue as a
  constructor, using the created object as the `this' object and
  passing \a arguments as arguments. If the return value from the
  constructor call is an object, then that object is returned;
  otherwise the default constructed object is returned.

  If this QScriptValue is not a function, construct() does nothing
  and returns an invalid QScriptValue.

  \a arguments can be an arguments object, an array, null or
  undefined. Any other type will cause a TypeError to be thrown.

  \sa call(), QScriptEngine::newObject(), QScriptContext::argumentsObject()
*/
QScriptValue QScriptValue::construct(const QScriptValue &arguments)
{
    Q_D(QScriptValue);
    if (!d || !d->value.isObject())
        return QScriptValue();
    QScriptEnginePrivate *eng = QScriptEnginePrivate::get(engine());
    return eng->toPublic(d->value.construct(eng->toImpl(arguments)));
}

/*!
  Returns the QScriptEngine that created this QScriptValue,
  or 0 if this QScriptValue is invalid or the value is not
  associated with a particular engine.
*/
QScriptEngine *QScriptValue::engine() const
{
    Q_D(const QScriptValue);
    if (!d)
        return 0;
    return d->engine;
}

/*!
  \obsolete

  Use isBool() instead.
*/
bool QScriptValue::isBoolean() const
{
    Q_D(const QScriptValue);
    return d && d->value.isBoolean();
}

/*!
  \since 4.5

  Returns true if this QScriptValue is of the primitive type Boolean;
  otherwise returns false.

  \sa toBool()
*/
bool QScriptValue::isBool() const
{
    Q_D(const QScriptValue);
    return d && d->value.isBoolean();
}

/*!
  Returns true if this QScriptValue is of the primitive type Number;
  otherwise returns false.

  \sa toNumber()
*/
bool QScriptValue::isNumber() const
{
    Q_D(const QScriptValue);
    return d && d->value.isNumber();
}

/*!
  Returns true if this QScriptValue is of the primitive type String;
  otherwise returns false.

  \sa toString()
*/
bool QScriptValue::isString() const
{
    Q_D(const QScriptValue);
    return d && d->value.isString();
}

/*!
  Returns true if this QScriptValue is a function; otherwise returns
  false.

  \sa call()
*/
bool QScriptValue::isFunction() const
{
    Q_D(const QScriptValue);
    return d && d->value.isFunction();
}

/*!
  Returns true if this QScriptValue is of the primitive type Null;
  otherwise returns false.

  \sa QScriptEngine::nullValue()
*/
bool QScriptValue::isNull() const
{
    Q_D(const QScriptValue);
    return d && d->value.isNull();
}

/*!
  Returns true if this QScriptValue is of the primitive type Undefined;
  otherwise returns false.

  \sa QScriptEngine::undefinedValue()
*/
bool QScriptValue::isUndefined() const
{
    Q_D(const QScriptValue);
    return d && d->value.isUndefined();
}

/*!
  Returns true if this QScriptValue is of the Object type; otherwise
  returns false.

  Note that function values, variant values, and QObject values are
  objects, so this function returns true for such values.

  \sa toObject(), QScriptEngine::newObject()
*/
bool QScriptValue::isObject() const
{
    Q_D(const QScriptValue);
    return d && d->value.isObject();
}

/*!
  Returns true if this QScriptValue is a variant value;
  otherwise returns false.

  \sa toVariant(), QScriptEngine::newVariant()
*/
bool QScriptValue::isVariant() const
{
    Q_D(const QScriptValue);
    return d && d->value.isVariant();
}

/*!
  Returns true if this QScriptValue is a QObject; otherwise returns
  false.

  Note: This function returns true even if the QObject that this
  QScriptValue wraps has been deleted.

  \sa toQObject(), QScriptEngine::newQObject()
*/
bool QScriptValue::isQObject() const
{
    Q_D(const QScriptValue);
    return d && d->value.isQObject();
}

/*!
  Returns true if this QScriptValue is a QMetaObject; otherwise returns
  false.

  \sa toQMetaObject(), QScriptEngine::newQMetaObject()
*/
bool QScriptValue::isQMetaObject() const
{
    Q_D(const QScriptValue);
    return d && d->value.isQMetaObject();
}

/*!
  Returns true if this QScriptValue is valid; otherwise returns
  false.
*/
bool QScriptValue::isValid() const
{
    Q_D(const QScriptValue);
    return d && d->value.isValid();
}

/*!
  \since 4.4

  Returns the internal data of this QScriptValue object. QtScript uses
  this property to store the primitive value of Date, String, Number
  and Boolean objects. For other types of object, custom data may be
  stored using setData().
*/
QScriptValue QScriptValue::data() const
{
    Q_D(const QScriptValue);
    if (!d || !d->value.isObject())
        return QScriptValue();
    QScriptEnginePrivate *eng = QScriptEnginePrivate::get(engine());
    return eng->toPublic(d->value.internalValue());
}

/*!
  \since 4.4

  Sets the internal \a data of this QScriptValue object. You can use
  this function to set object-specific data that won't be directly
  accessible to scripts, but may be retrieved in C++ using the data()
  function.
*/
void QScriptValue::setData(const QScriptValue &data)
{
    Q_D(QScriptValue);
    if (!d || !d->value.isObject())
        return;
    QScriptEnginePrivate *eng = QScriptEnginePrivate::get(engine());
    d->value.setInternalValue(eng->toImpl(data));
}

/*!
  \since 4.4

  Returns the custom script class that this script object is an
  instance of, or 0 if the object is not of a custom class.

  \sa setScriptClass()
*/
QScriptClass *QScriptValue::scriptClass() const
{
    Q_D(const QScriptValue);
    if (!d || !d->value.isObject())
        return 0;
    QScriptClassInfo *info = d->value.classInfo();
    if ((info->type() & QScriptClassInfo::TypeMask) < QScriptClassInfo::CustomType)
        return 0;
    return QScriptClassPrivate::classFromInfo(info);
}

/*!
  \since 4.4

  Sets the custom script class of this script object to \a scriptClass.
  This can be used to "promote" a plain script object (e.g. created
  by the "new" operator in a script, or by QScriptEngine::newObject() in C++)
  to an object of a custom type.

  If \a scriptClass is 0, the object will be demoted to a plain
  script object.

  \sa scriptClass(), setData()
*/
void QScriptValue::setScriptClass(QScriptClass *scriptClass)
{
    Q_D(QScriptValue);
    if (!d || !d->value.isObject())
        return;
    if (!scriptClass) {
        QScriptEnginePrivate *eng_p = QScriptEnginePrivate::get(engine());
        d->value.setClassInfo(eng_p->m_class_object);
    } else {
        QScriptClassPrivate *cls_p = QScriptClassPrivate::get(scriptClass);
        d->value.setClassInfo(cls_p->classInfo());
    }
}

/*!
  \internal

  Returns the ID of this object, or -1 if this QScriptValue is not an
  object.

  \sa QScriptEngine::objectById()
*/
qint64 QScriptValue::objectId() const
{
    Q_D(const QScriptValue);
    if (!d || !d->value.isObject())
        return -1;
    return d->value.m_object_value->m_id;
}

QT_END_NAMESPACE

#endif // QT_NO_SCRIPT
