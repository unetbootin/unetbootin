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

    fileMenu->addAction(tr("E&xit"), this, SLOT(close()),
        QKeySequence(tr("Ctrl+Q", "File|Exit")));

    QMenu *actionsMenu = new QMenu(tr("&Actions"));
    actionsMenu->addAction(tr("&Highlight List Items"),
                        this, SLOT(highlightListItems()));
    actionsMenu->addAction(tr("&Show Current List"), this, SLOT(showList()));

    QMenu *insertMenu = new QMenu(tr("&Insert"));

    insertMenu->addAction(tr("&List"), this, SLOT(insertList()),
        QKeySequence(tr("Ctrl+L", "Insert|List")));

    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(insertMenu);
    menuBar()->addMenu(actionsMenu);

    editor = new QTextEdit(this);
    document = new QTextDocument(this);
    editor->setDocument(document);

    setCentralWidget(editor);
    setWindowTitle(tr("Text Document List Items"));
}

void MainWindow::highlightListItems()
{
    QTextCursor cursor = editor->textCursor();
    QTextList *list = cursor.currentList();

    if (!list)
        return;

    cursor.beginEditBlock();
//! [0]
    for (int index = 0; index < list->count(); ++index) {
        QTextBlock listItem = list->item(index);
//! [0]
        QTextBlockFormat newBlockFormat = listItem.blockFormat();
        newBlockFormat.setBackground(Qt::lightGray);
        QTextCursor itemCursor = cursor;
        itemCursor.setPosition(listItem.position());
        //itemCursor.movePosition(QTextCursor::StartOfBlock);
        itemCursor.movePosition(QTextCursor::EndOfBlock,
                                QTextCursor::KeepAnchor);
        itemCursor.setBlockFormat(newBlockFormat);
        /*
//! [1]
        processListItem(listItem);
//! [1]
        */
//! [2]
    }
//! [2]
    cursor.endEditBlock();
}

void MainWindow::showList()
{
    QTextCursor cursor = editor->textCursor();
    QTextFrame *frame = cursor.currentFrame();

    if (!frame)
        return;

    QTreeWidget *treeWidget = new QTreeWidget;
    treeWidget->setColumnCount(1);
    QStringList headerLabels;
    headerLabels << tr("Lists");
    treeWidget->setHeaderLabels(headerLabels);

    QTreeWidgetItem *parentItem = 0;
    QTreeWidgetItem *item;
    QTreeWidgetItem *lastItem = 0;
    parentItems.clear();
    previousItems.clear();

//! [3]
    QTextFrame::iterator it;
    for (it = frame->begin(); !(it.atEnd()); ++it) {

        QTextBlock block = it.currentBlock();

        if (block.isValid()) {

            QTextList *list = block.textList();

            if (list) {
                int index = list->itemNumber(block);
//! [3]
                if (index == 0) {
                    parentItems.append(parentItem);
                    previousItems.append(lastItem);
                    listStructures.append(list);
                    parentItem = lastItem;
                    lastItem = 0;

                    if (parentItem != 0)
                        item = new QTreeWidgetItem(parentItem, lastItem);
                    else
                        item = new QTreeWidgetItem(treeWidget, lastItem);

                } else {

                    while (parentItem != 0 && listStructures.last() != list) {
                        listStructures.pop_back();
                        parentItem = parentItems.takeLast();
                        lastItem = previousItems.takeLast();
                    }
                    if (parentItem != 0)
                        item = new QTreeWidgetItem(parentItem, lastItem);
                    else
                        item = new QTreeWidgetItem(treeWidget, lastItem);
                }
                item->setText(0, block.text());
                lastItem = item;
                /*
//! [4]
                processListItem(list, index);
//! [4]
                */
//! [5]
            }
//! [5] //! [6]
        }
//! [6] //! [7]
    }
//! [7]

    treeWidget->setWindowTitle(tr("List Contents"));
    treeWidget->show();
}

void MainWindow::insertList()
{
    QTextCursor cursor = editor->textCursor();
    cursor.beginEditBlock();

    QTextList *list = cursor.currentList();
    QTextListFormat listFormat;
    if (list)
        listFormat = list->format();

    listFormat.setStyle(QTextListFormat::ListDisc);
    listFormat.setIndent(listFormat.indent() + 1);
    cursor.insertList(listFormat);

    cursor.endEditBlock();
}
