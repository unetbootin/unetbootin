/****************************************************************************
**
** Copyright (C) 2006-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the documentation of the Qt Toolkit.
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

#include <QtGui>

#include "mainwindow.h"
#include "model.h"

MainWindow::MainWindow()
{
    QMenu *fileMenu = new QMenu(tr("&File"));

    QAction *quitAction = fileMenu->addAction(tr("E&xit"));
    quitAction->setShortcut(tr("Ctrl+Q"));

    QMenu *itemsMenu = new QMenu(tr("&Items"));

    insertAction = itemsMenu->addAction(tr("&Insert Item"));
    removeAction = itemsMenu->addAction(tr("&Remove Item"));
    QAction *ascendingAction = itemsMenu->addAction(tr("Sort in &Ascending Order"));
    QAction *descendingAction = itemsMenu->addAction(tr("Sort in &Descending Order"));

    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(itemsMenu);

    QStringList strings;
    strings << tr("Oak") << tr("Fir") << tr("Pine") << tr("Birch")
            << tr("Hazel") << tr("Redwood") << tr("Sycamore") << tr("Chestnut");
    model = new StringListModel(strings, this);
/*  For convenient quoting:
    QListView *listView = new QListView(this);
*/
    listView = new QListView(this);
    listView->setModel(model);
    listView->setSelectionMode(QAbstractItemView::SingleSelection);

    connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));
    connect(ascendingAction, SIGNAL(triggered()), this, SLOT(sortAscending()));
    connect(descendingAction, SIGNAL(triggered()), this, SLOT(sortDescending()));
    connect(insertAction, SIGNAL(triggered()), this, SLOT(insertItem()));
    connect(removeAction, SIGNAL(triggered()), this, SLOT(removeItem()));
    connect(listView->selectionModel(),
            SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),
            this, SLOT(updateMenus(const QModelIndex &)));

    updateMenus(listView->selectionModel()->currentIndex());

    setCentralWidget(listView);
    setWindowTitle(tr("List View"));
}

void MainWindow::sortAscending()
{
    model->sort(0, Qt::AscendingOrder);
}

void MainWindow::sortDescending()
{
    model->sort(0, Qt::DescendingOrder);
}

void MainWindow::insertItem()
{
    QModelIndex currentIndex = listView->currentIndex();
    if (!currentIndex.isValid())
        return;

    QString itemText = QInputDialog::getText(this, tr("Insert Item"),
        tr("Input text for the new item:"));

    if (itemText.isNull())
        return;

    if (model->insertRow(currentIndex.row(), QModelIndex())) {
        QModelIndex newIndex = model->index(currentIndex.row(), 0, QModelIndex());
        model->setData(newIndex, itemText, Qt::EditRole);

        QString toolTipText = tr("Tooltip:") + itemText;
        QString statusTipText = tr("Status tip:") + itemText;
        QString whatsThisText = tr("What's This?:") + itemText;
        model->setData(newIndex, toolTipText, Qt::ToolTipRole);
        model->setData(newIndex, toolTipText, Qt::StatusTipRole);
        model->setData(newIndex, whatsThisText, Qt::WhatsThisRole);
    }
}

void MainWindow::removeItem()
{
    QModelIndex currentIndex = listView->currentIndex();
    if (!currentIndex.isValid())
        return;

    model->removeRow(currentIndex.row(), QModelIndex());
}

void MainWindow::updateMenus(const QModelIndex &current)
{
    insertAction->setEnabled(current.isValid());
    removeAction->setEnabled(current.isValid());
}
