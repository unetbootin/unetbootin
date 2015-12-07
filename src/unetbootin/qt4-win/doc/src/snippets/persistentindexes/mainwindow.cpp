/****************************************************************************
**
** Copyright (C) 2005-2008 Trolltech ASA. All rights reserved.
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

/*!
    The main window constructor creates and populates a model with values
    from a string list then displays the contents of the model using a
    QListView widget.
*/

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QMenu *fileMenu = new QMenu(tr("&File"));

    QAction *quitAction = fileMenu->addAction(tr("E&xit"));
    quitAction->setShortcut(tr("Ctrl+Q"));

    QMenu *itemsMenu = new QMenu(tr("&Items"));

    insertAction = itemsMenu->addAction(tr("&Insert Item"));
    removeAction = itemsMenu->addAction(tr("&Remove Item"));

    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(itemsMenu);

    QStringList numbers;
    numbers << tr("One") << tr("Two") << tr("Three") << tr("Four") << tr("Five")
            << tr("Six") << tr("Seven") << tr("Eight") << tr("Nine") << tr("Ten");

    model = new StringListModel(numbers);
    QListView *view = new QListView(this);
    view->setModel(model);

    selectionModel = view->selectionModel();

    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
    connect(insertAction, SIGNAL(triggered()), this, SLOT(insertItem()));
    connect(removeAction, SIGNAL(triggered()), this, SLOT(removeItem()));
    connect(selectionModel,
            SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),
            this, SLOT(updateMenus(const QModelIndex &)));
    
    setCentralWidget(view);
    setWindowTitle("View onto a string list model");
}

void MainWindow::insertItem()
{
    if (!selectionModel->currentIndex().isValid())
        return;

    QString itemText = QInputDialog::getText(this, tr("Insert Item"),
        tr("Input text for the new item:"));

    if (itemText.isNull())
        return;

    int row = selectionModel->currentIndex().row();

    if (model->insertRows(row, 1))
        model->setData(model->index(row, 0), itemText, Qt::EditRole);
}

void MainWindow::removeItem()
{
    if (!selectionModel->currentIndex().isValid())
        return;

    int row = selectionModel->currentIndex().row();

    model->removeRows(row, 1);
}

void MainWindow::updateMenus(const QModelIndex &currentIndex)
{
    insertAction->setEnabled(currentIndex.isValid());
    removeAction->setEnabled(currentIndex.isValid());
}
