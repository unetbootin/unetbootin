/****************************************************************************
**
** Copyright (C) 2006-2008 Trolltech ASA. All rights reserved.
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

#include "robot.h"

RobotPart::RobotPart(QGraphicsItem *parent)
    : QGraphicsItem(parent), color(Qt::lightGray), dragOver(false)
{
    setAcceptDrops(true);
}

void RobotPart::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    if (event->mimeData()->hasColor()
        || (qgraphicsitem_cast<RobotHead *>(this) && event->mimeData()->hasImage())) {
        event->setAccepted(true);
        dragOver = true;
        update();
    } else {
        event->setAccepted(false);
    }
}

void RobotPart::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
{
    Q_UNUSED(event);
    dragOver = false;
    update();
}

void RobotPart::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    dragOver = false;
    if (event->mimeData()->hasColor())
        color = qVariantValue<QColor>(event->mimeData()->colorData());
    else if (event->mimeData()->hasImage())
        pixmap = qVariantValue<QPixmap>(event->mimeData()->imageData());
    update();
}

RobotHead::RobotHead(QGraphicsItem *parent)
    : RobotPart(parent)
{
}

QRectF RobotHead::boundingRect() const
{
    return QRectF(-15, -50, 30, 50);
}

void RobotHead::paint(QPainter *painter,
           const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    if (pixmap.isNull()) {
        painter->setBrush(dragOver ? color.light(130) : color);
        painter->drawRoundedRect(-10, -30, 20, 30, 25, 25, Qt::RelativeSize);
        painter->setBrush(Qt::white);
        painter->drawEllipse(-7, -3 - 20, 7, 7);
        painter->drawEllipse(0, -3 - 20, 7, 7);
        painter->setBrush(Qt::black);
        painter->drawEllipse(-5, -1 - 20, 2, 2);
        painter->drawEllipse(2, -1 - 20, 2, 2);
        painter->setPen(QPen(Qt::black, 2));
        painter->setBrush(Qt::NoBrush);
        painter->drawArc(-6, -2 - 20, 12, 15, 190 * 16, 160 * 16);
    } else {
        painter->scale(.2272, .2824);
        painter->drawPixmap(QPointF(-15 * 4.4, -50 * 3.54), pixmap);
    }
}

int RobotHead::type() const
{
    return Type;
}

RobotTorso::RobotTorso(QGraphicsItem *parent)
    : RobotPart(parent)
{
}

QRectF RobotTorso::boundingRect() const
{
    return QRectF(-30, -20, 60, 60);
}

void RobotTorso::paint(QPainter *painter,
           const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    
    painter->setBrush(dragOver ? color.light(130) : color);
    painter->drawRoundedRect(-20, -20, 40, 60, 25, 25, Qt::RelativeSize);
    painter->drawEllipse(-25, -20, 20, 20);
    painter->drawEllipse(5, -20, 20, 20);
    painter->drawEllipse(-20, 22, 20, 20);
    painter->drawEllipse(0, 22, 20, 20);
}

RobotLimb::RobotLimb(QGraphicsItem *parent)
    : RobotPart(parent)
{
}

QRectF RobotLimb::boundingRect() const
{
    return QRectF(-5, -5, 40, 10);
}

void RobotLimb::paint(QPainter *painter,
           const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setBrush(dragOver ? color.light(130) : color);
    painter->drawRoundedRect(boundingRect(), 50, 50, Qt::RelativeSize);
    painter->drawEllipse(-5, -5, 10, 10);
}

Robot::Robot()
{
    QGraphicsItem *torsoItem = new RobotTorso(this);    
    QGraphicsItem *headItem = new RobotHead(torsoItem);
    QGraphicsItem *upperLeftArmItem = new RobotLimb(torsoItem);
    QGraphicsItem *lowerLeftArmItem = new RobotLimb(upperLeftArmItem);
    QGraphicsItem *upperRightArmItem = new RobotLimb(torsoItem);
    QGraphicsItem *lowerRightArmItem = new RobotLimb(upperRightArmItem);
    QGraphicsItem *upperRightLegItem = new RobotLimb(torsoItem);
    QGraphicsItem *lowerRightLegItem = new RobotLimb(upperRightLegItem);
    QGraphicsItem *upperLeftLegItem = new RobotLimb(torsoItem);
    QGraphicsItem *lowerLeftLegItem = new RobotLimb(upperLeftLegItem);
    
    headItem->setPos(0, -18);
    upperLeftArmItem->setPos(-15, -10);
    lowerLeftArmItem->setPos(30, 0);
    upperRightArmItem->setPos(15, -10);
    lowerRightArmItem->setPos(30, 0);
    upperRightLegItem->setPos(10, 32);
    lowerRightLegItem->setPos(30, 0);
    upperLeftLegItem->setPos(-10, 32);
    lowerLeftLegItem->setPos(30, 0);

    timeLine = new QTimeLine;

    QGraphicsItemAnimation *headAnimation = new QGraphicsItemAnimation;
    headAnimation->setItem(headItem);
    headAnimation->setTimeLine(timeLine);
    headAnimation->setRotationAt(0, 20);
    headAnimation->setRotationAt(1, -20);
    headAnimation->setScaleAt(1, 1.1, 1.1);

    QGraphicsItemAnimation *upperLeftArmAnimation = new QGraphicsItemAnimation;
    upperLeftArmAnimation->setItem(upperLeftArmItem);
    upperLeftArmAnimation->setTimeLine(timeLine);
    upperLeftArmAnimation->setRotationAt(0, 190);
    upperLeftArmAnimation->setRotationAt(1, 180);

    QGraphicsItemAnimation *lowerLeftArmAnimation = new QGraphicsItemAnimation;
    lowerLeftArmAnimation->setItem(lowerLeftArmItem);
    lowerLeftArmAnimation->setTimeLine(timeLine);
    lowerLeftArmAnimation->setRotationAt(0, 50);
    lowerLeftArmAnimation->setRotationAt(1, 10);
    
    QGraphicsItemAnimation *upperRightArmAnimation = new QGraphicsItemAnimation;
    upperRightArmAnimation->setItem(upperRightArmItem);
    upperRightArmAnimation->setTimeLine(timeLine);
    upperRightArmAnimation->setRotationAt(0, 300);
    upperRightArmAnimation->setRotationAt(1, 310);

    QGraphicsItemAnimation *lowerRightArmAnimation = new QGraphicsItemAnimation;
    lowerRightArmAnimation->setItem(lowerRightArmItem);
    lowerRightArmAnimation->setTimeLine(timeLine);
    lowerRightArmAnimation->setRotationAt(0, 0);
    lowerRightArmAnimation->setRotationAt(1, -70);

    QGraphicsItemAnimation *upperLeftLegAnimation = new QGraphicsItemAnimation;
    upperLeftLegAnimation->setItem(upperLeftLegItem);
    upperLeftLegAnimation->setTimeLine(timeLine);
    upperLeftLegAnimation->setRotationAt(0, 150);
    upperLeftLegAnimation->setRotationAt(1, 80);

    QGraphicsItemAnimation *lowerLeftLegAnimation = new QGraphicsItemAnimation;
    lowerLeftLegAnimation->setItem(lowerLeftLegItem);
    lowerLeftLegAnimation->setTimeLine(timeLine);
    lowerLeftLegAnimation->setRotationAt(0, 70);
    lowerLeftLegAnimation->setRotationAt(1, 10);

    QGraphicsItemAnimation *upperRightLegAnimation = new QGraphicsItemAnimation;
    upperRightLegAnimation->setItem(upperRightLegItem);
    upperRightLegAnimation->setTimeLine(timeLine);
    upperRightLegAnimation->setRotationAt(0, 40);
    upperRightLegAnimation->setRotationAt(1, 120);
    
    QGraphicsItemAnimation *lowerRightLegAnimation = new QGraphicsItemAnimation;
    lowerRightLegAnimation->setItem(lowerRightLegItem);
    lowerRightLegAnimation->setTimeLine(timeLine);
    lowerRightLegAnimation->setRotationAt(0, 10);
    lowerRightLegAnimation->setRotationAt(1, 50);
    
    QGraphicsItemAnimation *torsoAnimation = new QGraphicsItemAnimation;
    torsoAnimation->setItem(torsoItem);
    torsoAnimation->setTimeLine(timeLine);
    torsoAnimation->setRotationAt(0, 5);
    torsoAnimation->setRotationAt(1, -20);

    timeLine->setUpdateInterval(1000 / 25);
    timeLine->setCurveShape(QTimeLine::SineCurve);
    timeLine->setLoopCount(0);
    timeLine->setDuration(2000);
    timeLine->start();
}

Robot::~Robot()
{
    delete timeLine;
}

QRectF Robot::boundingRect() const
{
    return QRectF();
}

void Robot::paint(QPainter *painter,
                  const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(painter);
    Q_UNUSED(option);
    Q_UNUSED(widget);
}
