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

#include "qscriptdebuggeragent_p.h"
#include "qscriptdebuggeragent_p_p.h"
#include "qscriptdebuggerbackend_p_p.h"

#include <QtCore/qcoreapplication.h>
#include <QtCore/qset.h>
#include <QtScript/qscriptengine.h>

QT_BEGIN_NAMESPACE

/*!
  \since 4.5
  \class QScriptDebuggerAgent
  \internal

  This class implements a state machine that uses the low-level events
  reported by the QScriptEngineAgent interface to implement debugging-
  specific functionality such as stepping and breakpoints. It is used
  internally by the QScriptDebuggerBackend class.
*/

QScriptDebuggerAgentPrivate::QScriptDebuggerAgentPrivate()
{
    state = NoState;
    nextBreakpointId = 1;
    nextContextId = 0;
    statementCounter = 0;
}

QScriptDebuggerAgentPrivate::~QScriptDebuggerAgentPrivate()
{
}

QScriptDebuggerAgentPrivate *QScriptDebuggerAgentPrivate::get(
    QScriptDebuggerAgent *q)
{
    if (!q)
        return 0;
    return q->d_func();
}


/*!
  Constructs a new agent for the given \a engine. The agent will
  report debugging-related events (e.g. step completion) to the given
  \a backend.
*/
QScriptDebuggerAgent::QScriptDebuggerAgent(
    QScriptDebuggerBackendPrivate *backend, QScriptEngine *engine)
#if QT_VERSION >= 0x040500
    : QScriptEngineAgent(*new QScriptDebuggerAgentPrivate, engine)
#else
    : QScriptEngineAgent(engine), d_ptr(new QScriptDebuggerAgentPrivate)
#endif
{
    Q_D(QScriptDebuggerAgent);
#if QT_VERSION < 0x040500
    d_ptr->q_ptr = this;
#endif
    d->backend = backend;

    QScriptContext *ctx = engine->currentContext();
    while (ctx) {
        d->scriptIdStack.append(QList<qint64>());
        d->contextIdStack.append(d->nextContextId);
        ++d->nextContextId;
        ctx = ctx->parentContext();
    }
}

/*!
  Destroys this QScriptDebuggerAgent.
*/
QScriptDebuggerAgent::~QScriptDebuggerAgent()
{
    Q_D(QScriptDebuggerAgent);
    if (d->backend)
        d->backend->agentDestroyed(this);
#if QT_VERSION < 0x040500
    delete d_ptr;
#endif
}

/*!
  Instructs the agent to perform a "step into" operation.  This
  function returns immediately. The agent will report step completion
  at a later time, i.e. when script statements are evaluted.
*/
void QScriptDebuggerAgent::enterStepIntoMode(int count)
{
    Q_D(QScriptDebuggerAgent);
    d->state = QScriptDebuggerAgentPrivate::SteppingIntoState;
    d->stepCount = count;
    d->stepResult = QScriptValue();
}

/*!
  Instructs the agent to perform a "step over" operation.  This
  function returns immediately. The agent will report step completion
  at a later time, i.e. when script statements are evaluted.
*/
void QScriptDebuggerAgent::enterStepOverMode(int count)
{
    Q_D(QScriptDebuggerAgent);
    d->state = QScriptDebuggerAgentPrivate::SteppingOverState;
    if (engine()->isEvaluating())
        d->stepDepth = 0;
    else
        d->stepDepth = -1;
    d->stepCount = count;
    d->stepResult = QScriptValue();
}

/*!
  Instructs the agent to perform a "step out" operation.  This
  function returns immediately. The agent will report step completion
  at a later time, i.e. when script statements are evaluted.
*/
void QScriptDebuggerAgent::enterStepOutMode()
{
    Q_D(QScriptDebuggerAgent);
    d->state = QScriptDebuggerAgentPrivate::SteppingOutState;
    if (engine()->isEvaluating())
        d->stepDepth = 0;
    else
        d->stepDepth = -1;
}

/*!
  Instructs the agent to continue evaluation.
  This function returns immediately.
*/
void QScriptDebuggerAgent::enterContinueMode()
{
    Q_D(QScriptDebuggerAgent);
    d->state = QScriptDebuggerAgentPrivate::NoState;
}

/*!
  Instructs the agent to interrupt evaluation.
  This function returns immediately.
*/
void QScriptDebuggerAgent::enterInterruptMode()
{
    Q_D(QScriptDebuggerAgent);
    d->state = QScriptDebuggerAgentPrivate::InterruptingState;
}

/*!
  Instructs the agent to continue evaluation until the location
  described by \a fileName and \a lineNumber is reached.  This
  function returns immediately.
*/
void QScriptDebuggerAgent::enterRunToLocationMode(const QString &fileName, int lineNumber)
{
    Q_D(QScriptDebuggerAgent);
    d->targetFileName = fileName;
    d->targetLineNumber = lineNumber;
    d->targetScriptId = resolveScript(fileName);
    d->state = QScriptDebuggerAgentPrivate::RunningToLocationState;
}

/*!
  Instructs the agent to continue evaluation until the location
  described by \a scriptId and \a lineNumber is reached.  This
  function returns immediately.
*/
void QScriptDebuggerAgent::enterRunToLocationMode(qint64 scriptId, int lineNumber)
{
    Q_D(QScriptDebuggerAgent);
    d->targetScriptId = scriptId;
    d->targetFileName = QString();
    d->targetLineNumber = lineNumber;
    d->state = QScriptDebuggerAgentPrivate::RunningToLocationState;
}

void QScriptDebuggerAgent::enterReturnByForceMode(int contextIndex, const QScriptValue &value)
{
    Q_D(QScriptDebuggerAgent);
    d->returnCounter = contextIndex + 1;
    d->returnValue = QScriptValue();
    d->state = QScriptDebuggerAgentPrivate::ReturningByForceState;
    // throw an exception; we will catch it when the proper frame is popped
    engine()->currentContext()->throwValue(value);
}

/*!
  Sets a breakpoint defined by the given \a data.
  Returns an integer that uniquely identifies the new breakpoint,
  or -1 if setting the breakpoint failed.
*/
int QScriptDebuggerAgent::setBreakpoint(const QScriptBreakpointData &data)
{
    Q_D(QScriptDebuggerAgent);
    qint64 scriptId = data.scriptId();
    if (scriptId != -1) {
        if (!d->scripts.contains(scriptId)) {
            // that script has been unloaded, so invalidate the ID
            scriptId = -1;
            const_cast<QScriptBreakpointData&>(data).setScriptId(-1);
        } else if (data.fileName().isEmpty()) {
            QString fileName = d->scripts[scriptId].fileName();
            const_cast<QScriptBreakpointData&>(data).setFileName(fileName);
        }
    }

    int id = d->nextBreakpointId;
    ++d->nextBreakpointId;

    if (scriptId != -1) {
        d->resolvedBreakpoints[scriptId].append(id);
    } else {
        QString fileName = data.fileName();
        bool resolved = false;
        QScriptScriptMap::const_iterator it;
        for (it = d->scripts.constBegin(); it != d->scripts.constEnd(); ++it) {
            if (it.value().fileName() == fileName) {
                d->resolvedBreakpoints[it.key()].append(id);
                resolved = true;
                break;
            }
        }
        if (!resolved)
            d->unresolvedBreakpoints[fileName].append(id);
    }

    d->breakpoints.insert(id, data);

    return id;
}

/*!
  Deletes the breakpoint with the given \a id.
  Returns true if the breakpoint was deleted, false if
  no such breakpoint exists.
*/
bool QScriptDebuggerAgent::deleteBreakpoint(int id)
{
    Q_D(QScriptDebuggerAgent);
    if (!d->breakpoints.contains(id))
        return false;
    d->breakpoints.remove(id);
    bool found = false;
    {
        QHash<qint64, QList<int> >::iterator it;
        it = d->resolvedBreakpoints.begin();
        for ( ; !found && (it != d->resolvedBreakpoints.end()); ) {
            QList<int> &lst = it.value();
            Q_ASSERT(!lst.isEmpty());
            for (int i = 0; i < lst.size(); ++i) {
                if (lst.at(i) == id) {
                    lst.removeAt(i);
                    found = true;
                    break;
                }
            }
            if (lst.isEmpty())
                it = d->resolvedBreakpoints.erase(it);
            else
                ++it;
        }
    }
    if (!found) {
        QHash<QString, QList<int> >::iterator it;
        it = d->unresolvedBreakpoints.begin();
        for ( ; !found && (it != d->unresolvedBreakpoints.end()); ) {
            QList<int> &lst = it.value();
            Q_ASSERT(!lst.isEmpty());
            for (int i = 0; i < lst.size(); ++i) {
                if (lst.at(i) == id) {
                    lst.removeAt(i);
                    found = true;
                    break;
                }
            }
            if (lst.isEmpty())
                it = d->unresolvedBreakpoints.erase(it);
            else
                ++it;
        }
    }
    return found;
}

/*!
  Deletes all breakpoints.
*/
void QScriptDebuggerAgent::deleteAllBreakpoints()
{
    Q_D(QScriptDebuggerAgent);
    d->breakpoints.clear();
    d->resolvedBreakpoints.clear();
    d->unresolvedBreakpoints.clear();
}

/*!
  Returns the data associated with the breakpoint with the given \a
  id.
*/
QScriptBreakpointData QScriptDebuggerAgent::breakpointData(int id) const
{
    Q_D(const QScriptDebuggerAgent);
    return d->breakpoints.value(id);
}

/*!
  Sets the data associated with the breakpoint with the given \a
  id.
*/
bool QScriptDebuggerAgent::setBreakpointData(int id,
                                             const QScriptBreakpointData &data)
{
    Q_D(QScriptDebuggerAgent);
    if (!d->breakpoints.contains(id))
        return false;
    d->breakpoints[id] = data;
    return true;
}

/*!
  Returns all breakpoints.
*/
QScriptBreakpointMap QScriptDebuggerAgent::breakpoints() const
{
    Q_D(const QScriptDebuggerAgent);
    return d->breakpoints;
}

/*!
  Returns all scripts.
*/
QScriptScriptMap QScriptDebuggerAgent::scripts() const
{
    Q_D(const QScriptDebuggerAgent);
    return d->scripts;
}

/*!
  Returns the data associated with the script with the given \a id.
*/
QScriptScriptData QScriptDebuggerAgent::scriptData(qint64 id) const
{
    Q_D(const QScriptDebuggerAgent);
    return d->scripts.value(id);
}

/*!
  Checkpoints the current scripts.
*/
void QScriptDebuggerAgent::scriptsCheckpoint()
{
    Q_D(QScriptDebuggerAgent);
    d->previousCheckpointScripts = d->checkpointScripts;
    d->checkpointScripts = d->scripts;
}

/*!
  Returns the difference between the current checkpoint and the
  previous checkpoint. The first item in the pair is a list containing
  the identifiers of the scripts that were added. The second item in
  the pair is a list containing the identifiers of the scripts that
  were removed.
*/
QPair<QList<qint64>, QList<qint64> > QScriptDebuggerAgent::scriptsDelta() const
{
    Q_D(const QScriptDebuggerAgent);
    QSet<qint64> prevSet = d->previousCheckpointScripts.keys().toSet();
    QSet<qint64> currSet = d->checkpointScripts.keys().toSet();
    QSet<qint64> addedScriptIds = currSet - prevSet;
    QSet<qint64> removedScriptIds = prevSet - currSet;
    return qMakePair(addedScriptIds.toList(), removedScriptIds.toList());
}

/*!
  Returns the identifier of the script that has the given \a fileName,
  or -1 if there is no such script.
*/
qint64 QScriptDebuggerAgent::resolveScript(const QString &fileName) const
{
    Q_D(const QScriptDebuggerAgent);
    QScriptScriptMap::const_iterator it;
    for (it = d->scripts.constBegin(); it != d->scripts.constEnd(); ++it) {
        if (it.value().fileName() == fileName)
            return it.key();
    }
    return -1;
}

QList<qint64> QScriptDebuggerAgent::contextIds() const
{
    Q_D(const QScriptDebuggerAgent);
    return d->contextIdStack;
}

QPair<QList<qint64>, QList<qint64> > QScriptDebuggerAgent::contextsCheckpoint()
{
    Q_D(QScriptDebuggerAgent);
    int i = d->checkpointContextIdStack.size() - 1;
    int j = d->contextIdStack.size() - 1;
    for ( ; (i >= 0) && (j >= 0); --i, --j) {
        if (d->checkpointContextIdStack.at(i) != d->contextIdStack.at(j))
            break;
    }
    QList<qint64> removed = d->checkpointContextIdStack.mid(0, i+1);
    QList<qint64> added = d->contextIdStack.mid(0, j+1);
    d->checkpointContextIdStack = d->contextIdStack;
    return qMakePair(removed, added);
}

void QScriptDebuggerAgent::nullifyBackendPointer()
{
    Q_D(QScriptDebuggerAgent);
    d->backend = 0;
}

/*!
  \reimp
*/
void QScriptDebuggerAgent::scriptLoad(qint64 id, const QString &program,
                                      const QString &fileName, int baseLineNumber)
{
    Q_D(QScriptDebuggerAgent);
    QScriptScriptData data = QScriptScriptData(program, fileName, baseLineNumber);
    d->scripts.insert(id, data);

    if ((d->state == QScriptDebuggerAgentPrivate::RunningToLocationState)
        && (d->targetScriptId == -1)
        && ((d->targetFileName == fileName) || d->targetFileName.isEmpty())) {
        d->targetScriptId = id;
    }

    if (!fileName.isEmpty()) {
        QList<int> lst = d->unresolvedBreakpoints.take(fileName);
        if (!lst.isEmpty())
            d->resolvedBreakpoints.insert(id, lst);
    }
}

/*!
  \reimp
*/
void QScriptDebuggerAgent::scriptUnload(qint64 id)
{
    Q_D(QScriptDebuggerAgent);
    QScriptScriptData data = d->scripts.take(id);
    QString fileName = data.fileName();

    if ((d->state == QScriptDebuggerAgentPrivate::RunningToLocationState)
        && (d->targetScriptId == id)) {
        d->targetScriptId = -1;
        d->targetFileName = fileName;
    }

    if (!fileName.isEmpty()) {
        QList<int> lst = d->resolvedBreakpoints.take(id);
        if (!lst.isEmpty())
            d->unresolvedBreakpoints.insert(fileName, lst);
    }
}

/*!
  \reimp
*/
void QScriptDebuggerAgent::contextPush()
{
    Q_D(QScriptDebuggerAgent);
    d->scriptIdStack.append(QList<qint64>());
    d->contextIdStack.prepend(d->nextContextId);
    ++d->nextContextId;
}

/*!
  \reimp
*/
void QScriptDebuggerAgent::contextPop()
{
    Q_D(QScriptDebuggerAgent);
    d->scriptIdStack.removeLast();
    d->contextIdStack.removeFirst();
}

/*!
  \reimp
*/
void QScriptDebuggerAgent::functionEntry(qint64 scriptId)
{
    Q_D(QScriptDebuggerAgent);
    QList<qint64> &ids = d->scriptIdStack.last();
    ids.append(scriptId);
    if ((d->state == QScriptDebuggerAgentPrivate::SteppingOverState)
        || (d->state == QScriptDebuggerAgentPrivate::SteppingOutState)) {
        ++d->stepDepth;
    }
}

/*!
  \reimp
*/
void QScriptDebuggerAgent::functionExit(qint64 scriptId,
                                        const QScriptValue &returnValue)
{
    Q_UNUSED(scriptId);
    Q_D(QScriptDebuggerAgent);
    QList<qint64> &ids = d->scriptIdStack.last();
    ids.removeLast();
    if (d->state == QScriptDebuggerAgentPrivate::SteppingOverState) {
        --d->stepDepth;
    } else if (d->state == QScriptDebuggerAgentPrivate::SteppingOutState) {
        if (--d->stepDepth < 0) {
            d->stepResult = returnValue;
            d->state = QScriptDebuggerAgentPrivate::SteppedOutState;
        }
    } else if (d->state == QScriptDebuggerAgentPrivate::ReturningByForceState) {
        if (--d->returnCounter == 0) {
            d->returnValue = returnValue;
            d->state = QScriptDebuggerAgentPrivate::ReturnedByForceState;
            engine()->clearExceptions();
        }
    }
}

/*!
  \reimp
*/
void QScriptDebuggerAgent::positionChange(qint64 scriptId,
                                          int lineNumber, int columnNumber)
{
    Q_D(QScriptDebuggerAgent);
    if (engine()->processEventsInterval() == -1) {
        // see if it's time to call processEvents()
        if ((++d->statementCounter % 25000) == 0) {
            if (!d->processEventsTimer.isNull()) {
                if (d->processEventsTimer.elapsed() > 30) {
                    QCoreApplication::processEvents();
                    d->processEventsTimer.restart();
                }
            } else {
                d->processEventsTimer.start();
            }
        }
    }

    // check breakpoints
    {
        QList<int> lst = d->resolvedBreakpoints.value(scriptId);
        for (int i = 0; i < lst.size(); ++i) {
            int id = lst.at(i);
            QScriptBreakpointData &data = d->breakpoints[id];
            if (!data.isEnabled())
                continue;
            if (data.lineNumber() != lineNumber)
                continue;
            if (!data.condition().isEmpty()) {
                // ### careful, evaluate() can cause an exception
                // ### disable callbacks in nested evaluate?
                QScriptDebuggerAgentPrivate::State was = d->state;
                d->state = QScriptDebuggerAgentPrivate::NoState;
                QScriptValue ret = engine()->evaluate(
                    data.condition(),
                    QString::fromLatin1("Breakpoint %0 condition checker").arg(id));
                if (!ret.isError())
                    d->state = was;
                if (!ret.toBoolean())
                    continue;
            }
            if (!data.hit())
                continue;
            d->hitBreakpointId = id;
            d->state = QScriptDebuggerAgentPrivate::BreakpointState;
        }
    }

    switch (d->state) {
    case QScriptDebuggerAgentPrivate::NoState:
    case QScriptDebuggerAgentPrivate::SteppingOutState:
    case QScriptDebuggerAgentPrivate::ReturningByForceState:
        // Do nothing
        break;

    case QScriptDebuggerAgentPrivate::SteppingIntoState:
        if (--d->stepCount == 0) {
            d->state = QScriptDebuggerAgentPrivate::NoState;
            if (d->backend)
                d->backend->stepped(scriptId, lineNumber, columnNumber, QScriptValue());
        }
        break;

    case QScriptDebuggerAgentPrivate::SteppingOverState:
        if ((d->stepDepth > 0) || (--d->stepCount != 0))
            break;
        // fallthrough
    case QScriptDebuggerAgentPrivate::SteppedOverState:
        d->state = QScriptDebuggerAgentPrivate::NoState;
        if (d->backend)
            d->backend->stepped(scriptId, lineNumber, columnNumber, d->stepResult);
        break;

    case QScriptDebuggerAgentPrivate::SteppedOutState:
        d->state = QScriptDebuggerAgentPrivate::NoState;
        if (d->backend)
            d->backend->stepped(scriptId, lineNumber, columnNumber, d->stepResult);
        break;

    case QScriptDebuggerAgentPrivate::RunningToLocationState:
        if (((lineNumber == d->targetLineNumber) || (d->targetLineNumber == -1))
            && (scriptId == d->targetScriptId)) {
            d->state = QScriptDebuggerAgentPrivate::NoState;
            if (d->backend)
                d->backend->locationReached(scriptId, lineNumber, columnNumber);
        }
        break;

    case QScriptDebuggerAgentPrivate::InterruptingState:
        d->state = QScriptDebuggerAgentPrivate::NoState;
        if (d->backend)
            d->backend->interrupted(scriptId, lineNumber, columnNumber);
        break;

    case QScriptDebuggerAgentPrivate::BreakpointState:
        d->state = QScriptDebuggerAgentPrivate::NoState;
        if (d->backend)
            d->backend->breakpoint(scriptId, lineNumber, columnNumber, d->hitBreakpointId);
        if (d->breakpoints.value(d->hitBreakpointId).isSingleShot())
            deleteBreakpoint(d->hitBreakpointId);
        break;

    case QScriptDebuggerAgentPrivate::ReturnedByForceState:
        d->state = QScriptDebuggerAgentPrivate::NoState;
        if (d->backend)
            d->backend->forcedReturn(scriptId, lineNumber, columnNumber, d->returnValue);
        break;

    case QScriptDebuggerAgentPrivate::SteppedIntoState:
    case QScriptDebuggerAgentPrivate::ReachedLocationState:
    case QScriptDebuggerAgentPrivate::InterruptedState:
// ### deal with the case when code is evaluated while we're already paused
//        Q_ASSERT(false);
        break;
    }
}

/*!
  \reimp
*/
void QScriptDebuggerAgent::exceptionThrow(qint64 scriptId,
                                          const QScriptValue &exception,
                                          bool hasHandler)
{
    Q_D(QScriptDebuggerAgent);
    if (d->state == QScriptDebuggerAgentPrivate::ReturningByForceState) {
        // we threw this exception ourselves, so ignore it for now
        // (see functionExit()).
        return;
    }
    if (d->backend)
        d->backend->exception(scriptId, exception, hasHandler);
}

/*!
  \reimp
*/
void QScriptDebuggerAgent::exceptionCatch(qint64 scriptId,
                                          const QScriptValue &exception)
{
    Q_UNUSED(scriptId);
    Q_UNUSED(exception);
}

/*!
  \reimp
*/
bool QScriptDebuggerAgent::supportsExtension(Extension extension) const
{
#if QT_VERSION >= 0x040500
    return (extension == DebuggerInvocationRequest);
#else
    Q_UNUSED(extension);
    return false;
#endif
}

/*!
  \reimp
*/
QVariant QScriptDebuggerAgent::extension(Extension extension,
                                         const QVariant &argument)
{
    Q_UNUSED(extension);
#if QT_VERSION >= 0x040500
    Q_D(QScriptDebuggerAgent);
    Q_ASSERT(extension == DebuggerInvocationRequest);
    QVariantList lst = argument.toList();
    qint64 scriptId = lst.at(0).toLongLong();
    int lineNumber = lst.at(1).toInt();
    int columnNumber = lst.at(2).toInt();
    d->state = QScriptDebuggerAgentPrivate::NoState;
    if (d->backend) {
        d->backend->debuggerInvocationRequest(
            scriptId, lineNumber, columnNumber);
    }
#else
    Q_UNUSED(argument);
#endif
    return QVariant();
}

QT_END_NAMESPACE
