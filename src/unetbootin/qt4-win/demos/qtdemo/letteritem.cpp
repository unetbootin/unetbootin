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

#include <cmath>
#include "letteritem.h"
#include "colors.h"

LetterItem::LetterItem(char letter, QGraphicsScene *scene, QGraphicsItem *parent) : DemoItem(scene, parent), letter(letter)
{
    useSharedImage(QString(__FILE__) + letter);
}

LetterItem::~LetterItem()
{
}

QImage *LetterItem::createImage(const QMatrix &matrix) const
{
    QRect scaledRect = matrix.mapRect(QRect(0, 0, 25, 25));
    QImage *image = new QImage(scaledRect.width(), scaledRect.height(), QImage::Format_ARGB32_Premultiplied);
    image->fill(0);
    QPainter painter(image);
    painter.scale(matrix.m11(), matrix.m22());
    painter.setRenderHints(QPainter::TextAntialiasing | QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    painter.setPen(Qt::NoPen);
    if (Colors::useEightBitPalette){
        painter.setBrush(QColor(170, 160, 160));
        painter.drawEllipse(0, 0, 25, 25);
        painter.setFont(Colors::tickerFont());
        painter.setPen(QColor(255, 255, 255));
        painter.drawText(10, 15, QString(this->letter));
    }
    else {
        QLinearGradient brush(0, 0, 0, 25);
        brush.setSpread(QLinearGradient::PadSpread);
        brush.setColorAt(0.0, QColor(206, 246, 117, 200));
        brush.setColorAt(1.0, QColor(206, 246, 117, 60));
        painter.setBrush(brush);
        painter.drawEllipse(0, 0, 25, 25);
        painter.setFont(Colors::tickerFont());
        painter.setPen(QColor(255, 255, 255, 255));
        painter.drawText(10, 15, QString(this->letter));
    }
    return image;
}


