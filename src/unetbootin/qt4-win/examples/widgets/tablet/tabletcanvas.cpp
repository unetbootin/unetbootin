/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
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
#include <math.h>

#include "tabletcanvas.h"

//! [0]
TabletCanvas::TabletCanvas()
{
    myBrush = QBrush();
    myPen = QPen();
    image = QImage(500, 500, QImage::Format_ARGB32);
    QPainter painter(&image);
    painter.fillRect(0, 0, 499, 499, Qt::white);
    setAutoFillBackground(true);
    deviceDown = false;
    myColor = Qt::red;
    myTabletDevice = QTabletEvent::Stylus;
    alphaChannelType = NoAlpha;
    colorSaturationType = NoSaturation;
    lineWidthType = LineWidthPressure;
}
//! [0]

//! [1]
bool TabletCanvas::saveImage(const QString &file)
{
    return image.save(file);
}
//! [1]

//! [2]
bool TabletCanvas::loadImage(const QString &file)
{
    bool success = image.load(file);

    if (success) {
        update();
        return true;
    }
    return false;
}
//! [2]

//! [3]
void TabletCanvas::tabletEvent(QTabletEvent *event)
{

    switch (event->type()) {
        case QEvent::TabletPress:
            if (!deviceDown)
                deviceDown = true;
            break;
        case QEvent::TabletRelease:
            if (deviceDown)
                deviceDown = false;
            break;
        case QEvent::TabletMove:
            polyLine[2] = polyLine[1];
            polyLine[1] = polyLine[0];
            polyLine[0] = event->pos();

            if (deviceDown) {
                updateBrush(event);
                QPainter painter(&image);
                paintImage(painter, event);
            }
            break;
        default:
            break;
    }
    update();
}
//! [3]

//! [4]
void TabletCanvas::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawImage(QPoint(0, 0), image);
}
//! [4]

//! [5]
void TabletCanvas::paintImage(QPainter &painter, QTabletEvent *event)
{
    QPoint brushAdjust(10, 10);

    switch (myTabletDevice) {
        case QTabletEvent::Stylus:
            painter.setBrush(myBrush);
            painter.setPen(myPen);
            painter.drawLine(polyLine[1], event->pos());
            break;
        case QTabletEvent::Airbrush:
            myBrush.setColor(myColor);
            myBrush.setStyle(brushPattern(event->pressure()));
            painter.setPen(Qt::NoPen);
            painter.setBrush(myBrush);

            for (int i = 0; i < 3; ++i) {
                painter.drawEllipse(QRect(polyLine[i] - brushAdjust,
                                    polyLine[i] + brushAdjust));
            }
            break;
        default:
            qWarning("Unsupported tablet device.");
    }
}
//! [5]

//! [6]
Qt::BrushStyle TabletCanvas::brushPattern(qreal value)
{
    int pattern = int((value) * 100.0) % 7;

    switch (pattern) {
        case 0:
            return Qt::SolidPattern;
        case 1:
            return Qt::Dense1Pattern;
        case 2:
            return Qt::Dense2Pattern;
        case 3:
            return Qt::Dense3Pattern;
        case 4:
            return Qt::Dense4Pattern;
        case 5:
            return Qt::Dense5Pattern;
        case 6:
            return Qt::Dense6Pattern;
        default:
            return Qt::Dense7Pattern;
    }
}
//! [6]

//! [7]
void TabletCanvas::updateBrush(QTabletEvent *event)
{
    int hue, saturation, value, alpha;
    myColor.getHsv(&hue, &saturation, &value, &alpha);

    int vValue = int(((event->yTilt() + 60.0) / 120.0) * 255);
    int hValue = int(((event->xTilt() + 60.0) / 120.0) * 255);
//! [7] //! [8]

    switch (alphaChannelType) {
        case AlphaPressure:
            myColor.setAlpha(int(event->pressure() * 255.0));
            break;
        case AlphaTilt:
            myColor.setAlpha(maximum(abs(vValue - 127), abs(hValue - 127)));
            break;
        default:
            myColor.setAlpha(255);
    }

//! [8] //! [9]
    switch (colorSaturationType) {
        case SaturationVTilt:
            myColor.setHsv(hue, vValue, value, alpha);
            break;
        case SaturationHTilt:
            myColor.setHsv(hue, hValue, value, alpha);
            break;
        case SaturationPressure:
            myColor.setHsv(hue, int(event->pressure() * 255.0), value, alpha);
            break;
        default:
            ;
    }

//! [9] //! [10]
    switch (lineWidthType) {
        case LineWidthPressure:
            myPen.setWidthF(event->pressure() * 10 + 1);
            break;
        case LineWidthTilt:
            myPen.setWidthF(maximum(abs(vValue - 127), abs(hValue - 127)) / 12);
            break;
        default:
            myPen.setWidthF(1);
    }

//! [10] //! [11]
    if (event->pointerType() == QTabletEvent::Eraser) {
        myBrush.setColor(Qt::white);
        myPen.setColor(Qt::white);
        myPen.setWidthF(event->pressure() * 10 + 1);
    } else {
        myBrush.setColor(myColor);
        myPen.setColor(myColor);
    }
}
//! [11]
