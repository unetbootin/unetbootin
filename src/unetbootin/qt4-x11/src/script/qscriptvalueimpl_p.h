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

#ifndef QSCRIPTVALUEIMPL_P_H
#define QSCRIPTVALUEIMPL_P_H

#include "qscriptvalueimplfwd_p.h"

#ifndef QT_NO_SCRIPT

#include "qscriptclassinfo_p.h"
#include "qscriptecmaarray_p.h"
#include "qscriptecmadate_p.h"
#include "qscriptecmaerror_p.h"
#include "qscriptecmaregexp_p.h"
#include "qscriptextqobject_p.h"
#include "qscriptextvariant_p.h"
#include "qscriptvaluefwd_p.h"
#include "qscriptnameid_p.h"
#include "qscriptenginefwd_p.h"
#include "qscriptcontextfwd_p.h"

#include <QtCore/QDateTime>

QT_BEGIN_NAMESPACE

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

inline QScriptValueImpl::QScriptValueImpl()
    : m_type(QScript::InvalidType) {}

inline QScriptValueImpl::QScriptValueImpl(QScriptValue::SpecialValue val)
{
    if (val == QScriptValue::NullValue)
        m_type = QScript::NullType;
    else if (val == QScriptValue::UndefinedValue)
        m_type = QScript::UndefinedType;
    else
        m_type = QScript::InvalidType;
}

inline QScriptValueImpl::QScriptValueImpl(bool val)
    : m_type(QScript::BooleanType), m_bool_value(val)
{
}

inline QScriptValueImpl::QScriptValueImpl(int val)
    : m_type(QScript::NumberType), m_number_value(val)
{
}

inline QScriptValueImpl::QScriptValueImpl(uint val)
    : m_type(QScript::NumberType), m_number_value(val)
{
}

inline QScriptValueImpl::QScriptValueImpl(qsreal val)
    : m_type(QScript::NumberType), m_number_value(val)
{
}

inline QScriptValueImpl::QScriptValueImpl(QScriptEnginePrivate *engine, const QString &val)
{
    engine->newString(this, val);
}

inline QScriptValueImpl::QScriptValueImpl(QScriptNameIdImpl *val)
    : m_type(QScript::StringType), m_string_value(val)
{
}

inline QScript::Type QScriptValueImpl::type() const
{
    return m_type;
}

inline QScriptEnginePrivate *QScriptValueImpl::engine() const
{
    if (!isObject())
        return 0;
    return m_object_value->m_class->engine();
}

inline QScriptClassInfo *QScriptValueImpl::classInfo() const
{
    if (!isObject())
        return 0;
    return m_object_value->m_class;
}

inline void QScriptValueImpl::setClassInfo(QScriptClassInfo *cls)
{
    Q_ASSERT(isObject());
    m_object_value->m_class = cls;
}

inline QScriptNameIdImpl *QScriptValueImpl::stringValue() const
{
    Q_ASSERT(isString());
    return m_string_value;
}

inline QScriptObject *QScriptValueImpl::objectValue() const
{
    Q_ASSERT(isObject());
    return m_object_value;
}

inline void QScriptValueImpl::incr()
{
    ++m_number_value;
}

inline void QScriptValueImpl::decr()
{
    --m_number_value;
}

inline void QScriptValueImpl::invalidate()
{
    m_type = QScript::InvalidType;
}

inline bool QScriptValueImpl::isValid() const
{
    return m_type != QScript::InvalidType;
}

inline bool QScriptValueImpl::isUndefined() const
{
    return (m_type == QScript::UndefinedType);
}

inline bool QScriptValueImpl::isNull() const
{
    return (m_type == QScript::NullType);
}

inline bool QScriptValueImpl::isBoolean() const
{
    return (m_type == QScript::BooleanType);
}

inline bool QScriptValueImpl::isNumber() const
{
    return (m_type == QScript::NumberType);
}

inline bool QScriptValueImpl::isString() const
{
    return (m_type == QScript::StringType)
        || (m_type == QScript::LazyStringType);
}

inline bool QScriptValueImpl::isReference() const
{
    return (m_type == QScript::ReferenceType);
}

inline bool QScriptValueImpl::isObject() const
{
    return (m_type == QScript::ObjectType);
}

inline bool QScriptValueImpl::isFunction() const
{
    return (m_type == QScript::ObjectType)
        && (classInfo()->type() & QScriptClassInfo::FunctionBased);
}

inline bool QScriptValueImpl::isVariant() const
{
    return (m_type == QScript::ObjectType)
        && (classInfo()->type() == QScriptClassInfo::VariantType);
}

inline bool QScriptValueImpl::isQObject() const
{
    return (m_type == QScript::ObjectType)
        && (classInfo()->type() == QScriptClassInfo::QObjectType);
}

inline bool QScriptValueImpl::isQMetaObject() const
{
    return (m_type == QScript::ObjectType)
        && (classInfo()->type() == QScriptClassInfo::QMetaObjectType);
}

inline bool QScriptValueImpl::isArray() const
{
    if (!isObject())
        return false;
    return classInfo() == engine()->arrayConstructor->classInfo();
}

inline bool QScriptValueImpl::isDate() const
{
    if (!isObject())
        return false;
    return classInfo() == engine()->dateConstructor->classInfo();
}

inline bool QScriptValueImpl::isError() const
{
    if (!isObject())
        return false;
    return classInfo() == engine()->errorConstructor->classInfo();
}

inline bool QScriptValueImpl::isRegExp() const
{
    if (!isObject())
        return false;
    return classInfo() == engine()->regexpConstructor->classInfo();
}

inline qsreal QScriptValueImpl::toNumber() const
{
    if (!isValid())
        return 0;
    return QScriptEnginePrivate::convertToNativeDouble(*this);
}

inline bool QScriptValueImpl::toBoolean() const
{
    if (!isValid())
        return false;
    return QScriptEnginePrivate::convertToNativeBoolean(*this);
}

inline QString QScriptValueImpl::toString() const
{
    if (!isValid())
        return QString();
    return QScriptEnginePrivate::convertToNativeString(*this);
}

inline qint32 QScriptValueImpl::toInt32() const
{
    if (!isValid())
        return 0;
    double d = QScriptEnginePrivate::convertToNativeDouble(*this);
    return QScriptEnginePrivate::toInt32(d);
}

inline quint32 QScriptValueImpl::toUInt32() const
{
    if (!isValid())
        return 0;
    double d = QScriptEnginePrivate::convertToNativeDouble(*this);
    return QScriptEnginePrivate::toUint32(d);
}

inline quint16 QScriptValueImpl::toUInt16() const
{
    if (!isValid())
        return 0;
    double d = QScriptEnginePrivate::convertToNativeDouble(*this);
    return QScriptEnginePrivate::toUint16(d);
}

inline qsreal QScriptValueImpl::toInteger() const
{
    if (!isValid())
        return 0;
    double d = QScriptEnginePrivate::convertToNativeDouble(*this);
    return QScriptEnginePrivate::toInteger(d);
}

inline QDateTime QScriptValueImpl::toDateTime() const
{
    if (!isDate())
        return QDateTime();
    return engine()->toDateTime(*this);
}

#ifndef QT_NO_REGEXP
inline QRegExp QScriptValueImpl::toRegExp() const
{
    if (!isRegExp())
        return QRegExp();
    return engine()->regexpConstructor->toRegExp(*this);
}
#endif // QT_NO_REGEXP

inline QObject *QScriptValueImpl::toQObject() const
{
#ifndef QT_NO_QOBJECT
    if (isQObject()) {
        QScript::ExtQObject *ctor = engine()->qobjectConstructor;
        Q_ASSERT(ctor != 0);

        QScript::ExtQObject::Instance *data = ctor->get(*this);
        Q_ASSERT(data != 0);

        return data->value;
    } else if (isVariant()) {
        int type = variantValue().userType();
        if ((type == QMetaType::QObjectStar) || (type == QMetaType::QWidgetStar))
            return *reinterpret_cast<QObject* const *>(variantValue().constData());
    }
#endif
    return 0;
}

inline const QMetaObject *QScriptValueImpl::toQMetaObject() const
{
#ifndef QT_NO_QOBJECT
    if (isQMetaObject()) {
        QScript::ExtQMetaObject *ctor = engine()->qmetaObjectConstructor;
        Q_ASSERT(ctor != 0);

        QScript::ExtQMetaObject::Instance *data = ctor->get(*this);
        Q_ASSERT(data != 0);

        return data->value;
    }
#endif
    return 0;
}

inline QScriptValueImpl QScriptValueImpl::prototype() const
{
    if (!isObject())
        return QScriptValueImpl();
    return m_object_value->m_prototype;
}

inline void QScriptValueImpl::setPrototype(const QScriptValueImpl &prototype)
{
    if (isObject())
        m_object_value->m_prototype = prototype;
}

inline QScriptObjectData *QScriptValueImpl::objectData() const
{
    Q_ASSERT(isObject());
    return m_object_value->m_data;
}

inline void QScriptValueImpl::setObjectData(QScriptObjectData *data)
{
    Q_ASSERT(isObject());
    m_object_value->m_data = data;
}

inline bool QScriptValueImpl::resolve(QScriptNameIdImpl *nameId, QScript::Member *member,
                                      QScriptValueImpl *object, QScriptValue::ResolveFlags mode,
                                      QScript::AccessMode access) const
{
    Q_ASSERT(isValid());
    Q_ASSERT(isObject());
    Q_ASSERT(member);
    Q_ASSERT(object);

    Q_ASSERT(nameId->unique);

    QScriptObject *object_data = m_object_value;

    // Search in properties...
    if (object_data->findMember(nameId, member)) {
        *object = *this;
        return true;
    }

    return resolve_helper(nameId, member, object, mode, access);
}

inline void QScriptValueImpl::get(const QScript::Member &member, QScriptValueImpl *out) const
{
    Q_ASSERT(out);
    Q_ASSERT(isObject());
    Q_ASSERT(member.isValid());

    if (! member.isObjectProperty()) {
        get_helper(member, out);
        return;
    }

    Q_ASSERT(member.id() >= 0);
    Q_ASSERT(member.id() < m_object_value->memberCount());

    m_object_value->get(member, out);
}

inline void QScriptValueImpl::get(QScriptNameIdImpl *nameId, QScriptValueImpl *out)
{
    QScript::Member m;
    QScriptValueImpl o;
    if (resolve(nameId, &m, &o, QScriptValue::ResolvePrototype, QScript::Read))
        o.get(m, out);
    else
        *out = QScriptValueImpl(QScriptValue::UndefinedValue);
}

inline void QScriptValueImpl::get_helper(const QScript::Member &member, QScriptValueImpl *out) const
{
    if (member.nameId() == engine()->idTable()->id___proto__) {
        *out = prototype();

        if (!out->isValid())
            *out = QScriptValueImpl(QScriptValue::UndefinedValue);

        return;
    }

    if (QScriptClassData *data = classInfo()->data()) {
        if (data->get(*this, member, out))
            return;
    }

    out->invalidate();
}

inline void QScriptValueImpl::put(const QScript::Member &member, const QScriptValueImpl &value)
{
    Q_ASSERT(isObject());
    Q_ASSERT(member.isValid());
    // Q_ASSERT(member.isWritable());

    QScriptEnginePrivate *eng_p = engine();

    if (member.isObjectProperty()) {
        Q_ASSERT(member.nameId()->unique);
        Q_ASSERT(member.id() >= 0);
        Q_ASSERT(member.id() < m_object_value->memberCount());
        m_object_value->put(member, value);
    }

    else if (member.nameId() == eng_p->idTable()->id___proto__) {
        if (value.isNull()) // only Object.prototype.__proto__ can be null
            setPrototype(eng_p->undefinedValue());
        else {
            QScriptValueImpl was = prototype();
            setPrototype(value);
            if (detectedCycle()) {
                eng_p->currentContext()->throwError(QLatin1String("cycle in prototype chain"));
                setPrototype(was);
            }
        }
    }

    else {
        Q_ASSERT(classInfo()->data());
        classInfo()->data()->put(this, member, value);
    }
}

inline void QScriptValueImpl::setQObjectValue(QObject *object)
{
#ifndef QT_NO_QOBJECT
    Q_ASSERT(isQObject());

    QScript::ExtQObject *ctor = engine()->qobjectConstructor;
    Q_ASSERT(ctor != 0);

    QScript::ExtQObject::Instance *data = ctor->get(*this);
    Q_ASSERT(data != 0);

    data->value = object;
#else
    Q_UNUSED(object);
#endif
}

inline QVariant &QScriptValueImpl::variantValue() const
{
    Q_ASSERT(isVariant());

    QScript::Ext::Variant *ctor = engine()->variantConstructor;
    Q_ASSERT(ctor != 0);

    QScript::Ext::Variant::Instance *data = ctor->get(*this);
    Q_ASSERT(data != 0);

    return data->value;
}

inline void QScriptValueImpl::setVariantValue(const QVariant &value)
{
    if (!isVariant())
        return;

    QScript::Ext::Variant *ctor = engine()->variantConstructor;
    Q_ASSERT(ctor != 0);

    QScript::Ext::Variant::Instance *data = ctor->get(*this);
    Q_ASSERT(data != 0);

    data->value = value;
}

inline QScriptValueImpl QScriptValueImpl::internalValue() const
{
    Q_ASSERT(isObject());
    return m_object_value->m_internalValue;
}

inline void QScriptValueImpl::setInternalValue(const QScriptValueImpl &internalValue)
{
    Q_ASSERT(isObject());
    m_object_value->m_internalValue = internalValue;
}

inline void QScriptValueImpl::removeMember(const QScript::Member &member)
{
    if (member.isObjectProperty())
        m_object_value->removeMember(member);

    else if (QScriptClassData *data = classInfo()->data())
        data->removeMember(*this, member);
}

inline void QScriptValueImpl::createMember(QScriptNameIdImpl *nameId,
                                    QScript::Member *member, uint flags)
{
    Q_ASSERT(isObject());

    QScriptObject *object_data = m_object_value;
    object_data->createMember(nameId, member, flags);
    Q_ASSERT(member->isObjectProperty());
}

inline QScriptValueImpl QScriptValueImpl::scope() const
{
    Q_ASSERT(isObject());
    return m_object_value->m_scope;
}

inline void QScriptValueImpl::setScope(const QScriptValueImpl &scope)
{
    Q_ASSERT(isObject());
    m_object_value->m_scope = scope;
}

inline int QScriptValueImpl::memberCount() const
{
    Q_ASSERT(isObject());
    return m_object_value->memberCount();
}

inline void QScriptValueImpl::member(int index, QScript::Member *member) const
{
    Q_ASSERT(isObject());
    Q_ASSERT(index >= 0);
    Q_ASSERT(index < m_object_value->memberCount());
    m_object_value->member(index, member);
}

inline QScriptFunction *QScriptValueImpl::toFunction() const
{
    if (!isFunction())
        return 0;
    return engine()->convertToNativeFunction(*this);
}

inline QScriptValueImpl QScriptValueImpl::property(QScriptNameIdImpl *nameId,
                                            const QScriptValue::ResolveFlags &mode) const
{
    if (!isObject())
        return QScriptValueImpl();

    QScriptValueImpl base;
    QScript::Member member;

    if (! resolve(nameId, &member, &base, mode, QScript::Read))
        return QScriptValueImpl();

    QScriptValueImpl value;
    base.get(member, &value);
    if (member.isGetterOrSetter()) {
        QScriptValueImpl getter;
        if (member.isObjectProperty() && !member.isGetter()) {
            if (!base.m_object_value->findGetter(&member))
                return QScriptValueImpl();
        }
        base.get(member, &getter);
        value = getter.call(*this);
    }
    return value;
}

inline void QScriptValueImpl::setProperty(const QString &name, const QScriptValueImpl &value,
                                   const QScriptValue::PropertyFlags &flags)
{
    if (!isObject())
        return;
    QScriptNameIdImpl *nameId = engine()->nameId(name);
    setProperty(nameId, value, flags);
}

inline QScriptValueImpl QScriptValueImpl::property(const QString &name,
                                            const QScriptValue::ResolveFlags &mode) const
{
    if (!isObject())
        return QScriptValueImpl();
    QScriptNameIdImpl *nameId = engine()->nameId(name);
    return property(nameId, mode);
}

inline QScriptValueImpl QScriptValueImpl::property(quint32 arrayIndex,
                                            const QScriptValue::ResolveFlags &mode) const
{
    if (!isObject())
        return QScriptValueImpl();

    QScriptEnginePrivate *eng_p = engine();
    QScript::Ecma::Array::Instance *instance = eng_p->arrayConstructor->get(*this);
    if (instance && (arrayIndex != 0xFFFFFFFF))
        return instance->value.at(arrayIndex);

    return property(QScriptValueImpl(arrayIndex).toString(), mode);
}

inline void QScriptValueImpl::setProperty(quint32 arrayIndex, const QScriptValueImpl &value,
                                   const QScriptValue::PropertyFlags &flags)
{
    if (!isObject())
        return;

    QScriptEnginePrivate *eng_p = engine();
    QScript::Ecma::Array::Instance *instance = eng_p->arrayConstructor->get(*this);
    if (instance && (arrayIndex != 0xFFFFFFFF)) {
        instance->value.assign(arrayIndex, value);
        return;
    }

    setProperty(QScriptValueImpl(arrayIndex).toString(), value, flags);
}

inline QScriptValue::PropertyFlags QScriptValueImpl::propertyFlags(const QString &name,
                                                const QScriptValue::ResolveFlags &mode) const
{
    QScriptNameIdImpl *nameId = engine()->nameId(name);
    return propertyFlags(nameId, mode);
}

inline QScriptValue::PropertyFlags QScriptValueImpl::propertyFlags(QScriptNameIdImpl *nameId,
                                                const QScriptValue::ResolveFlags &mode) const
{
    if (!isObject())
        return 0;

    QScriptValueImpl base;
    QScript::Member member;
    if (! resolve(nameId, &member, &base, mode, QScript::ReadWrite))
        return 0;

    return QScriptValue::PropertyFlags(member.flags() & ~QScript::Member::InternalRange);
}

inline bool QScriptValueImpl::deleteProperty(QScriptNameIdImpl *nameId,
                                             const QScriptValue::ResolveFlags &mode)
{
    if (!isObject())
        return true;
    QScript::Member member;
    QScriptValueImpl base;
    if (resolve(nameId, &member, &base, mode, QScript::Write)) {
        if (!member.isDeletable())
            return false;
        base.removeMember(member);
        if (member.isGetterOrSetter() && (member.isGetter() != member.isSetter())) {
            // delete the "other half" of the property too (getter or setter)
            return deleteProperty(nameId, mode);
        }
    }
    return true;
}

inline QScriptValueImpl QScriptValueImpl::call(const QScriptValueImpl &thisObject,
                                               const QScriptValueImplList &args)
{
    if (!isFunction())
        return QScriptValueImpl();

    return engine()->call(*this, thisObject, args, /*asConstructor=*/false);
}

inline QScriptValueImpl QScriptValueImpl::call(const QScriptValueImpl &thisObject,
                                               const QScriptValueImpl &args)
{
    if (!isFunction())
        return QScriptValueImpl();

    return engine()->call(*this, thisObject, args, /*asConstructor=*/false);
}

inline QScriptValueImpl QScriptValueImpl::construct(const QScriptValueImplList &args)
{
    if (!isFunction())
        return QScriptValueImpl();

    QScriptEnginePrivate *eng_p = engine();

    QScriptValueImpl proto = property(QLatin1String("prototype"), QScriptValue::ResolveLocal);
    QScriptValueImpl object;
    eng_p->newObject(&object, proto);

    QScriptValueImpl result = eng_p->call(*this, object, args, /*asConstructor=*/true);
    if (result.isObject())
        return result;
    return object;
}

inline QScriptValueImpl QScriptValueImpl::construct(const QScriptValueImpl &args)
{
    if (!isFunction())
        return QScriptValueImpl();

    QScriptEnginePrivate *eng_p = engine();

    QScriptValueImpl proto = property(QLatin1String("prototype"), QScriptValue::ResolveLocal);
    QScriptValueImpl object;
    eng_p->newObject(&object, proto);

    QScriptValueImpl result = eng_p->call(*this, object, args, /*asConstructor=*/true);
    if (result.isObject())
        return result;
    return object;
}

inline void QScriptValueImpl::mark(int generation) const
{
    if (! isValid())
        return;

    else if (isString())
        engine()->markString(m_string_value, generation);

    else if (isObject())
        engine()->markObject(*this, generation);
}

QT_END_NAMESPACE

#endif // QT_NO_SCRIPT

#endif
