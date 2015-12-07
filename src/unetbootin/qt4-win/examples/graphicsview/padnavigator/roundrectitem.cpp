/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
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

#include "roundrectitem.h"

#include <QtGui/QtGui>

RoundRectItem::RoundRectItem(const QRectF &rect, const QBrush &brush, QWidget *embeddedWidget)
    : QGraphicsRectItem(rect),
      brush(brush),
      timeLine(75),
      lastVal(0),
      opa(1),
      proxyWidget(0)
{
    connect(&timeLine, SIGNAL(valueChanged(qreal)),
            this, SLOT(updateValue(qreal)));
    
    if (embeddedWidget) {
        proxyWidget = new QGraphicsProxyWidget(this);
        proxyWidget->setFocusPolicy(Qt::StrongFocus);
        proxyWidget->setWidget(embeddedWidget);
        proxyWidget->setGeometry(boundingRect().adjusted(25, 25, -25, -25));
    }
}

void RoundRectItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    QTransform x = painter->worldTransform();

    QLineF unit = x.map(QLineF(0, 0, 1, 1));
    if (unit.p1().x() > unit.p2().x() || unit.p1().y() > unit.p2().y()) {
        if (proxyWidget && proxyWidget->isVisible()) {
            proxyWidget->hide();
            proxyWidget->setGeometry(rect());
        }
        return;
    }

    if (proxyWidget && !proxyWidget->isVisible()) {
        proxyWidget->show();
        proxyWidget->setFocus();
    }
    if (proxyWidget && proxyWidget->pos() != QPoint())
        proxyWidget->setGeometry(boundingRect().adjusted(25, 25, -25, -25));

    painter->setOpacity(opacity());
    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor(0, 0, 0, 64));
    painter->drawRoundRect(rect().translated(2, 2));

    if (!proxyWidget) {
        QLinearGradient gradient(rect().topLeft(), rect().bottomRight());
        const QColor col = brush.color();
        gradient.setColorAt(0, col);
        gradient.setColorAt(1, col.dark(int(200 + lastVal * 50)));
        painter->setBrush(gradient);
    } else {
        painter->setBrush(brush);
    }

    painter->setPen(QPen(Qt::black, 1));
    painter->drawRoundRect(rect());
    if (!pix.isNull()) {
        painter->scale(1.95, 1.95);
        painter->drawPixmap(-pix.width() / 2, -pix.height() / 2, pix);;
    }
}

QRectF RoundRectItem::boundingRect() const
{
    qreal penW = 0.5;
    qreal shadowW = 2.0;
    return rect().adjusted(-penW, -penW, penW + shadowW, penW + shadowW);
}

void RoundRectItem::setPixmap(const QPixmap &pixmap)
{
    pix = pixmap;
    if (scene() && isVisible())
        update();
}

qreal RoundRectItem::opacity() const
{
    RoundRectItem *parent = parentItem() ? (RoundRectItem *)parentItem() : 0;
    return opa + (parent ? parent->opacity() : 0);
}

void RoundRectItem::setOpacity(qreal opacity)
{
    opa = opacity;
    update();
}

void RoundRectItem::keyPressEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat() || event->key() != Qt::Key_Return
        || (timeLine.state() == QTimeLine::Running && timeLine.direction() == QTimeLine::Forward)) {
        QGraphicsRectItem::keyPressEvent(event);
        return;
    }

    timeLine.stop();
    timeLine.setDirection(QTimeLine::Forward);
    timeLine.start();
    emit activated();
}

void RoundRectItem::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() != Qt::Key_Return) {
        QGraphicsRectItem::keyReleaseEvent(event);
        return;
    }
    timeLine.stop();
    timeLine.setDirection(QTimeLine::Backward);
    timeLine.start();
}

void RoundRectItem::updateValue(qreal value)
{
    lastVal = value;
    if (!proxyWidget)
        setTransform(QTransform().scale(1 - value / 10.0, 1 - value / 10.0));
}
