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

#ifndef QSCRIPTDEBUGGER_P_H
#define QSCRIPTDEBUGGER_P_H

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

#include <QtCore/qobject.h>

QT_BEGIN_NAMESPACE

class QScriptDebuggerFrontend;
class QScriptDebuggerConsoleWidgetInterface;
class QScriptDebuggerScriptsWidgetInterface;
class QScriptDebuggerCodeWidgetInterface;
class QScriptDebuggerCodeFinderWidgetInterface;
class QScriptBreakpointsWidgetInterface;
class QScriptDebuggerStackWidgetInterface;
class QScriptDebuggerLocalsWidgetInterface;
class QScriptDebugOutputWidgetInterface;
class QScriptErrorLogWidgetInterface;
class QScriptDebuggerWidgetFactoryInterface;
class QAction;
class QEvent;

class QScriptDebuggerPrivate;
class Q_AUTOTEST_EXPORT QScriptDebugger : public QObject
{
    Q_OBJECT
public:
    QScriptDebugger(QObject *parent = 0);
    ~QScriptDebugger();

    QScriptDebuggerFrontend *frontend() const;
    void setFrontend(QScriptDebuggerFrontend *frontend);

    QScriptDebuggerConsoleWidgetInterface *consoleWidget() const;
    void setConsoleWidget(QScriptDebuggerConsoleWidgetInterface *consoleWidget);

    QScriptDebuggerScriptsWidgetInterface *scriptsWidget() const;
    void setScriptsWidget(QScriptDebuggerScriptsWidgetInterface *scriptsWidget);

    QScriptDebuggerCodeWidgetInterface *codeWidget() const;
    void setCodeWidget(QScriptDebuggerCodeWidgetInterface *codeWidget);

    QScriptDebuggerCodeFinderWidgetInterface *codeFinderWidget() const;
    void setCodeFinderWidget(QScriptDebuggerCodeFinderWidgetInterface *codeFinderWidget);

    QScriptDebuggerStackWidgetInterface *stackWidget() const;
    void setStackWidget(QScriptDebuggerStackWidgetInterface *stackWidget);

    QScriptDebuggerLocalsWidgetInterface *localsWidget() const;
    void setLocalsWidget(QScriptDebuggerLocalsWidgetInterface *localsWidget);

    QScriptBreakpointsWidgetInterface *breakpointsWidget() const;
    void setBreakpointsWidget(QScriptBreakpointsWidgetInterface *breakpointsWidget);

    QScriptDebugOutputWidgetInterface *debugOutputWidget() const;
    void setDebugOutputWidget(QScriptDebugOutputWidgetInterface *debugOutputWidget);

    QScriptErrorLogWidgetInterface *errorLogWidget() const;
    void setErrorLogWidget(QScriptErrorLogWidgetInterface *errorLogWidget);

    QScriptDebuggerWidgetFactoryInterface *widgetFactory() const;
    void setWidgetFactory(QScriptDebuggerWidgetFactoryInterface *factory);

    QAction *interruptAction(QObject *parent) const;
    QAction *continueAction(QObject *parent) const;
    QAction *stepIntoAction(QObject *parent) const;
    QAction *stepOverAction(QObject *parent) const;
    QAction *stepOutAction(QObject *parent) const;
    QAction *runToCursorAction(QObject *parent) const;
    QAction *runToNewScriptAction(QObject *parent) const;

    QAction *toggleBreakpointAction(QObject *parent) const;

    QAction *findInScriptAction(QObject *parent) const;
    QAction *findNextInScriptAction(QObject *parent) const;
    QAction *findPreviousInScriptAction(QObject *parent) const;
    QAction *goToLineAction(QObject *parent) const;

    QAction *clearDebugOutputAction(QObject *parent) const;
    QAction *clearConsoleAction(QObject *parent) const;
    QAction *clearErrorLogAction(QObject *parent) const;

    bool eventFilter(QObject *, QEvent *e);

Q_SIGNALS:
    void stopped() const;
    void started() const;

protected:
    void timerEvent(QTimerEvent *e);

protected:
    QScriptDebugger(QScriptDebuggerPrivate &dd, QObject *parent);

private:
    Q_DECLARE_PRIVATE(QScriptDebugger)
    Q_DISABLE_COPY(QScriptDebugger)

    Q_PRIVATE_SLOT(d_func(), void _q_onLineEntered(const QString &))
    Q_PRIVATE_SLOT(d_func(), void _q_onCurrentFrameChanged(int))
    Q_PRIVATE_SLOT(d_func(), void _q_onCurrentScriptChanged(qint64))
    Q_PRIVATE_SLOT(d_func(), void _q_onScriptLocationSelected(int))

    Q_PRIVATE_SLOT(d_func(), void _q_interrupt())
    Q_PRIVATE_SLOT(d_func(), void _q_continue())
    Q_PRIVATE_SLOT(d_func(), void _q_stepInto())
    Q_PRIVATE_SLOT(d_func(), void _q_stepOver())
    Q_PRIVATE_SLOT(d_func(), void _q_stepOut())
    Q_PRIVATE_SLOT(d_func(), void _q_runToCursor())
    Q_PRIVATE_SLOT(d_func(), void _q_runToNewScript())

    Q_PRIVATE_SLOT(d_func(), void _q_toggleBreakpoint())

    Q_PRIVATE_SLOT(d_func(), void _q_clearDebugOutput())
    Q_PRIVATE_SLOT(d_func(), void _q_clearErrorLog())
    Q_PRIVATE_SLOT(d_func(), void _q_clearConsole())

    Q_PRIVATE_SLOT(d_func(), void _q_findInScript())
    Q_PRIVATE_SLOT(d_func(), void _q_findNextInScript())
    Q_PRIVATE_SLOT(d_func(), void _q_findPreviousInScript())
    Q_PRIVATE_SLOT(d_func(), void _q_onFindCodeRequest(const QString &, int))
    Q_PRIVATE_SLOT(d_func(), void _q_goToLine())
};

QT_END_NAMESPACE

#endif
