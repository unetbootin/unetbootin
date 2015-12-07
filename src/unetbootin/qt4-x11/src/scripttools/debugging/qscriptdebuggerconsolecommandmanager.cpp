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

#include "qscriptdebuggerconsolecommandmanager_p.h"
#include "qscriptdebuggerconsolecommand_p.h"
#include "qscriptdebuggerconsolecommandgroupdata_p.h"

#include <QtCore/qlist.h>
#include <QtCore/qstringlist.h>

QT_BEGIN_NAMESPACE

/*!
  \since 4.5
  \class QScriptDebuggerConsoleCommandManager
  \internal

  \brief The QScriptDebuggerConsoleCommandManager manages a collection of console commands.

*/

class QScriptDebuggerConsoleCommandManagerPrivate
{
    Q_DECLARE_PUBLIC(QScriptDebuggerConsoleCommandManager)
public:
    QScriptDebuggerConsoleCommandManagerPrivate();
    ~QScriptDebuggerConsoleCommandManagerPrivate();

    QList<QScriptDebuggerConsoleCommand*> commands;
    QMap<QString, QScriptDebuggerConsoleCommandGroupData> groups;

    QScriptDebuggerConsoleCommandManager *q_ptr;
};

QScriptDebuggerConsoleCommandManagerPrivate::QScriptDebuggerConsoleCommandManagerPrivate()
{
    groups[QLatin1String("breakpoints")] =
        QScriptDebuggerConsoleCommandGroupData(QLatin1String("Making program stop at certain points"),
                                               QLatin1String(""));
    groups[QLatin1String("files")] =
        QScriptDebuggerConsoleCommandGroupData(QLatin1String("Examining files"),
                                               QLatin1String(""));
    groups[QLatin1String("stack")] =
        QScriptDebuggerConsoleCommandGroupData(QLatin1String("Examining the stack"),
                                               QLatin1String(""));
    groups[QLatin1String("running")] =
        QScriptDebuggerConsoleCommandGroupData(QLatin1String("Running the program"),
                                               QLatin1String(""));
    groups[QLatin1String("status")] =
        QScriptDebuggerConsoleCommandGroupData(QLatin1String("Status inquiries"),
                                               QLatin1String(""));
    groups[QLatin1String("void")] =
        QScriptDebuggerConsoleCommandGroupData(QLatin1String("No such group"),
                                               QLatin1String("It's a secret to everyone"));
}

QScriptDebuggerConsoleCommandManagerPrivate::~QScriptDebuggerConsoleCommandManagerPrivate()
{
    qDeleteAll(commands);
}

QScriptDebuggerConsoleCommandManager::QScriptDebuggerConsoleCommandManager()
    : d_ptr(new QScriptDebuggerConsoleCommandManagerPrivate)
{
    d_ptr->q_ptr = this;
}

QScriptDebuggerConsoleCommandManager::~QScriptDebuggerConsoleCommandManager()
{
    delete d_ptr;
}

/*!
  Adds the given \a command.
  The manager takes ownership of the command.
*/
void QScriptDebuggerConsoleCommandManager::addCommand(QScriptDebuggerConsoleCommand *command)
{
    Q_D(QScriptDebuggerConsoleCommandManager);
    Q_ASSERT(command != 0);
    if (command->name().isEmpty()) {
        qWarning("addCommand(): nameless command ignored");
        return;
    }
    if (command->group().isEmpty()) {
        qWarning("addCommand(): groupless command '%s' ignored",
                 qPrintable(command->name()));
        return;
    }
    if (findCommand(command->name()) != 0) {
        qWarning("addCommand(): duplicate command '%s' (group '%s') ignored",
                 qPrintable(command->name()), qPrintable(command->group()));
        return;
    }
    if (!d->groups.contains(command->group())) {
        qWarning("addCommand(): group '%s' for command '%s' is unknown!",
                 qPrintable(command->group()), qPrintable(command->name()));
    }
    d->commands.append(command);
}

/*!
  Registers a command group with the given \a name and \a data.
*/
void QScriptDebuggerConsoleCommandManager::addCommandGroup(
    const QString &name, const QScriptDebuggerConsoleCommandGroupData &data)
{
    Q_D(QScriptDebuggerConsoleCommandManager);
    if (name.isEmpty()) {
        qWarning("addCommandGroup(): nameless group ignored");
        return;
    }
    if (d->groups.contains(name)) {
        qWarning("addCommandGroup(): group '%s' already defined",
                 qPrintable(name));
        return;
    }
    d->groups[name] = data;
}

/*!
  Returns the command with the given \a name if one exists, otherwise
  returns 0.
*/
QScriptDebuggerConsoleCommand *QScriptDebuggerConsoleCommandManager::findCommand(const QString &name) const
{
    Q_D(const QScriptDebuggerConsoleCommandManager);
    for (int i = 0; i < d->commands.size(); ++i) {
        QScriptDebuggerConsoleCommand *cmd = d->commands.at(i);
        if (cmd->name() == name)
            return cmd;
        else if (cmd->aliases().contains(name))
            return cmd;
    }
    return 0;
}

/*!
  Returns the commands organized into groups.
*/
QMap<QString, QList<QScriptDebuggerConsoleCommand*> > QScriptDebuggerConsoleCommandManager::commands() const
{
    Q_D(const QScriptDebuggerConsoleCommandManager);
    QMap<QString, QList<QScriptDebuggerConsoleCommand*> > result;
    for (int i = 0; i < d->commands.size(); ++i) {
        QScriptDebuggerConsoleCommand *cmd = d->commands.at(i);
        result[cmd->group()].append(cmd);
    }
    return result;
}

/*!
  Returns commands in the group of the given \a name.
*/
QScriptDebuggerConsoleCommandList QScriptDebuggerConsoleCommandManager::commandsInGroup(const QString &name) const
{
    Q_D(const QScriptDebuggerConsoleCommandManager);
    QScriptDebuggerConsoleCommandList result;
    for (int i = 0; i < d->commands.size(); ++i) {
        QScriptDebuggerConsoleCommand *cmd = d->commands.at(i);
        if (cmd->group() == name)
            result.append(cmd);
    }
    return result;
}

/*!
  Returns data associated with the group of the given \a name.
*/
QScriptDebuggerConsoleCommandGroupData QScriptDebuggerConsoleCommandManager::commandGroupData(const QString &name) const
{
    Q_D(const QScriptDebuggerConsoleCommandManager);
    return d->groups.value(name);
}

/*!
  Returns all command groups.
*/
QMap<QString, QScriptDebuggerConsoleCommandGroupData> QScriptDebuggerConsoleCommandManager::commandGroups() const
{
    Q_D(const QScriptDebuggerConsoleCommandManager);
    return d->groups;
}

/*!
  Returns the possible completions for the given \a prefix.
*/
QStringList QScriptDebuggerConsoleCommandManager::completions(const QString &prefix) const
{
    Q_D(const QScriptDebuggerConsoleCommandManager);
    QStringList result;
    for (int i = 0; i < d->commands.size(); ++i) {
        QScriptDebuggerConsoleCommand *cmd = d->commands.at(i);
        QStringList names;
        names.append(cmd->name());
//        names += cmd->aliases();
        for (int j = 0; j < names.size(); ++j) {
            const QString &name = names.at(j);
            if ((name.length() > prefix.length()) && name.startsWith(prefix))
                result.append(name);
        }
    }
    qStableSort(result);
    return result;
}

QT_END_NAMESPACE
