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

#include "circlewidget.h"

#include <stdlib.h>

//! [0]
CircleWidget::CircleWidget(QWidget *parent)
    : QWidget(parent)
{
    floatBased = false;
    antialiased = false;
    frameNo = 0;

    setBackgroundRole(QPalette::Base);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}
//! [0]

//! [1]
void CircleWidget::setFloatBased(bool floatBased)
{
    this->floatBased = floatBased;
    update();
}
//! [1]

//! [2]
void CircleWidget::setAntialiased(bool antialiased)
{
    this->antialiased = antialiased;
    update();
}
//! [2]

//! [3]
QSize CircleWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}
//! [3]

//! [4]
QSize CircleWidget::sizeHint() const
{
    return QSize(180, 180);
}
//! [4]

//! [5]
void CircleWidget::nextAnimationFrame()
{
    ++frameNo;
    update();
}
//! [5]

//! [6]
void CircleWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, antialiased);
    painter.translate(width() / 2, height() / 2);
//! [6]

//! [7]
    for (int diameter = 0; diameter < 256; diameter += 9) {
        int delta = abs((frameNo % 128) - diameter / 2);
        int alpha = 255 - (delta * delta) / 4 - diameter;
//! [7] //! [8]
        if (alpha > 0) {
            painter.setPen(QPen(QColor(0, diameter / 2, 127, alpha), 3));

            if (floatBased) {
                painter.drawEllipse(QRectF(-diameter / 2.0, -diameter / 2.0,
                                           diameter, diameter));
            } else {
                painter.drawEllipse(QRect(-diameter / 2, -diameter / 2,
                                          diameter, diameter));
            }
        }
    }
}
//! [8]
