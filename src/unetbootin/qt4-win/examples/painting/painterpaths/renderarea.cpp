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

#include "renderarea.h"

//! [0]
RenderArea::RenderArea(const QPainterPath &path, QWidget *parent)
    : QWidget(parent), path(path)
{
    penWidth = 1;
    rotationAngle = 0;
    setBackgroundRole(QPalette::Base);
}
//! [0]

//! [1]
QSize RenderArea::minimumSizeHint() const
{
    return QSize(50, 50);
}
//! [1]

//! [2]
QSize RenderArea::sizeHint() const
{
    return QSize(100, 100);
}
//! [2]

//! [3]
void RenderArea::setFillRule(Qt::FillRule rule)
{
    path.setFillRule(rule);
    update();
}
//! [3]

//! [4]
void RenderArea::setFillGradient(const QColor &color1, const QColor &color2)
{
    fillColor1 = color1;
    fillColor2 = color2;
    update();
}
//! [4]

//! [5]
void RenderArea::setPenWidth(int width)
{
    penWidth = width;
    update();
}
//! [5]

//! [6]
void RenderArea::setPenColor(const QColor &color)
{
    penColor = color;
    update();
}
//! [6]

//! [7]
void RenderArea::setRotationAngle(int degrees)
{
    rotationAngle = degrees;
    update();
}
//! [7]

//! [8]
void RenderArea::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
//! [8] //! [9]
    painter.scale(width() / 100.0, height() / 100.0);
    painter.translate(50.0, 50.0);
    painter.rotate(-rotationAngle);
    painter.translate(-50.0, -50.0);

//! [9] //! [10]
    painter.setPen(QPen(penColor, penWidth, Qt::SolidLine, Qt::RoundCap,
                        Qt::RoundJoin));
    QLinearGradient gradient(0, 0, 0, 100);
    gradient.setColorAt(0.0, fillColor1);
    gradient.setColorAt(1.0, fillColor2);
    painter.setBrush(gradient);
    painter.drawPath(path);
}
//! [10]
