/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt3Support module of the Qt Toolkit.
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

#include "q3painter.h"
#include "qpaintengine.h"

#include <private/qpainter_p.h>

QT_BEGIN_NAMESPACE

/*!
    \class Q3Painter q3painter.h
    \brief The Q3Painter class is a Qt 3 compatibility wrapper for QPainter.

    \compat

    Prior to Qt 4, QPainter specialized the pen drawing for rectangle
    based functions (in particular: drawRect, drawEllipse,
    drawRoundRect, drawArc, drawChord and drawPie). When stroking a
    rectangle of width 10, the pen would draw a rectangle of width 10.
    Drawing a polygon defined by the corner points of the same
    rectangle the stroke would have a width of 11.

    The reason for this is best explained using the picture below:

    \img q3painter_rationale.png

    As we can see, stroking the rectangle so it gets a width of 10,
    means the pen is drawn on a rectangle on width 9. The polygon,
    however follows a consistent model.

    In Qt 4, all rectangle based functions have changed to follow the
    polygon approach, which means that the rectangle defines the size of
    the fill, and the pen follows the edges of the shape. For pen widths
    of 0 and 1 this means that the stroke will be inside the shape on the
    left and the top and outside on the bottom and right.

    The reason for the change in Qt 4 is so that we provide consistency
    for all drawing functions even with complex transformations.
*/

/*!
    \fn Q3Painter::Q3Painter()

    Constructs a Q3Painter.
*/

/*!
    \fn Q3Painter::Q3Painter(QPaintDevice *pdev)

    Constructs a Q3Painter that operates on device \a pdev.
*/

/*!
    \internal
*/
QRect Q3Painter::adjustedRectangle(const QRect &r)
{
    QRect rect = r.normalized();
    int subtract = d_func()->rectSubtraction();
    if (subtract != 0)
        rect.setSize(QSize(rect.width() - subtract, rect.height() - subtract));
    return rect;
}


/*!
    \fn void Q3Painter::drawRect(int x, int y, int w, int h)

    \overload

    Draws the rectangle that fits inside the bounds specified by \a x,
    \a y, \a w and \a h using the current pen and brush.
*/

/*!
    \fn void Q3Painter::drawRect(const QRect &r)

    Draws a rectangle that fits inside the rectangle \a r using the
    current pen and brush.

*/



/*!
    \fn Q3Painter::drawEllipse(const QRect &r)

    Draws the ellipse that fits inside the bounds \a r using the
    current pen and brush.

*/

/*!
    \fn Q3Painter::drawEllipse(int x, int y, int width, int height)

    \overload

    Draws an ellipse that fits inside the bounds specified by \a x,
    \a y, \a width and \a height using the current pen and brush.

*/

/*!
    \fn void Q3Painter::drawPie(int x, int y, int w, int h, int
    startAngle, int spanAngle)

    \overload

    Draws a pie segment that fits inside the bounds (\a{x}, \a{y},
    \a{w}, \a{h}) with the given \a startAngle and \a spanAngle.
*/

/*!
    \fn void Q3Painter::drawPie(const QRect &r, int a, int alen)

    Draws a pie defined by the rectangle \a r, the start angle \a a
    and the arc length \a alen.

    The pie is filled with the current brush().

    The angles \a a and \a alen are 1/16th of a degree, i.e. a full
    circle equals 5760 (16*360). Positive values of \a a and \a alen
    mean counter-clockwise while negative values mean the clockwise
    direction. Zero degrees is at the 3 o'clock position.

    \sa drawArc(), drawChord()
*/

/*!
    \fn void Q3Painter::drawArc(int x, int y, int w, int h, int
    startAngle, int spanAngle)

    \overload

    Draws the arc that fits inside the rectangle (\a{x}, \a{y}, \a{w},
    \a{h}), with the given \a startAngle and \a spanAngle.
*/

/*!
    \fn void Q3Painter::drawArc(const QRect &r, int a, int alen)

    Draws an arc defined by the rectangle \a r, the start angle \a a
    and the arc length \a alen.

    The angles \a a and \a alen are 1/16th of a degree, i.e. a full
    circle equals 5760 (16*360). Positive values of \a a and \a alen
    mean counter-clockwise while negative values mean the clockwise
    direction. Zero degrees is at the 3 o'clock position.

    Example:
    \snippet doc/src/snippets/code/src.qt3support.painting.q3painter.cpp 0

    \sa drawPie(), drawChord()
*/

/*!
    \fn void Q3Painter::drawChord(int x, int y, int w, int h, int
    startAngle, int spanAngle)

    \overload

    Draws a chord that fits inside the rectangle (\a{x}, \a{y}, \a{w},
    \a{h}) with the given \a startAngle and \a spanAngle.
*/


/*!
    \fn void Q3Painter::drawChord(const QRect &r, int a, int alen)

    Draws a chord defined by the rectangle \a r, the start angle \a a
    and the arc length \a alen.

    The chord is filled with the current brush().

    The angles \a a and \a alen are 1/16th of a degree, i.e. a full
    circle equals 5760 (16*360). Positive values of \a a and \a alen
    mean counter-clockwise while negative values mean the clockwise
    direction. Zero degrees is at the 3 o'clock position.

    \sa drawArc(), drawPie()
*/

/*!
    \fn void Q3Painter::drawRoundRect(const QRect &r, int xrnd, int yrnd)

    Draws a rounded rect that fits into the bounds \a r using the current
    pen and brush. The parameters \a xrnd and \a yrnd specifies the roundness
    in x and y direction.
*/

/*!
    \fn void Q3Painter::drawRoundRect(int x, int y, int w, int h, int xrnd, int yrnd)

    \overload

    Draws a rounded rect that fits into the bounds \a x, \a y, \a w
    and \a h using the current pen and brush. The parameters \a xrnd
    and \a yrnd specifies the roundness in x and y direction.
*/

QT_END_NAMESPACE
