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

#include "indexwindow.h"
#include "mainwindow.h"
#include "centralwidget.h"
#include "topicchooser.h"

#include <QtGui/QLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QKeyEvent>
#include <QtGui/QMenu>
#include <QtGui/QContextMenuEvent>
#include <QtGui/QListWidgetItem>

#include <QtHelp/QHelpEngine>
#include <QtHelp/QHelpIndexWidget>

QT_BEGIN_NAMESPACE

IndexWindow::IndexWindow(QHelpEngine *helpEngine, QWidget *parent)
    : QWidget(parent)
{
    m_helpEngine = helpEngine;
    
    QVBoxLayout *layout = new QVBoxLayout(this);
    QLabel *l = new QLabel(tr("&Look for:"));
    layout->addWidget(l);

    m_searchLineEdit = new QLineEdit();
    l->setBuddy(m_searchLineEdit);
    connect(m_searchLineEdit, SIGNAL(textChanged(const QString&)),
        this, SLOT(filterIndices(const QString&)));
    m_searchLineEdit->installEventFilter(this);
    layout->setMargin(4);
    layout->addWidget(m_searchLineEdit);

    m_indexWidget = m_helpEngine->indexWidget();
    m_indexWidget->installEventFilter(this);
    connect(m_helpEngine->indexModel(), SIGNAL(indexCreationStarted()),
        this, SLOT(disableSearchLineEdit()));
    connect(m_helpEngine->indexModel(), SIGNAL(indexCreated()),
        this, SLOT(enableSearchLineEdit()));
    connect(m_indexWidget, SIGNAL(linkActivated(const QUrl&, const QString&)),
        this, SIGNAL(linkActivated(const QUrl&)));
    connect(m_indexWidget, SIGNAL(linksActivated(const QMap<QString, QUrl>&, const QString&)),
        this, SIGNAL(linksActivated(const QMap<QString, QUrl>&, const QString&)));
    connect(m_searchLineEdit, SIGNAL(returnPressed()),
        m_indexWidget, SLOT(activateCurrentItem()));
    layout->addWidget(m_indexWidget);
}

IndexWindow::~IndexWindow()
{
}

void IndexWindow::filterIndices(const QString &filter)
{
    if (filter.contains(QLatin1Char('*')))
        m_indexWidget->filterIndices(filter, filter);
    else
        m_indexWidget->filterIndices(filter, QString());
}

bool IndexWindow::eventFilter(QObject *obj, QEvent *e)
{
    if (obj == m_searchLineEdit && e->type() == QEvent::KeyPress) {
        QKeyEvent *ke = static_cast<QKeyEvent*>(e);
        QModelIndex idx = m_indexWidget->currentIndex();
        switch (ke->key()) {
        case Qt::Key_Up:
            idx = m_indexWidget->model()->index(idx.row()-1,
                idx.column(), idx.parent());
            if (idx.isValid())
                m_indexWidget->setCurrentIndex(idx);
            break;
        case Qt::Key_Down:
            idx = m_indexWidget->model()->index(idx.row()+1,
                idx.column(), idx.parent());
            if (idx.isValid())
                m_indexWidget->setCurrentIndex(idx);
            break;
        case Qt::Key_Escape:
            MainWindow::activateCurrentCentralWidgetTab();
            break;
        default:
            ;
        }
    } else if (obj == m_indexWidget && e->type() == QEvent::ContextMenu) {
        QContextMenuEvent *ctxtEvent = static_cast<QContextMenuEvent*>(e);
        QModelIndex idx = m_indexWidget->indexAt(ctxtEvent->pos());
        if (idx.isValid()) {
            QMenu menu;
            QAction *curTab = menu.addAction(tr("Open Link"));
            QAction *newTab = menu.addAction(tr("Open Link in New Tab"));
            menu.move(m_indexWidget->mapToGlobal(ctxtEvent->pos()));

            QAction *action = menu.exec();
            if (curTab == action)
                m_indexWidget->activateCurrentItem();
            else if (newTab == action) {
                QHelpIndexModel *model = qobject_cast<QHelpIndexModel*>(m_indexWidget->model());
                QString keyword = model->data(idx, Qt::DisplayRole).toString();
                if (model) {
                    QMap<QString, QUrl> links = model->linksForKeyword(keyword);
                    if (links.count() == 1) {
                        CentralWidget::instance()->setSourceInNewTab(links.constBegin().value());
                    } else {
                        TopicChooser tc(this, keyword, links);
                        if (tc.exec() == QDialog::Accepted) {
                            CentralWidget::instance()->setSourceInNewTab(tc.link());
                        }
                    }
                }
            }
        }
    }
    return QWidget::eventFilter(obj, e);
}

void IndexWindow::enableSearchLineEdit()
{
    m_searchLineEdit->setDisabled(false);
    filterIndices(m_searchLineEdit->text());
}

void IndexWindow::disableSearchLineEdit()
{
    m_searchLineEdit->setDisabled(true);
}

void IndexWindow::setSearchLineEditText(const QString &text)
{
    m_searchLineEdit->setText(text);
}

void IndexWindow::focusInEvent(QFocusEvent *e)
{
    if (e->reason() != Qt::MouseFocusReason) {
        m_searchLineEdit->selectAll();
        m_searchLineEdit->setFocus();
    }
}

QT_END_NAMESPACE
