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

#ifndef QTREEVIEW_P_H
#define QTREEVIEW_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "private/qabstractitemview_p.h"

#ifndef QT_NO_TREEVIEW

QT_BEGIN_NAMESPACE

struct QTreeViewItem
{
    QTreeViewItem() : expanded(false), spanning(false), total(0), level(0), height(0) {}
    QModelIndex index; // we remove items whenever the indexes are invalidated
    uint expanded : 1;
    uint spanning : 1;
    uint total : 30; // total number of children visible
    uint level : 16; // indentation
    int height : 16; // row height
};

class QTreeViewPrivate : public QAbstractItemViewPrivate
{
    Q_DECLARE_PUBLIC(QTreeView)
public:

    QTreeViewPrivate()
        : QAbstractItemViewPrivate(),
          header(0), indent(20), lastViewedItem(0), defaultItemHeight(-1),
          uniformRowHeights(false), rootDecoration(true),
          itemsExpandable(true), sortingEnabled(false),
          expandsOnDoubleClick(true),
          allColumnsShowFocus(false),
          animationsEnabled(false), columnResizeTimerID(0),
          autoExpandDelay(-1), hoverBranch(-1), geometryRecursionBlock(false) {}

    ~QTreeViewPrivate() {}
    void initialize();

    struct AnimatedOperation
    {
        enum Type { Expand, Collapse };
        int item;
        int top;
        int duration;
        Type type;
        QPixmap before;
        QPixmap after;
    };

    void expand(int item, bool emitSignal);
    void collapse(int item, bool emitSignal);

    void prepareAnimatedOperation(int item, AnimatedOperation::Type type);
    void beginAnimatedOperation();
    void _q_endAnimatedOperation();
    void drawAnimatedOperation(QPainter *painter) const;
    QPixmap renderTreeToPixmapForAnimation(const QRect &rect) const;

    inline QRect animationRect() const
        { return QRect(0, animatedOperation.top, viewport->width(),
                       viewport->height() - animatedOperation.top); }

    void _q_currentChanged(const QModelIndex&, const QModelIndex&);
    void _q_columnsAboutToBeRemoved(const QModelIndex &, int, int);
    void _q_columnsRemoved(const QModelIndex &, int, int);
    void _q_modelAboutToBeReset();
    void _q_animate();
    void _q_sortIndicatorChanged(int column, Qt::SortOrder order);
    void _q_modelDestroyed();

    void layout(int item);

    int pageUp(int item) const;
    int pageDown(int item) const;

    int itemHeight(int item) const;
    int indentationForItem(int item) const;
    int coordinateForItem(int item) const;
    int itemAtCoordinate(int coordinate) const;

    int viewIndex(const QModelIndex &index) const;
    QModelIndex modelIndex(int i) const;

    int firstVisibleItem(int *offset = 0) const;
    int columnAt(int x) const;
    bool hasVisibleChildren( const QModelIndex& parent) const;

    void relayout(const QModelIndex &parent);
    bool expandOrCollapseItemAtPos(const QPoint &pos);

    void updateScrollBars();

    int itemDecorationAt(const QPoint &pos) const;
    QRect itemDecorationRect(const QModelIndex &index) const;


    QList<QPair<int, int> > columnRanges(const QModelIndex &topIndex, const QModelIndex &bottomIndex) const;
    void select(const QModelIndex &start, const QModelIndex &stop, QItemSelectionModel::SelectionFlags command);

    QPair<int,int> startAndEndColumns(const QRect &rect) const;

    void updateChildCount(const int parentItem, const int delta);
    void rowsRemoved(const QModelIndex &parent,
                     int start, int end, bool before);

    void paintAlternatingRowColors(QPainter *painter, QStyleOptionViewItemV4 *option, int y, int bottom) const;

    QHeaderView *header;
    int indent;

    mutable QVector<QTreeViewItem> viewItems;
    mutable int lastViewedItem;
    int defaultItemHeight; // this is just a number; contentsHeight() / numItems
    bool uniformRowHeights; // used when all rows have the same height
    bool rootDecoration;
    bool itemsExpandable;
    bool sortingEnabled;
    bool expandsOnDoubleClick;
    bool allColumnsShowFocus;

    // used for drawing
    mutable QPair<int,int> leftAndRight;
    mutable int current;
    mutable bool spanning;

    // used when expanding and collapsing items
    QSet<QPersistentModelIndex> expandedIndexes;
    QStack<bool> expandParent;
    AnimatedOperation animatedOperation;
    bool animationsEnabled;

    inline bool storeExpanded(const QPersistentModelIndex &idx) {
        if (expandedIndexes.contains(idx))
            return false;
        expandedIndexes.insert(idx);
        return true;
    }

    inline bool isIndexExpanded(const QModelIndex &idx) const {
        //We first check if the idx is a QPersistentModelIndex, because creating QPersistentModelIndex is slow
        return isPersistent(idx) && expandedIndexes.contains(idx);
    }

    // used when hiding and showing items
    QSet<QPersistentModelIndex> hiddenIndexes;

    inline bool isRowHidden(const QModelIndex &idx) const {
        //We first check if the idx is a QPersistentModelIndex, because creating QPersistentModelIndex is slow
        return isPersistent(idx) && hiddenIndexes.contains(idx);
    }

    inline bool isItemHiddenOrDisabled(int i) const {
        if (i < 0 || i >= viewItems.count())
            return false;
        const QModelIndex index = viewItems.at(i).index;
        return isRowHidden(index) || !isIndexEnabled(index);
    }

    inline int above(int item) const
        { int i = item; while (isItemHiddenOrDisabled(--item)){} return item < 0 ? i : item; }
    inline int below(int item) const
        { int i = item; while (isItemHiddenOrDisabled(++item)){} return item >= viewItems.count() ? i : item; }
    inline void invalidateHeightCache(int item) const
        { viewItems[item].height = 0; }

    // used for spanning rows
    QVector<QPersistentModelIndex> spanningIndexes;

    // used for updating resized columns
    int columnResizeTimerID;
    QList<int> columnsToUpdate;

    // used for the automatic opening of nodes during DND
    int autoExpandDelay;
    QBasicTimer openTimer;

    // used for drawing hilighted expand/collapse indicators
    int hoverBranch;

    // used for blocking recursion when calling setViewportMargins from updateGeometries
    bool geometryRecursionBlock;
};

QT_END_NAMESPACE

#endif // QT_NO_TREEVIEW

#endif // QTREEVIEW_P_H
