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

#include "qpathclipper_p.h"

#include <private/qbezier_p.h>
#include <private/qdatabuffer_p.h>
#include <qmath.h>

#include <QImage>
#include <QPainter>

/**
  The algorithm is as follows:

  1. Find all intersections between the two paths (including self-intersections),
     and build a winged edge structure of non-intersecting parts.
  2. While there are more unhandled edges:
    3. Pick a y-coordinate from an unhandled edge.
    4. Intersect the horizontal line at y-coordinate with all edges.
    5. Traverse intersections left to right deciding whether each subpath should be added or not.
    6. If the subpath should be added, traverse the winged-edge structure and add the edges to
       a separate winged edge structure.
    7. Mark all edges in subpaths crossing the horizontal line as handled.
 8. (Optional) Simplify the resulting winged edge structure by merging shared edges.
 9. Convert the resulting winged edge structure to a painter path.
 */

#include <qdebug.h>

QT_BEGIN_NAMESPACE

//#define QDEBUG_CLIPPER
static qreal dot(const QPointF &a, const QPointF &b)
{
    return a.x() * b.x() + a.y() * b.y();
}

static QPointF normalize(const QPointF &p)
{
    return p / qSqrt(p.x() * p.x() + p.y() * p.y());
}

static bool pathToRect(const QPainterPath &path, QRectF *rect = 0);

struct QIntersection
{
    qreal alphaA;
    qreal alphaB;

    QPointF pos;
};

class QIntersectionFinder
{
public:
    void produceIntersections(QPathSegments &segments);
    bool hasIntersections(const QPathSegments &a, const QPathSegments &b) const;

private:
    void intersectBeziers(const QBezier &one, const QBezier &two, QVector<QPair<qreal, qreal> > &t, QDataBuffer<QIntersection> &intersections);
    void intersectLines(const QLineF &a, const QLineF &b, QDataBuffer<QIntersection> &intersections);

    bool beziersIntersect(const QBezier &one, const QBezier &two) const;
    bool linesIntersect(const QLineF &a, const QLineF &b) const;
};

bool QIntersectionFinder::beziersIntersect(const QBezier &one, const QBezier &two) const
{
    return (one.pt1() == two.pt1() && one.pt2() == two.pt2() && one.pt3() == two.pt3() && one.pt4() == two.pt4())
           || (one.pt1() == two.pt4() && one.pt2() == two.pt3() && one.pt3() == two.pt2() && one.pt4() == two.pt1())
           || QBezier::findIntersections(one, two, 0);
}

bool QIntersectionFinder::linesIntersect(const QLineF &a, const QLineF &b) const
{
    const QPointF p1 = a.p1();
    const QPointF p2 = a.p2();

    const QPointF q1 = b.p1();
    const QPointF q2 = b.p2();

    if (p1 == p2 || q1 == q2)
        return false;

    const bool p1_equals_q1 = (p1 == q1);
    const bool p2_equals_q2 = (p2 == q2);

    if (p1_equals_q1 && p2_equals_q2)
        return true;

    const bool p1_equals_q2 = (p1 == q2);
    const bool p2_equals_q1 = (p2 == q1);

    if (p1_equals_q2 && p2_equals_q1)
        return true;

    const QPointF pDelta = p2 - p1;
    const QPointF qDelta = q2 - q1;

    const qreal par = pDelta.x() * qDelta.y() - pDelta.y() * qDelta.x();

    if (qFuzzyCompare(par + 1, 1)) {
        const QPointF normal(-pDelta.y(), pDelta.x());

        // coinciding?
        if (qFuzzyCompare(dot(normal, q1 - p1) + 1, 1)) {
            const qreal dp = dot(pDelta, pDelta);

            const qreal tq1 = dot(pDelta, q1 - p1);
            const qreal tq2 = dot(pDelta, q2 - p1);

            if ((tq1 > 0 && tq1 < dp) || (tq2 > 0 && tq2 < dp))
                return true;

            const qreal dq = dot(qDelta, qDelta);

            const qreal tp1 = dot(qDelta, p1 - q1);
            const qreal tp2 = dot(qDelta, p2 - q1);

            if ((tp1 > 0 && tp1 < dq) || (tp2 > 0 && tp2 < dq))
                return true;
        }

        return false;
    }

    // if the lines are not parallel and share a common end point, then they
    // don't intersect
    if (p1_equals_q1 || p1_equals_q2 || p2_equals_q1 || p2_equals_q2)
        return false;

    const qreal invPar = 1 / par;

    const qreal tp = (qDelta.y() * (q1.x() - p1.x()) -
                      qDelta.x() * (q1.y() - p1.y())) * invPar;

    if (tp < 0 || tp > 1)
        return false;

    const qreal tq = (pDelta.y() * (q1.x() - p1.x()) -
                      pDelta.x() * (q1.y() - p1.y())) * invPar;

    return tq >= 0 && tq <= 1;
}

void QIntersectionFinder::intersectBeziers(const QBezier &one, const QBezier &two, QVector<QPair<qreal, qreal> > &t, QDataBuffer<QIntersection> &intersections)
{
    if ((one.pt1() == two.pt1() && one.pt2() == two.pt2() && one.pt3() == two.pt3() && one.pt4() == two.pt4())
        || (one.pt1() == two.pt4() && one.pt2() == two.pt3() && one.pt3() == two.pt2() && one.pt4() == two.pt1())) {

        return;
    }

    t.clear();

    if (!QBezier::findIntersections(one, two, &t))
        return;

    int count = t.size();

    for (int i = 0; i < count; ++i) {
        qreal alpha_p = t.at(i).first;
        qreal alpha_q = t.at(i).second;

        QPointF pt;
        if (qFuzzyCompare(alpha_p + 1, 1)) {
            pt = one.pt1();
        } else if (qFuzzyCompare(alpha_p, 1)) {
            pt = one.pt4();
        } else if (qFuzzyCompare(alpha_q + 1, 1)) {
            pt = two.pt1();
        } else if (qFuzzyCompare(alpha_q, 1)) {
            pt = two.pt4();
        } else {
            pt = one.pointAt(alpha_p);
        }

        QIntersection intersection;
        intersection.alphaA = alpha_p;
        intersection.alphaB = alpha_q;
        intersection.pos = pt;
        intersections.add(intersection);
    }
}

void QIntersectionFinder::intersectLines(const QLineF &a, const QLineF &b, QDataBuffer<QIntersection> &intersections)
{
    const QPointF p1 = a.p1();
    const QPointF p2 = a.p2();

    const QPointF q1 = b.p1();
    const QPointF q2 = b.p2();

    if (p1 == p2 || q1 == q2)
        return;

    const bool p1_equals_q1 = (p1 == q1);
    const bool p2_equals_q2 = (p2 == q2);

    if (p1_equals_q1 && p2_equals_q2)
        return;

    const bool p1_equals_q2 = (p1 == q2);
    const bool p2_equals_q1 = (p2 == q1);

    if (p1_equals_q2 && p2_equals_q1)
        return;

    const QPointF pDelta = p2 - p1;
    const QPointF qDelta = q2 - q1;

    const qreal par = pDelta.x() * qDelta.y() - pDelta.y() * qDelta.x();

    if (qFuzzyCompare(par + 1, 1)) {
        const QPointF normal(-pDelta.y(), pDelta.x());

        // coinciding?
        if (qFuzzyCompare(dot(normal, q1 - p1) + 1, 1)) {
            const qreal invDp = 1 / dot(pDelta, pDelta);

            const qreal tq1 = dot(pDelta, q1 - p1) * invDp;
            const qreal tq2 = dot(pDelta, q2 - p1) * invDp;

            if (tq1 > 0 && tq1 < 1) {
                QIntersection intersection;
                intersection.alphaA = tq1;
                intersection.alphaB = 0;
                intersection.pos = q1;
                intersections.add(intersection);
            }

            if (tq2 > 0 && tq2 < 1) {
                QIntersection intersection;
                intersection.alphaA = tq2;
                intersection.alphaB = 1;
                intersection.pos = q2;
                intersections.add(intersection);
            }

            const qreal invDq = 1 / dot(qDelta, qDelta);

            const qreal tp1 = dot(qDelta, p1 - q1) * invDq;
            const qreal tp2 = dot(qDelta, p2 - q1) * invDq;

            if (tp1 > 0 && tp1 < 1) {
                QIntersection intersection;
                intersection.alphaA = 0;
                intersection.alphaB = tp1;
                intersection.pos = p1;
                intersections.add(intersection);
            }

            if (tp2 > 0 && tp2 < 1) {
                QIntersection intersection;
                intersection.alphaA = 1;
                intersection.alphaB = tp2;
                intersection.pos = p2;
                intersections.add(intersection);
            }
        }

        return;
    }

    // if the lines are not parallel and share a common end point, then they
    // don't intersect
    if (p1_equals_q1 || p1_equals_q2 || p2_equals_q1 || p2_equals_q2)
        return;


    const qreal tp = (qDelta.y() * (q1.x() - p1.x()) -
                      qDelta.x() * (q1.y() - p1.y())) / par;
    const qreal tq = (pDelta.y() * (q1.x() - p1.x()) -
                      pDelta.x() * (q1.y() - p1.y())) / par;

    if (tp<0 || tp>1 || tq<0 || tq>1)
        return;

    const bool p_zero = qFuzzyCompare(tp + 1, 1);
    const bool p_one = qFuzzyCompare(tp, 1);

    const bool q_zero = qFuzzyCompare(tq + 1, 1);
    const bool q_one = qFuzzyCompare(tq, 1);

    if ((q_zero || q_one) && (p_zero || p_one))
        return;

    QPointF pt;
    if (p_zero) {
        pt = p1;
    } else if (p_one) {
        pt = p2;
    } else if (q_zero) {
        pt = q1;
    } else if (q_one) {
        pt = q2;
    } else {
        pt = q1 + (q2 - q1) * tq;
    }

    QIntersection intersection;
    intersection.alphaA = tp;
    intersection.alphaB = tq;
    intersection.pos = pt;
    intersections.add(intersection);
}

static const QBezier bezierFromLine(const QLineF &line)
{
    const QPointF p1 = line.p1();
    const QPointF p2 = line.p2();
    const QPointF delta = (p2 - p1) / 3;
    return QBezier::fromPoints(p1, p1 + delta, p1 + 2 * delta, p2);
}

bool QIntersectionFinder::hasIntersections(const QPathSegments &a, const QPathSegments &b) const
{
    QBezier tempA;
    QBezier tempB;

    if (a.segments() == 0 || b.segments() == 0)
        return false;

    const QRectF &rb0 = b.elementBounds(0);

    qreal minX = rb0.left();
    qreal minY = rb0.top();
    qreal maxX = rb0.right();
    qreal maxY = rb0.bottom();

    for (int i = 1; i < b.segments(); ++i) {
        const QRectF &r = b.elementBounds(i);
        minX = qMin(minX, r.left());
        minY = qMin(minY, r.top());
        maxX = qMax(maxX, r.right());
        maxY = qMax(maxY, r.bottom());
    }

    QRectF rb(minX, minY, maxX - minX, maxY - minY);

    for (int i = 0; i < a.segments(); ++i) {
        const QBezier *bezierA = a.bezierAt(i);
        bool isBezierA = bezierA != 0;

        const QRectF &r1 = a.elementBounds(i);

        if (r1.left() > rb.right() || rb.left() > r1.right())
            continue;
        if (r1.top() > rb.bottom() || rb.top() > r1.bottom())
            continue;

        for (int j = 0; j < b.segments(); ++j) {
            const QRectF &r2 = b.elementBounds(j);

            if (r1.left() > r2.right() || r2.left() > r1.right())
                continue;
            if (r1.top() > r2.bottom() || r2.top() > r1.bottom())
                continue;

            bool isBezierB = b.bezierAt(j) != 0;

            if (isBezierA || isBezierB) {
                const QBezier *bezierB;
                if (isBezierB) {
                    bezierB = b.bezierAt(j);
                } else {
                    tempB = bezierFromLine(b.lineAt(j));
                    bezierB = &tempB;
                }

                if (!bezierA) {
                    tempA = bezierFromLine(a.lineAt(i));
                    bezierA = &tempA;
                }

                if (beziersIntersect(*bezierA, *bezierB))
                    return true;
            } else {
                if (linesIntersect(a.lineAt(i), b.lineAt(j)))
                    return true;
            }
        }
    }

    return false;
}

void QIntersectionFinder::produceIntersections(QPathSegments &segments)
{
    QBezier tempA;
    QBezier tempB;

    QVector<QPair<qreal, qreal> > t;
    QDataBuffer<QIntersection> intersections;

    for (int i = 0; i < segments.segments(); ++i) {
        const QBezier *bezierA = segments.bezierAt(i);
        bool isBezierA = bezierA != 0;

        const QRectF &r1 = segments.elementBounds(i);

        for (int j = 0; j < i; ++j) {
            const QRectF &r2 = segments.elementBounds(j);

            if (r1.left() > r2.right() || r2.left() > r1.right())
                continue;
            if (r1.top() > r2.bottom() || r2.top() > r1.bottom())
                continue;

            intersections.reset();

            bool isBezierB = segments.bezierAt(j) != 0;

            if (isBezierA || isBezierB) {
                const QBezier *bezierB;
                if (isBezierB) {
                    bezierB = segments.bezierAt(j);
                } else {
                    tempB = bezierFromLine(segments.lineAt(j));
                    bezierB = &tempB;
                }

                if (!bezierA) {
                    tempA = bezierFromLine(segments.lineAt(i));
                    bezierA = &tempA;
                }

                intersectBeziers(*bezierA, *bezierB, t, intersections);
            } else {
                const QLineF lineA = segments.lineAt(i);
                const QLineF lineB = segments.lineAt(j);

                intersectLines(lineA, lineB, intersections);
            }

            for (int k = 0; k < intersections.size(); ++k) {
                QPathSegments::Intersection i_isect, j_isect;
                i_isect.vertex = j_isect.vertex = segments.addPoint(intersections.at(k).pos);

                i_isect.t = intersections.at(k).alphaA;
                j_isect.t = intersections.at(k).alphaB;

                i_isect.next = 0;
                j_isect.next = 0;

                segments.addIntersection(i, i_isect);
                segments.addIntersection(j, j_isect);
            }
        }
    }
}

class QKdPointTree
{
public:
    enum Traversal {
        TraverseBoth,
        TraverseLeft,
        TraverseRight,
        TraverseNone
    };

    struct Node {
        int point;
        int id;

        Node *left;
        Node *right;
    };

    QKdPointTree(const QPathSegments &segments)
        : m_segments(&segments)
        , m_nodes(m_segments->points())
        , m_id(0)
    {
        m_nodes.resize(m_segments->points());

        for (int i = 0; i < m_nodes.size(); ++i) {
            m_nodes.at(i).point = i;
            m_nodes.at(i).id = -1;
        }

        m_rootNode = build(0, m_nodes.size());
    }

    int build(int begin, int end, int depth = 0);

    Node *rootNode()
    {
        return &m_nodes.at(m_rootNode);
    }

    inline int nextId()
    {
        return m_id++;
    }

private:
    const QPathSegments *m_segments;
    QDataBuffer<Node> m_nodes;

    int m_rootNode;
    int m_id;
};

template <typename T>
void qTraverseKdPointTree(QKdPointTree::Node &node, T &t, int depth = 0)
{
    QKdPointTree::Traversal status = t(node, depth);

    const bool traverseRight = (status == QKdPointTree::TraverseBoth || status == QKdPointTree::TraverseRight);
    const bool traverseLeft = (status == QKdPointTree::TraverseBoth || status == QKdPointTree::TraverseLeft);

    if (traverseLeft && node.left)
        QT_PREPEND_NAMESPACE(qTraverseKdPointTree<T>)(*node.left, t, depth + 1);

    if (traverseRight && node.right)
        QT_PREPEND_NAMESPACE(qTraverseKdPointTree<T>)(*node.right, t, depth + 1);
}

static inline qreal component(const QPointF &point, unsigned int i)
{
    Q_ASSERT(i < 2);
    const qreal components[] = { point.x(), point.y() };
    return components[i];
}

int QKdPointTree::build(int begin, int end, int depth)
{
    Q_ASSERT(end > begin);

    const qreal pivot = component(m_segments->pointAt(m_nodes.at(begin).point), depth & 1);

    int first = begin + 1;
    int last = end - 1;

    while (first <= last) {
        const qreal value = component(m_segments->pointAt(m_nodes.at(first).point), depth & 1);

        if (value < pivot)
            ++first;
        else {
            qSwap(m_nodes.at(first), m_nodes.at(last));
            --last;
        }
    }

    qSwap(m_nodes.at(last), m_nodes.at(begin));

    if (last > begin)
        m_nodes.at(last).left = &m_nodes.at(build(begin, last, depth + 1));
    else
        m_nodes.at(last).left = 0;

    if (last + 1 < end)
        m_nodes.at(last).right = &m_nodes.at(build(last + 1, end, depth + 1));
    else
        m_nodes.at(last).right = 0;

    return last;
}

class QKdPointFinder
{
public:
    QKdPointFinder(int point, const QPathSegments &segments, QKdPointTree &tree)
        : m_point(point)
        , m_result(-1)
        , m_segments(&segments)
        , m_tree(&tree)
    {
        pointComponents[0] = segments.pointAt(point).x();
        pointComponents[1] = segments.pointAt(point).y();
    }

    inline QKdPointTree::Traversal operator()(QKdPointTree::Node &node, int depth)
    {
        if (m_result != -1)
            return QKdPointTree::TraverseNone;

        const QPointF &nodePoint = m_segments->pointAt(node.point);

        const qreal pivotComponents[] = { nodePoint.x(), nodePoint.y() };

        const qreal pivot = pivotComponents[depth & 1];
        const qreal value = pointComponents[depth & 1];

        if (qFuzzyCompare(pivot, value)) {
            const qreal pivot2 = pivotComponents[(depth + 1) & 1];
            const qreal value2 = pointComponents[(depth + 1) & 1];

            if (qFuzzyCompare(pivot2, value2)) {
                if (node.id < 0)
                    node.id = m_tree->nextId();

                m_result = node.id;
                return QKdPointTree::TraverseNone;
            } else
                return QKdPointTree::TraverseBoth;
        } else if (value < pivot) {
            return QKdPointTree::TraverseLeft;
        } else {
            return QKdPointTree::TraverseRight;
        }
    }

    int result() const
    {
        return m_result;
    }

private:
    int m_point;
    qreal pointComponents[2];
    int m_result;
    const QPathSegments *m_segments;
    QKdPointTree *m_tree;
};

// merge all points that are within qFuzzyCompare range of each other
void QPathSegments::mergePoints()
{
    QKdPointTree tree(*this);

    if (tree.rootNode()) {
        QDataBuffer<QPointF> mergedPoints(points());
        QDataBuffer<int> pointIndices(points());

        for (int i = 0; i < points(); ++i) {
            QKdPointFinder finder(i, *this, tree);
            QT_PREPEND_NAMESPACE(qTraverseKdPointTree<QKdPointFinder>)(*tree.rootNode(), finder);

            Q_ASSERT(finder.result() != -1);

            if (finder.result() >= mergedPoints.size())
                mergedPoints << m_points.at(i);

            pointIndices << finder.result();
        }

        for (int i = 0; i < m_segments.size(); ++i) {
            m_segments.at(i).va = pointIndices.at(m_segments.at(i).va);
            m_segments.at(i).vb = pointIndices.at(m_segments.at(i).vb);
        }

        for (int i = 0; i < m_intersections.size(); ++i)
            m_intersections.at(i).vertex = pointIndices.at(m_intersections.at(i).vertex);

        m_points.swap(mergedPoints);
    }
}

void QWingedEdge::intersectAndAdd()
{
    QIntersectionFinder finder;
    finder.produceIntersections(m_segments);

    m_segments.mergePoints();

    for (int i = 0; i < m_segments.points(); ++i)
        addVertex(m_segments.pointAt(i));

    QDataBuffer<QPathSegments::Intersection> intersections;
    for (int i = 0; i < m_segments.segments(); ++i) {
        intersections.reset();

        int pathId = m_segments.pathId(i);

        const QPathSegments::Intersection *isect = m_segments.intersectionAt(i);
        while (isect) {
            intersections << *isect;

            if (isect->next) {
                isect += isect->next;
            } else {
                isect = 0;
            }
        }

        qSort(intersections.data(), intersections.data() + intersections.size());

        const QBezier *bezier = m_segments.bezierAt(i);
        if (bezier) {
            int first = m_segments.segmentAt(i).va;
            int second = m_segments.segmentAt(i).vb;

            qreal alpha = 0.0;
            int last = first;
            for (int j = 0; j < intersections.size(); ++j) {
                const QPathSegments::Intersection &isect = intersections.at(j);

                addBezierEdge(bezier, last, isect.vertex, alpha, isect.t, pathId);

                alpha = isect.t;
                last = isect.vertex;
            }

            addBezierEdge(bezier, last, second, alpha, 1.0, pathId);
        } else {
            int first = m_segments.segmentAt(i).va;
            int second = m_segments.segmentAt(i).vb;

            int last = first;
            for (int j = 0; j < intersections.size(); ++j) {
                const QPathSegments::Intersection &isect = intersections.at(j);

                QPathEdge *ep = edge(addEdge(last, isect.vertex));

                if (ep) {
                    const int dir = m_segments.pointAt(last).y() < m_segments.pointAt(isect.vertex).y() ? 1 : -1;
                    if (pathId == 0)
                        ep->windingA += dir;
                    else
                        ep->windingB += dir;
                }

                last = isect.vertex;
            }

            QPathEdge *ep = edge(addEdge(last, second));

            if (ep) {
                const int dir = m_segments.pointAt(last).y() < m_segments.pointAt(second).y() ? 1 : -1;
                if (pathId == 0)
                    ep->windingA += dir;
                else
                    ep->windingB += dir;
            }
        }
    }
}

QWingedEdge::QWingedEdge()
{
}

QWingedEdge::QWingedEdge(const QPainterPath &subject, const QPainterPath &clip)
{
    m_segments.setPath(subject);
    m_segments.addPath(clip);

    intersectAndAdd();
}

QWingedEdge::TraversalStatus QWingedEdge::next(const QWingedEdge::TraversalStatus &status) const
{
    const QPathEdge *sp = edge(status.edge);
    Q_ASSERT(sp);

    TraversalStatus result;
    result.edge = sp->next(status.traversal, status.direction);
    result.traversal = status.traversal;
    result.direction = status.direction;

    const QPathEdge *rp = edge(result.edge);
    Q_ASSERT(rp);

    if (sp->vertex(status.direction) == rp->vertex(status.direction))
        result.flip();

    return result;
}

static bool isLine(const QBezier &bezier)
{
    const bool equal_1_2 = bezier.pt1() == bezier.pt2();
    const bool equal_2_3 = bezier.pt2() == bezier.pt3();
    const bool equal_3_4 = bezier.pt3() == bezier.pt4();

    // point?
    if (equal_1_2 && equal_2_3 && equal_3_4)
        return true;

    if (bezier.pt1() == bezier.pt4())
        return equal_1_2 || equal_3_4;

    return (equal_1_2 && equal_3_4) || (equal_1_2 && equal_2_3) || (equal_2_3 && equal_3_4);
}

void QPathSegments::setPath(const QPainterPath &path)
{
    m_points.reset();
    m_beziers.reset();
    m_intersections.reset();
    m_segments.reset();

    m_pathId = 0;

    addPath(path);
}

void QPathSegments::addPath(const QPainterPath &path)
{
    int firstSegment = m_segments.size();

    bool hasMoveTo = false;
    int lastMoveTo = 0;
    int last = 0;
    for (int i = 0; i < path.elementCount(); ++i) {
        int current = m_points.size();

        QPointF currentPoint;
        if (path.elementAt(i).type == QPainterPath::CurveToElement)
            currentPoint = path.elementAt(i+2);
        else
            currentPoint = path.elementAt(i);

        if (i > 0 && m_points.at(lastMoveTo) == currentPoint)
            current = lastMoveTo;
        else
            m_points << currentPoint;

        switch (path.elementAt(i).type) {
        case QPainterPath::MoveToElement:
            if (hasMoveTo && last != lastMoveTo && m_points.at(last) != m_points.at(lastMoveTo))
                m_segments << Segment(m_pathId, last, lastMoveTo);
            hasMoveTo = true;
            last = lastMoveTo = current;
            break;
        case QPainterPath::LineToElement:
            m_segments << Segment(m_pathId, last, current);
            last = current;
            break;
        case QPainterPath::CurveToElement:
            {
                QBezier bezier = QBezier::fromPoints(m_points.at(last), path.elementAt(i), path.elementAt(i+1), path.elementAt(i+2));
                if (isLine(bezier)) {
                    m_segments << Segment(m_pathId, last, current);
                } else {
                    m_segments << Segment(m_pathId, last, current, m_beziers.size());
                    m_beziers << bezier;
                }
            }
            last = current;
            i += 2;
            break;
        default:
            Q_ASSERT(false);
            break;
        }
    }

    if (hasMoveTo && last != lastMoveTo && m_points.at(last) != m_points.at(lastMoveTo))
        m_segments << Segment(m_pathId, last, lastMoveTo);

    for (int i = firstSegment; i < m_segments.size(); ++i) {
        const QBezier *bezier = bezierAt(i);
        if (bezier) {
            m_segments.at(i).bounds = bezier->bounds();
        } else {
            const QLineF line = lineAt(i);

            qreal x1 = line.p1().x();
            qreal y1 = line.p1().y();
            qreal x2 = line.p2().x();
            qreal y2 = line.p2().y();

            if (x2 < x1)
                qSwap(x1, x2);
            if (y2 < y1)
                qSwap(y1, y2);

            m_segments.at(i).bounds = QRectF(x1, y1, x2 - x1, y2 - y1);
        }
    }

    ++m_pathId;
}

qreal QWingedEdge::delta(int vertex, int a, int b) const
{
    const QPathEdge *ap = edge(a);
    const QPathEdge *bp = edge(b);

    qreal a_angle = ap->angle;
    qreal b_angle = bp->angle;

    if (vertex == ap->second)
        a_angle = ap->invAngle;

    if (vertex == bp->second)
        b_angle = bp->invAngle;

    qreal result = b_angle - a_angle;

    if (qFuzzyCompare(result + 1, 1) || qFuzzyCompare(result, 128))
        return 0;

    if (result < 0)
        return result + 128.;
    else
        return result;
}

static inline QPointF tangentAt(const QWingedEdge &list, int vi, int ei)
{
    const QPathEdge *ep = list.edge(ei);
    Q_ASSERT(ep);

    qreal t;
    qreal sign;

    if (ep->first == vi) {
        t = ep->t0;
        sign = 1;
    } else {
        t = ep->t1;
        sign = -1;
    }

    QPointF normal;
    if (ep->bezier) {
        normal = ep->bezier->derivedAt(t);

        if (qFuzzyCompare(normal.x() + 1, 1) && qFuzzyCompare(normal.y() + 1, 1))
            normal = ep->bezier->secondDerivedAt(t);
    } else {
        const QPointF a = *list.vertex(ep->first);
        const QPointF b = *list.vertex(ep->second);
        normal = b - a;
    }

    return normalize(sign * normal);
}

static inline QPointF midPoint(const QWingedEdge &list, int ei)
{
    const QPathEdge *ep = list.edge(ei);
    Q_ASSERT(ep);

    if (ep->bezier) {
        return ep->bezier->pointAt(0.5 * (ep->t0 + ep->t1));
    } else {
        const QPointF a = *list.vertex(ep->first);
        const QPointF b = *list.vertex(ep->second);
        return a + 0.5 * (b - a);
    }
}

static QBezier transform(const QBezier &bezier, const QPointF &xAxis, const QPointF &yAxis, const QPointF &origin)
{
    QPointF points[4] = {
        bezier.pt1(),
        bezier.pt2(),
        bezier.pt3(),
        bezier.pt4()
    };

    for (int i = 0; i < 4; ++i) {
        const QPointF p = points[i] - origin;

        points[i].rx() = dot(xAxis, p);
        points[i].ry() = dot(yAxis, p);
    }

    return QBezier::fromPoints(points[0], points[1], points[2], points[3]);
}

static bool isLeftOf(const QWingedEdge &list, int vi, int ai, int bi)
{
    const QPathEdge *ap = list.edge(ai);
    const QPathEdge *bp = list.edge(bi);

    Q_ASSERT(ap);
    Q_ASSERT(bp);

    if (!(ap->bezier || bp->bezier))
        return false;

    const QPointF tangent = tangentAt(list, vi, ai);
    const QPointF normal(tangent.y(), -tangent.x());

    const QPointF origin = *list.vertex(vi);

    const QPointF dpA = midPoint(list, ai) - origin;
    const QPointF dpB = midPoint(list, bi) - origin;

    qreal xA = dot(normal, dpA);
    qreal xB = dot(normal, dpB);

    if (xA <= 0 && xB >= 0)
        return true;

    if (xA >= 0 && xB <= 0)
        return false;

    if (!ap->bezier)
        return xB > 0;

    if (!bp->bezier)
        return xA < 0;

    // both are beziers on the same side of the tangent

    // transform the beziers into the local coordinate system
    // such that positive y is along the tangent, and positive x is along the normal

    QBezier bezierA = transform(*ap->bezier, normal, tangent, origin);
    QBezier bezierB = transform(*bp->bezier, normal, tangent, origin);

    qreal y = qMin(bezierA.pointAt(0.5 * (ap->t0 + ap->t1)).y(),
                   bezierB.pointAt(0.5 * (bp->t0 + bp->t1)).y());

    xA = bezierA.pointAt(bezierA.tForY(ap->t0, ap->t1, y)).x();
    xB = bezierB.pointAt(bezierB.tForY(bp->t0, bp->t1, y)).x();

    return xA < xB;
}

QWingedEdge::TraversalStatus QWingedEdge::findInsertStatus(int vi, int ei) const
{
    const QPathVertex *vp = vertex(vi);

    Q_ASSERT(vp);
    Q_ASSERT(ei >= 0);
    Q_ASSERT(vp->edge >= 0);

    int position = vp->edge;
    qreal d = 128.;

    TraversalStatus status;
    status.direction = edge(vp->edge)->directionTo(vi);
    status.traversal = QPathEdge::RightTraversal;
    status.edge = vp->edge;

#ifdef QDEBUG_CLIPPER
    const QPathEdge *ep = edge(ei);
    qDebug() << "Finding insert status for edge" << ei << "at vertex" << QPointF(*vp) << ", angles: " << ep->angle << ep->invAngle;
#endif

    do {
        status = next(status);
        status.flip();

        Q_ASSERT(edge(status.edge)->vertex(status.direction) == vi);

        qreal d2 = delta(vi, ei, status.edge);

#ifdef QDEBUG_CLIPPER
        const QPathEdge *op = edge(status.edge);
        qDebug() << "Delta to edge" << status.edge << d2 << ", angles: " << op->angle << op->invAngle;
#endif

        if (!(qFuzzyCompare(d2 + 1, 1) && isLeftOf(*this, vi, status.edge, ei))
            && (d2 < d || (qFuzzyCompare(d2, d) && isLeftOf(*this, vi, status.edge, position)))) {
            position = status.edge;
            d = d2;
        }
    } while (status.edge != vp->edge);

    status.traversal = QPathEdge::LeftTraversal;
    status.direction = QPathEdge::Forward;
    status.edge = position;

    if (edge(status.edge)->vertex(status.direction) != vi)
        status.flip();

#ifdef QDEBUG_CLIPPER
    qDebug() << "Inserting edge" << ei << "to" << (status.traversal == QPathEdge::LeftTraversal ? "left" : "right") << "of edge" << status.edge;
#endif

    Q_ASSERT(edge(status.edge)->vertex(status.direction) == vi);

    return status;
}

void QWingedEdge::removeEdge(int ei)
{
    QPathEdge *ep = edge(ei);

    TraversalStatus status;
    status.direction = QPathEdge::Forward;
    status.traversal = QPathEdge::RightTraversal;
    status.edge = ei;

    TraversalStatus forwardRight = next(status);
    forwardRight.flipDirection();

    status.traversal = QPathEdge::LeftTraversal;
    TraversalStatus forwardLeft = next(status);
    forwardLeft.flipDirection();

    status.direction = QPathEdge::Backward;
    TraversalStatus backwardLeft = next(status);
    backwardLeft.flipDirection();

    status.traversal = QPathEdge::RightTraversal;
    TraversalStatus backwardRight = next(status);
    backwardRight.flipDirection();

    edge(forwardRight.edge)->setNext(forwardRight.traversal, forwardRight.direction, forwardLeft.edge);
    edge(forwardLeft.edge)->setNext(forwardLeft.traversal, forwardLeft.direction, forwardRight.edge);

    edge(backwardRight.edge)->setNext(backwardRight.traversal, backwardRight.direction, backwardLeft.edge);
    edge(backwardLeft.edge)->setNext(backwardLeft.traversal, backwardLeft.direction, backwardRight.edge);

    ep->setNext(QPathEdge::Forward, ei);
    ep->setNext(QPathEdge::Backward, ei);

    QPathVertex *a = vertex(ep->first);
    QPathVertex *b = vertex(ep->second);

    a->edge = backwardRight.edge;
    b->edge = forwardRight.edge;
}

static int commonEdge(const QWingedEdge &list, int a, int b)
{
    const QPathVertex *ap = list.vertex(a);
    Q_ASSERT(ap);

    const QPathVertex *bp = list.vertex(b);
    Q_ASSERT(bp);

    if (ap->edge < 0 || bp->edge < 0)
        return -1;

    QWingedEdge::TraversalStatus status;
    status.edge = ap->edge;
    status.direction = list.edge(status.edge)->directionTo(a);
    status.traversal = QPathEdge::RightTraversal;

    do {
        const QPathEdge *ep = list.edge(status.edge);

        if ((ep->first == a && ep->second == b)
            || (ep->first == b && ep->second == a))
            return status.edge;

        status = list.next(status);
        status.flip();
    } while (status.edge != ap->edge);

    return -1;
}

static qreal computeAngle(const QPointF &v)
{
#if 1
    if (v.x() == 0) {
        return v.y() <= 0 ? 0 : 64.;
    } else if (v.y() == 0) {
        return v.x() <= 0 ? 32. : 96.;
    }

    QPointF nv = normalize(v);
    if (nv.y() < 0) {
        if (nv.x() < 0) { // 0 - 32
            return -32. * nv.x();
        } else { // 96 - 128
            return 128. - 32. * nv.x();
        }
    } else { // 32 - 96
        return 64. + 32 * nv.x();
    }
#else
    // doesn't seem to be robust enough
    return atan2(v.x(), v.y()) + Q_PI;
#endif
}

int QWingedEdge::addEdge(const QPointF &a, const QPointF &b, const QBezier *bezier, qreal t0, qreal t1)
{
    int fi = insert(a);
    int si = insert(b);

    return addEdge(fi, si, bezier, t0, t1);
}

int QWingedEdge::addEdge(int fi, int si, const QBezier *bezier, qreal t0, qreal t1)
{
    if (fi == si)
        return -1;

    int common = commonEdge(*this, fi, si);
    if (common >= 0)
        return common;

    m_edges << QPathEdge(fi, si);

    int ei = m_edges.size() - 1;

    QPathVertex *fp = vertex(fi);
    QPathVertex *sp = vertex(si);

    QPathEdge *ep = edge(ei);

    ep->bezier = bezier;
    ep->t0 = t0;
    ep->t1 = t1;

    if (bezier) {
        QPointF aTangent = bezier->derivedAt(t0);
        QPointF bTangent = -bezier->derivedAt(t1);

        if (qFuzzyCompare(aTangent.x() + 1, 1) && qFuzzyCompare(aTangent.y() + 1, 1))
            aTangent = bezier->secondDerivedAt(t0);

        if (qFuzzyCompare(bTangent.x() + 1, 1) && qFuzzyCompare(bTangent.y() + 1, 1))
            bTangent = bezier->secondDerivedAt(t1);

        ep->angle = computeAngle(aTangent);
        ep->invAngle = computeAngle(bTangent);
    } else {
        const QPointF tangent = QPointF(*sp) - QPointF(*fp);
        ep->angle = computeAngle(tangent);
        ep->invAngle = ep->angle + 64;
        if (ep->invAngle >= 128)
            ep->invAngle -= 128;
    }

    QPathVertex *vertices[2] = { fp, sp };
    QPathEdge::Direction dirs[2] = { QPathEdge::Backward, QPathEdge::Forward };

#ifdef QDEBUG_CLIPPER
    printf("** Adding edge %d / vertices: %.07f %.07f, %.07f %.07f\n", ei, fp->x, fp->y, sp->x, sp->y);
#endif

    for (int i = 0; i < 2; ++i) {
        QPathVertex *vp = vertices[i];
        if (vp->edge < 0) {
            vp->edge = ei;
            ep->setNext(dirs[i], ei);
        } else {
            int vi = ep->vertex(dirs[i]);
            Q_ASSERT(vertex(vi) == vertices[i]);

            TraversalStatus os = findInsertStatus(vi, ei);
            QPathEdge *op = edge(os.edge);

            Q_ASSERT(vertex(op->vertex(os.direction)) == vertices[i]);

            TraversalStatus ns = next(os);
            ns.flipDirection();
            QPathEdge *np = edge(ns.edge);

            op->setNext(os.traversal, os.direction, ei);
            np->setNext(ns.traversal, ns.direction, ei);

            int oe = os.edge;
            int ne = ns.edge;

            os = next(os);
            ns = next(ns);

            os.flipDirection();
            ns.flipDirection();

            Q_ASSERT(os.edge == ei);
            Q_ASSERT(ns.edge == ei);

            ep->setNext(os.traversal, os.direction, oe);
            ep->setNext(ns.traversal, ns.direction, ne);
        }
    }

    Q_ASSERT(ep->next(QPathEdge::RightTraversal, QPathEdge::Forward) >= 0);
    Q_ASSERT(ep->next(QPathEdge::RightTraversal, QPathEdge::Backward) >= 0);
    Q_ASSERT(ep->next(QPathEdge::LeftTraversal, QPathEdge::Forward) >= 0);
    Q_ASSERT(ep->next(QPathEdge::LeftTraversal, QPathEdge::Backward) >= 0);

    return ei;
}

void QWingedEdge::addBezierEdge(const QBezier *bezier, int vertexA, int vertexB, qreal alphaA, qreal alphaB, int path)
{
    if (qFuzzyCompare(alphaA, alphaB))
        return;

    qreal alphaMid = (alphaA + alphaB) * 0.5;

    qreal s0 = 0;
    qreal s1 = 1;
    int count = bezier->stationaryYPoints(s0, s1);

    m_splitPoints.clear();
    m_splitPoints << alphaA;
    m_splitPoints << alphaMid;
    m_splitPoints << alphaB;

    if (count > 0 && !qFuzzyCompare(s0, alphaA) && !qFuzzyCompare(s0, alphaMid) && !qFuzzyCompare(s0, alphaB) && s0 > alphaA && s0 < alphaB)
        m_splitPoints << s0;

    if (count > 1 && !qFuzzyCompare(s1, alphaA) && !qFuzzyCompare(s1, alphaMid) && !qFuzzyCompare(s1, alphaB) && s1 > alphaA && s1 < alphaB)
        m_splitPoints << s1;

    if (count > 0)
        qSort(m_splitPoints.begin(), m_splitPoints.end());

    int last = vertexA;
    for (int i = 0; i < m_splitPoints.size() - 1; ++i) {
        const qreal t0 = m_splitPoints[i];
        const qreal t1 = m_splitPoints[i+1];

        int current;
        if ((i + 1) == (m_splitPoints.size() - 1)) {
            current = vertexB;
        } else {
            current = insert(bezier->pointAt(t1));
        }

        QPathEdge *ep = edge(addEdge(last, current, bezier, t0, t1));

        if (ep) {
            const int dir = m_vertices.at(last).y < m_vertices.at(current).y ? 1 : -1;
            if (path == 0)
                ep->windingA += dir;
            else
                ep->windingB += dir;
        }

        last = current;
    }
}

void QWingedEdge::addBezierEdge(const QBezier *bezier, const QPointF &a, const QPointF &b, qreal alphaA, qreal alphaB, int path)
{
    if (qFuzzyCompare(alphaA, alphaB))
        return;

    if (a == b) {
        int v = insert(a);

        addBezierEdge(bezier, v, v, alphaA, alphaB, path);
    } else {
        int va = insert(a);
        int vb = insert(b);

        addBezierEdge(bezier, va, vb, alphaA, alphaB, path);
    }
}

int QWingedEdge::insert(const QPathVertex &vertex)
{
    if (!m_vertices.isEmpty()) {
        const QPathVertex &last = m_vertices.last();
        if (vertex.x == last.x && vertex.y == last.y)
            return m_vertices.size() - 1;

        for (int i = 0; i < m_vertices.size(); ++i) {
            const QPathVertex &v = m_vertices.at(i);
            if (qFuzzyCompare(v.x, vertex.x) && qFuzzyCompare(v.y, vertex.y)) {
                return i;
            }
        }
    }

    m_vertices << vertex;
    return m_vertices.size() - 1;
}

static void addLineTo(QPainterPath &path, const QPointF &point)
{
    const int elementCount = path.elementCount();
    if (elementCount >= 2) {
        const QPainterPath::Element &middle = path.elementAt(elementCount - 1);
        if (middle.type == QPainterPath::LineToElement) {
            const QPointF first = path.elementAt(elementCount - 2);
            const QPointF d1 = point - first;
            const QPointF d2 = middle - first;

            const QPointF p(-d1.y(), d1.x());

            if (qFuzzyCompare(dot(p, d2) + 1, 1)) {
                path.setElementPositionAt(elementCount - 1, point.x(), point.y());
                return;
            }
        }
    }

    path.lineTo(point);
}

static void add(QPainterPath &path, const QWingedEdge &list, int edge, QPathEdge::Traversal traversal)
{
    QWingedEdge::TraversalStatus status;
    status.edge = edge;
    status.traversal = traversal;
    status.direction = QPathEdge::Forward;

    const QBezier *bezier = 0;
    qreal t0 = 1;
    qreal t1 = 0;
    bool forward = true;

    path.moveTo(*list.vertex(list.edge(edge)->first));

    do {
        const QPathEdge *ep = list.edge(status.edge);

        if (ep->bezier != bezier || (bezier && t0 != ep->t1 && t1 != ep->t0)) {
            if (bezier) {
                QBezier sub = bezier->bezierOnInterval(t0, t1);

                if (forward)
                    path.cubicTo(sub.pt2(), sub.pt3(), sub.pt4());
                else
                    path.cubicTo(sub.pt3(), sub.pt2(), sub.pt1());
            }

            bezier = ep->bezier;
            t0 = 1;
            t1 = 0;
            forward = status.direction == QPathEdge::Forward;
        }

        if (ep->bezier) {
            t0 = qMin(t0, ep->t0);
            t1 = qMax(t1, ep->t1);
        } else
            addLineTo(path, *list.vertex(ep->vertex(status.direction)));

        if (status.traversal == QPathEdge::LeftTraversal)
            ep->flag &= ~16;
        else
            ep->flag &= ~32;

        status = list.next(status);
    } while (status.edge != edge);

    if (bezier) {
        QBezier sub = bezier->bezierOnInterval(t0, t1);
        if (forward)
            path.cubicTo(sub.pt2(), sub.pt3(), sub.pt4());
        else
            path.cubicTo(sub.pt3(), sub.pt2(), sub.pt1());
    }
}

void QWingedEdge::simplify()
{
    for (int i = 0; i < edgeCount(); ++i) {
        const QPathEdge *ep = edge(i);

        // if both sides are part of the inside then we can collapse the edge
        int flag = 0x3 << 4;
        if ((ep->flag & flag) == flag) {
            removeEdge(i);

            ep->flag &= ~flag;
        }
    }
}

QPainterPath QWingedEdge::toPath() const
{
    QPainterPath path;

    for (int i = 0; i < edgeCount(); ++i) {
        const QPathEdge *ep = edge(i);

        if (ep->flag & 16) {
            add(path, *this, i, QPathEdge::LeftTraversal);
        }

        if (ep->flag & 32)
            add(path, *this, i, QPathEdge::RightTraversal);
    }

    return path;
}

bool QPathClipper::intersect()
{
    if (subjectPath == clipPath)
        return true;

    QRectF r1 = subjectPath.controlPointRect();
    QRectF r2 = clipPath.controlPointRect();
    if (qMax(r1.x(), r2.x()) > qMin(r1.x() + r1.width(), r2.x() + r2.width()) ||
        qMax(r1.y(), r2.y()) > qMin(r1.y() + r1.height(), r2.y() + r2.height())) {
        // no way we could intersect
        return false;
    }

    bool subjectIsRect = pathToRect(subjectPath);
    bool clipIsRect = pathToRect(clipPath);

    if (subjectIsRect && clipIsRect)
        return true;
    else if (subjectIsRect)
        return clipPath.intersects(r1);
    else if (clipIsRect)
        return subjectPath.intersects(r2);

    QPathSegments a;
    a.setPath(subjectPath);
    QPathSegments b;
    b.setPath(clipPath);

    QIntersectionFinder finder;
    if (finder.hasIntersections(a, b))
        return true;

    for (int i = 0; i < clipPath.elementCount(); ++i) {
        if (clipPath.elementAt(i).type == QPainterPath::MoveToElement) {
            const QPointF point = clipPath.elementAt(i);
            if (r1.contains(point) && subjectPath.contains(point))
                return true;
        }
    }

    for (int i = 0; i < subjectPath.elementCount(); ++i) {
        if (subjectPath.elementAt(i).type == QPainterPath::MoveToElement) {
            const QPointF point = subjectPath.elementAt(i);
            if (r2.contains(point) && clipPath.contains(point))
                return true;
        }
    }

    return false;
}

bool QPathClipper::contains()
{
    if (subjectPath == clipPath)
        return false;

    QRectF r1 = subjectPath.controlPointRect();
    QRectF r2 = clipPath.controlPointRect();
    if (qMax(r1.x(), r2.x()) > qMin(r1.x() + r1.width(), r2.x() + r2.width()) ||
        qMax(r1.y(), r2.y()) > qMin(r1.y() + r1.height(), r2.y() + r2.height())) {
        // no intersection -> not contained
        return false;
    }

    bool clipIsRect = pathToRect(clipPath);
    if (clipIsRect)
        return subjectPath.contains(r2);

    QPathSegments a;
    a.setPath(subjectPath);
    QPathSegments b;
    b.setPath(clipPath);

    QIntersectionFinder finder;
    if (finder.hasIntersections(a, b))
        return false;

    for (int i = 0; i < clipPath.elementCount(); ++i) {
        if (clipPath.elementAt(i).type == QPainterPath::MoveToElement) {
            const QPointF point = clipPath.elementAt(i);
            if (!r1.contains(point) || !subjectPath.contains(point))
                return false;
        }
    }

    return true;
}

QPathClipper::QPathClipper(const QPainterPath &subject,
                           const QPainterPath &clip)
    : subjectPath(subject)
    , clipPath(clip)
{
    aMask = subjectPath.fillRule() == Qt::WindingFill ? ~0x0 : 0x1;
    bMask = clipPath.fillRule() == Qt::WindingFill ? ~0x0 : 0x1;
}

template <typename Iterator, typename Equality>
Iterator qRemoveDuplicates(Iterator begin, Iterator end, Equality eq)
{
    if (begin == end)
        return end;

    Iterator last = begin;
    ++begin;
    Iterator insert = begin;
    for (Iterator it = begin; it != end; ++it) {
        if (!eq(*it, *last)) {
            *insert++ = *it;
            last = it;
        }
    }

    return insert;
}

static void clear(QWingedEdge& list, int edge, QPathEdge::Traversal traversal)
{
    QWingedEdge::TraversalStatus status;
    status.edge = edge;
    status.traversal = traversal;
    status.direction = QPathEdge::Forward;

    do {
        if (status.traversal == QPathEdge::LeftTraversal)
            list.edge(status.edge)->flag |= 1;
        else
            list.edge(status.edge)->flag |= 2;

        status = list.next(status);
    } while (status.edge != edge);
}

template <typename InputIterator>
InputIterator qFuzzyFind(InputIterator first, InputIterator last, qreal val)
{
    while (first != last && !qFuzzyCompare(qreal(*first), qreal(val)))
        ++first;
    return first;
}

static bool fuzzyCompare(qreal a, qreal b)
{
    return qFuzzyCompare(a, b);
}

static bool pathToRect(const QPainterPath &path, QRectF *rect)
{
    if (path.elementCount() != 5)
        return false;

    const bool mightBeRect = path.elementAt(0).isMoveTo()
        && path.elementAt(1).isLineTo()
        && path.elementAt(2).isLineTo()
        && path.elementAt(3).isLineTo()
        && path.elementAt(4).isLineTo();

    if (!mightBeRect)
        return false;

    const qreal x1 = path.elementAt(0).x;
    const qreal y1 = path.elementAt(0).y;

    const qreal x2 = path.elementAt(1).x;
    const qreal y2 = path.elementAt(2).y;

    if (path.elementAt(1).y != y1)
        return false;

    if (path.elementAt(2).x != x2)
        return false;

    if (path.elementAt(3).x != x1 || path.elementAt(3).y != y2)
        return false;

    if (path.elementAt(4).x != x1 || path.elementAt(4).y != y1)
        return false;

    if (rect)
        *rect = QRectF(QPointF(x1, y1), QPointF(x2, y2));

    return true;
}


QPainterPath QPathClipper::clip(Operation operation)
{
    op = operation;

    if (op != Simplify) {
        if (subjectPath == clipPath)
            return op == BoolSub ? QPainterPath() : subjectPath;

        const QRectF clipBounds = clipPath.boundingRect();
        const QRectF subjectBounds = subjectPath.boundingRect();

        if (!clipBounds.intersects(subjectBounds)) {
            switch (op) {
            case BoolSub:
                return subjectPath;
            case BoolAnd:
                return QPainterPath();
            case BoolOr: {
                QPainterPath result = subjectPath;
                if (result.fillRule() == clipPath.fillRule()) {
                    result.addPath(clipPath);
                } else if (result.fillRule() == Qt::WindingFill) {
                    result = result.simplified();
                    result.addPath(clipPath);
                } else {
                    result.addPath(clipPath.simplified());
                }
                return result;
             }
            default:
                break;
            }
        }

        if (clipBounds.contains(subjectBounds)) {
            QRectF clipRect;
            if (pathToRect(clipPath, &clipRect) && clipRect.contains(subjectBounds)) {
                switch (op) {
                case BoolSub:
                    return QPainterPath();
                case BoolAnd:
                    return subjectPath;
                case BoolOr:
                    return clipPath;
                default:
                    break;
                }
            }
        } else if (subjectBounds.contains(clipBounds)) {
            QRectF subjectRect;
            if (pathToRect(subjectPath, &subjectRect) && subjectRect.contains(clipBounds)) {
                switch (op) {
                case BoolSub:
                    if (clipPath.fillRule() == Qt::OddEvenFill) {
                        QPainterPath result = clipPath;
                        result.addRect(subjectRect);
                        return result;
                    } else {
                        QPainterPath result = clipPath.simplified();
                        result.addRect(subjectRect);
                        return result;
                    }
                    break;
                case BoolAnd:
                    return clipPath;
                case BoolOr:
                    return subjectPath;
                default:
                    break;
                }
            }
        }
    }

    QWingedEdge list(subjectPath, clipPath);

    doClip(list, ClipMode);

    QPainterPath path = list.toPath();
    return path;
}

bool QPathClipper::doClip(QWingedEdge &list, ClipperMode mode)
{
    QVector<qreal> y_coords;
    y_coords.reserve(list.vertexCount());
    for (int i = 0; i < list.vertexCount(); ++i)
        y_coords << list.vertex(i)->y;

    qSort(y_coords.begin(), y_coords.end());
    y_coords.resize(qRemoveDuplicates(y_coords.begin(), y_coords.end(), fuzzyCompare) - y_coords.begin());

#ifdef QDEBUG_CLIPPER
    printf("sorted y coords:\n");
    for (int i = 0; i < y_coords.size(); ++i) {
        printf("%.9f\n", y_coords[i]);
    }
#endif

    bool found;
    do {
        found = false;
        int index = 0;
        qreal maxHeight = 0;
        for (int i = 0; i < list.edgeCount(); ++i) {
            QPathEdge *edge = list.edge(i);

            // have both sides of this edge already been handled?
            if ((edge->flag & 0x3) == 0x3)
                continue;

            QPathVertex *a = list.vertex(edge->first);
            QPathVertex *b = list.vertex(edge->second);

            if (qFuzzyCompare(a->y, b->y))
                continue;

            found = true;

            qreal height = qAbs(a->y - b->y);
            if (height > maxHeight) {
                index = i;
                maxHeight = height;
            }
        }

        if (found) {
            QPathEdge *edge = list.edge(index);

            QPathVertex *a = list.vertex(edge->first);
            QPathVertex *b = list.vertex(edge->second);

            // FIXME: this can be optimized by using binary search
            const int first = qFuzzyFind(y_coords.begin(), y_coords.end(), qMin(a->y, b->y)) - y_coords.begin();
            const int last = qFuzzyFind(y_coords.begin() + first, y_coords.end(), qMax(a->y, b->y)) - y_coords.begin();

            Q_ASSERT(first < y_coords.size() - 1);
            Q_ASSERT(last < y_coords.size());

            qreal bestY = 0.5 * (y_coords[first] + y_coords[first+1]);
            qreal biggestGap = y_coords[first+1] - y_coords[first];

            for (int i = first + 1; i < last; ++i) {
                qreal gap = y_coords[i+1] - y_coords[i];

                if (gap > biggestGap) {
                    bestY = 0.5 * (y_coords[i] + y_coords[i+1]);
                    biggestGap = gap;
                }
            }

#ifdef QDEBUG_CLIPPER
            printf("y: %.9f, gap: %.9f\n", bestY, biggestGap);
#endif

            if (handleCrossingEdges(list, bestY, mode) && mode == CheckMode)
                return true;

            edge->flag |= 0x3;
        }
    } while (found);

    if (mode == ClipMode)
        list.simplify();

    return false;
}

static void traverse(QWingedEdge &list, int edge, QPathEdge::Traversal traversal)
{
    QWingedEdge::TraversalStatus status;
    status.edge = edge;
    status.traversal = traversal;
    status.direction = QPathEdge::Forward;

    do {
        int flag = status.traversal == QPathEdge::LeftTraversal ? 1 : 2;

        QPathEdge *ep = list.edge(status.edge);

        ep->flag |= (flag | (flag << 4));

#ifdef QDEBUG_CLIPPER
        qDebug() << "traverse: adding edge " << status.edge << ", mask:" << (flag << 4) <<ep->flag;
#endif

        status = list.next(status);
    } while (status.edge != edge);
}

struct QCrossingEdge
{
    int edge;
    qreal x;

    bool operator<(const QCrossingEdge &edge) const
    {
        return x < edge.x;
    }
};

static bool bool_op(bool a, bool b, QPathClipper::Operation op)
{
    switch (op) {
    case QPathClipper::BoolAnd:
        return a && b;
    case QPathClipper::BoolOr: // fall-through
    case QPathClipper::Simplify:
        return a || b;
    case QPathClipper::BoolSub:
        return a && !b;
    default:
        Q_ASSERT(false);
        return false;
    }
}

bool QWingedEdge::isInside(qreal x, qreal y) const
{
    int winding = 0;
    for (int i = 0; i < edgeCount(); ++i) {
        const QPathEdge *ep = edge(i);

        // left xor right
        int w = ((ep->flag >> 4) ^ (ep->flag >> 5)) & 1;

        if (!w)
            continue;

        QPointF a = *vertex(ep->first);
        QPointF b = *vertex(ep->second);

        if ((a.y() < y && b.y() > y) || (a.y() > y && b.y() < y)) {
            if (ep->bezier) {
                qreal maxX = qMax(a.x(), qMax(b.x(), qMax(ep->bezier->x2, ep->bezier->x3)));
                qreal minX = qMin(a.x(), qMin(b.x(), qMin(ep->bezier->x2, ep->bezier->x3)));

                if (minX > x) {
                    winding += w;
                } else if (maxX > x) {
                    const qreal t = ep->bezier->tForY(ep->t0, ep->t1, y);
                    const qreal intersection = ep->bezier->pointAt(t).x();

                    if (intersection > x)
                        winding += w;
                }
            } else {
                qreal intersectionX = a.x() + (b.x() - a.x()) * (y - a.y()) / (b.y() - a.y());

                if (intersectionX > x)
                    winding += w;
            }
        }
    }

    return winding & 1;
}

static QVector<QCrossingEdge> findCrossings(const QWingedEdge &list, qreal y)
{
    QVector<QCrossingEdge> crossings;
    for (int i = 0; i < list.edgeCount(); ++i) {
        const QPathEdge *edge = list.edge(i);
        QPointF a = *list.vertex(edge->first);
        QPointF b = *list.vertex(edge->second);

        if ((a.y() < y && b.y() > y) || (a.y() > y && b.y() < y)) {
            if (edge->bezier) {
                const qreal t = edge->bezier->tForY(edge->t0, edge->t1, y);
                const qreal intersection = edge->bezier->pointAt(t).x();

                const QCrossingEdge edge = { i, intersection };
                crossings << edge;
            } else {
                const qreal intersection = a.x() + (b.x() - a.x()) * (y - a.y()) / (b.y() - a.y());
                const QCrossingEdge edge = { i, intersection };
                crossings << edge;
            }
        }
    }
    return crossings;
}

bool QPathClipper::handleCrossingEdges(QWingedEdge &list, qreal y, ClipperMode mode)
{
    QVector<QCrossingEdge> crossings = findCrossings(list, y);

    Q_ASSERT(!crossings.isEmpty());
    qSort(crossings.begin(), crossings.end());

    int windingA = 0;
    int windingB = 0;

    int windingD = 0;

#ifdef QDEBUG_CLIPPER
    qDebug() << "crossings:" << crossings.size();
#endif
    for (int i = 0; i < crossings.size() - 1; ++i) {
        int ei = crossings.at(i).edge;
        const QPathEdge *edge = list.edge(ei);

        windingA += edge->windingA;
        windingB += edge->windingB;

        const bool hasLeft = (edge->flag >> 4) & 1;
        const bool hasRight = (edge->flag >> 4) & 2;

        windingD += hasLeft ^ hasRight;

        const bool inA = (windingA & aMask) != 0;
        const bool inB = (windingB & bMask) != 0;
        const bool inD = (windingD & 0x1) != 0;

        const bool inside = bool_op(inA, inB, op);
        const bool add = inD ^ inside;

#ifdef QDEBUG_CLIPPER
        printf("y %f, x %f, inA: %d, inB: %d, inD: %d, inside: %d, flag: %x, bezier: %p, edge: %d\n", y, crossings.at(i).x, inA, inB, inD, inside, edge->flag, edge->bezier, ei);
#endif

        if (add) {
            if (mode == CheckMode)
                return true;

            qreal y0 = list.vertex(edge->first)->y;
            qreal y1 = list.vertex(edge->second)->y;

            if (y0 < y1) {
                if (!(edge->flag & 1))
                    traverse(list, ei, QPathEdge::LeftTraversal);

                if (!(edge->flag & 2))
                    clear(list, ei, QPathEdge::RightTraversal);
            } else {
                if (!(edge->flag & 1))
                    clear(list, ei, QPathEdge::LeftTraversal);

                if (!(edge->flag & 2))
                    traverse(list, ei, QPathEdge::RightTraversal);
            }

            ++windingD;
        } else {
            if (!(edge->flag & 1))
                clear(list, ei, QPathEdge::LeftTraversal);

            if (!(edge->flag & 2))
                clear(list, ei, QPathEdge::RightTraversal);
        }
    }

    return false;
}

QT_END_NAMESPACE
