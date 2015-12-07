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
#include "dragdropmodel.h"

MainWindow::MainWindow()
{
    QMenu *fileMenu = new QMenu(tr("&File"));

    QAction *quitAction = fileMenu->addAction(tr("E&xit"));
    quitAction->setShortcut(tr("Ctrl+Q"));

    menuBar()->addMenu(fileMenu);

//  For convenient quoting:
    QTreeView *treeView = new QTreeView(this);
    treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    treeView->setDragEnabled(true);
    treeView->setAcceptDrops(true);
    treeView->setDropIndicatorShown(true);

    this->treeView = treeView;

    connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));

    setupItems();

    setCentralWidget(treeView);
    setWindowTitle(tr("Tree View"));
}

void MainWindow::setupItems()
{
    QStringList items;
    items << tr("Widgets\tUser interface objects used to create GUI applications.")
          << tr("  QWidget\tThe basic building block for all other widgets.")
          << tr("  QDialog\tThe base class for dialog windows.")
          << tr("Tools\tUtilities and applications for Qt developers.")
          << tr("  Qt Designer\tA GUI form designer for Qt applications.")
          << tr("  Qt Assistant\tA documentation browser for Qt documentation.");

    DragDropModel *model = new DragDropModel(items, this);
    QModelIndex index = model->index(0, 0, QModelIndex());
    model->insertRows(2, 3, index);
    index = model->index(0, 0, QModelIndex());
    index = model->index(2, 0, index);
    model->setData(index, QVariant("QFrame"));
    model->removeRows(3, 2, index.parent());
    treeView->setModel(model);
}
