/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#include "qbsptree_p.h"

QT_BEGIN_NAMESPACE

QBspTree::QBspTree() : depth(6), visited(0) {}

void QBspTree::create(int n, int d)
{
    // simple heuristics to find the best tree depth
    if (d == -1) {
        int c;
        for (c = 0; n; ++c)
            n = n / 10;
        depth = c << 1;
    } else {
        depth = d;
    }
    depth = qMax(depth, uint(1));

    nodes.resize((1 << depth) - 1); // resize to number of nodes
    leaves.resize(1 << depth); // resize to number of leaves
}

void QBspTree::destroy()
{
    leaves.clear();
    nodes.clear();
}

void QBspTree::climbTree(const QRect &rect, callback *function, QBspTreeData data)
{
    if (nodes.isEmpty())
        return;
    ++visited;
    climbTree(rect, function, data, 0);
}

void QBspTree::climbTree(const QRect &area, callback *function, QBspTreeData data, int index)
{
    if (index >= nodes.count()) { // the index points to a leaf
        Q_ASSERT(!nodes.isEmpty());
        function(leaf(index - nodes.count()), area, visited, data);
        return;
    }

    Node::Type t = (Node::Type) nodes.at(index).type;

    int pos = nodes.at(index).pos;
    int idx = firstChildIndex(index);
    if (t == Node::VerticalPlane) {
        if (area.left() < pos)
            climbTree(area, function, data, idx); // back
        if (area.right() >= pos)
            climbTree(area, function, data, idx + 1); // front
    } else {
        if (area.top() < pos)
            climbTree(area, function, data, idx); // back
        if (area.bottom() >= pos)
            climbTree(area, function, data, idx + 1); // front
    }
}

void QBspTree::init(const QRect &area, int depth, NodeType type, int index)
{
    Node::Type t = Node::None; // t should never have this value
    if (type == Node::Both) // if both planes are specified, use 2d bsp
        t = (depth & 1) ? Node::HorizontalPlane : Node::VerticalPlane;
    else
        t = type;
    QPoint center = area.center();
    nodes[index].pos = (t == Node::VerticalPlane ? center.x() : center.y());
    nodes[index].type = t;

    QRect front = area;
    QRect back = area;

    if (t == Node::VerticalPlane) {
        front.setLeft(center.x());
        back.setRight(center.x() - 1); // front includes the center
    } else { // t == Node::HorizontalPlane
        front.setTop(center.y());
        back.setBottom(center.y() - 1);
    }

    int idx = firstChildIndex(index);
    if (--depth) {
        init(back, depth, type, idx);
        init(front, depth, type, idx + 1);
    }
}

void QBspTree::insert(QVector<int> &leaf, const QRect &, uint, QBspTreeData data)
{
    leaf.append(data.i);
}

void QBspTree::remove(QVector<int> &leaf, const QRect &, uint, QBspTreeData data)
{
    int i = leaf.indexOf(data.i);
    if (i != -1)
        leaf.remove(i);
}

QT_END_NAMESPACE
