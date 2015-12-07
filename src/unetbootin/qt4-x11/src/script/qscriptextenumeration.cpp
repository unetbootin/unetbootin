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

#include "qscriptextenumeration_p.h"

#ifndef QT_NO_SCRIPT

#include "qscriptengine_p.h"
#include "qscriptvalueimpl_p.h"
#include "qscriptcontext_p.h"
#include "qscriptmember_p.h"
#include "qscriptobject_p.h"
#include "qscriptvalueiteratorimpl_p.h"

#include <QtCore/QtDebug>

QT_BEGIN_NAMESPACE

namespace QScript { namespace Ext {

EnumerationClassData::EnumerationClassData(QScriptClassInfo *classInfo):
    m_classInfo(classInfo)
{
}

EnumerationClassData::~EnumerationClassData()
{
}

void EnumerationClassData::mark(const QScriptValueImpl &object, int generation)
{
    Q_ASSERT(object.isValid());

    QScriptEnginePrivate *eng = object.engine();

    if (Enumeration::Instance *instance = Enumeration::Instance::get(object, classInfo())) {
        eng->markObject(instance->object, generation);
        if (instance->it)
            eng->markObject(instance->it->object(), generation);
    }
}

Enumeration::Enumeration(QScriptEnginePrivate *eng):
    Ecma::Core(eng, QLatin1String("Enumeration"), QScriptClassInfo::EnumerationType)
{
    classInfo()->setData(new EnumerationClassData(classInfo()));

    newEnumeration(&publicPrototype, eng->newArray());

    eng->newConstructor(&ctor, this, publicPrototype);

    addPrototypeFunction(QLatin1String("toFront"), method_toFront, 0);
    addPrototypeFunction(QLatin1String("hasNext"), method_hasNext, 0);
    addPrototypeFunction(QLatin1String("next"), method_next, 0);
}

Enumeration::~Enumeration()
{
}

Enumeration::Instance *Enumeration::Instance::get(const QScriptValueImpl &object, QScriptClassInfo *klass)
{
    if (! klass || klass == object.classInfo())
        return static_cast<Instance*> (object.objectData());

    return 0;
}

void Enumeration::execute(QScriptContextPrivate *context)
{
    if (context->argumentCount() > 0) {
        newEnumeration(&context->m_result, context->argument(0));
    } else {
        context->throwError(QScriptContext::TypeError,
                            QLatin1String("Enumeration.execute"));
    }
}

void Enumeration::newEnumeration(QScriptValueImpl *result, const QScriptValueImpl &object)
{
    Instance *instance = new Instance();
    instance->object = object;
    if (object.isObject()) {
        instance->it = new QScriptValueIteratorImpl(object);
        instance->it->setIgnoresDontEnum(false);
        instance->it->setEnumeratePrototype(true);
    } else {
        instance->it = 0;
    }
    engine()->newObject(result, publicPrototype, classInfo());
    result->setObjectData(instance);
}

QScriptValueImpl Enumeration::method_toFront(QScriptContextPrivate *context, QScriptEnginePrivate *eng, QScriptClassInfo *classInfo)
{
    if (Instance *instance = Instance::get(context->thisObject(), classInfo)) {
        if (instance->it)
            instance->it->toFront();
        return eng->undefinedValue();
    } else {
        return context->throwError(QScriptContext::TypeError,
                                   QLatin1String("Enumeration.toFront"));
    }
}

QScriptValueImpl Enumeration::method_hasNext(QScriptContextPrivate *context, QScriptEnginePrivate *, QScriptClassInfo *classInfo)
{
    Instance *instance = Instance::get(context->thisObject(), classInfo);
    if (!instance) {
        return context->throwError(QScriptContext::TypeError,
                                   QLatin1String("Enumeration.hasNext"));
    }

    QScriptValueImpl v;
    instance->hasNext(context, &v);
    return v;
}

QScriptValueImpl Enumeration::method_next(QScriptContextPrivate *context, QScriptEnginePrivate *, QScriptClassInfo *classInfo)
{
    Instance *instance = Instance::get(context->thisObject(), classInfo);
    if (!instance) {
        return context->throwError(QScriptContext::TypeError,
                                   QLatin1String("Enumeration.next"));
    }

    QScriptValueImpl v;
    instance->next(context, &v);
    return v;
}

Enumeration::Instance::~Instance()
{
    if (it) {
        delete it;
        it = 0;
    }
}

void Enumeration::Instance::toFront()
{
    if (it)
        it->toFront();
}

void Enumeration::Instance::hasNext(QScriptContextPrivate *, QScriptValueImpl *result)
{
    *result = QScriptValueImpl(it && it->hasNext());
}

void Enumeration::Instance::next(QScriptContextPrivate *context, QScriptValueImpl *result)
{
    QScriptEnginePrivate *eng = context->engine();
    Q_ASSERT(it != 0);
    it->next();
    QScript::Member *member = it->member();
    if (member->isObjectProperty() || member->nameId())
        eng->newNameId(result, member->nameId());

    else if (member->isNativeProperty() && !member->nameId())
        *result = QScriptValueImpl(uint(member->id()));

    else
        *result = eng->undefinedValue();
}

} } // namespace QScript::Ext

QT_END_NAMESPACE

#endif // QT_NO_SCRIPT
