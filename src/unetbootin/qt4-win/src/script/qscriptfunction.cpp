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

#include "qscriptfunction_p.h"

#ifndef QT_NO_SCRIPT

#include "qscriptengine_p.h"
#include "qscriptvalueimpl_p.h"
#include "qscriptcontext_p.h"
#include "qscriptmember_p.h"
#include "qscriptobject_p.h"

QT_BEGIN_NAMESPACE

QScriptFunction::~QScriptFunction()
{
}

QString QScriptFunction::toString(QScriptContextPrivate *) const
{
    QString result;
    result += QLatin1String("function () { [native] }");
    return result;
}

QString QScriptFunction::fileName() const
{
    return QString();
}

QString QScriptFunction::functionName() const
{
    return QString();
}

int QScriptFunction::startLineNumber() const
{
    return -1;
}

int QScriptFunction::endLineNumber() const
{
    return -1;
}

void QScriptFunction::mark(QScriptEnginePrivate *engine, int generation)
{
    for (int i = 0; i < formals.count(); ++i)
        engine->markString(formals.at(i), generation);
}

// public API function
void QScript::CFunction::execute(QScriptContextPrivate *context)
{
    QScriptEngine *eng = context->engine();
    QScriptEnginePrivate *eng_p = QScriptEnginePrivate::get(eng);

    eng_p->newUndefined(&context->m_result);

#ifndef Q_SCRIPT_NO_EVENT_NOTIFY
    eng_p->notifyFunctionEntry(context);
#endif

    QScriptValueImpl result = QScriptValuePrivate::valueOf((*m_funPtr)(eng->currentContext(), eng));
    if (result.isValid() && !eng_p->shouldAbort())
        context->m_result = result;

#ifndef Q_SCRIPT_NO_EVENT_NOTIFY
    eng_p->notifyFunctionExit(context);
#endif
}

QString QScript::CFunction::functionName() const
{
    return QString();
}

// internal API function
void QScript::C2Function::execute(QScriptContextPrivate *context)
{
    QScriptEngine *eng = context->engine();
    QScriptEnginePrivate *eng_p = QScriptEnginePrivate::get(eng);

    bool blocked = eng_p->blockGC(true);

#ifndef Q_SCRIPT_NO_EVENT_NOTIFY
    eng_p->notifyFunctionEntry(context);
#endif

    context->m_result = (*m_funPtr)(context, eng_p, m_classInfo);
    Q_ASSERT(context->m_result.isValid());

#ifndef Q_SCRIPT_NO_EVENT_NOTIFY
    eng_p->notifyFunctionExit(context);
#endif

    eng_p->blockGC(blocked);
}

QString QScript::C2Function::functionName() const
{
    if (!m_name.isEmpty())
        return m_name;
    return QString();
}

void QScript::C3Function::execute(QScriptContextPrivate *context)
{
    QScriptEngine *eng = context->engine();
    QScriptEnginePrivate *eng_p = QScriptEnginePrivate::get(eng);

    eng_p->newUndefined(&context->m_result);

#ifndef Q_SCRIPT_NO_EVENT_NOTIFY
    eng_p->notifyFunctionEntry(context);
#endif

    QScriptValueImpl result = QScriptValuePrivate::valueOf((*m_funPtr)(eng->currentContext(), eng, m_arg));
    if (result.isValid() && !eng_p->shouldAbort())
        context->m_result = result;

#ifndef Q_SCRIPT_NO_EVENT_NOTIFY
    eng_p->notifyFunctionExit(context);
#endif
}

QT_END_NAMESPACE

#endif // QT_NO_SCRIPT
