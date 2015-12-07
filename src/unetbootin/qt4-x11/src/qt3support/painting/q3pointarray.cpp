/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt3Support module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "q3pointarray.h"
#include "private/qbezier_p.h"
#include "private/qpainterpath_p.h"

QT_BEGIN_NAMESPACE

/*!
    \class Q3PointArray
    The Q3PointArray class provides an array of points.

    \compat

    Q3PointArray is a QPolygon subclass that provides functions
    to make it more source compatible with the \c QPointArray class
    in Qt 3.

    In Qt 4, we recommend that you use QPainterPath for representing
    arcs, ellipses, and Bezier curves, rather than QPolygon.
*/

/*!
    Sets the points of the array to those describing an arc of an
    ellipse with size, width \a w by height \a h, and position (\a x,
    \a y), starting from angle \a a1 and spanning by angle \a a2. The
    resulting array has sufficient resolution for pixel accuracy (see
    the overloaded function which takes an additional QMatrix
    parameter).

    Angles are specified in 16ths of a degree, i.e. a full circle
    equals 5760 (16*360). Positive values mean counter-clockwise,
    whereas negative values mean the clockwise direction. Zero degrees
    is at the 3 o'clock position.
*/
#ifndef QT_NO_WMATRIX
void Q3PointArray::makeArc(int x, int y, int w, int h, int a1, int a2)
{
    QRectF r(x, y, w, h);
    QPointF startPoint;
    qt_find_ellipse_coords(r, a1 / 16.0, a2 / 16.0, &startPoint, 0);

    QPainterPath path(startPoint);
    path.arcTo(r, a1 / 16.0, a2 / 16.0);

    if (path.isEmpty())
        *this = Q3PointArray();
    else
        *this = path.toSubpathPolygons().at(0).toPolygon();
}
#endif

#ifndef QT_NO_TRANSFORMATIONS
/*!
    \overload

    Sets the points of the array to those describing an arc of an
    ellipse with width \a w and height \a h and position (\a x, \a y),
    starting from angle \a a1, and spanning angle by \a a2, and
    transformed by the matrix \a xf. The resulting array has
    sufficient resolution for pixel accuracy.

    Angles are specified in 16ths of a degree, i.e. a full circle
    equals 5760 (16 * 360). Positive values mean counter-clockwise,
    whereas negative values mean the clockwise direction. Zero
    degrees is at the 3 o'clock position.
*/
void Q3PointArray::makeArc(int x, int y, int w, int h, int a1, int a2, const QMatrix &xf)
{
    QRectF r(x, y, w, h);
    QPointF startPoint;
    qt_find_ellipse_coords(r, a1 / 16.0, a2 / 16.0, &startPoint, 0);

    QPainterPath path(startPoint);
    path.arcTo(r, a1 / 16.0, a2 / 16.0);
    path = path * xf;
    if (path.isEmpty())
        *this = Q3PointArray();
    else
        *this = path.toSubpathPolygons().at(0).toPolygon();
}

#endif // QT_NO_TRANSFORMATIONS

/*!
    \fn Q3PointArray::Q3PointArray()

    Constructs an empty Q3PointArray.
*/

/*!
    \fn Q3PointArray::Q3PointArray(const QRect &r, bool closed)

    Constructs a point array from the rectangle \a r.

    If \a closed is false, then the point array just contains the
    following four points of the rectangle ordered clockwise. The
    bottom-right point is located at (r.x() + r.width(), r.y() +
    r.height()).
*/

/*!
    \fn Q3PointArray::Q3PointArray(const QPolygon& other)

    Constructs a copy of \a other.
*/

/*!
    \fn Q3PointArray Q3PointArray::copy() const

    Returns a copy of this Q3PointArray.
*/

/*!
    \fn bool Q3PointArray::isNull()

    Returns isEmpty(). Use isEmpty() instead.
*/

/*!
    Sets the points of the array to those describing an ellipse with
    size, width \a w by height \a h, and position (\a x, \a y).

    The returned array has sufficient resolution for use as pixels.
*/
void Q3PointArray::makeEllipse(int x, int y, int w, int h)
{
    QPainterPath path;
    path.addEllipse(x, y, w, h);
    *this = path.toSubpathPolygons().at(0).toPolygon();
}

#ifndef QT_NO_BEZIER

/*!
    Returns the Bezier points for the four control points in this
    array.
*/
Q3PointArray Q3PointArray::cubicBezier() const
{
    if (size() != 4) {
	qWarning( "Q3PointArray::bezier: The array must have 4 control points" );
        return QPolygon();
    }
    QPolygonF polygon = QBezier::fromPoints(at(0), at(1), at(2), at(3)).toPolygon();
    return polygon.toPolygon();
}
#endif //QT_NO_BEZIER

QT_END_NAMESPACE
