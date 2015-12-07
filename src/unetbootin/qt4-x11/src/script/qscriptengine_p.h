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

#ifndef QSCRIPTENGINE_P_H
#define QSCRIPTENGINE_P_H

#include "qscriptenginefwd_p.h"

#ifndef QT_NO_SCRIPT

#include <QtCore/QDateTime>
#include <QtCore/QMutex>
#include <QtCore/QLinkedList>
#include <QtCore/QString>
#include <QtCore/QVector>
#include <QtCore/QHash>
#include <QtCore/qnumeric.h>

#include "qscriptengine.h"
#include "qscriptnameid_p.h"
#include "qscriptobjectfwd_p.h"
#include "qscriptrepository_p.h"
#include "qscriptgc_p.h"
#include "qscriptecmaarray_p.h"
#include "qscriptecmadate_p.h"
#include "qscriptecmaobject_p.h"
#include "qscriptecmaboolean_p.h"
#include "qscriptecmanumber_p.h"
#include "qscriptecmastring_p.h"
#include "qscriptecmafunction_p.h"
#include "qscriptextvariant_p.h"
#include "qscriptextqobject_p.h"
#include "qscriptvalue_p.h"
#include "qscriptcontextfwd_p.h"

#include <math.h>

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

namespace QScript {

class ArgumentsObjectData: public QScriptObjectData
{
public:
    ArgumentsObjectData() {}
    virtual ~ArgumentsObjectData() {}

public: // attributes
    QScriptValueImpl activation;
    uint length;
};

} // namespace QScript

inline QScriptEnginePrivate *QScriptEnginePrivate::get(QScriptEngine *q)
{
    if (q)
        return q->d_func();
    return 0;
}

inline const QScriptEnginePrivate *QScriptEnginePrivate::get(const QScriptEngine *q)
{
    if (q)
        return q->d_func();
    return 0;
}

inline QScriptEngine *QScriptEnginePrivate::get(QScriptEnginePrivate *d)
{
    return d->q_func();
}

inline QString QScriptEnginePrivate::toString(QScriptNameIdImpl *id)
{
    if (! id)
        return QString();

    return id->s;
}

inline QString QScriptEnginePrivate::memberName(const QScript::Member &member) const
{
    return toString(member.nameId());
}

inline void QScriptEnginePrivate::newReference(QScriptValueImpl *o, int mode)
{
    Q_ASSERT(o);
    o->m_type = QScript::ReferenceType;
    o->m_int_value = (mode);
}

inline void QScriptEnginePrivate::newActivation(QScriptValueImpl *o)
{
    Q_ASSERT(o);
    newObject(o, nullValue(), m_class_activation);
}

inline void QScriptEnginePrivate::newPointer(QScriptValueImpl *o, void *ptr)
{
    Q_ASSERT(o);
    o->m_type = QScript::PointerType;
    o->m_ptr_value = ptr;
}

inline void QScriptEnginePrivate::newInteger(QScriptValueImpl *o, int i)
{
    Q_ASSERT(o);
    o->m_type = QScript::IntegerType;
    o->m_int_value = (i);
}

inline void QScriptEnginePrivate::newNameId(QScriptValueImpl *o, const QString &s)
{
    Q_ASSERT(o);
    o->m_type = QScript::StringType;
    o->m_string_value = (nameId(s, /*persistent=*/false));
}

inline void QScriptEnginePrivate::newString(QScriptValueImpl *o, const QString &s)
{
    Q_ASSERT(o);
    o->m_type = QScript::StringType;
    QScriptNameIdImpl *entry = new QScriptNameIdImpl(s);
    m_tempStringRepository.append(entry);
    o->m_string_value = (entry);
    m_newAllocatedTempStringRepositoryChars += s.length();
}

inline void QScriptEnginePrivate::newNameId(QScriptValueImpl *o, QScriptNameIdImpl *id)
{
    Q_ASSERT(o);
    o->m_type = QScript::StringType;
    o->m_string_value = (id);
}

inline const QScript::IdTable *QScriptEnginePrivate::idTable() const
{
    return &m_id_table;
}

inline qsreal QScriptEnginePrivate::convertToNativeDouble(const QScriptValueImpl &value)
{
    Q_ASSERT (value.isValid());

    if (value.isNumber())
        return value.m_number_value;

    return convertToNativeDouble_helper(value);
}

inline qint32 QScriptEnginePrivate::convertToNativeInt32(const QScriptValueImpl &value)
{
    Q_ASSERT (value.isValid());

    return toInt32 (convertToNativeDouble(value));
}


inline bool QScriptEnginePrivate::convertToNativeBoolean(const QScriptValueImpl &value)
{
    Q_ASSERT (value.isValid());

    if (value.isBoolean())
        return value.m_bool_value;

    return convertToNativeBoolean_helper(value);
}

inline QString QScriptEnginePrivate::convertToNativeString(const QScriptValueImpl &value)
{
    Q_ASSERT (value.isValid());

    if (value.isString())
        return value.m_string_value->s;

    return convertToNativeString_helper(value);
}

inline qsreal QScriptEnginePrivate::toInteger(qsreal n)
{
    if (qIsNaN(n))
        return 0;

    if (n == 0 || qIsInf(n))
        return n;

    int sign = n < 0 ? -1 : 1;
    return sign * ::floor(::fabs(n));
}

inline qint32 QScriptEnginePrivate::toInt32(qsreal n)
{
    if (qIsNaN(n) || qIsInf(n) || (n == 0))
        return 0;

    double sign = (n < 0) ? -1.0 : 1.0;
    qsreal abs_n = fabs(n);

    n = ::fmod(sign * ::floor(abs_n), D32);
    const double D31 = D32 / 2.0;

    if (sign == -1 && n < -D31)
        n += D32;

    else if (sign != -1 && n >= D31)
        n -= D32;

    return qint32 (n);
}

inline quint32 QScriptEnginePrivate::toUint32(qsreal n)
{
    if (qIsNaN(n) || qIsInf(n) || (n == 0))
        return 0;

    double sign = (n < 0) ? -1.0 : 1.0;
    qsreal abs_n = fabs(n);

    n = ::fmod(sign * ::floor(abs_n), D32);

    if (n < 0)
        n += D32;

    return quint32 (n);
}

inline quint16 QScriptEnginePrivate::toUint16(qsreal n)
{
    if (qIsNaN(n) || qIsInf(n) || (n == 0))
        return 0;

    double sign = (n < 0) ? -1.0 : 1.0;
    qsreal abs_n = fabs(n);

    n = ::fmod(sign * ::floor(abs_n), D16);

    if (n < 0)
        n += D16;

    return quint16 (n);
}

inline QScript::AST::Node *QScriptEnginePrivate::abstractSyntaxTree() const
{
    return m_abstractSyntaxTree;
}

inline QScript::MemoryPool *QScriptEnginePrivate::nodePool()
{
    return m_pool;
}

inline void QScriptEnginePrivate::setNodePool(QScript::MemoryPool *pool)
{
    m_pool = pool;
}

inline QScript::Lexer *QScriptEnginePrivate::lexer()
{
    return m_lexer;
}

inline void QScriptEnginePrivate::setLexer(QScript::Lexer *lexer)
{
    m_lexer = lexer;
}

inline QScriptObject *QScriptEnginePrivate::allocObject()
{
    return objectAllocator(m_objectGeneration);
}

inline QScriptContextPrivate *QScriptEnginePrivate::currentContext() const
{
    return m_context;
}

inline QScriptContextPrivate *QScriptEnginePrivate::pushContext()
{
    QScriptContext *context = m_frameRepository.get();
    QScriptContextPrivate *ctx_p = QScriptContextPrivate::get(context);
    ctx_p->init(m_context);
    m_context = ctx_p;
#ifndef Q_SCRIPT_NO_EVENT_NOTIFY
    notifyContextPush();
#endif
    return m_context;
}

inline void QScriptEnginePrivate::popContext()
{
    Q_ASSERT(m_context != 0);
#ifndef Q_SCRIPT_NO_EVENT_NOTIFY
    notifyContextPop();
#endif
    QScriptContextPrivate *context = m_context;
    m_context = context->parentContext();
    if (m_context) {
        QScriptContextPrivate *p1 = m_context;
        QScriptContextPrivate *p2 = context;
        if ((p1->m_state != QScriptContext::ExceptionState)
            || (p2->m_state == QScriptContext::ExceptionState)) {
            // propagate the state
            p1->m_result = p2->m_result;
            p1->m_state = p2->m_state;
            // only update errorLineNumber if there actually was an exception
            if (p2->m_state == QScriptContext::ExceptionState) {
                if (p2->errorLineNumber != -1)
                    p1->errorLineNumber = p2->errorLineNumber;
                else
                    p1->errorLineNumber = p1->currentLine;
            }
        }
    }
    m_frameRepository.release(QScriptContextPrivate::get(context));
}

inline void QScriptEnginePrivate::maybeGC()
{
    if (objectAllocator.blocked())
        return;

    bool do_string_gc = ((m_stringRepository.size() - m_oldStringRepositorySize) > 256)
                        || (m_newAllocatedStringRepositoryChars > 0x800000);
    do_string_gc |= ((m_tempStringRepository.size() - m_oldTempStringRepositorySize) > 1024)
                    || (m_newAllocatedTempStringRepositoryChars > 0x800000);

    if (! do_string_gc && ! objectAllocator.poll())
        return;

    maybeGC_helper(do_string_gc);
}

inline void QScriptEnginePrivate::adjustBytesAllocated(int bytes)
{
    objectAllocator.adjustBytesAllocated(bytes);
}

inline bool QScriptEnginePrivate::blockGC(bool block)
{
    return objectAllocator.blockGC(block);
}

inline void QScriptEnginePrivate::markString(QScriptNameIdImpl *id, int /*generation*/)
{
    id->used = true;
}

inline QScriptValueImpl QScriptEnginePrivate::createFunction(QScriptFunction *fun)
{
    QScriptValueImpl v;
    newFunction(&v, fun);
    return v;
}

inline QScriptValueImpl QScriptEnginePrivate::newArray(const QScript::Array &value)
{
    QScriptValueImpl v;
    newArray(&v, value);
    return v;
}

inline QScriptValueImpl QScriptEnginePrivate::newArray(uint length)
{
    QScriptValueImpl v;
    QScript::Array a(this);
    a.resize(length);
    newArray(&v, a);
    return v;
}

inline QScriptClassInfo *QScriptEnginePrivate::registerClass(const QString &pname, int type)
{
    if (type == -1)
        type = ++m_class_prev_id;

    QScriptClassInfo *oc = new QScriptClassInfo(this, QScriptClassInfo::Type(type), pname);
    m_allocated_classes.append(oc);

    return oc;
}

inline QScriptClassInfo *QScriptEnginePrivate::registerClass(const QString &name)
{
    return registerClass(name, -1);
}

inline QScriptValueImpl QScriptEnginePrivate::createFunction(QScriptInternalFunctionSignature fun,
                                       int length, QScriptClassInfo *classInfo, const QString &name)
{
    return createFunction(new QScript::C2Function(fun, length, classInfo, name));
}

inline void QScriptEnginePrivate::newFunction(QScriptValueImpl *o, QScriptFunction *function)
{
    QScriptValueImpl proto;
    if (functionConstructor)
        proto = functionConstructor->publicPrototype;
    else {
        // creating the Function prototype object
        Q_ASSERT(objectConstructor);
        proto = objectConstructor->publicPrototype;
    }
    newObject(o, proto, m_class_function);
    o->setObjectData(function);
}

inline void QScriptEnginePrivate::newConstructor(QScriptValueImpl *ctor,
                                          QScriptFunction *function,
                                          QScriptValueImpl &proto)
{
    newFunction(ctor, function);
    ctor->setProperty(m_id_table.id_prototype, proto,
                      QScriptValue::Undeletable
                      | QScriptValue::ReadOnly
                      | QScriptValue::SkipInEnumeration);
    proto.setProperty(m_id_table.id_constructor, *ctor,
                      QScriptValue::Undeletable
                      | QScriptValue::SkipInEnumeration);
}

inline void QScriptEnginePrivate::newArguments(QScriptValueImpl *object,
                                             const QScriptValueImpl &activation,
                                             uint length,
                                             const QScriptValueImpl &callee)
{
    QScript::ArgumentsObjectData *data = new QScript::ArgumentsObjectData();
    data->activation = activation;
    data->length = length;
    newObject(object, m_class_arguments);
    object->setObjectData(data);
    object->setProperty(m_id_table.id_callee, callee,
                        QScriptValue::SkipInEnumeration);
    object->setProperty(m_id_table.id_length, QScriptValueImpl(length),
                        QScriptValue::SkipInEnumeration);
}

inline QScriptFunction *QScriptEnginePrivate::convertToNativeFunction(const QScriptValueImpl &object)
{
    if (object.isFunction())
        return static_cast<QScriptFunction*> (object.objectData());
    return 0;
}

inline QScriptValue QScriptEnginePrivate::toPublic(const QScriptValueImpl &value)
{
    if (!value.isValid())
        return QScriptValue();

    QScriptValuePrivate *p = registerValue(value);
    QScriptValue v;
    QScriptValuePrivate::init(v, p);
    return v;
}

inline QScriptValueImpl QScriptEnginePrivate::toImpl(const QScriptValue &value)
{
    QScriptValuePrivate *p = QScriptValuePrivate::get(value);
    if (!p)
        return QScriptValueImpl();
    if (p->value.type() == QScript::LazyStringType)
        return toImpl_helper(value);
    return p->value;
}

inline QScriptValueImplList QScriptEnginePrivate::toImplList(const QScriptValueList &lst)
{
    QScriptValueImplList result;
    QScriptValueList::const_iterator it;
    for (it = lst.constBegin(); it != lst.constEnd(); ++it)
        result.append(toImpl(*it));
    return result;
}

inline QScriptValueImpl QScriptEnginePrivate::toObject(const QScriptValueImpl &value)
{
    if (value.isObject() || !value.isValid())
        return value;
    return toObject_helper(value);
}

inline QScriptValueImpl QScriptEnginePrivate::toPrimitive(const QScriptValueImpl &object,
                                                          QScriptValueImpl::TypeHint hint)
{
    Q_ASSERT(object.isValid());

    if (! object.isObject())
        return object;

    return toPrimitive_helper(object, hint);
}

inline QDateTime QScriptEnginePrivate::toDateTime(const QScriptValueImpl &value) const
{
    return dateConstructor->toDateTime(value);
}

inline void QScriptEnginePrivate::newArray(QScriptValueImpl *object, const QScript::Array &value)
{
    arrayConstructor->newArray(object, value);
}

inline void QScriptEnginePrivate::newObject(QScriptValueImpl *o, const QScriptValueImpl &proto,
                      QScriptClassInfo *oc)
{
    Q_ASSERT(o != 0);

    QScriptObject *od = allocObject();
    od->reset();
    od->m_id = ++m_next_object_id;
    if (proto.isValid())
        od->m_prototype = proto;
    else {
        Q_ASSERT(objectConstructor);
        od->m_prototype = objectConstructor->publicPrototype;
    }

    o->m_type = QScript::ObjectType;
    od->m_class = (oc ? oc : m_class_object);
    o->m_object_value = od;
}

inline void QScriptEnginePrivate::newObject(QScriptValueImpl *o, QScriptClassInfo *oc)
{
    newObject(o, objectConstructor->publicPrototype, oc);
}

inline QScriptValueImpl QScriptEnginePrivate::newObject()
{
    QScriptValueImpl v;
    newObject(&v);
    return v;
}

inline void QScriptEnginePrivate::newVariant(QScriptValueImpl *out,
                                             const QVariant &value,
                                             bool setDefaultPrototype)
{
    Q_ASSERT(variantConstructor != 0);
    variantConstructor->newVariant(out, value);
    if (setDefaultPrototype) {
        QScriptValueImpl proto = defaultPrototype(value.userType());
        if (proto.isValid())
            out->setPrototype(proto);
    }
}

#ifndef QT_NO_QOBJECT
#  ifndef Q_SCRIPT_NO_QMETAOBJECT_CACHE
inline QScriptMetaObject *QScriptEnginePrivate::cachedMetaObject(const QMetaObject *meta)
{
    QScriptMetaObject *value = m_cachedMetaObjects.value(meta);
    if (!value) {
        value = new QScriptMetaObject;
        m_cachedMetaObjects.insert(meta, value);
    }
    return value;
}
#  endif
#endif // !QT_NO_QOBJECT

inline QScriptNameIdImpl *QScriptEnginePrivate::nameId(const QString &str, bool persistent)
{
    QScriptNameIdImpl *entry = toStringEntry(str);
    if (! entry)
        entry = insertStringEntry(str);

    Q_ASSERT(entry->unique);

    if (persistent)
        entry->persistent = true;

    return entry;
}

inline QScriptNameIdImpl *QScriptEnginePrivate::intern(const QChar *u, int s)
{
    QString tmp(u, s);
    return nameId(tmp, /*persistent=*/ true);
}

inline QScriptValueImpl QScriptEnginePrivate::valueFromVariant(const QVariant &v)
{
    QScriptValueImpl result = create(v.userType(), v.data());
    Q_ASSERT(result.isValid());
    return result;
}

inline QScriptValueImpl QScriptEnginePrivate::undefinedValue()
{
    return m_undefinedValue;
}

inline QScriptValueImpl QScriptEnginePrivate::nullValue()
{
    return m_nullValue;
}

inline QScriptValueImpl QScriptEnginePrivate::defaultPrototype(int metaTypeId) const
{
    QScriptCustomTypeInfo info = m_customTypes.value(metaTypeId);
    return info.prototype;
}

inline void QScriptEnginePrivate::setDefaultPrototype(int metaTypeId, const QScriptValueImpl &prototype)
{
    QScriptCustomTypeInfo info = m_customTypes.value(metaTypeId);
    info.prototype = prototype;
    m_customTypes.insert(metaTypeId, info);
}

inline uint _q_scriptHash(const QString &key)
{
    const QChar *p = key.unicode();
    int n = qMin(key.size(), 128);
    uint h = key.size();
    uint g;

    while (n--) {
        h = (h << 4) + (*p++).unicode();
        if ((g = (h & 0xf0000000)) != 0)
            h ^= g >> 23;
        h &= ~g;
    }
    return h;
}

inline QScriptNameIdImpl *QScriptEnginePrivate::toStringEntry(const QString &s)
{
    uint h = _q_scriptHash(s) % m_string_hash_size;

    for (QScriptNameIdImpl *entry = m_string_hash_base[h]; entry && entry->h == h; entry = entry->next) {
        if (entry->s == s)
            return entry;
    }

    return 0;
}

inline bool QScriptEnginePrivate::lessThan(const QScriptValueImpl &lhs, const QScriptValueImpl &rhs)
{
    return QScriptContextPrivate::lt_cmp(lhs, rhs);
}

inline bool QScriptEnginePrivate::equals(const QScriptValueImpl &lhs, const QScriptValueImpl &rhs)
{
    return QScriptContextPrivate::eq_cmp(lhs, rhs);
}

inline bool QScriptEnginePrivate::strictlyEquals(const QScriptValueImpl &lhs, const QScriptValueImpl &rhs)
{
    return QScriptContextPrivate::strict_eq_cmp(lhs, rhs);
}

inline void QScriptEnginePrivate::unregisterValue(QScriptValuePrivate *p)
{
    QScriptValueImpl &v = p->value;
    Q_ASSERT(v.isValid());
    if (v.isString()) {
        QScriptNameIdImpl *id = v.stringValue();
        m_stringHandles.remove(id);
    } else if (v.isObject()) {
        QScriptObject *instance = v.objectValue();
        m_objectHandles.remove(instance);
    } else {
        int i = m_otherHandles.indexOf(p);
        Q_ASSERT(i != -1);
        m_otherHandles.remove(i);
    }
    m_handleRepository.release(p);
}

inline QScriptValueImpl QScriptEnginePrivate::globalObject() const
{
    return m_globalObject;
}

inline bool QScriptEnginePrivate::hasUncaughtException() const
{
    return (currentContext()->state() == QScriptContext::ExceptionState);
}

inline QScriptValueImpl QScriptEnginePrivate::uncaughtException() const
{
    if (!hasUncaughtException())
        return QScriptValueImpl();
    return currentContext()->returnValue();
}

inline void QScriptEnginePrivate::maybeProcessEvents()
{
    if (m_processEventsInterval > 0 && ++m_processEventIncr > 512) {
        m_processEventIncr = 0;
        processEvents();
    }
}

inline bool QScriptEnginePrivate::shouldAbort() const
{
    return m_abort;
}

inline void QScriptEnginePrivate::resetAbortFlag()
{
    m_abort = false;
}

#ifndef Q_SCRIPT_NO_EVENT_NOTIFY

inline bool QScriptEnginePrivate::shouldNotify() const
{
    return m_agent != 0;
}

inline void QScriptEnginePrivate::notifyScriptLoad(
    qint64 id, const QString &program,
    const QString &fileName, int lineNumber)
{
    if (shouldNotify())
        notifyScriptLoad_helper(id, program, fileName, lineNumber);
}

inline void QScriptEnginePrivate::notifyScriptUnload(qint64 id)
{
    if (shouldNotify())
        notifyScriptUnload_helper(id);
}

inline void QScriptEnginePrivate::notifyPositionChange(QScriptContextPrivate *ctx)
{
    Q_ASSERT(m_agent != 0);
    notifyPositionChange_helper(ctx);
}

inline void QScriptEnginePrivate::notifyContextPush()
{
    if (shouldNotify())
        notifyContextPush_helper();
}

inline void QScriptEnginePrivate::notifyContextPop()
{
    if (shouldNotify())
        notifyContextPop_helper();
}

inline void QScriptEnginePrivate::notifyFunctionEntry(QScriptContextPrivate *ctx)
{
    if (shouldNotify())
        notifyFunctionEntry_helper(ctx);
}

inline void QScriptEnginePrivate::notifyFunctionExit(QScriptContextPrivate *ctx)
{
    if (shouldNotify())
        notifyFunctionExit_helper(ctx);
}

inline void QScriptEnginePrivate::notifyException(QScriptContextPrivate *ctx)
{
    if (shouldNotify())
        notifyException_helper(ctx);
}

inline void QScriptEnginePrivate::notifyExceptionCatch(QScriptContextPrivate *ctx)
{
    if (shouldNotify())
        notifyExceptionCatch_helper(ctx);
}

#endif // Q_SCRIPT_NO_EVENT_NOTIFY

QT_END_NAMESPACE

#endif // QT_NO_SCRIPT

#endif
