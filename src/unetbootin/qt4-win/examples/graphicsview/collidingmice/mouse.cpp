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

#include "mouse.h"

#include <QGraphicsScene>
#include <QPainter>
#include <QStyleOption>

#include <math.h>

static const double Pi = 3.14159265358979323846264338327950288419717;
static double TwoPi = 2.0 * Pi;

static qreal normalizeAngle(qreal angle)
{
    while (angle < 0)
        angle += TwoPi;
    while (angle > TwoPi)
        angle -= TwoPi;
    return angle;
}

//! [0]
Mouse::Mouse()
    : angle(0), speed(0), mouseEyeDirection(0),
      color(qrand() % 256, qrand() % 256, qrand() % 256)
{
    rotate(qrand() % (360 * 16));
    startTimer(1000 / 33);
}
//! [0]

//! [1]
QRectF Mouse::boundingRect() const
{
    qreal adjust = 0.5;
    return QRectF(-18 - adjust, -22 - adjust,
                  36 + adjust, 60 + adjust);
}
//! [1]

//! [2]
QPainterPath Mouse::shape() const
{
    QPainterPath path;
    path.addRect(-10, -20, 20, 40);
    return path;
}
//! [2]

//! [3]
void Mouse::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    // Body
    painter->setBrush(color);
    painter->drawEllipse(-10, -20, 20, 40);

    // Eyes
    painter->setBrush(Qt::white);
    painter->drawEllipse(-10, -17, 8, 8);
    painter->drawEllipse(2, -17, 8, 8);

    // Nose
    painter->setBrush(Qt::black);
    painter->drawEllipse(QRectF(-2, -22, 4, 4));

    // Pupils
    painter->drawEllipse(QRectF(-8.0 + mouseEyeDirection, -17, 4, 4));
    painter->drawEllipse(QRectF(4.0 + mouseEyeDirection, -17, 4, 4));

    // Ears
    painter->setBrush(scene()->collidingItems(this).isEmpty() ? Qt::darkYellow : Qt::red);
    painter->drawEllipse(-17, -12, 16, 16);
    painter->drawEllipse(1, -12, 16, 16);

    // Tail
    QPainterPath path(QPointF(0, 20));
    path.cubicTo(-5, 22, -5, 22, 0, 25);
    path.cubicTo(5, 27, 5, 32, 0, 30);
    path.cubicTo(-5, 32, -5, 42, 0, 35);
    painter->setBrush(Qt::NoBrush);
    painter->drawPath(path);
}
//! [3]

//! [4]
void Mouse::timerEvent(QTimerEvent *)
{
//! [4]
    // Don't move too far away
//! [5]
    QLineF lineToCenter(QPointF(0, 0), mapFromScene(0, 0));
    if (lineToCenter.length() > 150) {
        qreal angleToCenter = ::acos(lineToCenter.dx() / lineToCenter.length());
        if (lineToCenter.dy() < 0)
            angleToCenter = TwoPi - angleToCenter;
        angleToCenter = normalizeAngle((Pi - angleToCenter) + Pi / 2);

        if (angleToCenter < Pi && angleToCenter > Pi / 4) {
            // Rotate left
            angle += (angle < -Pi / 2) ? 0.25 : -0.25;
        } else if (angleToCenter >= Pi && angleToCenter < (Pi + Pi / 2 + Pi / 4)) {
            // Rotate right
            angle += (angle < Pi / 2) ? 0.25 : -0.25;
        }
    } else if (::sin(angle) < 0) {
        angle += 0.25;
    } else if (::sin(angle) > 0) {
        angle -= 0.25;
//! [5] //! [6]
    }
//! [6]

    // Try not to crash with any other mice
//! [7]
    QList<QGraphicsItem *> dangerMice = scene()->items(QPolygonF()
                                                       << mapToScene(0, 0)
                                                       << mapToScene(-30, -50)
                                                       << mapToScene(30, -50));
    foreach (QGraphicsItem *item, dangerMice) {
        if (item == this)
            continue;
        
        QLineF lineToMouse(QPointF(0, 0), mapFromItem(item, 0, 0));
        qreal angleToMouse = ::acos(lineToMouse.dx() / lineToMouse.length());
        if (lineToMouse.dy() < 0)
            angleToMouse = TwoPi - angleToMouse;
        angleToMouse = normalizeAngle((Pi - angleToMouse) + Pi / 2);

        if (angleToMouse >= 0 && angleToMouse < Pi / 2) {
            // Rotate right
            angle += 0.5;
        } else if (angleToMouse <= TwoPi && angleToMouse > (TwoPi - Pi / 2)) {
            // Rotate left
            angle -= 0.5;
//! [7] //! [8]
        }
//! [8] //! [9]
    }
//! [9]

    // Add some random movement
//! [10]
    if (dangerMice.size() > 1 && (qrand() % 10) == 0) {
        if (qrand() % 1)
            angle += (qrand() % 100) / 500.0;
        else
            angle -= (qrand() % 100) / 500.0;
    }
//! [10]

//! [11]
    speed += (-50 + qrand() % 100) / 100.0;

    qreal dx = ::sin(angle) * 10;
    mouseEyeDirection = (qAbs(dx / 5) < 1) ? 0 : dx / 5;

    rotate(dx);
    setPos(mapToParent(0, -(3 + sin(speed) * 3)));
}
//! [11]
