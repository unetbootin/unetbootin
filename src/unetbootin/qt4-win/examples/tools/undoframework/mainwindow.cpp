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

#include "mainwindow.h"
#include "diagramscene.h"
#include "diagramitem.h"
#include "commands.h"

//! [0]
MainWindow::MainWindow()
{
    undoStack = new QUndoStack();

    createActions();
    createMenus();

    connect(undoStack, SIGNAL(canRedoChanged(bool)),
            redoAction, SLOT(setEnabled(bool)));
    connect(undoStack, SIGNAL(canUndoChanged(bool)),
            undoAction, SLOT(setEnabled(bool)));
    createUndoView();

    diagramScene = new DiagramScene();
    QBrush pixmapBrush(QPixmap(":/images/cross.png").scaled(30, 30));
    diagramScene->setBackgroundBrush(pixmapBrush);
    diagramScene->setSceneRect(QRect(0, 0, 500, 500));

    connect(diagramScene, SIGNAL(itemMoved(DiagramItem *, const QPointF &)),
            this, SLOT(itemMoved(DiagramItem *, const QPointF &)));

    setWindowTitle("Undo Framework");
    QGraphicsView *view = new QGraphicsView(diagramScene);
    setCentralWidget(view);
    resize(700, 500);
}
//! [0]

//! [1]
void MainWindow::createUndoView()
{
    undoView = new QUndoView(undoStack);
    undoView->setWindowTitle(tr("Command List"));
    undoView->show();
    undoView->setAttribute(Qt::WA_QuitOnClose, false);
}
//! [1]

//! [2]
void MainWindow::createActions()
{
    deleteAction = new QAction(tr("&Delete Item"), this);
    deleteAction->setShortcut(tr("Del"));
    connect(deleteAction, SIGNAL(triggered()), this, SLOT(deleteItem()));
//! [2] //! [3]

//! [3] //! [4]
    addBoxAction = new QAction(tr("Add &Box"), this);
//! [4]
    addBoxAction->setShortcut(tr("Ctrl+O"));
    connect(addBoxAction, SIGNAL(triggered()), this, SLOT(addBox()));

    addTriangleAction = new QAction(tr("Add &Triangle"), this);
    addTriangleAction->setShortcut(tr("Ctrl+T"));
    connect(addTriangleAction, SIGNAL(triggered()), this, SLOT(addTriangle()));

//! [5]
    undoAction = new QAction(tr("&Undo"), this);
    undoAction->setShortcut(tr("Ctrl+Z"));
    undoAction->setEnabled(false);
    connect(undoAction, SIGNAL(triggered()), undoStack, SLOT(undo()));

    redoAction = new QAction(tr("&Redo"), this);
    QList<QKeySequence> redoShortcuts;
    redoShortcuts << tr("Ctrl+Y") << tr("Shift+Ctrl+Z");
    redoAction->setShortcuts(redoShortcuts);
    redoAction->setEnabled(false);
    connect(redoAction, SIGNAL(triggered()), undoStack, SLOT(redo()));
//! [5]

    exitAction = new QAction(tr("E&xit"), this);
    exitAction->setShortcut(tr("Ctrl+Q"));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

    aboutAction = new QAction(tr("&About"), this);
    QList<QKeySequence> aboutShortcuts;
    aboutShortcuts << tr("Ctrl+A") << tr("Ctrl+B");
    aboutAction->setShortcuts(aboutShortcuts);
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));
}

//! [6]
void MainWindow::createMenus()
{
//! [6]
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(exitAction);

//! [7]
    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(undoAction);
    editMenu->addAction(redoAction);
    editMenu->addSeparator();
    editMenu->addAction(deleteAction);
    connect(editMenu, SIGNAL(aboutToShow()),
            this, SLOT(itemMenuAboutToShow()));
    connect(editMenu, SIGNAL(aboutToHide()),
            this, SLOT(itemMenuAboutToHide()));

//! [7]
    itemMenu = menuBar()->addMenu(tr("&Item"));
    itemMenu->addAction(addBoxAction);
    itemMenu->addAction(addTriangleAction);

    helpMenu = menuBar()->addMenu(tr("&About"));
    helpMenu->addAction(aboutAction);
//! [8]
}
//! [8]

//! [9]
void MainWindow::itemMoved(DiagramItem *movedItem,
                           const QPointF &oldPosition)
{
    undoStack->push(new MoveCommand(movedItem, oldPosition));
}
//! [9]

//! [10]
void MainWindow::deleteItem()
{
    if (diagramScene->selectedItems().isEmpty())
        return;

    QUndoCommand *deleteCommand = new DeleteCommand(diagramScene);
    undoStack->push(deleteCommand);
}
//! [10]

//! [11]
void MainWindow::itemMenuAboutToHide()
{
    deleteAction->setEnabled(true);
}
//! [11]

//! [12]
void MainWindow::itemMenuAboutToShow()
{
    undoAction->setText(tr("Undo ") + undoStack->undoText());
    redoAction->setText(tr("Redo ") + undoStack->redoText());
    deleteAction->setEnabled(!diagramScene->selectedItems().isEmpty());
}
//! [12]

//! [13]
void MainWindow::addBox()
{
    QUndoCommand *addCommand = new AddCommand(DiagramItem::Box, diagramScene);
    undoStack->push(addCommand);
}
//! [13]

//! [14]
void MainWindow::addTriangle()
{
    QUndoCommand *addCommand = new AddCommand(DiagramItem::Triangle,
                                              diagramScene);
    undoStack->push(addCommand);
}
//! [14]

//! [15]
void MainWindow::about()
{
    QMessageBox::about(this, tr("About Undo"),
                       tr("The <b>Undo</b> example demonstrates how to "
                          "use Qt's undo framework."));
}
//! [15]
