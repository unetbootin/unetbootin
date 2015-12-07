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
        case QVariant::Invalid:
            return eng->undefinedValue();
        case QVariant::String:
            return (QScriptValueImpl(eng, v.toString()));

        case QVariant::Int:
            return (QScriptValueImpl(v.toInt()));

        case QVariant::Bool:
            return (QScriptValueImpl(v.toBool()));

        case QVariant::Double:
            return (QScriptValueImpl(v.toDouble())); // ### hmmm

        case QVariant::Char:
            return (QScriptValueImpl(v.toChar().unicode()));

        case QVariant::UInt:
            return (QScriptValueImpl(v.toUInt()));

        default:
            return context->thisObject();
        } // switch
    }
    return context->thisObject();
}

} } // namespace QScript::Ecma

QT_END_NAMESPACE

#endif // QT_NO_SCRIPT
