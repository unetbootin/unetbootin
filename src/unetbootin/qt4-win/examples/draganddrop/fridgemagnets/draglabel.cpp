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

#include "draglabel.h"

//! [0]
DragLabel::DragLabel(const QString &text, QWidget *parent)
    : QLabel(parent)
{
    QFontMetrics metric(font());
    QSize size = metric.size(Qt::TextSingleLine, text);

    QImage image(size.width() + 12, size.height() + 12,
                 QImage::Format_ARGB32_Premultiplied);
    image.fill(qRgba(0, 0, 0, 0));

    QFont font;
    font.setStyleStrategy(QFont::ForceOutline);
//! [0]

//! [1]
    QPainter painter;
    painter.begin(&image);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(Qt::white);
    painter.drawRoundedRect(QRectF(0.5, 0.5, image.width()-1, image.height()-1),
                            25, 25, Qt::RelativeSize);

    painter.setFont(font);
    painter.setBrush(Qt::black);
    painter.drawText(QRect(QPoint(6, 6), size), Qt::AlignCenter, text);
    painter.end();
//! [1]

//! [2]
    setPixmap(QPixmap::fromImage(image));
    labelText = text;
}
//! [2]

//! [3]
void DragLabel::mousePressEvent(QMouseEvent *event)
//! [3]
{
//! [4]
    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    dataStream << labelText << QPoint(event->pos() - rect().topLeft());
//! [4]

//! [5]
    QMimeData *mimeData = new QMimeData;
    mimeData->setData("application/x-fridgemagnet", itemData);
    mimeData->setText(labelText);
//! [5]

//! [6]
    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->setHotSpot(event->pos() - rect().topLeft());
    drag->setPixmap(*pixmap());

    hide();
//! [6]

//! [7]
    if (drag->exec(Qt::MoveAction | Qt::CopyAction, Qt::CopyAction) == Qt::MoveAction)
        close();
    else
        show();
}
//! [7]
