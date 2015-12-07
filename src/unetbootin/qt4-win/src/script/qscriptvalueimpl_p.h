/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtScript module of the Qt Toolkit.
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
    : m_type(0) {}

inline QScriptValueImpl::QScriptValueImpl(QScriptEnginePrivate *engine, QScriptValue::SpecialValue val)
{
    if (val == QScriptValue::NullValue)
        engine->newNull(this);
    else if (val == QScriptValue::UndefinedValue)
        engine->newUndefined(this);
}

inline QScriptValueImpl::QScriptValueImpl(QScriptEnginePrivate *engine, bool val)
{
    engine->newBoolean(this, val);
}

inline QScriptValueImpl::QScriptValueImpl(QScriptEnginePrivate *engine, int val)
{
    engine->newNumber(this, val);
}

inline QScriptValueImpl::QScriptValueImpl(QScriptEnginePrivate *engine, uint val)
{
    engine->newNumber(this, val);
}

inline QScriptValueImpl::QScriptValueImpl(QScriptEnginePrivate *engine, qsreal val)
{
    engine->newNumber(this, val);
}

inline QScriptValueImpl::QScriptValueImpl(QScriptEnginePrivate *engine, const QString &val)
{
    engine->newString(this, val);
}

inline QScriptValueImpl::QScriptValueImpl(QScriptEnginePrivate *engine, QScriptNameIdImpl *val)
{
    engine->newNameId(this, val);
}

inline QScript::Type QScriptValueImpl::type() const
{
    Q_ASSERT(isValid());
    return m_type->type();
}

inline QScriptEngine *QScriptValueImpl::engine() const
{
    if (! m_type)
        return 0;
    return m_type->engine();
}

inline QScriptTypeInfo *QScriptValueImpl::typeInfo() const
{
    return m_type;
}

inline void QScriptValueImpl::setTypeInfo(QScriptTypeInfo *type)
{
    m_type = type;
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
    m_type = 0;
}

inline bool QScriptValueImpl::isValid() const
{
    return m_type != 0;
}

inline bool QScriptValueImpl::isUndefined() const
{
    return m_type && (m_type->type() == QScript::UndefinedType);
}

inline bool QScriptValueImpl::isNull() const
{
    return m_type && (m_type->type() == QScript::NullType);
}

inline bool QScriptValueImpl::isBoolean() const
{
    return m_type && (m_type->type() == QScript::BooleanType);
}

inline bool QScriptValueImpl::isNumber() const
{
    return m_type && (m_type->type() == QScript::NumberType);
}

inline bool QScriptValueImpl::isString() const
{
    return m_type && (m_type->type() == QScript::StringType);
}

inline bool QScriptValueImpl::isReference() const
{
    return m_type && (m_type->type() == QScript::ReferenceType);
}

inline bool QScriptValueImpl::isObject() const
{
    return m_type && (m_type->type() == QScript::ObjectType);
}

inline bool QScriptValueImpl::isFunction() const
{
    return m_type && (m_type->type() == QScript::ObjectType)
        && (classInfo()->type() & QScriptClassInfo::FunctionBased);
}

inline bool QScriptValueImpl::isVariant() const
{
    return m_type && (m_type->type() == QScript::ObjectType)
        && (classInfo()->type() == QScriptClassInfo::VariantType);
}

inline bool QScriptValueImpl::isQObject() const
{
    return m_type && (m_type->type() == QScript::ObjectType)
        && (classInfo()->type() == QScriptClassInfo::QObjectType);
}

inline bool QScriptValueImpl::isQMetaObject() const
{
    return m_type && (m_type->type() == QScript::ObjectType)
        && (classInfo()->type() == QScriptClassInfo::QMetaObjectType);
}

inline QScriptValueImpl::operator QScriptValue() const
{
    if (!isValid())
        return QScriptValue();

    QScriptValuePrivate *p = QScriptEnginePrivate::get(engine())->registerValue(*this);
    QScriptValue v;
    QScriptValuePrivate::init(v, p);
    return v;
}

inline bool QScriptValueImpl::isArray() const
{
    if (!isObject())
        return false;
    QScriptEnginePrivate *eng_p = QScriptEnginePrivate::get(engine());
    return classInfo() == eng_p->arrayConstructor->classInfo();
}

inline bool QScriptValueImpl::isDate() const
{
    if (!isObject())
        return false;
    QScriptEnginePrivate *eng_p = QScriptEnginePrivate::get(engine());
    return classInfo() == eng_p->dateConstructor->classInfo();
}

inline bool QScriptValueImpl::isError() const
{
    if (!isObject())
        return false;
    QScriptEnginePrivate *eng_p = QScriptEnginePrivate::get(engine());
    return classInfo() == eng_p->errorConstructor->classInfo();
}

inline bool QScriptValueImpl::isRegExp() const
{
    if (!isObject())
        return false;
    QScriptEnginePrivate *eng_p = QScriptEnginePrivate::get(engine());
    return classInfo() == eng_p->regexpConstructor->classInfo();
}

inline qsreal QScriptValueImpl::toNumber() const
{
    if (!isValid())
        return 0;
    return QScriptEnginePrivate::get(engine())->convertToNativeDouble(*this);
}

inline bool QScriptValueImpl::toBoolean() const
{
    if (!isValid())
        return false;
    return QScriptEnginePrivate::get(engine())->convertToNativeBoolean(*this);
}

inline QString QScriptValueImpl::toString() const
{
    if (!isValid())
        return QString();
    return QScriptEnginePrivate::get(engine())->convertToNativeString(*this);
}

inline qint32 QScriptValueImpl::toInt32() const
{
    if (!isValid())
        return 0;
    double d = QScriptEnginePrivate::get(engine())->convertToNativeDouble(*this);
    return QScriptEnginePrivate::toInt32(d);
}

inline quint32 QScriptValueImpl::toUInt32() const
{
    if (!isValid())
        return 0;
    double d = QScriptEnginePrivate::get(engine())->convertToNativeDouble(*this);
    return QScriptEnginePrivate::toUint32(d);
}

inline quint16 QScriptValueImpl::toUInt16() const
{
    if (!isValid())
        return 0;
    double d = QScriptEnginePrivate::get(engine())->convertToNativeDouble(*this);
    return QScriptEnginePrivate::toUint16(d);
}

inline qsreal QScriptValueImpl::toInteger() const
{
    if (!isValid())
        return 0;
    double d = QScriptEnginePrivate::get(engine())->convertToNativeDouble(*this);
    return QScriptEnginePrivate::toInteger(d);
}

inline QScriptValueImpl QScriptValueImpl::toObject() const
{
    if (!isValid())
        return QScriptValueImpl();
    return QScriptEnginePrivate::get(engine())->toObject(*this);
}

inline QDateTime QScriptValueImpl::toDateTime() const
{
    if (!isDate())
        return QDateTime();
    return QScriptEnginePrivate::get(engine())->toDateTime(*this);
}

#ifndef QT_NO_REGEXP
inline QRegExp QScriptValueImpl::toRegExp() const
{
    if (!isRegExp())
        return QRegExp();
    QScriptEnginePrivate *eng_p = QScriptEnginePrivate::get(engine());
    return eng_p->regexpConstructor->toRegExp(*this);
}
#endif // QT_NO_REGEXP

inline QScriptValueImpl QScriptValueImpl::toPrimitive(TypeHint hint) const
{
    if (!isValid())
        return QScriptValueImpl();
    return QScriptEnginePrivate::get(engine())->toPrimitive(*this, hint);
}

inline QObject *QScriptValueImpl::toQObject() const
{
#ifndef QT_NO_QOBJECT
    if (isQObject()) {
        QScript::ExtQObject *ctor = QScriptEnginePrivate::get(engine())->qobjectConstructor;
        Q_ASSERT(ctor != 0);

        QScript::ExtQObject::Instance *data = ctor->get(*this);
        Q_ASSERT(data != 0);

        return data->value;
    }
#endif

    return 0;
}

inline const QMetaObject *QScriptValueImpl::toQMetaObject() const
{
#ifndef QT_NO_QOBJECT
    if (isQMetaObject()) {
        QScript::ExtQMetaObject *ctor = QScriptEnginePrivate::get(engine())->qmetaObjectConstructor;
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
                               QScriptValueImpl *object, QScriptValue::ResolveFlags mode) const
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

    return resolve_helper(nameId, member, object, mode);
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
    if (resolve(nameId, &m, &o, QScriptValue::ResolvePrototype))
        o.get(m, out);
    else
        QScriptEnginePrivate::get(engine())->newUndefined(out);
}

inline void QScriptValueImpl::get_helper(const QScript::Member &member, QScriptValueImpl *out) const
{
    QScriptEnginePrivate *eng = QScriptEnginePrivate::get(engine());

    if (member.nameId() == eng->idTable()->id___proto__) {
        *out = prototype();

        if (!out->isValid())
            eng->newUndefined(out);

        return;
    }

    if (QScriptClassData *data = classInfo()->data().data()) {
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

    QScriptEnginePrivate *eng_p = QScriptEnginePrivate::get(engine());

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

    QScript::ExtQObject *ctor = QScriptEnginePrivate::get(engine())->qobjectConstructor;
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

    QScript::Ext::Variant *ctor = QScriptEnginePrivate::get(engine())->variantConstructor;
    Q_ASSERT(ctor != 0);

    QScript::Ext::Variant::Instance *data = ctor->get(*this);
    Q_ASSERT(data != 0);

    return data->value;
}

inline void QScriptValueImpl::setVariantValue(const QVariant &value)
{
    if (!isVariant())
        return;

    QScript::Ext::Variant *ctor = QScriptEnginePrivate::get(engine())->variantConstructor;
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

    else if (QScriptClassData *data = classInfo()->data().data())
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
    return QScriptEnginePrivate::get(engine())->convertToNativeFunction(*this);
}

inline QScriptValueImpl QScriptValueImpl::property(QScriptNameIdImpl *nameId,
                                            const QScriptValue::ResolveFlags &mode) const
{
    if (!isObject())
        return QScriptValueImpl();

    QScriptValueImpl base;
    QScript::Member member;

    if (! resolve(nameId, &member, &base, mode))
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
    QScriptNameIdImpl *nameId = QScriptEnginePrivate::get(engine())->nameId(name);
    setProperty(nameId, value, flags);
}

inline QScriptValueImpl QScriptValueImpl::property(const QString &name,
                                            const QScriptValue::ResolveFlags &mode) const
{
    if (!isObject())
        return QScriptValueImpl();
    QScriptNameIdImpl *nameId = QScriptEnginePrivate::get(engine())->nameId(name);
    return property(nameId, mode);
}

inline QScriptValueImpl QScriptValueImpl::property(quint32 arrayIndex,
                                            const QScriptValue::ResolveFlags &mode) const
{
    if (!isObject())
        return QScriptValueImpl();

    QScriptEnginePrivate *eng_p = QScriptEnginePrivate::get(engine());
    QScript::Ecma::Array::Instance *instance = eng_p->arrayConstructor->get(*this);
    if (instance)
        return instance->value.at(arrayIndex);

    QScriptValueImpl id;
    eng_p->newNumber(&id, arrayIndex);
    return property(id.toString(), mode);
}

inline void QScriptValueImpl::setProperty(quint32 arrayIndex, const QScriptValueImpl &value,
                                   const QScriptValue::PropertyFlags &flags)
{
    if (!isObject())
        return;

    QScriptEnginePrivate *eng_p = QScriptEnginePrivate::get(engine());
    QScript::Ecma::Array::Instance *instance = eng_p->arrayConstructor->get(*this);
    if (instance) {
        instance->value.assign(arrayIndex, value);
        return;
    }

    QScriptValueImpl id;
    eng_p->newNumber(&id, arrayIndex);
    setProperty(id.toString(), value, flags);
}

inline QScriptValue::PropertyFlags QScriptValueImpl::propertyFlags(const QString &name,
                                                const QScriptValue::ResolveFlags &mode) const
{
    QScriptNameIdImpl *nameId = QScriptEnginePrivate::get(engine())->nameId(name);
    return propertyFlags(nameId, mode);
}

inline QScriptValue::PropertyFlags QScriptValueImpl::propertyFlags(QScriptNameIdImpl *nameId,
                                                const QScriptValue::ResolveFlags &mode) const
{
    if (!isObject())
        return 0;

    QScriptValueImpl base;
    QScript::Member member;
    if (! resolve(nameId, &member, &base, mode))
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
    if (resolve(nameId, &member, &base, mode)) {
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

    QScriptEngine *eng = engine();
    QScriptEnginePrivate *eng_p = QScriptEnginePrivate::get(eng);
    return eng_p->call(*this, thisObject, args, /*asConstructor=*/false);
}

inline QScriptValueImpl QScriptValueImpl::call(const QScriptValueImpl &thisObject,
                                               const QScriptValueImpl &args)
{
    if (!isFunction())
        return QScriptValueImpl();

    QScriptEngine *eng = engine();
    QScriptEnginePrivate *eng_p = QScriptEnginePrivate::get(eng);
    return eng_p->call(*this, thisObject, args, /*asConstructor=*/false);
}

inline QScriptValueImpl QScriptValueImpl::construct(const QScriptValueImplList &args)
{
    if (!isFunction())
        return QScriptValueImpl();

    QScriptEngine *eng = engine();
    QScriptEnginePrivate *eng_p = QScriptEnginePrivate::get(eng);

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

    QScriptEngine *eng = engine();
    QScriptEnginePrivate *eng_p = QScriptEnginePrivate::get(eng);

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
        QScriptEnginePrivate::get(engine())->markString(m_string_value, generation);

    else if (isObject())
        QScriptEnginePrivate::get(engine())->markObject(*this, generation);
}

inline bool QScriptValueImpl::lessThan(const QScriptValueImpl &other) const
{
    if (!isValid() || !other.isValid())
        return false;

    QScriptEnginePrivate *eng_p = QScriptEnginePrivate::get(engine());
    return eng_p->lessThan(*this, other);
}

inline bool QScriptValueImpl::equals(const QScriptValueImpl &other) const
{
    if (!isValid() || !other.isValid())
        return isValid() == other.isValid();

    QScriptEnginePrivate *eng_p = QScriptEnginePrivate::get(engine());
    return eng_p->equals(*this, other);
}

inline bool QScriptValueImpl::strictlyEquals(const QScriptValueImpl &other) const
{
    if (!isValid() || !other.isValid())
        return isValid() == other.isValid();

    QScriptEnginePrivate *eng_p = QScriptEnginePrivate::get(engine());
    return eng_p->strictlyEquals(*this, other);
}

QT_END_NAMESPACE

#endif // QT_NO_SCRIPT

#endif
