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
    \class QGraphicsLinearLayout
    \brief The QGraphicsLinearLayout class provides a horizontal or vertical
    layout for managing widgets in Graphics View.
    \since 4.4
    \ingroup multimedia

    The default orientation for a linear layout is Qt::Horizontal. You can
    choose a vertical orientation either by calling setOrientation(), or by
    passing Qt::Vertical to QGraphicsLinearLayout's constructor.

    The most common way to use QGraphicsLinearLayout is to construct an object
    on the heap with no parent, add widgets and layouts by calling addItem(),
    and finally assign the layout to a widget by calling
    QGraphicsWidget::setLayout().

    \snippet doc/src/snippets/code/src.gui.graphicsview.qgraphicslinearlayout.cpp 0

    You can add widgets, layouts, stretches (addStretch(), insertStretch() or
    setStretchFactor()), and gaps (addGap() and setItemSpacing()) to a linear
    layout. The layout takes ownership of the items, with one natural
    exception: when a layout is assigned to a widget, all widgets inside the
    layout are automatically reparented to the widget, effectively signing
    over ownership from the layout to the layout-managed widget. You can
    access each item in the layout by calling count() and itemAt(). Calling
    removeAt() or removeItem() will remove an item from the layout, without
    destroying it.

    \section1 Size Hints and Size Policies in QGraphicsLinearLayout

    QGraphicsLinearLayout respects each item's size hints and size policies,
    and when the layout contains more space than the items can fill, each item
    is arranged according to the layout's alignment for that item. You can set
    an alignment for each item by calling setAlignment(), and check the
    alignment for any item by calling alignment(). By default, items are
    centered both vertically and horizontally.

    \section1 Spacing within QGraphicsLinearLayout

    Between the items, the layout distributes some space. The actual amount of
    space depends on the managed widget's current style, but the common
    spacing is 4. You can also set your own spacing by calling setSpacing(),
    and get the current spacing value by calling spacing(). If you want to
    configure individual spacing for your items, you can call setItemSpacing().

    \section1 Stretch Factor in QGraphicsLinearLayout

    You can assign a stretch factor to each item to control how much space it
    will get compared to the other items. By default, two identical widgets
    arranged in a linear layout will have the same size, but if the first
    widget has a stretch factor of 1 and the second widget has a stretch
    factor of 2, the first widget will get 1/3 of the available space, and the
    second will get 2/3.

    QGraphicsLinearLayout calculates the distribution of sizes by adding up
    the stretch factors of all items, and then dividing the available space
    accordingly. The default stretch factor is 0 for all items; a factor of 0
    means the item does not have any defined stretch factor; effectively this
    is the same as setting the stretch factor to 1. The stretch factor only
    applies to the available space in the lengthwise direction of the layout
    (following its orientation). If you want to control both the item's
    horizontal and vertical stretch, you can use QGraphicsGridLayout instead.

    \section1 QGraphicsLinearLayout Compared to Other Layouts

    QGraphicsLinearLayout is very similar to QVBoxLayout and QHBoxLayout, but
    in contrast to these classes, it is used to manage QGraphicsWidget and
    QGraphicsLayout instead of QWidget and QLayout.

    \sa QGraphicsGridLayout, QGraphicsWidget
*/

#include "qapplication.h"

#ifndef QT_NO_GRAPHICSVIEW

#include "qwidget.h"
#include "qgraphicslayout_p.h"
#include "qgraphicslayoutitem.h"
#include "qgraphicslinearlayout.h"
#include "qgraphicswidget.h"
#include "qgridlayoutengine_p.h"
#ifdef QT_DEBUG
#include <QtCore/qdebug.h>
#endif

QT_BEGIN_NAMESPACE

class QGraphicsLinearLayoutPrivate : public QGraphicsLayoutPrivate
{
public:
    QGraphicsLinearLayoutPrivate(Qt::Orientation orientation) : orientation(orientation) { }

    void removeGridItem(QGridLayoutItem *gridItem);
    QLayoutStyleInfo styleInfo() const;
    void fixIndex(int *index) const;
    int gridRow(int index) const;
    int gridColumn(int index) const;

    Qt::Orientation orientation;
    QGridLayoutEngine engine;
};

void QGraphicsLinearLayoutPrivate::removeGridItem(QGridLayoutItem *gridItem)
{
    int index = gridItem->firstRow(orientation);
    engine.removeItem(gridItem);
    engine.removeRow(index, orientation);
}

void QGraphicsLinearLayoutPrivate::fixIndex(int *index) const
{
    int count = engine.rowCount(orientation);
    if (uint(*index) > uint(count))
        *index = count;
}

int QGraphicsLinearLayoutPrivate::gridRow(int index) const
{
    if (orientation == Qt::Horizontal)
        return 0;
    return int(qMin(uint(index), uint(engine.rowCount())));
}

int QGraphicsLinearLayoutPrivate::gridColumn(int index) const
{
    if (orientation == Qt::Vertical)
        return 0;
    return int(qMin(uint(index), uint(engine.columnCount())));
}

QLayoutStyleInfo QGraphicsLinearLayoutPrivate::styleInfo() const
{
    static QWidget *wid = 0;
    if (!wid)
        wid = new QWidget;
    QGraphicsWidget *w = parentWidget();
    QStyle *style = w ? w->style() : qApp->style();
    return QLayoutStyleInfo(style, wid);
}

/*!
    Constructs a QGraphicsLinearLayout instance. You can pass the
    \a orientation for the layout, either horizontal or vertical, and
    \a parent is passed to QGraphicsLayout's constructor.
*/
QGraphicsLinearLayout::QGraphicsLinearLayout(Qt::Orientation orientation, QGraphicsLayoutItem *parent)
    : QGraphicsLayout(*new QGraphicsLinearLayoutPrivate(orientation), parent)
{
}

/*!
    Constructs a QGraphicsLinearLayout instance using Qt::Horizontal
    orientation. \a parent is passed to QGraphicsLayout's constructor.
*/
QGraphicsLinearLayout::QGraphicsLinearLayout(QGraphicsLayoutItem *parent)
    : QGraphicsLayout(*new QGraphicsLinearLayoutPrivate(Qt::Horizontal), parent)
{
}

/*!
    Destroys the QGraphicsLinearLayout object.
*/
QGraphicsLinearLayout::~QGraphicsLinearLayout()
{
}

/*!
    \property QGraphicsLinearLayout::orientation
    \brief the orientation of the linear layout

    This property toggles the layout orientation. Changing the layout
    orientation will automatically invalidate the layout.

    \sa invalidate()
*/
void QGraphicsLinearLayout::setOrientation(Qt::Orientation orientation)
{
    Q_D(QGraphicsLinearLayout);
    if (orientation != d->orientation) {
        d->engine.transpose();
        d->orientation = orientation;
        invalidate();
    }
}
Qt::Orientation QGraphicsLinearLayout::orientation() const
{
    Q_D(const QGraphicsLinearLayout);
    return d->orientation;
}

/*!
    \fn void QGraphicsLinearLayout::addItem(QGraphicsLayoutItem *item)

    This convenience function is equivalent to calling
    insertItem(-1, \a item).
*/

/*!
    \fn void QGraphicsLinearLayout::addStretch(int stretch)

    This convenience function is equivalent to calling
    insertStretch(-1, \a stretch).
*/

/*!
    \fn void QGraphicsLinearLayout::addGap(qreal spacing)

    This convenience function is equivalent to calling
    setItemSpacing(-1, \a spacing).
*/

/*!
    Inserts \a item into the layout at \a index, or before any item that is
    currently at \a index.

    \sa addItem(), itemAt(), insertStretch(), setItemSpacing()
*/
void QGraphicsLinearLayout::insertItem(int index, QGraphicsLayoutItem *item)
{
    Q_D(QGraphicsLinearLayout);
    if (!item) {
        qWarning("QGraphicsLinearLayout::insertItem: cannot insert null item");
        return;
    }
    if (item->isLayout())
        d->addChildLayout(static_cast<QGraphicsLayout *>(item));
    else
        d->addChildWidget(static_cast<QGraphicsWidget *>(item));

    Q_ASSERT(item);
    d->fixIndex(&index);
    d->engine.insertRow(index, d->orientation);
    new QGridLayoutItem(&d->engine, item, d->gridRow(index), d->gridColumn(index));
    invalidate();
}

/*!
    Inserts a stretch of \a stretch at \a index, or before any item that is
    currently at \a index.

    \sa addStretch(), setStretchFactor(), setItemSpacing(), insertItem()
*/
void QGraphicsLinearLayout::insertStretch(int index, int stretch)
{
    Q_D(QGraphicsLinearLayout);
    d->fixIndex(&index);
    d->engine.insertRow(index, d->orientation);
    d->engine.setRowStretchFactor(index, stretch, d->orientation);
    invalidate();
}

/*!
    Removes \a item from the layout without destroying it. Ownership of
    \a item is transferred to the caller.

    \sa removeAt(), insertItem()
*/
void QGraphicsLinearLayout::removeItem(QGraphicsLayoutItem *item)
{
    Q_D(QGraphicsLinearLayout);
    if (QGridLayoutItem *gridItem = d->engine.findLayoutItem(item)) {
        d->removeGridItem(gridItem);
        delete gridItem;
        invalidate();
    }
}

/*!
    Removes the item at \a index without destroying it. Ownership of the item
    is transferred to the caller.

    \sa removeItem(), insertItem()
*/
void QGraphicsLinearLayout::removeAt(int index)
{
    Q_D(QGraphicsLinearLayout);
    if (QGridLayoutItem *gridItem = d->engine.itemAt(d->gridRow(index), d->gridColumn(index))) {
        d->removeGridItem(gridItem);
        delete gridItem;
        invalidate();
    }
}

/*!
    \property QGraphicsLinearLayout::spacing
    \brief the layout's default spacing

    This property adjusts the layout's default spacing. The spacing applies to
    the vertical and horizontal distance between items.

    \sa setItemSpacing(), setStretchFactor(), QGraphicsGridLayout::setSpacing()
*/
void QGraphicsLinearLayout::setSpacing(qreal spacing)
{
    Q_D(QGraphicsLinearLayout);
    if (spacing < 0) {
        qWarning("QGraphicsLinearLayout::setSpacing: invalid spacing %g", spacing);
        return;
    }
    d->engine.setSpacing(spacing, Qt::Horizontal | Qt::Vertical);
    invalidate();
}

qreal QGraphicsLinearLayout::spacing() const
{
    Q_D(const QGraphicsLinearLayout);
    return d->engine.spacing(d->styleInfo(), d->orientation);
}

/*!
    Sets the spacing after item at \a index to \a spacing.
*/
void QGraphicsLinearLayout::setItemSpacing(int index, qreal spacing)
{
    Q_D(QGraphicsLinearLayout);
    d->engine.setRowSpacing(index, spacing, d->orientation);
    invalidate();
}
/*!
    Returns the spacing after item at \a index.
*/
qreal QGraphicsLinearLayout::itemSpacing(int index) const
{
    Q_D(const QGraphicsLinearLayout);
    return d->engine.rowSpacing(index, d->orientation);
}

/*!
    Sets the stretch factor for \a item to \a stretch. If an item's stretch
    factor changes, this function will invalidate the layout.

    Setting \a stretch to 0 removes the stretch factor from the item, and is
    effectively equivalent to setting \a stretch to 1.

    \sa stretchFactor()
*/
void QGraphicsLinearLayout::setStretchFactor(QGraphicsLayoutItem *item, int stretch)
{
    Q_D(QGraphicsLinearLayout);
    if (!item) {
        qWarning("QGraphicsLinearLayout::setStretchFactor: cannot assign"
                 " a stretch factor to a null item");
        return;
    }
    if (stretchFactor(item) == stretch)
        return;
    d->engine.setStretchFactor(item, stretch, d->orientation);
    invalidate();
}

/*!
    Returns the stretch factor for \a item. The default stretch factor is 0,
    meaning that the item has no assigned stretch factor.

    \sa setStretchFactor()
*/
int QGraphicsLinearLayout::stretchFactor(QGraphicsLayoutItem *item) const
{
    Q_D(const QGraphicsLinearLayout);
    if (!item) {
        qWarning("QGraphicsLinearLayout::setStretchFactor: cannot return"
                 " a stretch factor for a null item");
        return 0;
    }
    return d->engine.stretchFactor(item, d->orientation);
}

/*!
    Sets the alignment of \a item to \a alignment. If \a item's alignment
    changes, the layout is automatically invalidated.

    \sa alignment(), invalidate()
*/
void QGraphicsLinearLayout::setAlignment(QGraphicsLayoutItem *item, Qt::Alignment alignment)
{
    Q_D(QGraphicsLinearLayout);
    if (this->alignment(item) == alignment)
        return;
    d->engine.setAlignment(item, alignment);
    invalidate();
}

/*!
    Returns the alignment for \a item. The default alignment is
    Qt::AlignCenter.

    The alignment decides how the item is positioned within its assigned space
    in the case where there's more space available in the layout than the
    widgets can occupy.

    \sa setAlignment()
*/
Qt::Alignment QGraphicsLinearLayout::alignment(QGraphicsLayoutItem *item) const
{
    Q_D(const QGraphicsLinearLayout);
    return d->engine.alignment(item);
}

#if 0 // ###
QSizePolicy::ControlTypes QGraphicsLinearLayout::controlTypes(LayoutSide side) const
{
    return d->engine.controlTypes(side);
}
#endif

/*!
    \reimp
*/
int QGraphicsLinearLayout::count() const
{
    Q_D(const QGraphicsLinearLayout);
    return d->engine.rowCount(d->orientation);
}

/*!
    \reimp
*/
QGraphicsLayoutItem *QGraphicsLinearLayout::itemAt(int index) const
{
    Q_D(const QGraphicsLinearLayout);
    QGraphicsLayoutItem *item = 0;
    if (QGridLayoutItem *gridItem = d->engine.itemAt(d->gridRow(index), d->gridColumn(index)))
        item = gridItem->layoutItem();
    return item;
}

/*!
    \reimp
*/
void QGraphicsLinearLayout::setGeometry(const QRectF &rect)
{
    Q_D(QGraphicsLinearLayout);
    QGraphicsLayout::setGeometry(rect);
    QRectF effectiveRect = geometry();
    qreal left, top, right, bottom;
    getContentsMargins(&left, &top, &right, &bottom);
    effectiveRect.adjust(+left, +top, -right, -bottom);
#ifdef QT_DEBUG
    if (qt_graphicsLayoutDebug()) {
        static int counter = 0;
        qDebug() << counter++ << "QGraphicsLinearLayout::setGeometry - " << rect;
        dump(1);
    }
#endif
    d->engine.setGeometries(d->styleInfo(), effectiveRect);
#ifdef QT_DEBUG
    if (qt_graphicsLayoutDebug()) {
        qDebug() << "post dump";
        dump(1);
    }
#endif
}

/*!
    \reimp
*/
QSizeF QGraphicsLinearLayout::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const
{
    Q_D(const QGraphicsLinearLayout);
    qreal left, top, right, bottom;
    getContentsMargins(&left, &top, &right, &bottom);
    return d->engine.sizeHint(d->styleInfo(), which , constraint) + QSizeF(left + right, top + bottom);
}

/*!
    \reimp
*/
void QGraphicsLinearLayout::invalidate()
{
    Q_D(QGraphicsLinearLayout);
    d->engine.invalidate();
    QGraphicsLayout::invalidate();
}

#ifdef QT_DEBUG
void QGraphicsLinearLayout::dump(int indent) const
{
    if (qt_graphicsLayoutDebug()) {
        Q_D(const QGraphicsLinearLayout);
        qDebug("%*s%s layout", indent, "",
               d->orientation == Qt::Horizontal ? "Horizontal" : "Vertical");
        d->engine.dump(indent + 1);
    }
}
#endif

QT_END_NAMESPACE
        
#endif //QT_NO_GRAPHICSVIEW
