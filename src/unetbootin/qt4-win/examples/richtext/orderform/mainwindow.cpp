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

#include "detailsdialog.h"
#include "mainwindow.h"

//! [0]
MainWindow::MainWindow()
{
    QMenu *fileMenu = new QMenu(tr("&File"), this);
    QAction *newAction = fileMenu->addAction(tr("&New..."));
    newAction->setShortcut(tr("Ctrl+N"));
    printAction = fileMenu->addAction(tr("&Print..."), this, SLOT(printFile()));
    printAction->setShortcut(tr("Ctrl+P"));
    printAction->setEnabled(false);
    QAction *quitAction = fileMenu->addAction(tr("E&xit"));
    quitAction->setShortcut(tr("Ctrl+Q"));
    menuBar()->addMenu(fileMenu);

    letters = new QTabWidget;

    connect(newAction, SIGNAL(triggered()), this, SLOT(openDialog()));
    connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));

    setCentralWidget(letters);
    setWindowTitle(tr("Order Form"));
}
//! [0]

//! [1]
void MainWindow::createLetter(const QString &name, const QString &address,
                              QList<QPair<QString,int> > orderItems,
                              bool sendOffers)
{
    QTextEdit *editor = new QTextEdit;
    int tabIndex = letters->addTab(editor, name);
    letters->setCurrentIndex(tabIndex);
//! [1]

//! [2]
    QTextCursor cursor(editor->textCursor());
    cursor.movePosition(QTextCursor::Start);
//! [2] //! [3]
    QTextFrame *topFrame = cursor.currentFrame();
    QTextFrameFormat topFrameFormat = topFrame->frameFormat();
    topFrameFormat.setPadding(16);
    topFrame->setFrameFormat(topFrameFormat);

    QTextCharFormat textFormat;
    QTextCharFormat boldFormat;
    boldFormat.setFontWeight(QFont::Bold);

    QTextFrameFormat referenceFrameFormat;
    referenceFrameFormat.setBorder(1);
    referenceFrameFormat.setPadding(8);
    referenceFrameFormat.setPosition(QTextFrameFormat::FloatRight);
    referenceFrameFormat.setWidth(QTextLength(QTextLength::PercentageLength, 40));
    cursor.insertFrame(referenceFrameFormat);

    cursor.insertText("A company", boldFormat);
    cursor.insertBlock();
    cursor.insertText("321 City Street");
    cursor.insertBlock();
    cursor.insertText("Industry Park");
    cursor.insertBlock();
    cursor.insertText("Another country");
//! [3]

//! [4]
    cursor.setPosition(topFrame->lastPosition());

    cursor.insertText(name, textFormat);
    QString line;
    foreach (line, address.split("\n")) {
        cursor.insertBlock();
        cursor.insertText(line);
    }
//! [4] //! [5]
    cursor.insertBlock();
    cursor.insertBlock();

    QDate date = QDate::currentDate();
    cursor.insertText(tr("Date: %1").arg(date.toString("d MMMM yyyy")),
                      textFormat);
    cursor.insertBlock();

    QTextFrameFormat bodyFrameFormat;
    bodyFrameFormat.setWidth(QTextLength(QTextLength::PercentageLength, 100));
    cursor.insertFrame(bodyFrameFormat);
//! [5]

//! [6]
    cursor.insertText(tr("I would like to place an order for the following "
                         "items:"), textFormat);
    cursor.insertBlock();
//! [6] //! [7]
    cursor.insertBlock();
//! [7]

//! [8]
    QTextTableFormat orderTableFormat;
    orderTableFormat.setAlignment(Qt::AlignHCenter);
    QTextTable *orderTable = cursor.insertTable(1, 2, orderTableFormat);

    QTextFrameFormat orderFrameFormat = cursor.currentFrame()->frameFormat();
    orderFrameFormat.setBorder(1);
    cursor.currentFrame()->setFrameFormat(orderFrameFormat);
//! [8]

//! [9]
    cursor = orderTable->cellAt(0, 0).firstCursorPosition();
    cursor.insertText(tr("Product"), boldFormat);
    cursor = orderTable->cellAt(0, 1).firstCursorPosition();
    cursor.insertText(tr("Quantity"), boldFormat);
//! [9]

//! [10]
    for (int i = 0; i < orderItems.count(); ++i) {
        QPair<QString,int> item = orderItems[i];
        int row = orderTable->rows();

        orderTable->insertRows(row, 1);
        cursor = orderTable->cellAt(row, 0).firstCursorPosition();
        cursor.insertText(item.first, textFormat);
        cursor = orderTable->cellAt(row, 1).firstCursorPosition();
        cursor.insertText(QString("%1").arg(item.second), textFormat);
    }
//! [10]

//! [11]
    cursor.setPosition(topFrame->lastPosition());

    cursor.insertBlock();
//! [11] //! [12]
    cursor.insertText(tr("Please update my records to take account of the "
                         "following privacy information:"));
    cursor.insertBlock();
//! [12]

//! [13]
    QTextTable *offersTable = cursor.insertTable(2, 2);

    cursor = offersTable->cellAt(0, 1).firstCursorPosition();
    cursor.insertText(tr("I want to receive more information about your "
                         "company's products and special offers."), textFormat);
    cursor = offersTable->cellAt(1, 1).firstCursorPosition();
    cursor.insertText(tr("I do not want to receive any promotional information "
                         "from your company."), textFormat);

    if (sendOffers)
        cursor = offersTable->cellAt(0, 0).firstCursorPosition();
    else
        cursor = offersTable->cellAt(1, 0).firstCursorPosition();

    cursor.insertText("X", boldFormat);
//! [13]

//! [14]
    cursor.setPosition(topFrame->lastPosition());
    cursor.insertBlock();
    cursor.insertText(tr("Sincerely,"), textFormat);
    cursor.insertBlock();
    cursor.insertBlock();
    cursor.insertBlock();
    cursor.insertText(name);

    printAction->setEnabled(true);
}
//! [14]

//! [15]
void MainWindow::createSample()
{
    DetailsDialog dialog("Dialog with default values", this);
    createLetter("Mr. Smith", "12 High Street\nSmall Town\nThis country",
                 dialog.orderItems(), true);
}
//! [15]

//! [16]
void MainWindow::openDialog()
{
    DetailsDialog dialog(tr("Enter Customer Details"), this);

    if (dialog.exec() == QDialog::Accepted)
        createLetter(dialog.senderName(), dialog.senderAddress(),
                     dialog.orderItems(), dialog.sendOffers());
}
//! [16]

//! [17]
void MainWindow::printFile()
{
#ifndef QT_NO_PRINTER
    QTextEdit *editor = static_cast<QTextEdit*>(letters->currentWidget());
//! [18]
    QPrinter printer;

    QPrintDialog *dialog = new QPrintDialog(&printer, this);
    dialog->setWindowTitle(tr("Print Document"));
    if (editor->textCursor().hasSelection())
        dialog->addEnabledOption(QAbstractPrintDialog::PrintSelection);
    if (dialog->exec() != QDialog::Accepted)
        return;
//! [18]

    editor->print(&printer);
#endif
}
//! [17]
