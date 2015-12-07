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

#ifndef QSCRIPTDEBUGGERCONSOLE_P_H
#define QSCRIPTDEBUGGERCONSOLE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtCore/qobjectdefs.h>

#include "qscriptdebuggerconsolehistorianinterface_p.h"

QT_BEGIN_NAMESPACE

class QString;
class QScriptDebuggerConsoleCommandJob;
class QScriptMessageHandlerInterface;
class QScriptDebuggerCommandSchedulerInterface;
class QScriptDebuggerConsoleCommandManager;

class QScriptDebuggerConsolePrivate;
class Q_AUTOTEST_EXPORT QScriptDebuggerConsole
    : public QScriptDebuggerConsoleHistorianInterface
{
public:
    QScriptDebuggerConsole();
    ~QScriptDebuggerConsole();

    void loadScriptedCommands(const QString &scriptsPath,
                              QScriptMessageHandlerInterface *messageHandler);

    void showDebuggerInfoMessage(QScriptMessageHandlerInterface *messageHandler);

    QScriptDebuggerConsoleCommandManager *commandManager() const;

    QScriptDebuggerConsoleCommandJob *consumeInput(
        const QString &input,
        QScriptMessageHandlerInterface *messageHandler,
        QScriptDebuggerCommandSchedulerInterface *commandScheduler);
    bool hasIncompleteInput() const;
    QString incompleteInput() const;
    void setIncompleteInput(const QString &input);
    QString commandPrefix() const;

    int historyCount() const;
    QString historyAt(int index) const;
    void changeHistoryAt(int index, const QString &newHistory);

    int currentFrameIndex() const;
    void setCurrentFrameIndex(int index);

    qint64 currentScriptId() const;
    void setCurrentScriptId(qint64 id);

    int currentLineNumber() const;
    void setCurrentLineNumber(int lineNumber);

    int evaluateAction() const;
    void setEvaluateAction(int action);

    qint64 sessionId() const;
    void bumpSessionId();

private:
    QScriptDebuggerConsolePrivate *d_ptr;

    Q_DECLARE_PRIVATE(QScriptDebuggerConsole)
    Q_DISABLE_COPY(QScriptDebuggerConsole)
};

QT_END_NAMESPACE

#endif
