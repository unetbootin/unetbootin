/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt Assistant of the Qt Toolkit.
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

#include "remotecontrol.h"
#include "mainwindow.h"
#include "centralwidget.h"

#include <QtCore/QFile>
#include <QtCore/QThread>
#include <QtCore/QTextStream>
#include <QtCore/QSocketNotifier>

#include <QtGui/QMessageBox>
#include <QtGui/QApplication>

#include <QtHelp/QHelpEngine>
#include <QtHelp/QHelpIndexWidget>

#ifdef Q_OS_WIN
#   include "remotecontrol_win.h"
#endif

QT_BEGIN_NAMESPACE

#ifdef Q_OS_WIN

StdInListenerWin::StdInListenerWin(QObject *parent)
    : QThread(parent)
{
}

StdInListenerWin::~StdInListenerWin()
{
    terminate();
    wait();
}

void StdInListenerWin::run()
{
    bool ok = true;
    char chBuf[4096];
    DWORD dwRead;
    HANDLE hStdin, hStdinDup;

    hStdin = GetStdHandle(STD_INPUT_HANDLE);
    if (hStdin == INVALID_HANDLE_VALUE)
        return;

    DuplicateHandle(GetCurrentProcess(), hStdin,
        GetCurrentProcess(), &hStdinDup,
        0, false, DUPLICATE_SAME_ACCESS);

    CloseHandle(hStdin);

    while (ok) {
        ok = ReadFile(hStdinDup, chBuf, 4096, &dwRead, NULL);
        if (ok && dwRead != 0)
            emit receivedCommand(QString::fromLocal8Bit(chBuf));
    }
}
#endif

RemoteControl::RemoteControl(MainWindow *mainWindow, QHelpEngine *helpEngine)
    : QObject(mainWindow)
    , m_mainWindow(mainWindow)
    , m_helpEngine(helpEngine)
    , m_debug(false)
    , m_caching(true)
    , m_syncContents(false)
    , m_expandTOC(-3)

{
    connect(m_mainWindow, SIGNAL(initDone()), this, SLOT(applyCache()));
#ifdef Q_OS_WIN
    StdInListenerWin *l = new StdInListenerWin(this);
    connect(l, SIGNAL(receivedCommand(const QString&)),
        this, SLOT(handleCommandString(const QString&)));
    l->start();
#else
    QSocketNotifier *notifier = new QSocketNotifier(fileno(stdin),
        QSocketNotifier::Read, this);
    connect(notifier, SIGNAL(activated(int)), this, SLOT(receivedData()));
    notifier->setEnabled(true);
#endif
}

void RemoteControl::receivedData()
{
    QByteArray ba;
    while (true) {
        char c = getc(stdin);
        if (c == EOF || c == '\0')
            break;
        if (c)
            ba.append(c);
         if (c == '\n')
             break;
    }
    handleCommandString(QString::fromLocal8Bit(ba));
}

void RemoteControl::handleCommandString(const QString &cmdString)
{
    QStringList cmds = cmdString.split(QLatin1Char(';'));
    QStringList::const_iterator it = cmds.constBegin();
    QString cmdLine, cmd, arg;
    while (it != cmds.constEnd()) {
        cmdLine = (*it).trimmed();
        cmd = cmdLine;
        arg.clear();
        int i = cmdLine.indexOf(QLatin1Char(' '));
        if (i > 0) {
            cmd = cmdLine.left(i);
            arg = cmdLine.mid(i+1);
        }
        cmd = cmd.toLower();

        if (m_debug)
            QMessageBox::information(0, tr("Debugging Remote Control"),
                tr("Received Command: %1 %2").arg(cmd).arg(arg));

        if (cmd == QLatin1String("debug")) {
            if (arg == QLatin1String("on"))
                m_debug = true;
            else
                m_debug = false;
        } else if (cmd == QLatin1String("show")) {
            if (arg.toLower() == QLatin1String("contents")) {
                m_mainWindow->showContents();
            } else if (arg.toLower() == QLatin1String("index")) {
                m_mainWindow->showIndex();
            } else if (arg.toLower() == QLatin1String("bookmarks")) {
                m_mainWindow->showBookmarks();
            } else if (arg.toLower() == QLatin1String("search")) {
                m_mainWindow->showSearch();
            }
        } else if (cmd == QLatin1String("hide")) {
            if (arg.toLower() == QLatin1String("contents")) {
                m_mainWindow->hideContents();
            } else if (arg.toLower() == QLatin1String("index")) {
                m_mainWindow->hideIndex();
            } else if (arg.toLower() == QLatin1String("bookmarks")) {
                m_mainWindow->hideBookmarks();
            } else if (arg.toLower() == QLatin1String("search")) {
                m_mainWindow->hideSearch();
            }
        } else if (cmd == QLatin1String("setsource")) {
            QUrl url(arg);
            if (url.isValid()) {
                if (url.isRelative())
                    url = CentralWidget::instance()->currentSource().resolved(url);
                if (m_caching) {
                    clearCache();
                    m_setSource = url;
                } else {
                    CentralWidget::instance()->setSource(url);
                }
            }
        } else if (cmd == QLatin1String("synccontents")) {
            if (m_caching)
                m_syncContents = true;
            else
                m_mainWindow->syncContents();
        } else if (cmd == QLatin1String("activatekeyword")) {
            if (m_caching) {
                clearCache();
                m_activateKeyword = arg;
            } else {
                m_mainWindow->setIndexString(arg);
                if (!arg.isEmpty())
                    m_helpEngine->indexWidget()->activateCurrentItem();
            }
        } else if (cmd == QLatin1String("activateidentifier")) {
            if (m_caching) {
                clearCache();
                m_activateIdentifier = arg;
            } else {
                QMap<QString, QUrl> links =
                    m_helpEngine->linksForIdentifier(arg);
                if (links.count())
                    CentralWidget::instance()->setSource(links.constBegin().value());
            }
        } else if (cmd == QLatin1String("expandtoc")) {
            bool ok = false;
            int depth = -1;
            if (!arg.isEmpty())
                depth = arg.toInt(&ok);
            if (!ok)
                depth = -1;

            if (m_caching)
                m_expandTOC = depth;
            else
                m_mainWindow->expandTOC(depth);
        } else if (cmd == QLatin1String("setcurrentfilter")) {
            if (m_caching) {
                clearCache();
                m_currentFilter = arg;
            } else {
                m_helpEngine->setCurrentFilter(arg);
            }
        } else {
            return;
        }
        ++it;
    }
    m_mainWindow->raise();
    m_mainWindow->activateWindow();
}

void RemoteControl::applyCache()
{
    if (m_setSource.isValid()) {
        CentralWidget::instance()->setSource(m_setSource);
    } else if (!m_activateKeyword.isEmpty()) {
        m_mainWindow->setIndexString(m_activateKeyword);
        m_helpEngine->indexWidget()->activateCurrentItem();
    } else if (!m_activateIdentifier.isEmpty()) {
        QMap<QString, QUrl> links =
            m_helpEngine->linksForIdentifier(m_activateIdentifier);
        if (links.count())
            CentralWidget::instance()->setSource(links.constBegin().value());
    } else if (!m_currentFilter.isEmpty()) {
        m_helpEngine->setCurrentFilter(m_currentFilter);
    }

    if (m_syncContents)
        m_mainWindow->syncContents();

    if (m_expandTOC != -3)
        m_mainWindow->expandTOC(m_expandTOC);

    m_caching = false;
}

void RemoteControl::clearCache()
{
    m_currentFilter.clear();
    m_setSource.clear();
    m_syncContents = false;
    m_activateKeyword.clear();
    m_activateIdentifier.clear();
}

QT_END_NAMESPACE
