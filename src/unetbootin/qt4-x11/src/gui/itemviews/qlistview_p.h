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

#ifndef QLISTVIEW_P_H
#define QLISTVIEW_P_H

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

#include "private/qabstractitemview_p.h"
#include "qrubberband.h"
#include "qbitarray.h"
#include "qbsptree_p.h"
#include <limits.h>
#include <qscrollbar.h>

#ifndef QT_NO_LISTVIEW

QT_BEGIN_NAMESPACE

class QListViewItem
{
    friend class QListViewPrivate;
    friend class QStaticListViewBase;
    friend class QDynamicListViewBase;
public:
    inline QListViewItem()
        : x(-1), y(-1), w(0), h(0), indexHint(-1), visited(0xffff) {}
    inline QListViewItem(const QListViewItem &other)
        : x(other.x), y(other.y), w(other.w), h(other.h),
          indexHint(other.indexHint), visited(other.visited) {}
    inline QListViewItem(QRect r, int i)
        : x(r.x()), y(r.y()), w(qMin(r.width(), SHRT_MAX)), h(qMin(r.height(), SHRT_MAX)),
          indexHint(i), visited(0xffff) {}
    inline bool operator==(const QListViewItem &other) const {
        return (x == other.x && y == other.y && w == other.w && h == other.h &&
                indexHint == other.indexHint); }
    inline bool operator!=(const QListViewItem &other) const
        { return !(*this == other); }
    inline bool isValid() const
        { return (x > -1) && (y > -1) && (w > 0) && (h > 0) && (indexHint > -1); }
    inline void invalidate()
        { x = -1; y = -1; w = 0; h = 0; }
    inline void resize(const QSize &size)
        { w = qMin(size.width(), SHRT_MAX); h = qMin(size.height(), SHRT_MAX); }
    inline void move(const QPoint &position)
        { x = position.x(); y = position.y(); }
    inline int width() const { return w; }
    inline int height() const { return h; }
private:
    inline QRect rect() const
        { return QRect(x, y, w, h); }
    int x, y;
    short w, h;
    mutable int indexHint;
    uint visited;
};

struct QListViewLayoutInfo
{
    QRect bounds;
    QSize grid;
    int spacing;
    int first;
    int last;
    bool wrap;
    QListView::Flow flow;
    int max;
};

class QListView;
class QListViewPrivate;

class QCommonListViewBase
{
public:
    inline QCommonListViewBase(QListView *q, QListViewPrivate *d) : dd(d), qq(q) {}

    inline int spacing() const;
    inline bool isWrapping() const;
    inline QSize gridSize() const;
    inline QListView::Flow flow() const;
    inline QListView::Movement movement() const;

    inline QPoint offset() const;
    inline QPoint pressedPosition() const;
    inline bool uniformItemSizes() const;
    inline int column() const;

    inline int verticalScrollBarValue() const;
    inline int horizontalScrollBarValue() const;
    inline QListView::ScrollMode verticalScrollMode() const;
    inline QListView::ScrollMode horizontalScrollMode() const;

    inline QModelIndex modelIndex(int row) const;
    inline int rowCount() const;

    inline QStyleOptionViewItemV4 viewOptions() const;
    inline QWidget *viewport() const;
    inline QRect clipRect() const;

    inline QSize cachedItemSize() const;
    inline QRect viewItemRect(const QListViewItem &item) const;
    inline QSize itemSize(const QStyleOptionViewItemV2 &opt, const QModelIndex &idx) const;
    inline QAbstractItemDelegate *delegate(const QModelIndex &idx) const;

    inline bool isHidden(int row) const;
    inline int hiddenCount() const;

    inline void clearIntersections() const;
    inline void appendToIntersections(const QModelIndex &idx) const;

    inline bool isRightToLeft() const;

    QListViewPrivate *dd;
    QListView *qq;
};

// ### rename to QListModeViewBase
class QStaticListViewBase : public QCommonListViewBase
{
    friend class QListViewPrivate;
public:
    QStaticListViewBase(QListView *q, QListViewPrivate *d) : QCommonListViewBase(q, d),
        batchStartRow(0), batchSavedDeltaSeg(0), batchSavedPosition(0) {}

    QVector<int> flowPositions;
    QVector<int> segmentPositions;
    QVector<int> segmentStartRows;
    QVector<int> segmentExtents;

    QSize contentsSize;

    // used when laying out in batches
    int batchStartRow;
    int batchSavedDeltaSeg;
    int batchSavedPosition;

    bool doBatchedItemLayout(const QListViewLayoutInfo &info, int max);

    QPoint initStaticLayout(const QListViewLayoutInfo &info);
    void doStaticLayout(const QListViewLayoutInfo &info);
    void intersectingStaticSet(const QRect &area) const;

    int itemIndex(const QListViewItem &item) const;

    int perItemScrollingPageSteps(int length, int bounds, bool wrap) const;

    int perItemScrollToValue(int index, int value, int height,
                             QAbstractItemView::ScrollHint hint,
                             Qt::Orientation orientation, bool wrap, int extent) const;

    QRect mapToViewport(const QRect &rect) const;

    QListViewItem indexToListViewItem(const QModelIndex &index) const;

    void scrollContentsBy(int &dx, int &dy);

    int verticalPerItemValue(int itemIndex, int verticalValue, int areaHeight,
                       bool above, bool below, bool wrap, QListView::ScrollHint hint, int itemHeight) const;
    int horizontalPerItemValue(int itemIndex, int horizontalValue, int areaWidth,
                       bool leftOf, bool rightOf, bool wrap, QListView::ScrollHint hint, int itemWidth) const;

    void clear();
};

// ### rename to QIconModeViewBase
class QDynamicListViewBase : public QCommonListViewBase
{
    friend class QListViewPrivate;
public:
    QDynamicListViewBase(QListView *q, QListViewPrivate *d) : QCommonListViewBase(q, d),
        batchStartRow(0), batchSavedDeltaSeg(0) {}

    QBspTree tree;
    QVector<QListViewItem> items;
    QBitArray moved;

    QSize contentsSize;

    QVector<QModelIndex> draggedItems; // indices to the tree.itemVector
    mutable QPoint draggedItemsPos;

    // used when laying out in batches
    int batchStartRow;
    int batchSavedDeltaSeg;

    void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    bool doBatchedItemLayout(const QListViewLayoutInfo &info, int max);

    void initBspTree(const QSize &contents);
    QPoint initDynamicLayout(const QListViewLayoutInfo &info);
    void doDynamicLayout(const QListViewLayoutInfo &info);
    void intersectingDynamicSet(const QRect &area) const;

    static void addLeaf(QVector<int> &leaf, const QRect &area,
                        uint visited, QBspTree::Data data);

    void insertItem(int index);
    void removeItem(int index);
    void moveItem(int index, const QPoint &dest);

    int itemIndex(const QListViewItem &item) const;

    void createItems(int to);
    void drawItems(QPainter *painter, const QVector<QModelIndex> &indexes) const;
    QRect itemsRect(const QVector<QModelIndex> &indexes) const;

    QPoint draggedItemsDelta() const;
    QRect draggedItemsRect() const;

    QPoint snapToGrid(const QPoint &pos) const;

    void scrollElasticBandBy(int dx, int dy);

    QListViewItem indexToListViewItem(const QModelIndex &index) const;

    void clear();
    void updateContentsSize();
};

class QListViewPrivate: public QAbstractItemViewPrivate
{
    Q_DECLARE_PUBLIC(QListView)
public:
    QListViewPrivate();
    ~QListViewPrivate();

    void clear();
    void prepareItemsLayout();

    bool doItemsLayout(int num);

    inline void intersectingSet(const QRect &area, bool doLayout = true) const {
        if (doLayout) executePostedLayout();
        QRect a = (q_func()->isRightToLeft() ? flipX(area.normalized()) : area.normalized());
        if (viewMode == QListView::ListMode) staticListView->intersectingStaticSet(a);
        else dynamicListView->intersectingDynamicSet(a);
    }

    // ### FIXME:
    inline void resetBatchStartRow()
        { if (viewMode == QListView::ListMode) staticListView->batchStartRow = 0;
              else dynamicListView->batchStartRow = 0; }
    inline int batchStartRow() const
        { return (viewMode == QListView::ListMode
          ? staticListView->batchStartRow : dynamicListView->batchStartRow); }
    inline QSize contentsSize() const
        { return (viewMode == QListView::ListMode
          ? staticListView->contentsSize : dynamicListView->contentsSize); }
    inline void setContentsSize(int w, int h)
        { if (viewMode == QListView::ListMode) staticListView->contentsSize = QSize(w, h);
          else dynamicListView->contentsSize = QSize(w, h); }

    inline int flipX(int x) const
        { return qMax(viewport->width(), contentsSize().width()) - x; }
    inline QPoint flipX(const QPoint &p) const
        { return QPoint(flipX(p.x()), p.y()); }
    inline QRect flipX(const QRect &r) const
        { return QRect(flipX(r.x()) - r.width(), r.y(), r.width(), r.height()); }
    inline QRect viewItemRect(const QListViewItem &item) const
        { if (q_func()->isRightToLeft()) return flipX(item.rect()); return item.rect(); }

    int itemIndex(const QListViewItem &item) const;
    QListViewItem indexToListViewItem(const QModelIndex &index) const;
    inline QModelIndex listViewItemToIndex(const QListViewItem &item) const
        { return model->index(itemIndex(item), column, root); }

    QRect mapToViewport(const QRect &rect, bool greedy = false) const;

    QModelIndex closestIndex(const QRect &target, const QVector<QModelIndex> &candidates) const;
    QSize itemSize(const QStyleOptionViewItem &option, const QModelIndex &index) const;

    bool selectionAllowed(const QModelIndex &index) const
        { if (viewMode == QListView::ListMode && !showElasticBand) return index.isValid(); return true; }

    int horizontalScrollToValue(const QModelIndex &index, const QRect &rect, QListView::ScrollHint hint) const;
    int verticalScrollToValue(const QModelIndex &index, const QRect &rect, QListView::ScrollHint hint) const;

    QItemSelection selection(const QRect &rect) const;
    void selectAll(QItemSelectionModel::SelectionFlags command);

    inline void setGridSize(const QSize &size) { grid = size; }
    inline QSize gridSize() const { return grid; }
    inline void setWrapping(bool b) { wrap = b; }
    inline bool isWrapping() const { return wrap; }
    inline void setSpacing(int s) { space = s; }
    inline int spacing() const { return space; }
    inline void setSelectionRectVisible(bool visible) { showElasticBand = visible; }
    inline bool isSelectionRectVisible() const { return showElasticBand; }

    inline QModelIndex modelIndex(int row) const { return model->index(row, column, root); }
    inline bool isHidden(int row) const { return hiddenRows.contains(model->index(row, 0, root)); }
    inline bool isHiddenOrDisabled(int row) const { return isHidden(row) || !isIndexEnabled(modelIndex(row)); }

    inline void removeCurrentAndDisabled(QVector<QModelIndex> *indexes, const QModelIndex &current) const {
        QVector<QModelIndex>::iterator it = indexes->begin();
        while (it != indexes->end()) {
            if (!isIndexEnabled(*it) || (*it) == current)
                indexes->erase(it);
            else
                ++it;
        }
    }

    void scrollElasticBandBy(int dx, int dy);

    // ### FIXME: we only need one at a time
    QDynamicListViewBase *dynamicListView;
    QStaticListViewBase *staticListView;

    // ### FIXME: see if we can move the members into the dynamic/static classes

    bool wrap;
    int space;
    QSize grid;

    QListView::Flow flow;
    QListView::Movement movement;
    QListView::ResizeMode resizeMode;
    QListView::LayoutMode layoutMode;
    QListView::ViewMode viewMode;

    // the properties controlling the
    // icon- or list-view modes
    enum ModeProperties {
        Wrap = 1,
        Spacing = 2,
        GridSize = 4,
        Flow = 8,
        Movement = 16,
        ResizeMode = 32,
        SelectionRectVisible = 64
    };

    uint modeProperties : 8;

    QRect layoutBounds;

    // used for intersecting set
    mutable QVector<QModelIndex> intersectVector;

    // timers
    QBasicTimer batchLayoutTimer;

    // used for hidden items
    QVector<QPersistentModelIndex> hiddenRows;

    int column;
    bool uniformItemSizes;
    mutable QSize cachedItemSize;
    int batchSize;

    QRect elasticBand;
    bool showElasticBand;
};

// inline implementations

inline int QCommonListViewBase::spacing() const { return dd->spacing(); }
inline bool QCommonListViewBase::isWrapping() const { return dd->isWrapping(); }
inline QSize QCommonListViewBase::gridSize() const { return dd->gridSize(); }
inline QListView::Flow QCommonListViewBase::flow() const { return dd->flow; }
inline QListView::Movement QCommonListViewBase::movement() const { return dd->movement; }

inline QPoint QCommonListViewBase::offset() const { return dd->offset(); }
inline QPoint QCommonListViewBase::pressedPosition() const { return dd->pressedPosition; }
inline bool QCommonListViewBase::uniformItemSizes() const { return dd->uniformItemSizes; }
inline int QCommonListViewBase::column() const { return dd->column; }

inline int QCommonListViewBase::verticalScrollBarValue() const { return qq->verticalScrollBar()->value(); }
inline int QCommonListViewBase::horizontalScrollBarValue() const { return qq->horizontalScrollBar()->value(); }
inline QListView::ScrollMode QCommonListViewBase::verticalScrollMode() const { return qq->verticalScrollMode(); }
inline QListView::ScrollMode QCommonListViewBase::horizontalScrollMode() const { return qq->horizontalScrollMode(); }

inline QModelIndex QCommonListViewBase::modelIndex(int row) const
    { return dd->model->index(row, dd->column, dd->root); }
inline int QCommonListViewBase::rowCount() const { return dd->model->rowCount(dd->root); }

inline QStyleOptionViewItemV4 QCommonListViewBase::viewOptions() const { return dd->viewOptionsV4(); }
inline QWidget *QCommonListViewBase::viewport() const { return dd->viewport; }
inline QRect QCommonListViewBase::clipRect() const { return dd->clipRect(); }

inline QSize QCommonListViewBase::cachedItemSize() const { return dd->cachedItemSize; }
inline QRect QCommonListViewBase::viewItemRect(const QListViewItem &item) const { return dd->viewItemRect(item); }
inline QSize QCommonListViewBase::itemSize(const QStyleOptionViewItemV2 &opt, const QModelIndex &idx) const
    { return dd->itemSize(opt, idx); }

inline QAbstractItemDelegate *QCommonListViewBase::delegate(const QModelIndex &idx) const
    { return dd->delegateForIndex(idx); }

inline bool QCommonListViewBase::isHidden(int row) const { return dd->isHidden(row); }
inline int QCommonListViewBase::hiddenCount() const { return dd->hiddenRows.count(); }

inline void QCommonListViewBase::clearIntersections() const { dd->intersectVector.clear(); }
inline void QCommonListViewBase::appendToIntersections(const QModelIndex &idx) const { dd->intersectVector.append(idx); }

inline bool QCommonListViewBase::isRightToLeft() const { return qq->isRightToLeft(); }

QT_END_NAMESPACE

#endif // QT_NO_LISTVIEW

#endif // QLISTVIEW_P_H
