/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt Assistant of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License versions 2.0 or 3.0 as published by the Free Software
** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file.  Alternatively you may (at
** your option) use any later version of the GNU General Public
** License if such license has been publicly approved by Trolltech ASA
** (or its successors, if any) and the KDE Free Qt Foundation. In
** addition, as a special exception, Trolltech gives you certain
** additional rights. These rights are described in the Trolltech GPL
** Exception version 1.2, which can be found at
** http://www.trolltech.com/products/qt/gplexception/ and in the file
** GPL_EXCEPTION.txt in this package.
**
** Please review the following information to ensure GNU General
** Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/. If
** you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** In addition, as a special exception, Trolltech, as the sole
** copyright holder for Qt Designer, grants users of the Qt/Eclipse
** Integration plug-in the right for the Qt/Eclipse Integration to
** link to functionality provided by Qt Designer and its related
** libraries.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not expressly
** granted herein.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
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
{
    m_mainWindow = mainWindow;
    m_helpEngine = helpEngine;
    m_debug = false;
    m_caching = true;
    m_syncContents = false;
    m_expandTOC = -3;
    connect(m_mainWindow, SIGNAL(initDone()),
        this, SLOT(applyCache()));
#ifdef Q_OS_WIN
    StdInListenerWin *l = new StdInListenerWin(this);
    connect(l, SIGNAL(receivedCommand(const QString&)),
        this, SLOT(handleCommandString(const QString&)));
    l->start();
#else
    QSocketNotifier *notifier = new QSocketNotifier(fileno(stdin),
        QSocketNotifier::Read, this);
    connect(notifier, SIGNAL(activated(int)),
        this, SLOT(receivedData()));
    notifier->setEnabled(true);
#endif
}

void RemoteControl::receivedData()
{
    QByteArray ba;
    while (true) {
	int c = getc(stdin);
	if (c == EOF)
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
                    m_setSource = url;
                    m_syncContents = false;
                    m_activateKeyword.clear();
                    m_activateIdentifier.clear();
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
                m_activateKeyword = arg;
                m_syncContents = false;
                m_setSource.clear();
                m_activateIdentifier.clear();
            } else {
                m_mainWindow->setIndexString(arg);
                if (!arg.isEmpty())
                    m_helpEngine->indexWidget()->activateCurrentItem();
            }
        } else if (cmd == QLatin1String("activateidentifier")) {
            if (m_caching) {
                m_activateIdentifier = arg;
                m_setSource.clear();
                m_syncContents = false;
                m_activateKeyword.clear();
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
        if (!m_activateKeyword.isEmpty())
            m_helpEngine->indexWidget()->activateCurrentItem();
    } else if (!m_activateIdentifier.isEmpty()) {
        QMap<QString, QUrl> links =
            m_helpEngine->linksForIdentifier(m_activateIdentifier);
        if (links.count())
            CentralWidget::instance()->setSource(links.constBegin().value());
    }
    
    if (m_syncContents)
        m_mainWindow->syncContents();
    
    if (m_expandTOC != -3)
        m_mainWindow->expandTOC(m_expandTOC);

    m_caching = false;
}

QT_END_NAMESPACE
