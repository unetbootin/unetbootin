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

#include "qscriptecmaobject_p.h"

#ifndef QT_NO_SCRIPT

#include "qscriptengine_p.h"
#include "qscriptvalueimpl_p.h"
#include "qscriptcontext_p.h"
#include "qscriptmember_p.h"
#include "qscriptobject_p.h"

#include <QtCore/QtDebug>

QT_BEGIN_NAMESPACE

namespace QScript { namespace Ecma {

Object::Object(QScriptEnginePrivate *eng, QScriptClassInfo *classInfo):
    Core(eng, classInfo)
{
    newObject(&publicPrototype, eng->nullValue());
}

Object::~Object()
{
}

void Object::initialize()
{
    QScriptEnginePrivate *eng = engine();

    eng->newConstructor(&ctor, this, publicPrototype);

    addPrototypeFunction(QLatin1String("toString"), method_toString, 1);
    addPrototypeFunction(QLatin1String("toLocaleString"), method_toLocaleString, 1);
    addPrototypeFunction(QLatin1String("valueOf"), method_valueOf, 0);
    addPrototypeFunction(QLatin1String("hasOwnProperty"), method_hasOwnProperty, 1);
    addPrototypeFunction(QLatin1String("isPrototypeOf"), method_isPrototypeOf, 1);
    addPrototypeFunction(QLatin1String("propertyIsEnumerable"), method_propertyIsEnumerable, 1);
    addPrototypeFunction(QLatin1String("__defineGetter__"), method_defineGetter, 2);
    addPrototypeFunction(QLatin1String("__defineSetter__"), method_defineSetter, 2);
}

void Object::execute(QScriptContextPrivate *context)
{
#ifndef Q_SCRIPT_NO_EVENT_NOTIFY
    engine()->notifyFunctionEntry(context);
#endif
    QScriptValueImpl value;

    if (context->argumentCount() > 0)
        value = engine()->toObject(context->argument(0));
    else
        value.invalidate();

    if (! value.isValid())
        newObject(&value);

    context->setReturnValue(value);
#ifndef Q_SCRIPT_NO_EVENT_NOTIFY
    engine()->notifyFunctionExit(context);
#endif
}

void Object::newObject(QScriptValueImpl *result, const QScriptValueImpl &proto)
{
    engine()->newObject(result, proto, classInfo());
}

QScriptValueImpl Object::method_toString(QScriptContextPrivate *context, QScriptEnginePrivate *eng, QScriptClassInfo *)
{
    QScriptValueImpl glo = eng->globalObject();
    QString s = QLatin1String("[object ");
    QScriptValueImpl self = context->thisObject();
    s += self.classInfo()->name();
    s += QLatin1String("]");
    return (QScriptValueImpl(eng, s));
}

QScriptValueImpl Object::method_toLocaleString(QScriptContextPrivate *context, QScriptEnginePrivate *eng, QScriptClassInfo *classInfo)
{
    return method_toString(context, eng, classInfo);
}

QScriptValueImpl Object::method_valueOf(QScriptContextPrivate *context, QScriptEnginePrivate *, QScriptClassInfo *)
{
    return (context->thisObject());
}

QScriptValueImpl Object::method_hasOwnProperty(QScriptContextPrivate *context, QScriptEnginePrivate *eng, QScriptClassInfo *)
{
    bool result = false;

    if (context->thisObject().isObject() && (context->argumentCount() > 0)) {
        QScriptValueImpl arg = context->argument(0);

        QScriptNameIdImpl *id = 0;
        if (arg.isString())
            id = arg.stringValue();

        if (! id || ! id->unique) {
            QString str = arg.toString();
            id = eng->nameId(str);
        }

        QScript::Member member;
        QScriptValueImpl base;
        QScriptValueImpl self = context->thisObject();
        if (self.resolve(id, &member, &base, QScriptValue::ResolveLocal, QScript::Read))
            result = true;
    }

    return (QScriptValueImpl(result));
}

QScriptValueImpl Object::method_isPrototypeOf(QScriptContextPrivate *context, QScriptEnginePrivate *, QScriptClassInfo *)
{
    bool result = false;

    if (context->thisObject().isObject() && (context->argumentCount() > 0)) {
        QScriptValueImpl arg = context->argument(0);

        if (arg.isObject()) {
            QScriptValueImpl proto = arg.prototype();

            if (proto.isObject()) {
                QScriptValueImpl self = context->thisObject();
                result = self.objectValue() == proto.objectValue();
            }
        }
    }

    return (QScriptValueImpl(result));
}

QScriptValueImpl Object::method_propertyIsEnumerable(QScriptContextPrivate *context, QScriptEnginePrivate *eng, QScriptClassInfo *)
{
    bool result = false;

    if (context->thisObject().isObject() && (context->argumentCount() > 0)) {
        QScriptValueImpl arg = context->argument(0);

        QScriptNameIdImpl *id = 0;
        if (arg.isString())
            id = arg.stringValue();

        if (! id || ! id->unique) {
            QString str = arg.toString();
            id = eng->nameId(str);
        }

        QScript::Member member;
        QScriptValueImpl base;
        QScriptValueImpl self = context->thisObject();
        if (self.resolve(id, &member, &base, QScriptValue::ResolveLocal, QScript::Read)) {
            result = ! member.dontEnum();
            if (result) {
                QScriptValueImpl tmp;
                base.get(member, &tmp);
                result = tmp.isValid();
            }
        }
    }

    return (QScriptValueImpl(result));
}

QScriptValueImpl Object::method_defineGetter(QScriptContextPrivate *context, QScriptEnginePrivate *eng,
                                             QScriptClassInfo *)
{
    QString propertyName = context->argument(0).toString();
    if (context->thisObject().propertyFlags(propertyName) & QScriptValue::ReadOnly)
        return context->throwError(QLatin1String("cannot redefine read-only property"));
    QScriptValueImpl getter = context->argument(1);
    if (!getter.isFunction())
        return context->throwError(QLatin1String("getter must be a function"));
    context->thisObject().setProperty(propertyName, getter, QScriptValue::PropertyGetter);
    return eng->undefinedValue();
}

QScriptValueImpl Object::method_defineSetter(QScriptContextPrivate *context, QScriptEnginePrivate *eng,
                                             QScriptClassInfo *)
{
    QString propertyName = context->argument(0).toString();
    if (context->thisObject().propertyFlags(propertyName) & QScriptValue::ReadOnly)
        return context->throwError(QLatin1String("cannot redefine read-only property"));
    QScriptValueImpl setter = context->argument(1);
    if (!setter.isFunction())
        return context->throwError(QLatin1String("setter must be a function"));
    context->thisObject().setProperty(propertyName, setter, QScriptValue::PropertySetter);
    return eng->undefinedValue();
}

} } // namespace QScript::Ecma

QT_END_NAMESPACE

#endif // QT_NO_SCRIPT
