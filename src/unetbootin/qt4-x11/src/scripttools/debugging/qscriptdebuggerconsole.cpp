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

#include "qscriptdebuggerconsole_p.h"
#include "qscriptdebuggerconsolecommandjob_p.h"
#include "qscriptdebuggerconsolecommandmanager_p.h"
#include "qscriptdebuggerscriptedconsolecommand_p.h"
#include "qscriptmessagehandlerinterface_p.h"

#include <QtCore/qdir.h>
#include <QtCore/qfileinfo.h>
#include <QtCore/qstring.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qdebug.h>
#include <QtScript/qscriptengine.h>

QT_BEGIN_NAMESPACE

/*!
  \since 4.5
  \class QScriptDebuggerConsole
  \internal

  \brief The QScriptDebuggerConsole class provides the core functionality of a debugger console.
*/

class QScriptDebuggerConsolePrivate
{
    Q_DECLARE_PUBLIC(QScriptDebuggerConsole)
public:
    QScriptDebuggerConsolePrivate();
    ~QScriptDebuggerConsolePrivate();

    void loadScriptedCommands(const QString &scriptsPath,
                              QScriptMessageHandlerInterface *messageHandler);
    QScriptDebuggerConsoleCommandJob *createJob(
        const QString &command,
        QScriptMessageHandlerInterface *messageHandler,
        QScriptDebuggerCommandSchedulerInterface *commandScheduler);

    QScriptDebuggerConsoleCommandManager *commandManager;
    QString commandPrefix;
    QString input;
    QStringList commandHistory;
    int currentFrameIndex;
    qint64 currentScriptId;
    int currentLineNumber;
    int evaluateAction;
    qint64 sessionId;

    QScriptDebuggerConsole *q_ptr;
};

QScriptDebuggerConsolePrivate::QScriptDebuggerConsolePrivate()
{
    sessionId = 0;
    currentFrameIndex = 0;
    currentScriptId = -1;
    currentLineNumber = -1;
    evaluateAction = 0;
    commandPrefix = QLatin1String(".");
    commandManager = new QScriptDebuggerConsoleCommandManager();
}

QScriptDebuggerConsolePrivate::~QScriptDebuggerConsolePrivate()
{
    delete commandManager;
}

/*!
  Loads command definitions from scripts located in the given \a scriptsPath.
*/
void QScriptDebuggerConsolePrivate::loadScriptedCommands(
    const QString &scriptsPath,
    QScriptMessageHandlerInterface *messageHandler)
{
    QDir dir(scriptsPath);
    QFileInfoList entries = dir.entryInfoList(QStringList()
                                              << QLatin1String("*.qs"));
    for (int i = 0; i < entries.size(); ++i) {
        const QFileInfo &fi = entries.at(i);
        QString fileName = fi.fileName();
        QFile file(scriptsPath + QLatin1Char('/') + fileName);
        if (!file.open(QIODevice::ReadOnly))
            continue;
        QTextStream stream(&file);
        QString program = stream.readAll();
        QScriptDebuggerScriptedConsoleCommand *command;
        command = QScriptDebuggerScriptedConsoleCommand::parse(
            program, fileName, messageHandler);
        if (!command)
            continue;
        commandManager->addCommand(command);
    }
}


/*!
  Creates a job that will execute the given debugger \a command.
  Returns the new job, or 0 if the command is undefined.
*/
QScriptDebuggerConsoleCommandJob *QScriptDebuggerConsolePrivate::createJob(
    const QString &command, QScriptMessageHandlerInterface *messageHandler,
    QScriptDebuggerCommandSchedulerInterface *commandScheduler)
{
    QString name;
    int i = command.indexOf(QLatin1Char(' '));
    if (i == -1) {
        name = command;
        i = name.size();
    } else {
        name = command.left(i);
    }
    if (name.isEmpty())
        return 0;
    QScriptDebuggerConsoleCommand *cmd = commandManager->findCommand(name);
    if (!cmd) {
        // try to auto-complete
        QStringList completions = commandManager->completions(name);
        if (!completions.isEmpty()) {
            if (completions.size() > 1) {
                QString msg;
                msg.append(QString::fromLatin1("Ambiguous command \"%0\": ")
                           .arg(name));
                for (int j = 0; j < completions.size(); ++j) {
                    if (j > 0)
                        msg.append(QString::fromLatin1(", "));
                    msg.append(completions.at(j));
                }
                msg.append(QString::fromLatin1("."));
                messageHandler->message(QtWarningMsg, msg);
                return 0;
            }
            cmd = commandManager->findCommand(completions.at(0));
            Q_ASSERT(cmd != 0);
        }
        if (!cmd) {
            messageHandler->message(
                QtWarningMsg,
                QString::fromLatin1("Undefined command \"%0\". Try \"help\".")
                .arg(name));
            return 0;
        }
    }
    QStringList args;
    QString tmp = command.mid(i+1);
    if (cmd->argumentTypes().contains(QString::fromLatin1("script"))) {
        if (!tmp.isEmpty())
            args.append(tmp);
    } else {
        args = tmp.split(QLatin1Char(' '), QString::SkipEmptyParts);
    }
    return cmd->createJob(args, q_func(), messageHandler, commandScheduler);
}

QScriptDebuggerConsole::QScriptDebuggerConsole()
    : d_ptr(new QScriptDebuggerConsolePrivate)
{
    d_ptr->q_ptr = this;
}

QScriptDebuggerConsole::~QScriptDebuggerConsole()
{
    delete d_ptr;
}

void QScriptDebuggerConsole::loadScriptedCommands(const QString &scriptsPath,
                                                  QScriptMessageHandlerInterface *messageHandler)
{
    Q_D(QScriptDebuggerConsole);
    d->loadScriptedCommands(scriptsPath, messageHandler);
}

QScriptDebuggerConsoleCommandManager *QScriptDebuggerConsole::commandManager() const
{
    Q_D(const QScriptDebuggerConsole);
    return d->commandManager;
}

bool QScriptDebuggerConsole::hasIncompleteInput() const
{
    Q_D(const QScriptDebuggerConsole);
    return !d->input.isEmpty();
}

QString QScriptDebuggerConsole::incompleteInput() const
{
    Q_D(const QScriptDebuggerConsole);
    return d->input;
}

void QScriptDebuggerConsole::setIncompleteInput(const QString &input)
{
    Q_D(QScriptDebuggerConsole);
    d->input = input;
}

QString QScriptDebuggerConsole::commandPrefix() const
{
    Q_D(const QScriptDebuggerConsole);
    return d->commandPrefix;
}

/*!
  Consumes the given line of \a input.  If the input starts with the
  command prefix, it is regarded as a debugger command; otherwise the
  input is evaluated as a plain script.
*/
QScriptDebuggerConsoleCommandJob *QScriptDebuggerConsole::consumeInput(
    const QString &input, QScriptMessageHandlerInterface *messageHandler,
    QScriptDebuggerCommandSchedulerInterface *commandScheduler)
{
    Q_D(QScriptDebuggerConsole);
    static const int maximumHistoryCount = 100;
    QString cmd;
    if (d->input.isEmpty() && input.isEmpty()) {
        if (d->commandHistory.isEmpty())
            return 0;
        cmd = d->commandHistory.first();
    } else {
        cmd = input;
    }
    if (d->input.isEmpty() && cmd.startsWith(d->commandPrefix)) {
        if (!input.isEmpty()) {
            d->commandHistory.prepend(cmd);
            if (d->commandHistory.size() > maximumHistoryCount)
                d->commandHistory.removeLast();
        }
        cmd.remove(0, d->commandPrefix.length());
        return d->createJob(cmd, messageHandler, commandScheduler);
    }
    d->input += cmd;
    d->input += QLatin1Char('\n');
    QScriptSyntaxCheckResult check = QScriptEngine::checkSyntax(d->input);
    if (check.state() == QScriptSyntaxCheckResult::Intermediate)
        return false;
    d->input.chop(1); // remove the last \n
    cmd = QString();
    cmd.append(d->commandPrefix);
    cmd.append(QString::fromLatin1("eval "));
    cmd.append(d->input);
    d->commandHistory.prepend(cmd);
    if (d->commandHistory.size() > maximumHistoryCount)
        d->commandHistory.removeLast();
    d->input.clear();
    cmd.remove(0, d->commandPrefix.length());
    return d->createJob(cmd, messageHandler, commandScheduler);
}

int QScriptDebuggerConsole::currentFrameIndex() const
{
    Q_D(const QScriptDebuggerConsole);
    return d->currentFrameIndex;
}

void QScriptDebuggerConsole::setCurrentFrameIndex(int index)
{
    Q_D(QScriptDebuggerConsole);
    d->currentFrameIndex = index;
}

qint64 QScriptDebuggerConsole::currentScriptId() const
{
    Q_D(const QScriptDebuggerConsole);
    return d->currentScriptId;
}

void QScriptDebuggerConsole::setCurrentScriptId(qint64 id)
{
    Q_D(QScriptDebuggerConsole);
    d->currentScriptId = id;
}

int QScriptDebuggerConsole::currentLineNumber() const
{
    Q_D(const QScriptDebuggerConsole);
    return d->currentLineNumber;
}

void QScriptDebuggerConsole::setCurrentLineNumber(int lineNumber)
{
    Q_D(QScriptDebuggerConsole);
    d->currentLineNumber = lineNumber;
}

int QScriptDebuggerConsole::evaluateAction() const
{
    Q_D(const QScriptDebuggerConsole);
    return d->evaluateAction;
}

void QScriptDebuggerConsole::setEvaluateAction(int action)
{
    Q_D(QScriptDebuggerConsole);
    d->evaluateAction = action;
}

qint64 QScriptDebuggerConsole::sessionId() const
{
    Q_D(const QScriptDebuggerConsole);
    return d->sessionId;
}

void QScriptDebuggerConsole::bumpSessionId()
{
    Q_D(QScriptDebuggerConsole);
    ++d->sessionId;
}

void QScriptDebuggerConsole::showDebuggerInfoMessage(
    QScriptMessageHandlerInterface *messageHandler)
{
    messageHandler->message(
        QtDebugMsg,
        QString::fromLatin1(
            "Welcome to the Qt Script debugger.\n"
            "Debugger commands start with a . (period).\n"
            "Any other input will be evaluated by the script interpreter.\n"
            "Type \".help\" for help.\n"));
}

/*!
  \reimp
*/
int QScriptDebuggerConsole::historyCount() const
{
    Q_D(const QScriptDebuggerConsole);
    return d->commandHistory.size();
}

/*!
  \reimp
*/
QString QScriptDebuggerConsole::historyAt(int index) const
{
    Q_D(const QScriptDebuggerConsole);
    return d->commandHistory.value(index);
}

/*!
  \reimp
*/
void QScriptDebuggerConsole::changeHistoryAt(int index, const QString &newHistory)
{
    Q_D(QScriptDebuggerConsole);
    d->commandHistory[index] = newHistory;
}

QT_END_NAMESPACE
