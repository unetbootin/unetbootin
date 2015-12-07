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
#include <QStatusBar>

#include "model.h"
#include "window.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Selected items in a table model");

    model = new TableModel(8, 4, this);

    table = new QTableView(this);
    table->setModel(model);

    selectionModel = table->selectionModel();
    connect(selectionModel,
        SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
        this, SLOT(updateSelection(const QItemSelection &, const QItemSelection &)));
    connect(selectionModel,
        SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),
        this, SLOT(changeCurrent(const QModelIndex &, const QModelIndex &)));

    statusBar();
    setCentralWidget(table);
}

//! [0]
void MainWindow::updateSelection(const QItemSelection &selected,
    const QItemSelection &deselected)
{
    QModelIndex index;
    QModelIndexList items = selected.indexes();

    foreach (index, items) {
        QString text = QString("(%1,%2)").arg(index.row()).arg(index.column());
        model->setData(index, text);
//! [0] //! [1]
    }
//! [1]

//! [2]
    items = deselected.indexes();

    foreach (index, items)
        model->setData(index, "");
}
//! [2]

//! [3]
void MainWindow::changeCurrent(const QModelIndex &current,
    const QModelIndex &previous)
{
    statusBar()->showMessage(
        tr("Moved from (%1,%2) to (%3,%4)")
            .arg(previous.row()).arg(previous.column())
            .arg(current.row()).arg(current.column()));
}
//! [3]
