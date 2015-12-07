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

#include "demotextitem.h"
#include "colors.h"

DemoTextItem::DemoTextItem(const QString &text, const QFont &font, const QColor &textColor,
                           float textWidth, QGraphicsScene *scene, QGraphicsItem *parent, TYPE type, const QColor &bgColor)
                           : DemoItem(scene, parent)
{
    this->type = type;
    this->text = text;
    this->font = font;
    this->textColor = textColor;
    this->bgColor = bgColor;
    this->textWidth = textWidth;
    this->noSubPixeling = true;
}

void DemoTextItem::setText(const QString &text)
{
    this->text = text;
    this->update();
}

QImage *DemoTextItem::createImage(const QMatrix &matrix) const
{
    if (this->type == DYNAMIC_TEXT)
        return 0;

    float sx = qMin(matrix.m11(), matrix.m22());
    float sy = matrix.m22() < sx ? sx : matrix.m22();

    QGraphicsTextItem textItem(0, 0);
    textItem.setHtml(this->text);
    textItem.setTextWidth(this->textWidth);
    textItem.setFont(this->font);
    textItem.setDefaultTextColor(this->textColor);

    float w = textItem.boundingRect().width();
    float h = textItem.boundingRect().height();
    QImage *image = new QImage(int(w * sx), int(h * sy), QImage::Format_ARGB32_Premultiplied);
    image->fill(QColor(0, 0, 0, 0).rgba());
    QPainter painter(image);
    painter.scale(sx, sy);
    QStyleOptionGraphicsItem style;
    textItem.paint(&painter, &style, 0);
    return image;
}


void DemoTextItem::animationStarted(int)
{
    this->noSubPixeling = false;
}


void DemoTextItem::animationStopped(int)
{
    this->noSubPixeling = true;
}

QRectF DemoTextItem::boundingRect() const

{
    if (this->type == STATIC_TEXT)
        return DemoItem::boundingRect();
    return QRectF(0, 0, 50, 20); // Sorry for using magic number
}


void DemoTextItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (this->type == STATIC_TEXT) {
        DemoItem::paint(painter, option, widget);
        return;
    }

    painter->setPen(this->textColor);
    painter->drawText(0, 0, this->text);
}
