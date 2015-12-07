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

#include "helpwindow.h"
#include "mainwindow.h"
#include "tabbedbrowser.h"
#include "helpdialog.h"
#include "config.h"

#include <QApplication>
#include <QClipboard>
#include <QUrl>
#include <QMessageBox>
#include <QDir>
#include <QFile>
#include <QProcess>
#include <QAction>
#include <QFileInfo>
#include <QFont>
#include <QtEvents>
#include <QTextStream>
#include <QTextCodec>
#include <QStatusBar>
#include <QTextCursor>
#include <QTextObject>
#include <QTextLayout>
#include <QtDebug>
#include <qdesktopservices.h>

#include <QtGui/QClipboard>
#include <QtGui/QApplication>

#if defined(Q_OS_WIN32)
#  include <windows.h>
#endif

QT_BEGIN_NAMESPACE

HelpWindow::HelpWindow(MainWindow *w, QWidget *parent)
    : QTextBrowser(parent)
    , mw(w)
    , blockScroll(false)
    , shiftPressed(false)
    , newWindow(false)
{
    FontSettings settings = Config::configuration()->fontSettings();
    setFont(settings.browserFont);

    connect(this, SIGNAL(copyAvailable(bool)), w, SLOT(copyAvailable(bool)));
}

void HelpWindow::setSource(const QUrl &name)
{
    if (name.isValid()) {
        if (name.scheme() == QLatin1String("http") || name.scheme() == QLatin1String("ftp") 
            || name.scheme() == QLatin1String("mailto") || name.path().endsWith(QLatin1String("pdf"))) {
            bool launched = QDesktopServices::openUrl(name);
            if (!launched) {
                QMessageBox::information(mw, tr("Help"),
                             tr("Unable to launch web browser.\n"),
                             tr("OK"));
            }
            return;
        }

        QFileInfo fi(name.toLocalFile());
        if (name.scheme() == QLatin1String("file") && fi.exists()) {
            if (newWindow || (shiftPressed && hasFocus())) {
                shiftPressed = false;
                mw->saveSettings();
                MainWindow *nmw = new MainWindow;
                nmw->move(mw->geometry().topLeft());
                nmw->show();
                
                if (mw->isMaximized())
                    nmw->showMaximized();
                
                nmw->setup();
                nmw->showLink(name.toString());
            } else {
                QTextBrowser::setSource(name);
                QTextBrowser::scrollToAnchor(name.fragment());
            }
            return;
        }
    }

    mw->statusBar()->showMessage(tr("Failed to open link: '%1'").arg(name.toString()), 5000);
    setHtml(tr("<div align=\"center\"><h1>The page could not be found</h1><br>"
        "<h3>'%1'</h3></div>").arg(name.toString()));
    mw->browsers()->updateTitle(tr("Error..."));
}

void HelpWindow::openLinkInNewWindow()
{
    if (lastAnchor.isEmpty())
        return;
    newWindow = true;
    setSource(lastAnchor);
    newWindow = false;
}

void HelpWindow::openLinkInNewWindow(const QString &link)
{
    lastAnchor = link;
    openLinkInNewWindow();
}

void HelpWindow::openLinkInNewPage()
{
    if(lastAnchor.isEmpty())
        return;
    mw->browsers()->newTab(lastAnchor);
    lastAnchor.clear();
}

void HelpWindow::openLinkInNewPage(const QString &link)
{
    lastAnchor = link;
    openLinkInNewPage();
}

bool HelpWindow::hasAnchorAt(const QPoint& pos)
{
    lastAnchor = anchorAt(pos);
    if (lastAnchor.isEmpty()) 
        return false;
    lastAnchor = source().resolved(lastAnchor).toString();
    if (lastAnchor.at(0) == QLatin1Char('#')) {
        QString src = source().toString();
        int hsh = src.indexOf(QLatin1Char('#'));
        lastAnchor = (hsh>=0 ? src.left(hsh) : src) + lastAnchor;
    }
    return true;
}

void HelpWindow::contextMenuEvent(QContextMenuEvent *e)
{
    QMenu menu(QLatin1String(""), 0);

    QUrl link;
    QAction *copyAnchorAction = 0;
    if (hasAnchorAt(e->pos())) {
        link = anchorAt(e->pos());
        if (link.isRelative())            
            link = source().resolved(link);
        copyAnchorAction = menu.addAction(tr("Copy &Link Location"));
        copyAnchorAction->setEnabled(!link.isEmpty() && link.isValid());

        menu.addAction(tr("Open Link in New Tab"),
                       this, SLOT(openLinkInNewPage()));
        menu.addAction(tr("Open Link in New Window\tShift+LMB"),
                       this, SLOT(openLinkInNewWindow()));
    }
    mw->setupPopupMenu(&menu);
    QAction *action = menu.exec(e->globalPos());
    if (action == copyAnchorAction)
        QApplication::clipboard()->setText(link.toString());
}

void HelpWindow::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() == Qt::XButton1) {
        QTextBrowser::backward();
        return;
    } 
    
    if (e->button() == Qt::XButton2) {
        QTextBrowser::forward();
        return;
    }

    if (e->button() == Qt::MidButton && hasAnchorAt(e->pos())) {
        openLinkInNewPage();
        return;
    }
    QTextBrowser::mouseReleaseEvent(e);
}

void HelpWindow::blockScrolling(bool b)
{
    blockScroll = b;
}

void HelpWindow::ensureCursorVisible()
{
    if (!blockScroll)
        QTextBrowser::ensureCursorVisible();
}

void HelpWindow::mousePressEvent(QMouseEvent *e)
{
    shiftPressed = e->modifiers() & Qt::ShiftModifier;
    if (!(shiftPressed && hasAnchorAt(e->pos())))
        QTextBrowser::mousePressEvent(e);
}

void HelpWindow::keyPressEvent(QKeyEvent *e)
{
    shiftPressed = e->modifiers() & Qt::ShiftModifier;
    QTextBrowser::keyPressEvent(e);
}

bool HelpWindow::isKDERunning() const
{
    return !qgetenv("KDE_FULL_SESSION").isEmpty();
}

QT_END_NAMESPACE
