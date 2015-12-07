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

#include "qtableview.h"

#ifndef QT_NO_TABLEVIEW
#include <qheaderview.h>
#include <qitemdelegate.h>
#include <qapplication.h>
#include <qpainter.h>
#include <qstyle.h>
#include <qsize.h>
#include <qevent.h>
#include <qbitarray.h>
#include <qscrollbar.h>
#include <qabstractbutton.h>
#include <private/qtableview_p.h>
#ifndef QT_NO_ACCESSIBILITY
#include <qaccessible.h>
#endif

QT_BEGIN_NAMESPACE

class QTableCornerButton : public QAbstractButton
{
    Q_OBJECT
public:
    QTableCornerButton(QWidget *parent) : QAbstractButton(parent) {}
    void paintEvent(QPaintEvent*) {
        QStyleOptionHeader opt;
        opt.init(this);
        QStyle::State state = QStyle::State_None;
        if (isEnabled())
            state |= QStyle::State_Enabled;
        if (isActiveWindow())
            state |= QStyle::State_Active;
        if (isDown())
            state |= QStyle::State_Sunken;
        opt.state = state;
        opt.rect = rect();
        opt.position = QStyleOptionHeader::OnlyOneSection;
        QPainter painter(this);
        style()->drawControl(QStyle::CE_Header, &opt, &painter, this);
    }
};

void QTableViewPrivate::init()
{
    Q_Q(QTableView);

    q->setEditTriggers(editTriggers|QAbstractItemView::AnyKeyPressed);

    QHeaderView *vertical = new QHeaderView(Qt::Vertical, q);
    vertical->setClickable(true);
    vertical->setHighlightSections(true);
    q->setVerticalHeader(vertical);

    QHeaderView *horizontal = new QHeaderView(Qt::Horizontal, q);
    horizontal->setClickable(true);
    horizontal->setHighlightSections(true);
    q->setHorizontalHeader(horizontal);

    tabKeyNavigation = true;

    cornerWidget = new QTableCornerButton(q);
    cornerWidget->setFocusPolicy(Qt::NoFocus);
    QObject::connect(cornerWidget, SIGNAL(clicked()), q, SLOT(selectAll()));
}

/*!
  \internal
  Trims away indices that are hidden in the treeview due to hidden horizontal or vertical sections.
*/
void QTableViewPrivate::trimHiddenSelections(QItemSelectionRange *range) const
{
    Q_ASSERT(range && range->isValid());

    int top = range->top();
    int left = range->left();
    int bottom = range->bottom();
    int right = range->right();

    while (bottom >= top && verticalHeader->isSectionHidden(bottom))
        --bottom;
    while (right >= left && horizontalHeader->isSectionHidden(right))
        --right;

    if (top > bottom || left > right) { // everything is hidden
        *range = QItemSelectionRange();
        return;
    }

    while (verticalHeader->isSectionHidden(top) && top <= bottom)
        ++top;
    while (horizontalHeader->isSectionHidden(left) && left <= right)
        ++left;

    if (top > bottom || left > right) { // everything is hidden
        *range = QItemSelectionRange();
        return;
    }

    QModelIndex bottomRight = model->index(bottom, right, range->parent());
    QModelIndex topLeft = model->index(top, left, range->parent());
    *range = QItemSelectionRange(topLeft, bottomRight);
}

/*!
  \internal
  Sets the span for the cell at (\a row, \a column).
*/
void QTableViewPrivate::setSpan(int row, int column, int rowSpan, int columnSpan)
{
    if (row < 0 || column < 0 || rowSpan < 0 || columnSpan < 0)
        return;
    Span sp(row, column, rowSpan, columnSpan);
    QList<Span>::iterator it;
    for (it = spans.begin(); it != spans.end(); ++it) {
        if (((*it).top() == sp.top()) && ((*it).left() == sp.left())) {
            if ((sp.height() == 1) && (sp.width() == 1))
                spans.erase(it); // "Implicit" span (1, 1), no need to store it
            else
                *it = sp; // Replace
            return;
        }
    }
    spans.append(sp);
}

/*!
  \internal
  Gets the span information for the cell at (\a row, \a column).
*/
QTableViewPrivate::Span QTableViewPrivate::span(int row, int column) const
{
    QList<Span>::const_iterator it;
    for (it = spans.constBegin(); it != spans.constEnd(); ++it) {
        Span span = *it;
        if (isInSpan(row, column, span))
            return span;
    }
    return Span(row, column, 1, 1);
}

/*!
  \internal
  Returns the logical index of the last section that's part of the span.
*/
int QTableViewPrivate::sectionSpanEndLogical(const QHeaderView *header, int logical, int span) const
{
    int visual = header->visualIndex(logical);
    for (int i = 1; i < span; ) {
        if (++visual >= header->count())
            break;
        logical = header->logicalIndex(visual);
        ++i;
    }
    return logical;
}

/*!
  \internal
  Returns the size of the span starting at logical index \a logical
  and spanning \a span sections.
*/
int QTableViewPrivate::sectionSpanSize(const QHeaderView *header, int logical, int span) const
{
    int endLogical = sectionSpanEndLogical(header, logical, span);
    return header->sectionPosition(endLogical)
        - header->sectionPosition(logical)
        + header->sectionSize(endLogical);
}

/*!
  \internal
  Returns true if the section at logical index \a logical is part of the span
  starting at logical index \a spanLogical and spanning \a span sections;
  otherwise, returns false.
*/
bool QTableViewPrivate::spanContainsSection(const QHeaderView *header, int logical, int spanLogical, int span) const
{
    if (logical == spanLogical)
        return true; // it's the start of the span
    int visual = header->visualIndex(spanLogical);
    for (int i = 1; i < span; ) {
        if (++visual >= header->count())
            break;
        spanLogical = header->logicalIndex(visual);
        if (logical == spanLogical)
            return true;
        ++i;
    }
    return false;
}

/*!
  \internal
  Returns true if one or more spans intersect column \a column.
*/
bool QTableViewPrivate::spansIntersectColumn(int column) const
{
    QList<Span>::const_iterator it;
    for (it = spans.constBegin(); it != spans.constEnd(); ++it) {
        Span span = *it;
        if (spanContainsColumn(column, span.left(), span.width()))
            return true;
    }
    return false;
}

/*!
  \internal
  Returns true if one or more spans intersect row \a row.
*/
bool QTableViewPrivate::spansIntersectRow(int row) const
{
    QList<Span>::const_iterator it;
    for (it = spans.constBegin(); it != spans.constEnd(); ++it) {
        Span span = *it;
        if (spanContainsRow(row, span.top(), span.height()))
            return true;
    }
    return false;
}

/*!
  \internal
  Returns true if one or more spans intersect one or more columns.
*/
bool QTableViewPrivate::spansIntersectColumns(const QList<int> &columns) const
{
    QList<int>::const_iterator it;
    for (it = columns.constBegin(); it != columns.constEnd(); ++it) {
        if (spansIntersectColumn(*it))
            return true;
    }
    return false;
}

/*!
  \internal
  Returns true if one or more spans intersect one or more rows.
*/
bool QTableViewPrivate::spansIntersectRows(const QList<int> &rows) const
{
    QList<int>::const_iterator it;
    for (it = rows.constBegin(); it != rows.constEnd(); ++it) {
        if (spansIntersectRow(*it))
            return true;
    }
    return false;
}

/*!
  \internal
  Returns the visual rect for the given \a span.
*/
QRect QTableViewPrivate::visualSpanRect(const Span &span) const
{
    Q_Q(const QTableView);
    // vertical
    int row = span.top();
    int rowp = verticalHeader->sectionViewportPosition(row);
    int rowh = rowSpanHeight(row, span.height());
    // horizontal
    int column = span.left();
    int colw = columnSpanWidth(column, span.width());
    if (q->isRightToLeft())
        column = span.right();
    int colp = horizontalHeader->sectionViewportPosition(column);

    const int i = showGrid ? 1 : 0;
    if (q->isRightToLeft())
        return QRect(colp + i, rowp, colw - i, rowh - i);
    return QRect(colp, rowp, colw - i, rowh - i);
}

/*!
  \internal
  Draws the spanning cells within rect \a area, and clips them off as
  preparation for the main drawing loop.
  \a drawn is a QBitArray of visualRowCountxvisualCoulumnCount which say if particular cell has been drawn
*/
void QTableViewPrivate::drawAndClipSpans(const QRect &area, QPainter *painter,
                                              const QStyleOptionViewItemV4 &option,
                                              QBitArray *drawn)
{
    bool alternateBase = false;
    QRegion region = viewport->rect();

    int firstVisualRow = qMax(verticalHeader->visualIndexAt(0),0);
    int lastVisualRow = verticalHeader->visualIndexAt(viewport->height());
    if (lastVisualRow == -1)
        lastVisualRow = model->rowCount(root) - 1;

    int firstVisualColumn = horizontalHeader->visualIndexAt(0);
    int lastVisualColumn = horizontalHeader->visualIndexAt(viewport->width());
    if (q_func()->isRightToLeft())
        qSwap(firstVisualColumn, lastVisualColumn);
    if (firstVisualColumn == -1)
        firstVisualColumn = 0;
    if (lastVisualColumn == -1)
        lastVisualColumn = model->columnCount(root) - 1;

    QList<Span>::const_iterator it;
    for (it = spans.constBegin(); it != spans.constEnd(); ++it) {
        Span span = *it;

        int row = span.top();
        int col = span.left();
        if (isHidden(row, col))
            continue;
        QModelIndex index = model->index(row, col, root);
        if (!index.isValid())
            continue;
        QRect rect = visualSpanRect(span);
        rect.translate(scrollDelayOffset);
        if (!rect.intersects(area))
            continue;
        QStyleOptionViewItemV4 opt = option;
        opt.rect = rect;
        alternateBase = alternatingColors && (span.top() & 1);
        if (alternateBase)
            opt.features |= QStyleOptionViewItemV2::Alternate;
        else
            opt.features &= ~QStyleOptionViewItemV2::Alternate;
        drawCell(painter, opt, index);
        region -= rect;
        for (int r = span.top(); r <= span.bottom(); ++r) {
            const int vr = visualRow(r);
            if (vr < firstVisualRow || vr > lastVisualRow)
                continue;
            for (int c = span.left(); c <= span.right(); ++c) {
                const int vc = visualColumn(c);
                if (vc < firstVisualColumn  || vc > lastVisualColumn)
                    continue;
                drawn->setBit((vr - firstVisualRow) * (lastVisualColumn - firstVisualColumn + 1)
                             + vc - firstVisualColumn);
            }
        }

    }
    painter->setClipRegion(region);
}

/*!
  \internal
  Draws a table cell.
*/
void QTableViewPrivate::drawCell(QPainter *painter, const QStyleOptionViewItemV4 &option, const QModelIndex &index)
{
    Q_Q(QTableView);
    QStyleOptionViewItemV4 opt = option;

    if (selectionModel && selectionModel->isSelected(index))
        opt.state |= QStyle::State_Selected;
    if (index == hover)
        opt.state |= QStyle::State_MouseOver;
    if (option.state & QStyle::State_Enabled) {
        QPalette::ColorGroup cg;
        if ((model->flags(index) & Qt::ItemIsEnabled) == 0) {
            opt.state &= ~QStyle::State_Enabled;
            cg = QPalette::Disabled;
        } else {
            cg = QPalette::Normal;
        }
        opt.palette.setCurrentColorGroup(cg);
    }

    if (index == q->currentIndex()) {
        const bool focus = (q->hasFocus() || viewport->hasFocus()) && q->currentIndex().isValid();
        if (focus)
            opt.state |= QStyle::State_HasFocus;
    }

    if (opt.features & QStyleOptionViewItemV2::Alternate)
        painter->fillRect(opt.rect, opt.palette.brush(QPalette::AlternateBase));

    if (const QWidget *widget = editorForIndex(index).editor) {
        painter->save();
        painter->setClipRect(widget->geometry());
        q->itemDelegate(index)->paint(painter, opt, index);
        painter->restore();
    } else {
        q->itemDelegate(index)->paint(painter, opt, index);
    }
}

/*!
    \class QTableView

    \brief The QTableView class provides a default model/view
    implementation of a table view.

    \ingroup model-view
    \ingroup advanced
    \mainclass

    A QTableView implements a table view that displays items from a
    model. This class is used to provide standard tables that were
    previously provided by the QTable class, but using the more
    flexible approach provided by Qt's model/view architecture.

    The QTableView class is one of the \l{Model/View Classes}
    and is part of Qt's \l{Model/View Programming}{model/view framework}.

    QTableView implements the interfaces defined by the
    QAbstractItemView class to allow it to display data provided by
    models derived from the QAbstractItemModel class.

    \section1 Navigation

    You can navigate the cells in the table by clicking on a cell with the
    mouse, or by using the arrow keys. Because QTableView enables
    \l{QAbstractItemView::tabKeyNavigation}{tabKeyNavigation} by default, you
    can also hit Tab and Backtab to move from cell to cell.

    \section1 Visual Appearance

    The table has a vertical header that can be obtained using the
    verticalHeader() function, and a horizontal header that is available
    through the horizontalHeader() function. The height of each row in the
    table can be found by using rowHeight(); similarly, the width of
    columns can be found using columnWidth().  Since both of these are plain
    widgets, you can hide either of them using their hide() functions.

    Rows and columns can be hidden and shown with hideRow(), hideColumn(),
    showRow(), and showColumn(). They can be selected with selectRow()
    and selectColumn(). The table will show a grid depending on the
    \l showGrid property.

    The items shown in a table view, like those in the other item views, are
    rendered and edited using standard \l{QItemDelegate}{delegates}. However,
    for some tasks it is sometimes useful to be able to insert widgets in a
    table instead. Widgets are set for particular indexes with the
    \l{QAbstractItemView::}{setIndexWidget()} function, and
    later retrieved with \l{QAbstractItemView::}{indexWidget()}.

    \table
    \row \o \inlineimage qtableview-resized.png
    \o By default, the cells in a table do not expand to fill the available space.

    You can make the cells fill the available space by stretching the last
    header section. Access the relevant header using horizontalHeader()
    or verticalHeader() and set the header's \l{QHeaderView::}{stretchLastSection}
    property.

    To distribute the available space according to the space requirement of
    each column or row, call the view's resizeColumnsToContents() or
    resizeRowsToContents() functions.
    \endtable

    \section1 Coordinate Systems

    For some specialized forms of tables it is useful to be able to
    convert between row and column indexes and widget coordinates.
    The rowAt() function provides the y-coordinate within the view of the
    specified row; the row index can be used to obtain a corresponding
    y-coordinate with rowViewportPosition(). The columnAt() and
    columnViewportPosition() functions provide the equivalent conversion
    operations between x-coordinates and column indexes.

    \section1 Styles

    QTableView is styled appropriately for each platform. The following images show
    how it looks on three different platforms. Go to the \l{Qt Widget Gallery} to see
    its appearance in other styles.

    \table 100%
    \row \o \inlineimage windowsxp-tableview.png Screenshot of a Windows XP style table view
         \o \inlineimage macintosh-tableview.png Screenshot of a Macintosh style table view
         \o \inlineimage plastique-tableview.png Screenshot of a Plastique style table view
    \row \o A \l{Windows XP Style Widget Gallery}{Windows XP style} table view.
         \o A \l{Macintosh Style Widget Gallery}{Macintosh style} table view.
         \o A \l{Plastique Style Widget Gallery}{Plastique style} table view.
    \endtable

    \sa QTableWidget, {View Classes}, QAbstractItemModel, QAbstractItemView,
        {Chart Example}, {Pixelator Example}, {Table Model Example}
*/

/*!
    Constructs a table view with a \a parent to represent the data.

    \sa QAbstractItemModel
*/

QTableView::QTableView(QWidget *parent)
    : QAbstractItemView(*new QTableViewPrivate, parent)
{
    Q_D(QTableView);
    d->init();
}

/*!
  \internal
*/
QTableView::QTableView(QTableViewPrivate &dd, QWidget *parent)
    : QAbstractItemView(dd, parent)
{
    Q_D(QTableView);
    d->init();
}

/*!
  Destroys the table view.
*/
QTableView::~QTableView()
{
}

/*!
  \reimp
*/
void QTableView::setModel(QAbstractItemModel *model)
{
    Q_D(QTableView);
    d->verticalHeader->setModel(model);
    d->horizontalHeader->setModel(model);
    QAbstractItemView::setModel(model);
}

/*!
  \reimp
*/
void QTableView::setRootIndex(const QModelIndex &index)
{
    Q_D(QTableView);
    if (index == d->root) {
        viewport()->update();
        return;
    }
    d->verticalHeader->setRootIndex(index);
    d->horizontalHeader->setRootIndex(index);
    QAbstractItemView::setRootIndex(index);
}

/*!
  \reimp
*/
void QTableView::setSelectionModel(QItemSelectionModel *selectionModel)
{
    Q_D(QTableView);
    Q_ASSERT(selectionModel);
    d->verticalHeader->setSelectionModel(selectionModel);
    d->horizontalHeader->setSelectionModel(selectionModel);
    QAbstractItemView::setSelectionModel(selectionModel);
}

/*!
    Returns the table view's horizontal header.

    \sa setHorizontalHeader(), verticalHeader(), QAbstractItemModel::headerData()
*/
QHeaderView *QTableView::horizontalHeader() const
{
    Q_D(const QTableView);
    return d->horizontalHeader;
}

/*!
    Returns the table view's vertical header.

    \sa setVerticalHeader(), horizontalHeader(), QAbstractItemModel::headerData()
*/
QHeaderView *QTableView::verticalHeader() const
{
    Q_D(const QTableView);
    return d->verticalHeader;
}

/*!
    Sets the widget to use for the horizontal header to \a header.

    \sa horizontalHeader() setVerticalHeader()
*/
void QTableView::setHorizontalHeader(QHeaderView *header)
{
    Q_D(QTableView);

    if (!header || header == d->horizontalHeader)
        return;
    if (d->horizontalHeader && d->horizontalHeader->parent() == this)
        delete d->horizontalHeader;
    d->horizontalHeader = header;
    d->horizontalHeader->setParent(this);
    if (!d->horizontalHeader->model()) {
        d->horizontalHeader->setModel(d->model);
        if (d->selectionModel)
            d->horizontalHeader->setSelectionModel(d->selectionModel);
    }

    connect(d->horizontalHeader,SIGNAL(sectionResized(int,int,int)),
            this, SLOT(columnResized(int,int,int)));
    connect(d->horizontalHeader, SIGNAL(sectionMoved(int,int,int)),
            this, SLOT(columnMoved(int,int,int)));
    connect(d->horizontalHeader, SIGNAL(sectionCountChanged(int,int)),
            this, SLOT(columnCountChanged(int,int)));
    connect(d->horizontalHeader, SIGNAL(sectionPressed(int)), this, SLOT(selectColumn(int)));
    connect(d->horizontalHeader, SIGNAL(sectionEntered(int)), this, SLOT(_q_selectColumn(int)));
    connect(d->horizontalHeader, SIGNAL(sectionHandleDoubleClicked(int)),
            this, SLOT(resizeColumnToContents(int)));
    connect(d->horizontalHeader, SIGNAL(geometriesChanged()), this, SLOT(updateGeometries()));
}

/*!
    Sets the widget to use for the vertical header to \a header.

    \sa verticalHeader() setHorizontalHeader()
*/
void QTableView::setVerticalHeader(QHeaderView *header)
{
    Q_D(QTableView);

    if (!header || header == d->verticalHeader)
        return;
    if (d->verticalHeader && d->verticalHeader->parent() == this)
        delete d->verticalHeader;
    d->verticalHeader = header;
    d->verticalHeader->setParent(this);
    if (!d->verticalHeader->model()) {
        d->verticalHeader->setModel(d->model);
        if (d->selectionModel)
            d->verticalHeader->setSelectionModel(d->selectionModel);
    }

    connect(d->verticalHeader, SIGNAL(sectionResized(int,int,int)),
            this, SLOT(rowResized(int,int,int)));
    connect(d->verticalHeader, SIGNAL(sectionMoved(int,int,int)),
            this, SLOT(rowMoved(int,int,int)));
    connect(d->verticalHeader, SIGNAL(sectionCountChanged(int,int)),
            this, SLOT(rowCountChanged(int,int)));
    connect(d->verticalHeader, SIGNAL(sectionPressed(int)), this, SLOT(selectRow(int)));
    connect(d->verticalHeader, SIGNAL(sectionEntered(int)), this, SLOT(_q_selectRow(int)));
    connect(d->verticalHeader, SIGNAL(sectionHandleDoubleClicked(int)),
            this, SLOT(resizeRowToContents(int)));
    connect(d->verticalHeader, SIGNAL(geometriesChanged()), this, SLOT(updateGeometries()));
}

/*!
    \internal

    Scroll the contents of the table view by (\a dx, \a dy).
*/
void QTableView::scrollContentsBy(int dx, int dy)
{
    Q_D(QTableView);

    d->delayedAutoScroll.stop(); // auto scroll was canceled by the user scrolling

    dx = isRightToLeft() ? -dx : dx;
    if (dx) {
        if (horizontalScrollMode() == QAbstractItemView::ScrollPerItem) {
            int oldOffset = d->horizontalHeader->offset();
            if (horizontalScrollBar()->value() == horizontalScrollBar()->maximum())
                d->horizontalHeader->setOffsetToLastSection();
            else
                d->horizontalHeader->setOffsetToSectionPosition(horizontalScrollBar()->value());
            int newOffset = d->horizontalHeader->offset();
            dx = isRightToLeft() ? newOffset - oldOffset : oldOffset - newOffset;
        } else {
            d->horizontalHeader->setOffset(horizontalScrollBar()->value());
        }
    }
    if (dy) {
        if (verticalScrollMode() == QAbstractItemView::ScrollPerItem) {
            int oldOffset = d->verticalHeader->offset();
            if (verticalScrollBar()->value() == verticalScrollBar()->maximum())
                d->verticalHeader->setOffsetToLastSection();
            else
                d->verticalHeader->setOffsetToSectionPosition(verticalScrollBar()->value());
            int newOffset = d->verticalHeader->offset();
            dy = oldOffset - newOffset;
        } else {
            d->verticalHeader->setOffset(verticalScrollBar()->value());
        }
    }
    d->scrollContentsBy(dx, dy);

    if (d->showGrid) {
        //we need to update the first line of the previous top item in the view
        //because it has the grid drawn if the header is invisible.
        //It is strictly related to what's done at then end of the paintEvent
        if (dy > 0 && d->horizontalHeader->isHidden() && d->verticalScrollMode == ScrollPerItem) {
            d->viewport->update(0, dy, d->viewport->width(), dy);
        }
        if (dx > 0 && d->verticalHeader->isHidden() && d->horizontalScrollMode == ScrollPerItem) {
            d->viewport->update(dx, 0, dx, d->viewport->height());
        }
    }
}

/*!
  \reimp
*/
QStyleOptionViewItem QTableView::viewOptions() const
{
    QStyleOptionViewItem option = QAbstractItemView::viewOptions();
    option.showDecorationSelected = true;
    return option;
}

/*!
    Paints the table on receipt of the given paint event \a event.
*/
void QTableView::paintEvent(QPaintEvent *event)
{
    Q_D(QTableView);
    // setup temp variables for the painting
    QStyleOptionViewItemV4 option = d->viewOptionsV4();
    const QPoint offset = d->scrollDelayOffset;
    const bool showGrid = d->showGrid;
    const int gridSize = showGrid ? 1 : 0;
    const int gridHint = style()->styleHint(QStyle::SH_Table_GridLineColor, &option, this);
    const QColor gridColor = static_cast<QRgb>(gridHint);
    const QPen gridPen = QPen(gridColor, 0, d->gridStyle);
    const QHeaderView *verticalHeader = d->verticalHeader;
    const QHeaderView *horizontalHeader = d->horizontalHeader;
    const QStyle::State state = option.state;
    const bool alternate = d->alternatingColors;
    const bool rightToLeft = isRightToLeft();

    QPainter painter(d->viewport);

    // if there's nothing to do, clear the area and return
    if (horizontalHeader->count() == 0 || verticalHeader->count() == 0 || !d->itemDelegate)
        return;

    uint x = horizontalHeader->length() - horizontalHeader->offset() - (rightToLeft ? 0 : 1);
    uint y = verticalHeader->length() - verticalHeader->offset() - 1;

    QVector<QRect> rects = event->region().rects();
    int firstVisualRow = qMax(verticalHeader->visualIndexAt(0),0);
    int lastVisualRow = verticalHeader->visualIndexAt(verticalHeader->viewport()->height());
    if (lastVisualRow == -1)
        lastVisualRow = d->model->rowCount(d->root);

    int firstVisualColumn = horizontalHeader->visualIndexAt(0);
    int lastVisualColumn = horizontalHeader->visualIndexAt(horizontalHeader->viewport()->width());
    if (rightToLeft)
        qSwap(firstVisualColumn, lastVisualColumn);
    if (firstVisualColumn == -1)
        firstVisualColumn = 0;
    if (lastVisualColumn == -1)
        lastVisualColumn = horizontalHeader->count() - 1;

    QBitArray drawn((lastVisualRow - firstVisualRow + 1) * (lastVisualColumn - firstVisualColumn + 1));

    for (int i = 0; i < rects.size(); ++i) {
        QRect dirtyArea = rects.at(i);
        dirtyArea.translate(offset);
        dirtyArea.setBottom(qMin(dirtyArea.bottom(), int(y)));
        if (rightToLeft) {
            dirtyArea.setLeft(qMax(dirtyArea.left(), d->viewport->width() - int(x)));
        } else {
            dirtyArea.setRight(qMin(dirtyArea.right(), int(x)));
        }

        if (d->hasSpans())
            d->drawAndClipSpans(dirtyArea, &painter, option, &drawn);

        // get the horizontal start and end visual sections
        int left = horizontalHeader->visualIndexAt(dirtyArea.left());
        int right = horizontalHeader->visualIndexAt(dirtyArea.right());
        if (rightToLeft)
            qSwap(left, right);
        if (left == -1) left = 0;
        if (right == -1) right = horizontalHeader->count() - 1;

        // get the vertical start and end visual sections and if alternate color
        int bottom = verticalHeader->visualIndexAt(dirtyArea.bottom());
        if (bottom == -1) bottom = verticalHeader->count() - 1;
        int top = 0;
        bool alternateBase = false;
        if (alternate && verticalHeader->sectionsHidden()) {
            uint verticalOffset = verticalHeader->offset();
            int row = verticalHeader->logicalIndex(top);
            for (int y = 0;
                 ((uint)(y += verticalHeader->sectionSize(top)) <= verticalOffset) && (top < bottom);
                 ++top) {
                row = verticalHeader->logicalIndex(top);
                if (alternate && !verticalHeader->isSectionHidden(row))
                    alternateBase = !alternateBase;
            }
        } else {
            top = verticalHeader->visualIndexAt(dirtyArea.top());
            alternateBase = (top & 1) && alternate;
        }
        if (top == -1 || top > bottom)
            continue;

        // Paint each row item
        for (int visualRowIndex = top; visualRowIndex <= bottom; ++visualRowIndex) {
            int row = verticalHeader->logicalIndex(visualRowIndex);
            if (verticalHeader->isSectionHidden(row))
                continue;
            int rowY = rowViewportPosition(row);
            rowY += offset.y();
            int rowh = rowHeight(row) - gridSize;

            // Paint each column item
            for (int visualColumnIndex = left; visualColumnIndex <= right; ++visualColumnIndex) {
                int currentBit = (visualRowIndex - firstVisualRow) * (lastVisualColumn - firstVisualColumn + 1)
                        + visualColumnIndex - firstVisualColumn;

                if (currentBit < 0 || currentBit >= drawn.size() || drawn.testBit(currentBit))
                    continue;
                drawn.setBit(currentBit);

                int col = horizontalHeader->logicalIndex(visualColumnIndex);
                if (horizontalHeader->isSectionHidden(col))
                    continue;
                int colp = columnViewportPosition(col);
                colp += offset.x();
                int colw = columnWidth(col) - gridSize;

                const QModelIndex index = d->model->index(row, col, d->root);
                if (index.isValid()) {
                    option.rect = QRect(colp + (showGrid && rightToLeft ? 1 : 0), rowY, colw, rowh);
                    if (alternate) {
                        if (alternateBase)
                            option.features |= QStyleOptionViewItemV2::Alternate;
                        else
                            option.features &= ~QStyleOptionViewItemV2::Alternate;
                    }
                    d->drawCell(&painter, option, index);
                }
            }
            alternateBase = !alternateBase && alternate;
        }

        if (showGrid) {
            // Find the bottom right (the last rows/coloumns might be hidden)
            while (verticalHeader->isSectionHidden(verticalHeader->logicalIndex(bottom))) --bottom;
            QPen old = painter.pen();
            painter.setPen(gridPen);
            // Paint each row
            for (int visualIndex = top; visualIndex <= bottom; ++visualIndex) {
                int row = verticalHeader->logicalIndex(visualIndex);
                if (verticalHeader->isSectionHidden(row))
                    continue;
                int rowY = rowViewportPosition(row);
                rowY += offset.y();
                int rowh = rowHeight(row) - gridSize;
                painter.drawLine(dirtyArea.left(), rowY + rowh, dirtyArea.right(), rowY + rowh);
            }

            // Paint each column
            for (int h = left; h <= right; ++h) {
                int col = horizontalHeader->logicalIndex(h);
                if (horizontalHeader->isSectionHidden(col))
                    continue;
                int colp = columnViewportPosition(col);
                colp += offset.x();
                if (!rightToLeft)
                    colp +=  columnWidth(col) - gridSize;
                painter.drawLine(colp, dirtyArea.top(), colp, dirtyArea.bottom());
            }

            //draw the top & left grid lines if the headers are not visible.
            //We do update this line when subsequent scroll happen (see scrollContentsBy)
            if (horizontalHeader->isHidden() && verticalScrollMode() == ScrollPerItem)
                painter.drawLine(dirtyArea.left(), 0, dirtyArea.right(), 0);
            if (verticalHeader->isHidden() && horizontalScrollMode() == ScrollPerItem)
                painter.drawLine(0, dirtyArea.top(), 0, dirtyArea.bottom());
            painter.setPen(old);
        }
    }

#ifndef QT_NO_DRAGANDDROP
    // Paint the dropIndicator
    d->paintDropIndicator(&painter);
#endif
}

/*!
    Returns the index position of the model item corresponding to the
    table item at position \a pos in contents coordinates.
*/
QModelIndex QTableView::indexAt(const QPoint &pos) const
{
    Q_D(const QTableView);
    d->executePostedLayout();
    int r = rowAt(pos.y());
    int c = columnAt(pos.x());
    if (r >= 0 && c >= 0) {
        if (d->hasSpans()) {
            QTableViewPrivate::Span span = d->span(r, c);
            r = span.top();
            c = span.left();
        }
        return d->model->index(r, c, d->root);
    }
    return QModelIndex();
}

/*!
    Returns the horizontal offset of the items in the table view.

    Note that the table view uses the horizontal header section
    positions to determine the positions of columns in the view.

    \sa verticalOffset()
*/
int QTableView::horizontalOffset() const
{
    Q_D(const QTableView);
    return d->horizontalHeader->offset();
}

/*!
    Returns the vertical offset of the items in the table view.

    Note that the table view uses the vertical header section
    positions to determine the positions of rows in the view.

    \sa horizontalOffset()
*/
int QTableView::verticalOffset() const
{
    Q_D(const QTableView);
    return d->verticalHeader->offset();
}

/*!
    \fn QModelIndex QTableView::moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers)

    Moves the cursor in accordance with the given \a cursorAction, using the
    information provided by the \a modifiers.

    \sa QAbstractItemView::CursorAction
*/
QModelIndex QTableView::moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers)
{
    Q_D(QTableView);
    Q_UNUSED(modifiers);

    int bottom = d->model->rowCount(d->root) - 1;
    // make sure that bottom is the bottommost *visible* row
    while (bottom >= 0 && isRowHidden(d->logicalRow(bottom)))
        --bottom;

    int right = d->model->columnCount(d->root) - 1;

    while (right >= 0 && isColumnHidden(d->logicalColumn(right)))
        --right;

    if (bottom == -1 || right == -1)
        return QModelIndex(); // model is empty

    QModelIndex current = currentIndex();

    if (!current.isValid()) {
        int row = 0;
        int column = 0;
        while (column < right && isColumnHidden(d->logicalColumn(column)))
            ++column;
        while (isRowHidden(d->logicalRow(row)) && row < bottom)
            ++row;
        return d->model->index(d->logicalRow(row), d->logicalColumn(column), d->root);
    }

    int visualRow = d->visualRow(current.row());
    Q_ASSERT(visualRow != -1);
    int visualColumn = d->visualColumn(current.column());
    Q_ASSERT(visualColumn != -1);

    if (isRightToLeft()) {
        if (cursorAction == MoveLeft)
            cursorAction = MoveRight;
        else if (cursorAction == MoveRight)
            cursorAction = MoveLeft;
    }

    switch (cursorAction) {
    case MoveUp:
#ifdef QT_KEYPAD_NAVIGATION
        if (QApplication::keypadNavigationEnabled() && visualRow == 0)
            visualRow = d->visualRow(model()->rowCount() - 1) + 1;
#endif
        --visualRow;
        while (visualRow > 0 && d->isVisualRowHiddenOrDisabled(visualRow, visualColumn))
            --visualRow;
        if (d->hasSpans()) {
            int row = d->logicalRow(visualRow);
            QTableViewPrivate::Span span = d->span(row, current.column());
            visualRow = d->visualRow(span.top());
            visualColumn = d->visualColumn(span.left());
        }
        break;
    case MoveDown:
        if (d->hasSpans()) {
            QTableViewPrivate::Span span = d->span(current.row(), current.column());
            visualRow = d->visualRow(d->rowSpanEndLogical(span.top(), span.height()));
        }
#ifdef QT_KEYPAD_NAVIGATION
        if (QApplication::keypadNavigationEnabled() && visualRow >= bottom)
            visualRow = -1;
#endif
        ++visualRow;
        while (visualRow < bottom && d->isVisualRowHiddenOrDisabled(visualRow, visualColumn))
            ++visualRow;
        if (d->hasSpans()) {
            int row = d->logicalRow(visualRow);
            QTableViewPrivate::Span span = d->span(row, current.column());
            visualColumn = d->visualColumn(span.left());
        }
        break;
    case MovePrevious: {
        int left = 0;
        while (d->isVisualColumnHiddenOrDisabled(visualRow, left) && left < right)
            ++left;
        if (visualColumn == left) {
            visualColumn = right;
            int top = 0;
            while (top < bottom && d->isVisualRowHiddenOrDisabled(top, visualColumn))
                ++top;
            if (visualRow == top)
                visualRow = bottom;
            else
                --visualRow;
            while (visualRow > 0 && d->isVisualRowHiddenOrDisabled(visualRow, visualColumn))
                --visualRow;
            break;
        } // else MoveLeft
    }
    case MoveLeft:
        --visualColumn;
        while (visualColumn > 0 && d->isVisualColumnHiddenOrDisabled(visualRow, visualColumn))
            --visualColumn;
        if (d->hasSpans()) {
            int column = d->logicalColumn(visualColumn);
            QTableViewPrivate::Span span = d->span(current.row(), column);
            visualRow = d->visualRow(span.top());
            visualColumn = d->visualColumn(span.left());
        }
        break;
    case MoveNext:
        if (visualColumn == right) {
            visualColumn = 0;
            while (visualColumn < right && d->isVisualColumnHiddenOrDisabled(visualRow, visualColumn))
                ++visualColumn;
            if (visualRow == bottom)
                visualRow = 0;
            else
                ++visualRow;
            while (visualRow < bottom && d->isVisualRowHiddenOrDisabled(visualRow, visualColumn))
                ++visualRow;
            break;
        } // else MoveRight
    case MoveRight:
        if (d->hasSpans()) {
            QTableViewPrivate::Span span = d->span(current.row(), current.column());
            visualColumn = d->visualColumn(d->columnSpanEndLogical(span.left(), span.width()));
        }
        ++visualColumn;
        while (visualColumn < right && d->isVisualColumnHiddenOrDisabled(visualRow, visualColumn))
            ++visualColumn;
        if (d->hasSpans()) {
            int column = d->logicalColumn(visualColumn);
            QTableViewPrivate::Span span = d->span(current.row(), column);
            visualRow = d->visualRow(span.top());
        }
        break;
    case MoveHome:
        visualColumn = 0;
        while (visualColumn < right && d->isVisualColumnHiddenOrDisabled(visualRow, visualColumn))
            ++visualColumn;
        if (modifiers & Qt::ControlModifier) {
            visualRow = 0;
            while (visualRow < bottom && d->isVisualRowHiddenOrDisabled(visualRow, visualColumn))
                ++visualRow;
        }
        break;
    case MoveEnd:
        visualColumn = right;
        if (modifiers & Qt::ControlModifier)
            visualRow = bottom;
        break;
    case MovePageUp: {
        int top = 0;
        while (top < bottom && d->isVisualRowHiddenOrDisabled(top, visualColumn))
            ++top;
        int newRow = qMax(rowAt(visualRect(current).top() - d->viewport->height()), top);
        return d->model->index(qBound(0, newRow, bottom), current.column(), d->root);
    }
    case MovePageDown: {
        int newRow = qMin(rowAt(visualRect(current).bottom() + d->viewport->height()), bottom);
        if (newRow < 0)
            newRow = bottom;
        return d->model->index(qBound(0, newRow, bottom), current.column(), d->root);
    }}

    int logicalRow = d->logicalRow(visualRow);
    int logicalColumn = d->logicalColumn(visualColumn);
    if (!d->model->hasIndex(logicalRow, logicalColumn, d->root))
        return QModelIndex();

    QModelIndex result = d->model->index(logicalRow, logicalColumn, d->root);
    if (!isIndexHidden(result) && d->isIndexEnabled(result))
        return d->model->index(logicalRow, logicalColumn, d->root);

    return QModelIndex();
}

/*!
    \fn void QTableView::setSelection(const QRect &rect,
    QItemSelectionModel::SelectionFlags flags)

    Selects the items within the given \a rect and in accordance with
    the specified selection \a flags.
*/
void QTableView::setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags command)
{
    Q_D(QTableView);
    QModelIndex tl = indexAt(QPoint(isRightToLeft() ? qMax(rect.left(), rect.right())
                                    : qMin(rect.left(), rect.right()), qMin(rect.top(), rect.bottom())));
    QModelIndex br = indexAt(QPoint(isRightToLeft() ? qMin(rect.left(), rect.right()) :
                                    qMax(rect.left(), rect.right()), qMax(rect.top(), rect.bottom())));
    if (!d->selectionModel || !tl.isValid() || !br.isValid() || !d->isIndexEnabled(tl) || !d->isIndexEnabled(br))
        return;

    bool verticalMoved = verticalHeader()->sectionsMoved();
    bool horizontalMoved = horizontalHeader()->sectionsMoved();

    QItemSelection selection;

    if (d->hasSpans()) {
        bool expanded;
        int top = qMin(d->visualRow(tl.row()), d->visualRow(br.row()));
        int left = qMin(d->visualColumn(tl.column()), d->visualColumn(br.column()));
        int bottom = qMax(d->visualRow(tl.row()), d->visualRow(br.row()));
        int right = qMax(d->visualColumn(tl.column()), d->visualColumn(br.column()));
        do {
            expanded = false;
            QList<QTableViewPrivate::Span>::const_iterator it;
            for (it = d->spans.constBegin(); it != d->spans.constEnd(); ++it) {
                QTableViewPrivate::Span span = *it;
                int t = d->visualRow(span.top());
                int l = d->visualColumn(span.left());
                int b = d->visualRow(d->rowSpanEndLogical(span.top(), span.height()));
                int r = d->visualColumn(d->columnSpanEndLogical(span.left(), span.width()));
                if ((t > bottom) || (l > right) || (top > b) || (left > r))
                    continue; // no intersect
                if (t < top) {
                    top = t;
                    expanded = true;
                }
                if (l < left) {
                    left = l;
                    expanded = true;
                }
                if (b > bottom) {
                    bottom = b;
                    expanded = true;
                }
                if (r > right) {
                    right = r;
                    expanded = true;
                }
                if (expanded)
                    break;
            }
        } while (expanded);
         for (int horizontal = left; horizontal <= right; ++horizontal) {
             int column = d->logicalColumn(horizontal);
             for (int vertical = top; vertical <= bottom; ++vertical) {
                 int row = d->logicalRow(vertical);
                 QModelIndex index = d->model->index(row, column, d->root);
                 selection.append(QItemSelectionRange(index));
             }
         }
    } else if (verticalMoved && horizontalMoved) {
         int top = d->visualRow(tl.row());
         int left = d->visualColumn(tl.column());
         int bottom = d->visualRow(br.row());
         int right = d->visualColumn(br.column());
         for (int horizontal = left; horizontal <= right; ++horizontal) {
             int column = d->logicalColumn(horizontal);
             for (int vertical = top; vertical <= bottom; ++vertical) {
                 int row = d->logicalRow(vertical);
                 QModelIndex index = d->model->index(row, column, d->root);
                 selection.append(QItemSelectionRange(index));
             }
         }
    } else if (horizontalMoved) {
        int left = d->visualColumn(tl.column());
        int right = d->visualColumn(br.column());
        for (int visual = left; visual <= right; ++visual) {
            int column = d->logicalColumn(visual);
            QModelIndex topLeft = d->model->index(tl.row(), column, d->root);
            QModelIndex bottomRight = d->model->index(br.row(), column, d->root);
            selection.append(QItemSelectionRange(topLeft, bottomRight));
        }
    } else if (verticalMoved) {
        int top = d->visualRow(tl.row());
        int bottom = d->visualRow(br.row());
        for (int visual = top; visual <= bottom; ++visual) {
            int row = d->logicalRow(visual);
            QModelIndex topLeft = d->model->index(row, tl.column(), d->root);
            QModelIndex bottomRight = d->model->index(row, br.column(), d->root);
            selection.append(QItemSelectionRange(topLeft, bottomRight));
        }
    } else { // nothing moved
        selection.append(QItemSelectionRange(tl, br));
    }

    d->selectionModel->select(selection, command);
}

/*!
    \internal

    Returns the rectangle from the viewport of the items in the given
    \a selection.
*/
QRegion QTableView::visualRegionForSelection(const QItemSelection &selection) const
{
    Q_D(const QTableView);

    if (selection.isEmpty())
        return QRegion();

    QRegion selectionRegion;
    bool verticalMoved = verticalHeader()->sectionsMoved();
    bool horizontalMoved = horizontalHeader()->sectionsMoved();

    if ((verticalMoved && horizontalMoved) || d->hasSpans()) {
        for (int i = 0; i < selection.count(); ++i) {
            QItemSelectionRange range = selection.at(i);
            if (range.parent() != d->root || !range.isValid())
                continue;
            for (int r = range.top(); r <= range.bottom(); ++r)
                for (int c = range.left(); c <= range.right(); ++c)
                    selectionRegion += QRegion(visualRect(d->model->index(r, c, d->root)));
        }
    } else if (horizontalMoved) {
        for (int i = 0; i < selection.count(); ++i) {
            QItemSelectionRange range = selection.at(i);
            if (range.parent() != d->root || !range.isValid())
                continue;
            int top = rowViewportPosition(range.top());
            int bottom = rowViewportPosition(range.bottom()) + rowHeight(range.bottom());
            if (top > bottom)
                qSwap<int>(top, bottom);
            int height = bottom - top;
            for (int c = range.left(); c <= range.right(); ++c)
                selectionRegion += QRegion(QRect(columnViewportPosition(c), top,
                                                 columnWidth(c), height));
        }
    } else if (verticalMoved) {
        for (int i = 0; i < selection.count(); ++i) {
            QItemSelectionRange range = selection.at(i);
            if (range.parent() != d->root || !range.isValid())
                continue;
            int left = columnViewportPosition(range.left());
            int right = columnViewportPosition(range.right()) + columnWidth(range.right());
            if (left > right)
                qSwap<int>(left, right);
            int width = right - left;
            for (int r = range.top(); r <= range.bottom(); ++r)
                selectionRegion += QRegion(QRect(left, rowViewportPosition(r),
                                                 width, rowHeight(r)));
        }
    } else { // nothing moved
        for (int i = 0; i < selection.count(); ++i) {
            QItemSelectionRange range = selection.at(i);
            if (range.parent() != d->root || !range.isValid())
                continue;
            d->trimHiddenSelections(&range);
            QRect tl = visualRect(range.topLeft());
            QRect br = visualRect(range.bottomRight());
            selectionRegion += QRegion(tl|br);
        }
    }

    return selectionRegion;
}


/*!
  \reimp
*/
QModelIndexList QTableView::selectedIndexes() const
{
    Q_D(const QTableView);
    QModelIndexList viewSelected;
    QModelIndexList modelSelected;
    if (d->selectionModel)
        modelSelected = d->selectionModel->selectedIndexes();
    for (int i = 0; i < modelSelected.count(); ++i) {
        QModelIndex index = modelSelected.at(i);
        if (!isIndexHidden(index) && index.parent() == d->root)
            viewSelected.append(index);
    }
    return viewSelected;
}


/*!
    This slot is called whenever rows are added or deleted. The
    previous number of rows is specified by \a oldCount, and the new
    number of rows is specified by \a newCount.
*/
void QTableView::rowCountChanged(int /*oldCount*/, int /*newCount*/ )
{
    Q_D(QTableView);
    updateGeometries();
    if (verticalScrollMode() == QAbstractItemView::ScrollPerItem)
        d->verticalHeader->setOffsetToSectionPosition(verticalScrollBar()->value());
    else
        d->verticalHeader->setOffset(verticalScrollBar()->value());
    d->viewport->update();
}

/*!
    This slot is called whenever columns are added or deleted. The
    previous number of columns is specified by \a oldCount, and the new
    number of columns is specified by \a newCount.
*/
void QTableView::columnCountChanged(int, int)
{
    Q_D(QTableView);
    updateGeometries();
    if (horizontalScrollMode() == QAbstractItemView::ScrollPerItem)
        d->horizontalHeader->setOffsetToSectionPosition(horizontalScrollBar()->value());
    else
        d->horizontalHeader->setOffset(horizontalScrollBar()->value());
    d->viewport->update();
}

/*!
    \reimp
*/
void QTableView::updateGeometries()
{
    Q_D(QTableView);
    if (d->geometryRecursionBlock)
        return;
    d->geometryRecursionBlock = true;

    int width = 0;
    if (!d->verticalHeader->isHidden()) {
        width = qMax(d->verticalHeader->minimumWidth(), d->verticalHeader->sizeHint().width());
        width = qMin(width, d->verticalHeader->maximumWidth());
    }
    int height = 0;
    if (!d->horizontalHeader->isHidden()) {
        height = qMax(d->horizontalHeader->minimumHeight(), d->horizontalHeader->sizeHint().height());
        height = qMin(height, d->horizontalHeader->maximumHeight());
    }
    bool reverse = isRightToLeft();
     if (reverse)
         setViewportMargins(0, height, width, 0);
     else
         setViewportMargins(width, height, 0, 0);

    // update headers

    QRect vg = d->viewport->geometry();

    int verticalLeft = reverse ? vg.right() + 1 : (vg.left() - width);
    d->verticalHeader->setGeometry(verticalLeft, vg.top(), width, vg.height());
    if (d->verticalHeader->isHidden())
        QMetaObject::invokeMethod(d->verticalHeader, "updateGeometries");

    int horizontalTop = vg.top() - height;
    d->horizontalHeader->setGeometry(vg.left(), horizontalTop, vg.width(), height);
    if (d->horizontalHeader->isHidden())
        QMetaObject::invokeMethod(d->horizontalHeader, "updateGeometries");

    // update cornerWidget
    if (d->horizontalHeader->isHidden() || d->verticalHeader->isHidden()) {
        d->cornerWidget->setHidden(true);
    } else {
        d->cornerWidget->setHidden(false);
        d->cornerWidget->setGeometry(verticalLeft, horizontalTop, width, height);
    }

    // update scroll bars

    // ### move this block into the if
    QSize vsize = d->viewport->size();
    QSize max = maximumViewportSize();
    uint horizontalLength = d->horizontalHeader->length();
    uint verticalLength = d->verticalHeader->length();
    if ((uint)max.width() >= horizontalLength && (uint)max.height() >= verticalLength)
        vsize = max;

    // horizontal scroll bar
    const int columnCount = d->horizontalHeader->count();
    const int viewportWidth = vsize.width();
    int columnsInViewport = 0;
    for (int width = 0, column = columnCount - 1; column >= 0; --column) {
        int logical = d->horizontalHeader->logicalIndex(column);
        if (!d->horizontalHeader->isSectionHidden(logical)) {
            width += d->horizontalHeader->sectionSize(logical);
            if (width > viewportWidth)
                break;
            ++columnsInViewport;
        }
    }
    if (horizontalScrollMode() == QAbstractItemView::ScrollPerItem) {
        const int visibleColumns = columnCount - d->horizontalHeader->hiddenSectionCount();
        horizontalScrollBar()->setRange(0, visibleColumns - columnsInViewport);
        horizontalScrollBar()->setPageStep(columnsInViewport);
        if (columnsInViewport >= visibleColumns)
            d->horizontalHeader->setOffset(0);
        horizontalScrollBar()->setSingleStep(1);
    } else { // ScrollPerPixel
        horizontalScrollBar()->setPageStep(vsize.width());
        horizontalScrollBar()->setRange(0, horizontalLength - vsize.width());
        horizontalScrollBar()->setSingleStep(qMax(vsize.width() / (columnsInViewport + 1), 2));
    }

    // vertical scroll bar
    const int rowCount = d->verticalHeader->count();
    const int viewportHeight = vsize.height();
    int rowsInViewport = 0;
    for (int height = 0, row = rowCount - 1; row >= 0; --row) {
        int logical = d->verticalHeader->logicalIndex(row);
        if (!d->verticalHeader->isSectionHidden(logical)) {
            height += d->verticalHeader->sectionSize(logical);
            if (height > viewportHeight)
                break;
            ++rowsInViewport;
        }
    }
    if (verticalScrollMode() == QAbstractItemView::ScrollPerItem) {
        const int visibleRows = rowCount - d->verticalHeader->hiddenSectionCount();
        verticalScrollBar()->setRange(0, visibleRows - rowsInViewport);
        verticalScrollBar()->setPageStep(rowsInViewport);
        if (rowsInViewport >= visibleRows)
            d->verticalHeader->setOffset(0);
        verticalScrollBar()->setSingleStep(1);
    } else { // ScrollPerPixel
        verticalScrollBar()->setPageStep(vsize.height());
        verticalScrollBar()->setRange(0, verticalLength - vsize.height());
        verticalScrollBar()->setSingleStep(qMax(vsize.height() / (rowsInViewport + 1), 2));
    }

    d->geometryRecursionBlock = false;
    QAbstractItemView::updateGeometries();
}

/*!
    Returns the size hint for the given \a row's height or -1 if there
    is no model.

    If you need to set the height of a given row to a fixed value, call
    QHeaderView::resizeSection() on the table's vertical header.

    If you reimplement this function in a subclass, note that the value you
    return is only used when resizeRowToContents() is called. In that case,
    if a larger row height is required by either the vertical header or
    the item delegate, that width will be used instead.

    \sa QWidget::sizeHint, verticalHeader()
*/
int QTableView::sizeHintForRow(int row) const
{
    Q_D(const QTableView);

    if (!model())
        return -1;

    int left = qMax(0, columnAt(0));
    int right = columnAt(d->viewport->width());
    if (right == -1) // the table don't have enough columns to fill the viewport
        right = d->model->columnCount(d->root) - 1;

    QStyleOptionViewItemV4 option = d->viewOptionsV4();

    int hint = 0;
    QModelIndex index;
    for (int column = left; column <= right; ++column) {
        int logicalColumn = d->horizontalHeader->logicalIndex(column);
        if (d->horizontalHeader->isSectionHidden(logicalColumn))
            continue;
        index = d->model->index(row, logicalColumn, d->root);
        if (d->wrapItemText) {// for wrapping boundaries
            option.rect.setY(rowViewportPosition(index.row()));
            option.rect.setHeight(rowHeight(index.row()));
            option.rect.setX(columnViewportPosition(index.column()));
            option.rect.setWidth(columnWidth(index.column()));
        }
        
        QWidget *editor = d->editorForIndex(index).editor;
        if (editor && d->persistent.contains(editor)) {
            hint = qMax(hint, editor->sizeHint().height());
            int min = editor->minimumSize().height();
            int max = editor->maximumSize().height();
            hint = qBound(min, hint, max);
        }
        
        hint = qMax(hint, itemDelegate(index)->sizeHint(option, index).height());
    }

    return d->showGrid ? hint + 1 : hint;
}

/*!
    Returns the size hint for the given \a column's width or -1 if
    there is no model.

    If you need to set the width of a given column to a fixed value, call
    QHeaderView::resizeSection() on the table's horizontal header.

    If you reimplement this function in a subclass, note that the value you
    return will be used when resizeColumnToContents() or
    QHeaderView::resizeSections() is called. If a larger column width is
    required by either the horizontal header or the item delegate, the larger
    width will be used instead.

    \sa QWidget::sizeHint, horizontalHeader()
*/
int QTableView::sizeHintForColumn(int column) const
{
    Q_D(const QTableView);

    if (!model())
        return -1;

    int top = qMax(0, rowAt(0));
    int bottom = rowAt(d->viewport->height());
    if (!isVisible() || bottom == -1) // the table don't have enough rows to fill the viewport
        bottom = d->model->rowCount(d->root) - 1;

    QStyleOptionViewItemV4 option = d->viewOptionsV4();

    int hint = 0;
    QModelIndex index;
    for (int row = top; row <= bottom; ++row) {
        int logicalRow = d->verticalHeader->logicalIndex(row);
        if (d->verticalHeader->isSectionHidden(logicalRow))
            continue;
        index = d->model->index(logicalRow, column, d->root);
        
        QWidget *editor = d->editorForIndex(index).editor;
        if (editor && d->persistent.contains(editor)) {
            hint = qMax(hint, editor->sizeHint().width());
            int min = editor->minimumSize().width();
            int max = editor->maximumSize().width();
            hint = qBound(min, hint, max);
        }
        
        hint = qMax(hint, itemDelegate(index)->sizeHint(option, index).width());
    }

    return d->showGrid ? hint + 1 : hint;
}

/*!
    Returns the y-coordinate in contents coordinates of the given \a
    row.
*/
int QTableView::rowViewportPosition(int row) const
{
    Q_D(const QTableView);
    return d->verticalHeader->sectionViewportPosition(row);
}

/*!
    Returns the row in which the given y-coordinate, \a y, in contents
    coordinates is located.

    \note This function returns -1 if the given coordinate is not valid
    (has no row).

    \sa columnAt()
*/
int QTableView::rowAt(int y) const
{
    Q_D(const QTableView);
    return d->verticalHeader->logicalIndexAt(y);
}

/*!
    \since 4.1

    Sets the height of the given \a row to be \a height.
*/
void QTableView::setRowHeight(int row, int height)
{
    Q_D(const QTableView);
    d->verticalHeader->resizeSection(row, height);
}

/*!
    Returns the height of the given \a row.

    \sa resizeRowToContents(), columnWidth()
*/
int QTableView::rowHeight(int row) const
{
    Q_D(const QTableView);
    return d->verticalHeader->sectionSize(row);
}

/*!
    Returns the x-coordinate in contents coordinates of the given \a
    column.
*/
int QTableView::columnViewportPosition(int column) const
{
    Q_D(const QTableView);
    return d->horizontalHeader->sectionViewportPosition(column);
}

/*!
    Returns the column in which the given x-coordinate, \a x, in contents
    coordinates is located.

    \note This function returns -1 if the given coordinate is not valid
    (has no column).

    \sa rowAt()
*/
int QTableView::columnAt(int x) const
{
    Q_D(const QTableView);
    return d->horizontalHeader->logicalIndexAt(x);
}

/*!
    \since 4.1

    Sets the width of the given \a column to be \a width.
*/
void QTableView::setColumnWidth(int column, int width)
{
    Q_D(const QTableView);
    d->horizontalHeader->resizeSection(column, width);
}

/*!
    Returns the width of the given \a column.

    \sa resizeColumnToContents(), rowHeight()
*/
int QTableView::columnWidth(int column) const
{
    Q_D(const QTableView);
    return d->horizontalHeader->sectionSize(column);
}

/*!
    Returns true if the given \a row is hidden; otherwise returns false.

    \sa isColumnHidden()
*/
bool QTableView::isRowHidden(int row) const
{
    Q_D(const QTableView);
    return d->verticalHeader->isSectionHidden(row);
}

/*!
    If \a hide is true \a row will be hidden, otherwise it will be shown.

    \sa setColumnHidden()
*/
void QTableView::setRowHidden(int row, bool hide)
{
    Q_D(QTableView);
    if (row < 0 || row >= d->verticalHeader->count())
        return;
    d->verticalHeader->setSectionHidden(row, hide);
}

/*!
    Returns true if the given \a column is hidden; otherwise returns false.

    \sa isRowHidden()
*/
bool QTableView::isColumnHidden(int column) const
{
    Q_D(const QTableView);
    return d->horizontalHeader->isSectionHidden(column);
}

/*!
  If \a hide is true the given \a column will be hidden; otherwise it
  will be shown.

  \sa setRowHidden()
*/
void QTableView::setColumnHidden(int column, bool hide)
{
    Q_D(QTableView);
    if (column < 0 || column >= d->horizontalHeader->count())
        return;
    d->horizontalHeader->setSectionHidden(column, hide);
}

/*!
    \since 4.2
    \property QTableView::sortingEnabled
    \brief whether sorting is enabled

    If this property is true, sorting is enabled for the table; if the
    property is false, sorting is not enabled. The default value is false.

    \sa sortByColumn()
*/

void QTableView::setSortingEnabled(bool enable)
{
    Q_D(QTableView);
    d->sortingEnabled = enable;
    horizontalHeader()->setSortIndicatorShown(enable);
    if (enable) {
        disconnect(d->horizontalHeader, SIGNAL(sectionEntered(int)),
                   this, SLOT(_q_selectColumn(int)));
        disconnect(horizontalHeader(), SIGNAL(sectionPressed(int)),
                   this, SLOT(selectColumn(int)));
        connect(horizontalHeader(), SIGNAL(sortIndicatorChanged(int,Qt::SortOrder)),
                this, SLOT(sortByColumn(int)));
        sortByColumn(horizontalHeader()->sortIndicatorSection(),
                     horizontalHeader()->sortIndicatorOrder());
    } else {
        connect(d->horizontalHeader, SIGNAL(sectionEntered(int)),
                this, SLOT(_q_selectColumn(int)));
        connect(horizontalHeader(), SIGNAL(sectionPressed(int)),
                this, SLOT(selectColumn(int)));
        disconnect(horizontalHeader(), SIGNAL(sortIndicatorChanged(int,Qt::SortOrder)),
                   this, SLOT(sortByColumn(int)));
    }
}

bool QTableView::isSortingEnabled() const
{
    Q_D(const QTableView);
    return d->sortingEnabled;
}

/*!
    \property QTableView::showGrid
    \brief whether the grid is shown

    If this property is true a grid is drawn for the table; if the
    property is false, no grid is drawn. The default value is true.
*/
bool QTableView::showGrid() const
{
    Q_D(const QTableView);
    return d->showGrid;
}

void QTableView::setShowGrid(bool show)
{
    Q_D(QTableView);
    if (d->showGrid != show) {
        d->showGrid = show;
        d->viewport->update();
    }
}

/*!
  \property QTableView::gridStyle
  \brief  the pen style used to draw the grid.

  This property holds the style used when drawing the grid (see \l{showGrid}).
*/
Qt::PenStyle QTableView::gridStyle() const
{
    Q_D(const QTableView);
    return d->gridStyle;
}

void QTableView::setGridStyle(Qt::PenStyle style)
{
    Q_D(QTableView);
    if (d->gridStyle != style) {
        d->gridStyle = style;
        d->viewport->update();
    }
}

/*!
    \property QTableView::wordWrap
    \brief the item text word-wrapping policy
    \since 4.3

    If this property is true then the item text is wrapped where
    necessary at word-breaks; otherwise it is not wrapped at all.
    This property is true by default.

    Note that even of wrapping is enabled, the cell will not be
    expanded to fit all text. Ellipsis will be inserted according to
    the current \l{QAbstractItemView::}{textElideMode}.

*/
void QTableView::setWordWrap(bool on)
{
    Q_D(QTableView);
    if (d->wrapItemText == on)
        return;
    d->wrapItemText = on;
    QMetaObject::invokeMethod(d->verticalHeader, "resizeSections");
    QMetaObject::invokeMethod(d->horizontalHeader, "resizeSections");
}

bool QTableView::wordWrap() const
{
    Q_D(const QTableView);
    return d->wrapItemText;
}

/*!
    \property QTableView::cornerButtonEnabled
    \brief whether the button in the top-left corner is enabled
    \since 4.3

    If this property is true then button in the top-left corner
    of the table view is enabled. Clicking on this button will
    select all the cells in the table view.

    This property is true by default.
*/
void QTableView::setCornerButtonEnabled(bool enable)
{
    Q_D(QTableView);
    d->cornerWidget->setEnabled(enable);
}

bool QTableView::isCornerButtonEnabled() const
{
    Q_D(const QTableView);
    return d->cornerWidget->isEnabled();
}

/*!
    \internal

    Returns the rectangle on the viewport occupied by the given \a
    index.
    If the index is hidden in the view it will return a null QRect.
*/
QRect QTableView::visualRect(const QModelIndex &index) const
{
    Q_D(const QTableView);
    if (!d->isIndexValid(index) || index.parent() != d->root || isIndexHidden(index) )
        return QRect();

    d->executePostedLayout();

    if (d->hasSpans()) {
        QTableViewPrivate::Span span = d->span(index.row(), index.column());
        return d->visualSpanRect(span);
    }

    int rowp = rowViewportPosition(index.row());
    int rowh = rowHeight(index.row());
    int colp = columnViewportPosition(index.column());
    int colw = columnWidth(index.column());

    const int i = showGrid() ? 1 : 0;
    return QRect(colp, rowp, colw - i, rowh - i);
}

/*!
    \internal

    Makes sure that the given \a item is visible in the table view,
    scrolling if necessary.
*/
void QTableView::scrollTo(const QModelIndex &index, ScrollHint hint)
{
    Q_D(QTableView);

    // check if we really need to do anything
    if (!d->isIndexValid(index)
        || (d->model->parent(index) != d->root)
        || isIndexHidden(index))
        return;

    QTableViewPrivate::Span span;
    if (d->hasSpans())
        span = d->span(index.row(), index.column());

    // Adjust horizontal position

    int viewportWidth = d->viewport->width();
    int horizontalOffset = d->horizontalHeader->offset();
    int horizontalPosition = d->horizontalHeader->sectionPosition(index.column());
    int horizontalIndex = d->horizontalHeader->visualIndex(index.column());
    int cellWidth = d->hasSpans()
                    ? d->columnSpanWidth(index.column(), span.width())
                    : d->horizontalHeader->sectionSize(index.column());

    if (horizontalScrollMode() == QAbstractItemView::ScrollPerItem) {

        bool positionAtLeft = (horizontalPosition - horizontalOffset < 0);
        bool positionAtRight = (horizontalPosition - horizontalOffset + cellWidth > viewportWidth);

        if (hint == PositionAtCenter || positionAtRight) {
            int w = (hint == PositionAtCenter ? viewportWidth / 2 : viewportWidth);
            int x = cellWidth;
            while (horizontalIndex > 0) {
                x += columnWidth(d->horizontalHeader->logicalIndex(horizontalIndex-1));
                if (x > w)
                    break;
                --horizontalIndex;
            }
        }

        if (positionAtRight || hint == PositionAtCenter || positionAtLeft) {
            int hiddenSections = 0;
            if (d->horizontalHeader->sectionsHidden()) {
                for (int s = horizontalIndex; s >= 0; --s) {
                    int column = d->horizontalHeader->logicalIndex(s);
                    if (d->horizontalHeader->isSectionHidden(column))
                        ++hiddenSections;
                }
            }
            horizontalScrollBar()->setValue(horizontalIndex - hiddenSections);
        }

    } else { // ScrollPerPixel
        if (hint == PositionAtCenter) {
            horizontalScrollBar()->setValue(horizontalPosition - ((viewportWidth - cellWidth) / 2));
        } else {
            if (horizontalPosition - horizontalOffset < 0 || cellWidth > viewportWidth)
                horizontalScrollBar()->setValue(horizontalPosition);
            else if (horizontalPosition - horizontalOffset + cellWidth > viewportWidth)
                horizontalScrollBar()->setValue(horizontalPosition - viewportWidth + cellWidth);
        }
    }

    // Adjust vertical position

    int viewportHeight = d->viewport->height();
    int verticalOffset = d->verticalHeader->offset();
    int verticalPosition = d->verticalHeader->sectionPosition(index.row());
    int verticalIndex = d->verticalHeader->visualIndex(index.row());
    int cellHeight = d->hasSpans()
                     ? d->rowSpanHeight(index.row(), span.height())
                     : d->verticalHeader->sectionSize(index.row());

    if (verticalPosition - verticalOffset < 0 || cellHeight > viewportHeight) {
        if (hint == EnsureVisible)
            hint = PositionAtTop;
    } else if (verticalPosition - verticalOffset + cellHeight > viewportHeight) {
        if (hint == EnsureVisible)
            hint = PositionAtBottom;
    }

    if (verticalScrollMode() == QAbstractItemView::ScrollPerItem) {

        if (hint == PositionAtBottom || hint == PositionAtCenter) {
            int h = (hint == PositionAtCenter ? viewportHeight / 2 : viewportHeight);
            int y = cellHeight;
            while (verticalIndex > 0) {
                int row = d->verticalHeader->logicalIndex(verticalIndex - 1);
                y += d->verticalHeader->sectionSize(row);
                if (y > h)
                    break;
                --verticalIndex;
            }
        }

        if (hint == PositionAtBottom || hint == PositionAtCenter || hint == PositionAtTop) {
            int hiddenSections = 0;
            if (d->verticalHeader->sectionsHidden()) {
                for (int s = verticalIndex; s >= 0; --s) {
                    int row = d->verticalHeader->logicalIndex(s);
                    if (d->verticalHeader->isSectionHidden(row))
                        ++hiddenSections;
                }
            }
            verticalScrollBar()->setValue(verticalIndex - hiddenSections);
        }

    } else { // ScrollPerPixel
        if (hint == PositionAtTop) {
            verticalScrollBar()->setValue(verticalPosition);
        } else if (hint == PositionAtBottom) {
            verticalScrollBar()->setValue(verticalPosition - viewportHeight + cellHeight);
        } else if (hint == PositionAtCenter) {
            verticalScrollBar()->setValue(verticalPosition - ((viewportHeight - cellHeight) / 2));
        }
    }

    d->setDirtyRegion(visualRect(index));
}

/*!
    This slot is called to change the height of the given \a row. The
    old height is specified by \a oldHeight, and the new height by \a
    newHeight.

    \sa columnResized()
*/
void QTableView::rowResized(int row, int, int)
{
    Q_D(QTableView);
    d->rowsToUpdate.append(row);
    if (d->rowResizeTimerID == 0)
        d->rowResizeTimerID = startTimer(0);
}

/*!
    This slot is called to change the width of the given \a column.
    The old width is specified by \a oldWidth, and the new width by \a
    newWidth.

    \sa rowResized()
*/
void QTableView::columnResized(int column, int, int)
{
    Q_D(QTableView);
    d->columnsToUpdate.append(column);
    if (d->columnResizeTimerID == 0)
        d->columnResizeTimerID = startTimer(0);
}

/*!
 \reimp
 */
void QTableView::timerEvent(QTimerEvent *event)
{
    Q_D(QTableView);

    if (event->timerId() == d->columnResizeTimerID) {
        updateGeometries();
        killTimer(d->columnResizeTimerID);
        d->columnResizeTimerID = 0;

        QRect rect;
        int viewportHeight = d->viewport->height();
        int viewportWidth = d->viewport->width();
        if (d->hasSpans() && d->spansIntersectColumns(d->columnsToUpdate)) {
            rect = QRect(0, 0, viewportWidth, viewportHeight);
        } else {
            for (int i = d->columnsToUpdate.size()-1; i >= 0; --i) {
                int column = d->columnsToUpdate.at(i);
                int x = columnViewportPosition(column);
                if (isRightToLeft())
                    rect |= QRect(0, 0, x + columnWidth(column), viewportHeight);
                else
                    rect |= QRect(x, 0, viewportWidth - x, viewportHeight);
            }
        }

        d->viewport->update(rect.normalized());
        d->columnsToUpdate.clear();
    }

    if (event->timerId() == d->rowResizeTimerID) {
        updateGeometries();
        killTimer(d->rowResizeTimerID);
        d->rowResizeTimerID = 0;

        int viewportHeight = d->viewport->height();
        int viewportWidth = d->viewport->width();
        int top;
        if (d->hasSpans() && d->spansIntersectRows(d->rowsToUpdate)) {
            top = 0;
        } else {
            top = viewportHeight;
            for (int i = d->rowsToUpdate.size()-1; i >= 0; --i) {
                int y = rowViewportPosition(d->rowsToUpdate.at(i));
                top = qMin(top, y);
            }
        }

        d->viewport->update(QRect(0, top, viewportWidth, viewportHeight - top));
        d->rowsToUpdate.clear();
    }

    QAbstractItemView::timerEvent(event);
}

/*!
    This slot is called to change the index of the given \a row in the
    table view. The old index is specified by \a oldIndex, and the new
    index by \a newIndex.

    \sa columnMoved()
*/
void QTableView::rowMoved(int, int oldIndex, int newIndex)
{
    Q_D(QTableView);

    updateGeometries();
    int logicalOldIndex = d->verticalHeader->logicalIndex(oldIndex);
    int logicalNewIndex = d->verticalHeader->logicalIndex(newIndex);
    if (d->hasSpans() && (d->spansIntersectRow(logicalOldIndex) || d->spansIntersectRow(logicalNewIndex))) {
        d->viewport->update();
    } else {
        int oldTop = rowViewportPosition(logicalOldIndex);
        int newTop = rowViewportPosition(logicalNewIndex);
        int oldBottom = oldTop + rowHeight(logicalOldIndex);
        int newBottom = newTop + rowHeight(logicalNewIndex);
        int top = qMin(oldTop, newTop);
        int bottom = qMax(oldBottom, newBottom);
        int height = bottom - top;
        d->viewport->update(0, top, d->viewport->width(), height);
    }
}

/*!
    This slot is called to change the index of the given \a column in
    the table view. The old index is specified by \a oldIndex, and
    the new index by \a newIndex.

    \sa rowMoved()
*/
void QTableView::columnMoved(int, int oldIndex, int newIndex)
{
    Q_D(QTableView);

    updateGeometries();
    int logicalOldIndex = d->horizontalHeader->logicalIndex(oldIndex);
    int logicalNewIndex = d->horizontalHeader->logicalIndex(newIndex);
    if (d->hasSpans() && (d->spansIntersectColumn(logicalOldIndex) || d->spansIntersectColumn(logicalNewIndex))) {
        d->viewport->update();
    } else {
        int oldLeft = columnViewportPosition(logicalOldIndex);
        int newLeft = columnViewportPosition(logicalNewIndex);
        int oldRight = oldLeft + columnWidth(logicalOldIndex);
        int newRight = newLeft + columnWidth(logicalNewIndex);
        int left = qMin(oldLeft, newLeft);
        int right = qMax(oldRight, newRight);
        int width = right - left;
        d->viewport->update(left, 0, width, d->viewport->height());
    }
}

/*!
    Selects the given \a row in the table view if the current
    SelectionMode and SelectionBehavior allows rows to be selected.

    \sa selectColumn()
*/
void QTableView::selectRow(int row)
{
    Q_D(QTableView);
    d->selectRow(row, true);
}

/*!
    Selects the given \a column in the table view if the current
    SelectionMode and SelectionBehavior allows columns to be selected.

    \sa selectRow()
*/
void QTableView::selectColumn(int column)
{
    Q_D(QTableView);
    d->selectColumn(column, true);
}

/*!
    Hide the given \a row.

    \sa showRow() hideColumn()
*/
void QTableView::hideRow(int row)
{
    Q_D(QTableView);
    d->verticalHeader->hideSection(row);
}

/*!
    Hide the given \a column.

    \sa showColumn() hideRow()
*/
void QTableView::hideColumn(int column)
{
    Q_D(QTableView);
    d->horizontalHeader->hideSection(column);
}

/*!
    Show the given \a row.

    \sa hideRow() showColumn()
*/
void QTableView::showRow(int row)
{
    Q_D(QTableView);
    d->verticalHeader->showSection(row);
}

/*!
    Show the given \a column.

    \sa hideColumn() showRow()
*/
void QTableView::showColumn(int column)
{
    Q_D(QTableView);
    d->horizontalHeader->showSection(column);
}

/*!
    Resizes the given \a row based on the size hints of the delegate
    used to render each item in the row.
*/
void QTableView::resizeRowToContents(int row)
{
    Q_D(QTableView);
    int content = sizeHintForRow(row);
    int header = d->verticalHeader->sectionSizeHint(row);
    d->verticalHeader->resizeSection(row, qMax(content, header));
}

/*!
    Resizes all rows based on the size hints of the delegate
    used to render each item in the rows.
*/
void QTableView::resizeRowsToContents()
{
    Q_D(QTableView);
    d->verticalHeader->resizeSections(QHeaderView::ResizeToContents);
}

/*!
    Resizes the given \a column based on the size hints of the delegate
    used to render each item in the column.

    \note Only visible columns will be resized. Reimplement sizeHintForColumn()
    to resize hidden columns as well.
*/
void QTableView::resizeColumnToContents(int column)
{
    Q_D(QTableView);
    int content = sizeHintForColumn(column);
    int header = d->horizontalHeader->sectionSizeHint(column);
    d->horizontalHeader->resizeSection(column, qMax(content, header));
}

/*!
    Resizes all columns based on the size hints of the delegate
    used to render each item in the columns.
*/
void QTableView::resizeColumnsToContents()
{
    Q_D(QTableView);
    d->horizontalHeader->resizeSections(QHeaderView::ResizeToContents);
}

/*!
  \obsolete
  \overload

  Sorts the model by the values in the given \a column.
*/
void QTableView::sortByColumn(int column)
{
    Q_D(QTableView);
    if (column == -1)
        return;
    d->model->sort(column, d->horizontalHeader->sortIndicatorOrder());
}

/*!
  \since 4.2

  Sorts the model by the values in the given \a column in the given \a order.

  \sa sortingEnabled
 */
void QTableView::sortByColumn(int column, Qt::SortOrder order)
{
    Q_D(QTableView);
    d->horizontalHeader->setSortIndicator(column, order);
    sortByColumn(column);
}

/*!
    \internal
*/
void QTableView::verticalScrollbarAction(int action)
{
    QAbstractItemView::verticalScrollbarAction(action);
}

/*!
    \internal
*/
void QTableView::horizontalScrollbarAction(int action)
{
    QAbstractItemView::horizontalScrollbarAction(action);
}

/*!
  \reimp
*/
bool QTableView::isIndexHidden(const QModelIndex &index) const
{
    Q_D(const QTableView);
    Q_ASSERT(d->isIndexValid(index));
    if (isRowHidden(index.row()) || isColumnHidden(index.column()))
        return true;
    if (d->hasSpans()) {
        QTableViewPrivate::Span span = d->span(index.row(), index.column());
        return !((span.top() == index.row()) && (span.left() == index.column()));
    }
    return false;
}

/*!
    \fn void QTableView::setSpan(int row, int column, int rowSpanCount, int columnSpanCount)
    \since 4.2

    Sets the span of the table element at (\a row, \a column) to the number of
    rows and columns specified by (\a rowSpanCount, \a columnSpanCount).

    \sa rowSpan(), columnSpan()
*/
void QTableView::setSpan(int row, int column, int rowSpan, int columnSpan)
{
    Q_D(QTableView);
    if (row < 0 || column < 0 || rowSpan < 0 || columnSpan < 0)
        return;
    d->setSpan(row, column, rowSpan, columnSpan);
    d->viewport->update();
}

/*!
  \since 4.2

  Returns the row span of the table element at (\a row, \a column).
  The default is 1.

  \sa setSpan(), columnSpan()
*/
int QTableView::rowSpan(int row, int column) const
{
    Q_D(const QTableView);
    return d->rowSpan(row, column);
}

/*!
  \since 4.2

  Returns the column span of the table element at (\a row, \a
  column). The default is 1.

  \sa setSpan(), rowSpan()
*/
int QTableView::columnSpan(int row, int column) const
{
    Q_D(const QTableView);
    return d->columnSpan(row, column);
}

/*!
  \since 4.4

  Removes all row and column spans in the table view.

  \sa setSpan()
*/

void QTableView::clearSpans()
{
    Q_D(QTableView);
    d->spans.clear();
    d->viewport->update();
}

void QTableViewPrivate::_q_selectRow(int row)
{
    selectRow(row, false);
}

void QTableViewPrivate::_q_selectColumn(int column)
{
    selectColumn(column, false);
}

void QTableViewPrivate::selectRow(int row, bool anchor)
{
    Q_Q(QTableView);

    if (q->selectionBehavior() == QTableView::SelectColumns
        || (q->selectionMode() == QTableView::SingleSelection
            && q->selectionBehavior() == QTableView::SelectItems))
        return;

    if (row >= 0 && row < model->rowCount(root)) {
        int column = horizontalHeader->logicalIndexAt(q->isRightToLeft() ? viewport->width() : 0);
        QModelIndex index = model->index(row, column, root);
        QItemSelectionModel::SelectionFlags command = q->selectionCommand(index);
        selectionModel->setCurrentIndex(index, QItemSelectionModel::NoUpdate);
        if ((!(command & QItemSelectionModel::Current) && anchor)
            || (q->selectionMode() == QTableView::SingleSelection))
            rowSectionAnchor = row;
        QModelIndex tl = model->index(qMin(rowSectionAnchor, row), 0, root);
        QModelIndex br = model->index(qMax(rowSectionAnchor, row), model->columnCount(root) - 1, root);
        if (verticalHeader->sectionsMoved() && tl.row() != br.row())
            q->setSelection(q->visualRect(tl)|q->visualRect(br), command);
        else
            selectionModel->select(QItemSelection(tl, br), command);
    }
}

void QTableViewPrivate::selectColumn(int column, bool anchor)
{
    Q_Q(QTableView);

    if (q->selectionBehavior() == QTableView::SelectRows
        || (q->selectionMode() == QTableView::SingleSelection
            && q->selectionBehavior() == QTableView::SelectItems))
        return;

    if (column >= 0 && column < model->columnCount(root)) {
        int row = verticalHeader->logicalIndexAt(0);
        QModelIndex index = model->index(row, column, root);
        QItemSelectionModel::SelectionFlags command = q->selectionCommand(index);
        selectionModel->setCurrentIndex(index, QItemSelectionModel::NoUpdate);
        if ((!(command & QItemSelectionModel::Current) && anchor)
            || (q->selectionMode() == QTableView::SingleSelection))
            columnSectionAnchor = column;
        QModelIndex tl = model->index(0, qMin(columnSectionAnchor, column), root);
        QModelIndex br = model->index(model->rowCount(root) - 1,
                                      qMax(columnSectionAnchor, column), root);
        if (horizontalHeader->sectionsMoved() && tl.column() != br.column())
            q->setSelection(q->visualRect(tl)|q->visualRect(br), command);
        else
            selectionModel->select(QItemSelection(tl, br), command);
    }
}

/*!
  \reimp
 */
void QTableView::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
#ifndef QT_NO_ACCESSIBILITY
    if (QAccessible::isActive()) {
        if (current.isValid()) {
            int entry = visualIndex(current) + 1;
            if (horizontalHeader())
                ++entry;
            QAccessible::updateAccessibility(viewport(), entry, QAccessible::Focus);
        }
    }
#endif
    QAbstractItemView::currentChanged(current, previous);
}

/*!
  \reimp
 */
void QTableView::selectionChanged(const QItemSelection &selected,
                                  const QItemSelection &deselected)
{
#ifndef QT_NO_ACCESSIBILITY
    if (QAccessible::isActive()) {
        // ### does not work properly for selection ranges.
        QModelIndex sel = selected.indexes().value(0);
        if (sel.isValid()) {
            int entry = visualIndex(sel);
            if (horizontalHeader())
                ++entry;
            QAccessible::updateAccessibility(viewport(), entry, QAccessible::Selection);
        }
        QModelIndex desel = deselected.indexes().value(0);
        if (desel.isValid()) {
            int entry = visualIndex(sel);
            if (horizontalHeader())
                ++entry;
            QAccessible::updateAccessibility(viewport(), entry, QAccessible::SelectionRemove);
        }
    }
#endif
    QAbstractItemView::selectionChanged(selected, deselected);
}

int QTableView::visualIndex(const QModelIndex &index) const
{
    return index.row();
}

QT_END_NAMESPACE

#include "qtableview.moc"

#include "moc_qtableview.cpp"

#endif // QT_NO_TABLEVIEW
