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

#include "mainwindow.h"
#include "searchwidget.h"

#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>

#include <QtGui/QMenu>
#include <QtGui/QLayout>
#include <QtGui/QKeyEvent>
#include <QtGui/QClipboard>
#include <QtGui/QApplication>
#include <QtGui/QTextBrowser>

#include <QtHelp/QHelpSearchEngine>
#include <QtHelp/QHelpSearchQueryWidget>
#include <QtHelp/QHelpSearchResultWidget>

QT_BEGIN_NAMESPACE

SearchWidget::SearchWidget(QHelpSearchEngine *engine, QWidget *parent)
    : QWidget(parent)
    , zoomCount(0)
    , searchEngine(engine)
{
    QVBoxLayout *vLayout = new QVBoxLayout(this);

    resultWidget = searchEngine->resultWidget();
    QHelpSearchQueryWidget *queryWidget = searchEngine->queryWidget();

    vLayout->addWidget(queryWidget);
    vLayout->addWidget(resultWidget);        
    
    setFocusProxy(queryWidget);

    connect(queryWidget, SIGNAL(search()), this, SLOT(search()));
    connect(resultWidget, SIGNAL(requestShowLink(const QUrl&)), 
        this, SIGNAL(requestShowLink(const QUrl&)));

    connect(searchEngine, SIGNAL(searchingStarted()), this, SLOT(searchingStarted()));
    connect(searchEngine, SIGNAL(searchingFinished(int)), this, SLOT(searchingFinished(int)));

    QTextBrowser* browser = qFindChild<QTextBrowser*>(resultWidget);
    browser->viewport()->installEventFilter(this);
}

SearchWidget::~SearchWidget()
{
    // nothing todo
}

void SearchWidget::zoomIn()
{
#ifndef QT_CLUCENE_SUPPORT
    return;
#endif

    QTextBrowser* browser = qFindChild<QTextBrowser*>(resultWidget);
    if (browser && zoomCount != 10) {
        zoomCount++;
        browser->zoomIn();
    }
}

void SearchWidget::zoomOut()
{
#ifndef QT_CLUCENE_SUPPORT
        return;
#endif

    QTextBrowser* browser = qFindChild<QTextBrowser*>(resultWidget);
    if (browser && zoomCount != -5) {
        zoomCount--;
        browser->zoomOut();
    }
}

void SearchWidget::resetZoom()
{
#ifndef QT_CLUCENE_SUPPORT
    return;
#endif

    if (zoomCount == 0)
        return;

    QTextBrowser* browser = qFindChild<QTextBrowser*>(resultWidget);
    if (browser) {
        browser->zoomOut(zoomCount);
        zoomCount = 0;
    }
}

void SearchWidget::search() const
{
    QList<QHelpSearchQuery> query = searchEngine->queryWidget()->query();
    searchEngine->search(query);
}

void SearchWidget::searchingStarted()
{
    qApp->setOverrideCursor(QCursor(Qt::WaitCursor));
}

void SearchWidget::searchingFinished(int hits)
{
    Q_UNUSED(hits)
    qApp->restoreOverrideCursor();
}

bool SearchWidget::eventFilter(QObject* o, QEvent *e)
{
    QTextBrowser* browser = qFindChild<QTextBrowser*>(resultWidget);
    if (browser && o == browser->viewport() && e->type() == QEvent::MouseButtonRelease){
        QMouseEvent *me = static_cast<QMouseEvent*>(e);
        QUrl link = resultWidget->linkAt(me->pos());
        if (!link.isEmpty() || link.isValid()) {
            bool controlPressed = me->modifiers() & Qt::ControlModifier;
            if((me->button() == Qt::LeftButton && controlPressed)
                || (me->button() == Qt::MidButton)) {
                    emit requestShowLinkInNewTab(link);
            }
        }
    }
    return QWidget::eventFilter(o,e);
}

void SearchWidget::keyPressEvent(QKeyEvent *keyEvent)
{
    if (keyEvent->key() == Qt::Key_Escape)
        MainWindow::activateCurrentBrowser();
    else
        keyEvent->ignore();
}

void SearchWidget::contextMenuEvent(QContextMenuEvent *contextMenuEvent)
{
    QMenu menu;
    QPoint point = contextMenuEvent->globalPos();

#ifdef QT_CLUCENE_SUPPORT
    QTextBrowser* browser = qFindChild<QTextBrowser*>(resultWidget);
    if (!browser)
        return;

    point = browser->mapFromGlobal(point);
    if (!browser->rect().contains(point, true))
        return;

    QUrl link = browser->anchorAt(point);

    QAction *copyAction = menu.addAction(tr("&Copy") + 
        QString(QLatin1String("\t") + QString(QKeySequence(Qt::CTRL | Qt::Key_C))));
    copyAction->setEnabled(QTextCursor(browser->textCursor()).hasSelection());

    QAction *copyAnchorAction = menu.addAction(tr("Copy &Link Location"));
    copyAnchorAction->setEnabled(!link.isEmpty() && link.isValid());

    QAction *newTabAction = menu.addAction(tr("Open Link in New Tab") + 
        QString(QLatin1String("\t") + QString(QKeySequence(Qt::CTRL))) + 
        QLatin1String("LMB"));
    newTabAction->setEnabled(!link.isEmpty() && link.isValid());

    menu.addSeparator();

    QAction *selectAllAction = menu.addAction(tr("Select All") + 
        QString(QLatin1String("\t") + QString(QKeySequence(Qt::CTRL | Qt::Key_A))));

    QAction *usedAction = menu.exec(mapToGlobal(contextMenuEvent->pos()));
    if (usedAction == copyAction) {
        QTextCursor cursor = browser->textCursor();
        if (!cursor.isNull() && cursor.hasSelection()) {
            QString selectedText = cursor.selectedText();
            QMimeData *data = new QMimeData();
            data->setText(selectedText);
            QApplication::clipboard()->setMimeData(data);
        }
    }
    else if (usedAction == copyAnchorAction) {
        QApplication::clipboard()->setText(link.toString());
    }
    else if (usedAction == newTabAction) {
        emit requestShowLinkInNewTab(link);
    } 
    else if (usedAction == selectAllAction) {
        browser->selectAll();
    }
#else
    point = resultWidget->mapFromGlobal(point);
    QUrl link = resultWidget->linkAt(point);
    if (link.isEmpty() || !link.isValid())
        return;

    QAction *curTab = menu.addAction(tr("Open Link"));
    QAction *newTab = menu.addAction(tr("Open Link in New Tab"));

    QAction *action = menu.exec(mapToGlobal(contextMenuEvent->pos()));
    if (curTab == action)
        emit requestShowLink(link);
    else if (newTab == action)
        emit requestShowLinkInNewTab(link);
#endif
}

QT_END_NAMESPACE
