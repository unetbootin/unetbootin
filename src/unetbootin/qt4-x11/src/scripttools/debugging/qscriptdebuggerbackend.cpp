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

#include "qscriptdebuggerbackend_p.h"
#include "qscriptdebuggerbackend_p_p.h"
#include "qscriptdebuggeragent_p.h"
#include "qscriptdebuggercommandexecutor_p.h"
#include "qscriptdebuggerevent_p.h"
#include "qscriptdebuggervalue_p.h"
#include "qscriptscriptdata_p.h"
#include "qscriptbreakpointdata_p.h"
#include "qscriptobjectsnapshot_p.h"

#include <QtScript/qscriptengine.h>
#include <QtScript/qscriptcontextinfo.h>
#include <QtScript/qscriptvalueiterator.h>

#include <QtCore/qcoreapplication.h>
#include <QtCore/qdebug.h>

Q_DECLARE_METATYPE(QScriptDebuggerValue)
Q_DECLARE_METATYPE(QScriptDebuggerBackendPrivate*)

QT_BEGIN_NAMESPACE

/*!
  \since 4.5
  \class QScriptDebuggerBackend
  \internal

  \brief The QScriptDebuggerBackend class is the base class of debugger back-ends.

  QScriptDebuggerBackend builds on the QScriptDebuggerAgent class.

  This class is usually used together with the QScriptDebuggerFrontend
  class, in order to form a (front-end, back-end) pair.

  Call attachTo() to attach to a QScriptEngine object. Call detach()
  to detach from the current engine.

  Call stepInto() to step into the next script statement; call stepOver()
  to step over the next script statement; and call stepOut() to step out
  of the currently executing script function. An event() will be generated
  when the stepping is completed.

  Call runToLocation() to execute script statements until a certain
  location has been reached. An event() will be generated when the location
  has been reached.

  Call interruptEvaluation() to request that evaluation should be
  interrupted. An event() will be generated upon the next script
  statement that is reached.

  Call continueEvalution() to allow script evaluation to continue.

  Call setBreakpoint() to set a breakpoint. A breakpoint event() will
  be generated when a breakpoint is hit. Call deleteBreakpoint() to
  delete a breakpoint. Call modifyBreakpoint() to change the state of
  an existing breakpoint.

  Call contextCount() to obtain the number of active contexts
  (frames). Call context() to obtain a pointer to a QScriptContext.

  \section1 Subclassing

  When subclassing QScriptDebuggerBackend, you must implement the pure
  virtual event() function. This function typically forwards the event
  to a QScriptDebuggerFrontend object. For most type of events,
  event() should block until the back-end is instructed to resume
  execution (e.g. until continueEvalution() is called). You must
  implement resume(), which is responsible for making event() return.

  \sa QScriptDebuggerFrontend, QScriptDebuggerEvent
*/

// helper class that's used to handle our custom Qt events
class QScriptDebuggerBackendEventReceiver : public QObject
{
public:
    QScriptDebuggerBackendEventReceiver(QScriptDebuggerBackendPrivate *backend,
                                        QObject *parent = 0)
        : QObject(parent), m_backend(backend) {}
    ~QScriptDebuggerBackendEventReceiver() {}

    bool event(QEvent *e)
    {
        return m_backend->event(e);
    }

private:
    QScriptDebuggerBackendPrivate *m_backend;
};


QScriptDebuggerBackendPrivate::QScriptDebuggerBackendPrivate()
{
    eventReceiver = 0;
    agent = 0;
    commandExecutor = 0;
    pendingEvaluateLineNumber = -1;
    ignoreExceptions = false;
    nextScriptValueIteratorId = 0;
    nextScriptObjectSnapshotId = 0;
}

QScriptDebuggerBackendPrivate::~QScriptDebuggerBackendPrivate()
{
    if (agent)
        agent->nullifyBackendPointer();
    delete commandExecutor;
    delete eventReceiver;
    qDeleteAll(scriptValueIterators);
    qDeleteAll(scriptObjectSnapshots);
}

void QScriptDebuggerBackendPrivate::postEvent(QEvent *e)
{
    if (!eventReceiver) {
        eventReceiver = new QScriptDebuggerBackendEventReceiver(this);
        eventReceiver->moveToThread(agent->engine()->thread());
    }
    QCoreApplication::postEvent(eventReceiver, e);
}

bool QScriptDebuggerBackendPrivate::event(QEvent *e)
{
    if (e->type() == QEvent::User+1) {
        QScriptDebuggerEventEvent *de = static_cast<QScriptDebuggerEventEvent*>(e);
        q_func()->event(de->event());
        return true;
    }
    return false;
}

void QScriptDebuggerBackendPrivate::agentDestroyed(QScriptDebuggerAgent *ag)
{
    // Since agents are owned by the script engine, this in practice means
    // that the engine has been destroyed. Invalidate our pointer so we
    // don't crash later.
    if (agent == ag)
        agent = 0;
}

/*!
  The agent calls this function when it has completed a step
  operation.
*/
void QScriptDebuggerBackendPrivate::stepped(qint64 scriptId,
                                            int lineNumber,
                                            int columnNumber,
                                            const QScriptValue &result)
{
    Q_Q(QScriptDebuggerBackend);
    QScriptDebuggerEvent e(QScriptDebuggerEvent::SteppingFinished,
                           scriptId, lineNumber, columnNumber);
    e.setFileName(agent->scriptData(scriptId).fileName());
    QScriptDebuggerValue value(result);
    e.setScriptValue(value);
    if (!result.isUndefined())
        e.setMessage(result.toString()); // for convenience -- we always need it
    q->event(e);
}

/*!
  The agent calls this function when it has run to a particular
  location.
*/
void QScriptDebuggerBackendPrivate::locationReached(qint64 scriptId,
                                                    int lineNumber,
                                                    int columnNumber)
{
    Q_Q(QScriptDebuggerBackend);
    QScriptDebuggerEvent e(QScriptDebuggerEvent::LocationReached,
                           scriptId, lineNumber, columnNumber);
    e.setFileName(agent->scriptData(scriptId).fileName());
    q->event(e);
}

/*!
  The agent calls this function when evaluation has been interrupted.
*/
void QScriptDebuggerBackendPrivate::interrupted(qint64 scriptId,
                                                int lineNumber,
                                                int columnNumber)
{
    Q_Q(QScriptDebuggerBackend);
    QScriptDebuggerEvent e(QScriptDebuggerEvent::Interrupted,
                           scriptId, lineNumber, columnNumber);
    e.setFileName(agent->scriptData(scriptId).fileName());
    q->event(e);
}

/*!
  The agent calls this function when a breakpoint has been triggered.
*/
void QScriptDebuggerBackendPrivate::breakpoint(qint64 scriptId,
                                               int lineNumber,
                                               int columnNumber,
                                               int breakpointId)
{
    Q_Q(QScriptDebuggerBackend);
    QScriptDebuggerEvent e(QScriptDebuggerEvent::Breakpoint,
                           scriptId, lineNumber, columnNumber);
    e.setFileName(agent->scriptData(scriptId).fileName());
    e.setBreakpointId(breakpointId);
    q->event(e);
}

/*!
  The agent calls this function when an uncaught exception has
  occurred.
*/
void QScriptDebuggerBackendPrivate::exception(qint64 scriptId,
                                              const QScriptValue &exception,
                                              bool hasHandler)
{
    Q_Q(QScriptDebuggerBackend);
    if (ignoreExceptions) {
        // don't care (it's caught by us)
        return;
    }
    QScriptDebuggerEvent e(QScriptDebuggerEvent::Exception);
    e.setScriptId(scriptId);
    e.setFileName(agent->scriptData(scriptId).fileName());
    e.setMessage(exception.toString());
    e.setHasExceptionHandler(hasHandler);
    int lineNumber = -1;
    QString fileName;
    if (exception.property(QLatin1String("lineNumber")).isNumber())
        lineNumber = exception.property(QLatin1String("lineNumber")).toInt32();
    if (exception.property(QLatin1String("fileName")).isString())
        fileName = exception.property(QLatin1String("fileName")).toString();
    if (lineNumber == -1) {
        QScriptContextInfo info(q->engine()->currentContext());
        lineNumber = info.lineNumber();
        fileName = info.fileName();
    }
    if (lineNumber != -1)
        e.setLineNumber(lineNumber);
    if (!fileName.isEmpty())
        e.setFileName(fileName);
    QScriptDebuggerValue value(exception);
    e.setScriptValue(value);
    q->event(e);
}

QScriptValue QScriptDebuggerBackendPrivate::trace(QScriptContext *context,
                                                  QScriptEngine *engine)
{
    QScriptValue data = context->callee().data();
    QScriptDebuggerBackendPrivate *self = qscriptvalue_cast<QScriptDebuggerBackendPrivate*>(data);
    if (!self)
        return engine->undefinedValue();
    QString str;
    for (int i = 0; i < context->argumentCount(); ++i) {
        if (i > 0)
            str.append(QLatin1String(" "));
        str.append(context->argument(i).toString());
    }
    QScriptDebuggerEvent e(QScriptDebuggerEvent::Trace);
    e.setMessage(str);
    self->q_func()->event(e);
    return engine->undefinedValue();
}

QScriptValue QScriptDebuggerBackendPrivate::qsassert(QScriptContext *context,
                                                     QScriptEngine *engine)
{
    QScriptValue arg = context->argument(0);
    if (arg.toBoolean())
        return arg;
    QScriptContextInfo info(context->parentContext());
    QString msg;
    QString fileName = info.fileName();
    if (fileName.isEmpty())
        fileName = QString::fromLatin1("<anonymous script, id=%0>").arg(info.scriptId());
    msg.append(fileName);
    msg.append(QLatin1Char(':'));
    msg.append(QString::number(info.lineNumber()));
    msg.append(QString::fromLatin1(": Assertion failed"));
    for (int i = 1; i < context->argumentCount(); ++i) {
        if (i == 1)
            msg.append(QLatin1Char(':'));
        msg.append(QLatin1Char(' '));
        msg.append(context->argument(i).toString());
    }
    QScriptValue err = context->throwError(msg);
    err.setProperty(QString::fromLatin1("name"), QScriptValue(engine, QString::fromLatin1("AssertionError")));
    return err;
}

QScriptValue QScriptDebuggerBackendPrivate::fileName(QScriptContext *context,
                                                     QScriptEngine *engine)
{
    QScriptContextInfo info(context->parentContext());
    QString fn = info.fileName();
    if (fn.isEmpty())
        return engine->undefinedValue();
    return QScriptValue(engine, fn);
}

QScriptValue QScriptDebuggerBackendPrivate::lineNumber(QScriptContext *context,
                                                       QScriptEngine *engine)
{
    QScriptContextInfo info(context->parentContext());
    return QScriptValue(engine, info.lineNumber());
}

/*!
  The agent calls this function when the engine has reached a
  "debugger" statement.
*/
void QScriptDebuggerBackendPrivate::debuggerInvocationRequest(
    qint64 scriptId, int lineNumber, int columnNumber)
{
    Q_Q(QScriptDebuggerBackend);
    QScriptDebuggerEvent e(QScriptDebuggerEvent::DebuggerInvocationRequest,
                           scriptId, lineNumber, columnNumber);
    e.setFileName(agent->scriptData(scriptId).fileName());
    q->event(e);
}

void QScriptDebuggerBackendPrivate::forcedReturn(
    qint64 scriptId, int lineNumber, int columnNumber,
    const QScriptValue &value)
{
    Q_Q(QScriptDebuggerBackend);
    QScriptDebuggerEvent e(QScriptDebuggerEvent::ForcedReturn,
                           scriptId, lineNumber, columnNumber);
    e.setFileName(agent->scriptData(scriptId).fileName());
    e.setScriptValue(QScriptDebuggerValue(value));
    q->event(e);
}

/*!
  Creates a QScriptDebuggerBackend object.
*/
QScriptDebuggerBackend::QScriptDebuggerBackend()
    : d_ptr(new QScriptDebuggerBackendPrivate)
{
    d_ptr->q_ptr = this;
}

/*!
  Destroys this QScriptDebuggerBackend.
*/
QScriptDebuggerBackend::~QScriptDebuggerBackend()
{
    detach();
    delete d_ptr;
}

/*!
  \internal
*/
QScriptDebuggerBackend::QScriptDebuggerBackend(QScriptDebuggerBackendPrivate &dd)
    : d_ptr(&dd)
{
    d_ptr->q_ptr = this;
}

/*!
  Attaches this backend to the given \a engine.
  The backend automatically detaches from the old engine, if any.

  This function installs its own agent on the \a engine using
  QScriptEngine::setAgent(); any existing agent will be replaced.

  \sa detach(). engine()
*/
void QScriptDebuggerBackend::attachTo(QScriptEngine *engine)
{
    Q_D(QScriptDebuggerBackend);
    detach();
    d->agent = new QScriptDebuggerAgent(d, engine);
    QScriptValue global = engine->globalObject();
    global.setProperty(QString::fromLatin1("print"), traceFunction());
//    global.setProperty(QString::fromLatin1("qAssert"), assertFunction());
    global.setProperty(QString::fromLatin1("__FILE__"), fileNameFunction(),
                       QScriptValue::PropertyGetter | QScriptValue::PropertySetter
                       | QScriptValue::ReadOnly);
    global.setProperty(QString::fromLatin1("__LINE__"), lineNumberFunction(),
                       QScriptValue::PropertyGetter | QScriptValue::PropertySetter
                       | QScriptValue::ReadOnly);
    engine->setAgent(d->agent);
}

/*!
  Detaches this backend from the current script engine.
  The backend's state (including breakpoints and information on loaded
  scripts) will be invalidated.

  \sa attach()
*/
void QScriptDebuggerBackend::detach()
{
    Q_D(QScriptDebuggerBackend);
    if (!d->agent)
        return;
    QScriptEngine *eng = d->agent->engine();
    if (eng && eng->agent() == d->agent) {
        eng->setAgent(0);
        QScriptValue global = eng->globalObject();
        if (global.property(QString::fromLatin1("print")).strictlyEquals(traceFunction()))
            global.setProperty(QString::fromLatin1("print"), QScriptValue());
//        global.setProperty(QString::fromLatin1("qAssert"), QScriptValue());
        if (global.property(QString::fromLatin1("__FILE__")).strictlyEquals(fileNameFunction()))
            global.setProperty(QString::fromLatin1("__FILE__"), QScriptValue());
        if (global.property(QString::fromLatin1("__LINE__")).strictlyEquals(lineNumberFunction()))
            global.setProperty(QString::fromLatin1("__LINE__"), QScriptValue());
        d->agent->nullifyBackendPointer();
        d->agent = 0; // agent is owned by engine
    }

    d->pendingEvaluateLineNumber = -1;
    d->ignoreExceptions = false;
    d->nextScriptValueIteratorId = 0;
    qDeleteAll(d->scriptValueIterators);
    d->scriptValueIterators.clear();
    qDeleteAll(d->scriptObjectSnapshots);
    d->scriptObjectSnapshots.clear();
}

/*!
  Returns the script engine that this backend is attached to, or 0 if
  the backend is not attached to an engine.

  \sa attachTo()
*/
QScriptEngine *QScriptDebuggerBackend::engine() const
{
    Q_D(const QScriptDebuggerBackend);
    if (!d->agent)
        return 0;
    return d->agent->engine();
}

/*!
  Steps into the next script statement.
  When stepping is complete, an event() will be generated.
*/
void QScriptDebuggerBackend::stepInto(int count)
{
    Q_D(QScriptDebuggerBackend);
    if (d->agent) {
        d->agent->enterStepIntoMode(count);
        resume();
    }
}

/*!
  Steps over the next script statement.
  When stepping is complete, an event() will be generated.
*/
void QScriptDebuggerBackend::stepOver(int count)
{
    Q_D(QScriptDebuggerBackend);
    if (d->agent) {
        d->agent->enterStepOverMode(count);
        resume();
    }
}

/*!
  Steps out of the current script function.
  When stepping is complete, an event() will be generated.
*/
void QScriptDebuggerBackend::stepOut()
{
    Q_D(QScriptDebuggerBackend);
    if (d->agent) {
        d->agent->enterStepOutMode();
        resume();
    }
}

/*!
  Continues script evaluation. Evaluation will proceed without
  interruption until either 1) an uncaught exception occurs, 2) a
  breakpoint is triggered, or 3) interruptEvaluation() is called.
  In each case, a proper event() will be generated.
*/
void QScriptDebuggerBackend::continueEvalution()
{
    Q_D(QScriptDebuggerBackend);
    if (d->agent) {
        d->agent->enterContinueMode();
        resume();
    }
}

/*!
  Interrupts script evaluation. When the next script statement is
  reached, an event() will be generated.
*/
void QScriptDebuggerBackend::interruptEvaluation()
{
    Q_D(QScriptDebuggerBackend);
    if (d->agent)
        d->agent->enterInterruptMode();
}

/*!
  Continues evaluation until the location defined by the given \a
  fileName and \a lineNumber is reached. When the location is reached,
  an event() will be generated.
*/
void QScriptDebuggerBackend::runToLocation(const QString &fileName, int lineNumber)
{
    Q_D(QScriptDebuggerBackend);
    if (d->agent) {
        d->agent->enterRunToLocationMode(fileName, lineNumber);
        resume();
    }
}

/*!
  Continues evaluation until the location defined by the given \a
  scriptId and \a lineNumber is reached. When the location is reached,
  an event() will be generated.
*/
void QScriptDebuggerBackend::runToLocation(qint64 scriptId, int lineNumber)
{
    Q_D(QScriptDebuggerBackend);
    if (d->agent) {
        d->agent->enterRunToLocationMode(scriptId, lineNumber);
        resume();
    }
}

void QScriptDebuggerBackend::returnToCaller(int contextIndex, const QScriptValue &value)
{
    Q_D(QScriptDebuggerBackend);
    if (d->agent) {
        d->agent->enterReturnByForceMode(contextIndex, value);
        resume();
    }
}

/*!
  Evaluates the given \a program. When evaluation is complete, an
  event() is generated.
*/
void QScriptDebuggerBackend::evaluate(int contextIndex, const QString &program,
                                      const QString &fileName, int lineNumber)
{
    Q_D(QScriptDebuggerBackend);
    d->pendingEvaluateContextIndex = contextIndex;
    d->pendingEvaluateProgram = program;
    d->pendingEvaluateFileName = fileName;
    d->pendingEvaluateLineNumber = lineNumber;
    if (!engine()->isEvaluating())
        doPendingEvaluate(/*postEvent=*/true);
    else
        resume();
}

/*!
  Executes the pending evaluate, if any.
*/
void QScriptDebuggerBackend::doPendingEvaluate(bool postEvent)
{
    Q_D(QScriptDebuggerBackend);
    QString program = d->pendingEvaluateProgram;
    if (program.isEmpty())
        return;
    int contextIndex = d->pendingEvaluateContextIndex;
    QScriptContext *ctx = context(contextIndex);
    Q_ASSERT(ctx != 0);
    QString fileName = d->pendingEvaluateFileName;
    int lineNumber = d->pendingEvaluateLineNumber;
    d->pendingEvaluateProgram = QString();
    d->pendingEvaluateFileName = QString();
    d->pendingEvaluateLineNumber = -1;
    d->pendingEvaluateContextIndex = -1;

    // push a new context and initialize its scope chain etc.
    {
        QScriptContext *evalContext = engine()->pushContext();
#if QT_VERSION >= 0x040500
        QScriptValueList scopeChain = ctx->scopeChain();
        if (scopeChain.isEmpty())
            scopeChain.append(engine()->globalObject());
        while (!scopeChain.isEmpty())
            evalContext->pushScope(scopeChain.takeLast());
#endif
        evalContext->setActivationObject(ctx->activationObject());
        evalContext->setThisObject(ctx->thisObject());
    }

    d->agent->enterContinueMode();
    // set a flag so that any exception that happens in
    // the evaluate() is not sent to the debugger
    d->ignoreExceptions = true;
    bool hadException = engine()->hasUncaughtException();
    QScriptValue ret = engine()->evaluate(program, fileName, lineNumber);
    d->ignoreExceptions = false;
    if (!hadException && engine()->hasUncaughtException())
        engine()->clearExceptions();
    engine()->popContext();

    QScriptDebuggerValue retret(ret);
    QScriptDebuggerEvent e(QScriptDebuggerEvent::InlineEvalFinished);
    e.setScriptValue(retret);
    if (!ret.isUndefined())
        e.setMessage(ret.toString()); // for convenience -- we always need it

    e.setNestedEvaluate(engine()->isEvaluating());

    if (postEvent) {
        QScriptDebuggerEventEvent *de = new QScriptDebuggerEventEvent(e);
        d->postEvent(de);
    } else {
        event(e);
    }
}

/*!
  Sets a breakpoint defined by the given \a data, and returns a unique
  identifier for the new breakpoint.

  If the conditions of the breakpoint is satisfied at some point
  during script evaluation, a breakpoint event() will be generated.

  \sa deleteBreakpoint(), breakpoints()
*/
int QScriptDebuggerBackend::setBreakpoint(const QScriptBreakpointData &data)
{
    Q_D(QScriptDebuggerBackend);
    if (!d->agent)
        return -1;
    if (!data.isValid())
        return -1;
    return d->agent->setBreakpoint(data);
}

/*!
  Deletes the breakpoint identified by the given \a id.  Returns true
  if the breakpoint was deleted (i.e. the \a id was valid), otherwise
  returns false.

  \sa setBreakpoint()
*/
bool QScriptDebuggerBackend::deleteBreakpoint(int id)
{
    Q_D(QScriptDebuggerBackend);
    if (!d->agent)
        return false;
    return d->agent->deleteBreakpoint(id);
}

/*!
  Deletes all breakpoints.
*/
void QScriptDebuggerBackend::deleteAllBreakpoints()
{
    Q_D(QScriptDebuggerBackend);
    if (d->agent)
        d->agent->deleteAllBreakpoints();
}

/*!
  Returns the data associated with the breakpoint identified by the
  given \a id.
*/
QScriptBreakpointData QScriptDebuggerBackend::breakpointData(int id) const
{
    Q_D(const QScriptDebuggerBackend);
    if (!d->agent)
        return QScriptBreakpointData();
    return d->agent->breakpointData(id);
}

/*!
  Sets the \a data associated with the breakpoint identified by the
  given \a id.
*/
bool QScriptDebuggerBackend::setBreakpointData(int id, const QScriptBreakpointData &data)
{
    Q_D(QScriptDebuggerBackend);
    if (d->agent)
        return d->agent->setBreakpointData(id, data);
    return false;
}

/*!
  Returns this backend's breakpoints.

  \sa setBreakpoint()
*/
QScriptBreakpointMap QScriptDebuggerBackend::breakpoints() const
{
    Q_D(const QScriptDebuggerBackend);
    if (!d->agent)
        return QScriptBreakpointMap();
    return d->agent->breakpoints();
}

/*!
  Returns the scripts that this backend knows about.

  \sa scriptData()
*/
QScriptScriptMap QScriptDebuggerBackend::scripts() const
{
    Q_D(const QScriptDebuggerBackend);
    if (!d->agent)
        return QScriptScriptMap();
    return d->agent->scripts();
}

/*!
  Returns the data for the script identified by the given \a id.

  \sa scripts()
*/
QScriptScriptData QScriptDebuggerBackend::scriptData(qint64 id) const
{
    Q_D(const QScriptDebuggerBackend);
    if (!d->agent)
        return QScriptScriptData();
    return d->agent->scriptData(id);
}

/*!
  Makes a checkpoint of the currently loaded scripts.

  \sa scriptsDelta()
*/
void QScriptDebuggerBackend::scriptsCheckpoint()
{
    Q_D(QScriptDebuggerBackend);
    if (d->agent)
        d->agent->scriptsCheckpoint();
}

/*!
  Returns the difference between the latest scripts checkpoint and the
  previous checkpoint.  The first item in the pair is a list
  containing the identifiers of the scripts that were added. The
  second item in the pair is a list containing the identifiers of the
  scripts that were removed.

  \sa scriptsCheckpoint()
*/
QScriptScriptsDelta QScriptDebuggerBackend::scriptsDelta() const
{
    Q_D(const QScriptDebuggerBackend);
    if (!d->agent)
        return QPair<QList<qint64>, QList<qint64> >();
    return d->agent->scriptsDelta();
}

qint64 QScriptDebuggerBackend::resolveScript(const QString &fileName) const
{
    Q_D(const QScriptDebuggerBackend);
    if (!d->agent)
        return -1;
    return d->agent->resolveScript(fileName);
}

/*!
  Returns the number of contexts (frames).
*/
int QScriptDebuggerBackend::contextCount() const
{
    if (!engine())
        return 0;
    int count = 0;
    QScriptContext *ctx = engine()->currentContext();
    while (ctx) {
        ++count;
        ctx = ctx->parentContext();
    }
    return count;
}

/*!
  Returns the context for the frame with the given \a index.
*/
QScriptContext *QScriptDebuggerBackend::context(int index) const
{
    if (index < 0)
        return 0;
    QScriptContext *ctx = engine()->currentContext();
    while (ctx) {
        if (index == 0)
            return ctx;
        ctx = ctx->parentContext();
        --index;
    }
    return 0;
}

/*!
  Returns a backtrace of the current execution.
*/
QStringList QScriptDebuggerBackend::backtrace() const
{
    if (!engine())
        return QStringList();
    return engine()->currentContext()->backtrace();
}

QList<qint64> QScriptDebuggerBackend::contextIds() const
{
    Q_D(const QScriptDebuggerBackend);
    if (!d->agent)
        return QList<qint64>();
    return d->agent->contextIds();
}

QScriptContextsDelta QScriptDebuggerBackend::contextsCheckpoint()
{
    Q_D(QScriptDebuggerBackend);
    if (!d->agent)
        return QScriptContextsDelta();
    return d->agent->contextsCheckpoint();
}

int QScriptDebuggerBackend::newScriptObjectSnapshot()
{
    Q_D(QScriptDebuggerBackend);
    int id = d->nextScriptObjectSnapshotId;
    ++d->nextScriptObjectSnapshotId;
    d->scriptObjectSnapshots[id] = new QScriptObjectSnapshot();
    return id;
}

QScriptObjectSnapshot *QScriptDebuggerBackend::scriptObjectSnapshot(int id) const
{
    Q_D(const QScriptDebuggerBackend);
    return d->scriptObjectSnapshots.value(id);
}

void QScriptDebuggerBackend::deleteScriptObjectSnapshot(int id)
{
    Q_D(QScriptDebuggerBackend);
    QScriptObjectSnapshot *snap = d->scriptObjectSnapshots.take(id);
    delete snap;
}

int QScriptDebuggerBackend::newScriptValueIterator(const QScriptValue &object)
{
    Q_D(QScriptDebuggerBackend);
    int id = d->nextScriptValueIteratorId;
    ++d->nextScriptValueIteratorId;
    d->scriptValueIterators[id] = new QScriptValueIterator(object);
    return id;
}

QScriptValueIterator *QScriptDebuggerBackend::scriptValueIterator(int id) const
{
    Q_D(const QScriptDebuggerBackend);
    return d->scriptValueIterators.value(id);
}

void QScriptDebuggerBackend::deleteScriptValueIterator(int id)
{
    Q_D(QScriptDebuggerBackend);
    QScriptValueIterator *it = d->scriptValueIterators.take(id);
    delete it;
}

bool QScriptDebuggerBackend::ignoreExceptions() const
{
    Q_D(const QScriptDebuggerBackend);
    return d->ignoreExceptions;
}

void QScriptDebuggerBackend::setIgnoreExceptions(bool ignore)
{
    Q_D(QScriptDebuggerBackend);
    d->ignoreExceptions = ignore;
}

/*!
  Returns a trace function. The trace function has similar semantics
  to the built-in print() function; however, instead of writing text
  to standard output, it generates a trace event containing the text.
*/
QScriptValue QScriptDebuggerBackend::traceFunction() const
{
    Q_D(const QScriptDebuggerBackend);
    if (!engine())
        return QScriptValue();
    QScriptValue fun = engine()->newFunction(QScriptDebuggerBackendPrivate::trace);
    fun.setData(qScriptValueFromValue(engine(), const_cast<QScriptDebuggerBackendPrivate*>(d)));
    return fun;
}

QScriptValue QScriptDebuggerBackend::assertFunction() const
{
    if (!engine())
        return QScriptValue();
    QScriptValue fun = engine()->newFunction(QScriptDebuggerBackendPrivate::qsassert);
    return fun;
}

QScriptValue QScriptDebuggerBackend::fileNameFunction() const
{
    if (!engine())
        return QScriptValue();
    QScriptValue fun = engine()->newFunction(QScriptDebuggerBackendPrivate::fileName);
    return fun;
}

QScriptValue QScriptDebuggerBackend::lineNumberFunction() const
{
    if (!engine())
        return QScriptValue();
    QScriptValue fun = engine()->newFunction(QScriptDebuggerBackendPrivate::lineNumber);
    return fun;
}

QScriptDebuggerCommandExecutor *QScriptDebuggerBackend::commandExecutor() const
{
    Q_D(const QScriptDebuggerBackend);
    if (d->commandExecutor)
        return d->commandExecutor;
    QScriptDebuggerBackendPrivate *dd = const_cast<QScriptDebuggerBackendPrivate*>(d);
    dd->commandExecutor = new QScriptDebuggerCommandExecutor();
    return dd->commandExecutor;
}

void QScriptDebuggerBackend::setCommandExecutor(QScriptDebuggerCommandExecutor *executor)
{
    Q_D(QScriptDebuggerBackend);
    d->commandExecutor = executor;
}

/*!
  \fn void QScriptDebuggerBackend::resume()

  This function is called when control should be returned back to the
  back-end, i.e. when script evaluation should be resumed after an
  event has been delivered.

  Subclasses must reimplement this function to make event() return.

  \sa event()
*/

/*!
  \fn void QScriptDebuggerBackend::event(const QScriptDebuggerEvent &event)

  This function is called when the back-end has generated the given \a event.

  Subclasses must reimplement this function to handle the
  event. Typically the event is forwarded to a
  QScriptDebuggerFrontend, which will in turn forward it to its
  QScriptDebuggerClient. The client may then query the front-end for
  information about the execution state, and call e.g.
  continueEvalution() to resume execution. This function should block
  until resume() is called.

  \sa resume()
*/

QT_END_NAMESPACE
