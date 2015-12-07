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

#include "qscriptextvariant_p.h"

#ifndef QT_NO_SCRIPT

#include "qscriptengine_p.h"
#include "qscriptvalueimpl_p.h"
#include "qscriptcontext_p.h"
#include "qscriptmember_p.h"
#include "qscriptobject_p.h"

#include <QtCore/QtDebug>

#include <QtCore/QStringList>

#include <limits.h>

QT_BEGIN_NAMESPACE

namespace QScript { namespace Ext {

Variant::Variant(QScriptEnginePrivate *eng):
    Ecma::Core(eng, QLatin1String("QVariant"), QScriptClassInfo::VariantType)
{
    newVariant(&publicPrototype, QVariant());

    eng->newConstructor(&ctor, this, publicPrototype);

    addPrototypeFunction(QLatin1String("toString"), method_toString, 0);
    addPrototypeFunction(QLatin1String("valueOf"), method_valueOf, 0);
}

Variant::~Variant()
{
}

Variant::Instance *Variant::Instance::get(const QScriptValueImpl &object, QScriptClassInfo *klass)
{
    if (! klass || klass == object.classInfo())
        return static_cast<Instance*> (object.objectData());

    return 0;
}

void Variant::execute(QScriptContextPrivate *context)
{
    QScriptValueImpl tmp;
    newVariant(&tmp, QVariant());
    context->setReturnValue(tmp);
}

void Variant::newVariant(QScriptValueImpl *result, const QVariant &value)
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
            instance = Instance::get(*result, classInfo());
        }
    }
    instance->value = value;
}

QScriptValueImpl Variant::method_toString(QScriptContextPrivate *context, QScriptEnginePrivate *eng, QScriptClassInfo *classInfo)
{
    if (Instance *instance = Instance::get(context->thisObject(), classInfo)) {
        QString result;
        QScriptValueImpl value = method_valueOf(context, eng, classInfo);
        if (value.isObject()) {
            result = instance->value.toString();
            if (result.isEmpty()) {
                result = QString::fromLatin1("QVariant(%0)")
                         .arg(QLatin1String(instance->value.typeName()));
            }
        } else {
            result = value.toString();
        }
        return QScriptValueImpl(eng, result);
    }
    return context->throwError(QScriptContext::TypeError,
                               QLatin1String("QVariant.prototype.toString"));
}

QScriptValueImpl Variant::method_valueOf(QScriptContextPrivate *context, QScriptEnginePrivate *eng, QScriptClassInfo *classInfo)
{
    if (Instance *instance = Instance::get(context->thisObject(), classInfo)) {
        QVariant v = instance->value;
        switch (v.type ()) {
        case QVariant::String:
            return (QScriptValueImpl(eng, v.toString()));

        case QVariant::Int:
            return (QScriptValueImpl(eng, v.toInt()));

        case QVariant::Bool:
            return (QScriptValueImpl(eng, v.toBool()));

        case QVariant::Double:
            return (QScriptValueImpl(eng, v.toDouble())); // ### hmmm

        case QVariant::Char:
            return (QScriptValueImpl(eng, v.toChar().unicode()));

        case QVariant::UInt:
            return (QScriptValueImpl(eng, v.toUInt()));

        default:
            return context->thisObject();
        } // switch
    }
    return context->thisObject();
}

} } // namespace QScript::Ecma

QT_END_NAMESPACE

#endif // QT_NO_SCRIPT
