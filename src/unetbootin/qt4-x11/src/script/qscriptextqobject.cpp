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

#include <QtCore/qglobal.h>

#ifndef QT_NO_SCRIPT

#include "qscriptengine_p.h"
#include "qscriptvalueimpl_p.h"
#include "qscriptcontext_p.h"
#include "qscriptmember_p.h"
#include "qscriptobject_p.h"
#include "qscriptable.h"
#include "qscriptable_p.h"
#include "qscriptextqobject_p.h"

#include <QtCore/QtDebug>
#include <QtCore/QMetaMethod>
#include <QtCore/QRegExp>
#include <QtCore/QVarLengthArray>
#include <QtCore/QPointer>

QT_BEGIN_NAMESPACE

// we use bits 15..12 of property flags
enum {
    PROPERTY_ID      = 0 << 12,
    DYNAPROPERTY_ID  = 1 << 12,
    METHOD_ID        = 2 << 12,
    CHILD_ID         = 3 << 12,
    ID_MASK          = 7 << 12,
    MAYBE_OVERLOADED = 8 << 12
};

static const bool GeneratePropertyFunctions = true;

int QScriptMetaType::typeId() const
{
    if (isVariant())
        return QMetaType::type("QVariant");
    return isMetaEnum() ? 2/*int*/ : m_typeId;
}

QByteArray QScriptMetaType::name() const
{
    if (!m_name.isEmpty())
        return m_name;
    else if (m_kind == Variant)
        return "QVariant";
    return QMetaType::typeName(typeId());
}

namespace QScript {

class QObjectNotifyCaller : public QObject
{
public:
    void callConnectNotify(const char *signal)
        { connectNotify(signal); }
    void callDisconnectNotify(const char *signal)
        { disconnectNotify(signal); }
};

class QtPropertyFunction: public QScriptFunction
{
public:
    QtPropertyFunction(const QMetaObject *meta, int index)
        : m_meta(meta), m_index(index)
        { }

    ~QtPropertyFunction() { }

    virtual void execute(QScriptContextPrivate *context);

    virtual Type type() const { return QScriptFunction::QtProperty; }

    virtual QString functionName() const;

private:
    const QMetaObject *m_meta;
    int m_index;
};

class QObjectPrototype : public QObject
{
    Q_OBJECT
public:
    QObjectPrototype(QObject *parent = 0)
        : QObject(parent) { }
    ~QObjectPrototype() { }
};

static inline QByteArray methodName(const QMetaMethod &method)
{
    QByteArray signature = method.signature();
    return signature.left(signature.indexOf('('));
}

static inline QVariant variantFromValue(QScriptEnginePrivate *eng,
                                        int targetType, const QScriptValueImpl &value)
{
    QVariant v(targetType, (void *)0);
    Q_ASSERT(eng);
    if (QScriptEnginePrivate::convert(value, targetType, v.data(), eng))
        return v;
    if (uint(targetType) == QVariant::LastType)
        return value.toVariant();
    if (value.isVariant()) {
        v = value.toVariant();
        if (v.canConvert(QVariant::Type(targetType))) {
            v.convert(QVariant::Type(targetType));
            return v;
        }
        QByteArray typeName = v.typeName();
        if (typeName.endsWith('*')
            && (QMetaType::type(typeName.left(typeName.size()-1)) == targetType)) {
            return QVariant(targetType, *reinterpret_cast<void* *>(v.data()));
        }
    }

    return QVariant();
}

void ExtQObject::Instance::finalize(QScriptEnginePrivate *eng)
{
    switch (ownership) {
    case QScriptEngine::QtOwnership:
        break;
    case QScriptEngine::ScriptOwnership:
        if (value)
            eng->disposeQObject(value);
        break;
    case QScriptEngine::AutoOwnership:
        if (value && !value->parent())
            eng->disposeQObject(value);
        break;
    }
}

ExtQObject::Instance *ExtQObject::Instance::get(const QScriptValueImpl &object, QScriptClassInfo *klass)
{
    if (! klass || klass == object.classInfo())
        return static_cast<Instance*> (object.objectData());

    return 0;
}


static inline QScriptable *scriptableFromQObject(QObject *qobj)
{
    void *ptr = qobj->qt_metacast("QScriptable");
    return reinterpret_cast<QScriptable*>(ptr);
}

static bool isObjectProperty(const QScriptValueImpl &object, const char *name)
{
    QScriptEnginePrivate *eng = object.engine();
    QScriptNameIdImpl *nameId = eng->nameId(QLatin1String(name));
    QScript::Member member;
    QScriptValueImpl base;
    return object.resolve(nameId, &member, &base, QScriptValue::ResolveLocal, QScript::Read)
        && member.testFlags(QScript::Member::ObjectProperty);
}

static bool hasMethodAccess(const QMetaMethod &method, int index, const QScriptEngine::QObjectWrapOptions &opt)
{
    return (method.access() != QMetaMethod::Private)
        && ((index != 2) || !(opt & QScriptEngine::ExcludeDeleteLater));
}

static bool isEnumerableMetaProperty(const QMetaProperty &prop,
                                     const QMetaObject *mo, int index)
{
    return prop.isScriptable() && prop.isValid()
        // the following lookup is to ensure that we have the
        // "most derived" occurrence of the property with this name
        && (mo->indexOfProperty(prop.name()) == index);
}

static uint flagsForMetaProperty(const QMetaProperty &prop)
{
    return (QScriptValue::Undeletable
        | (!prop.isWritable()
           ? QScriptValue::ReadOnly
           : QScriptValue::PropertyFlag(0))
        | (GeneratePropertyFunctions
           ? (QScriptValue::PropertyGetter
              | QScriptValue::PropertySetter)
           : QScriptValue::PropertyFlag(0))
        | QScriptValue::QObjectMember
        | PROPERTY_ID);
}


static int indexOfMetaEnum(const QMetaObject *meta, const QByteArray &str)
{
    QByteArray scope;
    QByteArray name;
    int scopeIdx = str.lastIndexOf("::");
    if (scopeIdx != -1) {
        scope = str.left(scopeIdx);
        name = str.mid(scopeIdx + 2);
    } else {
        name = str;
    }
    for (int i = meta->enumeratorCount() - 1; i >= 0; --i) {
        QMetaEnum m = meta->enumerator(i);
        if ((m.name() == name) && (scope.isEmpty() || (m.scope() == scope)))
            return i;
    }
    return -1;
}

static QMetaMethod metaMethod(const QMetaObject *meta,
                              QMetaMethod::MethodType type,
                              int index)
{
    if (type != QMetaMethod::Constructor)
        return meta->method(index);
    else
        return meta->constructor(index);
}
    
static void callQtMethod(QScriptContextPrivate *context, QMetaMethod::MethodType callType,
                         QObject *thisQObject, const QMetaObject *meta, int initialIndex,
                         bool maybeOverloaded)
{
    QScriptValueImpl result;
    QScriptEnginePrivate *engine = context->engine();

    int limit;
#ifndef Q_SCRIPT_NO_QMETAOBJECT_CACHE
    int lastFoundIndex = initialIndex;
    QScriptMetaObject *metaCache = engine->cachedMetaObject(meta);
    if (callType != QMetaMethod::Constructor)
        limit = metaCache->methodLowerBound(initialIndex);
    else
        limit = 0;
#else
    limit = 0;
#endif

    QByteArray funName;
    QScriptMetaMethod chosenMethod;
    int chosenIndex = -1;
    QVarLengthArray<QVariant, 9> args;
    QVector<QScriptMetaArguments> candidates;
    QVector<QScriptMetaArguments> unresolved;
    QVector<int> tooFewArgs;
    QVector<int> conversionFailed;
    int index;
    for (index = initialIndex; index >= limit; --index) {
        QScriptMetaMethod mtd;
#ifndef Q_SCRIPT_NO_QMETAOBJECT_CACHE
        if (callType != QMetaMethod::Constructor)
            mtd = metaCache->findMethod(index);
        if (!mtd.isValid())
#endif
        {
            QMetaMethod method = metaMethod(meta, callType, index);

            QVector<QScriptMetaType> types;
            // resolve return type
            QByteArray returnTypeName = method.typeName();
            int rtype = QMetaType::type(returnTypeName);
            if ((rtype == 0) && !returnTypeName.isEmpty()) {
                if (returnTypeName == "QVariant") {
                    types.append(QScriptMetaType::variant());
                } else {
                    int enumIndex = indexOfMetaEnum(meta, returnTypeName);
                    if (enumIndex != -1)
                        types.append(QScriptMetaType::metaEnum(enumIndex, returnTypeName));
                    else
                        types.append(QScriptMetaType::unresolved(returnTypeName));
                }
            } else {
                if (callType == QMetaMethod::Constructor)
                    types.append(QScriptMetaType::metaType(QMetaType::QObjectStar, "QObject*"));
                else if (returnTypeName == "QVariant")
                    types.append(QScriptMetaType::variant());
                else
                    types.append(QScriptMetaType::metaType(rtype, returnTypeName));
            }
            // resolve argument types
            QList<QByteArray> parameterTypeNames = method.parameterTypes();
            for (int i = 0; i < parameterTypeNames.count(); ++i) {
                QByteArray argTypeName = parameterTypeNames.at(i);
                int atype = QMetaType::type(argTypeName);
                if (atype == 0) {
                    if (argTypeName == "QVariant") {
                        types.append(QScriptMetaType::variant());
                    } else {
                        int enumIndex = indexOfMetaEnum(meta, argTypeName);
                        if (enumIndex != -1)
                            types.append(QScriptMetaType::metaEnum(enumIndex, argTypeName));
                        else
                            types.append(QScriptMetaType::unresolved(argTypeName));
                    }
                } else {
                    if (argTypeName == "QVariant")
                        types.append(QScriptMetaType::variant());
                    else
                        types.append(QScriptMetaType::metaType(atype, argTypeName));
                }
            }

            mtd = QScriptMetaMethod(methodName(method), types);

#ifndef Q_SCRIPT_NO_QMETAOBJECT_CACHE
            if (mtd.fullyResolved() && (callType != QMetaMethod::Constructor))
                metaCache->registerMethod(index, mtd);
#endif
        }

        if (index == initialIndex)
            funName = mtd.name();
        else {
            if (mtd.name() != funName)
                continue;
#ifndef Q_SCRIPT_NO_QMETAOBJECT_CACHE
            lastFoundIndex = index;
#endif
        }

        if (context->argumentCount() < mtd.argumentCount()) {
            tooFewArgs.append(index);
            continue;
        }

        if (!mtd.fullyResolved()) {
            // remember it so we can give an error message later, if necessary
            unresolved.append(QScriptMetaArguments(/*matchDistance=*/INT_MAX, index,
                                                   mtd, QVarLengthArray<QVariant, 9>()));
            if (mtd.hasUnresolvedReturnType())
                continue;
        }

        if (args.count() != mtd.count())
            args.resize(mtd.count());

        QScriptMetaType retType = mtd.returnType();
        args[0] = QVariant(retType.typeId(), (void *)0); // the result

        // try to convert arguments
        bool converted = true;
        int matchDistance = 0;
        for (int i = 0; converted && i < mtd.argumentCount(); ++i) {
            QScriptValueImpl actual = context->argument(i);
            QScriptMetaType argType = mtd.argumentType(i);
            int tid = -1;
            QVariant v;
            if (argType.isUnresolved()) {
                v = QVariant(QMetaType::QObjectStar, (void *)0);
                converted = engine->convertToNativeQObject(
                    actual, argType.name(), reinterpret_cast<void* *>(v.data()));
            } else if (argType.isVariant()) {
                if (actual.isVariant()) {
                    v = actual.variantValue();
                } else {
                    v = actual.toVariant();
                    converted = v.isValid() || actual.isUndefined() || actual.isNull();
                }
            } else {
                tid = argType.typeId();
                v = QVariant(tid, (void *)0);
                converted = QScriptEnginePrivate::convert(actual, tid, v.data(), engine);
                if (engine->hasUncaughtException())
                    return;
            }

            if (!converted) {
                if (actual.isVariant()) {
                    if (tid == -1)
                        tid = argType.typeId();
                    QVariant &vv = actual.variantValue();
                    if (vv.canConvert(QVariant::Type(tid))) {
                        v = vv;
                        converted = v.convert(QVariant::Type(tid));
                        if (converted && (vv.userType() != tid))
                            matchDistance += 10;
                    } else {
                        QByteArray vvTypeName = vv.typeName();
                        if (vvTypeName.endsWith('*')
                            && (vvTypeName.left(vvTypeName.size()-1) == argType.name())) {
                            v = QVariant(tid, *reinterpret_cast<void* *>(vv.data()));
                            converted = true;
                            matchDistance += 10;
                        }
                    }
                } else if (actual.isNumber()) {
                    // see if it's an enum value
                    QMetaEnum m;
                    if (argType.isMetaEnum()) {
                        m = meta->enumerator(argType.enumeratorIndex());
                    } else {
                        int mi = indexOfMetaEnum(meta, argType.name());
                        if (mi != -1)
                            m = meta->enumerator(mi);
                    }
                    if (m.isValid()) {
                        int ival = actual.toInt32();
                        if (m.valueToKey(ival) != 0) {
                            qVariantSetValue(v, ival);
                            converted = true;
                            matchDistance += 10;
                        }
                    }
                }
            } else {
                // determine how well the conversion matched
                if (actual.isNumber()) {
                    switch (tid) {
                    case QMetaType::Double:
                        // perfect
                        break;
                    case QMetaType::Float:
                        matchDistance += 1;
                        break;
                    case QMetaType::LongLong:
                    case QMetaType::ULongLong:
                        matchDistance += 2;
                        break;
                    case QMetaType::Long:
                    case QMetaType::ULong:
                        matchDistance += 3;
                        break;
                    case QMetaType::Int:
                    case QMetaType::UInt:
                        matchDistance += 4;
                        break;
                    case QMetaType::Short:
                    case QMetaType::UShort:
                        matchDistance += 5;
                        break;
                    case QMetaType::Char:
                    case QMetaType::UChar:
                        matchDistance += 6;
                        break;
                    default:
                        matchDistance += 10;
                        break;
                    }
                } else if (actual.isString()) {
                    switch (tid) {
                    case QMetaType::QString:
                        // perfect
                        break;
                    default:
                        matchDistance += 10;
                        break;
                    }
                } else if (actual.isBoolean()) {
                    switch (tid) {
                    case QMetaType::Bool:
                        // perfect
                        break;
                    default:
                        matchDistance += 10;
                        break;
                    }
                } else if (actual.isDate()) {
                    switch (tid) {
                    case QMetaType::QDateTime:
                        // perfect
                        break;
                    case QMetaType::QDate:
                        matchDistance += 1;
                        break;
                    case QMetaType::QTime:
                        matchDistance += 2;
                        break;
                    default:
                        matchDistance += 10;
                        break;
                    }
                } else if (actual.isRegExp()) {
                    switch (tid) {
                    case QMetaType::QRegExp:
                        // perfect
                        break;
                    default:
                        matchDistance += 10;
                        break;
                    }
                } else if (actual.isVariant()) {
                    if (argType.isVariant()
                        || (actual.variantValue().userType() == tid)) {
                        // perfect
                    } else {
                        matchDistance += 10;
                    }
                } else if (actual.isArray()) {
                    switch (tid) {
                    case QMetaType::QStringList:
                    case QMetaType::QVariantList:
                        matchDistance += 5;
                        break;
                    default:
                        matchDistance += 10;
                        break;
                    }
                } else if (actual.isQObject()) {
                    switch (tid) {
                    case QMetaType::QObjectStar:
                    case QMetaType::QWidgetStar:
                        // perfect
                        break;
                    default:
                        matchDistance += 10;
                        break;
                    }
                } else if (actual.isNull()) {
                    switch (tid) {
                    case QMetaType::VoidStar:
                    case QMetaType::QObjectStar:
                    case QMetaType::QWidgetStar:
                        // perfect
                        break;
                    default:
                        if (!argType.name().endsWith('*'))
                            matchDistance += 10;
                        break;
                    }
                } else {
                    matchDistance += 10;
                }
            }

            if (converted)
                args[i+1] = v;
        }

        if (converted) {
            if ((context->argumentCount() == mtd.argumentCount())
                && (matchDistance == 0)) {
                // perfect match, use this one
                chosenMethod = mtd;
                chosenIndex = index;
                break;
            } else {
                bool redundant = false;
                if ((callType != QMetaMethod::Constructor)
                    && (index < meta->methodOffset())) {
                    // it is possible that a virtual method is redeclared in a subclass,
                    // in which case we want to ignore the superclass declaration
                    for (int i = 0; i < candidates.size(); ++i) {
                        const QScriptMetaArguments &other = candidates.at(i);
                        if (mtd.types() == other.method.types()) {
                            redundant = true;
                            break;
                        }
                    }
                }
                if (!redundant) {
                    QScriptMetaArguments metaArgs(matchDistance, index, mtd, args);
                    if (candidates.isEmpty()) {
                        candidates.append(metaArgs);
                    } else {
                        const QScriptMetaArguments &otherArgs = candidates.at(0);
                        if ((args.count() > otherArgs.args.count())
                            || ((args.count() == otherArgs.args.count())
                                && (matchDistance <= otherArgs.matchDistance))) {
                            candidates.prepend(metaArgs);
                        } else {
                            candidates.append(metaArgs);
                        }
                    }
                }
            }
        } else if (mtd.fullyResolved()) {
            conversionFailed.append(index);
        }

        if (!maybeOverloaded)
            break;
    }

#ifndef Q_SCRIPT_NO_QMETAOBJECT_CACHE
    if ((index == -1) && (lastFoundIndex != limit) && maybeOverloaded
        && (callType != QMetaMethod::Constructor)) {
        metaCache->setMethodLowerBound(initialIndex, lastFoundIndex);
    }
#endif

    if ((chosenIndex == -1) && candidates.isEmpty()) {
        context->calleeMetaIndex = initialIndex;
#ifndef Q_SCRIPT_NO_EVENT_NOTIFY
        engine->notifyFunctionEntry(context);
#endif
        if (!conversionFailed.isEmpty()) {
            QString message = QString::fromLatin1("incompatible type of argument(s) in call to %0(); candidates were\n")
                              .arg(QLatin1String(funName));
            for (int i = 0; i < conversionFailed.size(); ++i) {
                if (i > 0)
                    message += QLatin1String("\n");
                QMetaMethod mtd = metaMethod(meta, callType, conversionFailed.at(i));
                message += QString::fromLatin1("    %0").arg(QString::fromLatin1(mtd.signature()));
            }
            result = context->throwError(QScriptContext::TypeError, message);
        } else if (!unresolved.isEmpty()) {
            QScriptMetaArguments argsInstance = unresolved.first();
            int unresolvedIndex = argsInstance.method.firstUnresolvedIndex();
            Q_ASSERT(unresolvedIndex != -1);
            QScriptMetaType unresolvedType = argsInstance.method.type(unresolvedIndex);
            QString unresolvedTypeName = QString::fromLatin1(unresolvedType.name());
            QString message = QString::fromLatin1("cannot call %0(): ")
                              .arg(QString::fromLatin1(funName));
            if (unresolvedIndex > 0) {
                message.append(QString::fromLatin1("argument %0 has unknown type `%1'").
                               arg(unresolvedIndex).arg(unresolvedTypeName));
            } else {
                message.append(QString::fromLatin1("unknown return type `%0'")
                               .arg(unresolvedTypeName));
            }
            message.append(QString::fromLatin1(" (register the type with qScriptRegisterMetaType())"));
            result = context->throwError(QScriptContext::TypeError, message);
        } else {
            QString message = QString::fromLatin1("too few arguments in call to %0(); candidates are\n")
                              .arg(QLatin1String(funName));
            for (int i = 0; i < tooFewArgs.size(); ++i) {
                if (i > 0)
                    message += QLatin1String("\n");
                QMetaMethod mtd = metaMethod(meta, callType, tooFewArgs.at(i));
                message += QString::fromLatin1("    %0").arg(QString::fromLatin1(mtd.signature()));
            }
            result = context->throwError(QScriptContext::SyntaxError, message);
        }
    } else {
        if (chosenIndex == -1) {
            QScriptMetaArguments metaArgs = candidates.at(0);
            if ((candidates.size() > 1)
                && (metaArgs.args.count() == candidates.at(1).args.count())
                && (metaArgs.matchDistance == candidates.at(1).matchDistance)) {
                // ambiguous call
                QString message = QString::fromLatin1("ambiguous call of overloaded function %0(); candidates were\n")
                                  .arg(QLatin1String(funName));
                for (int i = 0; i < candidates.size(); ++i) {
                    if (i > 0)
                        message += QLatin1String("\n");
                    QMetaMethod mtd = metaMethod(meta, callType, candidates.at(i).index);
                    message += QString::fromLatin1("    %0").arg(QString::fromLatin1(mtd.signature()));
                }
                result = context->throwError(QScriptContext::TypeError, message);
            } else {
                chosenMethod = metaArgs.method;
                chosenIndex = metaArgs.index;
                args = metaArgs.args;
            }
        }

        if (chosenIndex != -1) {
            // call it
            context->calleeMetaIndex = chosenIndex;

            QVarLengthArray<void*, 9> array(args.count());
            void **params = array.data();
            for (int i = 0; i < args.count(); ++i) {
                const QVariant &v = args[i];
                switch (chosenMethod.type(i).kind()) {
                case QScriptMetaType::Variant:
                    params[i] = const_cast<QVariant*>(&v);
                    break;
                case QScriptMetaType::MetaType:
                case QScriptMetaType::MetaEnum:
                case QScriptMetaType::Unresolved:
                    params[i] = const_cast<void*>(v.constData());
                    break;
                default:
                    Q_ASSERT(0);
                }
            }

            QScriptable *scriptable = 0;
            if (thisQObject)
                scriptable = scriptableFromQObject(thisQObject);
            QScriptEngine *oldEngine = 0;
            if (scriptable) {
                oldEngine = QScriptablePrivate::get(scriptable)->engine;
                QScriptablePrivate::get(scriptable)->engine = QScriptEnginePrivate::get(engine);
            }

#ifndef Q_SCRIPT_NO_EVENT_NOTIFY
            engine->notifyFunctionEntry(context);
#endif

            if (callType == QMetaMethod::Constructor) {
                Q_ASSERT(meta != 0);
                meta->static_metacall(QMetaObject::CreateInstance, chosenIndex, params);
            } else {
                Q_ASSERT(thisQObject != 0);
                thisQObject->qt_metacall(QMetaObject::InvokeMetaMethod, chosenIndex, params);
            }

            if (scriptable)
                QScriptablePrivate::get(scriptable)->engine = oldEngine;

            if (context->state() == QScriptContext::ExceptionState) {
                result = context->returnValue(); // propagate
            } else {
                QScriptMetaType retType = chosenMethod.returnType();
                if (retType.isVariant()) {
                    result = engine->valueFromVariant(*(QVariant *)params[0]);
                } else if (retType.typeId() != 0) {
                    result = engine->create(retType.typeId(), params[0]);
                    if (!result.isValid())
                        engine->newVariant(&result, QVariant(retType.typeId(), params[0]));
                } else {
                    result = engine->undefinedValue();
                }
            }
        }
    }

    context->m_result = result;
#ifndef Q_SCRIPT_NO_EVENT_NOTIFY
    engine->notifyFunctionExit(context);
#endif
}


class ExtQObjectDataIterator: public QScriptClassDataIterator
{
public:
    ExtQObjectDataIterator(const QScriptValueImpl &object);
    virtual ~ExtQObjectDataIterator();

    virtual bool hasNext() const;
    virtual void next(QScript::Member *member);

    virtual bool hasPrevious() const;
    virtual void previous(QScript::Member *member);

    virtual void toFront();
    virtual void toBack();

private:
    enum State {
        MetaProperties,
        DynamicProperties,
        MetaMethods
    };

    QScriptValueImpl m_object;
    int m_index;
    State m_state;
};

ExtQObjectDataIterator::ExtQObjectDataIterator(const QScriptValueImpl &object)
{
    m_object = object;
    toFront();
}

ExtQObjectDataIterator::~ExtQObjectDataIterator()
{
}

bool ExtQObjectDataIterator::hasNext() const
{
    ExtQObject::Instance *inst = ExtQObject::Instance::get(m_object);
    if (!inst->value)
        return false;
    const QMetaObject *meta = inst->value->metaObject();
    int i = m_index;

    switch (m_state) {
    case MetaProperties: {
        for ( ; i < meta->propertyCount(); ++i) {
            QMetaProperty prop = meta->property(i);
            if (isEnumerableMetaProperty(prop, meta, i)
                && !isObjectProperty(m_object, prop.name())) {
                return true;
            }
        }
        i = 0;
        // fall-through
    }

    case DynamicProperties: {
        QList<QByteArray> dpNames = inst->value->dynamicPropertyNames();
        for ( ; i < dpNames.count(); ++i) {
            if (!isObjectProperty(m_object, dpNames.at(i))) {
                return true;
            }
        }
        if (inst->options & QScriptEngine::SkipMethodsInEnumeration)
            return false;
        i = (inst->options & QScriptEngine::ExcludeSuperClassMethods)
            ? meta->methodOffset() : 0;
        // fall-through
    }

    case MetaMethods: {
        for ( ; i < meta->methodCount(); ++i) {
            QMetaMethod method = meta->method(i);
            if (hasMethodAccess(method, i, inst->options)
                && !isObjectProperty(m_object, method.signature())) {
                return true;
            }
        }
    }

    } // switch

    return false;
}

void ExtQObjectDataIterator::next(QScript::Member *member)
{
    QScriptEnginePrivate *eng = m_object.engine();
    ExtQObject::Instance *inst = ExtQObject::Instance::get(m_object);
    if (!inst->value)
        return;
    const QMetaObject *meta = inst->value->metaObject();
    int i = m_index;

    switch (m_state) {
    case MetaProperties: {
        for ( ; i < meta->propertyCount(); ++i) {
            QMetaProperty prop = meta->property(i);
            if (isEnumerableMetaProperty(prop, meta, i)
                && !isObjectProperty(m_object, prop.name())) {
                QScriptNameIdImpl *nameId = eng->nameId(QLatin1String(prop.name()));
                member->native(nameId, i, flagsForMetaProperty(prop));
                m_index = i + 1;
                return;
            }
        }
        m_state = DynamicProperties;
        m_index = 0;
        i = m_index;
        // fall-through
    }

    case DynamicProperties: {
        QList<QByteArray> dpNames = inst->value->dynamicPropertyNames();
        for ( ; i < dpNames.count(); ++i) {
            if (!isObjectProperty(m_object, dpNames.at(i))) {
                QByteArray name = dpNames.at(i);
                QScriptNameIdImpl *nameId = eng->nameId(QLatin1String(name));
                member->native(nameId, i,
                               QScriptValue::QObjectMember
                               | DYNAPROPERTY_ID);
                m_index = i + 1;
                return;
            }
        }
        m_state = MetaMethods;
        m_index = (inst->options & QScriptEngine::ExcludeSuperClassMethods)
                  ? meta->methodOffset() : 0;
        i = m_index;
        // fall-through
    }

    case MetaMethods: {
        for ( ; i < meta->methodCount(); ++i) {
            QMetaMethod method = meta->method(i);
            if (hasMethodAccess(method, i, inst->options)
                && !isObjectProperty(m_object, method.signature())) {
                QMetaMethod method = meta->method(i);
                QScriptNameIdImpl *nameId = eng->nameId(QLatin1String(method.signature()));
                member->native(nameId, i,
                               QScriptValue::QObjectMember
                               | METHOD_ID);
                m_index = i + 1;
                return;
            }
        }
    }

    } // switch

    member->invalidate();
}

bool ExtQObjectDataIterator::hasPrevious() const
{
    ExtQObject::Instance *inst = ExtQObject::Instance::get(m_object);
    if (!inst->value)
        return false;
    const QMetaObject *meta = inst->value->metaObject();
    int i = m_index - 1;

    switch (m_state) {
    case MetaMethods: {
        int limit = (inst->options & QScriptEngine::ExcludeSuperClassMethods)
                    ? meta->methodOffset() : 0;
        for ( ; i >= limit; --i) {
            QMetaMethod method = meta->method(i);
            if (hasMethodAccess(method, i, inst->options)
                && !isObjectProperty(m_object, method.signature())) {
                return true;
            }
        }
        i = inst->value->dynamicPropertyNames().count() - 1;
        // fall-through
    }

    case DynamicProperties: {
        QList<QByteArray> dpNames = inst->value->dynamicPropertyNames();
        for ( ; i >= 0; --i) {
            if (!isObjectProperty(m_object, dpNames.at(i))) {
                return true;
            }
        }
        i = meta->propertyCount() - 1;
        // fall-through
    }

    case MetaProperties: {
        int limit = (inst->options & QScriptEngine::ExcludeSuperClassProperties)
                    ? meta->propertyOffset() : 0;
        for ( ; i >= limit; --i) {
            QMetaProperty prop = meta->property(i);
            if (isEnumerableMetaProperty(prop, meta, i)
                && !isObjectProperty(m_object, prop.name())) {
                return true;
            }
        }
    }

    } // switch

    return false;
}

void ExtQObjectDataIterator::previous(QScript::Member *member)
{
    QScriptEnginePrivate *eng = m_object.engine();
    ExtQObject::Instance *inst = ExtQObject::Instance::get(m_object);
    if (!inst->value)
        return;
    const QMetaObject *meta = inst->value->metaObject();
    int i = m_index - 1;

    switch (m_state) {
    case MetaMethods: {
        int limit = (inst->options & QScriptEngine::ExcludeSuperClassMethods)
                    ? meta->methodOffset() : 0;
        for ( ; i >= limit; --i) {
            QMetaMethod method = meta->method(i);
            if (hasMethodAccess(method, i, inst->options)
                && !isObjectProperty(m_object, method.signature())) {
                QMetaMethod method = meta->method(i);
                QScriptNameIdImpl *nameId = eng->nameId(QLatin1String(method.signature()));
                member->native(nameId, i,
                               QScriptValue::QObjectMember
                               | METHOD_ID);
                m_index = i;
                return;
            }
        }
        m_state = DynamicProperties;
        m_index = inst->value->dynamicPropertyNames().count() - 1;
        i = m_index;
        // fall-through
    }

    case DynamicProperties: {
        QList<QByteArray> dpNames = inst->value->dynamicPropertyNames();
        for ( ; i >= 0; --i) {
            if (!isObjectProperty(m_object, dpNames.at(i))) {
                QByteArray name = dpNames.at(i);
                QScriptNameIdImpl *nameId = eng->nameId(QLatin1String(name));
                member->native(nameId, i,
                               QScriptValue::QObjectMember
                               | DYNAPROPERTY_ID);
                m_index = i;
                return;
            }
        }
        m_state = MetaProperties;
        m_index = meta->propertyCount() - 1;
        i = m_index;
        // fall-through
    }

    case MetaProperties: {
        int limit = (inst->options & QScriptEngine::ExcludeSuperClassProperties)
                    ? meta->propertyOffset() : 0;
        for ( ; i >= limit; --i) {
            QMetaProperty prop = meta->property(i);
            if (isEnumerableMetaProperty(prop, meta, i)
                && !isObjectProperty(m_object, prop.name())) {
                QScriptNameIdImpl *nameId = eng->nameId(QLatin1String(prop.name()));
                member->native(nameId, i, flagsForMetaProperty(prop));
                m_index = i;
                return;
            }
        }
    }

    } // switch

    member->invalidate();
}

void ExtQObjectDataIterator::toFront()
{
    ExtQObject::Instance *inst = ExtQObject::Instance::get(m_object);
    if (!inst->value)
        return;
    m_state = MetaProperties;
    const QMetaObject *meta = inst->value->metaObject();
    m_index = (inst->options & QScriptEngine::ExcludeSuperClassProperties)
              ? meta->propertyOffset() : 0;
}

void ExtQObjectDataIterator::toBack()
{
    ExtQObject::Instance *inst = ExtQObject::Instance::get(m_object);
    if (!inst->value)
        return;
    if (inst->options & QScriptEngine::SkipMethodsInEnumeration) {
        m_state = DynamicProperties;
        m_index = inst->value->dynamicPropertyNames().count();
    } else {
        m_state = MetaMethods;
        const QMetaObject *meta = inst->value->metaObject();
        m_index = meta->methodCount();
    }
}

class ExtQObjectData: public QScriptClassData
{
public:
    ExtQObjectData(QScriptClassInfo *classInfo)
        : m_classInfo(classInfo)
    {
    }

    virtual bool resolve(const QScriptValueImpl &object, QScriptNameIdImpl *nameId,
                         QScript::Member *member, QScriptValueImpl *,
                         QScript::AccessMode access)
    {
        ExtQObject::Instance *inst = ExtQObject::Instance::get(object, m_classInfo);
        QObject *qobject = inst->value;
        if (! qobject) {
            // the object was deleted. We return true so we can
            // throw an error in get()/put()
            member->native(nameId, /*id=*/-1, /*flags=*/0);
            return true;
        }

        const QScriptEngine::QObjectWrapOptions &opt = inst->options;
        const QMetaObject *meta = qobject->metaObject();

        QScriptEnginePrivate *eng = object.engine();

#ifndef Q_SCRIPT_NO_QMETAOBJECT_CACHE
        QScriptMetaObject *metaCache = eng->cachedMetaObject(meta);
        if (metaCache->findMember(nameId, member)) {
            bool ignore = false;
            switch (member->flags() & ID_MASK) {
            case PROPERTY_ID:
                ignore = (opt & QScriptEngine::ExcludeSuperClassProperties)
                         && (member->id() < meta->propertyOffset());
                break;
            case METHOD_ID:
                ignore = ((opt & QScriptEngine::ExcludeSuperClassMethods)
                          && (member->id() < meta->methodOffset()))
                         || ((opt & QScriptEngine::ExcludeDeleteLater)
                             && (member->id() == 2));
                break;
            // we don't cache dynamic properties nor children,
            // so no need to handle DYNAPROPERTY_ID and CHILD_ID
            default:
                break;
            }
            if (!ignore)
                return true;
        }
#endif

        QString memberName = eng->toString(nameId);
        QByteArray name = memberName.toLatin1();

        int index = -1;

        if (name.contains('(')) {
            QByteArray normalized = QMetaObject::normalizedSignature(name);
            if (-1 != (index = meta->indexOfMethod(normalized))) {
                QMetaMethod method = meta->method(index);
                if (hasMethodAccess(method, index, opt)) {
                    member->native(nameId, index,
                                   QScriptValue::QObjectMember
                                   | METHOD_ID);
#ifndef Q_SCRIPT_NO_QMETAOBJECT_CACHE
                    metaCache->registerMember(nameId, *member);
#endif
                    if (!(opt & QScriptEngine::ExcludeSuperClassMethods)
                        || (index >= meta->methodOffset())) {
                        return true;
                    }
                }
            }
        }

        index = meta->indexOfProperty(name);
        if (index != -1) {
            QMetaProperty prop = meta->property(index);
            if (prop.isScriptable()) {
                member->native(nameId, index, flagsForMetaProperty(prop));
#ifndef Q_SCRIPT_NO_QMETAOBJECT_CACHE
                metaCache->registerMember(nameId, *member);
#endif
                if (!(opt & QScriptEngine::ExcludeSuperClassProperties)
                    || (index >= meta->propertyOffset())) {
                    return true;
                }
            }
        }

        index = qobject->dynamicPropertyNames().indexOf(name);
        if (index != -1) {
            member->native(nameId, index,
                           QScriptValue::QObjectMember
                           | DYNAPROPERTY_ID);
            // not cached because it can be removed
            return true;
        }

        const int offset = (opt & QScriptEngine::ExcludeSuperClassMethods)
                           ? meta->methodOffset() : 0;
        for (index = meta->methodCount() - 1; index >= offset; --index) {
            QMetaMethod method = meta->method(index);
            if (hasMethodAccess(method, index, opt)
                && (methodName(method) == name)) {
                member->native(nameId, index,
                               QScriptValue::QObjectMember
                               | METHOD_ID
                               | MAYBE_OVERLOADED);
#ifndef Q_SCRIPT_NO_QMETAOBJECT_CACHE
                metaCache->registerMember(nameId, *member);
#endif
                return true;
            }
        }

        if (!(opt & QScriptEngine::ExcludeChildObjects)) {
            QList<QObject*> children = qobject->children();
            for (index = 0; index < children.count(); ++index) {
                QObject *child = children.at(index);
                if (child->objectName() == memberName) {
                    member->native(nameId, index,
                                   QScriptValue::ReadOnly
                                   | QScriptValue::Undeletable
                                   | QScriptValue::SkipInEnumeration
                                   | CHILD_ID);
                    // not cached because it can be removed or change name
                    return true;
                }
            }
        }

        if ((access & QScript::Write) && (opt & QScriptEngine::AutoCreateDynamicProperties)) {
            member->native(nameId, -1, DYNAPROPERTY_ID);
            return true;
        }

        return false;
     }

    virtual bool get(const QScriptValueImpl &obj, const QScript::Member &member, QScriptValueImpl *result)
    {
        if (! member.isNativeProperty())
            return false;

        QScriptEnginePrivate *eng = obj.engine();

        ExtQObject::Instance *inst = ExtQObject::Instance::get(obj, m_classInfo);
        QObject *qobject = inst->value;
        if (!qobject) {
            QScriptContextPrivate *ctx = eng->currentContext();
            *result = ctx->throwError(
                QString::fromLatin1("cannot access member `%0' of deleted QObject")
                .arg(member.nameId()->s));
            return true;
        }

        switch (member.flags() & ID_MASK) {
        case PROPERTY_ID: {
            const QMetaObject *meta = qobject->metaObject();
            const int propertyIndex = member.id();
            QMetaProperty prop = meta->property(propertyIndex);
            Q_ASSERT(prop.isScriptable());
            if (GeneratePropertyFunctions) {
                QScriptValueImpl accessor;
#ifndef Q_SCRIPT_NO_QMETAOBJECT_CACHE
                QScriptMetaObject *metaCache = eng->cachedMetaObject(meta);
                accessor = metaCache->findPropertyAccessor(propertyIndex);
                if (!accessor.isValid()) {
#endif
                    accessor = eng->createFunction(new QtPropertyFunction(meta, propertyIndex));
#ifndef Q_SCRIPT_NO_QMETAOBJECT_CACHE
                    metaCache->registerPropertyAccessor(propertyIndex, accessor);
                }
#endif
                *result = accessor;
            } else {
                QVariant v = prop.read(qobject);
                *result = eng->valueFromVariant(v);
            }
        }   break;

        case DYNAPROPERTY_ID: {
            if (member.id() != -1) {
                QVariant v = qobject->property(member.nameId()->s.toLatin1());
                *result = eng->valueFromVariant(v);
            } else {
                *result = eng->undefinedValue();
            }
        }   break;

        case METHOD_ID: {
            QScript::Member m;
            bool maybeOverloaded = (member.flags() & MAYBE_OVERLOADED) != 0;
            *result = eng->createFunction(new QtFunction(obj, member.id(),
                                                         maybeOverloaded));
            // make it persist (otherwise Function.prototype.disconnect() would fail)
            uint flags = QScriptValue::QObjectMember;
            if (inst->options & QScriptEngine::SkipMethodsInEnumeration)
                flags |= QScriptValue::SkipInEnumeration;
            QScriptObject *instance = obj.objectValue();
            if (!instance->findMember(member.nameId(), &m))
                instance->createMember(member.nameId(), &m, flags);
            instance->put(m, *result);
        }   break;

        case CHILD_ID: {
            QObject *child = qobject->children().at(member.id());
            result->invalidate();
            QScriptEngine::QObjectWrapOptions opt = QScriptEngine::PreferExistingWrapperObject;
            eng->newQObject(result, child, QScriptEngine::QtOwnership, opt);
        }   break;

        } // switch

        return true;
    }

    virtual bool put(QScriptValueImpl *object, const QScript::Member &member, const QScriptValueImpl &value)
    {
        if (! member.isNativeProperty() || ! member.isWritable())
            return false;

        ExtQObject::Instance *inst = ExtQObject::Instance::get(*object, m_classInfo);
        QObject *qobject = inst->value;
        if (!qobject) {
            QScriptEnginePrivate *eng = object->engine();
            QScriptContextPrivate *ctx = eng->currentContext();
            ctx->throwError(QString::fromLatin1("cannot access member `%0' of deleted QObject")
                            .arg(member.nameId()->s));
            return true;
        }

        switch (member.flags() & ID_MASK) {
        case CHILD_ID:
            return false;

        case METHOD_ID: {
            QScript::Member m;
            QScriptObject *instance = object->objectValue();
            if (!instance->findMember(member.nameId(), &m)) {
                instance->createMember(member.nameId(), &m,
                                       /*flags=*/0);
            }
            instance->put(m, value);
            return true;
        }

        case PROPERTY_ID:
            if (GeneratePropertyFunctions) {
                // we shouldn't get here, QScriptValueImpl::setProperty() messed up
                Q_ASSERT_X(0, "put", "Q_PROPERTY access cannot be overridden");
                return false;
            } else {
                const QMetaObject *meta = qobject->metaObject();
                QMetaProperty prop = meta->property(member.id());
                Q_ASSERT(prop.isScriptable());
                QVariant v = variantFromValue(object->engine(), prop.userType(), value);
                bool ok = prop.write(qobject, v);
                return ok;
            }

        case DYNAPROPERTY_ID: {
            QVariant v = value.toVariant();
            return ! qobject->setProperty(member.nameId()->s.toLatin1(), v);
        }

        } // switch
        return false;
    }

    virtual bool removeMember(const QScriptValueImpl &object,
                              const QScript::Member &member)
    {
        QObject *qobject = object.toQObject();
        if (!qobject || !member.isNativeProperty() || !member.isDeletable())
            return false;

        if ((member.flags() & ID_MASK) == DYNAPROPERTY_ID) {
            qobject->setProperty(member.nameId()->s.toLatin1(), QVariant());
            return true;
        }

        return false;
    }

    virtual void mark(const QScriptValueImpl &, int)
    {
    }

    virtual QScriptClassDataIterator *newIterator(const QScriptValueImpl &object)
    {
        return new ExtQObjectDataIterator(object);
    }

private:
    QScriptClassInfo *m_classInfo;
};

struct QObjectConnection
{
    int slotIndex;
    QScriptValueImpl receiver;
    QScriptValueImpl slot;
    QScriptValueImpl senderWrapper;

    QObjectConnection(int i, const QScriptValueImpl &r, const QScriptValueImpl &s,
               const QScriptValueImpl &sw)
        : slotIndex(i), receiver(r), slot(s), senderWrapper(sw) {}
    QObjectConnection() : slotIndex(-1) {}

    bool hasTarget(const QScriptValueImpl &r, const QScriptValueImpl &s) const
    {
        if (r.isObject() != receiver.isObject())
            return false;
        if ((r.isObject() && receiver.isObject())
            && (r.objectValue() != receiver.objectValue())) {
            return false;
        }
        return (s.objectValue() == slot.objectValue());
    }

    void mark(int generation)
    {
        if (senderWrapper.isValid() && !senderWrapper.isMarked(generation)) {
            // see if the sender should be marked or not
            ExtQObject::Instance *inst = ExtQObject::Instance::get(senderWrapper);
            if ((inst->ownership == QScriptEngine::ScriptOwnership)
                || ((inst->ownership == QScriptEngine::AutoOwnership)
                    && inst->value && !inst->value->parent())) {
                senderWrapper.invalidate();
            } else {
                senderWrapper.mark(generation);
            }
        }
        if (receiver.isValid())
            receiver.mark(generation);
        if (slot.isValid())
            slot.mark(generation);
    }
};

class QObjectConnectionManager: public QObject
{
public:
    QObjectConnectionManager();
    ~QObjectConnectionManager();

    bool addSignalHandler(QObject *sender, int signalIndex,
        const QScriptValueImpl &receiver,
        const QScriptValueImpl &slot,
        const QScriptValueImpl &senderWrapper = QScriptValueImpl());
    bool removeSignalHandler(
        QObject *sender, int signalIndex,
        const QScriptValueImpl &receiver,
        const QScriptValueImpl &slot);

    static const QMetaObject staticMetaObject;
    virtual const QMetaObject *metaObject() const;
    virtual void *qt_metacast(const char *);
    virtual int qt_metacall(QMetaObject::Call, int, void **argv);

    void execute(int slotIndex, void **argv);

    void mark(int generation);

private:
    int m_slotCounter;
    QVector<QVector<QObjectConnection> > connections;
};

} // ::QScript



QScript::ExtQObject::ExtQObject(QScriptEnginePrivate *eng):
    Ecma::Core(eng, QLatin1String("QObject"), QScriptClassInfo::QObjectType)
{
    newQObject(&publicPrototype, new QScript::QObjectPrototype(),
               QScriptEngine::AutoOwnership,
               QScriptEngine::ExcludeSuperClassMethods
               | QScriptEngine::ExcludeSuperClassProperties
               | QScriptEngine::ExcludeChildObjects);

    eng->newConstructor(&ctor, this, publicPrototype);
    addPrototypeFunction(QLatin1String("toString"), method_toString, 0);
    addPrototypeFunction(QLatin1String("findChild"), method_findChild, 1);
    addPrototypeFunction(QLatin1String("findChildren"), method_findChildren, 1);

    classInfo()->setData(new QScript::ExtQObjectData(classInfo()));
}

QScript::ExtQObject::~ExtQObject()
{
}

void QScript::ExtQObject::execute(QScriptContextPrivate *context)
{
    QScriptValueImpl tmp;
    newQObject(&tmp, 0);
    context->setReturnValue(tmp);
}

void QScript::ExtQObject::newQObject(QScriptValueImpl *result, QObject *value,
                                     QScriptEngine::ValueOwnership ownership,
                                     const QScriptEngine::QObjectWrapOptions &options)
{
    Instance *instance;
    if (!result->isValid()) {
        engine()->newObject(result, publicPrototype, classInfo());
        instance = new Instance();
        result->setObjectData(instance);
    } else {
        Q_ASSERT(result->isObject());
        if (result->classInfo() != classInfo()) {
            result->destroyObjectData();
            result->setClassInfo(classInfo());
            instance = new Instance();
            result->setObjectData(instance);
        } else {
            instance = Instance::get(*result);
        }
    }
    instance->value = value;
    instance->ownership = ownership;
    instance->options = options;
}

QScriptValueImpl QScript::ExtQObject::method_findChild(QScriptContextPrivate *context, QScriptEnginePrivate *eng, QScriptClassInfo *classInfo)
{
    if (Instance *instance = Instance::get(context->thisObject(), classInfo)) {
        QObject *obj = instance->value;
        QString name = context->argument(0).toString();
        QObject *child = qFindChild<QObject*>(obj, name);
        QScriptEngine::QObjectWrapOptions opt = QScriptEngine::PreferExistingWrapperObject;
        QScriptValueImpl result;
        eng->newQObject(&result, child, QScriptEngine::QtOwnership, opt);
        return result;
    }
    return eng->undefinedValue();
}

QScriptValueImpl QScript::ExtQObject::method_findChildren(QScriptContextPrivate *context, QScriptEnginePrivate *eng, QScriptClassInfo *classInfo)
{
    if (Instance *instance = Instance::get(context->thisObject(), classInfo)) {
        QObject *obj = instance->value;
        QList<QObject*> found;
        QScriptValueImpl arg = context->argument(0);
#ifndef QT_NO_REGEXP
        if (arg.isRegExp()) {
            QRegExp re = arg.toRegExp();
            found = qFindChildren<QObject*>(obj, re);
        } else
#endif
        {
            QString name = arg.isUndefined() ? QString() : arg.toString();
            found = qFindChildren<QObject*>(obj, name);
        }
        QScriptValueImpl result = eng->newArray(found.size());
        QScriptEngine::QObjectWrapOptions opt = QScriptEngine::PreferExistingWrapperObject;
        for (int i = 0; i < found.size(); ++i) {
            QScriptValueImpl value;
            eng->newQObject(&value, found.at(i), QScriptEngine::QtOwnership, opt);
            result.setProperty(i, value);
        }
        return result;
    }
    return eng->undefinedValue();
}

QScriptValueImpl QScript::ExtQObject::method_toString(QScriptContextPrivate *context, QScriptEnginePrivate *eng, QScriptClassInfo *classInfo)
{
    if (Instance *instance = Instance::get(context->thisObject(), classInfo)) {
        QObject *obj = instance->value;
        const QMetaObject *meta = obj ? obj->metaObject() : &QObject::staticMetaObject;
        QString name = obj ? obj->objectName() : QString::fromUtf8("unnamed");

        QString str = QString::fromUtf8("%0(name = \"%1\")")
                      .arg(QLatin1String(meta->className())).arg(name);
        return QScriptValueImpl(eng, str);
    }
    return eng->undefinedValue();
}



static const uint qt_meta_data_QObjectConnectionManager[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      35,   34,   34,   34, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_QObjectConnectionManager[] = {
    "QScript::QObjectConnectionManager\0\0execute()\0"
};

const QMetaObject QScript::QObjectConnectionManager::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QObjectConnectionManager,
      qt_meta_data_QObjectConnectionManager, 0 }
};

const QMetaObject *QScript::QObjectConnectionManager::metaObject() const
{
    return &staticMetaObject;
}

void *QScript::QObjectConnectionManager::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QObjectConnectionManager))
        return static_cast<void*>(const_cast<QObjectConnectionManager*>(this));
    return QObject::qt_metacast(_clname);
}

int QScript::QObjectConnectionManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        execute(_id, _a);
        _id -= m_slotCounter;
    }
    return _id;
}

void QScript::QObjectConnectionManager::execute(int slotIndex, void **argv)
{
    QScriptValueImpl receiver;
    QScriptValueImpl slot;
    QScriptValueImpl senderWrapper;
    int signalIndex = -1;
    for (int i = 0; i < connections.size(); ++i) {
        const QVector<QObjectConnection> &cs = connections.at(i);
        for (int j = 0; j < cs.size(); ++j) {
            const QObjectConnection &c = cs.at(j);
            if (c.slotIndex == slotIndex) {
                receiver = c.receiver;
                slot = c.slot;
                senderWrapper = c.senderWrapper;
                signalIndex = i;
                break;
            }
        }
    }
    Q_ASSERT(slot.isValid());

    QScriptEnginePrivate *eng = slot.engine();

    if (eng->isCollecting()) {
        // we can't do a script function call during GC,
        // so we're forced to ignore this signal
        return;
    }

    QScriptFunction *fun = eng->convertToNativeFunction(slot);
    if (fun == 0) {
        // the signal handler has been GC'ed. This can only happen when
        // a QObject is owned by the engine, the engine is destroyed, and
        // there is a script function connected to the destroyed() signal
        Q_ASSERT(signalIndex <= 1); // destroyed(QObject*)
        return;
    }

    const QMetaObject *meta = sender()->metaObject();
    const QMetaMethod method = meta->method(signalIndex);

    QList<QByteArray> parameterTypes = method.parameterTypes();
    int argc = parameterTypes.count();

    QScriptValueImpl activation;
    eng->newActivation(&activation);
    QScriptObject *activation_data = activation.objectValue();
    activation_data->m_scope = slot.scope();

    int formalCount = fun->formals.count();
    int mx = qMax(formalCount, argc);
    activation_data->m_members.resize(mx + 1);
    activation_data->m_values.resize(mx + 1);
    for (int i = 0; i < mx; ++i) {
        QScriptNameIdImpl *nameId;
        if (i < formalCount)
            nameId = fun->formals.at(i);
        else
            nameId = 0;
        activation_data->m_members[i].object(nameId, i,
                                             QScriptValue::Undeletable
                                             | QScriptValue::SkipInEnumeration);
        if (i < argc) {
            int argType = QMetaType::type(parameterTypes.at(i));
            activation_data->m_values[i] = eng->create(argType, argv[i + 1]);
        } else {
            activation_data->m_values[i] = eng->undefinedValue();
        }
    }

    QScriptValueImpl senderObject;
    if (senderWrapper.isQObject()) {
        senderObject = senderWrapper;
    } else {
        QScriptEngine::QObjectWrapOptions opt = QScriptEngine::PreferExistingWrapperObject;
        eng->newQObject(&senderObject, sender(), QScriptEngine::QtOwnership, opt);
    }
    activation_data->m_members[mx].object(eng->idTable()->id___qt_sender__, mx,
                                          QScriptValue::SkipInEnumeration);
    activation_data->m_values[mx] = senderObject;

    QScriptValueImpl thisObject;
    if (receiver.isObject())
        thisObject = receiver;
    else
        thisObject = eng->globalObject();

    QScriptContextPrivate *context_data = eng->pushContext();
    context_data->m_activation = activation;
    context_data->m_callee = slot;
    context_data->m_thisObject = thisObject;
    context_data->argc = argc;
    context_data->args = const_cast<QScriptValueImpl*> (activation_data->m_values.constData());

    fun->execute(context_data);

    eng->popContext();
    if (eng->hasUncaughtException())
        eng->emitSignalHandlerException();
}

QScript::QObjectConnectionManager::QObjectConnectionManager()
    : m_slotCounter(0)
{
}

QScript::QObjectConnectionManager::~QObjectConnectionManager()
{
}

void QScript::QObjectConnectionManager::mark(int generation)
{
    for (int i = 0; i < connections.size(); ++i) {
        QVector<QObjectConnection> &cs = connections[i];
        for (int j = 0; j < cs.size(); ++j)
            cs[j].mark(generation);
    }
}

bool QScript::QObjectConnectionManager::addSignalHandler(
    QObject *sender, int signalIndex, const QScriptValueImpl &receiver,
    const QScriptValueImpl &function, const QScriptValueImpl &senderWrapper)
{
    if (connections.size() <= signalIndex)
        connections.resize(signalIndex+1);
    QVector<QObjectConnection> &cs = connections[signalIndex];
    int absSlotIndex = m_slotCounter + metaObject()->methodOffset();
    bool ok = QMetaObject::connect(sender, signalIndex, this, absSlotIndex);
    if (ok) {
        cs.append(QScript::QObjectConnection(m_slotCounter++, receiver, function, senderWrapper));
        QMetaMethod signal = sender->metaObject()->method(signalIndex);
        QByteArray signalString;
        signalString.append('2'); // signal code
        signalString.append(signal.signature());
        static_cast<QScript::QObjectNotifyCaller*>(sender)->callConnectNotify(signalString);
    }
    return ok;
}

bool QScript::QObjectConnectionManager::removeSignalHandler(
    QObject *sender, int signalIndex,
    const QScriptValueImpl &receiver,
    const QScriptValueImpl &slot)
{
    if (connections.size() <= signalIndex)
        return false;
    QVector<QObjectConnection> &cs = connections[signalIndex];
    for (int i = 0; i < cs.size(); ++i) {
        const QObjectConnection &c = cs.at(i);
        if (c.hasTarget(receiver, slot)) {
            int absSlotIndex = c.slotIndex + metaObject()->methodOffset();
            bool ok = QMetaObject::disconnect(sender, signalIndex, this, absSlotIndex);
            if (ok) {
                cs.remove(i);
                QMetaMethod signal = sender->metaObject()->method(signalIndex);
                QByteArray signalString;
                signalString.append('2'); // signal code
                signalString.append(signal.signature());
                static_cast<QScript::QObjectNotifyCaller*>(sender)->callDisconnectNotify(signalString);
            }
            return ok;
        }
    }
    return false;
}



QString QScript::QtPropertyFunction::functionName() const
{
    QMetaProperty prop = m_meta->property(m_index);
    return QLatin1String(prop.name());
}

void QScript::QtPropertyFunction::execute(QScriptContextPrivate *context)
{
    context->calleeMetaIndex = m_index;

    QScriptEnginePrivate *eng_p = context->engine();
#ifndef Q_SCRIPT_NO_EVENT_NOTIFY
    eng_p->notifyFunctionEntry(context);
#endif
    QScriptValueImpl result = eng_p->undefinedValue();

    QScriptValueImpl object = context->thisObject();
    QObject *qobject = object.toQObject();
    while ((!qobject || (qobject->metaObject() != m_meta))
           && object.prototype().isObject()) {
        object = object.prototype();
        qobject = object.toQObject();
    }
    Q_ASSERT(qobject);

    QMetaProperty prop = m_meta->property(m_index);
    Q_ASSERT(prop.isScriptable());
    if (context->argumentCount() == 0) {
        // get
        if (prop.isValid()) {
            QScriptable *scriptable = scriptableFromQObject(qobject);
            QScriptEngine *oldEngine = 0;
            if (scriptable) {
                oldEngine = QScriptablePrivate::get(scriptable)->engine;
                QScriptablePrivate::get(scriptable)->engine = QScriptEnginePrivate::get(eng_p);
            }

            QVariant v = prop.read(qobject);

            if (scriptable)
                QScriptablePrivate::get(scriptable)->engine = oldEngine;

            result = eng_p->valueFromVariant(v);
        }
    } else {
        // set
        QVariant v = variantFromValue(eng_p, prop.userType(), context->argument(0));

        QScriptable *scriptable = scriptableFromQObject(qobject);
        QScriptEngine *oldEngine = 0;
        if (scriptable) {
            oldEngine = QScriptablePrivate::get(scriptable)->engine;
            QScriptablePrivate::get(scriptable)->engine = QScriptEnginePrivate::get(eng_p);
        }

        prop.write(qobject, v);

        if (scriptable)
            QScriptablePrivate::get(scriptable)->engine = oldEngine;

        result = context->argument(0);
    }
    if (!eng_p->hasUncaughtException())
        context->m_result = result;
#ifndef Q_SCRIPT_NO_EVENT_NOTIFY
    eng_p->notifyFunctionExit(context);
#endif
}

QString QScript::QtFunction::functionName() const
{
    const QMetaObject *meta = metaObject();
    if (!meta)
        return QString();
    QMetaMethod method = meta->method(m_initialIndex);
    return QLatin1String(methodName(method));
}

void QScript::QtFunction::mark(QScriptEnginePrivate *engine, int generation)
{
    if (m_object.isValid())
        engine->markObject(m_object, generation);
    QScriptFunction::mark(engine, generation);
}

void QScript::QtFunction::execute(QScriptContextPrivate *context)
{
    QScriptEnginePrivate *eng_p = context->engine();
    QObject *qobj = qobject();
    if (!qobj) {
        context->calleeMetaIndex = m_initialIndex;
#ifndef Q_SCRIPT_NO_EVENT_NOTIFY
        eng_p->notifyFunctionEntry(context);
#endif
        context->throwError(QLatin1String("cannot call function of deleted QObject"));
#ifndef Q_SCRIPT_NO_EVENT_NOTIFY
        eng_p->notifyFunctionExit(context);
#endif
        return;
    }

    QScriptValueImpl result = eng_p->undefinedValue();

    const QMetaObject *meta = qobj->metaObject();

    QObject *thisQObject = context->thisObject().toQObject();
    if (!thisQObject) // ### TypeError
        thisQObject = qobj;

    if (!meta->cast(thisQObject)) {
#if 0
        // ### find common superclass, see if initialIndex is
        //     in that class (or a superclass of that class),
        //     then it's still safe to execute it
        funName = methodName(meta->method(m_initialIndex));
        context->throwError(
            QString::fromUtf8("cannot execute %0: %1 does not inherit %2")
            .arg(QLatin1String(funName))
            .arg(QLatin1String(thisQObject->metaObject()->className()))
            .arg(QLatin1String(meta->className())));
        return;
#endif
        // invoking a function in the prototype
        thisQObject = qobj;
    }

    callQtMethod(context, QMetaMethod::Method, thisQObject,
                 meta, m_initialIndex, m_maybeOverloaded);
}

int QScript::QtFunction::mostGeneralMethod(QMetaMethod *out) const
{
    const QMetaObject *meta = metaObject();
    if (!meta)
        return -1;
    int index = m_initialIndex;
    QMetaMethod method = meta->method(index);
    if (maybeOverloaded() && (method.attributes() & QMetaMethod::Cloned)) {
        // find the most general method
        do {
            method = meta->method(--index);
        } while (method.attributes() & QMetaMethod::Cloned);
    }
    if (out)
        *out = method;
    return index;
}

QList<int> QScript::QtFunction::overloadedIndexes() const
{
    if (!maybeOverloaded())
        return QList<int>();
    QList<int> result;
    QString name = functionName();
    const QMetaObject *meta = metaObject();
    for (int index = mostGeneralMethod() - 1; index >= 0; --index) {
        QString otherName = QString::fromLatin1(methodName(meta->method(index)));
        if (otherName == name)
            result.append(index);
    }
    return result;
}

/////////////////////////////////////////////////////////

namespace QScript
{

ExtQMetaObject::Instance *ExtQMetaObject::Instance::get(const QScriptValueImpl &object,
                                                        QScriptClassInfo *klass)
{
    if (! klass || klass == object.classInfo())
        return static_cast<Instance*> (object.objectData());

    return 0;
}

void ExtQMetaObject::Instance::execute(QScriptContextPrivate *context)
{
    if (ctor.isFunction()) {
        QScriptValueImplList args;
        for (int i = 0; i < context->argumentCount(); ++i)
            args << context->argument(i);
        QScriptEnginePrivate *eng = context->engine();
        context->m_result = eng->call(ctor, context->thisObject(), args,
                                      context->isCalledAsConstructor());
    } else {
        if (value->constructorCount() > 0) {
            callQtMethod(context, QMetaMethod::Constructor, /*thisQObject=*/0,
                         value, value->constructorCount()-1, /*maybeOverloaded=*/true);
            if (context->state() == QScriptContext::NormalState) {
                ExtQObject::Instance *inst = ExtQObject::Instance::get(context->m_result);
                Q_ASSERT(inst != 0);
                inst->ownership = QScriptEngine::AutoOwnership;
                context->m_result.setPrototype(prototype);
            }
        } else {
            context->m_result = context->throwError(
                QScriptContext::TypeError,
                QString::fromUtf8("no constructor for %0")
                .arg(QLatin1String(value->className())));
        }
    }
}

struct StaticQtMetaObject : public QObject
{
    static const QMetaObject *get()
        { return &static_cast<StaticQtMetaObject*> (0)->staticQtMetaObject; }
};

class ExtQMetaObjectData: public QScriptClassData
{
public:
    ExtQMetaObjectData(QScriptEnginePrivate *, QScriptClassInfo *classInfo);

    virtual bool resolve(const QScriptValueImpl &object, QScriptNameIdImpl *nameId,
                         QScript::Member *member, QScriptValueImpl *base,
                         QScript::AccessMode access);
    virtual bool get(const QScriptValueImpl &object, const QScript::Member &member,
                     QScriptValueImpl *result);
    virtual bool put(QScriptValueImpl *object, const QScript::Member &member,
                     const QScriptValueImpl &value);
    virtual void mark(const QScriptValueImpl &object, int generation);

private:
    QScriptClassInfo *m_classInfo;
};

ExtQMetaObjectData::ExtQMetaObjectData(QScriptEnginePrivate *,
                                       QScriptClassInfo *classInfo)
    : m_classInfo(classInfo)
{
}

bool ExtQMetaObjectData::resolve(const QScriptValueImpl &object,
                                 QScriptNameIdImpl *nameId,
                                 QScript::Member *member,
                                 QScriptValueImpl *base,
                                 QScript::AccessMode /*access*/)
{
    const QMetaObject *meta = object.toQMetaObject();
    if (!meta)
        return false;

    QScriptEnginePrivate *eng_p = object.engine();
    if (eng_p->idTable()->id_prototype == nameId) {
        // prototype property is a proxy to constructor's prototype property
        member->native(nameId, /*id=*/0, QScriptValue::Undeletable);
        return true;
    }

    QByteArray name = eng_p->toString(nameId).toLatin1();

    for (int i = 0; i < meta->enumeratorCount(); ++i) {
        QMetaEnum e = meta->enumerator(i);

        for (int j = 0; j < e.keyCount(); ++j) {
            const char *key = e.key(j);

            if (! qstrcmp (key, name.constData())) {
                member->native(nameId, e.value(j), QScriptValue::ReadOnly);
                *base = object;
                return true;
            }
        }
    }

    return false;
}

bool ExtQMetaObjectData::get(const QScriptValueImpl &object,
                             const QScript::Member &member,
                             QScriptValueImpl *result)
{
    if (! member.isNativeProperty())
        return false;

    QScriptEnginePrivate *eng_p = object.engine();
    if (eng_p->idTable()->id_prototype == member.nameId()) {
        ExtQMetaObject::Instance *inst = ExtQMetaObject::Instance::get(object, m_classInfo);
        if (inst->ctor.isFunction())
            *result = inst->ctor.property(eng_p->idTable()->id_prototype);
        else
            *result = inst->prototype;
    } else {
        *result = QScriptValueImpl(member.id());
    }
    return true;
}

bool ExtQMetaObjectData::put(QScriptValueImpl *object, const Member &member,
                             const QScriptValueImpl &value)
{
    if (! member.isNativeProperty())
        return false;

    QScriptEnginePrivate *eng_p = object->engine();
    if (eng_p->idTable()->id_prototype == member.nameId()) {
        ExtQMetaObject::Instance *inst = ExtQMetaObject::Instance::get(*object, m_classInfo);
        if (inst->ctor.isFunction())
            inst->ctor.setProperty(eng_p->idTable()->id_prototype, value);
        else
            inst->prototype = value;
    }

    return true;
}

void ExtQMetaObjectData::mark(const QScriptValueImpl &object, int generation)
{
    ExtQMetaObject::Instance *inst = ExtQMetaObject::Instance::get(object, m_classInfo);
    if (inst->ctor.isObject() || inst->ctor.isString())
        inst->ctor.mark(generation);
}

} // namespace QScript

QScript::ExtQMetaObject::ExtQMetaObject(QScriptEnginePrivate *eng)
    : Ecma::Core(eng, QLatin1String("QMetaObject"), QScriptClassInfo::QMetaObjectType)
{
    newQMetaObject(&publicPrototype, QScript::StaticQtMetaObject::get());

    eng->newConstructor(&ctor, this, publicPrototype);
    addPrototypeFunction(QLatin1String("className"), method_className, 0);

    classInfo()->setData(new QScript::ExtQMetaObjectData(eng, classInfo()));
}

QScript::ExtQMetaObject::~ExtQMetaObject()
{
}

void QScript::ExtQMetaObject::execute(QScriptContextPrivate *context)
{
    QScriptValueImpl tmp;
    newQMetaObject(&tmp, 0);
    context->setReturnValue(tmp);
}

void QScript::ExtQMetaObject::newQMetaObject(QScriptValueImpl *result, const QMetaObject *value,
                                             const QScriptValueImpl &ctor)
{
    Instance *instance = new Instance();
    instance->value = value;
    if (ctor.isFunction()) {
        instance->ctor = ctor;
    } else {
        instance->prototype = engine()->newObject();
        instance->prototype.setPrototype(engine()->qobjectConstructor->publicPrototype);
    }

    engine()->newObject(result, publicPrototype, classInfo());
    result->setObjectData(instance);
}

QScriptValueImpl QScript::ExtQMetaObject::method_className(QScriptContextPrivate *context, QScriptEnginePrivate *eng, QScriptClassInfo *classInfo)
{
    if (Instance *instance = Instance::get(context->thisObject(), classInfo)) {
        return QScriptValueImpl(eng, QString::fromLatin1(instance->value->className()));
    }
    return eng->undefinedValue();
}

QScriptQObjectData::QScriptQObjectData()
    : m_connectionManager(0)
{
}

QScriptQObjectData::~QScriptQObjectData()
{
    if (m_connectionManager) {
        delete m_connectionManager;
        m_connectionManager = 0;
    }
}

bool QScriptQObjectData::addSignalHandler(QObject *sender,
                                          int signalIndex,
                                          const QScriptValueImpl &receiver,
                                          const QScriptValueImpl &slot,
                                          const QScriptValueImpl &senderWrapper)
{
    if (!m_connectionManager)
        m_connectionManager = new QScript::QObjectConnectionManager();
    return m_connectionManager->addSignalHandler(
        sender, signalIndex, receiver, slot, senderWrapper);
}

bool QScriptQObjectData::removeSignalHandler(QObject *sender,
                                             int signalIndex,
                                             const QScriptValueImpl &receiver,
                                             const QScriptValueImpl &slot)
{
    if (!m_connectionManager)
        return false;
    return m_connectionManager->removeSignalHandler(
        sender, signalIndex, receiver, slot);
}

bool QScriptQObjectData::findWrapper(QScriptEngine::ValueOwnership ownership,
                                     const QScriptEngine::QObjectWrapOptions &options,
                                     QScriptValueImpl *out)
{
    for (int i = 0; i < wrappers.size(); ++i) {
        const QScriptQObjectWrapperInfo &info = wrappers.at(i);
        if ((info.ownership == ownership) && (info.options == options)) {
            *out = info.object;
            return true;
        }
    }
    return false;
}

void QScriptQObjectData::registerWrapper(const QScriptValueImpl &wrapper,
                                         QScriptEngine::ValueOwnership ownership,
                                         const QScriptEngine::QObjectWrapOptions &options)
{
    wrappers.append(QScriptQObjectWrapperInfo(wrapper, ownership, options));
}

void QScriptQObjectData::mark(int generation)
{
    if (m_connectionManager)
        m_connectionManager->mark(generation);

    {
        QList<QScriptQObjectWrapperInfo>::iterator it;
        for (it = wrappers.begin(); it != wrappers.end(); ) {
            const QScriptQObjectWrapperInfo &info = *it;
            if (info.object.isMarked(generation)) {
                ++it;
            } else {
                it = wrappers.erase(it);
            }
        }
    }
}

QT_END_NAMESPACE

#include "qscriptextqobject.moc"

#endif // QT_NO_SCRIPT
