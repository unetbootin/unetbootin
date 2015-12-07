/****************************************************************************
**
** Copyright (C) 2005-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the demonstration applications of the Qt Toolkit.
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

#include "customproxy.h"

#include <QtGui>

CustomProxy::CustomProxy(QGraphicsItem *parent, Qt::WindowFlags wFlags)
    : QGraphicsProxyWidget(parent, wFlags), popupShown(false)
{
    timeLine = new QTimeLine(250, this);
    connect(timeLine, SIGNAL(valueChanged(qreal)),
            this, SLOT(updateStep(qreal)));
    connect(timeLine, SIGNAL(stateChanged(QTimeLine::State)),
            this, SLOT(stateChanged(QTimeLine::State)));
}

QRectF CustomProxy::boundingRect() const
{
    return QGraphicsProxyWidget::boundingRect().adjusted(0, 0, 10, 10);
}

void CustomProxy::paintWindowFrame(QPainter *painter, const QStyleOptionGraphicsItem *option,
                                   QWidget *widget)
{
    const QColor color(0, 0, 0, 64);
   
    QRectF r = windowFrameRect();
    QRectF right(r.right(), r.top() + 10, 10, r.height() - 10);
    QRectF bottom(r.left() + 10, r.bottom(), r.width(), 10);
    bool intersectsRight = right.intersects(option->exposedRect);
    bool intersectsBottom = bottom.intersects(option->exposedRect);
    if (intersectsRight && intersectsBottom) {
        QPainterPath path;
        path.addRect(right);
        path.addRect(bottom);
        painter->setPen(Qt::NoPen);
        painter->setBrush(color);
        painter->drawPath(path);
    } else if (intersectsBottom) {
        painter->fillRect(bottom, color);
    } else if (intersectsRight) {
        painter->fillRect(right, color);
    }

    QGraphicsProxyWidget::paintWindowFrame(painter, option, widget);
}

void CustomProxy::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    QGraphicsProxyWidget::hoverEnterEvent(event);
    scene()->setActiveWindow(this);
    if (timeLine->currentValue() != 1)
        zoomIn();
}

void CustomProxy::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    QGraphicsProxyWidget::hoverLeaveEvent(event);
    if (!popupShown && (timeLine->direction() != QTimeLine::Backward || timeLine->currentValue() != 0))
        zoomOut();
}

bool CustomProxy::sceneEventFilter(QGraphicsItem *watched, QEvent *event)
{
    if (watched->isWindow() && (event->type() == QEvent::UngrabMouse || event->type() == QEvent::GrabMouse)) {
        popupShown = watched->isVisible();
        if (!popupShown && !isUnderMouse())
            zoomOut();
    }
    return QGraphicsProxyWidget::sceneEventFilter(watched, event);
}

QVariant CustomProxy::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemChildAddedChange || change == ItemChildRemovedChange) {
        QGraphicsItem *item = qVariantValue<QGraphicsItem *>(value);
        if (change == ItemChildAddedChange)
            item->installSceneEventFilter(this);
        else
            item->removeSceneEventFilter(this);
    }
    return QGraphicsProxyWidget::itemChange(change, value);
}

void CustomProxy::updateStep(qreal step)
{
    QRectF r = boundingRect();
    setTransform(QTransform()
                 .translate(r.width() / 2, r.height() / 2)
                 .rotate(step * 30, Qt::XAxis)
                 .rotate(step * 10, Qt::YAxis)
                 .rotate(step * 5, Qt::ZAxis)
                 .scale(1 + 1.5 * step, 1 + 1.5 * step)
                 .translate(-r.width() / 2, -r.height() / 2));
}

void CustomProxy::stateChanged(QTimeLine::State state)
{
    if (state == QTimeLine::Running) {
	if (timeLine->direction() == QTimeLine::Forward)
            setCacheMode(NoCache);
    } else if (state == QTimeLine::NotRunning) {
	if (timeLine->direction() == QTimeLine::Backward)
            setCacheMode(DeviceCoordinateCache);
    }
}

void CustomProxy::zoomIn()
{
    if (timeLine->direction() != QTimeLine::Forward)
        timeLine->setDirection(QTimeLine::Forward);
    if (timeLine->state() == QTimeLine::NotRunning) 
        timeLine->start();
}

void CustomProxy::zoomOut()
{
    if (timeLine->direction() != QTimeLine::Backward)
        timeLine->setDirection(QTimeLine::Backward);
    if (timeLine->state() == QTimeLine::NotRunning) 
        timeLine->start();
}
