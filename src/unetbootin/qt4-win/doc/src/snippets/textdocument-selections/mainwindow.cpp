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

    fileMenu->addAction(tr("&Open..."), this, SLOT(openFile()),
                        QKeySequence(tr("Ctrl+O", "File|Open")));

    QAction *quitAction = fileMenu->addAction(tr("E&xit"), this, SLOT(close()));
    quitAction->setShortcut(tr("Ctrl+Q"));

    QMenu *editMenu = new QMenu(tr("&Edit"));

    cutAction = editMenu->addAction(tr("Cu&t"), this, SLOT(cutSelection()));
    cutAction->setShortcut(tr("Ctrl+X"));
    cutAction->setEnabled(false);

    copyAction = editMenu->addAction(tr("&Copy"), this, SLOT(copySelection()));
    copyAction->setShortcut(tr("Ctrl+C"));
    copyAction->setEnabled(false);

    pasteAction = editMenu->addAction(tr("&Paste"), this, SLOT(pasteSelection()));
    pasteAction->setShortcut(tr("Ctrl+V"));
    pasteAction->setEnabled(false);

    QMenu *selectMenu = new QMenu(tr("&Select"));
    selectMenu->addAction(tr("&Word"), this, SLOT(selectWord()));
    selectMenu->addAction(tr("&Line"), this, SLOT(selectLine()));
    selectMenu->addAction(tr("&Block"), this, SLOT(selectBlock()));
    selectMenu->addAction(tr("&Frame"), this, SLOT(selectFrame()));

    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(editMenu);
    menuBar()->addMenu(selectMenu);

    editor = new QTextEdit(this);
    document = new QTextDocument(this);
    editor->setDocument(document);

    connect(editor, SIGNAL(selectionChanged()), this, SLOT(updateMenus()));

    setCentralWidget(editor);
    setWindowTitle(tr("Text Document Writer"));
}

void MainWindow::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open file"), currentFile, "HTML files (*.html);;Text files (*.txt)");
    
    if (!fileName.isEmpty()) {
        QFileInfo info(fileName);
        if (info.completeSuffix() == "html") {
            QFile file(fileName);
            
            if (file.open(QFile::ReadOnly)) {
                editor->setHtml(QString(file.readAll()));
                file.close();
                currentFile = fileName;
            }
        } else if (info.completeSuffix() == "txt") {
            QFile file(fileName);
            
            if (file.open(QFile::ReadOnly)) {
                editor->setPlainText(file.readAll());
                file.close();
                currentFile = fileName;
            }
        }
    }
}

void MainWindow::cutSelection()
{
    QTextCursor cursor = editor->textCursor();
    if (cursor.hasSelection()) {
        selection = cursor.selection();
        cursor.removeSelectedText();
    }
}

void MainWindow::copySelection()
{
    QTextCursor cursor = editor->textCursor();
    if (cursor.hasSelection()) {
        selection = cursor.selection();
        cursor.clearSelection();
    }
}

void MainWindow::pasteSelection()
{
    QTextCursor cursor = editor->textCursor();
    cursor.insertFragment(selection);
}

void MainWindow::selectWord()
{
    QTextCursor cursor = editor->textCursor();
    QTextBlock block = cursor.block();

//! [0]
    cursor.beginEditBlock();
//! [1]
    cursor.movePosition(QTextCursor::StartOfWord);
    cursor.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
//! [1]
    cursor.endEditBlock();
//! [0]

    editor->setTextCursor(cursor);
}

void MainWindow::selectLine()
{
    QTextCursor cursor = editor->textCursor();
    QTextBlock block = cursor.block();

    cursor.beginEditBlock();
    cursor.movePosition(QTextCursor::StartOfLine);
    cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
    cursor.endEditBlock();

    editor->setTextCursor(cursor);
}

void MainWindow::selectBlock()
{
    QTextCursor cursor = editor->textCursor();
    QTextBlock block = cursor.block();

    cursor.beginEditBlock();
    cursor.movePosition(QTextCursor::StartOfBlock);
    cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
    cursor.endEditBlock();

    editor->setTextCursor(cursor);
}

void MainWindow::selectFrame()
{
    QTextCursor cursor = editor->textCursor();
    QTextFrame *frame = cursor.currentFrame();

    cursor.beginEditBlock();
    cursor.setPosition(frame->firstPosition());
    cursor.setPosition(frame->lastPosition(), QTextCursor::KeepAnchor);
    cursor.endEditBlock();

    editor->setTextCursor(cursor);
}

void MainWindow::updateMenus()
{
    QTextCursor cursor = editor->textCursor();
    cutAction->setEnabled(cursor.hasSelection());
    copyAction->setEnabled(cursor.hasSelection());

    pasteAction->setEnabled(!selection.isEmpty());
}
