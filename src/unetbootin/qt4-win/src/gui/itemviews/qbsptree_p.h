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

#ifndef QBSPTREE_P_H
#define QBSPTREE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <qvector.h>
#include <qrect.h>

QT_BEGIN_NAMESPACE

class QBspTree
{
public:

    struct Node
    {
        enum Type { None = 0, VerticalPlane = 1, HorizontalPlane = 2, Both = 3 };
        inline Node() : pos(0), type(None) {}
        int pos;
        Type type;
    };
    typedef Node::Type NodeType;

    struct Data
    {
        Data(void *p) : ptr(p) {}
        Data(int n) : i(n) {}
        union {
            void *ptr;
            int i;
        };
    };
    typedef QBspTree::Data QBspTreeData;
    typedef void callback(QVector<int> &leaf, const QRect &area, uint visited, QBspTreeData data);

    QBspTree();

    void create(int n, int d = -1);
    void destroy();

    inline void init(const QRect &area, NodeType type) { init(area, depth, type, 0); }

    void climbTree(const QRect &rect, callback *function, QBspTreeData data);

    inline int leafCount() const { return leaves.count(); }
    inline QVector<int> &leaf(int i) { return leaves[i]; }
    inline void insertLeaf(const QRect &r, int i) { climbTree(r, &insert, i, 0); }
    inline void removeLeaf(const QRect &r, int i) { climbTree(r, &remove, i, 0); }

protected:
    void init(const QRect &area, int depth, NodeType type, int index);
    void climbTree(const QRect &rect, callback *function, QBspTreeData data, int index);

    inline int parentIndex(int i) const { return (i & 1) ? ((i - 1) / 2) : ((i - 2) / 2); }
    inline int firstChildIndex(int i) const { return ((i * 2) + 1); }

    static void insert(QVector<int> &leaf, const QRect &area, uint visited, QBspTreeData data);
    static void remove(QVector<int> &leaf, const QRect &area, uint visited, QBspTreeData data);

private:
    uint depth;
    mutable uint visited;
    QVector<Node> nodes;
    mutable QVector< QVector<int> > leaves; // the leaves are just indices into the items
};

QT_END_NAMESPACE

#endif // QBSPTREE_P_H
