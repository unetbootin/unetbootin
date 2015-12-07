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

#include "qscriptdebugger_p.h"
#include "qscriptdebuggerconsole_p.h"
#include "qscriptdebuggerconsolecommandmanager_p.h"
#include "qscriptdebuggerconsolecommandjob_p.h"
#include "qscriptstdmessagehandler_p.h"
#include "qscriptdebuggerfrontend_p.h"
#include "qscriptdebuggereventhandlerinterface_p.h"
#include "qscriptdebuggerresponsehandlerinterface_p.h"
#include "qscriptdebuggerjobschedulerinterface_p.h"
#include "qscriptdebuggerconsolewidgetinterface_p.h"
#include "qscriptcompletionproviderinterface_p.h"
#include "qscriptcompletiontask_p.h"
#include "qscripttooltipproviderinterface_p.h"
#include "qscriptdebuggerstackwidgetinterface_p.h"
#include "qscriptdebuggerstackmodel_p.h"
#include "qscriptdebuggerscriptswidgetinterface_p.h"
#include "qscriptdebuggerscriptsmodel_p.h"
#include "qscriptdebuggerlocalswidgetinterface_p.h"
#include "qscriptdebuggerlocalsmodel_p.h"
#include "qscriptdebuggercodewidgetinterface_p.h"
#include "qscriptdebuggercodeviewinterface_p.h"
#include "qscriptdebuggercodefinderwidgetinterface_p.h"
#include "qscriptbreakpointswidgetinterface_p.h"
#include "qscriptbreakpointsmodel_p.h"
#include "qscriptdebugoutputwidgetinterface_p.h"
#include "qscripterrorlogwidgetinterface_p.h"
#include "qscriptdebuggerwidgetfactoryinterface_p.h"
#include "qscriptdebuggerevent_p.h"
#include "qscriptdebuggervalue_p.h"
#include "qscriptdebuggerresponse_p.h"
#include "qscriptdebuggercommand_p.h"
#include "qscriptdebuggercommandschedulerfrontend_p.h"
#include "qscriptdebuggercommandschedulerjob_p.h"
#include "qscriptdebuggerjob_p_p.h"
#include "qscriptxmlparser_p.h"

#include "qscriptenginedebuggerfrontend_p.h"
#include "qscriptdebuggerbackend_p.h"
#include <QtScript/qscriptengine.h>

#include "private/qobject_p.h"

#include <QtScript/qscriptcontext.h>
#include <QtScript/qscriptcontextinfo.h>

#include <QtCore/qcoreapplication.h>
#include <QtCore/qdir.h>
#include <QtCore/qfileinfo.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qdebug.h>

#include <QtGui/qaction.h>
#include <QtGui/qevent.h>
#include <QtGui/qicon.h>
#include <QtGui/qinputdialog.h>

QT_BEGIN_NAMESPACE
typedef QPair<QList<qint64>, QList<qint64> > QScriptScriptsDelta;
typedef QPair<QList<qint64>, QList<qint64> > QScriptContextsDelta;
QT_END_NAMESPACE

Q_DECLARE_METATYPE(QScriptScriptsDelta)

QT_BEGIN_NAMESPACE

Q_SCRIPT_EXPORT QString qt_scriptToXml(const QString &program, int lineNumber = 1);

namespace {

static int scriptDebuggerCount = 0;
static bool eventCallbackRegistered = false;
static bool widgetInPaintEvent = false;

static bool scriptDebuggerEventCallback(void **data)
{
    QEvent *event = reinterpret_cast<QEvent*>(data[1]);
    if (event->type() == QEvent::Paint) {
        QObject *receiver = reinterpret_cast<QObject*>(data[0]);
        bool was = widgetInPaintEvent;
        widgetInPaintEvent = true;
        QCoreApplication::instance()->notify(receiver, event);
        widgetInPaintEvent = was;
        bool *result = reinterpret_cast<bool*>(data[2]);
        *result = true;
        return true;
    }
    return false;
}

}

/*!
  \since 4.5
  \class QScriptDebugger
  \internal

  \brief The QScriptDebugger class provides a Qt Script debugger.

  \ingroup scripttools
  \mainclass
*/

class QScriptDebuggerPrivate
    : public QObjectPrivate,
      public QScriptDebuggerCommandSchedulerInterface,
      public QScriptDebuggerJobSchedulerInterface,
      public QScriptDebuggerEventHandlerInterface,
      public QScriptDebuggerResponseHandlerInterface,
      public QScriptCompletionProviderInterface,
      public QScriptToolTipProviderInterface
{
    Q_DECLARE_PUBLIC(QScriptDebugger)
public:
    QScriptDebuggerPrivate();
    ~QScriptDebuggerPrivate();

    int scheduleJob(QScriptDebuggerJob *job);
    void finishJob(QScriptDebuggerJob *job);
    void hibernateUntilEvaluateFinished(QScriptDebuggerJob *job);

    void maybeStartNewJob();

    int scheduleCommand(const QScriptDebuggerCommand &command,
                        QScriptDebuggerResponseHandlerInterface *responseHandler);

    void handleResponse(
        const QScriptDebuggerResponse &response, int commandId);
    bool debuggerEvent(const QScriptDebuggerEvent &event);

    QScriptCompletionTaskInterface *createCompletionTask(
        const QString &contents, int cursorPosition, int frameIndex, int options);

    QString toolTip(int frameIndex, int lineNumber,
                    const QStringList &path);

    static QPixmap pixmap(const QString &path);

    void startInteraction(QScriptDebuggerEvent::Type type,
                          qint64 scriptId, int lineNumber);
    void sync();
    void loadLocals(int frameIndex);
    QScriptDebuggerLocalsModel *createLocalsModel();
    void selectScriptForFrame(int frameIndex);
    void emitStoppedSignal();

    void maybeDelete(QWidget *widget);

    // private slots
    void _q_onLineEntered(const QString &contents);
    void _q_onCurrentFrameChanged(int frameIndex);
    void _q_onCurrentScriptChanged(qint64 scriptId);
    void _q_onScriptLocationSelected(int lineNumber);
    void _q_interrupt();
    void _q_continue();
    void _q_stepInto();
    void _q_stepOver();
    void _q_stepOut();
    void _q_runToCursor();
    void _q_runToNewScript();
    void _q_toggleBreakpoint();
    void _q_clearDebugOutput();
    void _q_clearErrorLog();
    void _q_clearConsole();
    void _q_findInScript();
    void _q_findNextInScript();
    void _q_findPreviousInScript();
    void _q_onFindCodeRequest(const QString &, int);
    void _q_goToLine();

    void executeConsoleCommand(const QString &command);
    void findCode(const QString &exp, int options);

    QScriptDebuggerFrontend *frontend;

    bool interactive;
    QScriptDebuggerConsole *console;

    int nextJobId;
    QList<QScriptDebuggerJob*> pendingJobs;
    QList<int> pendingJobIds;
    QScriptDebuggerJob *activeJob;
    bool activeJobHibernating;
    QHash<int, QScriptDebuggerCommand> watchedCommands;
    QHash<int, QScriptDebuggerResponseHandlerInterface*> responseHandlers;

    QScriptDebuggerConsoleWidgetInterface *consoleWidget;
    QScriptDebuggerStackWidgetInterface *stackWidget;
    QScriptDebuggerStackModel *stackModel;
    QScriptDebuggerScriptsWidgetInterface *scriptsWidget;
    QScriptDebuggerScriptsModel *scriptsModel;
    QScriptDebuggerLocalsWidgetInterface *localsWidget;
    QHash<int, QScriptDebuggerLocalsModel*> localsModels;
    QScriptDebuggerCodeWidgetInterface *codeWidget;
    QScriptDebuggerCodeFinderWidgetInterface *codeFinderWidget;
    QScriptBreakpointsWidgetInterface *breakpointsWidget;
    QScriptBreakpointsModel *breakpointsModel;
    QScriptDebugOutputWidgetInterface *debugOutputWidget;
    QScriptErrorLogWidgetInterface *errorLogWidget;
    QScriptDebuggerWidgetFactoryInterface *widgetFactory;

    QAction *interruptAction;
    QAction *continueAction;
    QAction *stepIntoAction;
    QAction *stepOverAction;
    QAction *stepOutAction;
    QAction *runToCursorAction;
    QAction *runToNewScriptAction;

    QAction *toggleBreakpointAction;

    QAction *clearDebugOutputAction;
    QAction *clearErrorLogAction;
    QAction *clearConsoleAction;

    QAction *findInScriptAction;
    QAction *findNextInScriptAction;
    QAction *findPreviousInScriptAction;
    QAction *goToLineAction;

    int updatesEnabledTimerId;
};

QScriptDebuggerPrivate::QScriptDebuggerPrivate()
{
    frontend = 0;
    activeJob = 0;
    activeJobHibernating = false;
    nextJobId = 0;
    interactive = false;

    console = new QScriptDebuggerConsole();
    QString scriptsPath = QLatin1String(":/qt/scripttools/debugging/scripts/commands");
    QScriptStdMessageHandler tmp;
    console->loadScriptedCommands(scriptsPath, &tmp);

    consoleWidget = 0;
    stackWidget = 0;
    stackModel = 0;
    scriptsWidget = 0;
    scriptsModel = 0;
    localsWidget = 0;
    codeWidget = 0;
    codeFinderWidget = 0;
    breakpointsWidget = 0;
    breakpointsModel = 0;
    debugOutputWidget = 0;
    errorLogWidget = 0;
    widgetFactory = 0;

    interruptAction = 0;
    continueAction = 0;
    stepIntoAction = 0;
    stepOverAction = 0;
    stepOutAction = 0;
    runToCursorAction = 0;
    runToNewScriptAction = 0;

    toggleBreakpointAction = 0;

    clearErrorLogAction = 0;
    clearDebugOutputAction = 0;
    clearConsoleAction = 0;

    findInScriptAction = 0;
    findNextInScriptAction = 0;
    findPreviousInScriptAction = 0;
    goToLineAction = 0;

    updatesEnabledTimerId = -1;
}

QScriptDebuggerPrivate::~QScriptDebuggerPrivate()
{
    delete console;
    qDeleteAll(pendingJobs);
    delete activeJob;
    maybeDelete(consoleWidget);
    maybeDelete(stackWidget);
    maybeDelete(scriptsWidget);
    maybeDelete(localsWidget);
    maybeDelete(codeWidget);
    maybeDelete(codeFinderWidget);
    maybeDelete(breakpointsWidget);
    maybeDelete(debugOutputWidget);
    maybeDelete(errorLogWidget);
}

void QScriptDebuggerPrivate::maybeDelete(QWidget *widget)
{
    if (widget && !widget->parent())
        delete widget;
}

QPixmap QScriptDebuggerPrivate::pixmap(const QString &path)
{
    static QString prefix = QString::fromLatin1(":/qt/scripttools/debugging/images/");
    return QPixmap(prefix + path);
}

/*!
  \reimp
*/
int QScriptDebuggerPrivate::scheduleJob(QScriptDebuggerJob *job)
{
    QScriptDebuggerJobPrivate *priv = QScriptDebuggerJobPrivate::get(job);
    Q_ASSERT(priv->jobScheduler == 0);
    priv->jobScheduler = this;
    int id = nextJobId;
    pendingJobs.append(job);
    pendingJobIds.append(id);
    maybeStartNewJob();
    return id;
}

/*!
  \reimp
*/
void QScriptDebuggerPrivate::finishJob(QScriptDebuggerJob *job)
{
    Q_UNUSED(job);
    Q_ASSERT(activeJob == job);
    delete activeJob;
    activeJob = 0;
    activeJobHibernating = false;
    maybeStartNewJob();
}

/*!
  \reimp
*/
void QScriptDebuggerPrivate::hibernateUntilEvaluateFinished(QScriptDebuggerJob *job)
{
    Q_UNUSED(job);
    Q_ASSERT(activeJob == job);
    activeJobHibernating = true;
}

/*!
  Starts a new job if appropriate.
*/
void QScriptDebuggerPrivate::maybeStartNewJob()
{
    if (activeJob || pendingJobs.isEmpty())
        return;
    activeJob = pendingJobs.takeFirst();
    activeJob->start();
}

/*!
  \reimp
*/
int QScriptDebuggerPrivate::scheduleCommand(
    const QScriptDebuggerCommand &command,
    QScriptDebuggerResponseHandlerInterface *responseHandler)
{
    if (!frontend)
        return -1;
    int id = frontend->scheduleCommand(command, this);
    if (responseHandler && (responseHandler != this))
        responseHandlers.insert(id, responseHandler);
    if ((command.type() == QScriptDebuggerCommand::SetBreakpoint)
        || (command.type() == QScriptDebuggerCommand::SetBreakpointData)
        || (command.type() == QScriptDebuggerCommand::DeleteBreakpoint)) {
        // need to watch this command and update the breakpoints model afterwards
        watchedCommands.insert(id, command);
    }
    return id;
}

/*!
  \reimp
*/
void QScriptDebuggerPrivate::handleResponse(
    const QScriptDebuggerResponse &response, int commandId)
{
    Q_Q(QScriptDebugger);
    if (watchedCommands.contains(commandId)) {
        QScriptDebuggerCommand command = watchedCommands.take(commandId);
        if (response.error() == QScriptDebuggerResponse::NoError) {
            if (!breakpointsModel)
                breakpointsModel = new QScriptBreakpointsModel(this, this, q);
            switch (command.type()) {
            case QScriptDebuggerCommand::SetBreakpoint: {
                int breakpointId = response.resultAsInt();
                QScriptBreakpointData data = command.breakpointData();
                breakpointsModel->addBreakpoint(breakpointId, data);
            }   break;
            case QScriptDebuggerCommand::SetBreakpointData: {
                int breakpointId = command.breakpointId();
                QScriptBreakpointData data = command.breakpointData();
                breakpointsModel->modifyBreakpoint(breakpointId, data);
            }   break;
            case QScriptDebuggerCommand::DeleteBreakpoint: {
                int breakpointId = command.breakpointId();
                breakpointsModel->removeBreakpoint(breakpointId);
            }   break;
            default:
                Q_ASSERT(false);
            }
        }
    } else if (response.async()) {
        interactive = false;
        // disable/invalidate/enable stuff
        if (continueAction)
            continueAction->setEnabled(false);
        if (stepIntoAction)
            stepIntoAction->setEnabled(false);
        if (stepOverAction)
            stepOverAction->setEnabled(false);
        if (stepOutAction)
            stepOutAction->setEnabled(false);
        if (runToCursorAction)
            runToCursorAction->setEnabled(false);
        if (runToNewScriptAction)
            runToNewScriptAction->setEnabled(false);
        if (interruptAction)
            interruptAction->setEnabled(true);

        // the timer is to avoid flicker when stepping
        if (stackWidget) {
            stackWidget->setUpdatesEnabled(false);
            stackWidget->setEnabled(false);
            if (updatesEnabledTimerId == -1)
                updatesEnabledTimerId = q->startTimer(75);
        }
        if (localsWidget) {
            localsWidget->setUpdatesEnabled(false);
            localsWidget->setEnabled(false);
            if (updatesEnabledTimerId == -1)
                updatesEnabledTimerId = q->startTimer(75);
        }
        if (codeWidget)
            codeWidget->invalidateExecutionLineNumbers();

        emit q->started();
    }

    QScriptDebuggerResponseHandlerInterface *realHandler = responseHandlers.take(commandId);
    if (realHandler)
        realHandler->handleResponse(response, commandId);
}

/*!
  \reimp

  Handles a debugger event from the frontend.
*/
bool QScriptDebuggerPrivate::debuggerEvent(const QScriptDebuggerEvent &event)
{
    Q_Q(QScriptDebugger);
    switch (event.type()) {
    case QScriptDebuggerEvent::None:
    case QScriptDebuggerEvent::UserEvent:
    case QScriptDebuggerEvent::MaxUserEvent:
        Q_ASSERT(false);
        break;

    case QScriptDebuggerEvent::Trace:
        if (!debugOutputWidget && widgetFactory)
            q->setDebugOutputWidget(widgetFactory->createDebugOutputWidget());
        if (debugOutputWidget)
            debugOutputWidget->message(QtDebugMsg, event.message());
        return true; // trace doesn't stall execution

    case QScriptDebuggerEvent::SteppingFinished: {
        if (!consoleWidget && widgetFactory)
            q->setConsoleWidget(widgetFactory->createConsoleWidget());
        if (consoleWidget) {
            QString msg = event.message();
            if (!msg.isEmpty())
                consoleWidget->message(QtDebugMsg, msg);
        }
    }   break;

    case QScriptDebuggerEvent::Interrupted:
    case QScriptDebuggerEvent::LocationReached:
        break;

    case QScriptDebuggerEvent::Breakpoint: {
        int bpId = event.breakpointId();
        if (!consoleWidget && widgetFactory)
            q->setConsoleWidget(widgetFactory->createConsoleWidget());
        if (consoleWidget) {
            consoleWidget->message(QtDebugMsg,
                                   QString::fromLatin1("Breakpoint %0 at %1, line %2.")
                                   .arg(bpId).arg(event.fileName())
                                   .arg(event.lineNumber()));
        }
        if (breakpointsModel->breakpointData(bpId).isSingleShot())
            breakpointsModel->removeBreakpoint(bpId);
    }   break;

    case QScriptDebuggerEvent::Exception: {
        if (event.hasExceptionHandler()) {
            // Let the exception be handled like normal.
            // We may want to add a "Break on all exceptions" option
            // to be able to customize this behavior.
            return true;
        }
        if (!consoleWidget && widgetFactory)
            q->setConsoleWidget(widgetFactory->createConsoleWidget());
        if (!errorLogWidget && widgetFactory)
            q->setErrorLogWidget(widgetFactory->createErrorLogWidget());
        if (consoleWidget || errorLogWidget) {
            QString fn = event.fileName();
            if (fn.isEmpty()) {
                if (event.scriptId() != -1)
                    fn = QString::fromLatin1("<anonymous script, id=%0>").arg(event.scriptId());
                else
                    fn = QString::fromLatin1("<native>");
            }
            QString msg = QString::fromLatin1("Uncaught exception at %0:%1: %2").arg(fn)
                          .arg(event.lineNumber()).arg(event.message());
            if (consoleWidget)
                consoleWidget->message(QtCriticalMsg, msg);
            if (errorLogWidget)
                errorLogWidget->message(QtCriticalMsg, msg);
        }
    }   break;

    case QScriptDebuggerEvent::InlineEvalFinished: {
        QScriptDebuggerValue result = event.scriptValue();
        Q_ASSERT(console != 0);
        int action = console->evaluateAction();
        console->setEvaluateAction(0);
        switch (action) {
        case 0: { // eval command
            if (activeJob) {
                if (activeJobHibernating) {
                    activeJobHibernating = false;
                    activeJob->evaluateFinished(result);
                }
            } else if (consoleWidget) {
                // ### if the result is an object, need to do a tostring job on it
//          messageHandler->message(QtDebugMsg, result.toString());
                if (result.type() != QScriptDebuggerValue::UndefinedValue)
                    consoleWidget->message(QtDebugMsg, event.message());
            }
        }   break;
        case 1: { // return command
            QScriptDebuggerCommandSchedulerFrontend frontend(this, this);
            frontend.scheduleForceReturn(console->currentFrameIndex(), result);
        }   return false;
        }
        if (!event.isNestedEvaluate()) {
            // in the case when evaluate() was called while the
            // engine was not running, we don't want to enter interactive mode
            return true;
        }
    }   break;

    case QScriptDebuggerEvent::DebuggerInvocationRequest: {
        if (!consoleWidget && widgetFactory)
            q->setConsoleWidget(widgetFactory->createConsoleWidget());
        if (consoleWidget) {
            QString fn = event.fileName();
            if (fn.isEmpty())
                fn = QString::fromLatin1("<anonymous script, id=%0>").arg(event.scriptId());
            consoleWidget->message(QtDebugMsg,
                                   QString::fromLatin1("Debugger invoked from %1, line %2.")
                                   .arg(fn).arg(event.lineNumber()));
        }
    }   break;

    case QScriptDebuggerEvent::ForcedReturn: {
    }   break;

    }

    if (widgetInPaintEvent) {
        QString msg = QString::fromLatin1("Suspending evaluation in paintEvent() is not supported; resuming.");
        if (!consoleWidget && widgetFactory)
            q->setConsoleWidget(widgetFactory->createConsoleWidget());
        if (!errorLogWidget && widgetFactory)
            q->setErrorLogWidget(widgetFactory->createErrorLogWidget());
        if (consoleWidget)
            consoleWidget->message(QtWarningMsg, msg);
        if (errorLogWidget)
            errorLogWidget->message(QtCriticalMsg, msg);
        return true;
    }

    if (activeJobHibernating) {
        // evaluate() did not finish normally (e.g. due to a breakpoint),
        // so cancel the job that's waiting for it
        delete activeJob;
        activeJob = 0;
        activeJobHibernating = false;
    }

    startInteraction(event.type(), event.scriptId(), event.lineNumber());
    return !interactive;
}

/*!
  \reimp
*/
QString QScriptDebuggerPrivate::toolTip(int frameIndex, int lineNumber,
                                        const QStringList &path)
{
    if (frameIndex == -1) {
        if (stackWidget)
            frameIndex = stackWidget->currentFrameIndex();
        else
            frameIndex = console->currentFrameIndex();
    }
    // ### cheating for now, need to use async API
    QScriptEngineDebuggerFrontend *edf = static_cast<QScriptEngineDebuggerFrontend*>(frontend);
    QScriptDebuggerBackend *backend = edf->backend();
    QScriptContext *ctx = backend->context(frameIndex);
    if (!ctx || path.isEmpty())
        return QString();
    QScriptContextInfo ctxInfo(ctx);
    if (ctx->callee().isValid()
        && ((lineNumber < ctxInfo.functionStartLineNumber())
            || (lineNumber > ctxInfo.functionEndLineNumber()))) {
        return QString();
    }
    QScriptValueList objects;
    int pathIndex = 0;
    if (path.at(0) == QLatin1String("this")) {
        objects.append(ctx->thisObject());
        ++pathIndex;
    } else {
#if QT_VERSION >= 0x040500
        objects << ctx->scopeChain();
#else
        objects.append(ctx->activationObject());
#endif
    }
    for (int i = 0; i < objects.size(); ++i) {
        QScriptValue val = objects.at(i);
        for (int j = pathIndex; val.isValid() && (j < path.size()); ++j) {
            val = val.property(path.at(j));
        }
        if (val.isValid()) {
            bool hadException = (ctx->state() == QScriptContext::ExceptionState);
            QString str = val.toString();
            if (!hadException && backend->engine()->hasUncaughtException())
                backend->engine()->clearExceptions();
            return str;
        }
    }
    return QString();
}

/*!
  \reimp
*/
QScriptCompletionTaskInterface *QScriptDebuggerPrivate::createCompletionTask(
    const QString &contents, int cursorPosition, int frameIndex, int options)
{
    return new QScriptCompletionTask(
        contents, cursorPosition, frameIndex, frontend,
        (options & QScriptCompletionProviderInterface::ConsoleCommandCompletion) ? console : 0);
}

/*!
  Slot called when a line has been entered in the console widget.
*/
void QScriptDebuggerPrivate::_q_onLineEntered(const QString &contents)
{
    QScriptDebuggerConsoleCommandJob *commandJob;
    commandJob = console->consumeInput(contents, consoleWidget, this);
    if (commandJob != 0) {
        scheduleJob(commandJob);
        consoleWidget->setLineContinuationMode(false);
    } else if (console->hasIncompleteInput()) {
        consoleWidget->setLineContinuationMode(true);
    }
}

/*!
  Slot called when the current index has changed in the stack widget.
*/
void QScriptDebuggerPrivate::_q_onCurrentFrameChanged(int frameIndex)
{
    loadLocals(frameIndex);
    selectScriptForFrame(frameIndex);
}

/*!
  Slot called when the current script has changed in the scripts widget.
*/
void QScriptDebuggerPrivate::_q_onCurrentScriptChanged(qint64 scriptId)
{
    if (codeWidget && (codeWidget->currentScriptId() != scriptId)) {
        codeWidget->setCurrentScript(scriptId);
        QScriptDebuggerCodeViewInterface *view = codeWidget->currentView();
        if (view)
            view->setExecutionLineNumber(-1, /*error=*/false);
    }
}

void QScriptDebuggerPrivate::_q_onScriptLocationSelected(int lineNumber)
{
    QScriptDebuggerCodeViewInterface *view = codeWidget->currentView();
    if (!view)
        return;
    view->gotoLine(lineNumber);
}

void QScriptDebuggerPrivate::_q_interrupt()
{
    executeConsoleCommand(QString::fromLatin1("interrupt"));
}

void QScriptDebuggerPrivate::_q_continue()
{
    executeConsoleCommand(QString::fromLatin1("continue"));
}

void QScriptDebuggerPrivate::_q_stepInto()
{
    executeConsoleCommand(QString::fromLatin1("step"));
}

void QScriptDebuggerPrivate::_q_stepOver()
{
    executeConsoleCommand(QString::fromLatin1("next"));
}

void QScriptDebuggerPrivate::_q_stepOut()
{
    executeConsoleCommand(QString::fromLatin1("finish"));
}

void QScriptDebuggerPrivate::_q_runToCursor()
{
    qint64 scriptId = codeWidget->currentScriptId();
    int lineNumber = codeWidget->currentView()->cursorLineNumber();
    QScriptDebuggerCommandSchedulerFrontend frontend(this, this);
    frontend.scheduleRunToLocation(scriptId, lineNumber);
}

void QScriptDebuggerPrivate::_q_runToNewScript()
{
    QScriptDebuggerCommandSchedulerFrontend frontend(this, this);
    frontend.scheduleRunToLocation(QString(), -1);
}

void QScriptDebuggerPrivate::_q_toggleBreakpoint()
{
    Q_ASSERT(codeWidget != 0);
    QScriptDebuggerCodeViewInterface *view = codeWidget->currentView();
    if (!view)
        return;
    qint64 scriptId = codeWidget->currentScriptId();
    int lineNumber = view->cursorLineNumber();
    Q_ASSERT(breakpointsModel != 0);
    int bpId = breakpointsModel->resolveBreakpoint(scriptId, lineNumber);
    if (bpId != -1) {
        breakpointsModel->deleteBreakpoint(bpId);
    } else {
        QScriptBreakpointData data(scriptId, lineNumber);
        if (scriptsModel)
            data.setFileName(scriptsModel->scriptData(scriptId).fileName());
        breakpointsModel->setBreakpoint(data);
    }
}

void QScriptDebuggerPrivate::_q_clearDebugOutput()
{
    if (debugOutputWidget)
        debugOutputWidget->clear();
}

void QScriptDebuggerPrivate::_q_clearErrorLog()
{
    if (errorLogWidget)
        errorLogWidget->clear();
}

void QScriptDebuggerPrivate::_q_clearConsole()
{
    if (consoleWidget)
        consoleWidget->clear();
}

void QScriptDebuggerPrivate::executeConsoleCommand(const QString &command)
{
    QString tmp = console->incompleteInput();
    console->setIncompleteInput(QString());
    QScriptDebuggerJob *job = console->consumeInput(console->commandPrefix() + command, debugOutputWidget, this);
    console->setIncompleteInput(tmp);
    if (job != 0) {
        scheduleJob(job);
        // once to send the command...
        QCoreApplication::processEvents();
        // ... and once to receive the response
        QCoreApplication::processEvents();
    }
}

void QScriptDebuggerPrivate::_q_findInScript()
{
    if (!codeFinderWidget && widgetFactory)
        q_func()->setCodeFinderWidget(widgetFactory->createCodeFinderWidget());
    if (codeFinderWidget) {
        codeFinderWidget->show();
        codeFinderWidget->setFocus(Qt::OtherFocusReason);
    }
}

void QScriptDebuggerPrivate::_q_findNextInScript()
{
    findCode(codeFinderWidget->text(), codeFinderWidget->findOptions());
}

void QScriptDebuggerPrivate::_q_findPreviousInScript()
{
    int options = codeFinderWidget->findOptions();
    options |= QTextDocument::FindBackward;
    findCode(codeFinderWidget->text(), options);
}

void QScriptDebuggerPrivate::_q_onFindCodeRequest(
    const QString &exp, int options)
{
    findCode(exp, options);
    if (findNextInScriptAction)
        findNextInScriptAction->setEnabled(!exp.isEmpty());
    if (findPreviousInScriptAction)
        findPreviousInScriptAction->setEnabled(!exp.isEmpty());
}

void QScriptDebuggerPrivate::findCode(const QString &exp, int options)
{
    QScriptDebuggerCodeViewInterface *view = codeWidget->currentView();
    if (!view)
        return;
    int result = view->find(exp, options);
    codeFinderWidget->setOK(((result & 0x1) != 0) || exp.isEmpty());
    codeFinderWidget->setWrapped((result & 0x2) != 0);
}

void QScriptDebuggerPrivate::_q_goToLine()
{
    QScriptDebuggerCodeViewInterface *view = codeWidget->currentView();
    if (!view)
        return;
    bool ok = false;
    int lineNumber = QInputDialog::getInteger(0, QObject::tr("Go to Line"),
                                              QObject::tr("Line:"),
                                              view->cursorLineNumber(),
                                              1, INT_MAX, 1, &ok);
    if (ok)
        view->gotoLine(lineNumber);
}

class QScriptDebuggerShowLineJob : public QScriptDebuggerCommandSchedulerJob
{
public:
    QScriptDebuggerShowLineJob(qint64 scriptId, int lineNumber,
                               QScriptMessageHandlerInterface *messageHandler,
                               QScriptDebuggerCommandSchedulerInterface *scheduler)
        : QScriptDebuggerCommandSchedulerJob(scheduler),
          m_scriptId(scriptId), m_lineNumber(lineNumber),
        m_messageHandler(messageHandler) {}

    void start()
    {
        QScriptDebuggerCommandSchedulerFrontend frontend(commandScheduler(), this);
        frontend.scheduleGetScriptData(m_scriptId);
    }
    void handleResponse(const QScriptDebuggerResponse &response, int /*commandId*/)
    {
        QScriptScriptData data = response.resultAsScriptData();
        QString line = data.lines(m_lineNumber, 1).value(0);
        m_messageHandler->message(QtDebugMsg, QString::fromLatin1("%0\t%1")
                                  .arg(m_lineNumber).arg(line));
        finish();
    }

private:
    qint64 m_scriptId;
    int m_lineNumber;
    QScriptMessageHandlerInterface *m_messageHandler;
};

namespace {

class SyncStackJob : public QScriptDebuggerCommandSchedulerJob
{
public:
    SyncStackJob(QScriptDebuggerPrivate *debugger)
        : QScriptDebuggerCommandSchedulerJob(debugger),
          m_debugger(debugger), m_index(0) {}
    void start()
    {
        QScriptDebuggerCommandSchedulerFrontend frontend(commandScheduler(), this);
        frontend.scheduleGetContextInfo(m_index); // ### getContextInfos()
    }
    void handleResponse(const QScriptDebuggerResponse &response,
                        int)
    {
        QScriptDebuggerCommandSchedulerFrontend frontend(commandScheduler(), this);
        if (response.error() != QScriptDebuggerResponse::InvalidContextIndex) {
            m_infos.append(response.resultAsContextInfo());
            frontend.scheduleGetContextInfo(++m_index);
        } else {
            m_debugger->stackModel->setContextInfos(m_infos);
            if (m_debugger->stackWidget->currentFrameIndex() == -1)
                m_debugger->stackWidget->setCurrentFrameIndex(0);
            m_debugger->stackWidget->setUpdatesEnabled(true);
            m_debugger->stackWidget->setEnabled(true);
            finish();
        }
    }

private:
    QScriptDebuggerPrivate *m_debugger;
    int m_index;
    QList<QScriptContextInfo> m_infos;
};

class SyncScriptsJob : public QScriptDebuggerCommandSchedulerJob
{
public:
    SyncScriptsJob(QScriptDebuggerPrivate *debugger)
        : QScriptDebuggerCommandSchedulerJob(debugger),
          m_debugger(debugger), m_index(-1) {}

    void start()
    {
        QScriptDebuggerCommandSchedulerFrontend frontend(commandScheduler(), this);
        frontend.scheduleScriptsCheckpoint();
    }
    void handleResponse(const QScriptDebuggerResponse &response,
                        int)
    {
        QScriptDebuggerCommandSchedulerFrontend frontend(commandScheduler(), this);
        if (m_index == -1) {
            QScriptScriptsDelta delta;
            delta = qvariant_cast<QScriptScriptsDelta>(response.result());

            const QList<qint64> &removed = delta.second;
            for (int i = 0; i < removed.size(); ++i)
                m_debugger->scriptsModel->removeScript(removed.at(i));

            m_added = delta.first;
            if (!m_added.isEmpty()) {
                frontend.scheduleGetScriptData(m_added.at(++m_index));
            } else {
                m_debugger->scriptsModel->commit();
                finish();
            }
        } else {
            QScriptScriptData data = response.resultAsScriptData();
            qint64 scriptId = m_added.at(m_index);
            m_debugger->scriptsModel->addScript(scriptId, data);

#if QT_VERSION >= 0x040500
            // ### could be slow, might want to do this in a separate thread
            QString xml = qt_scriptToXml(data.contents(), data.baseLineNumber());
            QScriptXmlParser::Result extraInfo = QScriptXmlParser::parse(xml);
            m_debugger->scriptsModel->addExtraScriptInfo(
                scriptId, extraInfo.functionsInfo, extraInfo.executableLineNumbers);
#endif

            if (++m_index < m_added.size())
                frontend.scheduleGetScriptData(m_added.at(m_index));
            else {
                m_debugger->scriptsModel->commit();
                finish();
            }
        }
    }

private:
    QScriptDebuggerPrivate *m_debugger;
    int m_index;
    QList<qint64> m_added;
};

class SyncBreakpointsJob : public QScriptDebuggerCommandSchedulerJob
{
public:
    SyncBreakpointsJob(QScriptDebuggerPrivate *debugger)
        : QScriptDebuggerCommandSchedulerJob(debugger),
          m_debugger(debugger) {}
    void start()
    {
        QScriptDebuggerCommandSchedulerFrontend frontend(commandScheduler(), this);
        frontend.scheduleGetBreakpoints();
    }
    void handleResponse(const QScriptDebuggerResponse &response,
                        int)
    {
        QScriptBreakpointMap breakpoints = response.resultAsBreakpoints();
        QScriptBreakpointMap::const_iterator it;
        for (it = breakpoints.constBegin(); it != breakpoints.constEnd(); ++it) {
            int id = it.key();
            QScriptBreakpointData newData = it.value();
            QScriptBreakpointData existingData = m_debugger->breakpointsModel->breakpointData(id);
            if (existingData.isValid() && (existingData != newData))
                m_debugger->breakpointsModel->modifyBreakpoint(id, newData);
        }
        finish();
    }

private:
    QScriptDebuggerPrivate *m_debugger;
    int m_index;
    QList<QScriptContextInfo> m_infos;
};

class SyncLocalsJob : public QScriptDebuggerCommandSchedulerJob
{
public:
    SyncLocalsJob(QScriptDebuggerPrivate *debugger)
        : QScriptDebuggerCommandSchedulerJob(debugger),
          m_debugger(debugger) {}

    void start()
    {
        QScriptDebuggerCommandSchedulerFrontend frontend(commandScheduler(), this);
        frontend.scheduleContextsCheckpoint();
    }
    void handleResponse(const QScriptDebuggerResponse &response,
                        int)
    {
        QScriptContextsDelta delta = qvariant_cast<QScriptContextsDelta>(response.result());
        for (int i = 0; i < delta.first.size(); ++i) {
            QScriptDebuggerLocalsModel *model = m_debugger->localsModels.take(delta.first.at(i));
            delete model;
        }
        finish();
    }

private:
    QScriptDebuggerPrivate *m_debugger;
};

class LoadLocalsJob : public QScriptDebuggerCommandSchedulerJob
{
public:
    LoadLocalsJob(QScriptDebuggerPrivate *debugger, int frameIndex)
        : QScriptDebuggerCommandSchedulerJob(debugger),
          m_debugger(debugger), m_frameIndex(frameIndex), m_state(0) {}

    void start()
    {
        QScriptDebuggerCommandSchedulerFrontend frontend(commandScheduler(), this);
        frontend.scheduleGetContextId(m_frameIndex);
    }
    void handleResponse(const QScriptDebuggerResponse &response,
                        int)
    {
        QScriptDebuggerCommandSchedulerFrontend frontend(commandScheduler(), this);
        qint64 contextId = response.resultAsLongLong();
        QScriptDebuggerLocalsModel *model = m_debugger->localsModels.value(contextId);
        if (model) {
            model->sync(m_frameIndex);
        } else {
            model = m_debugger->createLocalsModel();
            m_debugger->localsModels.insert(contextId, model);
            model->init(m_frameIndex);
        }
        if (m_debugger->localsWidget) {
            if (m_debugger->localsWidget->localsModel() != model) // ### bug in qtreeview
                m_debugger->localsWidget->setLocalsModel(model);
            m_debugger->localsWidget->setUpdatesEnabled(true);
            m_debugger->localsWidget->setEnabled(true);
        }
        finish();
    }

private:
    QScriptDebuggerPrivate *m_debugger;
    int m_frameIndex;
    int m_state;
};

class EmitStoppedSignalJob : public QScriptDebuggerJob
{
public:
    EmitStoppedSignalJob(QScriptDebuggerPrivate *debugger)
        : m_debugger(debugger) {}

    void start()
    {
        m_debugger->emitStoppedSignal();
        finish();
    }

private:
    QScriptDebuggerPrivate *m_debugger;
};

} // namespace

void QScriptDebuggerPrivate::startInteraction(QScriptDebuggerEvent::Type type,
                                              qint64 scriptId, int lineNumber)
{
    Q_Q(QScriptDebugger);
    if (type != QScriptDebuggerEvent::InlineEvalFinished) {
        if (stackWidget)
            stackWidget->setCurrentFrameIndex(0);
        console->setCurrentFrameIndex(0);
        console->setCurrentScriptId(scriptId);
        console->setCurrentLineNumber(lineNumber);
    }

    if ((scriptId != -1) && consoleWidget) {
        QScriptDebuggerJob *job = new QScriptDebuggerShowLineJob(scriptId, lineNumber, consoleWidget, this);
        scheduleJob(job);
    }

    sync();

    if (!interactive) {
        interactive = true;
        if (updatesEnabledTimerId != -1) {
            q->killTimer(updatesEnabledTimerId);
            updatesEnabledTimerId = -1;
        }
        console->bumpSessionId();
        scheduleJob(new EmitStoppedSignalJob(this));
    }

    if (consoleWidget)
        consoleWidget->activateWindow();
    else if (codeWidget)
        codeWidget->activateWindow();

    if (continueAction)
        continueAction->setEnabled(true);
    if (stepIntoAction)
        stepIntoAction->setEnabled(true);
    if (stepOverAction)
        stepOverAction->setEnabled(true);
    if (stepOutAction)
        stepOutAction->setEnabled(true);
    if (runToCursorAction)
        runToCursorAction->setEnabled(true);
    if (runToNewScriptAction)
        runToNewScriptAction->setEnabled(true);
    if (interruptAction)
        interruptAction->setEnabled(false);

    bool hasScript = (codeWidget != 0);
    if (findInScriptAction)
        findInScriptAction->setEnabled(hasScript);
    if (toggleBreakpointAction)
        toggleBreakpointAction->setEnabled(hasScript);
    if (goToLineAction)
        goToLineAction->setEnabled(hasScript);
}

void QScriptDebuggerPrivate::sync()
{
    if (localsWidget) {
        QScriptDebuggerJob *job = new SyncLocalsJob(this);
        scheduleJob(job);
    }
    if (scriptsModel) {
        QScriptDebuggerJob *job = new SyncScriptsJob(this);
        scheduleJob(job);
    }
    if (stackModel) {
        QScriptDebuggerJob *job = new SyncStackJob(this);
        scheduleJob(job);
    }
    if (breakpointsModel) {
        // need to sync because the ignore-count could have changed
        QScriptDebuggerJob *job = new SyncBreakpointsJob(this);
        scheduleJob(job);
    }

    if (stackWidget && (stackWidget->currentFrameIndex() != -1)) {
        int index = stackWidget->currentFrameIndex();
        loadLocals(index);
        selectScriptForFrame(index);
    } else if (codeWidget && (console->currentFrameIndex() != -1)) {
        selectScriptForFrame(console->currentFrameIndex());
    }
}

void QScriptDebuggerPrivate::loadLocals(int frameIndex)
{
    LoadLocalsJob *job = new LoadLocalsJob(this, frameIndex);
    scheduleJob(job);
}

QScriptDebuggerLocalsModel *QScriptDebuggerPrivate::createLocalsModel()
{
    return new QScriptDebuggerLocalsModel(this, this, q_func());
}

namespace {

class ShowFrameCodeJob : public QScriptDebuggerCommandSchedulerJob
{
public:
    ShowFrameCodeJob(QScriptDebuggerPrivate *debugger, int frameIndex)
        : QScriptDebuggerCommandSchedulerJob(debugger),
          m_debugger(debugger), m_frameIndex(frameIndex) {}

    void start()
    {
        QScriptDebuggerCommandSchedulerFrontend frontend(commandScheduler(), this);
        frontend.scheduleGetContextInfo(m_frameIndex);
    }
    void handleResponse(const QScriptDebuggerResponse &response,
                        int)
    {
        if (m_info.isNull()) {
            m_info = response.resultAsContextInfo();
            QScriptDebuggerCommandSchedulerFrontend frontend(commandScheduler(), this);
            frontend.scheduleGetContextState(m_frameIndex);
        } else {
            int contextState = response.resultAsInt();
            bool error = (contextState == QScriptContext::ExceptionState);
            if (m_debugger->scriptsWidget) {
                m_debugger->scriptsWidget->setCurrentScript(m_info.scriptId());
            }
            if (m_debugger->codeWidget) {
                m_debugger->codeWidget->setCurrentScript(m_info.scriptId());
                QScriptDebuggerCodeViewInterface *view = m_debugger->codeWidget->currentView();
                if (view)
                    view->setExecutionLineNumber(m_info.lineNumber(), error);
            }
            finish();
        }
    }

private:
    QScriptDebuggerPrivate *m_debugger;
    int m_frameIndex;
    QScriptContextInfo m_info;
};

} // namespace

void QScriptDebuggerPrivate::selectScriptForFrame(int frameIndex)
{
    QScriptDebuggerJob *job = new ShowFrameCodeJob(this, frameIndex);
    scheduleJob(job);
}

void QScriptDebuggerPrivate::emitStoppedSignal()
{
    emit q_func()->stopped();
}

/*!
  Constructs a new QScriptDebugger object.
*/
QScriptDebugger::QScriptDebugger(QObject *parent)
    : QObject(*new QScriptDebuggerPrivate, parent)
{
    ++scriptDebuggerCount;
}

/*!
  Destroys this QScriptDebugger.
*/
QScriptDebugger::~QScriptDebugger()
{
    --scriptDebuggerCount;
    if ((scriptDebuggerCount == 0) && eventCallbackRegistered) {
        eventCallbackRegistered = false;
        QInternal::unregisterCallback(QInternal::EventNotifyCallback,
                                      scriptDebuggerEventCallback);
    }
}

/*!
  \internal
*/
QScriptDebugger::QScriptDebugger(QScriptDebuggerPrivate &dd, QObject *parent)
    : QObject(dd, parent)
{
}

QScriptDebuggerFrontend *QScriptDebugger::frontend() const
{
    Q_D(const QScriptDebugger);
    return d->frontend;
}

void QScriptDebugger::setFrontend(QScriptDebuggerFrontend *frontend)
{
    Q_D(QScriptDebugger);
    if (d->frontend)
        d->frontend->setEventHandler(0);
    d->frontend = frontend;
    if (frontend) {
        frontend->setEventHandler(d);
        if (!eventCallbackRegistered) {
            eventCallbackRegistered = true;
            QInternal::registerCallback(QInternal::EventNotifyCallback,
                                        scriptDebuggerEventCallback);
        }
    }
}

QScriptDebuggerConsoleWidgetInterface *QScriptDebugger::consoleWidget() const
{
    Q_D(const QScriptDebugger);
    return d->consoleWidget;
}

void QScriptDebugger::setConsoleWidget(QScriptDebuggerConsoleWidgetInterface *consoleWidget)
{
    Q_D(QScriptDebugger);
    if (d->consoleWidget) {
        QObject::disconnect(d->consoleWidget, 0, this, 0);
    }
    d->consoleWidget = consoleWidget;
    if (consoleWidget) {
        consoleWidget->setCommandHistorian(d->console);
        consoleWidget->setCompletionProvider(d);
        QObject::connect(consoleWidget, SIGNAL(lineEntered(QString)),
                         this, SLOT(_q_onLineEntered(QString)));

        d->console->showDebuggerInfoMessage(consoleWidget);
    }
}

QScriptDebuggerStackWidgetInterface *QScriptDebugger::stackWidget() const
{
    Q_D(const QScriptDebugger);
    return d->stackWidget;
}

void QScriptDebugger::setStackWidget(QScriptDebuggerStackWidgetInterface *stackWidget)
{
    Q_D(QScriptDebugger);
    if (d->stackWidget) {
        QObject::disconnect(d->stackWidget, 0, this, 0);
    }
    d->stackWidget = stackWidget;
    if (stackWidget) {
        if (!d->stackModel) {
            d->stackModel = new QScriptDebuggerStackModel(this);
            if (d->interactive)
                d->scheduleJob(new SyncStackJob(d));
        }
        stackWidget->setStackModel(d->stackModel);
        QObject::connect(stackWidget, SIGNAL(currentFrameChanged(int)),
                         this, SLOT(_q_onCurrentFrameChanged(int)));
    }
}

QScriptDebuggerScriptsWidgetInterface *QScriptDebugger::scriptsWidget() const
{
    Q_D(const QScriptDebugger);
    return d->scriptsWidget;
}

void QScriptDebugger::setScriptsWidget(QScriptDebuggerScriptsWidgetInterface *scriptsWidget)
{
    Q_D(QScriptDebugger);
    if (d->scriptsWidget) {
        QObject::disconnect(d->scriptsWidget, 0, this, 0);
    }
    d->scriptsWidget = scriptsWidget;
    if (scriptsWidget) {
        if (!d->scriptsModel) {
            d->scriptsModel = new QScriptDebuggerScriptsModel(this);
            if (d->interactive)
                d->scheduleJob(new SyncScriptsJob(d));
        }
        scriptsWidget->setScriptsModel(d->scriptsModel);
        QObject::connect(scriptsWidget, SIGNAL(currentScriptChanged(qint64)),
                         this, SLOT(_q_onCurrentScriptChanged(qint64)));
        QObject::connect(d->scriptsWidget, SIGNAL(scriptLocationSelected(int)),
                         this, SLOT(_q_onScriptLocationSelected(int)));
    }
}

QScriptDebuggerLocalsWidgetInterface *QScriptDebugger::localsWidget() const
{
    Q_D(const QScriptDebugger);
    return d->localsWidget;
}

void QScriptDebugger::setLocalsWidget(QScriptDebuggerLocalsWidgetInterface *localsWidget)
{
    Q_D(QScriptDebugger);
    if (d->localsWidget) {
        // ### d->localsWidget->setLocalsModel(0);
    }
    localsWidget->setCompletionProvider(d);
    d->localsWidget = localsWidget;
}

QScriptDebuggerCodeWidgetInterface *QScriptDebugger::codeWidget() const
{
    Q_D(const QScriptDebugger);
    return d->codeWidget;
}

void QScriptDebugger::setCodeWidget(QScriptDebuggerCodeWidgetInterface *codeWidget)
{
    Q_D(QScriptDebugger);
    if (d->codeWidget) {
        d->codeWidget->removeEventFilter(this);
    }
    d->codeWidget = codeWidget;
    if (codeWidget) {
        if (!d->scriptsModel) {
            d->scriptsModel = new QScriptDebuggerScriptsModel(this);
            if (d->interactive)
                d->scheduleJob(new SyncScriptsJob(d));
        }
        codeWidget->setScriptsModel(d->scriptsModel);
        if (!d->breakpointsModel) {
            d->breakpointsModel = new QScriptBreakpointsModel(d, d, this);
            if (d->interactive)
                d->scheduleJob(new SyncBreakpointsJob(d));
        }
        codeWidget->setBreakpointsModel(d->breakpointsModel);
        codeWidget->setToolTipProvider(d);
        codeWidget->installEventFilter(this);
    }
    bool hasScript = (codeWidget != 0) && (codeWidget->currentView() != 0);
    if (d->findInScriptAction)
        d->findInScriptAction->setEnabled(hasScript && (d->codeFinderWidget != 0));
    if (d->goToLineAction)
        d->goToLineAction->setEnabled(hasScript);
    if (d->toggleBreakpointAction)
        d->toggleBreakpointAction->setEnabled(hasScript);
}

QScriptDebuggerCodeFinderWidgetInterface *QScriptDebugger::codeFinderWidget() const
{
    Q_D(const QScriptDebugger);
    return d->codeFinderWidget;
}

void QScriptDebugger::setCodeFinderWidget(QScriptDebuggerCodeFinderWidgetInterface *codeFinderWidget)
{
    Q_D(QScriptDebugger);
    if (d->codeFinderWidget) {
        QObject::disconnect(d->codeFinderWidget, 0, this, 0);
    }
    d->codeFinderWidget = codeFinderWidget;
    if (codeFinderWidget) {
        QObject::connect(codeFinderWidget, SIGNAL(findRequest(QString,int)),
                         this, SLOT(_q_onFindCodeRequest(QString,int)));
    }
    if (d->findInScriptAction) {
        d->findInScriptAction->setEnabled(
            (codeFinderWidget != 0)
            && (d->codeWidget != 0)
            && (d->codeWidget->currentView() != 0));
    }
}

QScriptDebugOutputWidgetInterface *QScriptDebugger::debugOutputWidget() const
{
    Q_D(const QScriptDebugger);
    return d->debugOutputWidget;
}

void QScriptDebugger::setDebugOutputWidget(QScriptDebugOutputWidgetInterface *debugOutputWidget)
{
    Q_D(QScriptDebugger);
    d->debugOutputWidget = debugOutputWidget;
}

QScriptBreakpointsWidgetInterface *QScriptDebugger::breakpointsWidget() const
{
    Q_D(const QScriptDebugger);
    return d->breakpointsWidget;
}

void QScriptDebugger::setBreakpointsWidget(QScriptBreakpointsWidgetInterface *breakpointsWidget)
{
    Q_D(QScriptDebugger);
    if (d->breakpointsWidget) {
        // ### invalidate
    }
    d->breakpointsWidget = breakpointsWidget;
    if (breakpointsWidget) {
        if (!d->breakpointsModel) {
            d->breakpointsModel = new QScriptBreakpointsModel(d, d, this);
            if (d->interactive)
                d->scheduleJob(new SyncBreakpointsJob(d));
        }
        d->breakpointsWidget->setBreakpointsModel(d->breakpointsModel);
        d->breakpointsWidget->setScriptsModel(d->scriptsModel);
    }
}

QScriptErrorLogWidgetInterface *QScriptDebugger::errorLogWidget() const
{
    Q_D(const QScriptDebugger);
    return d->errorLogWidget;
}

void QScriptDebugger::setErrorLogWidget(QScriptErrorLogWidgetInterface *errorLogWidget)
{
    Q_D(QScriptDebugger);
    d->errorLogWidget = errorLogWidget;
}

QScriptDebuggerWidgetFactoryInterface *QScriptDebugger::widgetFactory() const
{
    Q_D(const QScriptDebugger);
    return d->widgetFactory;
}

void QScriptDebugger::setWidgetFactory(QScriptDebuggerWidgetFactoryInterface *factory)
{
    Q_D(QScriptDebugger);
    d->widgetFactory = factory;
}

QAction *QScriptDebugger::interruptAction(QObject *parent) const
{
    Q_D(const QScriptDebugger);
    if (!d->interruptAction) {
        QIcon interruptIcon;
        interruptIcon.addPixmap(d->pixmap(QString::fromLatin1("interrupt.png")), QIcon::Normal);
        interruptIcon.addPixmap(d->pixmap(QString::fromLatin1("d_interrupt.png")), QIcon::Disabled);
        QScriptDebugger *that = const_cast<QScriptDebugger*>(this);
        that->d_func()->interruptAction = new QAction(interruptIcon, QObject::tr("Interrupt"), parent);
        d->interruptAction->setEnabled(!d->interactive);
        d->interruptAction->setShortcut(QObject::tr("Shift+F5"));
        QObject::connect(d->interruptAction, SIGNAL(triggered()),
                         that, SLOT(_q_interrupt()));
    }
    return d->interruptAction;
}

QAction *QScriptDebugger::continueAction(QObject *parent) const
{
    Q_D(const QScriptDebugger);
    if (!d->continueAction) {
        QIcon continueIcon;
        continueIcon.addPixmap(d->pixmap(QString::fromLatin1("play.png")), QIcon::Normal);
        continueIcon.addPixmap(d->pixmap(QString::fromLatin1("d_play.png")), QIcon::Disabled);
        QScriptDebugger *that = const_cast<QScriptDebugger*>(this);
        that->d_func()->continueAction = new QAction(continueIcon, QObject::tr("Continue"), parent);
        d->continueAction->setEnabled(d->interactive);
        d->continueAction->setShortcut(QObject::tr("F5"));
        QObject::connect(d->continueAction, SIGNAL(triggered()),
                         that, SLOT(_q_continue()));
    }
    return d->continueAction;
}

QAction *QScriptDebugger::stepIntoAction(QObject *parent) const
{
    Q_D(const QScriptDebugger);
    if (!d->stepIntoAction) {
        QIcon stepIntoIcon;
        stepIntoIcon.addPixmap(d->pixmap(QString::fromLatin1("stepinto.png")), QIcon::Normal);
        stepIntoIcon.addPixmap(d->pixmap(QString::fromLatin1("d_stepinto.png")), QIcon::Disabled);
        QScriptDebugger *that = const_cast<QScriptDebugger*>(this);
        that->d_func()->stepIntoAction = new QAction(stepIntoIcon, QObject::tr("Step Into"), parent);
        d->stepIntoAction->setEnabled(d->interactive);
        d->stepIntoAction->setShortcut(QObject::tr("F11"));
        QObject::connect(d->stepIntoAction, SIGNAL(triggered()),
                         that, SLOT(_q_stepInto()));
    }
    return d->stepIntoAction;
}

QAction *QScriptDebugger::stepOverAction(QObject *parent) const
{
    Q_D(const QScriptDebugger);
    if (!d->stepOverAction) {
        QIcon stepOverIcon;
        stepOverIcon.addPixmap(d->pixmap(QString::fromLatin1("stepover.png")), QIcon::Normal);
        stepOverIcon.addPixmap(d->pixmap(QString::fromLatin1("d_stepover.png")), QIcon::Disabled);
        QScriptDebugger *that = const_cast<QScriptDebugger*>(this);
        that->d_func()->stepOverAction = new QAction(stepOverIcon, QObject::tr("Step Over"), parent);
        d->stepOverAction->setEnabled(d->interactive);
        d->stepOverAction->setShortcut(QObject::tr("F10"));
        QObject::connect(d->stepOverAction, SIGNAL(triggered()),
                         that, SLOT(_q_stepOver()));
    }
    return d->stepOverAction;
}

QAction *QScriptDebugger::stepOutAction(QObject *parent) const
{
    Q_D(const QScriptDebugger);
    if (!d->stepOutAction) {
        QIcon stepOutIcon;
        stepOutIcon.addPixmap(d->pixmap(QString::fromLatin1("stepout.png")), QIcon::Normal);
        stepOutIcon.addPixmap(d->pixmap(QString::fromLatin1("d_stepout.png")), QIcon::Disabled);
        QScriptDebugger *that = const_cast<QScriptDebugger*>(this);
        that->d_func()->stepOutAction = new QAction(stepOutIcon, QObject::tr("Step Out"), parent);
        d->stepOutAction->setEnabled(d->interactive);
        d->stepOutAction->setShortcut(QObject::tr("Shift+F11"));
        QObject::connect(d->stepOutAction, SIGNAL(triggered()),
                         that, SLOT(_q_stepOut()));
    }
    return d->stepOutAction;
}

QAction *QScriptDebugger::runToCursorAction(QObject *parent) const
{
    Q_D(const QScriptDebugger);
    if (!d->runToCursorAction) {
        QIcon runToCursorIcon;
        runToCursorIcon.addPixmap(d->pixmap(QString::fromLatin1("runtocursor.png")), QIcon::Normal);
        runToCursorIcon.addPixmap(d->pixmap(QString::fromLatin1("d_runtocursor.png")), QIcon::Disabled);
        QScriptDebugger *that = const_cast<QScriptDebugger*>(this);
        that->d_func()->runToCursorAction = new QAction(runToCursorIcon, QObject::tr("Run to Cursor"), parent);
        d->runToCursorAction->setEnabled(d->interactive);
        d->runToCursorAction->setShortcut(QObject::tr("Ctrl+F10"));
        QObject::connect(d->runToCursorAction, SIGNAL(triggered()),
                         that, SLOT(_q_runToCursor()));
    }
    return d->runToCursorAction;
}

QAction *QScriptDebugger::runToNewScriptAction(QObject *parent) const
{
    Q_D(const QScriptDebugger);
    if (!d->runToNewScriptAction) {
        QIcon runToNewScriptIcon;
        runToNewScriptIcon.addPixmap(d->pixmap(QString::fromLatin1("runtonewscript.png")), QIcon::Normal);
        runToNewScriptIcon.addPixmap(d->pixmap(QString::fromLatin1("d_breakonscriptload.png")), QIcon::Disabled);
        QScriptDebugger *that = const_cast<QScriptDebugger*>(this);
        that->d_func()->runToNewScriptAction = new QAction(runToNewScriptIcon,
                                                           QObject::tr("Run to New Script"), parent);
        d->runToNewScriptAction->setEnabled(d->interactive);
        QObject::connect(d->runToNewScriptAction, SIGNAL(triggered()),
                         that, SLOT(_q_runToNewScript()));
    }
    return d->runToNewScriptAction;
}

QAction *QScriptDebugger::toggleBreakpointAction(QObject *parent) const
{
    Q_D(const QScriptDebugger);
    if (!d->toggleBreakpointAction) {
        QIcon toggleBreakpointIcon;
        QScriptDebugger *that = const_cast<QScriptDebugger*>(this);
        that->d_func()->toggleBreakpointAction = new QAction(toggleBreakpointIcon,
                                                             QObject::tr("Toggle Breakpoint"), parent);
        d->toggleBreakpointAction->setShortcut(QObject::tr("F9"));
        d->toggleBreakpointAction->setEnabled((d->codeWidget != 0) && (d->codeWidget->currentView() != 0));
        QObject::connect(d->toggleBreakpointAction, SIGNAL(triggered()),
                         that, SLOT(_q_toggleBreakpoint()));
    }
    return d->toggleBreakpointAction;
}

QAction *QScriptDebugger::clearDebugOutputAction(QObject *parent) const
{
    Q_D(const QScriptDebugger);
    if (!d->clearDebugOutputAction) {
        QIcon clearDebugOutputIcon;
        QScriptDebugger *that = const_cast<QScriptDebugger*>(this);
        that->d_func()->clearDebugOutputAction = new QAction(clearDebugOutputIcon, QObject::tr("Clear Debug Output"), parent);
        QObject::connect(d->clearDebugOutputAction, SIGNAL(triggered()),
                         that, SLOT(_q_clearDebugOutput()));
    }
    return d->clearDebugOutputAction;
}

QAction *QScriptDebugger::clearErrorLogAction(QObject *parent) const
{
    Q_D(const QScriptDebugger);
    if (!d->clearErrorLogAction) {
        QIcon clearErrorLogIcon;
        QScriptDebugger *that = const_cast<QScriptDebugger*>(this);
        that->d_func()->clearErrorLogAction = new QAction(clearErrorLogIcon, QObject::tr("Clear Error Log"), parent);
        QObject::connect(d->clearErrorLogAction, SIGNAL(triggered()),
                         that, SLOT(_q_clearErrorLog()));
    }
    return d->clearErrorLogAction;
}

QAction *QScriptDebugger::clearConsoleAction(QObject *parent) const
{
    Q_D(const QScriptDebugger);
    if (!d->clearConsoleAction) {
        QIcon clearConsoleIcon;
        QScriptDebugger *that = const_cast<QScriptDebugger*>(this);
        that->d_func()->clearConsoleAction = new QAction(clearConsoleIcon, QObject::tr("Clear Console"), parent);
        QObject::connect(d->clearConsoleAction, SIGNAL(triggered()),
                         that, SLOT(_q_clearConsole()));
    }
    return d->clearConsoleAction;
}

QAction *QScriptDebugger::findInScriptAction(QObject *parent) const
{
    Q_D(const QScriptDebugger);
    if (!d->findInScriptAction) {
        QIcon findInScriptIcon;
        findInScriptIcon.addPixmap(d->pixmap(QString::fromLatin1("find.png")), QIcon::Normal);
        QScriptDebugger *that = const_cast<QScriptDebugger*>(this);
        that->d_func()->findInScriptAction = new QAction(findInScriptIcon, QObject::tr("&Find in Script..."), parent);
        d->findInScriptAction->setShortcut(QObject::tr("Ctrl+F"));
        d->findInScriptAction->setEnabled(
            (d->codeFinderWidget != 0)
            && (d->codeWidget != 0)
            && (d->codeWidget->currentView() != 0));
        QObject::connect(d->findInScriptAction, SIGNAL(triggered()),
                         that, SLOT(_q_findInScript()));
    }
    return d->findInScriptAction;
}

QAction *QScriptDebugger::findNextInScriptAction(QObject *parent) const
{
    Q_D(const QScriptDebugger);
    if (!d->findNextInScriptAction) {
        QIcon findNextInScriptIcon;
        QScriptDebugger *that = const_cast<QScriptDebugger*>(this);
        that->d_func()->findNextInScriptAction = new QAction(findNextInScriptIcon, QObject::tr("Find &Next"), parent);
        d->findNextInScriptAction->setEnabled(d->codeFinderWidget && !d->codeFinderWidget->text().isEmpty());
        d->findNextInScriptAction->setShortcut(QObject::tr("F3"));
        QObject::connect(d->findNextInScriptAction, SIGNAL(triggered()),
                         that, SLOT(_q_findNextInScript()));
    }
    return d->findNextInScriptAction;
}

QAction *QScriptDebugger::findPreviousInScriptAction(QObject *parent) const
{
    Q_D(const QScriptDebugger);
    if (!d->findPreviousInScriptAction) {
        QIcon findPreviousInScriptIcon;
        QScriptDebugger *that = const_cast<QScriptDebugger*>(this);
        that->d_func()->findPreviousInScriptAction = new QAction(findPreviousInScriptIcon, QObject::tr("Find &Previous"), parent);
        d->findPreviousInScriptAction->setEnabled(d->codeFinderWidget && !d->codeFinderWidget->text().isEmpty());
        d->findPreviousInScriptAction->setShortcut(QObject::tr("Shift+F3"));
        QObject::connect(d->findPreviousInScriptAction, SIGNAL(triggered()),
                         that, SLOT(_q_findPreviousInScript()));
    }
    return d->findPreviousInScriptAction;
}

QAction *QScriptDebugger::goToLineAction(QObject *parent) const
{
    Q_D(const QScriptDebugger);
    if (!d->goToLineAction) {
        QIcon goToLineIcon;
        QScriptDebugger *that = const_cast<QScriptDebugger*>(this);
        that->d_func()->goToLineAction = new QAction(goToLineIcon, QObject::tr("Go to Line"), parent);
        d->goToLineAction->setShortcut(QObject::tr("Ctrl+G"));
        d->goToLineAction->setEnabled((d->codeWidget != 0) && (d->codeWidget->currentView() != 0));
        QObject::connect(d->goToLineAction, SIGNAL(triggered()),
                         that, SLOT(_q_goToLine()));
    }
    return d->goToLineAction;
}

/*!
  \reimp
*/
bool QScriptDebugger::eventFilter(QObject *watched, QEvent *e)
{
    Q_D(QScriptDebugger);
    if (watched == d->codeWidget) {
        if (e->type() == QEvent::KeyPress) {
            d->_q_findInScript();
            d->codeFinderWidget->setText(static_cast<QKeyEvent*>(e)->text());
            return true;
        }
    }
    return false;
}

/*!
  \reimp
*/
void QScriptDebugger::timerEvent(QTimerEvent *e)
{
    Q_D(QScriptDebugger);
    if (e->timerId() == d->updatesEnabledTimerId) {
        killTimer(d->updatesEnabledTimerId);
        d->updatesEnabledTimerId = -1;
        if (d->stackWidget)
            d->stackWidget->setUpdatesEnabled(true);
        if (d->localsWidget)
            d->localsWidget->setUpdatesEnabled(true);
    } else {
        QObject::timerEvent(e);
    }
}

QT_END_NAMESPACE

#include "moc_qscriptdebugger_p.cpp"
