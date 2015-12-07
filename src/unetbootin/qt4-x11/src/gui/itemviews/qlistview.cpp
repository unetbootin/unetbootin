/***************************************************************************
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

#include "qlistview.h"

#ifndef QT_NO_LISTVIEW
#include <qabstractitemdelegate.h>
#include <qapplication.h>
#include <qpainter.h>
#include <qbitmap.h>
#include <qvector.h>
#include <qstyle.h>
#include <qevent.h>
#include <qscrollbar.h>
#include <qrubberband.h>
#include <private/qlistview_p.h>
#include <qdebug.h>
#ifndef QT_NO_ACCESSIBILITY
#include <qaccessible.h>
#endif

QT_BEGIN_NAMESPACE

/*!
    \class QListView

    \brief The QListView class provides a list or icon view onto a model.

    \ingroup model-view
    \ingroup advanced
    \mainclass

    A QListView presents items stored in a model, either as a simple
    non-hierarchical list, or as a collection of icons. This class is used
    to provide lists and icon views that were previously provided by the
    \c QListBox and \c QIconView classes, but using the more flexible
    approach provided by Qt's model/view architecture.

    The QListView class is one of the \l{Model/View Classes}
    and is part of Qt's \l{Model/View Programming}{model/view framework}.

    This view does not display horizontal or vertical headers; to display
    a list of items with a horizontal header, use QTreeView instead.

    QListView implements the interfaces defined by the
    QAbstractItemView class to allow it to display data provided by
    models derived from the QAbstractItemModel class.

    Items in a list view can be displayed using one of two view modes:
    In \l ListMode, the items are displayed in the form of a simple list;
    in \l IconMode, the list view takes the form of an \e{icon view} in
    which the items are displayed with icons like files in a file manager.
    By default, the list view is in \l ListMode. To change the view mode,
    use the setViewMode() function, and to determine the current view mode,
    use viewMode().

    Items in these views are laid out in the direction specified by the
    flow() of the list view. The items may be fixed in place, or allowed
    to move, depending on the view's movement() state.

    If the items in the model cannot be completely laid out in the
    direction of flow, they can be wrapped at the boundary of the view
    widget; this depends on isWrapping(). This property is useful when the
    items are being represented by an icon view.

    The resizeMode() and layoutMode() govern how and when the items are
    laid out. Items are spaced according to their spacing(), and can exist
    within a notional grid of size specified by gridSize(). The items can
    be rendered as large or small icons depending on their iconSize().

    \table 100%
    \row \o \inlineimage windowsxp-listview.png Screenshot of a Windows XP style list view
         \o \inlineimage macintosh-listview.png Screenshot of a Macintosh style table view
         \o \inlineimage plastique-listview.png Screenshot of a Plastique style table view
    \row \o A \l{Windows XP Style Widget Gallery}{Windows XP style} list view.
         \o A \l{Macintosh Style Widget Gallery}{Macintosh style} list view.
         \o A \l{Plastique Style Widget Gallery}{Plastique style} list view.
    \endtable

    \section1 Improving Performance

    It is possible to give the view hints about the data it is handling in order
    to improve its performance when displaying large numbers of items. One approach
    that can be taken for views that are intended to display items with equal sizes
    is to set the \l uniformItemSizes property to true.

    \sa {View Classes}, QTreeView, QTableView, QListWidget
*/

/*!
    \enum QListView::ViewMode

    \value ListMode The items are laid out using TopToBottom flow, with Small size and Static movement
    \value IconMode The items are laid out using LeftToRight flow, with Large size and Free movement
*/

/*!
  \enum QListView::Movement

  \value Static The items cannot be moved by the user.
  \value Free The items can be moved freely by the user.
  \value Snap The items snap to the specified grid when moved; see
  setGridSize().
*/

/*!
  \enum QListView::Flow

  \value LeftToRight The items are laid out in the view from the left
  to the right.
  \value TopToBottom The items are laid out in the view from the top
  to the bottom.
*/

/*!
  \enum QListView::ResizeMode

  \value Fixed The items will only be laid out the first time the view is shown.
  \value Adjust The items will be laid out every time the view is resized.
*/

/*!
  \enum QListView::LayoutMode

  \value SinglePass The items are laid out all at once.
  \value Batched The items are laid out in batches of \l batchSize items.
  \sa batchSize
*/

/*!
  \since 4.2
  \fn void QListView::indexesMoved(const QModelIndexList &indexes)

  This signal is emitted when the specified \a indexes are moved in the view.
*/

/*!
    Creates a new QListView with the given \a parent to view a model.
    Use setModel() to set the model.
*/
QListView::QListView(QWidget *parent)
    : QAbstractItemView(*new QListViewPrivate, parent)
{
    setViewMode(ListMode);
    setSelectionMode(SingleSelection);
    setAttribute(Qt::WA_MacShowFocusRect);
    Q_D(QListView);               // We rely on a qobject_cast for PM_DefaultFrameWidth to change
    d->updateStyledFrameWidths(); // hence we have to force an update now that the object has been constructed
}

/*!
  \internal
*/
QListView::QListView(QListViewPrivate &dd, QWidget *parent)
    : QAbstractItemView(dd, parent)
{
    setViewMode(ListMode);
    setSelectionMode(SingleSelection);
    setAttribute(Qt::WA_MacShowFocusRect);
    Q_D(QListView);               // We rely on a qobject_cast for PM_DefaultFrameWidth to change
    d->updateStyledFrameWidths(); // hence we have to force an update now that the object has been constructed
}

/*!
  Destroys the view.
*/
QListView::~QListView()
{
}

/*!
    \property QListView::movement
    \brief whether the items can be moved freely, are snapped to a
    grid, or cannot be moved at all.

    This property determines how the user can move the items in the
    view. \l Static means that the items can't be moved the user. \l
    Free means that the user can drag and drop the items to any
    position in the view. \l Snap means that the user can drag and
    drop the items, but only to the positions in a notional grid
    signified by the gridSize property.

    Setting this property when the view is visible will cause the
    items to be laid out again.

    By default, this property is set to \l Static.

    \sa gridSize, resizeMode, viewMode
*/
void QListView::setMovement(Movement movement)
{
    Q_D(QListView);
    d->modeProperties |= uint(QListViewPrivate::Movement);
    d->movement = movement;

#ifndef QT_NO_DRAGANDDROP
    bool movable = (movement != Static);
    setDragEnabled(movable);
    d->viewport->setAcceptDrops(movable);
#endif
    d->doDelayedItemsLayout();
}

QListView::Movement QListView::movement() const
{
    Q_D(const QListView);
    return d->movement;
}

/*!
    \property QListView::flow
    \brief which direction the items layout should flow.

    If this property is \l LeftToRight, the items will be laid out left
    to right. If the \l isWrapping property is true, the layout will wrap
    when it reaches the right side of the visible area. If this
    property is \l TopToBottom, the items will be laid out from the top
    of the visible area, wrapping when it reaches the bottom.

    Setting this property when the view is visible will cause the
    items to be laid out again.

    By default, this property is set to \l TopToBottom.

    \sa viewMode
*/
void QListView::setFlow(Flow flow)
{
    Q_D(QListView);
    d->modeProperties |= uint(QListViewPrivate::Flow);
    d->flow = flow;
    d->doDelayedItemsLayout();
}

QListView::Flow QListView::flow() const
{
    Q_D(const QListView);
    return d->flow;
}

/*!
    \property QListView::isWrapping
    \brief whether the items layout should wrap.

    This property holds whether the layout should wrap when there is
    no more space in the visible area. The point at which the layout wraps
    depends on the \l flow property.

    Setting this property when the view is visible will cause the
    items to be laid out again.

    By default, this property is false.

    \sa viewMode
*/
void QListView::setWrapping(bool enable)
{
    Q_D(QListView);
    d->modeProperties |= uint(QListViewPrivate::Wrap);
    d->setWrapping(enable);
    d->doDelayedItemsLayout();
}

bool QListView::isWrapping() const
{
    Q_D(const QListView);
    return d->isWrapping();
}

/*!
    \property QListView::resizeMode
    \brief whether the items are laid out again when the view is resized.

    If this property is \l Adjust, the items will be laid out again
    when the view is resized. If the value is \l Fixed, the items will
    not be laid out when the view is resized.

    By default, this property is set to \l Fixed.

    \sa movement, gridSize, viewMode
*/
void QListView::setResizeMode(ResizeMode mode)
{
    Q_D(QListView);
    d->modeProperties |= uint(QListViewPrivate::ResizeMode);
    d->resizeMode = mode;
}

QListView::ResizeMode QListView::resizeMode() const
{
    Q_D(const QListView);
    return d->resizeMode;
}

/*!
    \property QListView::layoutMode
    \brief determines whether the layout of items should happen immediately or be delayed.

    This property holds the layout mode for the items. When the mode
    is \l SinglePass (the default), the items are laid out all in one go.
    When the mode is \l Batched, the items are laid out in batches of \l batchSize
    items, while processing events. This makes it possible to
    instantly view and interact with the visible items while the rest
    are being laid out.

    \sa viewMode
*/
void QListView::setLayoutMode(LayoutMode mode)
{
    Q_D(QListView);
    d->layoutMode = mode;
}

QListView::LayoutMode QListView::layoutMode() const
{
    Q_D(const QListView);
    return d->layoutMode;
}

/*!
    \property QListView::spacing
    \brief the space between items in the layout

    This property is the size of the empty space that is padded around
    an item in the layout.

    Setting this property when the view is visible will cause the
    items to be laid out again.

    By default, this property contains a value of 0.

    \sa viewMode
*/
// ### Qt5: Use same semantic as layouts (spacing is the size of space
// *between* items)
void QListView::setSpacing(int space)
{
    Q_D(QListView);
    d->modeProperties |= uint(QListViewPrivate::Spacing);
    d->setSpacing(space);
    d->doDelayedItemsLayout();
}

int QListView::spacing() const
{
    Q_D(const QListView);
    return d->spacing();
}

/*!
    \property QListView::batchSize
    \brief the number of items laid out in each batch if \l layoutMode is
    set to \l Batched

    The default value is 100.

    \since 4.2
*/

void QListView::setBatchSize(int batchSize)
{
    Q_D(QListView);
    if (batchSize <= 0) {
        qWarning("Invalid batchSize (%d)", batchSize);
        return;
    }
    d->batchSize = batchSize;
}

int QListView::batchSize() const
{
    Q_D(const QListView);
    return d->batchSize;
}

/*!
    \property QListView::gridSize
    \brief the size of the layout grid

    This property is the size of the grid in which the items are laid
    out. The default is an empty size which means that there is no
    grid and the layout is not done in a grid. Setting this property
    to a non-empty size switches on the grid layout. (When a grid
    layout is in force the \l spacing property is ignored.)

    Setting this property when the view is visible will cause the
    items to be laid out again.

    \sa viewMode
*/
void QListView::setGridSize(const QSize &size)
{
    Q_D(QListView);
    d->modeProperties |= uint(QListViewPrivate::GridSize);
    d->setGridSize(size);
    d->doDelayedItemsLayout();
}

QSize QListView::gridSize() const
{
    Q_D(const QListView);
    return d->gridSize();
}

/*!
    \property QListView::viewMode
    \brief the view mode of the QListView.

    This property will change the other unset properties to conform
    with the set view mode. QListView-specific properties that have already been set
    will not be changed, unless clearPropertyFlags() has been called.

    Setting the view mode will enable or disable drag and drop based on the
    selected movement. For ListMode, the default movement is \l Static
    (drag and drop disabled); for IconMode, the default movement is
    \l Free (drag and drop enabled).

    \sa isWrapping, spacing, gridSize, flow, movement, resizeMode
*/
void QListView::setViewMode(ViewMode mode)
{
    Q_D(QListView);
    d->viewMode = mode;

    if (mode == ListMode) {
        delete d->dynamicListView;
        d->dynamicListView = 0;
        if (!d->staticListView)
            d->staticListView = new QStaticListViewBase(this, d);
        if (!(d->modeProperties & QListViewPrivate::Wrap))
            d->setWrapping(false);
        if (!(d->modeProperties & QListViewPrivate::Spacing))
            d->setSpacing(0);
        if (!(d->modeProperties & QListViewPrivate::GridSize))
            d->setGridSize(QSize());
        if (!(d->modeProperties & QListViewPrivate::Flow))
            d->flow = TopToBottom;
        if (!(d->modeProperties & QListViewPrivate::Movement))
            d->movement = Static;
        if (!(d->modeProperties & QListViewPrivate::ResizeMode))
            d->resizeMode = Fixed;
        if (!(d->modeProperties & QListViewPrivate::SelectionRectVisible))
            d->showElasticBand = false;
    } else {
        delete d->staticListView;
        d->staticListView = 0;
        if (!d->dynamicListView)
            d->dynamicListView = new QDynamicListViewBase(this, d);
        if (!(d->modeProperties & QListViewPrivate::Wrap))
            d->setWrapping(true);
        if (!(d->modeProperties & QListViewPrivate::Spacing))
            d->setSpacing(0);
        if (!(d->modeProperties & QListViewPrivate::GridSize))
            d->setGridSize(QSize());
        if (!(d->modeProperties & QListViewPrivate::Flow))
            d->flow = LeftToRight;
        if (!(d->modeProperties & QListViewPrivate::Movement))
            d->movement = Free;
        if (!(d->modeProperties & QListViewPrivate::ResizeMode))
            d->resizeMode = Fixed;
        if (!(d->modeProperties & QListViewPrivate::SelectionRectVisible))
            d->showElasticBand = true;
    }

#ifndef QT_NO_DRAGANDDROP
    bool movable = (d->movement != Static);
    setDragEnabled(movable);
    setAcceptDrops(movable);
#endif
    d->clear();
    d->doDelayedItemsLayout();
}

QListView::ViewMode QListView::viewMode() const
{
    Q_D(const QListView);
    return d->viewMode;
}

/*!
    Clears the QListView-specific property flags. See \l{viewMode}.

    Properties inherited from QAbstractItemView are not covered by the
    property flags. Specifically, \l{QAbstractItemView::dragEnabled}
    {dragEnabled} and \l{QAbstractItemView::acceptDrops}
    {acceptsDrops} are computed by QListView when calling
    setMovement() or setViewMode().
*/
void QListView::clearPropertyFlags()
{
    Q_D(QListView);
    d->modeProperties = 0;
}

/*!
    Returns true if the \a row is hidden; otherwise returns false.
*/
bool QListView::isRowHidden(int row) const
{
    Q_D(const QListView);
    return d->isHidden(row);
}

/*!
    If \a hide is true, the given \a row will be hidden; otherwise
    the \a row will be shown.
*/
void QListView::setRowHidden(int row, bool hide)
{
    Q_D(QListView);
    const bool hidden = d->isHidden(row);
    if (d->viewMode == ListMode) {
        if (hide && !hidden)
            d->hiddenRows.append(d->model->index(row, 0));
        else if (!hide && hidden)
            d->hiddenRows.remove(d->hiddenRows.indexOf(d->model->index(row, 0)));
        d->doDelayedItemsLayout();
    } else {
        if (hide && !hidden) {
            d->dynamicListView->removeItem(row);
            d->hiddenRows.append(d->model->index(row, 0));
        } else if (!hide && hidden) {
            d->hiddenRows.remove(d->hiddenRows.indexOf(d->model->index(row, 0)));
            d->dynamicListView->insertItem(row);
        }
        if (d->resizeMode == Adjust)
            d->doDelayedItemsLayout();
        d->viewport->update();
    }
}

/*!
  \reimp
*/
QRect QListView::visualRect(const QModelIndex &index) const
{
    Q_D(const QListView);
    return d->mapToViewport(rectForIndex(index), d->viewMode == QListView::ListMode);
}

/*!
  \reimp
*/
void QListView::scrollTo(const QModelIndex &index, ScrollHint hint)
{
    Q_D(QListView);

    if (index.parent() != d->root || index.column() != d->column)
        return;

    const QRect rect = visualRect(index);
    if (hint == EnsureVisible && d->viewport->rect().contains(rect)) {
        d->setDirtyRegion(rect);
        return;
    }

    if (d->flow == QListView::TopToBottom || d->isWrapping()) // vertical
        verticalScrollBar()->setValue(d->verticalScrollToValue(index, rect, hint));

    if (d->flow == QListView::LeftToRight || d->isWrapping()) // horizontal
        horizontalScrollBar()->setValue(d->horizontalScrollToValue(index, rect, hint));
}

int QListViewPrivate::horizontalScrollToValue(const QModelIndex &index, const QRect &rect,
                                              QListView::ScrollHint hint) const
{
    Q_Q(const QListView);
    const QRect area = viewport->rect();
    const bool leftOf = q->isRightToLeft()
                        ? (rect.left() < area.left()) && (rect.right() < area.right())
                        : rect.left() < area.left();
    const bool rightOf = q->isRightToLeft()
                         ? rect.right() > area.right()
                         : (rect.right() > area.right()) && (rect.left() > area.left());
    int horizontalValue = q->horizontalScrollBar()->value();

    // ScrollPerItem
    if (q->horizontalScrollMode() == QAbstractItemView::ScrollPerItem && viewMode == QListView::ListMode) {
        const QListViewItem item = indexToListViewItem(index);
        const QRect rect = q->visualRect(index);
        horizontalValue = staticListView->horizontalPerItemValue(itemIndex(item),
                                                                horizontalValue, area.width(),
                                                                leftOf, rightOf, isWrapping(), hint, rect.width());
    } else { // ScrollPerPixel
        if (q->isRightToLeft()) {
            if (hint == QListView::PositionAtCenter) {
                horizontalValue += ((area.width() - rect.width()) / 2) - rect.left();
            } else {
                if (leftOf)
                    horizontalValue -= rect.left();
                else if (rightOf)
                    horizontalValue += qMin(rect.left(), area.width() - rect.right());
            }
       } else {
            if (hint == QListView::PositionAtCenter) {
                horizontalValue += rect.left() - ((area.width()- rect.width()) / 2);
            } else {
                if (leftOf)
                    horizontalValue += rect.left();
                else if (rightOf)
                    horizontalValue += qMin(rect.left(), rect.right() - area.width());
            }
        }
    }
    return horizontalValue;
}

int QListViewPrivate::verticalScrollToValue(const QModelIndex &index, const QRect &rect,
                                            QListView::ScrollHint hint) const
{
    Q_Q(const QListView);

    const QRect area = viewport->rect();
    const bool above = (hint == QListView::EnsureVisible && rect.top() < area.top());
    const bool below = (hint == QListView::EnsureVisible && rect.bottom() > area.bottom());

    int verticalValue = q->verticalScrollBar()->value();

    // ScrollPerItem
    if (q->verticalScrollMode() == QAbstractItemView::ScrollPerItem && viewMode == QListView::ListMode) {
        const QListViewItem item = indexToListViewItem(index);
        const QRect rect = q->visualRect(index);
        verticalValue = staticListView->verticalPerItemValue(itemIndex(item),
                                                             verticalValue, area.height(),
                                                             above, below, isWrapping(), hint, rect.height());

    } else { // ScrollPerPixel
        QRect adjusted = rect.adjusted(-spacing(), -spacing(), spacing(), spacing());
        if (hint == QListView::PositionAtTop || above)
            verticalValue += adjusted.top();
        else if (hint == QListView::PositionAtBottom || below)
            verticalValue += qMin(adjusted.top(), adjusted.bottom() - area.height() + 1);
        else if (hint == QListView::PositionAtCenter)
            verticalValue += adjusted.top() - ((area.height() - adjusted.height()) / 2);
    }

    return verticalValue;
}

void QListViewPrivate::selectAll(QItemSelectionModel::SelectionFlags command)
{
    if (!selectionModel)
        return;

    QItemSelection selection;
    QModelIndex topLeft;
    int row = 0;
    const int colCount = model->columnCount(root);
    for(; row < model->rowCount(root); ++row) {
        if (isHidden(row)) {
            //it might be the end of a selection range
            if (topLeft.isValid()) {
                QModelIndex bottomRight = model->index(row - 1, colCount - 1, root);
                selection.append(QItemSelectionRange(topLeft, bottomRight));
                topLeft = QModelIndex();
            }
            continue;
        }

        if (!topLeft.isValid()) //start of a new selection range
            topLeft = model->index(row, 0, root);
    }

    if (topLeft.isValid()) {
        //last selected range
        QModelIndex bottomRight = model->index(row - 1, colCount - 1, root);
        selection.append(QItemSelectionRange(topLeft, bottomRight));
    }

    if (!selection.isEmpty())
        selectionModel->select(selection, command);
}


/*!
  \internal
*/
void QListView::reset()
{
    Q_D(QListView);
    d->clear();
    d->hiddenRows.clear();
    QAbstractItemView::reset();
}

/*!
  \internal
*/
void QListView::setRootIndex(const QModelIndex &index)
{
    Q_D(QListView);
    d->column = qBound(0, d->column, d->model->columnCount(index) - 1);
    QAbstractItemView::setRootIndex(index);
    // sometimes we get an update before reset() is called
    d->clear();
    d->hiddenRows.clear();
}

/*!
    \internal

    Scroll the view contents by \a dx and \a dy.
*/
void QListView::scrollContentsBy(int dx, int dy)
{
    Q_D(QListView);

    d->delayedAutoScroll.stop(); // auto scroll was canceled by the user scrolling

    if (d->viewMode == ListMode)
        d->staticListView->scrollContentsBy(dx, dy);
    else if (state() == DragSelectingState)
        d->scrollElasticBandBy(isRightToLeft() ? -dx : dx, dy);

    d->scrollContentsBy(isRightToLeft() ? -dx : dx, dy);

    // update the dragged items
    if (d->viewMode == IconMode) // ### move to dynamic class
    if (!d->dynamicListView->draggedItems.isEmpty())
        d->setDirtyRegion(d->dynamicListView->draggedItemsRect().translated(dx, dy));
}

/*!
    \internal

    Resize the internal contents to \a width and \a height and set the
    scroll bar ranges accordingly.
*/
void QListView::resizeContents(int width, int height)
{
    Q_D(QListView);
    d->setContentsSize(width, height);
}

/*!
    \internal
*/
QSize QListView::contentsSize() const
{
    Q_D(const QListView);
    return d->contentsSize();
}

/*!
  \reimp
*/
void QListView::dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    Q_D(QListView);
    if (d->viewMode == IconMode)
        d->dynamicListView->dataChanged(topLeft, bottomRight);
    QAbstractItemView::dataChanged(topLeft, bottomRight);
}

/*!
  \reimp
*/
void QListView::rowsInserted(const QModelIndex &parent, int start, int end)
{
    Q_D(QListView);
    // ### be smarter about inserted items
    d->clear();
    d->doDelayedItemsLayout();
    QAbstractItemView::rowsInserted(parent, start, end);
}

/*!
  \reimp
*/
void QListView::rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end)
{
    Q_D(QListView);
    // if the parent is above d->root in the tree, nothing will happen
    QAbstractItemView::rowsAboutToBeRemoved(parent, start, end);
    if (parent == d->root) {
        for (int i = d->hiddenRows.count() - 1; i >= 0; --i) {
            int hiddenRow = d->hiddenRows.at(i).row();
            if (hiddenRow >= start && hiddenRow <= end) {
                d->hiddenRows.remove(i);
            }
        }
    }
    d->clear();
    d->doDelayedItemsLayout();
}

/*!
  \reimp
*/
void QListView::mouseMoveEvent(QMouseEvent *e)
{
    Q_D(QListView);
    QAbstractItemView::mouseMoveEvent(e);
    if (state() == DragSelectingState
        && d->showElasticBand
        && d->selectionMode != SingleSelection
        && d->selectionMode != NoSelection) {
        QRect rect(d->pressedPosition, e->pos() + QPoint(horizontalOffset(), verticalOffset()));
        rect = rect.normalized();
        d->setDirtyRegion(d->mapToViewport(rect.united(d->elasticBand), d->viewMode == QListView::ListMode));
        d->elasticBand = rect;
    }
}

/*!
  \reimp
*/
void QListView::mouseReleaseEvent(QMouseEvent *e)
{
    Q_D(QListView);
    QAbstractItemView::mouseReleaseEvent(e);
    // #### move this implementation into a dynamic class
    if (d->showElasticBand && d->elasticBand.isValid()) {
        d->setDirtyRegion(d->mapToViewport(d->elasticBand, d->viewMode == QListView::ListMode));
        d->elasticBand = QRect();
    }
}

/*!
  \reimp
*/
void QListView::timerEvent(QTimerEvent *e)
{
    Q_D(QListView);
    if (e->timerId() == d->batchLayoutTimer.timerId()) {
        if (d->doItemsLayout(d->batchSize)) { // layout is done
            d->batchLayoutTimer.stop();
            updateGeometries();
            d->viewport->update();
        }
    }
    QAbstractItemView::timerEvent(e);
}

/*!
  \reimp
*/
void QListView::resizeEvent(QResizeEvent *e)
{
    Q_D(QListView);
    if (d->delayedPendingLayout)
        return;

    QSize delta = e->size() - e->oldSize();

    if (delta.isNull())
      return;

    bool listWrap = (d->viewMode == ListMode) && d->wrapItemText;
    bool flowDimensionChanged = (d->flow == LeftToRight && delta.width() != 0) 
				|| (d->flow == TopToBottom && delta.height() != 0);

    // We post a delayed relayout in the following cases :
    // - we're wrapping
    // - the state is NoState, we're adjusting and the size has changed in the flowing direction
    if (listWrap 
        || (state() == NoState && d->resizeMode == Adjust && flowDimensionChanged)) {
	d->doDelayedItemsLayout(100); // wait 1/10 sec before starting the layout
    } else {	
        QAbstractItemView::resizeEvent(e);
    }
}

#ifndef QT_NO_DRAGANDDROP

/*!
  \reimp
*/
void QListView::dragMoveEvent(QDragMoveEvent *e)
{
    // ### move implementation to dynamic
    Q_D(QListView);
    if (e->source() == this && d->viewMode == IconMode) {
        // the ignore by default
        e->ignore();
        if (d->canDecode(e)) {
            // get old dragged items rect
            QRect itemsRect = d->dynamicListView->itemsRect(d->dynamicListView->draggedItems);
            d->setDirtyRegion(itemsRect.translated(d->dynamicListView->draggedItemsDelta()));
            // update position
            d->dynamicListView->draggedItemsPos = e->pos();
            // get new items rect
            d->setDirtyRegion(itemsRect.translated(d->dynamicListView->draggedItemsDelta()));
            // set the item under the cursor to current
            QModelIndex index;
            if (d->movement == Snap) {
                QRect rect(d->dynamicListView->snapToGrid(e->pos() + d->offset()), d->gridSize());
                d->intersectingSet(rect);
                index = d->intersectVector.count() > 0
                                    ? d->intersectVector.last() : QModelIndex();
            } else {
                index = indexAt(e->pos());
            }
            // check if we allow drops here
            if (e->source() == this && d->dynamicListView->draggedItems.contains(index))
                e->accept(); // allow changing item position
            else if (d->model->flags(index) & Qt::ItemIsDropEnabled)
                e->accept(); // allow dropping on dropenabled items
            else if (!index.isValid())
                e->accept(); // allow dropping in empty areas
        }
        // do autoscrolling
        if (d->shouldAutoScroll(e->pos()))
            startAutoScroll();
    } else { // not internal
        QAbstractItemView::dragMoveEvent(e);
    }
}

/*!
  \reimp
*/
void QListView::dragLeaveEvent(QDragLeaveEvent *e)
{
    // ### move implementation to dynamic
    Q_D(QListView);
    if (d->viewMode == IconMode) {
        d->viewport->update(d->dynamicListView->draggedItemsRect()); // erase the area
        d->dynamicListView->draggedItemsPos = QPoint(-1, -1); // don't draw the dragged items
    }
    QAbstractItemView::dragLeaveEvent(e);
}

/*!
  \reimp
*/
void QListView::dropEvent(QDropEvent *event)
{
    Q_D(QListView);
    if (event->source() == this && d->viewMode == IconMode)
        internalDrop(event); // ### move to dynamic
    else
        QAbstractItemView::dropEvent(event);
}

/*!
  \reimp
*/
void QListView::startDrag(Qt::DropActions supportedActions)
{
    Q_D(QListView);
    if (d->viewMode == IconMode) // ### move to dynamic
        internalDrag(supportedActions);
    else
        QAbstractItemView::startDrag(supportedActions);
}

/*!
    \internal

    Called whenever items from the view is dropped on the viewport.
    The \a event provides additional information.
*/
void QListView::internalDrop(QDropEvent *event)
{
    Q_D(QListView);
    if (d->viewMode == QListView::ListMode)
        return;

    // ### move to dynamic class
    QPoint offset(horizontalOffset(), verticalOffset());
    QPoint end = event->pos() + offset;
    QPoint start = d->pressedPosition;
    QPoint delta = (d->movement == Snap ?
                    d->dynamicListView->snapToGrid(end)
                    - d->dynamicListView->snapToGrid(start) : end - start);
    QSize contents = d->contentsSize();
    QList<QModelIndex> indexes = d->selectionModel->selectedIndexes();
    for (int i = 0; i < indexes.count(); ++i) {
        QModelIndex index = indexes.at(i);
        QRect rect = rectForIndex(index);
        d->setDirtyRegion(d->mapToViewport(rect, d->viewMode == QListView::ListMode));
        QPoint dest = rect.topLeft() + delta;
        if (isRightToLeft())
            dest.setX(d->flipX(dest.x()) - rect.width());
        d->dynamicListView->moveItem(index.row(), dest);
        d->setDirtyRegion(visualRect(index));
    }
    stopAutoScroll();
    d->dynamicListView->draggedItems.clear();
    emit indexesMoved(indexes);
    event->accept(); // we have handled the event
    // if the size has not grown, we need to check if it has shrinked
    if (d->dynamicListView
        && (d->contentsSize().width() <= contents.width()
            || d->contentsSize().height() <= contents.height())) {
        d->dynamicListView->updateContentsSize();
    }
    if (d->contentsSize() != contents)
        updateGeometries();
}

/*!
    \internal

    Called whenever the user starts dragging items and the items are movable,
    enabling internal dragging and dropping of items.
*/
void QListView::internalDrag(Qt::DropActions supportedActions)
{
    Q_D(QListView);
    if (d->viewMode == QListView::ListMode)
        return;

    // #### move to dynamic class

    // This function does the same thing as in QAbstractItemView::startDrag(),
    // plus adding viewitems to the draggedItems list.
    // We need these items to draw the drag items
    QModelIndexList indexes = d->selectionModel->selectedIndexes();
    if (indexes.count() > 0 ) {
        if (d->viewport->acceptDrops()) {
            QModelIndexList::ConstIterator it = indexes.constBegin();
            for (; it != indexes.constEnd(); ++it)
                if (d->model->flags(*it) & Qt::ItemIsDragEnabled
                    && (*it).column() == d->column)
                    d->dynamicListView->draggedItems.push_back(*it);
        }
        QDrag *drag = new QDrag(this);
        drag->setMimeData(d->model->mimeData(indexes));
        Qt::DropAction action = drag->exec(supportedActions, Qt::CopyAction);
        d->dynamicListView->draggedItems.clear();
        if (action == Qt::MoveAction)
            d->clearOrRemove();
    }
}

#endif // QT_NO_DRAGANDDROP

/*!
  \reimp
*/
QStyleOptionViewItem QListView::viewOptions() const
{
    Q_D(const QListView);
    QStyleOptionViewItem option = QAbstractItemView::viewOptions();
    if (!d->iconSize.isValid()) { // otherwise it was already set in abstractitemview
        int pm = (d->viewMode == ListMode
                  ? style()->pixelMetric(QStyle::PM_ListViewIconSize, 0, this)
                  : style()->pixelMetric(QStyle::PM_IconViewIconSize, 0, this));
        option.decorationSize = QSize(pm, pm);
    }
    if (d->viewMode == IconMode) {
        option.showDecorationSelected = false;
        option.decorationPosition = QStyleOptionViewItem::Top;
        option.displayAlignment = Qt::AlignCenter;
    } else {
        option.decorationPosition = QStyleOptionViewItem::Left;
    }
    return option;
}

/*!
  \reimp
*/
void QListView::paintEvent(QPaintEvent *e)
{
    Q_D(QListView);
    if (!d->itemDelegate)
        return;
    QStyleOptionViewItemV4 option = d->viewOptionsV4();
    QPainter painter(d->viewport);
    QRect area = e->rect();

    QVector<QModelIndex> toBeRendered;
//     QVector<QRect> rects = e->region().rects();
//     for (int i = 0; i < rects.size(); ++i) {
//         d->intersectingSet(rects.at(i).translated(horizontalOffset(), verticalOffset()));
//         toBeRendered += d->intersectVector;
//     }
    d->intersectingSet(e->rect().translated(horizontalOffset(), verticalOffset()), false);
    toBeRendered = d->intersectVector;

    const QModelIndex current = currentIndex();
    const QModelIndex hover = d->hover;
    const QAbstractItemModel *itemModel = d->model;
    const QItemSelectionModel *selections = d->selectionModel;
    const bool focus = (hasFocus() || d->viewport->hasFocus()) && current.isValid();
    const bool alternate = d->alternatingColors;
    const QStyle::State state = option.state;
    const QAbstractItemView::State viewState = this->state();
    const bool enabled = (state & QStyle::State_Enabled) != 0;

    bool alternateBase = false;
    int previousRow = -2; // trigger the alternateBase adjustment on first pass

    QVector<QModelIndex>::const_iterator end = toBeRendered.constEnd();
    for (QVector<QModelIndex>::const_iterator it = toBeRendered.constBegin(); it != end; ++it) {
        Q_ASSERT((*it).isValid());
        option.rect = visualRect(*it);
        option.state = state;
        if (selections && selections->isSelected(*it))
            option.state |= QStyle::State_Selected;
        if (enabled) {
            QPalette::ColorGroup cg;
            if ((itemModel->flags(*it) & Qt::ItemIsEnabled) == 0) {
                option.state &= ~QStyle::State_Enabled;
                cg = QPalette::Disabled;
            } else {
                cg = QPalette::Normal;
            }
            option.palette.setCurrentColorGroup(cg);
        }
        if (focus && current == *it) {
            option.state |= QStyle::State_HasFocus;
            if (viewState == EditingState)
                option.state |= QStyle::State_Editing;
        }
        if (*it == hover)
            option.state |= QStyle::State_MouseOver;
        else
            option.state &= ~QStyle::State_MouseOver;

        if (alternate) {
            int row = (*it).row();
            if (row != previousRow + 1) {
                // adjust alternateBase according to rows in the "gap"
                if (!d->hiddenRows.isEmpty()) {
                    for (int r = qMax(previousRow + 1, 0); r < row; ++r) {
                        if (!d->isHidden(r))
                            alternateBase = !alternateBase;
                    }
                } else {
                    alternateBase = (row & 1) != 0;
                }
            }
            if (alternateBase) {
                option.features |= QStyleOptionViewItemV2::Alternate;
            } else {
                option.features &= ~QStyleOptionViewItemV2::Alternate;
            }

            // draw background of the item (only alternate row). rest of the background
            // is provided by the delegate
            QStyle::State oldState = option.state;
            option.state &= ~QStyle::State_Selected;
            style()->drawPrimitive(QStyle::PE_PanelItemViewRow, &option, &painter, this);
            option.state = oldState;

            alternateBase = !alternateBase;
            previousRow = row;
        }

        if (const QWidget *widget = d->editorForIndex(*it).editor) {
            QRegion itemGeometry(option.rect);
            QRegion widgetGeometry(widget->geometry());
            painter.save();
            painter.setClipRegion(itemGeometry.subtracted(widgetGeometry));
            d->delegateForIndex(*it)->paint(&painter, option, *it);
            painter.restore();
        } else {
            d->delegateForIndex(*it)->paint(&painter, option, *it);
        }
    }

#ifndef QT_NO_DRAGANDDROP
    // #### move this implementation into a dynamic class
    if (d->viewMode == IconMode)
    if (!d->dynamicListView->draggedItems.isEmpty()
        && d->viewport->rect().contains(d->dynamicListView->draggedItemsPos)) {
        QPoint delta = d->dynamicListView->draggedItemsDelta();
        painter.translate(delta.x(), delta.y());
        d->dynamicListView->drawItems(&painter, d->dynamicListView->draggedItems);
    }
    // FIXME: Until the we can provide a proper drop indicator
    // in IconMode, it makes no sense to show it
    if (d->viewMode == ListMode)
        d->paintDropIndicator(&painter);
#endif

#ifndef QT_NO_RUBBERBAND
    // #### move this implementation into a dynamic class
    if (d->showElasticBand && d->elasticBand.isValid()) {
        QStyleOptionRubberBand opt;
        opt.initFrom(this);
        opt.shape = QRubberBand::Rectangle;
        opt.opaque = false;
        opt.rect = d->mapToViewport(d->elasticBand, false).intersected(
            d->viewport->rect().adjusted(-16, -16, 16, 16));
        painter.save();
        style()->drawControl(QStyle::CE_RubberBand, &opt, &painter);
        painter.restore();
    }
#endif
}

/*!
  \reimp
*/
QModelIndex QListView::indexAt(const QPoint &p) const
{
    Q_D(const QListView);
    QRect rect(p.x() + horizontalOffset(), p.y() + verticalOffset(), 1, 1);
    d->intersectingSet(rect);
    QModelIndex index = d->intersectVector.count() > 0
                        ? d->intersectVector.last() : QModelIndex();
    if (index.isValid() && visualRect(index).contains(p))
        return index;
    return QModelIndex();
}

/*!
  \reimp
*/
int QListView::horizontalOffset() const
{
    Q_D(const QListView);
    // ### split into static and dynamic
    if (horizontalScrollMode() == QAbstractItemView::ScrollPerItem && d->viewMode == ListMode) {
        if (d->isWrapping()) {
            if (d->flow == TopToBottom && !d->staticListView->segmentPositions.isEmpty()) {
                const int max = d->staticListView->segmentPositions.count() - 1;
                int currentValue = qBound(0, horizontalScrollBar()->value(), max);
                int position = d->staticListView->segmentPositions.at(currentValue);
                int maximumValue = qBound(0, horizontalScrollBar()->maximum(), max);
                int maximum = d->staticListView->segmentPositions.at(maximumValue);
                return (isRightToLeft() ? maximum - position : position);
            }
            //return 0;
        } else {
            if (d->flow == LeftToRight && !d->staticListView->flowPositions.isEmpty()) {
                int position = d->staticListView->flowPositions.at(horizontalScrollBar()->value());
                int maximum = d->staticListView->flowPositions.at(horizontalScrollBar()->maximum());
                return (isRightToLeft() ? maximum - position : position);
            }
            //return 0;
        }
    }
    return (isRightToLeft()
            ? horizontalScrollBar()->maximum() - horizontalScrollBar()->value()
            : horizontalScrollBar()->value());
}

/*!
  \reimp
*/
int QListView::verticalOffset() const
{
    // ## split into static and dynamic
    Q_D(const QListView);
    if (verticalScrollMode() == QAbstractItemView::ScrollPerItem && d->viewMode == ListMode) {
        if (d->isWrapping()) {
            if (d->flow == LeftToRight && !d->staticListView->segmentPositions.isEmpty()) {
                int value = verticalScrollBar()->value();
                if (value >= d->staticListView->segmentPositions.count()) {
                    //qWarning("QListView: Vertical scroll bar is out of bounds");
                    return 0;
                }
                return d->staticListView->segmentPositions.at(value);
            }
        } else {
            if (d->flow == TopToBottom && !d->staticListView->flowPositions.isEmpty()) {
                int value = verticalScrollBar()->value();
                if (value > d->staticListView->flowPositions.count()) {
                    //qWarning("QListView: Vertical scroll bar is out of bounds");
                    return 0;
                }
                return d->staticListView->flowPositions.at(value) - d->spacing();
            }
        }
    }
    return verticalScrollBar()->value();
}

/*!
  \reimp
*/
QModelIndex QListView::moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers)
{
    Q_D(QListView);
    Q_UNUSED(modifiers);

    QModelIndex current = currentIndex();
    if (!current.isValid()) {
        int rowCount = d->model->rowCount(d->root);
        if (!rowCount)
            return QModelIndex();
        int row = 0;
        while (row < rowCount && d->isHiddenOrDisabled(row))
            ++row;
        if (row >= rowCount)
            return QModelIndex();
        return d->model->index(row, 0, d->root);
    }

    const QRect initialRect = rectForIndex(current);
    QRect rect = initialRect;
    if (rect.isEmpty()) {
        return d->model->index(0, 0, d->root);
    }
    if (d->gridSize().isValid()) rect.setSize(d->gridSize());

    QSize contents = d->contentsSize();
    d->intersectVector.clear();

    switch (cursorAction) {
    case MoveLeft:
        while (d->intersectVector.isEmpty()) {
            rect.translate(-rect.width(), 0);
            if (rect.right() <= 0)
                return current;
            if (rect.left() < 0)
                rect.setLeft(0);
            d->intersectingSet(rect);
            d->removeCurrentAndDisabled(&d->intersectVector, current);
        }
        return d->closestIndex(initialRect, d->intersectVector);
    case MoveRight:
        while (d->intersectVector.isEmpty()) {
            rect.translate(rect.width(), 0);
            if (rect.left() >= contents.width())
                return current;
            if (rect.right() > contents.width())
                rect.setRight(contents.width());
            d->intersectingSet(rect);
            d->removeCurrentAndDisabled(&d->intersectVector, current);
        }
        return d->closestIndex(initialRect, d->intersectVector);
    case MovePageUp:
        rect.moveTop(rect.top() - d->viewport->height());
        if (rect.top() < rect.height())
            rect.moveTop(rect.height());
    case MovePrevious:
    case MoveUp:
        while (d->intersectVector.isEmpty()) {
            rect.translate(0, -rect.height());
            if (rect.bottom() <= 0) {
#ifdef QT_KEYPAD_NAVIGATION
                if (QApplication::keypadNavigationEnabled()) {
                    int row = d->batchStartRow() - 1;
                    while (row >= 0 && d->isHiddenOrDisabled(row))
                        --row;
                    if (row >= 0)
                        return d->model->index(row, d->column, d->root);
                }
#endif
                return current;
            }
            if (rect.top() < 0)
                rect.setTop(0);
            d->intersectingSet(rect);
            d->removeCurrentAndDisabled(&d->intersectVector, current);
        }
        return d->closestIndex(initialRect, d->intersectVector);
    case MovePageDown:
        rect.moveTop(rect.top() + d->viewport->height());
        if (rect.bottom() > contents.height() - rect.height())
            rect.moveBottom(contents.height() - rect.height());
    case MoveNext:
    case MoveDown:
        while (d->intersectVector.isEmpty()) {
            rect.translate(0, rect.height());
            if (rect.top() >= contents.height()) {
#ifdef QT_KEYPAD_NAVIGATION
                if (QApplication::keypadNavigationEnabled()) {
                    int rowCount = d->model->rowCount(d->root);
                    int row = 0;
                    while (row < rowCount && d->isHiddenOrDisabled(row))
                        ++row;
                    if (row < rowCount)
                        return d->model->index(row, d->column, d->root);
                }
#endif
                return current;
            }
            if (rect.bottom() > contents.height())
                rect.setBottom(contents.height());
            d->intersectingSet(rect);
            d->removeCurrentAndDisabled(&d->intersectVector, current);
        }
        return d->closestIndex(initialRect, d->intersectVector);
    case MoveHome:
        return d->model->index(0, d->column, d->root);
    case MoveEnd:
        return d->model->index(d->batchStartRow() - 1, d->column, d->root);}

    return current;
}

/*!
    Returns the rectangle of the item at position \a index in the
    model. The rectangle is in contents coordinates.

    \sa visualRect()
*/
QRect QListView::rectForIndex(const QModelIndex &index) const
{
    Q_D(const QListView);
    if (!d->isIndexValid(index)
        || index.parent() != d->root
        || index.column() != d->column
        || isIndexHidden(index))
        return QRect();
    d->executePostedLayout();
    QListViewItem item = d->indexToListViewItem(index);
    return d->viewItemRect(item);
}

/*!
    \since 4.1

    Sets the contents position of the item at \a index in the model to the given
    \a position.
    If the list view's movement mode is Static, this function will have no
    effect.
*/
void QListView::setPositionForIndex(const QPoint &position, const QModelIndex &index)
{
    Q_D(QListView);
    if (d->movement == Static
        || !d->isIndexValid(index)
        || index.parent() != d->root
        || index.column() != d->column)
        return;

    d->executePostedLayout();
    if (index.row() >= d->dynamicListView->items.count())
        return;
    const QSize oldContents = d->contentsSize();
    d->setDirtyRegion(visualRect(index)); // update old position
    d->dynamicListView->moveItem(index.row(), position);
    d->setDirtyRegion(visualRect(index)); // update new position

    if (d->contentsSize() != oldContents)
        updateGeometries(); // update the scroll bars
}

/*!
  \reimp
*/
void QListView::setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags command)
{
    Q_D(QListView);
    if (!d->selectionModel)
        return;

    // if we are wrapping, we can only selecte inside the contents rectangle
    int w = qMax(d->contentsSize().width(), d->viewport->width());
    int h = qMax(d->contentsSize().height(), d->viewport->height());
    if (d->wrap && !QRect(0, 0, w, h).intersects(rect))
        return;

    QItemSelection selection;

    if (rect.width() == 1 && rect.height() == 1) {
        d->intersectingSet(rect.translated(horizontalOffset(), verticalOffset()));
        QModelIndex tl;
        if (!d->intersectVector.isEmpty())
            tl = d->intersectVector.last(); // special case for mouse press; only select the top item
        if (tl.isValid() && d->isIndexEnabled(tl))
            selection.select(tl, tl);
    } else {
        if (state() == DragSelectingState) { // visual selection mode (rubberband selection)
            selection = d->selection(rect.translated(horizontalOffset(), verticalOffset()));
        } else { // logical selection mode (key and mouse click selection)
            QModelIndex tl, br;
            // get the first item
            const QRect topLeft(rect.left() + horizontalOffset(), rect.top() + verticalOffset(), 1, 1);
            d->intersectingSet(topLeft);
            if (!d->intersectVector.isEmpty())
                tl = d->intersectVector.last();
            // get the last item
            const QRect bottomRight(rect.right() + horizontalOffset(), rect.bottom() + verticalOffset(), 1, 1);
            d->intersectingSet(bottomRight);
            if (!d->intersectVector.isEmpty())
                br = d->intersectVector.last();

            // get the ranges
            if (tl.isValid() && br.isValid()
                && d->isIndexEnabled(tl)
                && d->isIndexEnabled(br)) {
                QRect first = rectForIndex(tl);
                QRect last = rectForIndex(br);
                QRect middle;
                if (d->flow == LeftToRight) {
                    QRect &top = first;
                    QRect &bottom = last;
                    // if bottom is above top, swap them
                    if (top.center().y() > bottom.center().y()) {
                        QRect tmp = top;
                        top = bottom;
                        bottom = tmp;
                    }
                    // if the rect are on differnet lines, expand
                    if (top.top() != bottom.top()) {
                        // top rectangle
                        if (isRightToLeft())
                            top.setLeft(0);
                        else
                            top.setRight(contentsSize().width());
                        // bottom rectangle
                        if (isRightToLeft())
                            bottom.setRight(contentsSize().width());
                        else
                            bottom.setLeft(0);
                    } else if (top.left() > bottom.right()) {
                        if (isRightToLeft())
                            bottom.setLeft(top.right());
                        else
                            bottom.setRight(top.left());
                    } else {
                        if (isRightToLeft())
                            top.setLeft(bottom.right());
                        else
                            top.setRight(bottom.left());
                    }
                    // middle rectangle
                    if (top.bottom() < bottom.top()) {
                        middle.setTop(top.bottom() + 1);
                        middle.setLeft(qMin(top.left(), bottom.left()));
                        middle.setBottom(bottom.top() - 1);
                        middle.setRight(qMax(top.right(), bottom.right()));
                    }
                } else {    // TopToBottom
                    QRect &left = first;
                    QRect &right = last;
                    if (left.center().x() > right.center().x())
                        qSwap(left, right);

                    int ch = contentsSize().height();
                    if (left.left() != right.left()) {
                        // left rectangle
                        if (isRightToLeft())
                            left.setTop(0);
                        else
                            left.setBottom(ch);

                        // top rectangle
                        if (isRightToLeft())
                            right.setBottom(ch);
                        else
                            right.setTop(0);
                        // only set middle if the
                        middle.setTop(0);
                        middle.setBottom(ch);
                        middle.setLeft(left.right() + 1);
                        middle.setRight(right.left() - 1);
                    } else if (left.bottom() < right.top()) {
                        left.setBottom(right.top() - 1);
                    } else {
                        right.setBottom(left.top() - 1);
                    }
                }

                // do the selections
                QItemSelection topSelection = d->selection(first);
                QItemSelection middleSelection = d->selection(middle);
                QItemSelection bottomSelection = d->selection(last);
                // merge
                selection.merge(topSelection, QItemSelectionModel::Select);
                selection.merge(middleSelection, QItemSelectionModel::Select);
                selection.merge(bottomSelection, QItemSelectionModel::Select);
            }
        }
    }

    d->selectionModel->select(selection, command);
}

/*!
  \reimp
*/
QRegion QListView::visualRegionForSelection(const QItemSelection &selection) const
{
    Q_D(const QListView);
    // ### NOTE: this is a potential bottleneck in non-static mode
    int c = d->column;
    QRegion selectionRegion;
    for (int i = 0; i < selection.count(); ++i) {
        if (!selection.at(i).isValid())
            continue;
        QModelIndex parent = selection.at(i).topLeft().parent();
        int t = selection.at(i).topLeft().row();
        int b = selection.at(i).bottomRight().row();
        if (d->viewMode == IconMode || d->isWrapping()) { // in non-static mode, we have to go through all selected items
            for (int r = t; r <= b; ++r)
                selectionRegion += QRegion(visualRect(d->model->index(r, c, parent)));
        } else { // in static mode, we can optimize a bit
            while (t <= b && d->isHidden(t)) ++t;
            while (b >= t && d->isHidden(b)) --b;
            const QModelIndex top = d->model->index(t, c, d->root);
            const QModelIndex bottom = d->model->index(b, c, d->root);
            QRect rect(visualRect(top).topLeft(),
                       visualRect(bottom).bottomRight());
            selectionRegion += QRegion(rect);
        }
    }

    return selectionRegion;
}

/*!
  \reimp
*/
QModelIndexList QListView::selectedIndexes() const
{
    Q_D(const QListView);
    QModelIndexList viewSelected;
    QModelIndexList modelSelected;
    if (d->selectionModel)
        modelSelected = d->selectionModel->selectedIndexes();
    for (int i = 0; i < modelSelected.count(); ++i) {
        QModelIndex index = modelSelected.at(i);
        if (!isIndexHidden(index) && index.parent() == d->root && index.column() == d->column)
            viewSelected.append(index);
    }
    return viewSelected;
}

/*!
    \internal

    Layout the items according to the flow and wrapping properties.
*/
void QListView::doItemsLayout()
{
    Q_D(QListView);
    // showing the scroll bars will trigger a resize event,
    // so we set the state to expanding to avoid
    // triggering another layout
    QAbstractItemView::State oldState = state();
    setState(ExpandingState); 
    if (d->model->columnCount(d->root) > 0) { // no columns means no contents
        d->resetBatchStartRow();
        if (layoutMode() == SinglePass)
            d->doItemsLayout(d->model->rowCount(d->root)); // layout everything
        else if (!d->batchLayoutTimer.isActive()) {
            if (!d->doItemsLayout(d->batchSize)) // layout is done
                d->batchLayoutTimer.start(0, this); // do a new batch as fast as possible
        }
    }
    QAbstractItemView::doItemsLayout();
    setState(oldState);        // restoring the oldState
}

/*!
  \reimp
*/
void QListView::updateGeometries()
{
    Q_D(QListView);
    if (d->model->rowCount(d->root) <= 0 || d->model->columnCount(d->root) <= 0) {
        horizontalScrollBar()->setRange(0, 0);
        verticalScrollBar()->setRange(0, 0);
    } else {
        QModelIndex index = d->model->index(0, d->column, d->root);
        QStyleOptionViewItemV4 option = d->viewOptionsV4();
        QSize step = d->itemSize(option, index);

        QSize csize = d->contentsSize();
        QSize vsize = d->viewport->size();
        QSize max = maximumViewportSize();
        if (max.width() >= d->contentsSize().width() && max.height() >= d->contentsSize().height())
            vsize = max;

        // ### reorder the logic

        // ### split into static and dynamic

        const bool vertical = verticalScrollMode() == QAbstractItemView::ScrollPerItem;
        const bool horizontal = horizontalScrollMode() == QAbstractItemView::ScrollPerItem;

        if (d->flow == TopToBottom) {
            if (horizontal && d->isWrapping() && d->viewMode == ListMode) {
                const QVector<int> segmentPositions = d->staticListView->segmentPositions;
                const int steps = segmentPositions.count() - 1;
                if (steps > 0) {
                    int pageSteps = d->staticListView->perItemScrollingPageSteps(vsize.width(),
                                                                                 csize.width(),
                                                                                 isWrapping());
                    horizontalScrollBar()->setSingleStep(1);
                    horizontalScrollBar()->setPageStep(pageSteps);
                    horizontalScrollBar()->setRange(0, steps - pageSteps);
                } else {
                    horizontalScrollBar()->setRange(0, 0);
                }
            } else {
                horizontalScrollBar()->setSingleStep(step.width() + d->spacing());
                horizontalScrollBar()->setPageStep(vsize.width());
                horizontalScrollBar()->setRange(0, d->contentsSize().width() - vsize.width());
            }
            if (vertical && !d->isWrapping() && d->viewMode == ListMode) {
                const QVector<int> flowPositions = d->staticListView->flowPositions;
                const int steps = flowPositions.count() - 1;
                if (steps > 0) {
                    int pageSteps = d->staticListView->perItemScrollingPageSteps(vsize.height(),
                                                                                 csize.height(),
                                                                                 isWrapping());
                    verticalScrollBar()->setSingleStep(1);
                    verticalScrollBar()->setPageStep(pageSteps);
                    verticalScrollBar()->setRange(0, steps - pageSteps);
                } else {
                    verticalScrollBar()->setRange(0, 0);
                }
                // } else if (vertical && d->isWrapping() && d->movement == Static) {
                // ### wrapped scrolling in flow direction
            } else {
                verticalScrollBar()->setSingleStep(step.height() + d->spacing());
                verticalScrollBar()->setPageStep(vsize.height());
                verticalScrollBar()->setRange(0, d->contentsSize().height() - vsize.height());
            }
        } else { // LeftToRight
            if (horizontal && !d->isWrapping() && d->viewMode == ListMode) {
                const QVector<int> flowPositions = d->staticListView->flowPositions;
                int steps = flowPositions.count() - 1;
                if (steps > 0) {
                    int pageSteps = d->staticListView->perItemScrollingPageSteps(vsize.width(),
                                                                                 csize.width(),
                                                                                 isWrapping());
                    horizontalScrollBar()->setSingleStep(1);
                    horizontalScrollBar()->setPageStep(pageSteps);
                    horizontalScrollBar()->setRange(0, steps - pageSteps);
                } else {
                    horizontalScrollBar()->setRange(0, 0);
                }
                // } else if (horizontal && d->isWrapping() && d->movement == Static) {
                // ### wrapped scrolling in flow direction
            } else {
                horizontalScrollBar()->setSingleStep(step.width() + d->spacing());
                horizontalScrollBar()->setPageStep(vsize.width());
                horizontalScrollBar()->setRange(0, d->contentsSize().width() - vsize.width());
            }
            if (vertical && d->isWrapping() && d->viewMode == ListMode) {
                const QVector<int> segmentPositions = d->staticListView->segmentPositions;
                int steps = segmentPositions.count() - 1;
                if (steps > 0) {
                    int pageSteps = d->staticListView->perItemScrollingPageSteps(vsize.height(),
                                                                                 csize.height(),
                                                                                 isWrapping());
                    verticalScrollBar()->setSingleStep(1);
                    verticalScrollBar()->setPageStep(pageSteps);
                    verticalScrollBar()->setRange(0, steps - pageSteps);
                } else {
                    verticalScrollBar()->setRange(0, 0);
                }
            } else {
                verticalScrollBar()->setSingleStep(step.height() + d->spacing());
                verticalScrollBar()->setPageStep(vsize.height());
                verticalScrollBar()->setRange(0, d->contentsSize().height() - vsize.height());
            }
        }
    }

    QAbstractItemView::updateGeometries();

    // if the scroll bars are turned off, we resize the contents to the viewport
    if (d->movement == Static && !d->isWrapping()) {
        d->layoutChildren(); // we need the viewport size to be updated
        if (d->flow == TopToBottom) {
            if (horizontalScrollBarPolicy() == Qt::ScrollBarAlwaysOff) {
                d->setContentsSize(viewport()->width(), contentsSize().height());
                horizontalScrollBar()->setRange(0, 0); // we see all the contents anyway
            }
        } else { // LeftToRight
            if (verticalScrollBarPolicy() == Qt::ScrollBarAlwaysOff) {
                d->setContentsSize(contentsSize().width(), viewport()->height());
                verticalScrollBar()->setRange(0, 0); // we see all the contents anyway
            }
        }
    }
}

/*!
  \reimp
*/
bool QListView::isIndexHidden(const QModelIndex &index) const
{
    Q_D(const QListView);
    return (d->isHidden(index.row())
            && (index.parent() == d->root)
            && index.column() == d->column);
}

/*!
    \property QListView::modelColumn
    \brief the column in the model that is visible

    By default, this property contains 0, indicating that the first
    column in the model will be shown.
*/
void QListView::setModelColumn(int column)
{
    Q_D(QListView);
    if (column < 0 || column >= d->model->columnCount(d->root))
        return;
    d->column = column;
    d->doDelayedItemsLayout();
}

int QListView::modelColumn() const
{
    Q_D(const QListView);
    return d->column;
}

/*!
    \property QListView::uniformItemSizes
    \brief whether all items in the listview have the same size
    \since 4.1

    This property should only be set to true if it is guaranteed that all items
    in the view have the same size. This enables the view to do some
    optimizations for performance purposes.

    By default, this property is false.
*/
void QListView::setUniformItemSizes(bool enable)
{
    Q_D(QListView);
    d->uniformItemSizes = enable;
}

bool QListView::uniformItemSizes() const
{
    Q_D(const QListView);
    return d->uniformItemSizes;
}

/*!
    \property QListView::wordWrap
    \brief the item text word-wrapping policy
    \since 4.2

    If this property is true then the item text is wrapped where
    necessary at word-breaks; otherwise it is not wrapped at all.
    This property is false by default.

    Please note that even if wrapping is enabled, the cell will not be
    expanded to make room for the text. It will print ellipsis for
    text that cannot be shown, according to the view's
    \l{QAbstractItemView::}{textElideMode}.
*/
void QListView::setWordWrap(bool on)
{
    Q_D(QListView);
    if (d->wrapItemText == on)
        return;
    d->wrapItemText = on;
    d->doDelayedItemsLayout();
}

bool QListView::wordWrap() const
{
    Q_D(const QListView);
    return d->wrapItemText;
}

/*!
    \property QListView::selectionRectVisible
    \brief if the selection rectangle should be visible
    \since 4.3

    If this property is true then the selection rectangle is visible;
    otherwise it will be hidden.

    \note The selection rectangle will only be visible if the selection mode
    is in a mode where more than one item can be selected; i.e., it will not
    draw a selection rectangle if the selection mode is
    QAbstractItemView::SingleSelection.

    By default, this property is false.
*/
void QListView::setSelectionRectVisible(bool show)
{
    Q_D(QListView);
    d->modeProperties |= uint(QListViewPrivate::SelectionRectVisible);
    d->setSelectionRectVisible(show);
}

bool QListView::isSelectionRectVisible() const
{
    Q_D(const QListView);
    return d->isSelectionRectVisible();
}

/*!
    \reimp
*/
bool QListView::event(QEvent *e)
{
    return QAbstractItemView::event(e);
}

/*
 * private object implementation
 */

QListViewPrivate::QListViewPrivate()
    : QAbstractItemViewPrivate(),
      dynamicListView(0),
      staticListView(0),
      wrap(false),
      space(0),
      flow(QListView::TopToBottom),
      movement(QListView::Static),
      resizeMode(QListView::Fixed),
      layoutMode(QListView::SinglePass),
      viewMode(QListView::ListMode),
      modeProperties(0),
      column(0),
      uniformItemSizes(false),
      batchSize(100)
{
}

QListViewPrivate::~QListViewPrivate()
{
    delete staticListView;
    delete dynamicListView;
}

void QListViewPrivate::clear()
{
    // ### split into dynamic and static
    // initialization of data structs
    cachedItemSize = QSize();
    if (viewMode == QListView::ListMode)
        staticListView->clear();
    else
        dynamicListView->clear();
}

void QListViewPrivate::prepareItemsLayout()
{
    Q_Q(QListView);
    clear();

    //take the size as if there were scrollbar in order to prevent scrollbar to blink
    layoutBounds = QRect(QPoint(0,0), q->maximumViewportSize());

    int frameAroundContents = 0;
    if (q->style()->styleHint(QStyle::SH_ScrollView_FrameOnlyAroundContents))
        frameAroundContents = q->style()->pixelMetric(QStyle::PM_DefaultFrameWidth) * 2;
    int verticalMargin = vbarpolicy==Qt::ScrollBarAlwaysOff ? 0 :
        q->style()->pixelMetric(QStyle::PM_ScrollBarExtent, 0, q->verticalScrollBar()) + frameAroundContents;
    int horizontalMargin =  hbarpolicy==Qt::ScrollBarAlwaysOff ? 0 :
        q->style()->pixelMetric(QStyle::PM_ScrollBarExtent, 0, q->horizontalScrollBar()) + frameAroundContents;

    layoutBounds.adjust(0, 0, -verticalMargin, -horizontalMargin);

    int rowCount = model->rowCount(root);
    int colCount = model->columnCount(root);
    if (colCount <= 0)
        rowCount = 0; // no contents
    if (viewMode == QListView::ListMode) {
        staticListView->flowPositions.resize(rowCount);
    } else {
        dynamicListView->tree.create(qMax(rowCount - hiddenRows.count(), 0));
    }
}

/*!
  \internal
*/
bool QListViewPrivate::doItemsLayout(int delta)
{
    // ### split into static and dynamic
    int max = model->rowCount(root) - 1;
    int first = batchStartRow();
    int last = qMin(first + delta - 1, max);

    if (max < 0 || last < first)
        return true; // nothing to do

    if (first == 0) {
        layoutChildren(); // make sure the viewport has the right size
        prepareItemsLayout();
    }

    QListViewLayoutInfo info;
    info.bounds = layoutBounds;
    info.grid = gridSize();
    info.spacing = (info.grid.isValid() ? 0 : spacing());
    info.first = first;
    info.last = last;
    info.wrap = isWrapping();
    info.flow = flow;
    info.max = max;

    if (viewMode == QListView::ListMode)
        return staticListView->doBatchedItemLayout(info, max);
    return dynamicListView->doBatchedItemLayout(info, max);
}

QListViewItem QListViewPrivate::indexToListViewItem(const QModelIndex &index) const
{
    if (!index.isValid() || isHidden(index.row()))
        return QListViewItem();

    if (viewMode == QListView::ListMode)
        return staticListView->indexToListViewItem(index);
    return dynamicListView->indexToListViewItem(index);
}


int QListViewPrivate::itemIndex(const QListViewItem &item) const
{
    if (viewMode == QListView::ListMode)
        return staticListView->itemIndex(item);
    return dynamicListView->itemIndex(item);
}

QRect QListViewPrivate::mapToViewport(const QRect &rect, bool greedy) const
{
    Q_Q(const QListView);
    if (!rect.isValid())
        return rect;

    QRect result = rect;
    if (greedy)
        result = staticListView->mapToViewport(rect);

    int dx = -q->horizontalOffset();
    int dy = -q->verticalOffset();
    result.adjust(dx, dy, dx, dy);
    return result;
}

QModelIndex QListViewPrivate::closestIndex(const QRect &target,
                                           const QVector<QModelIndex> &candidates) const
{
    int distance = 0;
    int shortest = INT_MAX;
    QModelIndex closest;
    QVector<QModelIndex>::const_iterator it = candidates.begin();

    for (; it != candidates.end(); ++it) {
        if (!(*it).isValid())
            continue;

        const QRect indexRect = indexToListViewItem(*it).rect();

        //if the center x (or y) position of an item is included in the rect of the other item,
        //we define the distance between them as the difference in x (or y) of their respective center.
        // Otherwise, we use the nahattan  length between the 2 items
        if ((target.center().x() >= indexRect.x() && target.center().x() < indexRect.right())
            || (indexRect.center().x() >= target.x() && indexRect.center().x() < target.right())) {
                //one item's center is at the vertical of the other
                distance = qAbs(indexRect.center().y() - target.center().y());
        } else if ((target.center().y() >= indexRect.y() && target.center().y() < indexRect.bottom())
            || (indexRect.center().y() >= target.y() && indexRect.center().y() < target.bottom())) {
                //one item's center is at the vertical of the other
                distance = qAbs(indexRect.center().x() - target.center().x());
        } else {
            distance = (indexRect.center() - target.center()).manhattanLength();
        }
        if (distance < shortest) {
            shortest = distance;
            closest = *it;
        }
    }
    return closest;
}

QSize QListViewPrivate::itemSize(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (!uniformItemSizes) {
        const QAbstractItemDelegate *delegate = delegateForIndex(index);
        return delegate ? delegate->sizeHint(option, index) : QSize();
    }
    if (!cachedItemSize.isValid()) { // the last item is probaly the largest, so we use its size
        int row = model->rowCount(root) - 1;
        QModelIndex sample = model->index(row, column, root);
        const QAbstractItemDelegate *delegate = delegateForIndex(sample);
        cachedItemSize = delegate ? delegate->sizeHint(option, sample) : QSize();
    }
    return cachedItemSize;
}

QItemSelection QListViewPrivate::selection(const QRect &rect) const
{
    QItemSelection selection;
    QModelIndex tl, br;
    intersectingSet(rect);
    QVector<QModelIndex>::iterator it = intersectVector.begin();
    for (; it != intersectVector.end(); ++it) {
        if (!tl.isValid() && !br.isValid()) {
            tl = br = *it;
        } else if ((*it).row() == (tl.row() - 1)) {
            tl = *it; // expand current range
        } else if ((*it).row() == (br.row() + 1)) {
            br = (*it); // expand current range
        } else {
            selection.select(tl, br); // select current range
            tl = br = *it; // start new range
        }
    }

    if (tl.isValid() && br.isValid())
        selection.select(tl, br);
    else if (tl.isValid())
        selection.select(tl, tl);
    else if (br.isValid())
        selection.select(br, br);

    return selection;
}

/*
 * Static ListView Implementation
*/

int QStaticListViewBase::verticalPerItemValue(int itemIndex, int verticalValue, int areaHeight,
                                                 bool above, bool below, bool wrap,
                                                 QListView::ScrollHint hint, int itemHeight) const
{
    int value = qBound(0, verticalValue, flowPositions.count() - 1);
    if (above)
        return perItemScrollToValue(itemIndex, value, areaHeight, QListView::PositionAtTop,
                                    Qt::Vertical,wrap, itemHeight);
    else if (below)
        return perItemScrollToValue(itemIndex, value, areaHeight, QListView::PositionAtBottom,
                                    Qt::Vertical, wrap, itemHeight);
    else if (hint != QListView::EnsureVisible)
        return perItemScrollToValue(itemIndex, value, areaHeight, hint, Qt::Vertical, wrap, itemHeight);
    return value;
}

int QStaticListViewBase::horizontalPerItemValue(int itemIndex, int horizontalValue, int areaWidth,
                                                   bool leftOf, bool rightOf, bool wrap,
                                                   QListView::ScrollHint hint, int itemWidth) const
{
    int value = qBound(0, horizontalValue, flowPositions.count() - 1);
    if (leftOf)
        return perItemScrollToValue(itemIndex, value, areaWidth, QListView::PositionAtTop,
                                    Qt::Horizontal, wrap, itemWidth);
    else if (rightOf)
        return perItemScrollToValue(itemIndex, value, areaWidth, QListView::PositionAtBottom,
                                    Qt::Horizontal, wrap, itemWidth);
    else if (hint != QListView::EnsureVisible)
        return perItemScrollToValue(itemIndex, value, areaWidth, hint, Qt::Horizontal, wrap, itemWidth);
    return value;
}

void QStaticListViewBase::scrollContentsBy(int &dx, int &dy)
{
    // ### reorder this logic
    const int verticalValue = verticalScrollBarValue();
    const int horizontalValue = horizontalScrollBarValue();
    const bool vertical = (verticalScrollMode() == QAbstractItemView::ScrollPerItem);
    const bool horizontal = (horizontalScrollMode() == QAbstractItemView::ScrollPerItem);

    if (isWrapping()) {
        if (segmentPositions.isEmpty())
            return;
        const int max = segmentPositions.count() - 1;
        if (horizontal && flow() == QListView::TopToBottom && dx != 0) {
            int currentValue = qBound(0, horizontalValue, max);
            int previousValue = qBound(0, currentValue + dx, max);
            int currentCoordinate = segmentPositions.at(currentValue);
            int previousCoordinate = segmentPositions.at(previousValue);
            dx = previousCoordinate - currentCoordinate;
        } else if (vertical && flow() == QListView::LeftToRight && dy != 0) {
            int currentValue = qBound(0, verticalValue, max);
            int previousValue = qBound(0, currentValue + dy, max);
            int currentCoordinate = segmentPositions.at(currentValue);
            int previousCoordinate = segmentPositions.at(previousValue);
            dy = previousCoordinate - currentCoordinate;
        }
    } else {
        if (flowPositions.isEmpty())
            return;
        const int max = flowPositions.count() - 1;
        if (vertical && flow() == QListView::TopToBottom && dy != 0) {
            int currentValue = qBound(0, verticalValue, max);
            int previousValue = qBound(0, currentValue + dy, max);
            int currentCoordinate = flowPositions.at(currentValue);
            int previousCoordinate = flowPositions.at(previousValue);
            dy = previousCoordinate - currentCoordinate;
        } else if (horizontal && flow() == QListView::LeftToRight && dx != 0) {
            int currentValue = qBound(0, horizontalValue, max);
            int previousValue = qBound(0, currentValue + dx, max);
            int currentCoordinate = flowPositions.at(currentValue);
            int previousCoordinate = flowPositions.at(previousValue);
            dx = previousCoordinate - currentCoordinate;
        }
    }
}

bool QStaticListViewBase::doBatchedItemLayout(const QListViewLayoutInfo &info, int max)
{
    doStaticLayout(info);
    if (batchStartRow > max) { // stop items layout
        flowPositions.resize(flowPositions.count());
        segmentPositions.resize(segmentPositions.count());
        segmentStartRows.resize(segmentStartRows.count());
        return true; // done
    }
    return false; // not done
}

QListViewItem QStaticListViewBase::indexToListViewItem(const QModelIndex &index) const
{
    if (flowPositions.isEmpty()
        || segmentPositions.isEmpty()
        || index.row() > flowPositions.count())
        return QListViewItem();

    const int segment = qBinarySearch<int>(segmentStartRows, index.row(),
                                           0, segmentStartRows.count() - 1);


    QStyleOptionViewItemV4 options = viewOptions();
    options.rect.setSize(contentsSize);
    QSize size = (uniformItemSizes() && cachedItemSize().isValid())
                 ? cachedItemSize() : itemSize(options, index);

    QPoint pos;
    if (flow() == QListView::LeftToRight) {
        pos.setX(flowPositions.at(index.row()));
        pos.setY(segmentPositions.at(segment));
    } else { // TopToBottom
        pos.setY(flowPositions.at(index.row()));
        pos.setX(segmentPositions.at(segment));
        if (isWrapping()) { // make the items as wide as the segment
            int right = (segment + 1 >= segmentPositions.count()
                     ? contentsSize.width()
                     : segmentPositions.at(segment + 1));
            size.setWidth(right - pos.x());
        } else { // make the items as wide as the viewport
            size.setWidth(qMax(size.width(), viewport()->width()));
        }
    }

    return QListViewItem(QRect(pos, size), index.row());
}

QPoint QStaticListViewBase::initStaticLayout(const QListViewLayoutInfo &info)
{
    int x, y;
    if (info.first == 0) {
        flowPositions.clear();
        segmentPositions.clear();
        segmentStartRows.clear();
        segmentExtents.clear();
        x = info.bounds.left() + info.spacing;
        y = info.bounds.top() + info.spacing;
        segmentPositions.append(info.flow == QListView::LeftToRight ? y : x);
        segmentStartRows.append(0);
    } else if (info.wrap) {
        if (info.flow == QListView::LeftToRight) {
            x = batchSavedPosition;
            y = segmentPositions.last();
        } else { // flow == QListView::TopToBottom
            x = segmentPositions.last();
            y = batchSavedPosition;
        }
    } else { // not first and not wrap
        if (info.flow == QListView::LeftToRight) {
            x = batchSavedPosition;
            y = info.bounds.top() + info.spacing;
        } else { // flow == QListView::TopToBottom
            x = info.bounds.left() + info.spacing;
            y = batchSavedPosition;
        }
    }
    return QPoint(x, y);
}

/*!
  \internal
*/
void QStaticListViewBase::doStaticLayout(const QListViewLayoutInfo &info)
{
    const bool useItemSize = !info.grid.isValid();
    const QPoint topLeft = initStaticLayout(info);
    QStyleOptionViewItemV4 option = viewOptions();
    option.rect = info.bounds;

    // The static layout data structures are as follows:
    // One vector contains the coordinate in the direction of layout flow.
    // Another vector contains the coordinates of the segments.
    // A third vector contains the index (model row) of the first item
    // of each segment.

    int segStartPosition;
    int segEndPosition;
    int deltaFlowPosition;
    int deltaSegPosition;
    int deltaSegHint;
    int flowPosition;
    int segPosition;

    if (info.flow == QListView::LeftToRight) {
        segStartPosition = info.bounds.left();
        segEndPosition = info.bounds.width();
        flowPosition = topLeft.x();
        segPosition = topLeft.y();
        deltaFlowPosition = info.grid.width(); // dx
        deltaSegPosition = useItemSize ? batchSavedDeltaSeg : info.grid.height(); // dy
        deltaSegHint = info.grid.height();
    } else { // flow == QListView::TopToBottom
        segStartPosition = info.bounds.top();
        segEndPosition = info.bounds.height();
        flowPosition = topLeft.y();
        segPosition = topLeft.x();
        deltaFlowPosition = info.grid.height(); // dy
        deltaSegPosition = useItemSize ? batchSavedDeltaSeg : info.grid.width(); // dx
        deltaSegHint = info.grid.width();
    }

    for (int row = info.first; row <= info.last; ++row) {
        if (isHidden(row)) { // ###
            flowPositions.append(flowPosition);
        } else {
            // if we are not using a grid, we need to find the deltas
            if (useItemSize) {
                QSize hint = itemSize(option, modelIndex(row));
                if (info.flow == QListView::LeftToRight) {
                    deltaFlowPosition = hint.width() + info.spacing;
                    deltaSegHint = hint.height() + info.spacing;
                } else { // TopToBottom
                    deltaFlowPosition = hint.height() + info.spacing;
                    deltaSegHint = hint.width() + info.spacing;
                }
            }
            // create new segment
            if (info.wrap && (flowPosition + deltaFlowPosition >= segEndPosition)) {
                segmentExtents.append(flowPosition);
                flowPosition = info.spacing + segStartPosition;
                segPosition += deltaSegPosition;
                segmentPositions.append(segPosition);
                segmentStartRows.append(row);
                deltaSegPosition = 0;
            }
            // save the flow position of this item
            flowPositions.append(flowPosition);
            // prepare for the next item
            deltaSegPosition = qMax(deltaSegHint, deltaSegPosition);
            flowPosition += info.spacing + deltaFlowPosition;
        }
    }
    // used when laying out next batch
    batchSavedPosition = flowPosition;
    batchSavedDeltaSeg = deltaSegPosition;
    batchStartRow = info.last + 1;
    if (info.last == info.max)
        flowPosition -= info.spacing; // remove extra spacing
    // set the contents size
    QRect rect = info.bounds;
    if (info.flow == QListView::LeftToRight) {
        rect.setRight(segmentPositions.count() == 1 ? flowPosition : info.bounds.right());
        rect.setBottom(segPosition + deltaSegPosition);
    } else { // TopToBottom
        rect.setRight(segPosition + deltaSegPosition);
        rect.setBottom(segmentPositions.count() == 1 ? flowPosition : info.bounds.bottom());
    }
    contentsSize = QSize(rect.right(), rect.bottom());
    // if it is the last batch, save the end of the segments
    if (info.last == info.max) {
        segmentExtents.append(flowPosition);
        flowPositions.append(flowPosition);
        segmentPositions.append(info.wrap ? segPosition + deltaSegPosition : INT_MAX);
    }
    // if the new items are visble, update the viewport
    QRect changedRect(topLeft, rect.bottomRight());
    if (clipRect().intersects(changedRect))
        viewport()->update();
}

/*!
  \internal
  Finds the set of items intersecting with \a area.
  In this function, itemsize is counted from topleft to the start of the next item.
*/
void QStaticListViewBase::intersectingStaticSet(const QRect &area) const
{
    clearIntersections();
    int segStartPosition;
    int segEndPosition;
    int flowStartPosition;
    int flowEndPosition;
    if (flow() == QListView::LeftToRight) {
        segStartPosition = area.top();
        segEndPosition = area.bottom();
        flowStartPosition = area.left();
        flowEndPosition = area.right();
    } else {
        segStartPosition = area.left();
        segEndPosition = area.right();
        flowStartPosition = area.top();
        flowEndPosition = area.bottom();
    }
    if (segmentPositions.count() < 2 || flowPositions.isEmpty())
        return;
    // the last segment position is actually the edge of the last segment
    const int segLast = segmentPositions.count() - 2;
    int seg = qBinarySearch<int>(segmentPositions, segStartPosition, 0, segLast + 1);
    for (; seg <= segLast && segmentPositions.at(seg) <= segEndPosition; ++seg) {
        int first = segmentStartRows.at(seg);
        int last = (seg < segLast ? segmentStartRows.at(seg + 1) : batchStartRow) - 1;
        if (segmentExtents.at(seg) < flowStartPosition)
            continue;
        int row = qBinarySearch<int>(flowPositions, flowStartPosition, first, last);
        for (; row <= last && flowPositions.at(row) <= flowEndPosition; ++row) {
            if (isHidden(row))
                continue;
            QModelIndex index = modelIndex(row);
            if (index.isValid())
                appendToIntersections(index);
#if 0 // for debugging
            else
                qWarning("intersectingStaticSet: row %d was invalid", row);
#endif
        }
    }
}

int QStaticListViewBase::itemIndex(const QListViewItem &item) const
{
    return item.indexHint;
}

QRect QStaticListViewBase::mapToViewport(const QRect &rect) const
{
    if (isWrapping())
        return rect;
    // If the listview is in "listbox-mode", the items are as wide as the view.
    QRect result = rect;
    QSize vsize = viewport()->size();
    QSize csize = contentsSize;
    if (flow() == QListView::TopToBottom) {
        result.setLeft(spacing());
        result.setWidth(qMax(csize.width(), vsize.width()) - 2 * spacing());
    } else { // LeftToRight
        result.setTop(spacing());
        result.setHeight(qMax(csize.height(), vsize.height()) - 2 * spacing());
    }
    return result;
}

int QStaticListViewBase::perItemScrollingPageSteps(int length, int bounds, bool wrap) const
{
    const QVector<int> positions = (wrap ? segmentPositions : flowPositions);
    if (positions.isEmpty() || bounds <= length)
        return positions.count();
    if (uniformItemSizes()) {
        for (int i = 1; i < positions.count(); ++i)
            if (positions.at(i) > 0)
                return length / positions.at(i);
        return 0; // all items had height 0
    }
    int pageSteps = 0;
    int steps = positions.count() - 1;
    int max = qMax(length, bounds);
    int min = qMin(length, bounds);
    int pos = min - (max - positions.last());

    while (pos >= 0 && steps > 0) {
        pos -= (positions.at(steps) - positions.at(steps - 1));
        if (pos >= 0) //this item should be visible
            ++pageSteps;
        --steps;
    }

    // at this point we know that positions has at least one entry
    return qMax(pageSteps, 1);
}

int QStaticListViewBase::perItemScrollToValue(int index, int scrollValue, int viewportSize,
                                                 QAbstractItemView::ScrollHint hint,
                                                 Qt::Orientation orientation, bool wrap, int itemExtent) const
{
    if (index < 0)
        return scrollValue;
    if (!wrap) {
        int topIndex = index;
        const int bottomIndex = topIndex;
        const int bottomCoordinate = flowPositions.at(index);

        while (topIndex > 0 &&
            (bottomCoordinate - flowPositions.at(topIndex-1) + itemExtent) <= (viewportSize)) {
            topIndex--;
        }

        const int itemCount = bottomIndex - topIndex + 1;
        switch (hint) {
        case QAbstractItemView::PositionAtTop:
            return index;
        case QAbstractItemView::PositionAtBottom:
            return index - itemCount + 1;
        case QAbstractItemView::PositionAtCenter:
            return index - (itemCount / 2);
        default:
            break;
        }
    } else { // wrapping
        Qt::Orientation flowOrientation = (flow() == QListView::LeftToRight
                                           ? Qt::Horizontal : Qt::Vertical);
        if (flowOrientation == orientation) { // scrolling in the "flow" direction
            // ### wrapped scrolling in the flow direction
            return flowPositions.at(index); // ### always pixel based for now
        } else if (!segmentStartRows.isEmpty()) { // we are scrolling in the "segment" direction
            int segment = qBinarySearch<int>(segmentStartRows, index, 0, segmentStartRows.count() - 1);
            int leftSegment = segment;
            const int rightSegment = leftSegment;
            const int bottomCoordinate = segmentPositions.at(segment);

            while (leftSegment > scrollValue &&
                (bottomCoordinate - segmentPositions.at(leftSegment-1) + itemExtent) <= (viewportSize)) {
                    leftSegment--;
            }

            const int segmentCount = rightSegment - leftSegment + 1;
            switch (hint) {
            case QAbstractItemView::PositionAtTop:
                return segment;
            case QAbstractItemView::PositionAtBottom:
                return segment - segmentCount + 1;
            case QAbstractItemView::PositionAtCenter:
                return segment - (segmentCount / 2);
            default:
                break;
            }
        }
    }
    return scrollValue;
}

void QStaticListViewBase::clear()
{
    flowPositions.clear();
    segmentPositions.clear();
    segmentStartRows.clear();
    segmentExtents.clear();
    batchSavedPosition = 0;
    batchStartRow = 0;
    batchSavedDeltaSeg = 0;
}

/*
 * Dynamic ListView Implementation
*/

void QDynamicListViewBase::dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    if (column() >= topLeft.column() && column() <= bottomRight.column())  {
        QStyleOptionViewItemV4 option = viewOptions();
        int bottom = qMin(items.count(), bottomRight.row() + 1);
        for (int row = topLeft.row(); row < bottom; ++row)
            items[row].resize(itemSize(option, modelIndex(row)));
    }
}

bool QDynamicListViewBase::doBatchedItemLayout(const QListViewLayoutInfo &info, int max)
{
    if (info.last >= items.count()) {
        createItems(info.last + 1);
        doDynamicLayout(info);
    }
    return (batchStartRow > max); // done
}

QListViewItem QDynamicListViewBase::indexToListViewItem(const QModelIndex &index) const
{
    if (index.isValid() && index.row() < items.count())
        return items.at(index.row());
    return QListViewItem();
}

void QDynamicListViewBase::initBspTree(const QSize &contents)
{
    // remove all items from the tree
    int leafCount = tree.leafCount();
    for (int l = 0; l < leafCount; ++l)
        tree.leaf(l).clear();
    // we have to get the bounding rect of the items before we can initialize the tree
    QBspTree::Node::Type type = QBspTree::Node::Both; // 2D
    // simple heuristics to get better bsp
    if (contents.height() / contents.width() >= 3)
        type = QBspTree::Node::HorizontalPlane;
    else if (contents.width() / contents.height() >= 3)
        type = QBspTree::Node::VerticalPlane;
    // build tree for the bounding rect (not just the contents rect)
    tree.init(QRect(0, 0, contents.width(), contents.height()), type);
}

QPoint QDynamicListViewBase::initDynamicLayout(const QListViewLayoutInfo &info)
{
    int x, y;
    if (info.first == 0) {
        x = info.bounds.x() + info.spacing;
        y = info.bounds.y() + info.spacing;
        items.reserve(rowCount() - hiddenCount());
    } else {
        const QListViewItem item = items.at(info.first - 1);
        x = item.x;
        y = item.y;
        if (info.flow == QListView::LeftToRight)
            x += (info.grid.isValid() ? info.grid.width() : item.w) + info.spacing;
        else
            y += (info.grid.isValid() ? info.grid.height() : item.h) + info.spacing;
    }
    return QPoint(x, y);
}

/*!
  \internal
*/
void QDynamicListViewBase::doDynamicLayout(const QListViewLayoutInfo &info)
{
    const bool useItemSize = !info.grid.isValid();
    const QPoint topLeft = initDynamicLayout(info);

    int segStartPosition;
    int segEndPosition;
    int deltaFlowPosition;
    int deltaSegPosition;
    int deltaSegHint;
    int flowPosition;
    int segPosition;

    if (info.flow == QListView::LeftToRight) {
        segStartPosition = info.bounds.left() + info.spacing;
        segEndPosition = info.bounds.right();
        deltaFlowPosition = info.grid.width(); // dx
        deltaSegPosition = (useItemSize ? batchSavedDeltaSeg : info.grid.height()); // dy
        deltaSegHint = info.grid.height();
        flowPosition = topLeft.x();
        segPosition = topLeft.y();
    } else { // flow == QListView::TopToBottom
        segStartPosition = info.bounds.top() + info.spacing;
        segEndPosition = info.bounds.bottom();
        deltaFlowPosition = info.grid.height(); // dy
        deltaSegPosition = (useItemSize ? batchSavedDeltaSeg : info.grid.width()); // dx
        deltaSegHint = info.grid.width();
        flowPosition = topLeft.y();
        segPosition = topLeft.x();
    }

    if (moved.count() != items.count())
        moved.resize(items.count());

    QRect rect(QPoint(0, 0), topLeft);
    QListViewItem *item = 0;
    for (int row = info.first; row <= info.last; ++row) {
        item = &items[row];
        if (isHidden(row)) {
            item->invalidate();
        } else {
            // if we are not using a grid, we need to find the deltas
            if (useItemSize) {
                if (info.flow == QListView::LeftToRight)
                    deltaFlowPosition = item->w + info.spacing;
                else
                    deltaFlowPosition = item->h + info.spacing;
            } else {
                item->w = qMin<int>(info.grid.width(), item->w);
                item->h = qMin<int>(info.grid.height(), item->h);
            }

            // create new segment
            if (info.wrap
                && flowPosition + deltaFlowPosition > segEndPosition
                && flowPosition > segStartPosition) {
                flowPosition = segStartPosition;
                segPosition += deltaSegPosition;
                if (useItemSize)
                    deltaSegPosition = 0;
            }
            // We must delay calculation of the seg adjustment, as this item
            // may have caused a wrap to occur
            if (useItemSize) {
                if (info.flow == QListView::LeftToRight)
                    deltaSegHint = item->h + info.spacing;
                else
                    deltaSegHint = item->w + info.spacing;
                deltaSegPosition = qMax(deltaSegPosition, deltaSegHint);
            }

            // set the position of the item
            // ### idealy we should have some sort of alignment hint for the item
            // ### (normally that would be a point between the icon and the text)
            if (!moved.testBit(row)) {
                if (info.flow == QListView::LeftToRight) {
                    if (useItemSize) {
                        item->x = flowPosition;
                        item->y = segPosition;
                    } else { // use grid
                        item->x = flowPosition + ((deltaFlowPosition - item->w) / 2);
                        item->y = segPosition;
                    }
                } else { // TopToBottom
                    if (useItemSize) {
                        item->y = flowPosition;
                        item->x = segPosition;
                    } else { // use grid
                        item->y = flowPosition + ((deltaFlowPosition - item->h) / 2);
                        item->x = segPosition;
                    }
                }
            }

            // let the contents contain the new item
            if (useItemSize)
                rect |= item->rect();
            else if (info.flow == QListView::LeftToRight)
                rect |= QRect(flowPosition, segPosition, deltaFlowPosition, deltaSegPosition);
            else // flow == TopToBottom
                rect |= QRect(segPosition, flowPosition, deltaSegPosition, deltaFlowPosition);

            // prepare for next item
            flowPosition += deltaFlowPosition; // current position + item width + gap
        }
    }
    batchSavedDeltaSeg = deltaSegPosition;
    batchStartRow = info.last + 1;
    bool done = (info.last >= rowCount() - 1);
    // resize the content area
    if (done || !info.bounds.contains(item->rect()))
        contentsSize = QSize(rect.width(), rect.height());
    // resize tree
    int insertFrom = info.first;
    if (done || info.first == 0) {
        initBspTree(rect.size());
        insertFrom = 0;
    }
    // insert items in tree
    for (int row = insertFrom; row <= info.last; ++row)
        tree.insertLeaf(items.at(row).rect(), row);
    // if the new items are visble, update the viewport
    QRect changedRect(topLeft, rect.bottomRight());
    if (clipRect().intersects(changedRect))
        viewport()->update();
}

void QDynamicListViewBase::intersectingDynamicSet(const QRect &area) const
{
    clearIntersections();
    QListViewPrivate *that = const_cast<QListViewPrivate*>(dd);
    QBspTree::Data data(static_cast<void*>(that));
    that->dynamicListView->tree.climbTree(area, &QDynamicListViewBase::addLeaf, data);
}

void QDynamicListViewBase::createItems(int to)
{
    int count = items.count();
    QSize size;
    QStyleOptionViewItemV4 option = viewOptions();
    for (int row = count; row < to; ++row) {
        size = itemSize(option, modelIndex(row));
        QListViewItem item(QRect(0, 0, size.width(), size.height()), row); // default pos
        items.append(item);
    }
}

void QDynamicListViewBase::drawItems(QPainter *painter, const QVector<QModelIndex> &indexes) const
{
    QStyleOptionViewItemV4 option = viewOptions();
    option.state &= ~QStyle::State_MouseOver;
    QVector<QModelIndex>::const_iterator it = indexes.begin();
    QListViewItem item = indexToListViewItem(*it);
    for (; it != indexes.end(); ++it) {
        item = indexToListViewItem(*it);
        option.rect = viewItemRect(item);
        delegate(*it)->paint(painter, option, *it);
    }
}

QRect QDynamicListViewBase::itemsRect(const QVector<QModelIndex> &indexes) const
{
    QVector<QModelIndex>::const_iterator it = indexes.begin();
    QListViewItem item = indexToListViewItem(*it);
    QRect rect(item.x, item.y, item.w, item.h);
    for (; it != indexes.end(); ++it) {
        item = indexToListViewItem(*it);
        rect |= viewItemRect(item);
    }
    return rect;
}

int QDynamicListViewBase::itemIndex(const QListViewItem &item) const
{
    if (!item.isValid())
        return -1;
    int i = item.indexHint;
    if (i < items.count()) {
        if (items.at(i) == item)
            return i;
    } else {
        i = items.count() - 1;
    }

    int j = i;
    int c = items.count();
    bool a = true;
    bool b = true;

    while (a || b) {
        if (a) {
            if (items.at(i) == item) {
                items.at(i).indexHint = i;
                return i;
            }
            a = ++i < c;
        }
        if (b) {
            if (items.at(j) == item) {
                items.at(j).indexHint = j;
                return j;
            }
            b = --j > -1;
        }
    }
    return -1;
}

void QDynamicListViewBase::addLeaf(QVector<int> &leaf, const QRect &area,
                               uint visited, QBspTree::Data data)
{
    QListViewItem *vi;
    QListViewPrivate *_this = static_cast<QListViewPrivate *>(data.ptr);
    for (int i = 0; i < leaf.count(); ++i) {
        int idx = leaf.at(i);
        if (idx < 0 || idx >= _this->dynamicListView->items.count())
            continue;
        vi = &_this->dynamicListView->items[idx];
        Q_ASSERT(vi);
        if (vi->rect().intersects(area) && vi->visited != visited) {
            QModelIndex index = _this->listViewItemToIndex(*vi);
            Q_ASSERT(index.isValid());
            _this->intersectVector.append(index);
            vi->visited = visited;
        }
    }
}

void QDynamicListViewBase::insertItem(int index)
{
    if (index >= 0 && index < items.count())
        tree.insertLeaf(items.at(index).rect(), index);
}

void QDynamicListViewBase::removeItem(int index)
{
    if (index >= 0 && index < items.count())
        tree.removeLeaf(items.at(index).rect(), index);
}

void QDynamicListViewBase::moveItem(int index, const QPoint &dest)
{
    // does not impact on the bintree itself or the contents rect
    QListViewItem *item = &items[index];
    QRect rect = item->rect();

    // move the item without removing it from the tree
    tree.removeLeaf(rect, index);
    item->move(dest);
    tree.insertLeaf(QRect(dest, rect.size()), index);

    // resize the contents area
    contentsSize = (QRect(QPoint(0, 0), contentsSize)|QRect(dest, rect.size())).size();

    // mark the item as moved
    if (moved.count() != items.count())
        moved.resize(items.count());
    moved.setBit(index, true);
}

QPoint QDynamicListViewBase::snapToGrid(const QPoint &pos) const
{
    int x = pos.x() - (pos.x() % gridSize().width());
    int y = pos.y() - (pos.y() % gridSize().height());
    return QPoint(x, y);
}

QPoint QDynamicListViewBase::draggedItemsDelta() const
{
    if (movement() == QListView::Snap) {
        QPoint snapdelta = QPoint((offset().x() % gridSize().width()),
                                  (offset().y() % gridSize().height()));
        return snapToGrid(draggedItemsPos + snapdelta) - snapToGrid(pressedPosition()) - snapdelta;
    }
    return draggedItemsPos - pressedPosition();
}

QRect QDynamicListViewBase::draggedItemsRect() const
{
    QRect rect = itemsRect(draggedItems);
    rect.translate(draggedItemsDelta());
    return rect;
}

void QListViewPrivate::scrollElasticBandBy(int dx, int dy)
{
    if (dx > 0) // right
        elasticBand.moveRight(elasticBand.right() + dx);
    else if (dx < 0) // left
        elasticBand.moveLeft(elasticBand.left() - dx);
    if (dy > 0) // down
        elasticBand.moveBottom(elasticBand.bottom() + dy);
    else if (dy < 0) // up
        elasticBand.moveTop(elasticBand.top() - dy);
}

void QDynamicListViewBase::clear()
{
    tree.destroy();
    items.clear();
    moved.clear();
    batchStartRow = 0;
    batchSavedDeltaSeg = 0;
}

void QDynamicListViewBase::updateContentsSize()
{
    QRect bounding;
    for (int i = 0; i < items.count(); ++i)
        bounding |= items.at(i).rect();
    contentsSize = bounding.size();
}

/*!
  \reimp
*/
void QListView::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
#ifndef QT_NO_ACCESSIBILITY
    if (QAccessible::isActive()) {
        if (current.isValid()) {
            int entry = visualIndex(current) + 1;
            QAccessible::updateAccessibility(viewport(), entry, QAccessible::Focus);
        }
    }
#endif
    QAbstractItemView::currentChanged(current, previous);
}

/*!
  \reimp
*/
void QListView::selectionChanged(const QItemSelection &selected,
                                 const QItemSelection &deselected)
{
#ifndef QT_NO_ACCESSIBILITY
    if (QAccessible::isActive()) {
        // ### does not work properly for selection ranges.
        QModelIndex sel = selected.indexes().value(0);
        if (sel.isValid()) {
            int entry = visualIndex(sel) + 1;
            QAccessible::updateAccessibility(viewport(), entry, QAccessible::Selection);
        }
        QModelIndex desel = deselected.indexes().value(0);
        if (desel.isValid()) {
            int entry = visualIndex(desel) + 1;
            QAccessible::updateAccessibility(viewport(), entry, QAccessible::SelectionRemove);
        }
    }
#endif
    QAbstractItemView::selectionChanged(selected, deselected);
}

int QListView::visualIndex(const QModelIndex &index) const
{
    Q_D(const QListView);
    d->executePostedLayout();
    QListViewItem itm = d->indexToListViewItem(index);
    return d->itemIndex(itm);
}

QT_END_NAMESPACE

#endif // QT_NO_LISTVIEW
