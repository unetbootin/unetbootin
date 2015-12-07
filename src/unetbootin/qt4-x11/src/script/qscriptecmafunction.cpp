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

#include "qscriptecmafunction_p.h"

#ifndef QT_NO_SCRIPT

#include "qscriptengine_p.h"
#include "qscriptvalueimpl_p.h"
#include "qscriptcontext_p.h"
#include "qscriptmember_p.h"
#include "qscriptobject_p.h"

#include <QtCore/QtDebug>

#ifndef QT_NO_QOBJECT
#   include "qscriptextqobject_p.h"
#   include <QtCore/QMetaMethod>
#endif

QT_BEGIN_NAMESPACE

namespace QScript { namespace Ecma {

class FunctionClassData: public QScriptClassData
{
    QScriptClassInfo *m_classInfo;

public:
    FunctionClassData(QScriptClassInfo *classInfo);
    virtual ~FunctionClassData();

    inline QScriptClassInfo *classInfo() const
        { return m_classInfo; }

    virtual bool resolve(const QScriptValueImpl &object,
                         QScriptNameIdImpl *nameId,
                         QScript::Member *member, QScriptValueImpl *base,
                         QScript::AccessMode access);
    virtual bool get(const QScriptValueImpl &obj, const Member &m,
                     QScriptValueImpl *out_value);
    virtual bool put(QScriptValueImpl *object, const QScript::Member &member,
                     const QScriptValueImpl &value);
    virtual void mark(const QScriptValueImpl &object, int generation);
};

FunctionClassData::FunctionClassData(QScriptClassInfo *classInfo)
    : m_classInfo(classInfo)
{
}

FunctionClassData::~FunctionClassData()
{
}

bool FunctionClassData::resolve(const QScriptValueImpl &object,
                                QScriptNameIdImpl *nameId,
                                QScript::Member *member, QScriptValueImpl *base,
                                QScript::AccessMode /*access*/)
{
    if (object.classInfo() != classInfo())
        return false;

    QScriptEnginePrivate *eng = object.engine();

    if ((nameId == eng->idTable()->id_length)
        || (nameId == eng->idTable()->id_arguments)) {
        member->native(nameId, /*id=*/ 0,
                       QScriptValue::Undeletable
                       | QScriptValue::ReadOnly
                       | QScriptValue::SkipInEnumeration);
        *base = object;
        return true;
    }

    return false;
}

bool FunctionClassData::get(const QScriptValueImpl &object, const Member &member,
                            QScriptValueImpl *result)
{
    if (object.classInfo() != classInfo())
        return false;

    QScriptEnginePrivate *eng = object.engine();
    if (! member.isNativeProperty())
        return false;

    if (member.nameId() == eng->idTable()->id_length) {
        *result = QScriptValueImpl(object.toFunction()->length);
        return true;
    } else if (member.nameId() == eng->idTable()->id_arguments) {
        *result = eng->nullValue();
        return true;
    }

    return false;
}

bool FunctionClassData::put(QScriptValueImpl *, const QScript::Member &,
                            const QScriptValueImpl &)
{
    return false;
}

void FunctionClassData::mark(const QScriptValueImpl &object, int generation)
{
    if (object.classInfo() != classInfo())
        return;
    QScriptFunction *fun = object.toFunction();
    QScriptEnginePrivate *eng = object.engine();
    fun->mark(eng, generation);
}

Function::Function(QScriptEnginePrivate *eng, QScriptClassInfo *classInfo):
    Core(eng, classInfo)
{
    publicPrototype = eng->createFunction(method_void, 0, classInfo); // public prototype
}

Function::~Function()
{
}

void Function::initialize()
{
    QScriptEnginePrivate *eng = engine();
    eng->newConstructor(&ctor, this, publicPrototype);

    addPrototypeFunction(QLatin1String("toString"), method_toString, 1);
    addPrototypeFunction(QLatin1String("apply"), method_apply, 2);
    addPrototypeFunction(QLatin1String("call"), method_call, 1);
    addPrototypeFunction(QLatin1String("connect"), method_connect, 1);
    addPrototypeFunction(QLatin1String("disconnect"), method_disconnect, 1);

    classInfo()->setData(new FunctionClassData(classInfo()));
}

void Function::execute(QScriptContextPrivate *context)
{
#ifndef Q_SCRIPT_NO_EVENT_NOTIFY
    engine()->notifyFunctionEntry(context);
#endif
    int lineNumber = context->currentLine;
    QString contents = buildFunction(context);
    engine()->evaluate(context, contents, lineNumber);
#ifndef Q_SCRIPT_NO_EVENT_NOTIFY
    engine()->notifyFunctionExit(context);
#endif
}

QString Function::buildFunction(QScriptContextPrivate *context)
{
    int argc = context->argumentCount();

    QString code;
    code += QLatin1String("function(");

    // the formals
    for (int i = 0; i < argc - 1; ++i) {
        if (i != 0)
            code += QLatin1String(",");

        code += context->argument(i).toString();
    }

    code += QLatin1String("){");

    // the function body
    if (argc != 0)
        code += context->argument(argc - 1).toString();

    code += QLatin1String("\n}");

    return code;
}

void Function::newFunction(QScriptValueImpl *result, QScriptFunction *foo)
{
    engine()->newFunction(result, foo);
}

QScriptValueImpl Function::method_toString(QScriptContextPrivate *context, QScriptEnginePrivate *eng, QScriptClassInfo *)
{
    QScriptValueImpl self = context->thisObject();
    if (QScriptFunction *foo = self.toFunction()) {
        QString code = foo->toString(context);
        return QScriptValueImpl(eng, code);
    }

    return throwThisObjectTypeError(
        context, QLatin1String("Function.prototype.toString"));
}

QScriptValueImpl Function::method_call(QScriptContextPrivate *context, QScriptEnginePrivate *eng, QScriptClassInfo *)
{
    if (! context->thisObject().isFunction()) {
        return throwThisObjectTypeError(
            context, QLatin1String("Function.prototype.call"));
    }

    QScriptValueImpl thisObject = eng->toObject(context->argument(0));
    if (! (thisObject.isValid () && thisObject.isObject()))
        thisObject = eng->globalObject();

    QScriptValueImplList args;
    for (int i = 1; i < context->argumentCount(); ++i)
        args << context->argument(i);

    return context->thisObject().call(thisObject, args);
}

QScriptValueImpl Function::method_apply(QScriptContextPrivate *context, QScriptEnginePrivate *eng, QScriptClassInfo *)
{
    if (! context->thisObject().isFunction()) {
        return throwThisObjectTypeError(
            context, QLatin1String("Function.prototype.apply"));
    }

    QScriptValueImpl thisObject = eng->toObject(context->argument(0));
    if (! (thisObject.isValid () && thisObject.isObject()))
        thisObject = eng->globalObject();

    QScriptValueImplList args;
    QScriptValueImpl undefined = eng->undefinedValue();

    QScriptValueImpl arg = context->argument(1);

    if (Ecma::Array::Instance *arr = eng->arrayConstructor->get(arg)) {
        QScript::Array actuals = arr->value;

        for (quint32 i = 0; i < actuals.count(); ++i) {
            QScriptValueImpl a = actuals.at(i);
            if (! a.isValid())
                args << undefined;
            else
                args << a;
        }
    } else if (arg.classInfo() == eng->m_class_arguments) {
        QScript::ArgumentsObjectData *arguments;
        arguments = static_cast<QScript::ArgumentsObjectData*> (arg.objectData());
        QScriptObject *activation = arguments->activation.objectValue();
        for (uint i = 0; i < arguments->length; ++i)
            args << activation->m_values[i];
    } else if (!(arg.isUndefined() || arg.isNull())) {
        return context->throwError(QScriptContext::TypeError,
                                   QLatin1String("Function.prototype.apply: second argument is not an array"));
    }

    return context->thisObject().call(thisObject, args);
}

QScriptValueImpl Function::method_void(QScriptContextPrivate *, QScriptEnginePrivate *eng, QScriptClassInfo *)
{
    return eng->undefinedValue();
}

QScriptValueImpl Function::method_disconnect(QScriptContextPrivate *context, QScriptEnginePrivate *eng, QScriptClassInfo *)
{
#ifndef QT_NO_QOBJECT
    if (context->argumentCount() == 0) {
        return context->throwError(
            QLatin1String("Function.prototype.disconnect: no arguments given"));
    }

    QScriptValueImpl self = context->thisObject();
    QScriptFunction *fun = self.toFunction();
    if ((fun == 0) || (fun->type() != QScriptFunction::Qt)) {
        return context->throwError(
            QScriptContext::TypeError,
            QLatin1String("Function.prototype.disconnect: this object is not a signal"));
    }

    QtFunction *qtSignal = static_cast<QtFunction*>(fun);

    const QMetaObject *meta = qtSignal->metaObject();
    if (!meta) {
        return context->throwError(
            QScriptContext::TypeError,
            QString::fromLatin1("Function.prototype.disconnect: cannot disconnect from deleted QObject"));
    }

    QMetaMethod sig = meta->method(qtSignal->initialIndex());
    if (sig.methodType() != QMetaMethod::Signal) {
        return context->throwError(QScriptContext::TypeError,
            QString::fromLatin1("Function.prototype.disconnect: %0::%1 is not a signal")
            .arg(QLatin1String(qtSignal->metaObject()->className()))
            .arg(QLatin1String(sig.signature())));
    }

    QScriptValueImpl receiver;
    QScriptValueImpl slot;
    QScriptValueImpl arg0 = context->argument(0);
    if (context->argumentCount() < 2) {
        receiver = QScriptValueImpl();
        slot = arg0;
    } else {
        receiver = arg0;
        QScriptValueImpl arg1 = context->argument(1);
        if (arg1.isFunction())
            slot = arg1;
        else
            slot = receiver.property(arg1.toString(), QScriptValue::ResolvePrototype);
    }

    if (!slot.isFunction()) {
        return context->throwError(
            QScriptContext::TypeError,
            QLatin1String("Function.prototype.disconnect: target is not a function"));
    }

    bool ok = eng->scriptDisconnect(self, receiver, slot);
    if (!ok) {
        return context->throwError(
            QString::fromLatin1("Function.prototype.disconnect: failed to disconnect from %0::%1")
            .arg(QLatin1String(qtSignal->metaObject()->className()))
            .arg(QLatin1String(sig.signature())));
    }
    return eng->undefinedValue();
#else
    Q_UNUSED(eng);
    return context->throwError(QScriptContext::TypeError,
                               QLatin1String("Function.prototype.disconnect"));
#endif // QT_NO_QOBJECT
}

QScriptValueImpl Function::method_connect(QScriptContextPrivate *context, QScriptEnginePrivate *eng, QScriptClassInfo *classInfo)
{
    Q_UNUSED(classInfo);

#ifndef QT_NO_QOBJECT
    if (context->argumentCount() == 0) {
        return context->throwError(
            QLatin1String("Function.prototype.connect: no arguments given"));
    }

    QScriptValueImpl self = context->thisObject();
    QScriptFunction *fun = self.toFunction();
    if ((fun == 0) || (fun->type() != QScriptFunction::Qt)) {
        return context->throwError(
            QScriptContext::TypeError,
            QLatin1String("Function.prototype.connect: this object is not a signal"));
    }

    QtFunction *qtSignal = static_cast<QtFunction*>(fun);

    const QMetaObject *meta = qtSignal->metaObject();
    if (!meta) {
        return context->throwError(
            QScriptContext::TypeError,
            QString::fromLatin1("Function.prototype.connect: cannot connect to deleted QObject"));
    }

    QMetaMethod sig = meta->method(qtSignal->initialIndex());
    if (sig.methodType() != QMetaMethod::Signal) {
        return context->throwError(QScriptContext::TypeError,
            QString::fromLatin1("Function.prototype.connect: %0::%1 is not a signal")
            .arg(QLatin1String(qtSignal->metaObject()->className()))
            .arg(QLatin1String(sig.signature())));
    }

    {
        QList<int> overloads = qtSignal->overloadedIndexes();
        if (!overloads.isEmpty()) {
            overloads.append(qtSignal->initialIndex());
            QByteArray signature = sig.signature();
            QString message = QString::fromLatin1("Function.prototype.connect: ambiguous connect to %0::%1(); candidates are\n")
                              .arg(QLatin1String(qtSignal->metaObject()->className()))
                              .arg(QLatin1String(signature.left(signature.indexOf('('))));
            for (int i = 0; i < overloads.size(); ++i) {
                QMetaMethod mtd = meta->method(overloads.at(i));
                message.append(QString::fromLatin1("    %0\n").arg(QString::fromLatin1(mtd.signature())));
            }
            message.append(QString::fromLatin1("Use e.g. object['%0'].connect() to connect to a particular overload")
                           .arg(QLatin1String(signature)));
            return context->throwError(message);
        }
    }

    QScriptValueImpl receiver;
    QScriptValueImpl slot;
    QScriptValueImpl arg0 = context->argument(0);
    if (context->argumentCount() < 2) {
        receiver = QScriptValueImpl();
        slot = arg0;
    } else {
        receiver = arg0;
        QScriptValueImpl arg1 = context->argument(1);
        if (arg1.isFunction())
            slot = arg1;
        else
            slot = receiver.property(arg1.toString(), QScriptValue::ResolvePrototype);
    }

    if (!slot.isFunction()) {
        return context->throwError(
            QScriptContext::TypeError,
            QLatin1String("Function.prototype.connect: target is not a function"));
    }

    bool ok = eng->scriptConnect(self, receiver, slot);
    if (!ok) {
        return context->throwError(
            QString::fromLatin1("Function.prototype.connect: failed to connect to %0::%1")
            .arg(QLatin1String(qtSignal->metaObject()->className()))
            .arg(QLatin1String(sig.signature())));
    }
    return eng->undefinedValue();
#else
    Q_UNUSED(eng);
    Q_UNUSED(classInfo);
    return context->throwError(QScriptContext::TypeError,
                               QLatin1String("Function.prototype.connect"));
#endif // QT_NO_QOBJECT
}

} } // namespace QScript::Ecma

QT_END_NAMESPACE

#endif // QT_NO_SCRIPT
