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

#include "imageitem.h"
#include "colors.h"

ImageItem::ImageItem(const QImage &image, int maxWidth, int maxHeight, QGraphicsScene *scene,
        QGraphicsItem *parent, bool adjustSize, float scale) : DemoItem(scene, parent)
{
    this->image = image;
    this->maxWidth = maxWidth;
    this->maxHeight = maxHeight;
    this->adjustSize = adjustSize;
    this->scale = scale;
}

QImage *ImageItem::createImage(const QMatrix &matrix) const
{
    QImage *original = new QImage(image);
    if (original->isNull()){
        return original; // nothing we can do about it...
    }

    QPoint size = matrix.map(QPoint(this->maxWidth, this->maxHeight));
    float w = size.x(); // x, y is the used as width, height
    float h = size.y();

    // Optimization: if image is smaller than maximum allowed size, just return the loaded image
    if (original->size().height() <= h && original->size().width() <= w && !this->adjustSize && this->scale == 1)
        return original;

    // Calculate what the size of the final image will be:
    w = qMin(w, float(original->size().width()) * this->scale);
    h = qMin(h, float(original->size().height()) * this->scale);

    float adjustx = 1.0f;
    float adjusty = 1.0f;
    if (this->adjustSize){
        adjustx = qMin(matrix.m11(), matrix.m22());
        adjusty = matrix.m22() < adjustx ? adjustx : matrix.m22();
        w *= adjustx;
        h *= adjusty;
    }

    // Create a new image with correct size, and draw original on it
    QImage *image = new QImage(int(w+2), int(h+2), QImage::Format_ARGB32_Premultiplied);
    image->fill(QColor(0, 0, 0, 0).rgba());
    QPainter painter(image);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    if (this->adjustSize)
        painter.scale(adjustx, adjusty);
    if (this->scale != 1)
       painter.scale(this->scale, this->scale);
    painter.drawImage(0, 0, *original);

    if (!this->adjustSize){
       // Blur out edges
       int blur = 30;
       if (h < original->height()){
           QLinearGradient brush1(0, h - blur, 0, h);
           brush1.setSpread(QGradient::PadSpread);
           brush1.setColorAt(0.0, QColor(0, 0, 0, 0));
           brush1.setColorAt(1.0, Colors::sceneBg1);
           painter.fillRect(0, int(h) - blur, original->width(), int(h), brush1);
       }
       if (w < original->width()){
           QLinearGradient brush2(w - blur, 0, w, 0);
           brush2.setSpread(QGradient::PadSpread);
           brush2.setColorAt(0.0, QColor(0, 0, 0, 0));
           brush2.setColorAt(1.0, Colors::sceneBg1);
           painter.fillRect(int(w) - blur, 0, int(w), original->height(), brush2);
       }
    }
    delete original;
    return image;
}
