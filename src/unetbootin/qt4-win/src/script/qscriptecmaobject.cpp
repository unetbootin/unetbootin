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
        value = context->argument(0).toObject();
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
    if (self.objectValue() == glo.objectValue())
        s += QLatin1String("global");
    else
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
        if (self.resolve(id, &member, &base, QScriptValue::ResolveLocal))
            result = true;
    }

    return (QScriptValueImpl(eng, result));
}

QScriptValueImpl Object::method_isPrototypeOf(QScriptContextPrivate *context, QScriptEnginePrivate *eng, QScriptClassInfo *)
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

    return (QScriptValueImpl(eng, result));
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
        if (self.resolve(id, &member, &base, QScriptValue::ResolveLocal)) {
            result = ! member.dontEnum();
            if (result) {
                QScriptValueImpl tmp;
                base.get(member, &tmp);
                result = tmp.isValid();
            }
        }
    }

    return (QScriptValueImpl(eng, result));
}

QScriptValueImpl Object::method_defineGetter(QScriptContextPrivate *context, QScriptEnginePrivate *eng,
                                             QScriptClassInfo *)
{
    QString propertyName = context->argument(0).toString();
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
    QScriptValueImpl setter = context->argument(1);
    if (!setter.isFunction())
        return context->throwError(QLatin1String("setter must be a function"));
    context->thisObject().setProperty(propertyName, setter, QScriptValue::PropertySetter);
    return eng->undefinedValue();
}

} } // namespace QScript::Ecma

QT_END_NAMESPACE

#endif // QT_NO_SCRIPT
