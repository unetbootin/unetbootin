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
    QScriptValueImpl callee;
};

} // namespace QScript

inline QScriptEnginePrivate::QScriptEnginePrivate()
{
}

inline QScriptEnginePrivate *QScriptEnginePrivate::get(QScriptEngine *q)
{
    return q->d_func();
}

inline QString QScriptEnginePrivate::toString(QScriptNameIdImpl *id) const
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
    o->m_type = (m_type_reference);
    o->m_int_value = (mode);
}

inline void QScriptEnginePrivate::newActivation(QScriptValueImpl *o)
{
    Q_ASSERT(o);
    newObject(o, objectConstructor->publicPrototype, m_class_activation);
}

inline void QScriptEnginePrivate::newBoolean(QScriptValueImpl *o, bool b)
{
    Q_ASSERT(o);
    o->m_type = (m_type_boolean);
    o->m_bool_value = (b);
}

inline void QScriptEnginePrivate::newNull(QScriptValueImpl *o)
{
    Q_ASSERT(o);
    o->m_type = (m_type_null);
}

inline void QScriptEnginePrivate::newUndefined(QScriptValueImpl *o)
{
    Q_ASSERT(o);
    o->m_type = (m_type_undefined);
}

inline void QScriptEnginePrivate::newPointer(QScriptValueImpl *o, void *ptr)
{
    Q_ASSERT(o);
    o->m_type = (m_type_pointer);
    o->m_ptr_value = ptr;
}

inline void QScriptEnginePrivate::newInteger(QScriptValueImpl *o, int i)
{
    Q_ASSERT(o);
    o->m_type = (m_type_int);
    o->m_int_value = (i);
}

inline void QScriptEnginePrivate::newNumber(QScriptValueImpl *o, qsreal d)
{
    Q_ASSERT(o);
    o->m_type = (m_type_number);
    o->m_number_value = (d);
}

inline void QScriptEnginePrivate::newNameId(QScriptValueImpl *o, const QString &s)
{
    Q_ASSERT(o);
    o->m_type = (m_type_string);
    o->m_string_value = (nameId(s, /*persistent=*/false));
}

inline void QScriptEnginePrivate::newString(QScriptValueImpl *o, const QString &s)
{
    Q_ASSERT(o);
    o->m_type = (m_type_string);
    QScriptNameIdImpl *entry = new QScriptNameIdImpl(s);
    m_tempStringRepository.append(entry);
    o->m_string_value = (entry);
}

inline void QScriptEnginePrivate::newNameId(QScriptValueImpl *o, QScriptNameIdImpl *id)
{
    Q_ASSERT(o);
    o->m_type = (m_type_string);
    o->m_string_value = (id);
}

inline const QScript::IdTable *QScriptEnginePrivate::idTable() const
{
    return &m_id_table;
}

inline QString QScriptEnginePrivate::toString(qsreal d)
{
    if (isNaN(d))
        return QLatin1String("NaN");

    else if (isInf(d))
        return QLatin1String(d < 0 ? "-Infinity" : "Infinity");

    else if (d == 0)
        return QLatin1String("0");

    return toString_helper(d);
}

inline qsreal QScriptEnginePrivate::toNumber(const QString &repr)
{
    bool converted = false;
    qsreal v;

    if (repr.length() > 2 && repr.at(0) == QLatin1Char('0') && repr.at(1).toUpper() == QLatin1Char('X'))
        v = repr.mid(2).toLongLong(&converted, 16);
    else
        v = repr.toDouble(&converted); // ### fixme

    if (converted)
        return v;

    if (repr.isEmpty())
        return 0;

    if (repr == QLatin1String("Infinity"))
        return +Inf();

    if (repr == QLatin1String("+Infinity"))
        return +Inf();

    if (repr == QLatin1String("-Infinity"))
        return -Inf();

    {
        QString trimmed = repr.trimmed();
        if (trimmed.length() < repr.length())
            return toNumber(trimmed);
    }

    return SNaN();
}

inline qsreal QScriptEnginePrivate::convertToNativeDouble(const QScriptValueImpl &object)
{
    Q_ASSERT (object.isValid());

    if (object.isNumber())
        return object.m_number_value;

    return convertToNativeDouble_helper(object);
}

inline qint32 QScriptEnginePrivate::convertToNativeInt32(const QScriptValueImpl &object)
{
    Q_ASSERT (object.isValid());

    return toInt32 (convertToNativeDouble(object));
}


inline bool QScriptEnginePrivate::convertToNativeBoolean(const QScriptValueImpl &object)
{
    Q_ASSERT (object.isValid());

    if (object.isBoolean())
        return object.m_bool_value;

    return convertToNativeBoolean_helper(object);
}

inline QString QScriptEnginePrivate::convertToNativeString(const QScriptValueImpl &object)
{
    Q_ASSERT (object.isValid());

    if (object.isString())
        return object.m_string_value->s;

    return convertToNativeString_helper(object);
}

inline qsreal QScriptEnginePrivate::Inf()
{
    return qInf();
}

// Signaling NAN
inline qsreal QScriptEnginePrivate::SNaN()
{
    return qSNaN();
}

// Quiet NAN
inline qsreal QScriptEnginePrivate::QNaN()
{
    return qQNaN();
}

inline bool QScriptEnginePrivate::isInf(qsreal d)
{
    return qIsInf(d);
}

inline bool QScriptEnginePrivate::isNaN(qsreal d)
{
    return qIsNaN(d);
}

inline bool QScriptEnginePrivate::isFinite(qsreal d)
{
    return qIsFinite(d);
}

inline qsreal QScriptEnginePrivate::toInteger(qsreal n)
{
    if (isNaN(n))
        return 0;

    if (n == 0 || isInf(n))
        return n;

    int sign = n < 0 ? -1 : 1;
    return sign * ::floor(::fabs(n));
}

inline qint32 QScriptEnginePrivate::toInt32(qsreal n)
{
    if (isNaN(n) || isInf(n) || (n == 0))
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
    if (isNaN(n) || isInf(n) || (n == 0))
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
    if (isNaN(n) || isInf(n) || (n == 0))
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
    return objectAllocator();
}

inline QScriptContext *QScriptEnginePrivate::currentContext() const
{
    return m_context;
}

inline QScriptContext *QScriptEnginePrivate::pushContext()
{
    QScriptContext *context = m_frameRepository.get();
    QScriptContextPrivate::get(context)->init(m_context);
    m_context = context;
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
    QScriptContext *context = m_context;
    m_context = context->parentContext();
    if (m_context) {
        QScriptContextPrivate *p1 = QScriptContextPrivate::get(m_context);
        QScriptContextPrivate *p2 = QScriptContextPrivate::get(context);
        if ((p1->m_state != QScriptContext::ExceptionState)
            || (p2->m_state == QScriptContext::ExceptionState)) {
            // propagate the state
            p1->m_result = p2->m_result;
            p1->m_state = p2->m_state;
            // only update errorLineNumber if there actually was an exception
            if (p2->m_state == QScriptContext::ExceptionState)
                p1->errorLineNumber = p2->errorLineNumber;
        }
    }
    m_frameRepository.release(context);
}

inline void QScriptEnginePrivate::maybeGC()
{
    if (objectAllocator.blocked())
        return;

    bool do_string_gc = ((m_stringRepository.size() - m_oldStringRepositorySize) > 256);
    do_string_gc |= ((m_tempStringRepository.size() - m_oldTempStringRepositorySize) > 1024);

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
    QScript::Array a;
    a.resize(length);
    newArray(&v, a);
    return v;
}

inline QScriptClassInfo *QScriptEnginePrivate::registerClass(const QString &pname, int type)
{
    if (type == -1)
        type = ++m_class_prev_id;

    QScriptClassInfo *oc = new QScriptClassInfo(QScriptClassInfo::Type(type), pname);
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
    ctor->setProperty(QLatin1String("prototype"), proto,
                      QScriptValue::Undeletable
                      | QScriptValue::ReadOnly
                      | QScriptValue::SkipInEnumeration);
    proto.setProperty(QLatin1String("constructor"), *ctor,
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
    data->callee = callee;

    newObject(object, m_class_arguments);
    object->setObjectData(data);
}

inline QScriptFunction *QScriptEnginePrivate::convertToNativeFunction(const QScriptValueImpl &object)
{
    if (object.isFunction())
        return static_cast<QScriptFunction*> (object.objectData());
    return 0;
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

    o->m_type = m_type_object;
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
    if (!result.isValid())
        newVariant(&result, v);
    return result;
}

inline QScriptValueImpl QScriptEnginePrivate::undefinedValue()
{
    QScriptValueImpl v;
    newUndefined(&v);
    return v;
}

inline QScriptValueImpl QScriptEnginePrivate::nullValue()
{
    QScriptValueImpl v;
    newNull(&v);
    return v;
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

inline bool QScriptEnginePrivate::lessThan(const QScriptValueImpl &lhs, const QScriptValueImpl &rhs) const
{
    return QScriptContextPrivate::get(currentContext())->lt_cmp(lhs, rhs);
}

inline bool QScriptEnginePrivate::equals(const QScriptValueImpl &lhs, const QScriptValueImpl &rhs) const
{
    return QScriptContextPrivate::get(currentContext())->eq_cmp(lhs, rhs);
}

inline bool QScriptEnginePrivate::strictlyEquals(const QScriptValueImpl &lhs, const QScriptValueImpl &rhs) const
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
    return QScriptContextPrivate::get(currentContext())->returnValue();
}

inline void QScriptEnginePrivate::maybeProcessEvents()
{
    if (m_processEventsInterval > 0 && ++m_processEventIncr > 512) {
        m_processEventIncr = 0;
        processEvents();
    }
}

#ifndef Q_SCRIPT_NO_EVENT_NOTIFY

inline bool QScriptEnginePrivate::shouldAbort() const
{
    return m_abort;
}

inline void QScriptEnginePrivate::resetAbortFlag()
{
    m_abort = false;
}

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
    if (shouldNotify())
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
