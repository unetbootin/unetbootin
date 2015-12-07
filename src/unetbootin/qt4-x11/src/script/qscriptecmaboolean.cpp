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

#include "qscriptecmaboolean_p.h"

#ifndef QT_NO_SCRIPT

#include "qscriptengine_p.h"
#include "qscriptvalueimpl_p.h"
#include "qscriptcontext_p.h"
#include "qscriptmember_p.h"
#include "qscriptobject_p.h"

#include <QtCore/QtDebug>

QT_BEGIN_NAMESPACE

namespace QScript { namespace Ecma {

Boolean::Boolean(QScriptEnginePrivate *eng):
    Core(eng, QLatin1String("Boolean"), QScriptClassInfo::BooleanType)
{
    newBoolean(&publicPrototype, false);

    eng->newConstructor(&ctor, this, publicPrototype);

    addPrototypeFunction(QLatin1String("toString"), method_toString, 0);
    addPrototypeFunction(QLatin1String("valueOf"), method_valueOf, 0);
}

Boolean::~Boolean()
{
}

void Boolean::execute(QScriptContextPrivate *context)
{
#ifndef Q_SCRIPT_NO_EVENT_NOTIFY
    engine()->notifyFunctionEntry(context);
#endif
    bool value;
    if (context->argumentCount() > 0)
        value = context->argument(0).toBoolean();
    else
        value = false;

    QScriptValueImpl boolean(value);
    if (!context->isCalledAsConstructor()) {
        context->setReturnValue(boolean);
    } else {
        QScriptValueImpl &obj = context->m_thisObject;
        obj.setClassInfo(classInfo());
        obj.setInternalValue(boolean);
        obj.setPrototype(publicPrototype);
        context->setReturnValue(obj);
    }
#ifndef Q_SCRIPT_NO_EVENT_NOTIFY
    engine()->notifyFunctionExit(context);
#endif
}

void Boolean::newBoolean(QScriptValueImpl *result, bool value)
{
    engine()->newObject(result, publicPrototype, classInfo());
    result->setInternalValue(QScriptValueImpl(value));
}

QScriptValueImpl Boolean::method_toString(QScriptContextPrivate *context,
                                          QScriptEnginePrivate *eng,
                                          QScriptClassInfo *classInfo)
{
    QScriptValueImpl self = context->thisObject();
    if (self.classInfo() != classInfo) {
        return throwThisObjectTypeError(
            context, QLatin1String("Boolean.prototype.toString"));
    }
    const QScript::IdTable *t = eng->idTable();
    bool v = self.internalValue().toBoolean();
    QScriptValueImpl result;
    eng->newNameId(&result, v ? t->id_true : t->id_false);
    return result;
}

QScriptValueImpl Boolean::method_valueOf(QScriptContextPrivate *context,
                                         QScriptEnginePrivate *,
                                         QScriptClassInfo *classInfo)
{
    QScriptValueImpl self = context->thisObject();
    if (self.classInfo() != classInfo) {
        return throwThisObjectTypeError(
            context, QLatin1String("Boolean.prototype.valueOf"));
    }
    return self.internalValue();
}

} } // namespace QScript::Ecma

QT_END_NAMESPACE

#endif // QT_NO_SCRIPT
