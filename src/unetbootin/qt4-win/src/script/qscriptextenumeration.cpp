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

    QScriptEnginePrivate *eng = QScriptEnginePrivate::get(object.engine());

    if (Enumeration::Instance *instance = Enumeration::Instance::get(object, classInfo())) {
        eng->markObject(instance->object, generation);
        eng->markObject(instance->it->object(), generation);
    }
}

Enumeration::Enumeration(QScriptEnginePrivate *eng):
    Ecma::Core(eng, QLatin1String("Enumeration"), QScriptClassInfo::EnumerationType)
{
    QExplicitlySharedDataPointer<QScriptClassData> data(new EnumerationClassData(classInfo()));
    classInfo()->setData(data);

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
    instance->it = new QScriptValueIteratorImpl(object);
    instance->it->setIgnoresDontEnum(false);
    instance->it->setEnumeratePrototype(true);

    engine()->newObject(result, publicPrototype, classInfo());
    result->setObjectData(instance);
}

QScriptValueImpl Enumeration::method_toFront(QScriptContextPrivate *context, QScriptEnginePrivate *eng, QScriptClassInfo *classInfo)
{
    if (Instance *instance = Instance::get(context->thisObject(), classInfo)) {
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
    it->toFront();
}

void Enumeration::Instance::hasNext(QScriptContextPrivate *context, QScriptValueImpl *result)
{
    QScriptEnginePrivate *eng = QScriptEnginePrivate::get(context->engine());
    *result = QScriptValueImpl(eng, it->hasNext());
}

void Enumeration::Instance::next(QScriptContextPrivate *context, QScriptValueImpl *result)
{
    QScriptEnginePrivate *eng = QScriptEnginePrivate::get(context->engine());
    it->next();
    QScript::Member *member = it->member();
    if (member->isObjectProperty() || member->nameId())
        eng->newNameId(result, member->nameId());

    else if (member->isNativeProperty() && !member->nameId())
        eng->newNumber(result, uint(member->id()));

    else
        eng->newUndefined(result);
}

} } // namespace QScript::Ext

QT_END_NAMESPACE

#endif // QT_NO_SCRIPT
