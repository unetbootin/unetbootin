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

//! [0]
#include <QtGui>

#include "mainwindow.h"
//! [0]

//! [1]
MainWindow::MainWindow()
{
    textEdit = new QTextEdit;
    setCentralWidget(textEdit);

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
    createDockWindows();

    setWindowTitle(tr("Dock Widgets"));

    newLetter();
}
//! [1]

//! [2]
void MainWindow::newLetter()
{
    textEdit->clear();

    QTextCursor cursor(textEdit->textCursor());
    cursor.movePosition(QTextCursor::Start);
    QTextFrame *topFrame = cursor.currentFrame();
    QTextFrameFormat topFrameFormat = topFrame->frameFormat();
    topFrameFormat.setPadding(16);
    topFrame->setFrameFormat(topFrameFormat);

    QTextCharFormat textFormat;
    QTextCharFormat boldFormat;
    boldFormat.setFontWeight(QFont::Bold);
    QTextCharFormat italicFormat;
    italicFormat.setFontItalic(true);

    QTextTableFormat tableFormat;
    tableFormat.setBorder(1);
    tableFormat.setCellPadding(16);
    tableFormat.setAlignment(Qt::AlignRight);
    cursor.insertTable(1, 1, tableFormat);
    cursor.insertText("The Firm", boldFormat);
    cursor.insertBlock();
    cursor.insertText("321 City Street", textFormat);
    cursor.insertBlock();
    cursor.insertText("Industry Park");
    cursor.insertBlock();
    cursor.insertText("Some Country");
    cursor.setPosition(topFrame->lastPosition());
    cursor.insertText(QDate::currentDate().toString("d MMMM yyyy"), textFormat);
    cursor.insertBlock();
    cursor.insertBlock();
    cursor.insertText("Dear ", textFormat);
    cursor.insertText("NAME", italicFormat);
    cursor.insertText(",", textFormat);
    for (int i = 0; i < 3; ++i)
        cursor.insertBlock();
    cursor.insertText(tr("Yours sincerely,"), textFormat);
    for (int i = 0; i < 3; ++i)
        cursor.insertBlock();
    cursor.insertText("The Boss", textFormat);
    cursor.insertBlock();
    cursor.insertText("ADDRESS", italicFormat);
}
//! [2]

//! [3]
void MainWindow::print()
{
#ifndef QT_NO_PRINTER
    QTextDocument *document = textEdit->document();
    QPrinter printer;

    QPrintDialog *dlg = new QPrintDialog(&printer, this);
    if (dlg->exec() != QDialog::Accepted)
        return;

    document->print(&printer);

    statusBar()->showMessage(tr("Ready"), 2000);
#endif
}
//! [3]

//! [4]
void MainWindow::save()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                        tr("Choose a file name"), ".",
                        tr("HTML (*.html *.htm)"));
    if (fileName.isEmpty())
        return;
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Dock Widgets"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return;
    }

    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    out << textEdit->toHtml();
    QApplication::restoreOverrideCursor();

    statusBar()->showMessage(tr("Saved '%1'").arg(fileName), 2000);
}
//! [4]

//! [5]
void MainWindow::undo()
{
    QTextDocument *document = textEdit->document();
    document->undo();
}
//! [5]

//! [6]
void MainWindow::insertCustomer(const QString &customer)
{
    if (customer.isEmpty())
        return;
    QStringList customerList = customer.split(", ");
    QTextDocument *document = textEdit->document();
    QTextCursor cursor = document->find("NAME");
    if (!cursor.isNull()) {
        cursor.beginEditBlock();
        cursor.insertText(customerList.at(0));
        QTextCursor oldcursor = cursor;
        cursor = document->find("ADDRESS");
        if (!cursor.isNull()) {
            for (int i = 1; i < customerList.size(); ++i) {
                cursor.insertBlock();
                cursor.insertText(customerList.at(i));
            }
            cursor.endEditBlock();
        }
        else
            oldcursor.endEditBlock();
    }
}
//! [6]

//! [7]
void MainWindow::addParagraph(const QString &paragraph)
{
    if (paragraph.isEmpty())
        return;
    QTextDocument *document = textEdit->document();
    QTextCursor cursor = document->find(tr("Yours sincerely,"));
    if (cursor.isNull())
        return;
    cursor.beginEditBlock();
    cursor.movePosition(QTextCursor::PreviousBlock, QTextCursor::MoveAnchor, 2);
    cursor.insertBlock();
    cursor.insertText(paragraph);
    cursor.insertBlock();
    cursor.endEditBlock();

}
//! [7]

void MainWindow::about()
{
   QMessageBox::about(this, tr("About Dock Widgets"),
            tr("The <b>Dock Widgets</b> example demonstrates how to "
               "use Qt's dock widgets. You can enter your own text, "
               "click a customer to add a customer name and "
               "address, and click standard paragraphs to add them."));
}

void MainWindow::createActions()
{
    newLetterAct = new QAction(QIcon(":/images/new.png"), tr("&New Letter"),
                               this);
    newLetterAct->setShortcut(tr("Ctrl+N"));
    newLetterAct->setStatusTip(tr("Create a new form letter"));
    connect(newLetterAct, SIGNAL(triggered()), this, SLOT(newLetter()));

    saveAct = new QAction(QIcon(":/images/save.png"), tr("&Save..."), this);
    saveAct->setShortcut(tr("Ctrl+S"));
    saveAct->setStatusTip(tr("Save the current form letter"));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

    printAct = new QAction(QIcon(":/images/print.png"), tr("&Print..."), this);
    printAct->setShortcut(tr("Ctrl+P"));
    printAct->setStatusTip(tr("Print the current form letter"));
    connect(printAct, SIGNAL(triggered()), this, SLOT(print()));

    undoAct = new QAction(QIcon(":/images/undo.png"), tr("&Undo"), this);
    undoAct->setShortcut(tr("Ctrl+Z"));
    undoAct->setStatusTip(tr("Undo the last editing action"));
    connect(undoAct, SIGNAL(triggered()), this, SLOT(undo()));

    quitAct = new QAction(tr("&Quit"), this);
    quitAct->setShortcut(tr("Ctrl+Q"));
    quitAct->setStatusTip(tr("Quit the application"));
    connect(quitAct, SIGNAL(triggered()), this, SLOT(close()));

    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newLetterAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(printAct);
    fileMenu->addSeparator();
    fileMenu->addAction(quitAct);

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(undoAct);

    viewMenu = menuBar()->addMenu(tr("&View"));

    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);
}

void MainWindow::createToolBars()
{
    fileToolBar = addToolBar(tr("File"));
    fileToolBar->addAction(newLetterAct);
    fileToolBar->addAction(saveAct);
    fileToolBar->addAction(printAct);

    editToolBar = addToolBar(tr("Edit"));
    editToolBar->addAction(undoAct);
}

//! [8]
void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}
//! [8]

//! [9]
void MainWindow::createDockWindows()
{
    QDockWidget *dock = new QDockWidget(tr("Customers"), this);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    customerList = new QListWidget(dock);
    customerList->addItems(QStringList()
            << "John Doe, Harmony Enterprises, 12 Lakeside, Ambleton"
            << "Jane Doe, Memorabilia, 23 Watersedge, Beaton"
            << "Tammy Shea, Tiblanka, 38 Sea Views, Carlton"
            << "Tim Sheen, Caraba Gifts, 48 Ocean Way, Deal"
            << "Sol Harvey, Chicos Coffee, 53 New Springs, Eccleston"
            << "Sally Hobart, Tiroli Tea, 67 Long River, Fedula");
    dock->setWidget(customerList);
    addDockWidget(Qt::RightDockWidgetArea, dock);
    viewMenu->addAction(dock->toggleViewAction());

    dock = new QDockWidget(tr("Paragraphs"), this);
    paragraphsList = new QListWidget(dock);
    paragraphsList->addItems(QStringList()
            << "Thank you for your payment which we have received today."
            << "Your order has been dispatched and should be with you "
               "within 28 days."
            << "We have dispatched those items that were in stock. The "
               "rest of your order will be dispatched once all the "
               "remaining items have arrived at our warehouse. No "
               "additional shipping charges will be made."
            << "You made a small overpayment (less than $5) which we "
               "will keep on account for you, or return at your request."
            << "You made a small underpayment (less than $1), but we have "
               "sent your order anyway. We'll add this underpayment to "
               "your next bill."
            << "Unfortunately you did not send enough money. Please remit "
               "an additional $. Your order will be dispatched as soon as "
               "the complete amount has been received."
            << "You made an overpayment (more than $5). Do you wish to "
               "buy more items, or should we return the excess to you?");
    dock->setWidget(paragraphsList);
    addDockWidget(Qt::RightDockWidgetArea, dock);
    viewMenu->addAction(dock->toggleViewAction());

    connect(customerList, SIGNAL(currentTextChanged(const QString &)),
            this, SLOT(insertCustomer(const QString &)));
    connect(paragraphsList, SIGNAL(currentTextChanged(const QString &)),
            this, SLOT(addParagraph(const QString &)));
}
//! [9]
