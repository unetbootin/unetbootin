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

#include "imageitem.h"

//! [0]
ImageItem::ImageItem(int id, const QPixmap &pixmap, QGraphicsItem *parent,
                     QGraphicsScene *scene)
    : QGraphicsPixmapItem(pixmap, parent, scene)
{
    recordId = id;
    setAcceptsHoverEvents(true);

    timeLine.setDuration(150);
    timeLine.setFrameRange(0, 150);

    connect(&timeLine, SIGNAL(frameChanged(int)), this, SLOT(setFrame(int)));
    connect(&timeLine, SIGNAL(finished()), this, SLOT(updateItemPosition()));

    adjust();
}
//! [0]

//! [1]
void ImageItem::hoverEnterEvent(QGraphicsSceneHoverEvent * /*event*/)
{
    timeLine.setDirection(QTimeLine::Forward);

    if (z != 1.0) {
        z = 1.0;
        updateItemPosition();
    }

    if (timeLine.state() == QTimeLine::NotRunning)
        timeLine.start();
}
//! [1]

//! [2]
void ImageItem::hoverLeaveEvent(QGraphicsSceneHoverEvent * /*event*/)
{
    timeLine.setDirection(QTimeLine::Backward);
    if (z != 0.0)
        z = 0.0;

    if (timeLine.state() == QTimeLine::NotRunning)
        timeLine.start();
}
//! [2]

//! [3]
void ImageItem::setFrame(int frame)
{
    adjust();
    QPointF center = boundingRect().center();

    translate(center.x(), center.y());
    scale(1 + frame / 330.0, 1 + frame / 330.0);
    translate(-center.x(), -center.y());
}
//! [3]

//! [4]
void ImageItem::adjust()
{
    QMatrix matrix;
    matrix.scale(150/ boundingRect().width(), 120/ boundingRect().height());
    setMatrix(matrix);
}
//! [4]

//! [5]
int ImageItem::id()
{
    return recordId;
}
//! [5]

//! [6]
void ImageItem::updateItemPosition()
{
    setZValue(z);
}
//! [6]


