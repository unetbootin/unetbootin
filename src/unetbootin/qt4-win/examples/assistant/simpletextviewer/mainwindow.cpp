/****************************************************************************
**
** Copyright (C) 2005-2008 Trolltech ASA. All rights reserved.
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
#include "findfiledialog.h"

//! [0]
MainWindow::MainWindow()
{
    textViewer = new QTextEdit;
    textViewer->setReadOnly(true);
    QFile file("documentation/intro.html");
    if (file.open(QIODevice::ReadOnly))
        textViewer->setHtml(file.readAll());

    setCentralWidget(textViewer);

    createActions();
    createMenus();

    initializeAssistant();

    setWindowTitle(tr("Simple Text Viewer"));
    resize(750, 400);
}
//! [0]

//! [1]
void MainWindow::closeEvent(QCloseEvent *)
{
    if (assistantClient)
        assistantClient->closeAssistant();
}
//! [1]

void MainWindow::about()
{
    QMessageBox::about(this, tr("About Simple Text Viewer"),
                         tr("This example demonstrates how to use\n" \
                            "Qt Assistant as help system for your\n" \
                            "own application."));
}

//! [2]
void MainWindow::assistant()
{
    assistantClient->showPage(QLibraryInfo::location(QLibraryInfo::ExamplesPath) +
                                              QDir::separator() +
                                              "assistant/simpletextviewer/documentation/index.html");
}
//! [2]

//! [3]
void MainWindow::open()
{
    FindFileDialog dialog(textViewer, assistantClient);
    dialog.exec();
}
//! [3]

void MainWindow::createActions()
{
    assistantAct = new QAction(tr("Help Contents"), this);
    assistantAct->setShortcut(tr("F1"));
    connect(assistantAct, SIGNAL(triggered()), this, SLOT(assistant()));

    openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcut(tr("Ctrl+O"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    clearAct = new QAction(tr("&Clear"), this);
    clearAct->setShortcut(tr("Ctrl+C"));
    connect(clearAct, SIGNAL(triggered()), textViewer, SLOT(clear()));

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcut(tr("Ctrl+Q"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    aboutAct = new QAction(tr("&About"), this);
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void MainWindow::createMenus()
{
    fileMenu = new QMenu(tr("&File"), this);
    fileMenu->addAction(openAct);
    fileMenu->addAction(clearAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    helpMenu = new QMenu(tr("&Help"), this);
    helpMenu->addAction(assistantAct);
    helpMenu->addSeparator();
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);


    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(helpMenu);
}

//! [4]
void MainWindow::initializeAssistant()
{
    assistantClient = new QAssistantClient(QLibraryInfo::location(QLibraryInfo::BinariesPath), this);

    QStringList arguments;
    arguments << "-profile" << QString("documentation") + QDir::separator() + QString("simpletextviewer.adp");
    assistantClient->setArguments(arguments);
}
//! [4]
