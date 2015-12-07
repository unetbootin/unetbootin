/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#include "qbezier_p.h"
#include <qdebug.h>
#include <qline.h>
#include <qpolygon.h>
#include <qvector.h>
#include <qlist.h>
#include <qmath.h>

#include <private/qnumeric_p.h>
#include <private/qmath_p.h>

QT_BEGIN_NAMESPACE

//#define QDEBUG_BEZIER

#ifdef FLOAT_ACCURACY
#define INV_EPS (1L<<23)
#else
/* The value of 1.0 / (1L<<14) is enough for most applications */
#define INV_EPS (1L<<14)
#endif

#ifndef M_SQRT2
#define M_SQRT2	1.41421356237309504880
#endif

#define log2(x) (qLn(x)/qLn(2.))

static inline qreal log4(qreal x)
{
    return qreal(0.5) * log2(x);
}

/*!
  \internal
*/
QBezier QBezier::fromPoints(const QPointF &p1, const QPointF &p2,
                            const QPointF &p3, const QPointF &p4)
{
    QBezier b;
    b.x1 = p1.x();
    b.y1 = p1.y();
    b.x2 = p2.x();
    b.y2 = p2.y();
    b.x3 = p3.x();
    b.y3 = p3.y();
    b.x4 = p4.x();
    b.y4 = p4.y();
    return b;
}

/*!
  \internal
*/
QPolygonF QBezier::toPolygon() const
{
    // flattening is done by splitting the bezier until we can replace the segment by a straight
    // line. We split further until the control points are close enough to the line connecting the
    // boundary points.
    //
    // the Distance of a point p from a line given by the points (a,b) is given by:
    //
    // d = abs( (bx - ax)(ay - py) - (by - ay)(ax - px) ) / line_length
    //
    // We can stop splitting if both control points are close enough to the line.
    // To make the algorithm faster we use the manhattan length of the line.

    QPolygonF polygon;
    polygon.append(QPointF(x1, y1));
    addToPolygon(&polygon);
    return polygon;
}

//0.5 is really low
static const qreal flatness = 0.5;

//based on "Fast, precise flattening of cubic Bezier path and offset curves"
//      by T. F. Hain, A. L. Ahmad, S. V. R. Racherla and D. D. Langan
static inline void flattenBezierWithoutInflections(QBezier &bez,
                                                   QPolygonF *&p)
{
    QBezier left;

    while (1) {
        qreal dx = bez.x2 - bez.x1;
        qreal dy = bez.y2 - bez.y1;

        qreal normalized = qSqrt(dx * dx + dy * dy);
        if (qFuzzyCompare(normalized + 1, 1))
           break;

        qreal d = qAbs(dx * (bez.y3 - bez.y2) - dy * (bez.x3 - bez.x2));

        qreal t = qSqrt(4. / 3. * normalized * flatness / d);
        if (t > 1 || qFuzzyCompare(t, (qreal)1.))
            break;
        bez.parameterSplitLeft(t, &left);
        p->append(bez.pt1());
    }
}


static inline int quadraticRoots(qreal a, qreal b, qreal c,
                                 qreal *x1, qreal *x2)
{
    if (qFuzzyCompare(a + 1, 1)) {
        if (qFuzzyCompare(b + 1, 1))
            return 0;
        *x1 = *x2 = (-c / b);
        return 1;
    } else {
        const qreal det = b * b - 4 * a * c;
        if (qFuzzyCompare(det + 1, 1)) {
            *x1 = *x2 = -b / (2 * a);
            return 1;
        }
        if (det > 0) {
            if (qFuzzyCompare(b + 1, 1)) {
                *x2 = qSqrt(-c / a);
                *x1 = -(*x2);
                return 2;
            }
            const qreal stableA = b / (2 * a);
            const qreal stableB = c / (a * stableA * stableA);
            const qreal stableC = -1 - qSqrt(1 - stableB);
            *x2 = stableA * stableC;
            *x1 = (stableA * stableB) / stableC;
            return 2;
        } else
            return 0;
    }
}

static inline bool findInflections(qreal a, qreal b, qreal c,
                                   qreal *t1 , qreal *t2, qreal *tCups)
{
    qreal r1 = 0, r2 = 0;

    short rootsCount = quadraticRoots(a, b, c, &r1, &r2);

    if (rootsCount >= 1) {
        if (r1 < r2) {
            *t1 = r1;
            *t2 = r2;
        } else {
            *t1 = r2;
            *t2 = r1;
        }
        if (!qFuzzyCompare(a + 1, 1))
            *tCups = 0.5 * (-b / a);
        else
            *tCups = 2;

        return true;
    }

    return false;
}


void QBezier::addToPolygon(QPolygonF *polygon) const
{
    QBezier beziers[32];
    beziers[0] = *this;
    QBezier *b = beziers;

    while (b >= beziers) {
        // check if we can pop the top bezier curve from the stack
        qreal y4y1 = b->y4 - b->y1;
        qreal x4x1 = b->x4 - b->x1;
        qreal l = qAbs(x4x1) + qAbs(y4y1);
        qreal d;
        if (l > 1.) {
            d = qAbs( (x4x1)*(b->y1 - b->y2) - (y4y1)*(b->x1 - b->x2) )
                + qAbs( (x4x1)*(b->y1 - b->y3) - (y4y1)*(b->x1 - b->x3) );
        } else {
            d = qAbs(b->x1 - b->x2) + qAbs(b->y1 - b->y2) +
                qAbs(b->x1 - b->x3) + qAbs(b->y1 - b->y3);
            l = 1.;
        }
        if (d < flatness*l || b == beziers + 31) {
            // good enough, we pop it off and add the endpoint
            polygon->append(QPointF(b->x4, b->y4));
            --b;
        } else {
            // split, second half of the polygon goes lower into the stack
            b->split(b+1, b);
            ++b;
        }
    }
}

void QBezier::addToPolygonMixed(QPolygonF *polygon) const
{
    qreal ax = -x1 + 3*x2 - 3*x3 + x4;
    qreal ay = -y1 + 3*y2 - 3*y3 + y4;
    qreal bx = 3*x1 - 6*x2 + 3*x3;
    qreal by = 3*y1 - 6*y2 + 3*y3;
    qreal cx = -3*x1 + 3*x2;
    qreal cy = -3*y1 + 2*y2;
    qreal a = 6 * (ay * bx - ax * by);
    qreal b = 6 * (ay * cx - ax * cy);
    qreal c = 2 * (by * cx - bx * cy);

    if ((qFuzzyCompare(a + 1, 1) && qFuzzyCompare(b + 1, 1)) ||
        (b * b - 4 * a *c) < 0) {
        QBezier bez(*this);
        flattenBezierWithoutInflections(bez, polygon);
        polygon->append(QPointF(x4, y4));
    } else {
        QBezier beziers[32];
        beziers[0] = *this;
        QBezier *b = beziers;

        while (b >= beziers) {
            // check if we can pop the top bezier curve from the stack
            qreal y4y1 = b->y4 - b->y1;
            qreal x4x1 = b->x4 - b->x1;
            qreal l = qAbs(x4x1) + qAbs(y4y1);
            qreal d;
            if (l > 1.) {
                d = qAbs( (x4x1)*(b->y1 - b->y2) - (y4y1)*(b->x1 - b->x2) )
                    + qAbs( (x4x1)*(b->y1 - b->y3) - (y4y1)*(b->x1 - b->x3) );
            } else {
                d = qAbs(b->x1 - b->x2) + qAbs(b->y1 - b->y2) +
                    qAbs(b->x1 - b->x3) + qAbs(b->y1 - b->y3);
                l = 1.;
            }
            if (d < .5*l || b == beziers + 31) {
                // good enough, we pop it off and add the endpoint
                polygon->append(QPointF(b->x4, b->y4));
                --b;
            } else {
                // split, second half of the polygon goes lower into the stack
                b->split(b+1, b);
                ++b;
            }
        }
    }
}

QRectF QBezier::bounds() const
{
    qreal xmin = x1;
    qreal xmax = x1;
    if (x2 < xmin)
        xmin = x2;
    else if (x2 > xmax)
        xmax = x2;
    if (x3 < xmin)
        xmin = x3;
    else if (x3 > xmax)
        xmax = x3;
    if (x4 < xmin)
        xmin = x4;
    else if (x4 > xmax)
        xmax = x4;

    qreal ymin = y1;
    qreal ymax = y1;
    if (y2 < ymin)
        ymin = y2;
    else if (y2 > ymax)
        ymax = y2;
    if (y3 < ymin)
        ymin = y3;
    else if (y3 > ymax)
        ymax = y3;
    if (y4 < ymin)
        ymin = y4;
    else if (y4 > ymax)
        ymax = y4;
    return QRectF(xmin, ymin, xmax-xmin, ymax-ymin);
}


enum ShiftResult {
    Ok,
    Discard,
    Split,
    Circle
};

static ShiftResult good_offset(const QBezier *b1, const QBezier *b2, qreal offset, qreal threshold)
{
    const qreal o2 = offset*offset;
    const qreal max_dist_line = threshold*offset*offset;
    const qreal max_dist_normal = threshold*offset;
    const qreal spacing = 0.25;
    for (qreal i = spacing; i < 0.99; i += spacing) {
        QPointF p1 = b1->pointAt(i);
        QPointF p2 = b2->pointAt(i);
        qreal d = (p1.x() - p2.x())*(p1.x() - p2.x()) + (p1.y() - p2.y())*(p1.y() - p2.y());
        if (qAbs(d - o2) > max_dist_line)
            return Split;

        QPointF normalPoint = b1->normalVector(i);
        qreal l = qAbs(normalPoint.x()) + qAbs(normalPoint.y());
        if (l != 0.) {
            d = qAbs( normalPoint.x()*(p1.y() - p2.y()) - normalPoint.y()*(p1.x() - p2.x()) ) / l;
            if (d > max_dist_normal)
                return Split;
        }
    }
    return Ok;
}

static inline QLineF qline_shifted(const QPointF &p1, const QPointF &p2, qreal offset)
{
    QLineF l(p1, p2);
    QLineF ln = l.normalVector().unitVector();
    l.translate(ln.dx() * offset, ln.dy() * offset);
    return l;
}

static bool qbezier_is_line(QPointF *points, int pointCount)
{
    Q_ASSERT(pointCount > 2);

    qreal dx13 = points[2].x() - points[0].x();
    qreal dy13 = points[2].y() - points[0].y();

    qreal dx12 = points[1].x() - points[0].x();
    qreal dy12 = points[1].y() - points[0].y();

    if (pointCount == 3) {
        return qFuzzyCompare(dx12 * dy13, dx13 * dy12);
    } else if (pointCount == 4) {
        qreal dx14 = points[3].x() - points[0].x();
        qreal dy14 = points[3].y() - points[0].y();

        return (qFuzzyCompare(dx12 * dy13, dx13 * dy12) && qFuzzyCompare(dx12 * dy14, dx14 * dy12));
    }

    return false;
}

static ShiftResult shift(const QBezier *orig, QBezier *shifted, qreal offset, qreal threshold)
{
    int map[4];
    bool p1_p2_equal = (orig->x1 == orig->x2 && orig->y1 == orig->y2);
    bool p2_p3_equal = (orig->x2 == orig->x3 && orig->y2 == orig->y3);
    bool p3_p4_equal = (orig->x3 == orig->x4 && orig->y3 == orig->y4);

    QPointF points[4];
    int np = 0;
    points[np] = QPointF(orig->x1, orig->y1);
    map[0] = 0;
    ++np;
    if (!p1_p2_equal) {
        points[np] = QPointF(orig->x2, orig->y2);
        ++np;
    }
    map[1] = np - 1;
    if (!p2_p3_equal) {
        points[np] = QPointF(orig->x3, orig->y3);
        ++np;
    }
    map[2] = np - 1;
    if (!p3_p4_equal) {
        points[np] = QPointF(orig->x4, orig->y4);
        ++np;
    }
    map[3] = np - 1;
    if (np == 1)
        return Discard;

    // We need to specialcase lines of 3 or 4 points due to numerical
    // instability in intersections below
    if (np > 2 && qbezier_is_line(points, np)) {
        if (points[0] == points[np-1])
            return Discard;

        QLineF l = qline_shifted(points[0], points[np-1], offset);
        *shifted = QBezier::fromPoints(l.p1(), l.pointAt(qreal(0.33)), l.pointAt(qreal(0.66)), l.p2());
        return Ok;
    }

    QRectF b = orig->bounds();
    if (np == 4 && b.width() < .1*offset && b.height() < .1*offset) {
        qreal l = (orig->x1 - orig->x2)*(orig->x1 - orig->x2) +
                  (orig->y1 - orig->y2)*(orig->y1 - orig->y1) *
                  (orig->x3 - orig->x4)*(orig->x3 - orig->x4) +
                  (orig->y3 - orig->y4)*(orig->y3 - orig->y4);
        qreal dot = (orig->x1 - orig->x2)*(orig->x3 - orig->x4) +
                    (orig->y1 - orig->y2)*(orig->y3 - orig->y4);
        if (dot < 0 && dot*dot < 0.8*l)
            // the points are close and reverse dirction. Approximate the whole
            // thing by a semi circle
            return Circle;
    }

    QPointF points_shifted[4];

    QLineF prev = QLineF(QPointF(), points[1] - points[0]);
    QPointF prev_normal = prev.normalVector().unitVector().p2();

    points_shifted[0] = points[0] + offset * prev_normal;

    for (int i = 1; i < np - 1; ++i) {
        QLineF next = QLineF(QPointF(), points[i + 1] - points[i]);
        QPointF next_normal = next.normalVector().unitVector().p2();

        QPointF normal_sum = prev_normal + next_normal;

        qreal r = 1.0 + prev_normal.x() * next_normal.x()
                  + prev_normal.y() * next_normal.y();

        if (qFuzzyCompare(r + 1, 1)) {
            points_shifted[i] = points[i] + offset * prev_normal;
        } else {
            qreal k = offset / r;
            points_shifted[i] = points[i] + k * normal_sum;
        }

        prev_normal = next_normal;
    }

    points_shifted[np - 1] = points[np - 1] + offset * prev_normal;

    *shifted = QBezier::fromPoints(points_shifted[map[0]], points_shifted[map[1]],
                                   points_shifted[map[2]], points_shifted[map[3]]);

    return good_offset(orig, shifted, offset, threshold);
}

// This value is used to determine the length of control point vectors
// when approximating arc segments as curves. The factor is multiplied
// with the radius of the circle.
#define KAPPA 0.5522847498


static bool addCircle(const QBezier *b, qreal offset, QBezier *o)
{
    QPointF normals[3];

    normals[0] = QPointF(b->y2 - b->y1, b->x1 - b->x2);
    qreal dist = qSqrt(normals[0].x()*normals[0].x() + normals[0].y()*normals[0].y());
    if (qFuzzyCompare(dist + 1, 1))
        return false;
    normals[0] /= dist;
    normals[2] = QPointF(b->y4 - b->y3, b->x3 - b->x4);
    dist = qSqrt(normals[2].x()*normals[2].x() + normals[2].y()*normals[2].y());
    if (qFuzzyCompare(dist + 1, 1))
        return false;
    normals[2] /= dist;

    normals[1] = QPointF(b->x1 - b->x2 - b->x3 + b->x4, b->y1 - b->y2 - b->y3 + b->y4);
    normals[1] /= -1*qSqrt(normals[1].x()*normals[1].x() + normals[1].y()*normals[1].y());

    qreal angles[2];
    qreal sign = 1.;
    for (int i = 0; i < 2; ++i) {
        qreal cos_a = normals[i].x()*normals[i+1].x() + normals[i].y()*normals[i+1].y();
        if (cos_a > 1.)
            cos_a = 1.;
        if (cos_a < -1.)
            cos_a = -1;
        angles[i] = acos(cos_a)/Q_PI;
    }

    if (angles[0] + angles[1] > 1.) {
        // more than 180 degrees
        normals[1] = -normals[1];
        angles[0] = 1. - angles[0];
        angles[1] = 1. - angles[1];
        sign = -1.;

    }

    QPointF circle[3];
    circle[0] = QPointF(b->x1, b->y1) + normals[0]*offset;
    circle[1] = QPointF(0.5*(b->x1 + b->x4), 0.5*(b->y1 + b->y4)) + normals[1]*offset;
    circle[2] = QPointF(b->x4, b->y4) + normals[2]*offset;

    for (int i = 0; i < 2; ++i) {
        qreal kappa = 2.*KAPPA * sign * offset * angles[i];

        o->x1 = circle[i].x();
        o->y1 = circle[i].y();
        o->x2 = circle[i].x() - normals[i].y()*kappa;
        o->y2 = circle[i].y() + normals[i].x()*kappa;
        o->x3 = circle[i+1].x() + normals[i+1].y()*kappa;
        o->y3 = circle[i+1].y() - normals[i+1].x()*kappa;
        o->x4 = circle[i+1].x();
        o->y4 = circle[i+1].y();

        ++o;
    }
    return true;
}

int QBezier::shifted(QBezier *curveSegments, int maxSegments, qreal offset, float threshold) const
{
    Q_ASSERT(curveSegments);
    Q_ASSERT(maxSegments > 0);

    if (x1 == x2 && x1 == x3 && x1 == x4 &&
        y1 == y2 && y1 == y3 && y1 == y4)
        return 0;

    --maxSegments;
    QBezier beziers[10];
redo:
    beziers[0] = *this;
    QBezier *b = beziers;
    QBezier *o = curveSegments;

    while (b >= beziers) {
        int stack_segments = b - beziers + 1;
        if ((stack_segments == 10) || (o - curveSegments == maxSegments - stack_segments)) {
            threshold *= 1.5;
            if (threshold > 2.)
                goto give_up;
            goto redo;
        }
        ShiftResult res = shift(b, o, offset, threshold);
        if (res == Discard) {
            --b;
        } else if (res == Ok) {
            ++o;
            --b;
            continue;
        } else if (res == Circle && maxSegments - (o - curveSegments) >= 2) {
            // add semi circle
            if (addCircle(b, offset, o))
                o += 2;
            --b;
        } else {
            b->split(b+1, b);
            ++b;
        }
    }

give_up:
    while (b >= beziers) {
        ShiftResult res = shift(b, o, offset, threshold);

        // if res isn't Ok or Split then *o is undefined
        if (res == Ok || res == Split)
            ++o;

        --b;
    }

    Q_ASSERT(o - curveSegments <= maxSegments);
    return o - curveSegments;
}

#if 0
static inline bool IntersectBB(const QBezier &a, const QBezier &b)
{
    return a.bounds().intersects(b.bounds());
}
#else
static int IntersectBB(const QBezier &a, const QBezier &b)
{
    // Compute bounding box for a
    qreal minax, maxax, minay, maxay;
    if (a.x1 > a.x4)	 // These are the most likely to be extremal
	minax = a.x4, maxax = a.x1;
    else
	minax = a.x1, maxax = a.x4;

    if (a.x3 < minax)
	minax = a.x3;
    else if (a.x3 > maxax)
	maxax = a.x3;

    if (a.x2 < minax)
	minax = a.x2;
    else if (a.x2 > maxax)
	maxax = a.x2;

    if (a.y1 > a.y4)
	minay = a.y4, maxay = a.y1;
    else
	minay = a.y1, maxay = a.y4;

    if (a.y3 < minay)
	minay = a.y3;
    else if (a.y3 > maxay)
	maxay = a.y3;

    if (a.y2 < minay)
	minay = a.y2;
    else if (a.y2 > maxay)
	maxay = a.y2;

    // Compute bounding box for b
    qreal minbx, maxbx, minby, maxby;
    if (b.x1 > b.x4)
	minbx = b.x4, maxbx = b.x1;
    else
	minbx = b.x1, maxbx = b.x4;

    if (b.x3 < minbx)
	minbx = b.x3;
    else if (b.x3 > maxbx)
	maxbx = b.x3;

    if (b.x2 < minbx)
	minbx = b.x2;
    else if (b.x2 > maxbx)
	maxbx = b.x2;

    if (b.y1 > b.y4)
	minby = b.y4, maxby = b.y1;
    else
	minby = b.y1, maxby = b.y4;

    if (b.y3 < minby)
	minby = b.y3;
    else if (b.y3 > maxby)
	maxby = b.y3;

    if (b.y2 < minby)
	minby = b.y2;
    else if (b.y2 > maxby)
	maxby = b.y2;

    // Test bounding box of b against bounding box of a
    if ((minax > maxbx) || (minay > maxby)  // Not >= : need boundary case
	|| (minbx > maxax) || (minby > maxay))
	return 0; // they don't intersect
    else
	return 1; // they intersect
}
#endif


#ifdef QDEBUG_BEZIER
static QDebug operator<<(QDebug dbg, const QBezier &bz)
{
    dbg <<"["<<bz.x1<<", "<<bz.y1<<"], "
        <<"["<<bz.x2<<", "<<bz.y2<<"], "
        <<"["<<bz.x3<<", "<<bz.y3<<"], "
        <<"["<<bz.x4<<", "<<bz.y4<<"]";
    return dbg;
}
#endif

static bool RecursivelyIntersect(const QBezier &a, qreal t0, qreal t1, int deptha,
                                 const QBezier &b, qreal u0, qreal u1, int depthb,
                                 QVector<QPair<qreal, qreal> > *t)
{
#ifdef QDEBUG_BEZIER
    static int I = 0;
    int currentD = I;
    fprintf(stderr, "%d) t0 = %lf, t1 = %lf, deptha = %d\n"
            "u0 = %lf, u1 = %lf, depthb = %d\n", I++, t0, t1, deptha,
            u0, u1, depthb);
#endif
    if (deptha > 0) {
	QBezier A[2];
        a.split(&A[0], &A[1]);
	qreal tmid = (t0+t1)*0.5;
        //qDebug()<<"\t1)"<<A[0];
        //qDebug()<<"\t2)"<<A[1];
	deptha--;
	if (depthb > 0) {
	    QBezier B[2];
            b.split(&B[0], &B[1]);
            //qDebug()<<"\t3)"<<B[0];
            //qDebug()<<"\t4)"<<B[1];
	    qreal umid = (u0+u1)*0.5;
	    depthb--;
	    if (IntersectBB(A[0], B[0])) {
                //fprintf(stderr, "\t 1 from %d\n", currentD);
		if (RecursivelyIntersect(A[0], t0, tmid, deptha,
				     B[0], u0, umid, depthb,
				     t) && !t)
                    return true;
            }
	    if (IntersectBB(A[1], B[0])) {
                //fprintf(stderr, "\t 2 from %d\n", currentD);
		if (RecursivelyIntersect(A[1], tmid, t1, deptha,
                                     B[0], u0, umid, depthb,
                                     t) && !t)
                    return true;
            }
	    if (IntersectBB(A[0], B[1])) {
                //fprintf(stderr, "\t 3 from %d\n", currentD);
		if (RecursivelyIntersect(A[0], t0, tmid, deptha,
                                     B[1], umid, u1, depthb,
                                     t) && !t)
                    return true;
            }
	    if (IntersectBB(A[1], B[1])) {
                //fprintf(stderr, "\t 4 from %d\n", currentD);
		if (RecursivelyIntersect(A[1], tmid, t1, deptha,
				     B[1], umid, u1, depthb,
				     t) && !t)
                    return true;
            }
            return t ? !t->isEmpty() : false;
        } else {
	    if (IntersectBB(A[0], b)) {
                //fprintf(stderr, "\t 5 from %d\n", currentD);
		if (RecursivelyIntersect(A[0], t0, tmid, deptha,
				     b, u0, u1, depthb,
				     t) && !t)
                    return true;
            }
	    if (IntersectBB(A[1], b)) {
                //fprintf(stderr, "\t 6 from %d\n", currentD);
		if (RecursivelyIntersect(A[1], tmid, t1, deptha,
                                     b, u0, u1, depthb,
                                     t) && !t)
                    return true;
            }
            return t ? !t->isEmpty() : false;
        }
    } else {
	if (depthb > 0) {
	    QBezier B[2];
            b.split(&B[0], &B[1]);
	    qreal umid = (u0 + u1)*0.5;
	    depthb--;
	    if (IntersectBB(a, B[0])) {
                //fprintf(stderr, "\t 7 from %d\n", currentD);
		if (RecursivelyIntersect(a, t0, t1, deptha,
                                     B[0], u0, umid, depthb,
                                     t) && !t)
                    return true;
            }
	    if (IntersectBB(a, B[1])) {
                //fprintf(stderr, "\t 8 from %d\n", currentD);
		if (RecursivelyIntersect(a, t0, t1, deptha,
                                     B[1], umid, u1, depthb,
                                     t) && !t)
                    return true;
            }
            return t ? !t->isEmpty() : false;
        }
	else {
            // Both segments are fully subdivided; now do line segments
	    qreal xlk = a.x4 - a.x1;
	    qreal ylk = a.y4 - a.y1;
	    qreal xnm = b.x4 - b.x1;
	    qreal ynm = b.y4 - b.y1;
	    qreal xmk = b.x1 - a.x1;
	    qreal ymk = b.y1 - a.y1;
	    qreal det = xnm * ylk - ynm * xlk;
	    if (1.0 + det == 1.0) {
		return false;
            } else {
                qreal detinv = 1.0 / det;
                qreal rs = (xnm * ymk - ynm *xmk) * detinv;
                qreal rt = (xlk * ymk - ylk * xmk) * detinv;
                if ((rs < 0.0) || (rs > 1.0) || (rt < 0.0) || (rt > 1.0))
                    return false;

                if (t) {
                    const qreal alpha_a = t0 + rs * (t1 - t0);
                    const qreal alpha_b = u0 + rt * (u1 - u0);

                    *t << qMakePair(alpha_a, alpha_b);
                }

                return true;
            }
        }
    }
}

QVector< QPair<qreal, qreal> > QBezier::findIntersections(const QBezier &a, const QBezier &b)
{
    QVector< QPair<qreal, qreal> > v(2);
    findIntersections(a, b, &v);
    return v;
}

bool QBezier::findIntersections(const QBezier &a, const QBezier &b,
                                QVector<QPair<qreal, qreal> > *t)
{
    if (IntersectBB(a, b)) {
        QPointF la1(fabs((a.x3 - a.x2) - (a.x2 - a.x1)),
                    fabs((a.y3 - a.y2) - (a.y2 - a.y1)));
	QPointF la2(fabs((a.x4 - a.x3) - (a.x3 - a.x2)),
                    fabs((a.y4 - a.y3) - (a.y3 - a.y2)));
	QPointF la;
	if (la1.x() > la2.x()) la.setX(la1.x()); else la.setX(la2.x());
	if (la1.y() > la2.y()) la.setY(la1.y()); else la.setY(la2.y());
	QPointF lb1(fabs((b.x3 - b.x2) - (b.x2 - b.x1)),
                    fabs((b.y3 - b.y2) - (b.y2 - b.y1)));
	QPointF lb2(fabs((b.x4 - b.x3) - (b.x3 - b.x2)),
                    fabs((b.y4 - b.y3) - (b.y3 - b.y2)));
	QPointF lb;
	if (lb1.x() > lb2.x()) lb.setX(lb1.x()); else lb.setX(lb2.x());
	if (lb1.y() > lb2.y()) lb.setY(lb1.y()); else lb.setY(lb2.y());
	qreal l0;
	if (la.x() > la.y())
	    l0 = la.x();
	else
	    l0 = la.y();
	int ra;
	if (l0 * 0.75 * M_SQRT2 + 1.0 == 1.0)
	    ra = 0;
	else
	    ra = qCeil(log4(M_SQRT2 * 6.0 / 8.0 * INV_EPS * l0));
	if (lb.x() > lb.y())
	    l0 = lb.x();
	else
	    l0 = lb.y();
	int rb;
	if (l0 * 0.75 * M_SQRT2 + 1.0 == 1.0)
	    rb = 0;
	else
	    rb = qCeil(log4(M_SQRT2 * 6.0 / 8.0 * INV_EPS * l0));

        // if qreal is float then halve the number of subdivisions
        if (sizeof(qreal) == 4) {
            ra /= 2;
            rb /= 2;
        }

	return RecursivelyIntersect(a, 0., 1., ra, b, 0., 1., rb, t);
    }

    //Don't sort here because it breaks the orders of corresponding
    //  intersections points. this way t's at the same locations correspond
    //  to the same intersection point.
    //qSort(parameters[0].begin(), parameters[0].end(), qLess<qreal>());
    //qSort(parameters[1].begin(), parameters[1].end(), qLess<qreal>());

    return false;
}

static inline void splitBezierAt(const QBezier &bez, qreal t,
                                 QBezier *left, QBezier *right)
{
    left->x1 = bez.x1;
    left->y1 = bez.y1;

    left->x2 = bez.x1 + t * ( bez.x2 - bez.x1 );
    left->y2 = bez.y1 + t * ( bez.y2 - bez.y1 );

    left->x3 = bez.x2 + t * ( bez.x3 - bez.x2 ); // temporary holding spot
    left->y3 = bez.y2 + t * ( bez.y3 - bez.y2 ); // temporary holding spot

    right->x3 = bez.x3 + t * ( bez.x4 - bez.x3 );
    right->y3 = bez.y3 + t * ( bez.y4 - bez.y3 );

    right->x2 = left->x3 + t * ( right->x3 - left->x3);
    right->y2 = left->y3 + t * ( right->y3 - left->y3);

    left->x3 = left->x2 + t * ( left->x3 - left->x2 );
    left->y3 = left->y2 + t * ( left->y3 - left->y2 );

    left->x4 = right->x1 = left->x3 + t * (right->x2 - left->x3);
    left->y4 = right->y1 = left->y3 + t * (right->y2 - left->y3);

    right->x4 = bez.x4;
    right->y4 = bez.y4;
}

QVector< QList<QBezier> > QBezier::splitAtIntersections(QBezier &b)
{
    QVector< QList<QBezier> > curves(2);

    QVector< QPair<qreal, qreal> > allInters = findIntersections(*this, b);

    QList<qreal> inters1;
    QList<qreal> inters2;

    for (int i = 0; i < allInters.size(); ++i) {
        inters1 << allInters[i].first;
        inters2 << allInters[i].second;
    }

    qSort(inters1.begin(), inters1.end(), qLess<qreal>());
    qSort(inters2.begin(), inters2.end(), qLess<qreal>());

    Q_ASSERT(inters1.count() == inters2.count());

    int i;
    for (i = 0; i < inters1.count(); ++i) {
        qreal t1 = inters1.at(i);
        qreal t2 = inters2.at(i);

        QBezier curve1, curve2;
        parameterSplitLeft(t1, &curve1);
	b.parameterSplitLeft(t2, &curve2);
        curves[0].append(curve1);
        curves[0].append(curve2);
    }
    curves[0].append(*this);
    curves[1].append(b);

    return curves;
}

qreal QBezier::length(qreal error) const
{
    qreal length = 0.0;

    addIfClose(&length, error);

    return length;
}

void QBezier::addIfClose(qreal *length, qreal error) const
{
    QBezier left, right;     /* bez poly splits */

    qreal len = 0.0;        /* arc length */
    qreal chord;            /* chord length */

    len = len + QLineF(QPointF(x1, y1),QPointF(x2, y2)).length();
    len = len + QLineF(QPointF(x2, y2),QPointF(x3, y3)).length();
    len = len + QLineF(QPointF(x3, y3),QPointF(x4, y4)).length();

    chord = QLineF(QPointF(x1, y1),QPointF(x4, y4)).length();

    if((len-chord) > error) {
        split(&left, &right);                 /* split in two */
        left.addIfClose(length, error);       /* try left side */
        right.addIfClose(length, error);      /* try right side */
        return;
    }

    *length = *length + len;

    return;
}

qreal QBezier::tForY(qreal t0, qreal t1, qreal y) const
{
    qreal py0 = pointAt(t0).y();
    qreal py1 = pointAt(t1).y();

    if (py0 > py1) {
        qSwap(py0, py1);
        qSwap(t0, t1);
    }

    Q_ASSERT(py0 <= py1);

    if (py0 >= y)
        return t0;
    else if (py1 <= y)
        return t1;

    Q_ASSERT(py0 < y && y < py1);

    qreal lt = t0;
    qreal dt;
    do {
        qreal t = 0.5 * (t0 + t1);

        qreal a, b, c, d;
        QBezier::coefficients(t, a, b, c, d);
        qreal yt = a * y1 + b * y2 + c * y3 + d * y4;

        if (yt < y) {
            t0 = t;
            py0 = yt;
        } else {
            t1 = t;
            py1 = yt;
        }
        dt = lt - t;
        lt = t;
    } while (qAbs(dt) > 1e-7);

    return t0;
}

int QBezier::stationaryYPoints(qreal &t0, qreal &t1) const
{
    // y(t) = (1 - t)^3 * y1 + 3 * (1 - t)^2 * t * y2 + 3 * (1 - t) * t^2 * y3 + t^3 * y4
    // y'(t) = 3 * (-(1-2t+t^2) * y1 + (1 - 4 * t + 3 * t^2) * y2 + (2 * t - 3 * t^2) * y3 + t^2 * y4)
    // y'(t) = 3 * ((-y1 + 3 * y2 - 3 * y3 + y4)t^2 + (2 * y1 - 4 * y2 + 2 * y3)t + (-y1 + y2))

    const qreal a = -y1 + 3 * y2 - 3 * y3 + y4;
    const qreal b = 2 * y1 - 4 * y2 + 2 * y3;
    const qreal c = -y1 + y2;

    qreal reciprocal = b * b - 4 * a * c;

    QList<qreal> result;

    if (qFuzzyCompare(reciprocal + 1, 1)) {
        t0 = -b / (2 * a);
        return 1;
    } else if (reciprocal > 0) {
        qreal temp = qSqrt(reciprocal);

        t0 = (-b - temp)/(2*a);
        t1 = (-b + temp)/(2*a);

        if (t1 < t0)
            qSwap(t0, t1);

        int count = 0;
        qreal t[2] = { 0, 1 };

        if (t0 > 0 && t0 < 1)
            t[count++] = t0;
        if (t1 > 0 && t1 < 1)
            t[count++] = t1;

        t0 = t[0];
        t1 = t[1];

        return count;
    }

    return 0;
}

qreal QBezier::tAtLength(qreal l) const
{
    qreal len = length();
    qreal t   = 1.0;
    const qreal error = (qreal)0.01;
    if (l > len || qFuzzyCompare(l, len))
        return t;

    t *= 0.5;
    //int iters = 0;
    //qDebug()<<"LEN is "<<l<<len;
    qreal lastBigger = 1.;
    while (1) {
        //qDebug()<<"\tt is "<<t;
        QBezier right = *this;
        QBezier left;
        right.parameterSplitLeft(t, &left);
        qreal lLen = left.length();
        if (qAbs(lLen - l) < error)
            break;

        if (lLen < l) {
            t += (lastBigger - t)*.5;
        } else {
            lastBigger = t;
            t -= t*.5;
        }
        //++iters;
    }
    //qDebug()<<"number of iters is "<<iters;
    return t;
}

QBezier QBezier::bezierOnInterval(qreal t0, qreal t1) const
{
    if (t0 == 0 && t1 == 1)
        return *this;

    QBezier bezier = *this;

    QBezier result;
    bezier.parameterSplitLeft(t0, &result);
    qreal trueT = (t1-t0)/(1-t0);
    bezier.parameterSplitLeft(trueT, &result);

    return result;
}


static inline void bindInflectionPoint(const QBezier &bez, const qreal t,
                                       qreal *tMinus , qreal *tPlus)
{
    if (t <= 0) {
        *tMinus = *tPlus = -1;
        return;
    } else if (t >= 1) {
        *tMinus = *tPlus = 2;
        return;
    }

    QBezier left, right;
    splitBezierAt(bez, t, &left, &right);

    qreal ax = -right.x1 + 3*right.x2 - 3*right.x3 + right.x4;
    qreal ay = -right.y1 + 3*right.y2 - 3*right.y3 + right.y4;
    qreal ex = 3 * (right.x2 - right.x3);
    qreal ey = 3 * (right.y2 - right.y3);

    qreal s4 = qAbs(6 * (ey * ax - ex * ay) / qSqrt(ex * ex + ey * ey)) + 0.00001f;
    qreal tf = pow(qreal(9 * flatness / s4), qreal(1./3.));
    *tMinus = t - (1 - t) * tf;
    *tPlus  = t + (1 - t) * tf;
}

void QBezier::addToPolygonIterative(QPolygonF *p) const
{
    qreal t1, t2, tcusp;
    qreal t1min, t1plus, t2min, t2plus;

    qreal ax = -x1 + 3*x2 - 3*x3 + x4;
    qreal ay = -y1 + 3*y2 - 3*y3 + y4;
    qreal bx = 3*x1 - 6*x2 + 3*x3;
    qreal by = 3*y1 - 6*y2 + 3*y3;
    qreal cx = -3*x1 + 3*x2;
    qreal cy = -3*y1 + 2*y2;

    if (findInflections(6 * (ay * bx - ax * by),
                        6 * (ay * cx - ax * cy),
                        2 * (by * cx - bx * cy),
                        &t1, &t2, &tcusp)) {
        bindInflectionPoint(*this, t1, &t1min, &t1plus);
        bindInflectionPoint(*this, t2, &t2min, &t2plus);

        QBezier tmpBez = *this;
        QBezier left, right, bez1, bez2, bez3;
	if (t1min > 0) {
            if (t1min >= 1) {
                flattenBezierWithoutInflections(tmpBez, p);
            } else {
                splitBezierAt(tmpBez, t1min, &left, &right);
                flattenBezierWithoutInflections(left, p);
                p->append(tmpBez.pointAt(t1min));

                if (t2min < t1plus) {
                    if (tcusp < 1) {
                        p->append(tmpBez.pointAt(tcusp));
                    }
                    if (t2plus < 1) {
                        splitBezierAt(tmpBez, t2plus, &left, &right);
                        flattenBezierWithoutInflections(right, p);
                    }
                } else if (t1plus < 1) {
                    if (t2min < 1) {
                        splitBezierAt(tmpBez, t2min, &bez3, &right);
                        splitBezierAt(bez3, t1plus, &left, &bez2);

                        flattenBezierWithoutInflections(bez2, p);
                        p->append(tmpBez.pointAt(t2min));

                        if (t2plus < 1) {
                            splitBezierAt(tmpBez, t2plus, &left, &bez2);
                            flattenBezierWithoutInflections(bez2, p);
                        }
                    } else {
                        splitBezierAt(tmpBez, t1plus, &left, &bez2);
                        flattenBezierWithoutInflections(bez2, p);
                    }
                }
            }
	} else if (t1plus > 0) {
            p->append(QPointF(x1, y1));
            if (t2min < t1plus)	{
                if (tcusp < 1) {
                    p->append(tmpBez.pointAt(tcusp));
                }
                if (t2plus < 1) {
                    splitBezierAt(tmpBez, t2plus, &left, &bez2);
                    flattenBezierWithoutInflections(bez2, p);
                }
            } else if (t1plus < 1) {
                if (t2min < 1) {
                    splitBezierAt(tmpBez, t2min, &bez3, &right);
                    splitBezierAt(bez3, t1plus, &left, &bez2);

                    flattenBezierWithoutInflections(bez2, p);

                    p->append(tmpBez.pointAt(t2min));
                    if (t2plus < 1) {
                        splitBezierAt(tmpBez, t2plus, &left, &bez2);
                        flattenBezierWithoutInflections(bez2, p);
                    }
                } else {
                    splitBezierAt(tmpBez, t1plus, &left, &bez2);
                    flattenBezierWithoutInflections(bez2, p);
                }
            }
        } else if (t2min > 0) {
            if (t2min < 1) {
                splitBezierAt(tmpBez, t2min, &bez1, &right);
                flattenBezierWithoutInflections(bez1, p);
                p->append(tmpBez.pointAt(t2min));

                if (t2plus < 1) {
                    splitBezierAt(tmpBez, t2plus, &left, &bez2);
                    flattenBezierWithoutInflections(bez2, p);
                }
            } else {
                //### in here we should check whether the area of the
                //    triangle formed between pt1/pt2/pt3 is smaller
                //    or equal to 0 and then do iterative flattening
                //    if not we should fallback and do the recursive
                //    flattening.
                flattenBezierWithoutInflections(tmpBez, p);
            }
        } else if (t2plus > 0) {
            p->append(QPointF(x1, y1));
            if (t2plus < 1) {
                splitBezierAt(tmpBez, t2plus, &left, &bez2);
                flattenBezierWithoutInflections(bez2, p);
            }
        } else {
            flattenBezierWithoutInflections(tmpBez, p);
        }
    } else {
        QBezier bez = *this;
        flattenBezierWithoutInflections(bez, p);
    }

    p->append(QPointF(x4, y4));
}

QT_END_NAMESPACE
