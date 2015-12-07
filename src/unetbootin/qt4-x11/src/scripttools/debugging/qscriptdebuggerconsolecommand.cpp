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

#include "qscriptdebuggerconsolecommand_p.h"
#include "qscriptdebuggerconsolecommand_p_p.h"

#include <QtCore/qstringlist.h>

QT_BEGIN_NAMESPACE

/*!
  \since 4.5
  \class QScriptDebuggerConsoleCommand
  \internal

  \brief The QScriptDebuggerConsoleCommand class is the base class of console commands.

  \sa QScriptDebuggerConsoleCommandManager
*/

QScriptDebuggerConsoleCommandPrivate::QScriptDebuggerConsoleCommandPrivate()
{
}

QScriptDebuggerConsoleCommandPrivate::~QScriptDebuggerConsoleCommandPrivate()
{
}

QScriptDebuggerConsoleCommand::QScriptDebuggerConsoleCommand()
    : d_ptr(new QScriptDebuggerConsoleCommandPrivate)
{
    d_ptr->q_ptr = this;
}

QScriptDebuggerConsoleCommand::~QScriptDebuggerConsoleCommand()
{
    delete d_ptr;
}

QScriptDebuggerConsoleCommand::QScriptDebuggerConsoleCommand(QScriptDebuggerConsoleCommandPrivate &dd)
    : d_ptr(&dd)
{
    d_ptr->q_ptr = this;
}

/*!
  \fn QString QScriptDebuggerConsoleCommand::name() const

  Returns the name of this console command.
*/

/*!
  \fn QString QScriptDebuggerConsoleCommand::group() const

  Returns the group that this console command belongs to.
*/

/*!
  \fn QString QScriptDebuggerConsoleCommand::shortDescription() const

  Returns a short (one line) description of the command.
*/

/*!
  \fn QString QScriptDebuggerConsoleCommand::longDescription() const

  Returns a detailed description of how to use the command.
*/

/*!
  \fn QScriptDebuggerConsoleCommandJob *QScriptDebuggerConsoleCommand::createJob(
        const QStringList &arguments,
        QScriptDebuggerConsole *console,
        QScriptMessageHandlerInterface *messageHandler,
        QScriptDebuggerCommandSchedulerInterface *scheduler) = 0

  Creates a job that will execute this command with the given \a
  arguments. If the command cannot be executed (e.g. because one or
  more arguments are invalid), a suitable error message should be
  output to the \a messageHandler, and 0 should be returned.
*/

/*!
  Returns a list of names of commands that may also be of interest to
  users of this command.
*/
QStringList QScriptDebuggerConsoleCommand::seeAlso() const
{
    return QStringList();
}

/*!
  Returns a list of aliases for this command.
*/
QStringList QScriptDebuggerConsoleCommand::aliases() const
{
    return QStringList();
}

QStringList QScriptDebuggerConsoleCommand::argumentTypes() const
{
    return QStringList();
}

QStringList QScriptDebuggerConsoleCommand::subCommands() const
{
    return QStringList();
}

QT_END_NAMESPACE
