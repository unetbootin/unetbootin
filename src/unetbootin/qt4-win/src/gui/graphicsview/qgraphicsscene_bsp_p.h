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

#ifndef QGRAPHICSSCENEBSPTREE_P_H
#define QGRAPHICSSCENEBSPTREE_P_H

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

#include <QtCore/qlist.h>

#if !defined(QT_NO_GRAPHICSVIEW) || (QT_EDITION & QT_MODULE_GRAPHICSVIEW) != QT_MODULE_GRAPHICSVIEW

#include <QtCore/qrect.h>
#include <QtCore/qset.h>
#include <QtCore/qvector.h>

QT_BEGIN_NAMESPACE

class QGraphicsItem;
class QGraphicsSceneBspTreeVisitor;
class QGraphicsSceneInsertItemBspTreeVisitor;
class QGraphicsSceneRemoveItemBspTreeVisitor;
class QGraphicsSceneFindItemBspTreeVisitor;

class QGraphicsSceneBspTree
{
public:
    struct Node
    {
        enum Type { Horizontal, Vertical, Leaf };
        union {
            qreal offset;
            int leafIndex;
        };
        Type type;
    };

    QGraphicsSceneBspTree();
    ~QGraphicsSceneBspTree();

    void initialize(const QRectF &rect, int depth);
    void clear();

    void insertItem(QGraphicsItem *item, const QRectF &rect);
    void removeItem(QGraphicsItem *item, const QRectF &rect);
    void removeItems(const QSet<QGraphicsItem *> &items);

    QList<QGraphicsItem *> items(const QRectF &rect);
    QList<QGraphicsItem *> items(const QPointF &pos);
    int leafCount() const;

    inline int firstChildIndex(int index) const
    { return index * 2 + 1; }

    inline int parentIndex(int index) const
    { return index > 0 ? ((index & 1) ? ((index - 1) / 2) : ((index - 2) / 2)) : -1; }

    QString debug(int index) const;

private:
    void initialize(const QRectF &rect, int depth, int index);
    void climbTree(QGraphicsSceneBspTreeVisitor *visitor, const QPointF &pos, int index = 0);
    void climbTree(QGraphicsSceneBspTreeVisitor *visitor, const QRectF &rect, int index = 0);

    void findItems(QList<QGraphicsItem *> *foundItems, const QRectF &rect, int index);
    void findItems(QList<QGraphicsItem *> *foundItems, const QPointF &pos, int index);
    QRectF rectForIndex(int index) const;

    QVector<Node> nodes;
    QVector<QList<QGraphicsItem *> > leaves;
    int leafCnt;
    QRectF rect;

    QGraphicsSceneInsertItemBspTreeVisitor *insertVisitor;
    QGraphicsSceneRemoveItemBspTreeVisitor *removeVisitor;
    QGraphicsSceneFindItemBspTreeVisitor *findVisitor;
};

class QGraphicsSceneBspTreeVisitor
{
public:
    virtual ~QGraphicsSceneBspTreeVisitor() { }
    virtual void visit(QList<QGraphicsItem *> *items) = 0;
};

QT_END_NAMESPACE

#endif // QT_NO_GRAPHICSVIEW

#endif // QGRAPHICSSCENEBSPTREE_P_H
