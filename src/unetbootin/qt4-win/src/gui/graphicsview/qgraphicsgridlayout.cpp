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

/*!
    \class QGraphicsGridLayout
    \brief The QGraphicsGridLayout class provides a grid layout for managing
    widgets in Graphics View.
    \since 4.4
    \ingroup multimedia

    The most common way to use QGraphicsGridLayout is to construct an object
    on the heap with no parent, add widgets and layouts by calling addItem(),
    and finally assign the layout to a widget by calling
    QGraphicsWidget::setLayout(). QGraphicsGridLayout automatically computes
    the dimensions of the grid as you add items.

    \snippet doc/src/snippets/code/src.gui.graphicsview.qgraphicsgridlayout.cpp 0

    \sa QGraphicsLinearLayout, QGraphicsWidget
*/

#include "qglobal.h"

#ifndef QT_NO_GRAPHICSVIEW

#include "qapplication.h"
#include "qwidget.h"
#include "qgraphicslayout_p.h"
#include "qgraphicslayoutitem.h"
#include "qgraphicsgridlayout.h"
#include "qgraphicswidget.h"
#include "qgridlayoutengine_p.h"
#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

class QGraphicsGridLayoutPrivate : public QGraphicsLayoutPrivate
{
public:
    QGraphicsGridLayoutPrivate() { }
    QLayoutStyleInfo styleInfo() const;

    QGridLayoutEngine engine;
};

QLayoutStyleInfo QGraphicsGridLayoutPrivate::styleInfo() const
{
    static QWidget *wid = 0;
    if (!wid)
        wid = new QWidget;
    QGraphicsWidget *w = parentWidget();
    QStyle *style = w ? w->style() : qApp->style();
    return QLayoutStyleInfo(style, wid);
}

/*!
    Constructs a QGraphicsGridLayout instance.  \a parent is passed to
    QGraphicsLayout's constructor.
*/
QGraphicsGridLayout::QGraphicsGridLayout(QGraphicsLayoutItem *parent)
    : QGraphicsLayout(*new QGraphicsGridLayoutPrivate(), parent)
{
}

/*!
    Destroys the QGraphicsGridLayout object.
*/
QGraphicsGridLayout::~QGraphicsGridLayout()
{
}

/*!
    Adds \a item to the grid on \a row and \a column. You can specify a
    \a rowSpan and \a columnSpan and an optional \a alignment.
*/
void QGraphicsGridLayout::addItem(QGraphicsLayoutItem *item, int row, int column,
                                  int rowSpan, int columnSpan, Qt::Alignment alignment)
{
    Q_D(QGraphicsGridLayout);
    if (row < 0 || column < 0) {
	qWarning("QGraphicsGridLayout::addItem: invalid row/column: %d",
		 row < 0 ? row : column);
	return;
    }
    if (columnSpan < 1 || rowSpan < 1) {
	qWarning("QGraphicsGridLayout::addItem: invalid row span/column span: %d",
		 rowSpan < 1 ? rowSpan : columnSpan);
	return;
    }
    if (!item) {
	qWarning("QGraphicsGridLayout::addItem: cannot add null item");
	return;
    }

    if (item->isLayout())
        d->addChildLayout(static_cast<QGraphicsLayout *>(item));
    else
        d->addChildWidget(static_cast<QGraphicsWidget *>(item));

    new QGridLayoutItem(&d->engine, item, row, column, rowSpan, columnSpan, alignment);
    invalidate();
}

/*!
    \fn QGraphicsGridLayout::addItem(QGraphicsLayoutItem *item, int row, int column, Qt::Alignment alignment = 0)

    Adds \a item to the grid on \a row and \a column. You can specify
    an optional \a alignment for \a item.
*/

/*!
    Sets the default horizontal spacing for the grid layout to \a spacing.
*/
void QGraphicsGridLayout::setHorizontalSpacing(qreal spacing)
{
    Q_D(QGraphicsGridLayout);
    d->engine.setSpacing(spacing, Qt::Horizontal);
    invalidate();
}

/*!
    Returns the default horizontal spacing for the grid layout.
*/
qreal QGraphicsGridLayout::horizontalSpacing() const
{
    Q_D(const QGraphicsGridLayout);
    return d->engine.spacing(d->styleInfo(), Qt::Horizontal);
}

/*!
    Sets the default vertical spacing for the grid layout to \a spacing.
*/
void QGraphicsGridLayout::setVerticalSpacing(qreal spacing)
{
    Q_D(QGraphicsGridLayout);
    d->engine.setSpacing(spacing, Qt::Vertical);
    invalidate();
}

/*!
    Returns the default vertical spacing for the grid layout.
*/
qreal QGraphicsGridLayout::verticalSpacing() const
{
    Q_D(const QGraphicsGridLayout);
    return d->engine.spacing(d->styleInfo(), Qt::Vertical);
}

/*!
    Sets the grid layout's default spacing, both vertical and
    horizontal, to \a spacing.

    \sa rowSpacing(), columnSpacing()
*/
void QGraphicsGridLayout::setSpacing(qreal spacing)
{
    Q_D(QGraphicsGridLayout);
    d->engine.setSpacing(spacing, Qt::Horizontal | Qt::Vertical);
    invalidate();
}

/*!
    Sets the spacing for \a row to \a spacing.
*/
void QGraphicsGridLayout::setRowSpacing(int row, qreal spacing)
{
    Q_D(QGraphicsGridLayout);
    d->engine.setRowSpacing(row, spacing, Qt::Vertical);
    invalidate();
}

/*!
    Returns the row spacing for \a row.
*/
qreal QGraphicsGridLayout::rowSpacing(int row) const
{
    Q_D(const QGraphicsGridLayout);
    return d->engine.rowSpacing(row, Qt::Vertical);
}

/*!
    Sets the spacing for \a column to \a spacing.
*/
void QGraphicsGridLayout::setColumnSpacing(int column, qreal spacing)
{
    Q_D(QGraphicsGridLayout);
    d->engine.setRowSpacing(column, spacing, Qt::Horizontal);
    invalidate();
}

/*!
    Returns the column spacing for \a column.
*/
qreal QGraphicsGridLayout::columnSpacing(int column) const
{
    Q_D(const QGraphicsGridLayout);
    return d->engine.rowSpacing(column, Qt::Horizontal);
}

/*!
    Sets the stretch factor for \a row to \a stretch.
*/
void QGraphicsGridLayout::setRowStretchFactor(int row, int stretch)
{
    Q_D(QGraphicsGridLayout);
    d->engine.setRowStretchFactor(row, stretch, Qt::Vertical);
    invalidate();
}

/*!
    Returns the stretch factor for \a row.
*/
int QGraphicsGridLayout::rowStretchFactor(int row) const
{
    Q_D(const QGraphicsGridLayout);
    return d->engine.rowStretchFactor(row, Qt::Vertical);
}

/*!
    Sets the stretch factor for \a column to \a stretch.
*/
void QGraphicsGridLayout::setColumnStretchFactor(int column, int stretch)
{
    Q_D(QGraphicsGridLayout);
    d->engine.setRowStretchFactor(column, stretch, Qt::Horizontal);
    invalidate();
}

/*!
    Returns the stretch factor for \a column.
*/
int QGraphicsGridLayout::columnStretchFactor(int column) const
{
    Q_D(const QGraphicsGridLayout);
    return d->engine.rowStretchFactor(column, Qt::Horizontal);
}

/*!
    Sets the minimum height for row, \a row, to \a height.
*/
void QGraphicsGridLayout::setRowMinimumHeight(int row, qreal height)
{
    Q_D(QGraphicsGridLayout);
    d->engine.setRowSizeHint(Qt::MinimumSize, row, height, Qt::Vertical);
    invalidate();
}

/*!
    Returns the minimum height for row, \a row.
*/
qreal QGraphicsGridLayout::rowMinimumHeight(int row) const
{
    Q_D(const QGraphicsGridLayout);
    return d->engine.rowSizeHint(Qt::MinimumSize, row, Qt::Vertical);
}

/*!
    Sets the preferred height for row, \a row, to \a height.
*/
void QGraphicsGridLayout::setRowPreferredHeight(int row, qreal height)
{
    Q_D(QGraphicsGridLayout);
    d->engine.setRowSizeHint(Qt::PreferredSize, row, height, Qt::Vertical);
    invalidate();
}

/*!
    Returns the preferred height for row, \a row.
*/
qreal QGraphicsGridLayout::rowPreferredHeight(int row) const
{
    Q_D(const QGraphicsGridLayout);
    return d->engine.rowSizeHint(Qt::PreferredSize, row, Qt::Vertical);
}

/*!
    Sets the maximum height for row, \a row, to \a height.
*/
void QGraphicsGridLayout::setRowMaximumHeight(int row, qreal height)
{
    Q_D(QGraphicsGridLayout);
    d->engine.setRowSizeHint(Qt::MaximumSize, row, height, Qt::Vertical);
    invalidate();
}

/*!
    Returns the maximum height for row, \a row.
*/
qreal QGraphicsGridLayout::rowMaximumHeight(int row) const
{
    Q_D(const QGraphicsGridLayout);
    return d->engine.rowSizeHint(Qt::MaximumSize, row, Qt::Vertical);
}

/*!
    Sets the fixed height for row, \a row, to \a height.
*/
void QGraphicsGridLayout::setRowFixedHeight(int row, qreal height)
{
    Q_D(QGraphicsGridLayout);
    d->engine.setRowSizeHint(Qt::MinimumSize, row, height, Qt::Vertical);
    d->engine.setRowSizeHint(Qt::MaximumSize, row, height, Qt::Vertical);
    invalidate();
}

/*!
    Sets the minimum width for \a column to \a width.
*/
void QGraphicsGridLayout::setColumnMinimumWidth(int column, qreal width)
{
    Q_D(QGraphicsGridLayout);
    d->engine.setRowSizeHint(Qt::MinimumSize, column, width, Qt::Horizontal);
    invalidate();
}

/*!
    Returns the minimum width for \a column.
*/
qreal QGraphicsGridLayout::columnMinimumWidth(int column) const
{
    Q_D(const QGraphicsGridLayout);
    return d->engine.rowSizeHint(Qt::MinimumSize, column, Qt::Horizontal);
}

/*!
    Sets the preferred width for \a column to \a width.
*/
void QGraphicsGridLayout::setColumnPreferredWidth(int column, qreal width)
{
    Q_D(QGraphicsGridLayout);
    d->engine.setRowSizeHint(Qt::PreferredSize, column, width, Qt::Horizontal);
    invalidate();
}

/*!
    Returns the preferred width for \a column.
*/
qreal QGraphicsGridLayout::columnPreferredWidth(int column) const
{
    Q_D(const QGraphicsGridLayout);
    return d->engine.rowSizeHint(Qt::PreferredSize, column, Qt::Horizontal);
}

/*!
    Sets the maximum width of \a column to \a width.
*/
void QGraphicsGridLayout::setColumnMaximumWidth(int column, qreal width)
{
    Q_D(QGraphicsGridLayout);
    d->engine.setRowSizeHint(Qt::MaximumSize, column, width, Qt::Horizontal);
    invalidate();
}

/*!
    Returns the maximum width for \a column.
*/
qreal QGraphicsGridLayout::columnMaximumWidth(int column) const
{
    Q_D(const QGraphicsGridLayout);
    return d->engine.rowSizeHint(Qt::MaximumSize, column, Qt::Horizontal);
}

/*!
    Sets the fixed width of \a column to \a width.
*/
void QGraphicsGridLayout::setColumnFixedWidth(int column, qreal width)
{
    Q_D(QGraphicsGridLayout);
    d->engine.setRowSizeHint(Qt::MinimumSize, column, width, Qt::Horizontal);
    d->engine.setRowSizeHint(Qt::MaximumSize, column, width, Qt::Horizontal);
    invalidate();
}

/*!
    Sets the alignment of \a row to \a alignment.
*/
void QGraphicsGridLayout::setRowAlignment(int row, Qt::Alignment alignment)
{
    Q_D(QGraphicsGridLayout);
    d->engine.setRowAlignment(row, alignment, Qt::Vertical);
    invalidate();
}

/*!
    Returns the alignment of \a row.
*/
Qt::Alignment QGraphicsGridLayout::rowAlignment(int row) const
{
    Q_D(const QGraphicsGridLayout);
    return d->engine.rowAlignment(row, Qt::Vertical);
}

/*!
    Sets the alignment for \a column to \a alignment.
*/
void QGraphicsGridLayout::setColumnAlignment(int column, Qt::Alignment alignment)
{
    Q_D(QGraphicsGridLayout);
    d->engine.setRowAlignment(column, alignment, Qt::Horizontal);
    invalidate();
}

/*!
    Returns the alignment for \a column.
*/
Qt::Alignment QGraphicsGridLayout::columnAlignment(int column) const
{
    Q_D(const QGraphicsGridLayout);
    return d->engine.rowAlignment(column, Qt::Horizontal);
}

/*!
    Sets the alignment for \a item to \a alignment.
*/
void QGraphicsGridLayout::setAlignment(QGraphicsLayoutItem *item, Qt::Alignment alignment)
{
    Q_D(QGraphicsGridLayout);
    d->engine.setAlignment(item, alignment);
    invalidate();
}

/*!
    Returns the alignment for \a item.
*/
Qt::Alignment QGraphicsGridLayout::alignment(QGraphicsLayoutItem *item) const
{
    Q_D(const QGraphicsGridLayout);
    return d->engine.alignment(item);
}

/*!
    Returns the number of rows in the grid layout. This is always one more
    than the index of the last row that is occupied by a layout item (empty
    rows are counted except for those at the end).
*/
int QGraphicsGridLayout::rowCount() const
{
    Q_D(const QGraphicsGridLayout);
    return d->engine.effectiveLastRow(Qt::Vertical) + 1;
}

/*!
    Returns the number of columns in the grid layout. This is always one more
    than the index of  the last column that is occupied by a layout item (empty
    columns are counted except for those at the end).
*/
int QGraphicsGridLayout::columnCount() const
{
    Q_D(const QGraphicsGridLayout);
    return d->engine.effectiveLastRow(Qt::Horizontal) + 1;
}

/*!
    Returns a pointer to the layout item at (\a row, \a column).
*/
QGraphicsLayoutItem *QGraphicsGridLayout::itemAt(int row, int column) const
{
    Q_D(const QGraphicsGridLayout);
    if (row < 0 || row >= rowCount() || column < 0 || column >= columnCount()) {
        qWarning("QGraphicsGridLayout::itemAt: invalid row, column %d, %d", row, column);
        return 0;
    }
    if (QGridLayoutItem *item = d->engine.itemAt(row, column))
        return item->layoutItem();
    return 0;
}

/*!
    Returns the number of layout items in this grid layout.
*/
int QGraphicsGridLayout::count() const
{
    Q_D(const QGraphicsGridLayout);
    return d->engine.itemCount();
}

/*!
    Returns the layout item at \a index, or 0 if there is no layout item at
    this index.
*/
QGraphicsLayoutItem *QGraphicsGridLayout::itemAt(int index) const
{
    Q_D(const QGraphicsGridLayout);
    if (index < 0 || index >= d->engine.itemCount()) {
        qWarning("QGraphicsGridLayout::itemAt: invalid index %d", index);
        return 0;
    }
    QGraphicsLayoutItem *item = 0;
    if (QGridLayoutItem *gridItem = d->engine.itemAt(index))
        item = gridItem->layoutItem();
    return item;
}

/*!
    Removes the layout item at \a index without destroying it. Ownership of
    the item is transferred to the caller.

    \sa addItem()
*/
void QGraphicsGridLayout::removeAt(int index)
{
    Q_D(QGraphicsGridLayout);
    if (index < 0 || index >= d->engine.itemCount()) {
        qWarning("QGraphicsGridLayout::removeAt: invalid index %d", index);
        return;
    }
    if (QGridLayoutItem *gridItem = d->engine.itemAt(index)) {
        d->engine.removeItem(gridItem);
        delete gridItem;
        invalidate();
    }
}

/*!
    \reimp
*/
void QGraphicsGridLayout::invalidate()
{
    Q_D(QGraphicsGridLayout);
    d->engine.invalidate();
    QGraphicsLayout::invalidate();
}

/*!
    Sets the bounding geometry of the grid layout to \a rect.
*/
void QGraphicsGridLayout::setGeometry(const QRectF &rect)
{
    Q_D(QGraphicsGridLayout);
    QGraphicsLayout::setGeometry(rect);
    QRectF effectiveRect = geometry();
    qreal left, top, right, bottom;
    getContentsMargins(&left, &top, &right, &bottom);

    effectiveRect.adjust(+left, +top, -right, -bottom);
#ifdef QT_DEBUG
    if (qt_graphicsLayoutDebug()) {
        static int counter = 0;
        qDebug() << counter++ << "QGraphicsGridLayout::setGeometry - " << rect;
        // dump(1);
    }
#endif
    d->engine.setGeometries(d->styleInfo(), effectiveRect);
#ifdef QT_DEBUG
    if (qt_graphicsLayoutDebug()) {
        qDebug() << "post dump";
        // dump(1);
    }
#endif
}

/*!
    \reimp
*/
QSizeF QGraphicsGridLayout::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const
{
    Q_D(const QGraphicsGridLayout);
    qreal left, top, right, bottom;
    getContentsMargins(&left, &top, &right, &bottom);
    return d->engine.sizeHint(d->styleInfo(), which , constraint) + QSizeF(left + right, top + bottom);
}


#if 0
// ### kill? (implement and kill?)
QRect QGraphicsGridLayout::cellRect(int row, int column, int rowSpan, int columnSpan) const
{
    Q_D(const QGraphicsGridLayout);
    return QRect();
//    return d->engine.cellRect(parentLayoutable(), contentsGeometry(), row, column, rowSpan, columnSpan);
}

QSizePolicy::ControlTypes QGraphicsGridLayout::controlTypes(LayoutSide side) const
{
    Q_D(const QGraphicsGridLayout);
    return d->engine.controlTypes(side);
}
#endif

QT_END_NAMESPACE
        
#endif //QT_NO_GRAPHICSVIEW        
