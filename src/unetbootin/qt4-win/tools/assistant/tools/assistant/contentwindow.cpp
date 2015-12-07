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

#include "contentwindow.h"
#include "mainwindow.h"
#include "centralwidget.h"

#include <QtGui/QLayout>
#include <QtGui/QFocusEvent>
#include <QtGui/QMenu>
#include <QtGui/QTreeView>

#include <QtHelp/QHelpEngine>
#include <QtHelp/QHelpContentWidget>

QT_BEGIN_NAMESPACE

ContentWindow::ContentWindow(QHelpEngine *helpEngine)
{
	m_helpEngine = helpEngine;
    m_expandDepth = -2;

    QVBoxLayout *layout = new QVBoxLayout(this);
	m_contentWidget = m_helpEngine->contentWidget();
	connect(m_contentWidget, SIGNAL(linkActivated(const QUrl&)),
		this, SIGNAL(linkActivated(const QUrl&)));
    connect(m_contentWidget, SIGNAL(clicked(const QModelIndex&)),
        this, SLOT(itemClicked(const QModelIndex&)));
    layout->setMargin(4);
	layout->addWidget(m_contentWidget);

    m_contentWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_contentWidget, SIGNAL(customContextMenuRequested(const QPoint&)),
        this, SLOT(showContextMenu(const QPoint&)));
    QHelpContentModel *contentModel = 
        qobject_cast<QHelpContentModel*>(m_contentWidget->model());
    connect(contentModel, SIGNAL(contentsCreated()),
        this, SLOT(expandTOC()));
}

ContentWindow::~ContentWindow()
{
}

bool ContentWindow::syncToContent(const QUrl& url)
{
    QModelIndex idx = m_contentWidget->indexOf(url);
    if (!idx.isValid())
        return false;
    m_contentWidget->setCurrentIndex(idx);
    return true;
}

void ContentWindow::expandTOC()
{
    if (m_expandDepth > -2) {
        expandToDepth(m_expandDepth);
        m_expandDepth = -2;
    }    
}

void ContentWindow::expandToDepth(int depth)
{
    m_expandDepth = depth;
    if (depth == -1)
        m_contentWidget->expandAll();
    else
        m_contentWidget->expandToDepth(depth);
}

void ContentWindow::focusInEvent(QFocusEvent *e)
{
    if (e->reason() != Qt::MouseFocusReason)
        m_contentWidget->setFocus();
}

void ContentWindow::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape)
        MainWindow::activateCurrentCentralWidgetTab();
}

void ContentWindow::showContextMenu(const QPoint &pos)
{
    if (!m_contentWidget->indexAt(pos).isValid())
        return;

    QMenu menu;
    QAction *curTab = menu.addAction(tr("Open Link"));
    QAction *newTab = menu.addAction(tr("Open Link in New Tab"));
    menu.move(m_contentWidget->mapToGlobal(pos));

    QHelpContentModel *contentModel = 
        qobject_cast<QHelpContentModel*>(m_contentWidget->model());
    QHelpContentItem *itm = 
        contentModel->contentItemAt(m_contentWidget->currentIndex());

    QAction *action = menu.exec();
    if (curTab == action)
        emit linkActivated(itm->url());
    else if (newTab == action)
        CentralWidget::instance()->setSourceInNewTab(itm->url());    
}

void ContentWindow::itemClicked(const QModelIndex &index)
{
    if (!index.isValid())
        return;
    QHelpContentModel *contentModel = 
        qobject_cast<QHelpContentModel*>(m_contentWidget->model());
    QHelpContentItem *itm = 
        contentModel->contentItemAt(index);
    if (itm)
        emit linkActivated(itm->url());
}

QT_END_NAMESPACE
