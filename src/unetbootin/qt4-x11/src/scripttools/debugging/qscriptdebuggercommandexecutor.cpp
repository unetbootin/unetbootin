/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtScriptTools module of the Qt Toolkit.
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

#include "qscriptdebuggercommandexecutor_p.h"

#include "qscriptdebuggerbackend_p.h"
#include "qscriptdebuggercommand_p.h"
#include "qscriptdebuggerresponse_p.h"
#include "qscriptdebuggervalue_p.h"
#include "qscriptdebuggervalueproperty_p.h"
#include "qscriptbreakpointdata_p.h"
#include "qscriptobjectsnapshot_p.h"
#include "qscriptdebuggerobjectsnapshotdelta_p.h"

#include <QtCore/qstringlist.h>
#include <QtScript/qscriptengine.h>
#include <QtScript/qscriptcontextinfo.h>
#include <QtScript/qscriptvalueiterator.h>
#include <QtCore/qdebug.h>

Q_DECLARE_METATYPE(QScriptScriptsDelta)
Q_DECLARE_METATYPE(QScriptDebuggerValueProperty)
Q_DECLARE_METATYPE(QScriptDebuggerValuePropertyList)
Q_DECLARE_METATYPE(QScriptDebuggerObjectSnapshotDelta)

QT_BEGIN_NAMESPACE

/*!
  \since 4.5
  \class QScriptDebuggerCommandExecutor
  \internal

  \brief The QScriptDebuggerCommandExecutor applies debugger commands to a back-end.

  The execute() function takes a command (typically produced by a
  QScriptDebuggerFrontend) and applies it to a QScriptDebuggerBackend.

  \sa QScriptDebuggerCommmand
*/

class QScriptDebuggerCommandExecutorPrivate
{
public:
    QScriptDebuggerCommandExecutorPrivate();
    ~QScriptDebuggerCommandExecutorPrivate();
};

QScriptDebuggerCommandExecutorPrivate::QScriptDebuggerCommandExecutorPrivate()
{
}

QScriptDebuggerCommandExecutorPrivate::~QScriptDebuggerCommandExecutorPrivate()
{
}

QScriptDebuggerCommandExecutor::QScriptDebuggerCommandExecutor()
    : d_ptr(new QScriptDebuggerCommandExecutorPrivate())
{
}

QScriptDebuggerCommandExecutor::~QScriptDebuggerCommandExecutor()
{
    delete d_ptr;
}

/*!
  Applies the given \a command to the given \a backend.
*/
QScriptDebuggerResponse QScriptDebuggerCommandExecutor::execute(
        QScriptDebuggerBackend *backend,
        const QScriptDebuggerCommand &command)
{
    QScriptDebuggerResponse response;
    switch (command.type()) {
    case QScriptDebuggerCommand::None:
        break;

    case QScriptDebuggerCommand::Interrupt:
        backend->interruptEvaluation();
        break;

    case QScriptDebuggerCommand::Continue:
        if (backend->engine()->isEvaluating()) {
            backend->continueEvalution();
            response.setAsync(true);
        }
        break;

    case QScriptDebuggerCommand::StepInto: {
        QVariant attr = command.attribute(QScriptDebuggerCommand::StepCount);
        int count = attr.isValid() ? attr.toInt() : 1;
        backend->stepInto(count);
        response.setAsync(true);
    }   break;

    case QScriptDebuggerCommand::StepOver: {
        QVariant attr = command.attribute(QScriptDebuggerCommand::StepCount);
        int count = attr.isValid() ? attr.toInt() : 1;
        backend->stepOver(count);
        response.setAsync(true);
    }   break;

    case QScriptDebuggerCommand::StepOut:
        backend->stepOut();
        response.setAsync(true);
        break;

    case QScriptDebuggerCommand::RunToLocation:
        backend->runToLocation(command.fileName(), command.lineNumber());
        response.setAsync(true);
        break;

    case QScriptDebuggerCommand::RunToLocationByID:
        backend->runToLocation(command.scriptId(), command.lineNumber());
        response.setAsync(true);
        break;

    case QScriptDebuggerCommand::ForceReturn: {
        int contextIndex = command.contextIndex();
        QScriptDebuggerValue value = command.scriptValue();
        QScriptEngine *engine = backend->engine();
        QScriptValue realValue = value.toScriptValue(engine);
        backend->returnToCaller(contextIndex, realValue);
        response.setAsync(true);
    }   break;

    case QScriptDebuggerCommand::Resume:
        backend->resume();
        response.setAsync(true);
        break;

    case QScriptDebuggerCommand::SetBreakpoint: {
        QScriptBreakpointData data = command.breakpointData();
        if (!data.isValid())
            data = QScriptBreakpointData(command.fileName(), command.lineNumber());
        int id = backend->setBreakpoint(data);
        response.setResult(id);
    }   break;

    case QScriptDebuggerCommand::DeleteBreakpoint: {
        int id = command.breakpointId();
        if (!backend->deleteBreakpoint(id))
            response.setError(QScriptDebuggerResponse::InvalidBreakpointID);
    }   break;

    case QScriptDebuggerCommand::DeleteAllBreakpoints:
        backend->deleteAllBreakpoints();
        break;

    case QScriptDebuggerCommand::GetBreakpoints: {
        QScriptBreakpointMap bps = backend->breakpoints();
        if (!bps.isEmpty())
            response.setResult(bps);
    }   break;

    case QScriptDebuggerCommand::GetBreakpointData: {
        int id = command.breakpointId();
        QScriptBreakpointData data = backend->breakpointData(id);
        if (data.isValid())
            response.setResult(data);
        else
            response.setError(QScriptDebuggerResponse::InvalidBreakpointID);
    }   break;

    case QScriptDebuggerCommand::SetBreakpointData: {
        int id = command.breakpointId();
        QScriptBreakpointData data = command.breakpointData();
        if (!backend->setBreakpointData(id, data))
            response.setError(QScriptDebuggerResponse::InvalidBreakpointID);
    }   break;

    case QScriptDebuggerCommand::GetScripts: {
        QScriptScriptMap scripts = backend->scripts();
        if (!scripts.isEmpty())
            response.setResult(scripts);
    }   break;

    case QScriptDebuggerCommand::GetScriptData: {
        qint64 id = command.scriptId();
        QScriptScriptData data = backend->scriptData(id);
        if (data.isValid())
            response.setResult(data);
        else
            response.setError(QScriptDebuggerResponse::InvalidScriptID);
    }   break;

    case QScriptDebuggerCommand::ScriptsCheckpoint:
        backend->scriptsCheckpoint();
        response.setResult(qVariantFromValue(backend->scriptsDelta()));
        break;

    case QScriptDebuggerCommand::GetScriptsDelta:
        response.setResult(qVariantFromValue(backend->scriptsDelta()));
        break;

    case QScriptDebuggerCommand::ResolveScript:
        response.setResult(backend->resolveScript(command.fileName()));
        break;

    case QScriptDebuggerCommand::GetBacktrace:
        response.setResult(backend->backtrace());
        break;

    case QScriptDebuggerCommand::GetContextCount:
        response.setResult(backend->contextCount());
        break;

    case QScriptDebuggerCommand::GetContextState: {
        QScriptContext *ctx = backend->context(command.contextIndex());
        if (ctx)
            response.setResult(static_cast<int>(ctx->state()));
        else
            response.setError(QScriptDebuggerResponse::InvalidContextIndex);
    }   break;

    case QScriptDebuggerCommand::GetContextID: {
        int idx = command.contextIndex();
        if ((idx >= 0) && (idx < backend->contextCount()))
            response.setResult(backend->contextIds()[idx]);
        else
            response.setError(QScriptDebuggerResponse::InvalidContextIndex);
    }   break;

    case QScriptDebuggerCommand::GetContextInfo: {
        QScriptContext *ctx = backend->context(command.contextIndex());
        if (ctx)
            response.setResult(QScriptContextInfo(ctx));
        else
            response.setError(QScriptDebuggerResponse::InvalidContextIndex);
    }   break;

    case QScriptDebuggerCommand::GetThisObject: {
        QScriptContext *ctx = backend->context(command.contextIndex());
        if (ctx)
            response.setResult(ctx->thisObject());
        else
            response.setError(QScriptDebuggerResponse::InvalidContextIndex);
    }   break;

    case QScriptDebuggerCommand::GetActivationObject: {
        QScriptContext *ctx = backend->context(command.contextIndex());
        if (ctx)
            response.setResult(ctx->activationObject());
        else
            response.setError(QScriptDebuggerResponse::InvalidContextIndex);
    }   break;

    case QScriptDebuggerCommand::GetScopeChain: {
        QScriptContext *ctx = backend->context(command.contextIndex());
        if (ctx) {
            QScriptDebuggerValueList dest;
#if QT_VERSION >= 0x040500
            QScriptValueList src = ctx->scopeChain();
            for (int i = 0; i < src.size(); ++i)
                dest.append(src.at(i));
#else
            dest.append(ctx->activationObject());
#endif
            response.setResult(dest);
        } else {
            response.setError(QScriptDebuggerResponse::InvalidContextIndex);
        }
    }   break;

    case QScriptDebuggerCommand::ContextsCheckpoint: {
        response.setResult(qVariantFromValue(backend->contextsCheckpoint()));
    }   break;

    case QScriptDebuggerCommand::NewScriptObjectSnapshot: {
        int id = backend->newScriptObjectSnapshot();
        response.setResult(id);
    }   break;

    case QScriptDebuggerCommand::ScriptObjectSnapshotCapture: {
        int id = command.snapshotId();
        QScriptObjectSnapshot *snap = backend->scriptObjectSnapshot(id);
        Q_ASSERT(snap != 0);
        QScriptDebuggerValue object = command.scriptValue();
        Q_ASSERT(object.type() == QScriptDebuggerValue::ObjectValue);
        QScriptEngine *engine = backend->engine();
        QScriptValue realObject = object.toScriptValue(engine);
        Q_ASSERT(realObject.isObject());
        QScriptObjectSnapshot::Delta delta = snap->capture(realObject);
        QScriptDebuggerObjectSnapshotDelta result;
        result.removedProperties = delta.removedProperties;
        bool didIgnoreExceptions = backend->ignoreExceptions();
        backend->setIgnoreExceptions(true);
        for (int i = 0; i < delta.changedProperties.size(); ++i) {
            const QScriptValueProperty &src = delta.changedProperties.at(i);
            bool hadException = engine->hasUncaughtException();
            QString str = src.value().toString();
            if (!hadException && engine->hasUncaughtException())
                engine->clearExceptions();
            QScriptDebuggerValueProperty dest(src.name(), src.value(), str, src.flags());
            result.changedProperties.append(dest);
        }
        for (int j = 0; j < delta.addedProperties.size(); ++j) {
            const QScriptValueProperty &src = delta.addedProperties.at(j);
            bool hadException = engine->hasUncaughtException();
            QString str = src.value().toString();
            if (!hadException && engine->hasUncaughtException())
                engine->clearExceptions();
            QScriptDebuggerValueProperty dest(src.name(), src.value(), str, src.flags());
            result.addedProperties.append(dest);
        }
        backend->setIgnoreExceptions(didIgnoreExceptions);
        response.setResult(qVariantFromValue(result));
    }   break;

    case QScriptDebuggerCommand::DeleteScriptObjectSnapshot: {
        int id = command.snapshotId();
        backend->deleteScriptObjectSnapshot(id);
    }   break;

    case QScriptDebuggerCommand::NewScriptValueIterator: {
        QScriptDebuggerValue object = command.scriptValue();
        Q_ASSERT(object.type() == QScriptDebuggerValue::ObjectValue);
        QScriptEngine *engine = backend->engine();
        QScriptValue realObject = object.toScriptValue(engine);
        Q_ASSERT(realObject.isObject());
        int id = backend->newScriptValueIterator(realObject);
        response.setResult(id);
    }   break;

    case QScriptDebuggerCommand::GetPropertiesByIterator: {
        int id = command.iteratorId();
        int count = 1000;
        QScriptValueIterator *it = backend->scriptValueIterator(id);
        Q_ASSERT(it != 0);
        QScriptDebuggerValuePropertyList props;
        for (int i = 0; (i < count) && it->hasNext(); ++i) {
            it->next();
            QString name = it->name();
            QScriptValue value = it->value();
            QString valueAsString = value.toString();
            QScriptValue::PropertyFlags flags = it->flags();
            QScriptDebuggerValueProperty prp(name, value, valueAsString, flags);
            props.append(prp);
        }
        response.setResult(props);
    }   break;

    case QScriptDebuggerCommand::DeleteScriptValueIterator: {
        int id = command.iteratorId();
        backend->deleteScriptValueIterator(id);
    }   break;

    case QScriptDebuggerCommand::Evaluate: {
        int contextIndex = command.contextIndex();
        QString program = command.program();
        QString fileName = command.fileName();
        int lineNumber = command.lineNumber();
        backend->evaluate(contextIndex, program, fileName, lineNumber);
        response.setAsync(true);
    }   break;

    case QScriptDebuggerCommand::ScriptValueToString: {
        QScriptDebuggerValue value = command.scriptValue();
        QScriptEngine *engine = backend->engine();
        QScriptValue realValue = value.toScriptValue(engine);
        response.setResult(realValue.toString());
    }   break;

    case QScriptDebuggerCommand::SetScriptValueProperty: {
        QScriptDebuggerValue object = command.scriptValue();
        QScriptEngine *engine = backend->engine();
        QScriptValue realObject = object.toScriptValue(engine);
        QScriptDebuggerValue value = command.subordinateScriptValue();
        QScriptValue realValue = value.toScriptValue(engine);
        QString name = command.name();
        realObject.setProperty(name, realValue);
    }   break;

    case QScriptDebuggerCommand::ClearExceptions:
        backend->engine()->clearExceptions();
        break;

    case QScriptDebuggerCommand::UserCommand:
    case QScriptDebuggerCommand::MaxUserCommand:
        break;
    }
    return response;
}

QT_END_NAMESPACE
