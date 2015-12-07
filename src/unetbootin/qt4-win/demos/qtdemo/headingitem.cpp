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

#include "headingitem.h"
#include "colors.h"

HeadingItem::HeadingItem(const QString &text, QGraphicsScene *scene, QGraphicsItem *parent)
    : DemoItem(scene, parent)
{
    this->text = text;
    this->noSubPixeling = true;
}

QImage *HeadingItem::createImage(const QMatrix &matrix) const
{
    float sx = qMin(matrix.m11(), matrix.m22());
    float sy = matrix.m22() < sx ? sx : matrix.m22();
    QFontMetrics fm(Colors::headingFont());

    float w = fm.width(this->text) + 1;
    float h = fm.height();
    float xShadow = 3.0f;
    float yShadow = 3.0f;

    QImage *image = new QImage(int((w + xShadow) * sx), int((h + yShadow) * sy), QImage::Format_ARGB32_Premultiplied);
    image->fill(QColor(0, 0, 0, 0).rgba());
    QPainter painter(image);
    painter.setFont(Colors::headingFont());
    painter.scale(sx, sy);

    //draw shadow
    QLinearGradient brush_shadow(xShadow, yShadow, w, yShadow);
    brush_shadow.setSpread(QLinearGradient::PadSpread);
    if (Colors::useEightBitPalette)
        brush_shadow.setColorAt(0.0f, QColor(0, 0, 0));
    else
        brush_shadow.setColorAt(0.0f, QColor(0, 0, 0, 100));
    QPen pen_shadow;
    pen_shadow.setBrush(brush_shadow);
    painter.setPen(pen_shadow);
    painter.drawText(int(xShadow), int(yShadow), int(w), int(h), Qt::AlignLeft, this->text);

    // draw text
    QLinearGradient brush_text(0, 0, w, w);
    brush_text.setSpread(QLinearGradient::PadSpread);
    brush_text.setColorAt(0.0f, QColor(255, 255, 255));
    brush_text.setColorAt(0.2f, QColor(255, 255, 255));
    brush_text.setColorAt(0.5f, QColor(190, 190, 190));
    QPen pen_text;
    pen_text.setBrush(brush_text);
    painter.setPen(pen_text);
    painter.drawText(0, 0, int(w), int(h), Qt::AlignLeft, this->text);
    return image;
}


void HeadingItem::animationStarted(int)
{
    this->noSubPixeling = false;
}


void HeadingItem::animationStopped(int)
{
    this->noSubPixeling = true;
}
