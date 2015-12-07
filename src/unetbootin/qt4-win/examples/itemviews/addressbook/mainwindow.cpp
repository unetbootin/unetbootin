/****************************************************************************
**
** Copyright (C) 2008-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
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

//! [0]
MainWindow::MainWindow()
{
    addressWidget = new AddressWidget;
    setCentralWidget(addressWidget);
    createMenus();
    setWindowTitle(tr("Address Book"));
}
//! [0]

//! [1a]
void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    
    openAct = new QAction(tr("&Open..."), this);
    fileMenu->addAction(openAct);
    connect(openAct, SIGNAL(triggered()),
        this, SLOT(openFile()));
//! [1a]

    saveAct = new QAction(tr("&Save As..."), this);
    fileMenu->addAction(saveAct);
    connect(saveAct, SIGNAL(triggered()),
        this, SLOT(saveFile()));

    fileMenu->addSeparator();

    exitAct = new QAction(tr("E&xit"), this);
    fileMenu->addAction(exitAct);
    connect(exitAct, SIGNAL(triggered()),
        this, SLOT(close()));

    toolMenu = menuBar()->addMenu(tr("&Tools"));

    addAct = new QAction(tr("&Add Entry..."), this);
    toolMenu->addAction(addAct);
    connect(addAct, SIGNAL(triggered()),
        addressWidget, SLOT(addEntry()));
    
//! [1b]
    editAct = new QAction(tr("&Edit Entry..."), this);
    editAct->setEnabled(false);
    toolMenu->addAction(editAct);
    connect(editAct, SIGNAL(triggered()),
        addressWidget, SLOT(editEntry()));

    toolMenu->addSeparator();

    removeAct = new QAction(tr("&Remove Entry"), this);
    removeAct->setEnabled(false);
    toolMenu->addAction(removeAct);
    connect(removeAct, SIGNAL(triggered()),
        addressWidget, SLOT(removeEntry()));

    connect(addressWidget, SIGNAL(selectionChanged(const QItemSelection &)),
        this, SLOT(updateActions(const QItemSelection &)));
}
//! [1b]

//! [2]
void MainWindow::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty()) {
        addressWidget->readFromFile(fileName);
    }
}
//! [2]

//! [3]
void MainWindow::saveFile()
{
    QString fileName = QFileDialog::getSaveFileName(this);
    if (!fileName.isEmpty()) {
        addressWidget->writeToFile(fileName);
    }
}
//! [3]

//! [4]
void MainWindow::updateActions(const QItemSelection &selection)
{
    QModelIndexList indexes = selection.indexes();
    
    if (!indexes.isEmpty()) {
        removeAct->setEnabled(true);
        editAct->setEnabled(true);
    } else {
        removeAct->setEnabled(false);
        editAct->setEnabled(false);
    }
}
//! [4]
