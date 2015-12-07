/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
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
#include <QtWebKit>
#include "mainwindow.h"

//! [0]
MainWindow::MainWindow()
{
    createActions();
    createMenus();
    centralWidget = new Previewer(this);
    setCentralWidget(centralWidget);

    connect(centralWidget->webView, SIGNAL(loadFinished(bool)),
        this, SLOT(updateTextEdit()));
    setStartupText();
}
//! [0]

//! [1]
void MainWindow::createActions()
{
    openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcut(tr("Ctrl+O"));
    openAct->setStatusTip(tr("Open an existing HTML file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    openUrlAct = new QAction(tr("&Open URL..."), this);
    openUrlAct->setShortcut(tr("Ctrl+U"));
    openUrlAct->setStatusTip(tr("Open a URL"));
    connect(openUrlAct, SIGNAL(triggered()), this, SLOT(openUrl()));
//! [1]

    saveAct = new QAction(tr("&Save"), this);
    saveAct->setShortcut(tr("Ctrl+S"));
    saveAct->setStatusTip(tr("Save the HTML file to disk"));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setStatusTip(tr("Exit the application"));
    exitAct->setShortcut(tr("Ctrl+Q"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

//! [2]
void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(openAct);
    fileMenu->addAction(openUrlAct);
    fileMenu->addAction(saveAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);
}
//! [2]

//! [3]
void MainWindow::about()
{
    QMessageBox::about(this, tr("About Application"),
        tr("The <b>Application</b> example demonstrates how to "
           "view HTML documents using a QWebView."));
}
//! [3]

//! [4]
void MainWindow::open()
{
    QString fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty()) {
        // read from file
        QFile file(fileName);

        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox::information(this, tr("Unable to open file"),
                file.errorString());
            return;
        }

        QTextStream out(&file);
        QString output = out.readAll();

        // display contents
        centralWidget->plainTextEdit->setPlainText(output);
    }
}
//! [4]

//! [5]
void MainWindow::openUrl()
{
    bool ok;
    QString url = QInputDialog::getText(this, tr("Enter a URL"),
                  tr("URL:"), QLineEdit::Normal, "http://", &ok);

    if (ok && !url.isEmpty()) {
        centralWidget->webView->setUrl(url);
    }
}
//! [5]

//! [6]
void MainWindow::save()
{
    QString content = centralWidget->plainTextEdit->toPlainText();
    QString fileName = QFileDialog::getSaveFileName(this);

    if (!fileName.isEmpty()) {
        // save to file
        QFile file(fileName);

        if (!file.open(QIODevice::WriteOnly)) {
            QMessageBox::information(this, tr("Unable to open file"),
                file.errorString());
            return;
        }

        QTextStream in(&file);
        in << content;
    }
}
//! [6]

//! [7]
void MainWindow::updateTextEdit()
{
    QWebFrame *mainFrame = centralWidget->webView->page()->mainFrame();
    QString frameText = mainFrame->toHtml();
    centralWidget->plainTextEdit->setPlainText(frameText);
}
//! [7]

//! [8]
void MainWindow::setStartupText()
{
    QString string = "<html><body><h1>HTML Previewer</h1>"
                     " <p>This example shows you how to use QWebView to"
                     " preview HTML data written in a QPlainTextEdit.</p>"
                     " </body></html>";
    centralWidget->webView->setHtml(string);
}
//! [8]
