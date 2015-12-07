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

#include <QtGui>

#include "mainwindow.h"

MainWindow::MainWindow()
{
    QMenu *fileMenu = new QMenu(tr("&File"));

    QAction *quitAction = fileMenu->addAction(tr("E&xit"));
    quitAction->setShortcut(tr("Ctrl+Q"));

    QMenu *tableMenu = new QMenu(tr("&Table"));

    QAction *tableWidthAction = tableMenu->addAction(tr("Change Table &Width"));
    QAction *tableHeightAction = tableMenu->addAction(tr("Change Table &Height"));

    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(tableMenu);

//! [0]
    tableWidget = new QTableWidget(this);
//! [0]
    tableWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);

    connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));
    connect(tableWidthAction, SIGNAL(triggered()), this, SLOT(changeWidth()));
    connect(tableHeightAction, SIGNAL(triggered()), this, SLOT(changeHeight()));

    setupTableItems();

    setCentralWidget(tableWidget);
    setWindowTitle(tr("Table Widget Resizing"));
}

void MainWindow::setupTableItems()
{
//! [1]
    tableWidget->setRowCount(10);
    tableWidget->setColumnCount(5);
//! [1]

    for (int row = 0; row < tableWidget->rowCount(); ++row) {
        for (int column = 0; column < tableWidget->columnCount(); ++column) {
//! [2]
    QTableWidgetItem *newItem = new QTableWidgetItem(tr("%1").arg(
        (row+1)*(column+1)));
    tableWidget->setItem(row, column, newItem);
//! [2]
        }
    }
}

void MainWindow::changeWidth()
{
    bool ok;

    int newWidth = QInputDialog::getInteger(this, tr("Change table width"),
        tr("Input the number of columns required (1-20):"),
        tableWidget->columnCount(), 1, 20, 1, &ok);

    if (ok)
        tableWidget->setColumnCount(newWidth);
}

void MainWindow::changeHeight()
{
    bool ok;

    int newHeight = QInputDialog::getInteger(this, tr("Change table height"),
        tr("Input the number of rows required (1-20):"),
        tableWidget->rowCount(), 1, 20, 1, &ok);

    if (ok)
        tableWidget->setRowCount(newHeight);
}
