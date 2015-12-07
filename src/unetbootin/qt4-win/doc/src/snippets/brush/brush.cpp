/****************************************************************************
**
** Copyright (C) 2005-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the documentation of the Qt Toolkit.
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

int main()
{
    QWidget anyPaintDevice;
    {
        // PEN SNIPPET
        QPainter painter;
        QPen pen(Qt::red, 2);                 // red solid line, 2 pixels wide
        painter.begin(&anyPaintDevice);   // paint something
        painter.setPen(pen);              // set the red, wide pen
        painter.drawRect(40,30, 200,100); // draw a rectangle
        painter.setPen(Qt::blue);             // set blue pen, 0 pixel width
        painter.drawLine(40,30, 240,130); // draw a diagonal in rectangle
        painter.end();                    // painting done
    }

    {
        // BRUSH SNIPPET
        QPainter painter;
        QBrush brush(Qt::yellow);           // yellow solid pattern
        painter.begin(&anyPaintDevice);   // paint something
        painter.setBrush(brush);          // set the yellow brush
        painter.setPen(Qt::NoPen);        // do not draw outline
        painter.drawRect(40,30, 200,100); // draw filled rectangle
        painter.setBrush(Qt::NoBrush);    // do not fill
        painter.setPen(Qt::black);            // set black pen, 0 pixel width
        painter.drawRect(10,10, 30,20);   // draw rectangle outline
        painter.end();                    // painting done
    }

    // LINEAR
//! [0]
    QLinearGradient linearGrad(QPointF(100, 100), QPointF(200, 200));
    linearGrad.setColorAt(0, Qt::black);
    linearGrad.setColorAt(1, Qt::white);
//! [0]

    // RADIAL
//! [1]
    QRadialGradient radialGrad(QPointF(100, 100), 100);
    radialGrad.setColorAt(0, Qt::red);
    radialGrad.setColorAt(0.5, Qt::blue);
    radialGrad.setColorAt(1, Qt::green);
//! [1]
}
