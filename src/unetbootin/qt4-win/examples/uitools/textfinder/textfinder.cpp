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

#include <QtUiTools>
#include <QtGui>
#include "textfinder.h"

//! [0]
TextFinder::TextFinder(QWidget *parent)
    : QWidget(parent)
{
    QWidget *formWidget = loadUiFile();

//! [1]
    ui_findButton = qFindChild<QPushButton*>(this, "findButton");
    ui_textEdit = qFindChild<QTextEdit*>(this, "textEdit");
    ui_lineEdit = qFindChild<QLineEdit*>(this, "lineEdit");
//! [0] //! [1]

//! [2]
    QMetaObject::connectSlotsByName(this);
//! [2]

//! [3]
    loadTextFile();

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(formWidget);
    setLayout(layout);
    setWindowTitle(tr("Text Finder"));

    isFirstTime = true;
}
//! [3]

//! [4]
QWidget* TextFinder::loadUiFile()
{
    QUiLoader loader;

    QFile file(":/forms/textfinder.ui");
    file.open(QFile::ReadOnly);

    QWidget *formWidget = loader.load(&file, this);
    file.close();

    return formWidget;
}
//! [4]

//! [5]
void TextFinder::loadTextFile()
{
    QFile inputFile(":/forms/input.txt");
    inputFile.open(QIODevice::ReadOnly);
    QTextStream in(&inputFile);
    QString line = in.readAll();
    inputFile.close();

    ui_textEdit->append(line);
    ui_textEdit->setUndoRedoEnabled(false);
    ui_textEdit->setUndoRedoEnabled(true);
}
//! [5]

//! [6] //! [7]
void TextFinder::on_findButton_clicked()
{
    QString searchString = ui_lineEdit->text();
    QTextDocument *document = ui_textEdit->document();

    bool found = false;

    if (isFirstTime == false)
        document->undo();

    if (searchString == "") {
        QMessageBox::information(this, tr("Empty Search Field"),
                "The search field is empty. Please enter a word and click Find.");
    } else {

        QTextCursor highlightCursor(document);  
        QTextCursor cursor(document);
        
        cursor.beginEditBlock();
//! [6]

        QTextCharFormat plainFormat(highlightCursor.charFormat());
        QTextCharFormat colorFormat = plainFormat;
        colorFormat.setForeground(Qt::red);

        while (!highlightCursor.isNull() && !highlightCursor.atEnd()) {
            highlightCursor = document->find(searchString, highlightCursor, QTextDocument::FindWholeWords);

            if (!highlightCursor.isNull()) {
                found = true;
                highlightCursor.movePosition(QTextCursor::WordRight,
                                       QTextCursor::KeepAnchor);
                highlightCursor.mergeCharFormat(colorFormat);
            }
        }

//! [8]
        cursor.endEditBlock();
//! [7] //! [9]
        isFirstTime = false;

        if (found == false) {
            QMessageBox::information(this, tr("Word Not Found"),
                "Sorry, the word cannot be found.");
        }    
    }
}
//! [8] //! [9]
