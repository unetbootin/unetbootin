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

#include "qscriptecmacore_p.h"

#ifndef QT_NO_SCRIPT

#include "qscriptengine_p.h"
#include "qscriptvalueimpl_p.h"
#include "qscriptcontext_p.h"
#include "qscriptmember_p.h"
#include "qscriptobject_p.h"

QT_BEGIN_NAMESPACE

namespace QScript { namespace Ecma {

Core::Core(QScriptEnginePrivate *engine, const QString &className,
           QScriptClassInfo::Type type)
    : m_engine(engine)
{
    m_classInfo = engine->registerClass(className, type);
    this->length = 1;
}

Core::Core(QScriptEnginePrivate *engine, QScriptClassInfo *classInfo)
    : m_engine(engine), m_classInfo(classInfo)
{
    this->length = 1;
}

Core::~Core()
{
}

void Core::addPrototypeFunction(const QString &name, QScriptInternalFunctionSignature fun,
                                int length, const QScriptValue::PropertyFlags flags)
{
    addFunction(publicPrototype, name, fun, length, flags);
}

void Core::addConstructorFunction(const QString &name, QScriptInternalFunctionSignature fun,
                                  int length, const QScriptValue::PropertyFlags flags)
{
    addFunction(ctor, name, fun, length, flags);
}

void Core::addFunction(QScriptValueImpl &object, const QString &name,
                       QScriptInternalFunctionSignature fun, int length,
                       const QScriptValue::PropertyFlags flags)
{
    QScriptValueImpl val = engine()->createFunction(fun, length, m_classInfo, name);
    object.setProperty(name, val, flags);
}

QString Core::functionName() const
{
    return m_classInfo->name();
}

void Core::mark(QScriptEnginePrivate *eng, int generation)
{
    QScriptFunction::mark(eng, generation);
    eng->markObject(ctor, generation);
    eng->markObject(publicPrototype, generation);
}

} // namespace Ecma


} // namespace QScript

QT_END_NAMESPACE

#endif // QT_NO_SCRIPT
