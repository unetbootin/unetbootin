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

#include "qscriptdebuggerscriptedconsolecommand_p.h"
#include "qscriptdebuggerconsolecommand_p_p.h"
#include "qscriptdebuggerconsolecommandjob_p.h"
#include "qscriptdebuggerconsolecommandjob_p_p.h"
#include "qscriptmessagehandlerinterface_p.h"
#include "qscriptdebuggerconsoleglobalobject_p.h"
#include "qscriptdebuggerresponse_p.h"
#include "qscriptdebuggervalue_p.h"
#include "qscriptdebuggervalueproperty_p.h"
#include "qscriptdebuggercommandschedulerinterface_p.h"

#include <QtCore/qstring.h>
#include <QtCore/qstringlist.h>
#include <QtScript/qscriptengine.h>
#include <QtScript/qscriptvalue.h>
#include <QtScript/qscriptvalueiterator.h>
#include <QtScript/qscriptcontextinfo.h>
#include <QtCore/qdebug.h>

Q_DECLARE_METATYPE(QScriptDebuggerResponse)
Q_DECLARE_METATYPE(QScriptBreakpointData)
Q_DECLARE_METATYPE(QScriptBreakpointMap)
Q_DECLARE_METATYPE(QScriptScriptData)
Q_DECLARE_METATYPE(QScriptScriptMap)
Q_DECLARE_METATYPE(QScriptContextInfo)
Q_DECLARE_METATYPE(QScriptDebuggerValue)
Q_DECLARE_METATYPE(QScriptDebuggerValueProperty)
Q_DECLARE_METATYPE(QScriptDebuggerValuePropertyList)
Q_DECLARE_METATYPE(QScriptDebuggerConsoleCommand*)
Q_DECLARE_METATYPE(QScriptDebuggerConsoleCommandList)
Q_DECLARE_METATYPE(QScriptDebuggerConsoleCommandGroupData)
Q_DECLARE_METATYPE(QScriptDebuggerConsoleCommandGroupMap)

QT_BEGIN_NAMESPACE

static QScriptValue debuggerResponseToScriptValue(QScriptEngine *eng, const QScriptDebuggerResponse &in)
{
    QScriptValue out = eng->newObject();
    out.setProperty(QString::fromLatin1("result"), qScriptValueFromValue(eng, in.result()));
    out.setProperty(QString::fromLatin1("error"), QScriptValue(eng, in.error()));
    out.setProperty(QString::fromLatin1("async"), QScriptValue(eng, in.async()));
    return out;
}

static void debuggerResponseFromScriptValue(const QScriptValue &, QScriptDebuggerResponse &)
{
    Q_ASSERT(0);
}

static QScriptValue breakpointDataToScriptValue(QScriptEngine *eng, const QScriptBreakpointData &in)
{
    QScriptValue out = eng->newObject();
    out.setProperty(QString::fromLatin1("scriptId"), QScriptValue(eng, qsreal(in.scriptId())));
    out.setProperty(QString::fromLatin1("fileName"), QScriptValue(eng, in.fileName()));
    out.setProperty(QString::fromLatin1("lineNumber"), QScriptValue(eng, in.lineNumber()));
    out.setProperty(QString::fromLatin1("enabled"), QScriptValue(eng, in.isEnabled()));
    out.setProperty(QString::fromLatin1("singleShot"), QScriptValue(eng, in.isSingleShot()));
    out.setProperty(QString::fromLatin1("ignoreCount"), QScriptValue(eng, in.ignoreCount()));
    out.setProperty(QString::fromLatin1("condition"), QScriptValue(eng, in.condition()));
    return out;
}

static void breakpointDataFromScriptValue(const QScriptValue &in, QScriptBreakpointData &out)
{
    QScriptValue scriptId = in.property(QString::fromLatin1("scriptId"));
    if (scriptId.isValid())
        out.setScriptId((qint64)scriptId.toNumber());
    out.setFileName(in.property(QString::fromLatin1("fileName")).toString());
    out.setLineNumber(in.property(QString::fromLatin1("lineNumber")).toInt32());
    QScriptValue enabled = in.property(QString::fromLatin1("enabled"));
    if (enabled.isValid())
        out.setEnabled(enabled.toBoolean());
    QScriptValue singleShot = in.property(QString::fromLatin1("singleShot"));
    if (singleShot.isValid())
        out.setSingleShot(singleShot.toBoolean());
    out.setIgnoreCount(in.property(QString::fromLatin1("ignoreCount")).toInt32());
    out.setCondition(in.property(QString::fromLatin1("condition")).toString());
}

static QScriptValue breakpointMapToScriptValue(QScriptEngine *eng, const QScriptBreakpointMap &in)
{
    QScriptValue out = eng->newObject();
    QScriptBreakpointMap::const_iterator it;
    for (it = in.constBegin(); it != in.constEnd(); ++it) {
        out.setProperty(QString::number(it.key()), qScriptValueFromValue(eng, it.value()));
    }
    return out;
}

static void breakpointMapFromScriptValue(const QScriptValue &, QScriptBreakpointMap &)
{
    Q_ASSERT(0);
}

static QScriptValue scriptDataToScriptValue(QScriptEngine *eng, const QScriptScriptData &in)
{
    QScriptValue out = eng->newObject();
    out.setProperty(QString::fromLatin1("contents"), QScriptValue(eng, in.contents()));
    out.setProperty(QString::fromLatin1("fileName"), QScriptValue(eng, in.fileName()));
    out.setProperty(QString::fromLatin1("baseLineNumber"), QScriptValue(eng, in.baseLineNumber()));
    return out;
}

static void scriptDataFromScriptValue(const QScriptValue &in, QScriptScriptData &out)
{
    QString contents = in.property(QString::fromLatin1("contents")).toString();
    QString fileName = in.property(QString::fromLatin1("fileName")).toString();
    int baseLineNumber = in.property(QString::fromLatin1("baseLineNumber")).toInt32();
    QScriptScriptData tmp(contents, fileName, baseLineNumber);
    out = tmp;
}

static QScriptValue scriptMapToScriptValue(QScriptEngine *eng, const QScriptScriptMap &in)
{
    QScriptValue out = eng->newObject();
    QScriptScriptMap::const_iterator it;
    for (it = in.constBegin(); it != in.constEnd(); ++it) {
        out.setProperty(QString::number(it.key()), qScriptValueFromValue(eng, it.value()));
    }
    return out;
}

static void scriptMapFromScriptValue(const QScriptValue &, QScriptScriptMap &)
{
    Q_ASSERT(0);
}

static QScriptValue consoleCommandToScriptValue(
    QScriptEngine *eng, QScriptDebuggerConsoleCommand* const &in)
{
    if (!in)
        return eng->undefinedValue();
    QScriptValue out = eng->newObject();
    out.setProperty(QString::fromLatin1("name"), QScriptValue(eng, in->name()));
    out.setProperty(QString::fromLatin1("group"), QScriptValue(eng, in->group()));
    out.setProperty(QString::fromLatin1("shortDescription"), QScriptValue(eng, in->shortDescription()));
    out.setProperty(QString::fromLatin1("longDescription"), QScriptValue(eng, in->longDescription()));
    out.setProperty(QString::fromLatin1("aliases"), qScriptValueFromValue(eng, in->aliases()));
    out.setProperty(QString::fromLatin1("seeAlso"), qScriptValueFromValue(eng, in->seeAlso()));
    return out;
}

static void consoleCommandFromScriptValue(
    const QScriptValue &, QScriptDebuggerConsoleCommand* &)
{
    Q_ASSERT(0);
}

static QScriptValue consoleCommandGroupDataToScriptValue(
    QScriptEngine *eng, const QScriptDebuggerConsoleCommandGroupData &in)
{
    QScriptValue out = eng->newObject();
    out.setProperty(QString::fromLatin1("longDescription"), QScriptValue(eng, in.longDescription()));
    out.setProperty(QString::fromLatin1("shortDescription"), QScriptValue(eng, in.shortDescription()));
    return out;
}

static void consoleCommandGroupDataFromScriptValue(
    const QScriptValue &, QScriptDebuggerConsoleCommandGroupData &)
{
    Q_ASSERT(0);
}

static QScriptValue consoleCommandGroupMapToScriptValue(
    QScriptEngine *eng, const QScriptDebuggerConsoleCommandGroupMap &in)
{
    QScriptValue out = eng->newObject();
    QScriptDebuggerConsoleCommandGroupMap::const_iterator it;
    for (it = in.constBegin(); it != in.constEnd(); ++it) {
        out.setProperty(it.key(), qScriptValueFromValue(eng, it.value()));
    }
    return out;
}

static void consoleCommandGroupMapFromScriptValue(
    const QScriptValue &, QScriptDebuggerConsoleCommandGroupMap &)
{
    Q_ASSERT(0);
}

static QScriptValue contextInfoToScriptValue(QScriptEngine *eng, const QScriptContextInfo &in)
{
    QScriptValue out = eng->newObject();
    out.setProperty(QString::fromLatin1("scriptId"), QScriptValue(eng, qsreal(in.scriptId())));
    out.setProperty(QString::fromLatin1("fileName"), QScriptValue(eng, in.fileName()));
    out.setProperty(QString::fromLatin1("lineNumber"), QScriptValue(eng, in.lineNumber()));
    out.setProperty(QString::fromLatin1("columnNumber"), QScriptValue(eng, in.columnNumber()));
    out.setProperty(QString::fromLatin1("functionName"), QScriptValue(eng, in.functionName()));
    return out;
}

static void contextInfoFromScriptValue(const QScriptValue &, QScriptContextInfo &)
{
    Q_ASSERT(0);
}

static QScriptValue debuggerScriptValuePropertyToScriptValue(QScriptEngine *eng, const QScriptDebuggerValueProperty &in)
{
    QScriptValue out = eng->newObject();
    out.setProperty(QString::fromLatin1("name"), QScriptValue(eng, in.name()));
    out.setProperty(QString::fromLatin1("value"), qScriptValueFromValue(eng, in.value()));
    out.setProperty(QString::fromLatin1("valueAsString"), QScriptValue(eng, in.valueAsString()));
    out.setProperty(QString::fromLatin1("flags"), QScriptValue(eng, static_cast<int>(in.flags())));
    return out;
}

static void debuggerScriptValuePropertyFromScriptValue(const QScriptValue &in, QScriptDebuggerValueProperty &out)
{
    QString name = in.property(QString::fromLatin1("name")).toString();
    QScriptDebuggerValue value = qscriptvalue_cast<QScriptDebuggerValue>(in.property(QString::fromLatin1("value")));
    QString valueAsString = in.property(QString::fromLatin1("valueAsString")).toString();
    int flags = in.property(QString::fromLatin1("flags")).toInt32();
    QScriptDebuggerValueProperty tmp(name, value, valueAsString, QScriptValue::PropertyFlags(flags));
    out = tmp;
}

/*!
  \since 4.5
  \class QScriptDebuggerScriptedConsoleCommand
  \internal

  \brief The QScriptDebuggerScriptedConsoleCommand class encapsulates a command defined in a script.
*/

class QScriptDebuggerScriptedConsoleCommandPrivate
    : public QScriptDebuggerConsoleCommandPrivate
{
    Q_DECLARE_PUBLIC(QScriptDebuggerScriptedConsoleCommand)
public:
    QScriptDebuggerScriptedConsoleCommandPrivate();
    ~QScriptDebuggerScriptedConsoleCommandPrivate();

    QString name;
    QString group;
    QString shortDescription;
    QString longDescription;
    QStringList aliases;
    QStringList seeAlso;
    QStringList argumentTypes;
    QStringList subCommands;
    QScriptEngine *engine;
    QScriptValue execFunction;
    QScriptValue responseFunction;
};

QScriptDebuggerScriptedConsoleCommandPrivate::QScriptDebuggerScriptedConsoleCommandPrivate()
{
    engine = 0;
}

QScriptDebuggerScriptedConsoleCommandPrivate::~QScriptDebuggerScriptedConsoleCommandPrivate()
{
    delete engine;
}

QScriptDebuggerScriptedConsoleCommand::QScriptDebuggerScriptedConsoleCommand(
    const QString &name, const QString &group,
    const QString &shortDescription, const QString &longDescription,
    const QStringList &aliases, const QStringList &seeAlso,
    const QStringList &argumentTypes, const QStringList &subCommands,
    const QScriptValue &execFunction, const QScriptValue &responseFunction)
    : QScriptDebuggerConsoleCommand(*new QScriptDebuggerScriptedConsoleCommandPrivate)
{
    Q_D(QScriptDebuggerScriptedConsoleCommand);
    d->name = name;
    d->group = group;
    d->shortDescription = shortDescription;
    d->longDescription = longDescription;
    d->aliases = aliases;
    d->seeAlso = seeAlso;
    d->argumentTypes = argumentTypes;
    d->subCommands = subCommands;
    d->execFunction = execFunction;
    d->responseFunction = responseFunction;
    d->engine = execFunction.engine();

    qScriptRegisterMetaType<QScriptBreakpointData>(d->engine, breakpointDataToScriptValue, breakpointDataFromScriptValue);
    qScriptRegisterMetaType<QScriptBreakpointMap>(d->engine, breakpointMapToScriptValue, breakpointMapFromScriptValue);
    qScriptRegisterMetaType<QScriptScriptData>(d->engine, scriptDataToScriptValue, scriptDataFromScriptValue);
    qScriptRegisterMetaType<QScriptScriptMap>(d->engine, scriptMapToScriptValue, scriptMapFromScriptValue);
    qScriptRegisterMetaType<QScriptContextInfo>(d->engine, contextInfoToScriptValue, contextInfoFromScriptValue);
    qScriptRegisterMetaType<QScriptDebuggerValueProperty>(d->engine, debuggerScriptValuePropertyToScriptValue, debuggerScriptValuePropertyFromScriptValue);
    qScriptRegisterSequenceMetaType<QScriptDebuggerValuePropertyList>(d->engine);
    qScriptRegisterMetaType<QScriptDebuggerResponse>(d->engine, debuggerResponseToScriptValue, debuggerResponseFromScriptValue);
    qScriptRegisterMetaType<QScriptDebuggerConsoleCommand*>(d->engine, consoleCommandToScriptValue, consoleCommandFromScriptValue);
    qScriptRegisterSequenceMetaType<QScriptDebuggerConsoleCommandList>(d->engine);
    qScriptRegisterMetaType<QScriptDebuggerConsoleCommandGroupData>(d->engine, consoleCommandGroupDataToScriptValue, consoleCommandGroupDataFromScriptValue);
    qScriptRegisterMetaType<QScriptDebuggerConsoleCommandGroupMap>(d->engine, consoleCommandGroupMapToScriptValue, consoleCommandGroupMapFromScriptValue);
// ### can't do this, if it's an object ID the conversion will be incorrect since
// ### the object ID refers to an object in a different engine!
//    qScriptRegisterMetaType(d->engine, debuggerScriptValueToScriptValue, debuggerScriptValueFromScriptValue);
}

QScriptDebuggerScriptedConsoleCommand::~QScriptDebuggerScriptedConsoleCommand()
{
}

class QScriptDebuggerScriptedConsoleCommandJobPrivate;
class QScriptDebuggerScriptedConsoleCommandJob
    : public QScriptDebuggerConsoleCommandJob,
      public QScriptDebuggerCommandSchedulerInterface
{
public:
    QScriptDebuggerScriptedConsoleCommandJob(
        QScriptDebuggerScriptedConsoleCommandPrivate *command,
        const QStringList &arguments,
        QScriptDebuggerConsole *console,
        QScriptMessageHandlerInterface *messageHandler,
        QScriptDebuggerCommandSchedulerInterface *commandScheduler);
    ~QScriptDebuggerScriptedConsoleCommandJob();

    int scheduleCommand(
        const QScriptDebuggerCommand &command,
        QScriptDebuggerResponseHandlerInterface *responseHandler);

    void start();
    void handleResponse(const QScriptDebuggerResponse &response,
                        int commandId);

private:
    Q_DECLARE_PRIVATE(QScriptDebuggerScriptedConsoleCommandJob)
    Q_DISABLE_COPY(QScriptDebuggerScriptedConsoleCommandJob)
};

class QScriptDebuggerScriptedConsoleCommandJobPrivate
    : public QScriptDebuggerConsoleCommandJobPrivate
{
public:
    QScriptDebuggerScriptedConsoleCommandJobPrivate() {}
    ~QScriptDebuggerScriptedConsoleCommandJobPrivate() {}

    QScriptDebuggerScriptedConsoleCommandPrivate *command;
    QStringList arguments;
    int commandCount;
};

QScriptDebuggerScriptedConsoleCommandJob::QScriptDebuggerScriptedConsoleCommandJob(
    QScriptDebuggerScriptedConsoleCommandPrivate *command,
    const QStringList &arguments,
    QScriptDebuggerConsole *console,
    QScriptMessageHandlerInterface *messageHandler,
    QScriptDebuggerCommandSchedulerInterface *commandScheduler)
    : QScriptDebuggerConsoleCommandJob(*new QScriptDebuggerScriptedConsoleCommandJobPrivate,
                                       console, messageHandler, commandScheduler)
{
    Q_D(QScriptDebuggerScriptedConsoleCommandJob);
    d->command = command;
    d->arguments = arguments;
}

QScriptDebuggerScriptedConsoleCommandJob::~QScriptDebuggerScriptedConsoleCommandJob()
{
}

int QScriptDebuggerScriptedConsoleCommandJob::scheduleCommand(
    const QScriptDebuggerCommand &command,
    QScriptDebuggerResponseHandlerInterface *responseHandler)
{
    Q_D(QScriptDebuggerScriptedConsoleCommandJob);
    ++d->commandCount;
    return commandScheduler()->scheduleCommand(command, responseHandler);
}

void QScriptDebuggerScriptedConsoleCommandJob::start()
{
    Q_D(QScriptDebuggerScriptedConsoleCommandJob);
    QScriptEngine *engine = d->command->engine;
    QScriptValueList args;
    for (int i = 0; i < d->arguments.size(); ++i)
        args.append(QScriptValue(engine, d->arguments.at(i)));
    QScriptDebuggerConsoleGlobalObject *global;
#if QT_VERSION >= 0x040500
    global = qobject_cast<QScriptDebuggerConsoleGlobalObject*>(engine->globalObject().toQObject());
#else
    global = qobject_cast<QScriptDebuggerConsoleGlobalObject*>(engine->globalObject().scope().toQObject());
#endif
    Q_ASSERT(global != 0);
    global->setScheduler(this);
    global->setResponseHandler(this);
    global->setMessageHandler(d->messageHandler);
    global->setConsole(d->console);
    d->commandCount = 0;
    QScriptValue ret = d->command->execFunction.call(QScriptValue(), args);
    global->setScheduler(0);
    global->setResponseHandler(0);
    global->setMessageHandler(0);
    global->setConsole(0);
    if (ret.isError()) {
        qWarning("*** internal error: %s", qPrintable(ret.toString()));
    }
    if (d->commandCount == 0)
        finish();
}

void QScriptDebuggerScriptedConsoleCommandJob::handleResponse(
    const QScriptDebuggerResponse &response,
    int commandId)
{
    Q_D(QScriptDebuggerScriptedConsoleCommandJob);
    // ### generalize
    QScriptEngine *engine = d->command->engine;
    QScriptValueList args;
    args.append(qScriptValueFromValue(engine, response));
    args.append(QScriptValue(engine, commandId));
    QScriptDebuggerConsoleGlobalObject *global;
#if QT_VERSION >= 0x040500
    global = qobject_cast<QScriptDebuggerConsoleGlobalObject*>(engine->globalObject().toQObject());
#else
    global = qobject_cast<QScriptDebuggerConsoleGlobalObject*>(engine->globalObject().scope().toQObject());
#endif
    Q_ASSERT(global != 0);
    global->setScheduler(this);
    global->setResponseHandler(this);
    global->setMessageHandler(d->messageHandler);
    global->setConsole(d->console);
    d->commandCount = 0;
    QScriptValue ret = d->command->responseFunction.call(QScriptValue(), args);
    global->setScheduler(0);
    global->setResponseHandler(0);
    global->setMessageHandler(0);
    global->setConsole(0);
    if (ret.isError()) {
        qWarning("*** internal error: %s", qPrintable(ret.toString()));
    }
    if (d->commandCount == 0)
        finish();
}

/*!
  \reimp
*/
QString QScriptDebuggerScriptedConsoleCommand::name() const
{
    Q_D(const QScriptDebuggerScriptedConsoleCommand);
    return d->name;
}

/*!
  \reimp
*/
QString QScriptDebuggerScriptedConsoleCommand::group() const
{
    Q_D(const QScriptDebuggerScriptedConsoleCommand);
    return d->group;
}

/*!
  \reimp
*/
QString QScriptDebuggerScriptedConsoleCommand::shortDescription() const
{
    Q_D(const QScriptDebuggerScriptedConsoleCommand);
    return d->shortDescription;
}

/*!
  \reimp
*/
QString QScriptDebuggerScriptedConsoleCommand::longDescription() const
{
    Q_D(const QScriptDebuggerScriptedConsoleCommand);
    return d->longDescription;
}

/*!
  \reimp
*/
QStringList QScriptDebuggerScriptedConsoleCommand::aliases() const
{
    Q_D(const QScriptDebuggerScriptedConsoleCommand);
    return d->aliases;
}

/*!
  \reimp
*/
QStringList QScriptDebuggerScriptedConsoleCommand::seeAlso() const
{
    Q_D(const QScriptDebuggerScriptedConsoleCommand);
    return d->seeAlso;
}

/*!
  \reimp
*/
QStringList QScriptDebuggerScriptedConsoleCommand::argumentTypes() const
{
    Q_D(const QScriptDebuggerScriptedConsoleCommand);
    return d->argumentTypes;
}

/*!
  \reimp
*/
QStringList QScriptDebuggerScriptedConsoleCommand::subCommands() const
{
    Q_D(const QScriptDebuggerScriptedConsoleCommand);
    return d->subCommands;
}

/*!
  \reimp
*/
QScriptDebuggerConsoleCommandJob *QScriptDebuggerScriptedConsoleCommand::createJob(
    const QStringList &arguments,
    QScriptDebuggerConsole *console,
    QScriptMessageHandlerInterface *messageHandler,
    QScriptDebuggerCommandSchedulerInterface *commandScheduler)
{
    Q_D(QScriptDebuggerScriptedConsoleCommand);
    return new QScriptDebuggerScriptedConsoleCommandJob(
        d, arguments, console, messageHandler, commandScheduler);
}

/*!
  Parses a command defined by the given \a program.
  Returns an object that encapsulates the command, or 0 if parsing failed.
*/
QScriptDebuggerScriptedConsoleCommand *QScriptDebuggerScriptedConsoleCommand::parse(
    const QString &program, const QString &fileName,
    QScriptMessageHandlerInterface *messageHandler)
{
    QScriptEngine *engine = new QScriptEngine();
    // create a custom global object
    QScriptDebuggerConsoleGlobalObject *cppGlobal = new QScriptDebuggerConsoleGlobalObject();
    QScriptValue global = engine->newQObject(cppGlobal,
                                             QScriptEngine::ScriptOwnership,
#if QT_VERSION >= 0x040500
                                             QScriptEngine::ExcludeSuperClassContents);
#else
                                             QScriptEngine::ExcludeSuperClassMethods
                                             | QScriptEngine::ExcludeSuperClassProperties);
#endif
    {
        QScriptValueIterator it(engine->globalObject());
        while (it.hasNext()) {
            it.next();
            global.setProperty(it.scriptName(), it.value(), it.flags());
        }
    }
#if QT_VERSION >= 0x040500
    engine->setGlobalObject(global);
#else
    engine->globalObject().setScope(global);
    global = engine->globalObject();
#endif

    cppGlobal->setMessageHandler(messageHandler);
    QScriptValue ret = engine->evaluate(program, fileName);
    cppGlobal->setMessageHandler(0);
    if (engine->hasUncaughtException()) {
        messageHandler->message(QtCriticalMsg, ret.toString(), fileName,
                                engine->uncaughtExceptionLineNumber());
        delete engine;
        return 0;
    }

    QScriptValue name = global.property(QLatin1String("name"));
    if (!name.isString()) {
        messageHandler->message(QtCriticalMsg, QLatin1String("command definition lacks a name"), fileName);
        delete engine;
        return 0;
    }
    QString nameStr = name.toString();

    QScriptValue group = global.property(QLatin1String("group"));
    if (!group.isString()) {
        messageHandler->message(QtCriticalMsg, QString::fromLatin1("definition of command \"%0\" lacks a group name")
                                .arg(nameStr), fileName);
        delete engine;
        return 0;
    }
    QString groupStr = group.toString();

    QScriptValue shortDesc = global.property(QLatin1String("shortDescription"));
    if (!shortDesc.isString()) {
        messageHandler->message(QtCriticalMsg, QString::fromLatin1("definition of command \"%0\" lacks shortDescription")
                                .arg(nameStr), fileName);
        delete engine;
        return 0;
    }
    QString shortDescStr = shortDesc.toString();

    QScriptValue longDesc = global.property(QLatin1String("longDescription"));
    if (!longDesc.isString()) {
        messageHandler->message(QtCriticalMsg, QString::fromLatin1("definition of command \"%0\" lacks longDescription")
                                .arg(nameStr), fileName);
        delete engine;
        return 0;
    }
    QString longDescStr = longDesc.toString();

    QStringList aliases;
    qScriptValueToSequence(global.property(QLatin1String("aliases")), aliases);

    QStringList seeAlso;
    qScriptValueToSequence(global.property(QLatin1String("seeAlso")), seeAlso);

    QStringList argTypes;
    qScriptValueToSequence(global.property(QLatin1String("argumentTypes")), argTypes);

    QStringList subCommands;
    qScriptValueToSequence(global.property(QLatin1String("subCommands")), subCommands);

    QScriptValue execFunction = global.property(QLatin1String("execute"));
    if (!execFunction.isFunction()) {
        messageHandler->message(QtCriticalMsg, QString::fromLatin1("definition of command \"%0\" lacks execute() function")
                                .arg(nameStr), fileName);
        delete engine;
        return 0;
    }

    QScriptValue responseFunction = global.property(QLatin1String("handleResponse"));

    QScriptDebuggerScriptedConsoleCommand *result = new QScriptDebuggerScriptedConsoleCommand(
        nameStr, groupStr,
        shortDescStr, longDescStr,
        aliases, seeAlso,
        argTypes, subCommands,
        execFunction, responseFunction);
    return result;
}

QT_END_NAMESPACE
