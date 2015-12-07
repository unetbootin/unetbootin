/****************************************************************************
**
** Copyright (C) 2004-2008 Trolltech ASA. All rights reserved.
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

/*
  window.cpp

  A minimal subclass of QTableView with slots to allow the selection model
  to be monitored.
*/

#include <QAbstractItemModel>
#include <QItemSelection>
#include <QItemSelectionModel>
#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>

#include "model.h"
#include "window.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Selected Items in a Table Model");

    model = new TableModel(8, 4, this);

    table = new QTableView(this);
    table->setModel(model);

    QMenu *actionMenu = new QMenu(tr("&Actions"), this);
    QAction *fillAction = actionMenu->addAction(tr("&Fill Selection"));
    QAction *clearAction = actionMenu->addAction(tr("&Clear Selection"));
    QAction *selectAllAction = actionMenu->addAction(tr("&Select All"));
    menuBar()->addMenu(actionMenu);

    connect(fillAction, SIGNAL(triggered()), this, SLOT(fillSelection()));
    connect(clearAction, SIGNAL(triggered()), this, SLOT(clearSelection()));
    connect(selectAllAction, SIGNAL(triggered()), this, SLOT(selectAll()));

    selectionModel = table->selectionModel();

    statusBar();
    setCentralWidget(table);
}

void MainWindow::fillSelection()
{
//! [0]
    QModelIndexList indexes = selectionModel->selectedIndexes();
    QModelIndex index;

    foreach(index, indexes) {
        QString text = QString("(%1,%2)").arg(index.row()).arg(index.column());
        model->setData(index, text);
    }
//! [0]
}

void MainWindow::clearSelection()
{
    QModelIndexList indexes = selectionModel->selectedIndexes();
    QModelIndex index;

    foreach(index, indexes)
        model->setData(index, "");
}

void MainWindow::selectAll()
{
//! [1]
    QModelIndex parent = QModelIndex();
//! [1] //! [2]
    QModelIndex topLeft = model->index(0, 0, parent);
    QModelIndex bottomRight = model->index(model->rowCount(parent)-1,
        model->columnCount(parent)-1, parent);
//! [2]

//! [3]
    QItemSelection selection(topLeft, bottomRight);
    selectionModel->select(selection, QItemSelectionModel::Select);
//! [3]
}
