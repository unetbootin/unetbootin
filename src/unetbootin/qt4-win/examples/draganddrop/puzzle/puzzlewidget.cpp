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

#include "puzzlewidget.h"

PuzzleWidget::PuzzleWidget(QWidget *parent)
    : QWidget(parent)
{
    setAcceptDrops(true);
    setMinimumSize(400, 400);
    setMaximumSize(400, 400);
}

void PuzzleWidget::clear()
{
    pieceLocations.clear();
    piecePixmaps.clear();
    pieceRects.clear();
    highlightedRect = QRect();
    inPlace = 0;
    update();
}

void PuzzleWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("image/x-puzzle-piece"))
        event->accept();
    else
        event->ignore();
}

void PuzzleWidget::dragLeaveEvent(QDragLeaveEvent *event)
{
    QRect updateRect = highlightedRect;
    highlightedRect = QRect();
    update(updateRect);
    event->accept();
}

void PuzzleWidget::dragMoveEvent(QDragMoveEvent *event)
{
    QRect updateRect = highlightedRect.unite(targetSquare(event->pos()));

    if (event->mimeData()->hasFormat("image/x-puzzle-piece")
        && findPiece(targetSquare(event->pos())) == -1) {

        highlightedRect = targetSquare(event->pos());
        event->setDropAction(Qt::MoveAction);
        event->accept();
    } else {
        highlightedRect = QRect();
        event->ignore();
    }

    update(updateRect);
}

void PuzzleWidget::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasFormat("image/x-puzzle-piece")
        && findPiece(targetSquare(event->pos())) == -1) {

        QByteArray pieceData = event->mimeData()->data("image/x-puzzle-piece");
        QDataStream dataStream(&pieceData, QIODevice::ReadOnly);
        QRect square = targetSquare(event->pos());
        QPixmap pixmap;
        QPoint location;
        dataStream >> pixmap >> location;

        pieceLocations.append(location);
        piecePixmaps.append(pixmap);
        pieceRects.append(square);

        highlightedRect = QRect();
        update(square);

        event->setDropAction(Qt::MoveAction);
        event->accept();

        if (location == QPoint(square.x()/80, square.y()/80)) {
            inPlace++;
            if (inPlace == 25)
                emit puzzleCompleted();
        }
    } else {
        highlightedRect = QRect();
        event->ignore();
    }
}

int PuzzleWidget::findPiece(const QRect &pieceRect) const
{
    for (int i = 0; i < pieceRects.size(); ++i) {
        if (pieceRect == pieceRects[i]) {
            return i;
        }
    }
    return -1;
}

void PuzzleWidget::mousePressEvent(QMouseEvent *event)
{
    QRect square = targetSquare(event->pos());
    int found = findPiece(square);

    if (found == -1)
        return;

    QPoint location = pieceLocations[found];
    QPixmap pixmap = piecePixmaps[found];
    pieceLocations.removeAt(found);
    piecePixmaps.removeAt(found);
    pieceRects.removeAt(found);

    if (location == QPoint(square.x()/80, square.y()/80))
        inPlace--;

    update(square);

    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);

    dataStream << pixmap << location;

    QMimeData *mimeData = new QMimeData;
    mimeData->setData("image/x-puzzle-piece", itemData);

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->setHotSpot(event->pos() - square.topLeft());
    drag->setPixmap(pixmap);

    if (!(drag->exec(Qt::MoveAction) == Qt::MoveAction)) {
        pieceLocations.insert(found, location);
        piecePixmaps.insert(found, pixmap);
        pieceRects.insert(found, square);
        update(targetSquare(event->pos()));

        if (location == QPoint(square.x()/80, square.y()/80))
            inPlace++;
    }
}

void PuzzleWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter;
    painter.begin(this);
    painter.fillRect(event->rect(), Qt::white);

    if (highlightedRect.isValid()) {
        painter.setBrush(QColor("#ffcccc"));
        painter.setPen(Qt::NoPen);
        painter.drawRect(highlightedRect.adjusted(0, 0, -1, -1));
    }

    for (int i = 0; i < pieceRects.size(); ++i) {
        painter.drawPixmap(pieceRects[i], piecePixmaps[i]);
    }
    painter.end();
}

const QRect PuzzleWidget::targetSquare(const QPoint &position) const
{
    return QRect(position.x()/80 * 80, position.y()/80 * 80, 80, 80);
}
