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

#include "splashitem.h"

#include <QtGui/QtGui>

SplashItem::SplashItem(QGraphicsItem *parent)
    : QGraphicsWidget(parent)
{
    opacity = 1.0;

    
    timeLine = new QTimeLine(350);
    timeLine->setCurveShape(QTimeLine::EaseInCurve);
    connect(timeLine, SIGNAL(valueChanged(qreal)), this, SLOT(setValue(qreal)));

    text = tr("Welcome to the Pad Navigator Example. You can use the"
              " keyboard arrows to navigate the icons, and press enter"
              " to activate an item. Please press any key to continue.");
    resize(400, 175);
}

void SplashItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setOpacity(opacity);
    painter->setPen(QPen(Qt::black, 2));
    painter->setBrush(QColor(245, 245, 255, 220));
    painter->setClipRect(rect());
    painter->drawRoundRect(3, -100 + 3, 400 - 6, 250 - 6);

    QRectF textRect = rect().adjusted(10, 10, -10, -10);
    int flags = Qt::AlignTop | Qt::AlignLeft | Qt::TextWordWrap;

    QFont font;
    font.setPixelSize(18);
    painter->setPen(Qt::black);
    painter->setFont(font);
    painter->drawText(textRect, flags, text);
}

void SplashItem::keyPressEvent(QKeyEvent * /* event */)
{
    if (timeLine->state() == QTimeLine::NotRunning)
        timeLine->start();
}

void SplashItem::setValue(qreal value)
{
    opacity = 1 - value;
    setPos(x(), scene()->sceneRect().top() - rect().height() * value);
    if (value == 1)
        hide();
}
