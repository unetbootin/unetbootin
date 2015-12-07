/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtOpenGL module of the Qt Toolkit.
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

#include "qgl2pexvertexarray_p.h"

#include <private/qbezier_p.h>

void QGL2PEXVertexArray::clear()
{
    vertexArray.reset();
    vertexArrayStops.clear();
    boundingRectDirty = true;
}


QGLRect QGL2PEXVertexArray::boundingRect() const
{
    if (boundingRectDirty)
        return QGLRect(0.0, 0.0, 0.0, 0.0);
    else
        return QGLRect(minX, minY, maxX, maxY);
}

void QGL2PEXVertexArray::addPath(const QVectorPath &path, GLfloat curveInverseScale)
{
    const QPointF* const points = reinterpret_cast<const QPointF*>(path.points());
    const QPainterPath::ElementType* const elements = path.elements();

    if (boundingRectDirty) {
        minX = maxX = points[0].x();
        minY = maxY = points[0].y();
        boundingRectDirty = false;
    }

    vertexArray.add(points[0]); // The first element is always a moveTo

    do {
        if (!elements) {
//             qDebug("QVectorPath has no elements");
            // If the path has a null elements pointer, the elements implicitly
            // start with a moveTo (already added) and continue with lineTos:
            for (int i=1; i<path.elementCount(); ++i)
                lineToArray(points[i].x(), points[i].y());

            break;
        }
//         qDebug("QVectorPath has element types");

        for (int i=1; i<path.elementCount(); ++i) {
            const QPainterPath::ElementType elementType = elements[i];
            switch (elementType) {
            case QPainterPath::MoveToElement:
//                qDebug("element[%d] is a MoveToElement", i);
                vertexArrayStops.append(vertexArray.size());
                vertexArray.add(points[i]); // Add the moveTo as a new vertex
                break;
            case QPainterPath::LineToElement:
//                qDebug("element[%d] is a LineToElement", i);
                lineToArray(points[i].x(), points[i].y());
                break;
            case QPainterPath::CurveToElement:
//                qDebug("element[%d] is a CurveToElement", i);
                curveToArray(points[i], points[i+1], points[i+2], curveInverseScale);
                i+=2;
                break;
            default:
                break;
            }
        }
    } while (0);

    vertexArrayStops.append(vertexArray.size());
}

void QGL2PEXVertexArray::lineToArray(const GLfloat x, const GLfloat y)
{
    vertexArray.add(QGLPoint(x, y));

    if (x > maxX)
        maxX = x;
    else if (x < minX)
        minX = x;
    if (y > maxY)
        maxY = y;
    else if (y < minY)
        minY = y;
}

void QGL2PEXVertexArray::curveToArray(const QGLPoint &cp1, const QGLPoint &cp2, const QGLPoint &ep, GLfloat inverseScale)
{
    qreal inverseScaleHalf = inverseScale / 2;

    QBezier beziers[32];
    beziers[0] = QBezier::fromPoints(vertexArray.last(), cp1, cp2, ep);
    QBezier *b = beziers;
    while (b >= beziers) {
        // check if we can pop the top bezier curve from the stack
        qreal l = qAbs(b->x4 - b->x1) + qAbs(b->y4 - b->y1);
        qreal d;
        if (l > inverseScale) {
            d = qAbs( (b->x4 - b->x1)*(b->y1 - b->y2) - (b->y4 - b->y1)*(b->x1 - b->x2) )
                + qAbs( (b->x4 - b->x1)*(b->y1 - b->y3) - (b->y4 - b->y1)*(b->x1 - b->x3) );
            d /= l;
        } else {
            d = qAbs(b->x1 - b->x2) + qAbs(b->y1 - b->y2) +
                qAbs(b->x1 - b->x3) + qAbs(b->y1 - b->y3);
        }
        if (d < inverseScaleHalf || b == beziers + 31) {
            // good enough, we pop it off and add the endpoint
            lineToArray(b->x4, b->y4);
            --b;
        } else {
            // split, second half of the polygon goes lower into the stack
            b->split(b+1, b);
           ++b;
        }
    }
}
