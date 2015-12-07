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
                         QScript::Member *member, QScriptValueImpl *base);
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
                                QScript::Member *member, QScriptValueImpl *base)
{
    if (object.classInfo() != classInfo())
        return false;

    QScriptEnginePrivate *eng = QScriptEnginePrivate::get(object.engine());

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

    QScriptEnginePrivate *eng = QScriptEnginePrivate::get(object.engine());
    if (! member.isNativeProperty())
        return false;

    if (member.nameId() == eng->idTable()->id_length) {
        eng->newNumber(result, object.toFunction()->length);
        return true;
    } else if (member.nameId() == eng->idTable()->id_arguments) {
        eng->newNull(result);
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
    QScriptEnginePrivate *eng = QScriptEnginePrivate::get(object.engine());
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
    addPrototypeFunction(QLatin1String("apply"), method_apply, 1);
    addPrototypeFunction(QLatin1String("call"), method_call, 1);
    addPrototypeFunction(QLatin1String("connect"), method_connect, 1);
    addPrototypeFunction(QLatin1String("disconnect"), method_disconnect, 1);

    QExplicitlySharedDataPointer<QScriptClassData> data(new FunctionClassData(classInfo()));
    classInfo()->setData(data);
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
    code += QLatin1String("return function(");

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

    return context->throwError(QScriptContext::TypeError,
                               QLatin1String("Function.prototype.toString"));
}

QScriptValueImpl Function::method_call(QScriptContextPrivate *context, QScriptEnginePrivate *eng, QScriptClassInfo *)
{
    if (! context->thisObject().isFunction()) {
        return context->throwError(QScriptContext::TypeError,
                                   QLatin1String("Function.prototype.call"));
    }

    QScriptValueImpl thisObject = context->argument(0).toObject();
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
        return context->throwError(QScriptContext::TypeError,
                                   QLatin1String("Function.prototype.apply"));
    }

    QScriptValueImpl thisObject = context->argument(0).toObject();
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
            args << activation->m_objects[i];
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
