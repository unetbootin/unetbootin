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
  main.cpp

  A simple example that shows how selections can be used directly on a model.
  It shows the result of some selections made using a table view.
*/

#include <QApplication>
#include <QItemSelection>
#include <QItemSelectionModel>
#include <QTableView>

#include "model.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

//! [0]
    TableModel *model = new TableModel(8, 4, &app);

    QTableView *table = new QTableView(0);
    table->setModel(model);

    QItemSelectionModel *selectionModel = table->selectionModel();
//! [0] //! [1]
    QModelIndex topLeft;
    QModelIndex bottomRight;

    topLeft = model->index(0, 0, QModelIndex());
    bottomRight = model->index(5, 2, QModelIndex());
//! [1]

//! [2]
    QItemSelection selection(topLeft, bottomRight);
    selectionModel->select(selection, QItemSelectionModel::Select);
//! [2]

//! [3]
    QItemSelection toggleSelection;

    topLeft = model->index(2, 1, QModelIndex());
    bottomRight = model->index(7, 3, QModelIndex());
    toggleSelection.select(topLeft, bottomRight);

    selectionModel->select(toggleSelection, QItemSelectionModel::Toggle);
//! [3]

//! [4]
    QItemSelection columnSelection;

    topLeft = model->index(0, 1, QModelIndex());
    bottomRight = model->index(0, 2, QModelIndex());

    columnSelection.select(topLeft, bottomRight);

    selectionModel->select(columnSelection,
        QItemSelectionModel::Select | QItemSelectionModel::Columns);

    QItemSelection rowSelection;

    topLeft = model->index(0, 0, QModelIndex());
    bottomRight = model->index(1, 0, QModelIndex());

    rowSelection.select(topLeft, bottomRight);

    selectionModel->select(rowSelection,
        QItemSelectionModel::Select | QItemSelectionModel::Rows);
//! [4]

    table->setWindowTitle("Selected items in a table model");
    table->show();
    table->resize(460, 280);
    return app.exec();
}
