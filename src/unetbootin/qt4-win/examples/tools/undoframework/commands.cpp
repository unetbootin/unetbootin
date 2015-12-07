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

#include "commands.h"
#include "diagramitem.h"

//! [0]
MoveCommand::MoveCommand(DiagramItem *diagramItem, const QPointF &oldPos,
                 QUndoCommand *parent)
    : QUndoCommand(parent)
{
    myDiagramItem = diagramItem;
    newPos = diagramItem->pos();
    myOldPos = oldPos;
}
//! [0]

//! [1]
bool MoveCommand::mergeWith(const QUndoCommand *command)
{
    const MoveCommand *moveCommand = static_cast<const MoveCommand *>(command);
    DiagramItem *item = moveCommand->myDiagramItem;

    if (myDiagramItem != item)
    return false;

    newPos = item->pos();
    setText(QObject::tr("Move %1")
        .arg(createCommandString(myDiagramItem, newPos)));

    return true;
}
//! [1]

//! [2]
void MoveCommand::undo()
{
    myDiagramItem->setPos(myOldPos);
    myDiagramItem->scene()->update();
    setText(QObject::tr("Move %1")
        .arg(createCommandString(myDiagramItem, newPos)));
}
//! [2]

//! [3]
void MoveCommand::redo()
{
    myDiagramItem->setPos(newPos);
    setText(QObject::tr("Move %1")
        .arg(createCommandString(myDiagramItem, newPos)));
}
//! [3]

//! [4]
DeleteCommand::DeleteCommand(QGraphicsScene *scene, QUndoCommand *parent)
    : QUndoCommand(parent)
{
    myGraphicsScene = scene;
    QList<QGraphicsItem *> list = myGraphicsScene->selectedItems();
    list.first()->setSelected(false);
    myDiagramItem = static_cast<DiagramItem *>(list.first());
    setText(QObject::tr("Delete %1")
        .arg(createCommandString(myDiagramItem, myDiagramItem->pos())));
}
//! [4]

//! [5]
void DeleteCommand::undo()
{
    myGraphicsScene->addItem(myDiagramItem);
    myGraphicsScene->update();
}
//! [5]

//! [6]
void DeleteCommand::redo()
{
    myGraphicsScene->removeItem(myDiagramItem);
}
//! [6]

//! [7]
AddCommand::AddCommand(DiagramItem::DiagramType addType,
                       QGraphicsScene *scene, QUndoCommand *parent)
    : QUndoCommand(parent)
{
    static int itemCount = 0;

    myGraphicsScene = scene;
    myDiagramItem = new DiagramItem(addType);
    initialPosition = QPointF((itemCount * 15) % int(scene->width()),
                              (itemCount * 15) % int(scene->height()));
    scene->update();
    ++itemCount;
    setText(QObject::tr("Add %1")
        .arg(createCommandString(myDiagramItem, initialPosition)));
}
//! [7]

//! [8]
void AddCommand::undo()
{
    myGraphicsScene->removeItem(myDiagramItem);
    myGraphicsScene->update();
}
//! [8]

//! [9]
void AddCommand::redo()
{
    myGraphicsScene->addItem(myDiagramItem);
    myDiagramItem->setPos(initialPosition);
    myGraphicsScene->clearSelection();
    myGraphicsScene->update();
}
//! [9]

QString createCommandString(DiagramItem *item, const QPointF &pos)
{
    return QObject::tr("%1 at (%2, %3)")
        .arg(item->diagramType() == DiagramItem::Box ? "Box" : "Triangle")
        .arg(pos.x()).arg(pos.y());
}
