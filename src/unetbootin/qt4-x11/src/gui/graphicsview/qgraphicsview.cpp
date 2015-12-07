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

//#define QGRAPHICSVIEW_DEBUG

static const int QGRAPHICSVIEW_REGION_RECT_THRESHOLD = 50;

static const int QGRAPHICSVIEW_PREALLOC_STYLE_OPTIONS = 503; // largest prime < 2^9

/*!
    \class QGraphicsView
    \brief The QGraphicsView class provides a widget for displaying the
    contents of a QGraphicsScene.
    \since 4.2
    \ingroup multimedia
    \ingroup graphicsview-api
    \mainclass

    QGraphicsView visualizes the contents of a QGraphicsScene in a scrollable
    viewport. To create a scene with geometrical items, see QGraphicsScene's
    documentation. QGraphicsView is part of \l{The Graphics View Framework}.

    To visualize a scene, you start by constructing a QGraphicsView object,
    passing the address of the scene you want to visualize to QGraphicsView's
    constructor. Alternatively, you can call setScene() to set the scene at a
    later point. After you call show(), the view will by default scroll to the
    center of the scene and display any items that are visible at this
    point. For example:

    \snippet doc/src/snippets/code/src_gui_graphicsview_qgraphicsview.cpp 0

    You can explicitly scroll to any position on the scene by using the
    scroll bars, or by calling centerOn(). By passing a point to centerOn(),
    QGraphicsView will scroll its viewport to ensure that the point is
    centered in the view. An overload is provided for scrolling to a
    QGraphicsItem, in which case QGraphicsView will see to that the center of
    the item is centered in the view. If all you want is to ensure that a
    certain area is visible, (but not necessarily centered,) you can call
    ensureVisible() instead.

    QGraphicsView can be used to visualize a whole scene, or only parts of it.
    The visualized area is by default detected automatically when the view is
    displayed for the first time (by calling
    QGraphicsScene::itemsBoundingRect()). To set the visualized area rectangle
    yourself, you can call setSceneRect(). This will adjust the scroll bars'
    ranges appropriately. Note that although the scene supports a virtually
    unlimited size, the range of the scroll bars will never exceed the range of
    an integer (INT_MIN, INT_MAX). When the scene is larger than the scroll
    bars' values, you can choose to use translate() to navigate the scene
    instead.

    QGraphicsView visualizes the scene by calling render(). By default, the
    items are drawn onto the viewport by using a regular QPainter, and using
    default render hints. To change the default render hints that
    QGraphicsView passes to QPainter when painting items, you can call
    setRenderHints().

    By default, QGraphicsView provides a regular QWidget for the viewport
    widget. You can access this widget by calling viewport(), or you can
    replace it by calling setViewport(). To render using OpenGL, simply call
    setViewport(new QGLWidget). QGraphicsView takes ownership of the viewport
    widget.

    QGraphicsView supports affine transformations, using QMatrix. You can
    either pass a matrix to setMatrix(), or you can call one of the
    convenience functions rotate(), scale(), translate() or shear(). The most
    two common transformations are scaling, which is used to implement
    zooming, and rotation. QGraphicsView keeps the center of the view fixed
    during a transformation.

    You can interact with the items on the scene by using the mouse and
    keyboard. QGraphicsView translates the mouse and key events into \e scene
    events, (events that inherit QGraphicsSceneEvent,), and forward them to
    the visualized scene. In the end, it's the individual item that handles
    the events and reacts to them. For example, if you click on a selectable
    item, the item will typically let the scene know that it has been
    selected, and it will also redraw itself to display a selection
    rectangle. Similiary, if you click and drag the mouse to move a movable
    item, it's the item that handles the mouse moves and moves itself.  Item
    interaction is enabled by default, and you can toggle it by calling
    setInteractive().

    You can also provide your own custom scene interaction, by creating a
    subclass of QGraphicsView, and reimplementing the mouse and key event
    handlers. To simplify how you programmatically interact with items in the
    view, QGraphicsView provides the mapping functions mapToScene() and
    mapFromScene(), and the item accessors items() and itemAt(). These
    functions allow you to map points, rectangles, polygons and paths between
    view coordinates and scene coordinates, and to find items on the scene
    using view coordinates.

    \img graphicsview-view.png

    \sa QGraphicsScene, QGraphicsItem, QGraphicsSceneEvent
*/

/*!
    \enum QGraphicsView::ViewportAnchor

    This enums describe the possible anchors that QGraphicsView can
    use when the user resizes the view or when the view is
    transformed.

    \value NoAnchor No anchor, i.e. the view leaves the scene's
                    position unchanged.
    \value AnchorViewCenter The scene point at the center of the view
                            is used as the anchor.
    \value AnchorUnderMouse The point under the mouse is used as the anchor.

    \sa resizeAnchor, transformationAnchor
*/

/*!
    \enum QGraphicsView::ViewportUpdateMode

    \since 4.3

    This enum describes how QGraphicsView updates its viewport when the scene
    contents change or are exposed.

    \value FullViewportUpdate When any visible part of the scene changes or is
    reexposed, QGraphicsView will update the entire viewport. This approach is
    fastest when QGraphicsView spends more time figuring out what to draw than
    it would spend drawing (e.g., when very many small items are repeatedly
    updated). This is the preferred update mode for viewports that do not
    support partial updates, such as QGLWidget, and for viewports that need to
    disable scroll optimization.

    \value MinimalViewportUpdate QGraphicsView will determine the minimal
    viewport region that requires a redraw, minimizing the time spent drawing
    by avoiding a redraw of areas that have not changed. This is
    QGraphicsView's default mode. Although this approach provides the best
    performance in general, if there are many small visible changes on the
    scene, QGraphicsView might end up spending more time finding the minimal
    approach than it will spend drawing.

    \value SmartViewportUpdate QGraphicsView will attempt to find an optimal
    update mode by analyzing the areas that require a redraw.

    \value BoundingRectViewportUpdate The bounding rectangle of all changes in
    the viewport will be redrawn. This mode has the advantage that
    QGraphicsView searches only one region for changes, minimizing time spent
    determining what needs redrawing. The disadvantage is that areas that have
    not changed also need to be redrawn.

    \value NoViewportUpdate QGraphicsView will never update its viewport when
    the scene changes; the user is expected to control all updates. This mode
    disables all (potentially slow) item visibility testing in QGraphicsView,
    and is suitable for scenes that either require a fixed frame rate, or where
    the viewport is otherwise updated externally.

    \sa viewportUpdateMode
*/

/*!
    \enum QGraphicsView::OptimizationFlag

    \since 4.3

    This enum describes flags that you can enable to improve rendering
    performance in QGraphicsView. By default, none of these flags are set.
    Note that setting a flag usually imposes a side effect, and this effect
    can vary between paint devices and platforms.

    \value DontClipPainter QGraphicsView sometimes clips the painter when
    rendering the scene contents. This can generally improve performance
    (e.g., rendering only small parts of a large pixmap), and protects against
    rendering mistakes (e.g., drawing outside bounding rectangles, or outside
    the exposed area). In some situations, however, the painter clip can slow
    down rendering; especially when all painting is restricted to inside
    exposed areas. By enabling this flag, QGraphicsView will completely
    disable its implicit clipping. Note that rendering artifacts from using a
    semi-transparent foreground or background brush can occur if clipping is
    disabled.

    \value DontSavePainterState When rendering, QGraphicsView protects the
    painter state (see QPainter::save()) when rendering the background or
    foreground, and when rendering each item. This allows you to leave the
    painter in an altered state (i.e., you can call QPainter::setPen() or
    QPainter::setBrush() without restoring the state after painting). However,
    if the items consistently do restore the state, you should enable this
    flag to prevent QGraphicsView from doing the same.

    \value DontAdjustForAntialiasing Disables QGraphicsView's antialiasing
    auto-adjustment of exposed areas. Items that render antialiased lines on
    the boundaries of their QGraphicsItem::boundingRect() can end up rendering
    parts of the line outside. To prevent rendering artifacts, QGraphicsView
    expands all exposed regions by 2 pixels in all directions. If you enable
    this flag, QGraphicsView will no longer perform these adjustments,
    minimizing the areas that require redrawing, which improves performance. A
    common side effect is that items that do draw with antialiasing can leave
    painting traces behind on the scene as they are moved.
*/

/*!
    \enum QGraphicsView::CacheModeFlag

    This enum describes the flags that you can set for a QGraphicsView's cache
    mode.

    \value CacheNone All painting is done directly onto the viewport.

    \value CacheBackground The background is cached. This affects both custom
    backgrounds, and backgrounds based on the backgroundBrush property. When
    this flag is enabled, QGraphicsView will allocate one pixmap with the full
    size of the viewport.

    \sa cacheMode
*/

/*!
    \enum QGraphicsView::DragMode

    This enum describes the default action for the view when pressing and
    dragging the mouse over the viewport.

    \value NoDrag Nothing happens; the mouse event is ignored.

    \value ScrollHandDrag The cursor changes into a pointing hand, and
    dragging the mouse around will scroll the scrolbars. This mode works both
    in \l{QGraphicsView::interactive}{interactive} and non-interactive mode.

    \value RubberBandDrag A rubber band will appear. Dragging the mouse will
    set the rubber band geometry, and all items covered by the rubber band are
    selected. This mode is disabled for non-interactive views.

    \sa dragMode, QGraphicsScene::setSelectionArea()
*/

#include "qgraphicsview.h"
#include "qgraphicsview_p.h"

#ifndef QT_NO_GRAPHICSVIEW

#include "qgraphicsitem.h"
#include "qgraphicsitem_p.h"
#include "qgraphicsscene.h"
#include "qgraphicsscene_p.h"
#include "qgraphicssceneevent.h"
#include "qgraphicswidget.h"

#include <QtCore/qdatetime.h>
#include <QtCore/qdebug.h>
#include <QtCore/qmath.h>
#include <QtGui/qapplication.h>
#include <QtGui/qdesktopwidget.h>
#include <QtGui/qevent.h>
#include <QtGui/qlayout.h>
#include <QtGui/qtransform.h>
#include <QtGui/qmatrix.h>
#include <QtGui/qpainter.h>
#include <QtGui/qscrollbar.h>
#include <QtGui/qstyleoption.h>
#ifdef Q_WS_X11
#include <private/qt_x11_p.h>
#endif

QT_BEGIN_NAMESPACE

inline int q_round_bound(qreal d) //### (int)(qreal) INT_MAX != INT_MAX for single precision
{
    if (d <= (qreal) INT_MIN)
        return INT_MIN;
    else if (d >= (qreal) INT_MAX)
        return INT_MAX;
    return d >= 0.0 ? int(d + 0.5) : int(d - int(d-1) + 0.5) + int(d-1);
}

/*!
    \internal
*/
QGraphicsViewPrivate::QGraphicsViewPrivate()
    : renderHints(QPainter::TextAntialiasing),
      dragMode(QGraphicsView::NoDrag),
      sceneInteractionAllowed(true), hasSceneRect(false),
      connectedToScene(false),
      mousePressButton(Qt::NoButton),
      identityMatrix(true),
      dirtyScroll(true),
      accelerateScrolling(true),
      leftIndent(0), topIndent(0),
      lastMouseEvent(QEvent::None, QPoint(), Qt::NoButton, 0, 0),
      useLastMouseEvent(false),
      keepLastCenterPoint(true),
      alignment(Qt::AlignCenter),
      transforming(false),
      transformationAnchor(QGraphicsView::AnchorViewCenter), resizeAnchor(QGraphicsView::NoAnchor),
      viewportUpdateMode(QGraphicsView::MinimalViewportUpdate),
      optimizationFlags(0),
      scene(0),
#ifndef QT_NO_RUBBERBAND
      rubberBanding(false),
      rubberBandSelectionMode(Qt::IntersectsItemShape),
#endif
      handScrolling(false), handScrollMotions(0), cacheMode(0),
      mustAllocateStyleOptions(false),
      mustResizeBackgroundPixmap(true),
#ifndef QT_NO_CURSOR
      hasStoredOriginalCursor(false),
#endif
      lastDragDropEvent(0),
      fullUpdatePending(true),
      dirtyRectCount(0),
      updatingLater(false),
      updateSceneSlotReimplementedChecked(false)
{
    styleOptions.reserve(QGRAPHICSVIEW_PREALLOC_STYLE_OPTIONS);
}

/*!
    \internal
*/
void QGraphicsViewPrivate::recalculateContentSize()
{
    Q_Q(QGraphicsView);

    QSize maxSize = q->maximumViewportSize();
    int width = maxSize.width();
    int height = maxSize.height();
    QRectF viewRect = matrix.mapRect(q->sceneRect());

    bool frameOnlyAround = (q->style()->styleHint(QStyle::SH_ScrollView_FrameOnlyAroundContents, 0, q));
    if (frameOnlyAround) {
        if (hbarpolicy == Qt::ScrollBarAlwaysOn)
            height -= frameWidth * 2;
        if (vbarpolicy == Qt::ScrollBarAlwaysOn)
            width -= frameWidth * 2;
    }

    // Adjust the maximum width and height of the viewport based on the width
    // of visible scroll bars.
    int scrollBarExtent = q->style()->pixelMetric(QStyle::PM_ScrollBarExtent, 0, q);
    if (frameOnlyAround)
        scrollBarExtent += frameWidth * 2;

    bool useHorizontalScrollBar = (viewRect.width() > width) && hbarpolicy != Qt::ScrollBarAlwaysOff;
    bool useVerticalScrollBar = (viewRect.height() > height) && vbarpolicy != Qt::ScrollBarAlwaysOff;
    if (useHorizontalScrollBar && !useVerticalScrollBar) {
        if (viewRect.height() > height - scrollBarExtent)
            useVerticalScrollBar = true;
    }
    if (useVerticalScrollBar && !useHorizontalScrollBar) {
        if (viewRect.width() > width - scrollBarExtent)
            useHorizontalScrollBar = true;
    }
    if (useHorizontalScrollBar && hbarpolicy != Qt::ScrollBarAlwaysOn)
        height -= scrollBarExtent;
    if (useVerticalScrollBar && vbarpolicy != Qt::ScrollBarAlwaysOn)
        width -= scrollBarExtent;

    // Setting the ranges of these scroll bars can/will cause the values to
    // change, and scrollContentsBy() will be called correspondingly. This
    // will reset the last center point.
    QPointF savedLastCenterPoint = lastCenterPoint;

    // Remember the former indent settings
    qreal oldLeftIndent = leftIndent;
    qreal oldTopIndent = topIndent;

    // If the whole scene fits horizontally, we center the scene horizontally,
    // and ignore the horizontal scroll bars.
    int left =  q_round_bound(viewRect.left());
    int right = q_round_bound(viewRect.right() - width);
    if (left >= right) {
        q->horizontalScrollBar()->setRange(0, 0);

        switch (alignment & Qt::AlignHorizontal_Mask) {
        case Qt::AlignLeft:
            leftIndent = -viewRect.left();
            break;
        case Qt::AlignRight:
            leftIndent = width - viewRect.width() - viewRect.left() - 1;
            break;
        case Qt::AlignHCenter:
        default:
            leftIndent = width / 2 - (viewRect.left() + viewRect.right()) / 2;
            break;
        }
    } else {
        q->horizontalScrollBar()->setRange(left, right);
        q->horizontalScrollBar()->setPageStep(width);
        q->horizontalScrollBar()->setSingleStep(width / 20);
        leftIndent = 0;
    }

    // If the whole scene fits vertically, we center the scene vertically, and
    // ignore the vertical scroll bars.
    int top = q_round_bound(viewRect.top());
    int bottom = q_round_bound(viewRect.bottom()  - height);
    if (top >= bottom) {
        q->verticalScrollBar()->setRange(0, 0);

        switch (alignment & Qt::AlignVertical_Mask) {
        case Qt::AlignTop:
            topIndent = -viewRect.top();
            break;
        case Qt::AlignBottom:
            topIndent = height - viewRect.height() - viewRect.top() - 1;
            break;
        case Qt::AlignVCenter:
        default:
            topIndent = height / 2 - (viewRect.top() + viewRect.bottom()) / 2;
            break;
        }
    } else {
        q->verticalScrollBar()->setRange(top, bottom);
        q->verticalScrollBar()->setPageStep(height);
        q->verticalScrollBar()->setSingleStep(height / 20);
        topIndent = 0;
    }

    // Restorethe center point from before the ranges changed.
    lastCenterPoint = savedLastCenterPoint;

    // Issue a full update if the indents change.
    // ### If the transform is still the same, we can get away with just a
    // scroll instead.
    if (oldLeftIndent != leftIndent || oldTopIndent != topIndent) {
        dirtyScroll = true;
        q->viewport()->update();
    } else if (q->isRightToLeft() && !leftIndent) {
        // In reverse mode, the horizontal scroll always changes after the content
        // size has changed, as the scroll is calculated by summing the min and
        // max values of the range and subtracting the current value. In normal
        // mode the scroll remains unchanged unless the indent has changed.
        dirtyScroll = true;
    }

    if (cacheMode & QGraphicsView::CacheBackground) {
        // Invalidate the background pixmap
        mustResizeBackgroundPixmap = true;
    }
}

/*!
    \internal
*/
void QGraphicsViewPrivate::centerView(QGraphicsView::ViewportAnchor anchor)
{
    Q_Q(QGraphicsView);
    switch (anchor) {
    case QGraphicsView::AnchorUnderMouse: {
        if (q->underMouse()) {
            // Last scene pos: lastMouseMoveScenePoint
            // Current mouse pos:
            QPointF transformationDiff = q->mapToScene(q->viewport()->rect().center())
                                         - q->mapToScene(q->mapFromGlobal(QCursor::pos()));
            q->centerOn(lastMouseMoveScenePoint + transformationDiff);;
        } else {
            q->centerOn(lastCenterPoint);
        }
        break;
    }
    case QGraphicsView::AnchorViewCenter:
        q->centerOn(lastCenterPoint);
        break;
    case QGraphicsView::NoAnchor:
        break;
    }
}

/*!
    \internal
*/
void QGraphicsViewPrivate::updateLastCenterPoint()
{
    Q_Q(QGraphicsView);
    lastCenterPoint = q->mapToScene(q->viewport()->rect().center());
}

/*!
    \internal

    Returns the horizontal scroll value (the X value of the left edge of the
    viewport).
*/
qint64 QGraphicsViewPrivate::horizontalScroll() const
{
    if (dirtyScroll)
        const_cast<QGraphicsViewPrivate *>(this)->updateScroll();
    return scrollX;
}

/*!
    \internal

    Returns the vertical scroll value (the X value of the top edge of the
    viewport).
*/
qint64 QGraphicsViewPrivate::verticalScroll() const
{
    if (dirtyScroll)
        const_cast<QGraphicsViewPrivate *>(this)->updateScroll();
    return scrollY;
}

/*!
    \internal
*/
void QGraphicsViewPrivate::updateScroll()
{
    Q_Q(QGraphicsView);
    scrollX = qint64(-leftIndent);
    if (q->isRightToLeft()) {
        if (!leftIndent) {
            scrollX += q->horizontalScrollBar()->minimum();
            scrollX += q->horizontalScrollBar()->maximum();
            scrollX -= q->horizontalScrollBar()->value();
        }
    } else {
        scrollX += q->horizontalScrollBar()->value();
    }

    scrollY = qint64(q->verticalScrollBar()->value() - topIndent);

    dirtyScroll = false;
}

/*!
    \internal
*/
void QGraphicsViewPrivate::replayLastMouseEvent()
{
    if (!useLastMouseEvent || !scene)
        return;
    mouseMoveEventHandler(&lastMouseEvent);
}

/*!
    \internal
*/
void QGraphicsViewPrivate::storeMouseEvent(QMouseEvent *event)
{
    useLastMouseEvent = true;
    lastMouseEvent = QMouseEvent(QEvent::MouseMove, event->pos(), event->globalPos(),
                                 event->button(), event->buttons(), event->modifiers());
}

void QGraphicsViewPrivate::mouseMoveEventHandler(QMouseEvent *event)
{
    Q_Q(QGraphicsView);

    storeMouseEvent(event);
    lastMouseEvent.setAccepted(false);

    if (!sceneInteractionAllowed)
        return;
    if (handScrolling)
        return;
    if (!scene)
        return;

    QGraphicsSceneMouseEvent mouseEvent(QEvent::GraphicsSceneMouseMove);
    mouseEvent.setWidget(q->viewport());
    mouseEvent.setButtonDownScenePos(mousePressButton, mousePressScenePoint);
    mouseEvent.setButtonDownScreenPos(mousePressButton, mousePressScreenPoint);
    mouseEvent.setScenePos(q->mapToScene(event->pos()));
    mouseEvent.setScreenPos(event->globalPos());
    mouseEvent.setLastScenePos(lastMouseMoveScenePoint);
    mouseEvent.setLastScreenPos(lastMouseMoveScreenPoint);
    mouseEvent.setButtons(event->buttons());
    mouseEvent.setButton(event->button());
    mouseEvent.setModifiers(event->modifiers());
    lastMouseMoveScenePoint = mouseEvent.scenePos();
    lastMouseMoveScreenPoint = mouseEvent.screenPos();
    mouseEvent.setAccepted(false);
    QApplication::sendEvent(scene, &mouseEvent);

    // Remember whether the last event was accepted or not.
    lastMouseEvent.setAccepted(mouseEvent.isAccepted());

    if (mouseEvent.isAccepted() && mouseEvent.buttons() != 0) {
        // The event was delivered to a mouse grabber; the press is likely to
        // have set a cursor, and we must not change it.
        return;
    }

#ifndef QT_NO_CURSOR
    // Find the topmost item under the mouse with a cursor.
    foreach (QGraphicsItem *item, scene->d_func()->cachedItemsUnderMouse) {
        if (item->hasCursor()) {
            _q_setViewportCursor(item->cursor());
            return;
        }
    }

    // No items with cursors found; revert to the view cursor.
    if (hasStoredOriginalCursor) {
        // Restore the original viewport cursor.
        hasStoredOriginalCursor = false;
        q->viewport()->setCursor(originalCursor);
    }
#endif
}

/*!
    \internal
*/
#ifndef QT_NO_RUBBERBAND
QRegion QGraphicsViewPrivate::rubberBandRegion(const QWidget *widget, const QRect &rect) const
{
    QStyleHintReturnMask mask;
    QStyleOptionRubberBand option;
    option.initFrom(widget);
    option.rect = rect;
    option.opaque = false;
    option.shape = QRubberBand::Rectangle;

    QRegion tmp;
    tmp += rect;
    if (widget->style()->styleHint(QStyle::SH_RubberBand_Mask, &option, widget, &mask))
        tmp &= mask.region;
    return tmp;
}
#endif

/*!
    \internal
*/
#ifndef QT_NO_CURSOR
void QGraphicsViewPrivate::_q_setViewportCursor(const QCursor &cursor)
{
    Q_Q(QGraphicsView);
    QWidget *viewport = q->viewport();
    if (!hasStoredOriginalCursor) {
        hasStoredOriginalCursor = true;
        originalCursor = viewport->cursor();
    }
    viewport->setCursor(cursor);
}
#endif

/*!
    \internal
*/
#ifndef QT_NO_CURSOR
void QGraphicsViewPrivate::_q_unsetViewportCursor()
{
    Q_Q(QGraphicsView);
    foreach (QGraphicsItem *item, q->items(lastMouseEvent.pos())) {
        if (item->hasCursor()) {
            _q_setViewportCursor(item->cursor());
            return;
        }
    }

    // Restore the original viewport cursor.
    hasStoredOriginalCursor = false;
    if (dragMode == QGraphicsView::ScrollHandDrag)
        q->viewport()->setCursor(Qt::OpenHandCursor);
    else
        q->viewport()->setCursor(originalCursor);
}
#endif

/*!
    \internal
*/
void QGraphicsViewPrivate::storeDragDropEvent(const QGraphicsSceneDragDropEvent *event)
{
    delete lastDragDropEvent;
    lastDragDropEvent = new QGraphicsSceneDragDropEvent(event->type());
    lastDragDropEvent->setScenePos(event->scenePos());
    lastDragDropEvent->setScreenPos(event->screenPos());
    lastDragDropEvent->setButtons(event->buttons());
    lastDragDropEvent->setModifiers(event->modifiers());
    lastDragDropEvent->setPossibleActions(event->possibleActions());
    lastDragDropEvent->setProposedAction(event->proposedAction());
    lastDragDropEvent->setDropAction(event->dropAction());
    lastDragDropEvent->setMimeData(event->mimeData());
    lastDragDropEvent->setWidget(event->widget());
    lastDragDropEvent->setSource(event->source());
}

/*!
    \internal
*/
void QGraphicsViewPrivate::populateSceneDragDropEvent(QGraphicsSceneDragDropEvent *dest,
                                                      QDropEvent *source)
{
#ifndef QT_NO_DRAGANDDROP
    Q_Q(QGraphicsView);
    dest->setScenePos(q->mapToScene(source->pos()));
    dest->setScreenPos(q->mapToGlobal(source->pos()));
    dest->setButtons(source->mouseButtons());
    dest->setModifiers(source->keyboardModifiers());
    dest->setPossibleActions(source->possibleActions());
    dest->setProposedAction(source->proposedAction());
    dest->setDropAction(source->dropAction());
    dest->setMimeData(source->mimeData());
    dest->setWidget(q->viewport());
    dest->setSource(source->source());
#else
    Q_UNUSED(dest)
    Q_UNUSED(source)
#endif
}

/*!
    \internal
*/
QRect QGraphicsViewPrivate::mapToViewRect(const QGraphicsItem *item, const QRectF &rect) const
{
    Q_Q(const QGraphicsView);
    if (dirtyScroll)
        const_cast<QGraphicsViewPrivate *>(this)->updateScroll();

    if (item->d_ptr->itemIsUntransformable()) {
        QTransform itv = item->deviceTransform(q->viewportTransform());
        return itv.mapRect(rect).toAlignedRect();
    }

    // Translate-only
    QPointF offset;
    const QGraphicsItem *parentItem = item;
    const QGraphicsItemPrivate *itemd;
    do {
        itemd = parentItem->d_ptr;
        if (itemd->hasTransform)
            break;
        offset += itemd->pos;
    } while ((parentItem = itemd->parent));

    QRectF baseRect = rect.translated(offset.x(), offset.y());
    if (!parentItem) {
        if (identityMatrix) {
            baseRect.translate(-scrollX, -scrollY);
            return baseRect.toAlignedRect();
        }
        return matrix.mapRect(baseRect).translated(-scrollX, -scrollY).toAlignedRect();
    }

    QTransform tr = parentItem->sceneTransform();
    if (!identityMatrix)
        tr *= matrix;
    QRectF r = tr.mapRect(baseRect);
    r.translate(-scrollX, -scrollY);
    return r.toAlignedRect();
}

/*!
    \internal
*/
QRegion QGraphicsViewPrivate::mapToViewRegion(const QGraphicsItem *item, const QRectF &rect) const
{
    Q_Q(const QGraphicsView);
    if (dirtyScroll)
        const_cast<QGraphicsViewPrivate *>(this)->updateScroll();

    // Accurate bounding region
    QTransform itv = item->sceneTransform() * q->viewportTransform();
    return item->boundingRegion(itv) & itv.mapRect(rect).toAlignedRect();
}

/*!
    \internal
*/
void QGraphicsViewPrivate::itemUpdated(QGraphicsItem *item, const QRectF &rect)
{
    if (fullUpdatePending || viewportUpdateMode == QGraphicsView::NoViewportUpdate)
        return;
    if (item->d_ptr->dirty)
        updateLater();

    QRectF updateRect = rect;
    if (item->isClipped()) {
        // Minimize unnecessary redraw.
        QGraphicsItem *p = item;
        while ((p = p->d_ptr->parent)) {
            if (p->flags() & QGraphicsItem::ItemClipsChildrenToShape) {
                updateRect &= p->itemTransform(item).mapRect(p->boundingRect());
                if (updateRect.isNull())
                    return;
            }

            if (!(p->d_ptr->ancestorFlags & QGraphicsItemPrivate::AncestorClipsChildren))
                break;
        }

        if (updateRect.isNull())
            return;
    }

    // Map the rect to view coordinates.
    QRect vr = viewport->rect();

    if (!item->d_ptr->hasBoundingRegionGranularity) {
        QRect r = mapToViewRect(item, updateRect) & vr;
        if (r.isNull())
            return;
        this->updateRect(r);
    } else {
        QRegion r = mapToViewRegion(item, updateRect) & vr;
        if (r.isEmpty())
            return;
        updateRegion(r);
    }
}

void QGraphicsViewPrivate::updateLater()
{
    Q_Q(QGraphicsView);
    if (updatingLater)
        return;
    updatingLater = true;
    QMetaObject::invokeMethod(q, "_q_updateLaterSlot", Qt::QueuedConnection);
}

void QGraphicsViewPrivate::_q_updateLaterSlot()
{
    Q_Q(QGraphicsView);
    if (!scene)
        return;

    QRect vr = viewport->rect();
    QTransform viewTransform = q->viewportTransform();
    const QList<QGraphicsItem *> &dirtyItems = scene->d_func()->dirtyItems;
    for (int i = 0; i < dirtyItems.size(); ++i) {
        const QGraphicsItem *item = dirtyItems.at(i);
        QTransform x = item->sceneTransform() * viewTransform;
        QRect viewRect = x.mapRect(item->boundingRect()).toAlignedRect() & vr;
        if (!viewRect.isNull())
            updateRect(viewRect);
    }

    dirtyRectCount += dirtyRects.size();

    bool noUpdate = !fullUpdatePending && viewportUpdateMode == QGraphicsView::FullViewportUpdate;
    if ((dirtyRectCount > 0 || !dirtyBoundingRect.isNull()) && !fullUpdatePending && !noUpdate) {
        if (viewportUpdateMode == QGraphicsView::BoundingRectViewportUpdate
            || (viewportUpdateMode == QGraphicsView::SmartViewportUpdate
                && dirtyRectCount >= QGRAPHICSVIEW_REGION_RECT_THRESHOLD)) {
            if (!(optimizationFlags & QGraphicsView::DontAdjustForAntialiasing)) {
                viewport->update(dirtyBoundingRect.adjusted(-2, -2, 2, 2));
            } else {
                viewport->update(dirtyBoundingRect);
            }
        } else {
            // ### Improve this block, which is very slow for complex regions. We
            // need to strike the balance between having an accurate update
            // region, and running fast. The below approach is the simplest way to
            // create a region from a bunch of rects, but we might want to use
            // other approaches; e.g., a grid of a fixed size representing
            // quadrants of the viewport, which we mark as dirty depending on the
            // rectangles in the list. Perhaps this should go into a
            // QRegion::fromRects(rects, how) function.
            QRegion region;
            if (!(optimizationFlags & QGraphicsView::DontAdjustForAntialiasing)) {
                for (int i = 0; i < dirtyRegions.size(); ++i) {
                    QVector<QRect> rects = dirtyRegions.at(i).rects();
                    for (int j = 0; j < rects.size(); ++j)
                        region += rects.at(j).adjusted(-2, -2, 2, 2);
                }
                for (int i = 0; i < dirtyRects.size(); ++i)
                    region += dirtyRects.at(i).adjusted(-2, -2, 2, 2);
            } else {
                for (int i = 0; i < dirtyRegions.size(); ++i)
                    region += dirtyRegions.at(i);
                for (int i = 0; i < dirtyRects.size(); ++i)
                    region += dirtyRects.at(i);
            }

            viewport->update(region);
        }
    }

    dirtyRegions.clear();
    dirtyRects.clear();
    dirtyRectCount = 0;
    dirtyBoundingRect = QRect();
    updatingLater = false;
}

void QGraphicsViewPrivate::updateAll()
{
    Q_Q(QGraphicsView);
    q->viewport()->update();
    fullUpdatePending = true;
    dirtyRectCount = 0;
    dirtyBoundingRect = QRect();
    updatingLater = false;
}

void QGraphicsViewPrivate::updateRegion(const QRegion &r)
{
    Q_Q(QGraphicsView);

    // Rect intersects viewport - update everything?
    switch (viewportUpdateMode) {
    case QGraphicsView::FullViewportUpdate:
        fullUpdatePending = true;
        q->viewport()->update();
        break;
    case QGraphicsView::BoundingRectViewportUpdate:
        dirtyBoundingRect |= r.boundingRect();
        if (dirtyBoundingRect == q->viewport()->rect()) {
            fullUpdatePending = true;
            q->viewport()->update();
        } else {
            updateLater();
        }
        break;
    case QGraphicsView::SmartViewportUpdate:
        dirtyBoundingRect |= r.boundingRect();
        if ((dirtyRectCount + r.numRects()) < QGRAPHICSVIEW_REGION_RECT_THRESHOLD)
            dirtyRegions << r;
        dirtyRectCount += r.numRects();
        updateLater();
        break;
    case QGraphicsView::MinimalViewportUpdate:
        dirtyRegions << r;
        dirtyRectCount += r.numRects();
        updateLater();
        break;
    case QGraphicsView::NoViewportUpdate:
        // Unreachable
        break;
    }

    // Compress the regions...
    if (dirtyRectCount > QGRAPHICSVIEW_REGION_RECT_THRESHOLD && dirtyRegions.size() > 1) {
        QRegion masterRegion;
        for (int i=0; i<dirtyRegions.size(); ++i) {
            masterRegion |= dirtyRegions.at(i);
        }
        dirtyRectCount = masterRegion.numRects();
        dirtyRegions.clear();
        dirtyRegions << masterRegion;
    }
}

void QGraphicsViewPrivate::updateRect(const QRect &r)
{
    Q_Q(QGraphicsView);

    // Rect intersects viewport - update everything?
    switch (viewportUpdateMode) {
    case QGraphicsView::FullViewportUpdate:
        fullUpdatePending = true;
        q->viewport()->update();
        break;
    case QGraphicsView::BoundingRectViewportUpdate:
        dirtyBoundingRect |= r;
        if (dirtyBoundingRect == q->viewport()->rect()) {
            fullUpdatePending = true;
            q->viewport()->update();
        } else {
            updateLater();
        }
        break;
    case QGraphicsView::SmartViewportUpdate:
        dirtyBoundingRect |= r;
        if ((dirtyRectCount + dirtyRects.size()) < QGRAPHICSVIEW_REGION_RECT_THRESHOLD)
            dirtyRects << r;
        updateLater();
        break;
    case QGraphicsView::MinimalViewportUpdate:
        dirtyRects << r;
        updateLater();
        break;
    case QGraphicsView::NoViewportUpdate:
        // Unreachable
        break;
    }
}

QStyleOptionGraphicsItem *QGraphicsViewPrivate::allocStyleOptionsArray(int numItems)
{
    if (mustAllocateStyleOptions || (numItems > styleOptions.capacity()))
        // too many items, let's allocate on-the-fly
        return new QStyleOptionGraphicsItem[numItems];

    // expand only whenever necessary
    if (numItems > styleOptions.size())
        styleOptions.resize(numItems);

    mustAllocateStyleOptions = true;
    return styleOptions.data();
}

void QGraphicsViewPrivate::freeStyleOptionsArray(QStyleOptionGraphicsItem *array)
{
    mustAllocateStyleOptions = false;
    if (array != styleOptions.data())
        delete [] array;
}


QList<QGraphicsItem *> QGraphicsViewPrivate::findItems(const QRegion &exposedRegion,
                                                       const QTransform &worldTransform,
                                                       bool *allItems) const
{
    Q_Q(const QGraphicsView);
    QList<QGraphicsItem *> itemList;
    QSet<QGraphicsItem *> tmp;
    bool simpleTransform = worldTransform.type() <= QTransform::TxScale;

    QPainterPath path;
    path.addRegion(exposedRegion);
    *allItems = path.contains(q->mapFromScene(scene->d_func()->growingItemsBoundingRect).boundingRect());
    QList<QRectF> exposedRects;
    QList<QPolygonF> exposedPolys;

    // Transform the exposed viewport rects to scene rects or polygons
    foreach (const QRect &rect, exposedRegion.rects()) {
        QPolygonF exposedPoly = q->mapToScene(rect.adjusted(-1, -1, 1, 1));
        QRectF exposedRect = exposedPoly.boundingRect();
        if (!simpleTransform)
            exposedPolys << exposedPoly;
        exposedRects << exposedRect;
    }

    // Find which items need to be drawn.
    if (*allItems) {
        // All items are guaranteed within the exposed region, don't bother using the index.
        foreach (QGraphicsItem *item, scene->items()) {
            // But we only want to include items that are visible
            if (item->isVisible())
                itemList << item;
        }
    } else if (simpleTransform) {
        // Simple rect lookups will do.
        if (exposedRects.size() > 1) {
            foreach (const QRectF &rect, exposedRects) {
                foreach (QGraphicsItem *item, scene->d_func()->items_helper(rect, Qt::IntersectsItemBoundingRect, Qt::SortOrder(-1) /* don't sort */)) {
                    if (!tmp.contains(item)) {
                        tmp << item;
                        itemList << item;
                    }
                }
            }
        } else {
            itemList += scene->d_func()->items_helper(exposedRects[0], Qt::IntersectsItemBoundingRect, Qt::SortOrder(-1) /* don't sort */);
        }
    } else {
        // Polygon lookup is necessary.
        if (exposedRects.size() > 1) {
            foreach (const QPolygonF &poly, exposedPolys) {
                foreach (QGraphicsItem *item, scene->d_func()->items_helper(poly, Qt::IntersectsItemBoundingRect, Qt::SortOrder(-1) /* don't sort */)) {
                    if (!tmp.contains(item)) {
                        tmp << item;
                        itemList << item;
                    }
                }
            }
        } else {
            itemList += scene->d_func()->items_helper(exposedPolys[0], Qt::IntersectsItemBoundingRect, Qt::SortOrder(-1) /* don't sort */);
        }
    }

    // Check for items that ignore inherited transformations, and add them if
    // necessary.
    QRectF untr = scene->d_func()->largestUntransformableItem;
    if (!*allItems && !untr.isNull()) {
        // Map the largest untransformable item subtree boundingrect from view
        // to scene coordinates, and use this to expand all exposed rects in
        // search for untransformable items.
        QRectF ltri = matrix.inverted().mapRect(untr);
        ltri.adjust(-untr.width(), -untr.height(), untr.width(), untr.height());

        foreach (const QRect &rect, exposedRegion.rects()) {
            QRectF exposed = q->mapToScene(rect.adjusted(-1, -1, 1, 1)).boundingRect();
            exposed.adjust(-ltri.width(), -ltri.height(), ltri.width(), ltri.height());

            foreach (QGraphicsItem *item, scene->d_func()->estimateItemsInRect(exposed)) {
                if (item->d_ptr->itemIsUntransformable()) {
                    if (!tmp.contains(item)) {
                        QPainterPath rectPath;
                        rectPath.addRect(rect);
                        QPainterPath path = item->deviceTransform(q->viewportTransform()).inverted().map(rectPath);
                        if (item->collidesWithPath(path, Qt::IntersectsItemBoundingRect)) {
                            itemList << item;
                            tmp << item;
                        }
                    }
                }
            }
        }
    }
    tmp.clear();

    // Sort the items.
    QGraphicsScenePrivate::sortItems(&itemList, Qt::DescendingOrder,
                                     scene->d_func()->sortCacheEnabled);

    return itemList;
}

void QGraphicsViewPrivate::generateStyleOptions(const QList<QGraphicsItem *> &itemList,
                                                QGraphicsItem **itemArray,
                                                QStyleOptionGraphicsItem *styleOptionArray,
                                                const QTransform &worldTransform,
                                                bool allItems,
                                                const QRegion &exposedRegion) const
{
    // Two unit vectors.
    QLineF v1(0, 0, 1, 0);
    QLineF v2(0, 0, 0, 1);
    QTransform itemToViewportTransform;
    QRectF brect;
    QTransform reverseMap;

    for (int i = 0; i < itemList.size(); ++i) {
        QGraphicsItem *item = itemArray[i] = itemList[i];

        QStyleOptionGraphicsItem &option = styleOptionArray[i];
        brect = item->boundingRect();
        option.state = QStyle::State_None;
        option.rect = brect.toRect();
        option.exposedRect = QRectF();
        if (item->d_ptr->selected)
            option.state |= QStyle::State_Selected;
        if (item->d_ptr->enabled)
            option.state |= QStyle::State_Enabled;
        if (item->hasFocus())
            option.state |= QStyle::State_HasFocus;
        if (scene->d_func()->hoverItems.contains(item))
            option.state |= QStyle::State_MouseOver;
        if (item == scene->mouseGrabberItem())
            option.state |= QStyle::State_Sunken;

        // Calculate a simple level-of-detail metric.
        // ### almost identical code in QGraphicsScene::render()
        //     and QGraphicsView::render() - consider refactoring
        if (item->d_ptr->itemIsUntransformable()) {
            itemToViewportTransform = item->deviceTransform(worldTransform);
        } else {
            itemToViewportTransform = item->sceneTransform() * worldTransform;
        }

        if (itemToViewportTransform.type() <= QTransform::TxTranslate) {
            // Translation and rotation only? The LOD is 1.
            option.levelOfDetail = 1;
        } else {
            // LOD is the transformed area of a 1x1 rectangle.
            option.levelOfDetail = qSqrt(itemToViewportTransform.map(v1).length() * itemToViewportTransform.map(v2).length());
        }
        option.matrix = itemToViewportTransform.toAffine(); //### discards perspective

        if (!allItems) {
            // Determine the item's exposed area
            reverseMap = itemToViewportTransform.inverted();
            foreach (const QRect &rect, exposedRegion.rects()) {
                option.exposedRect |= reverseMap.mapRect(QRectF(rect.adjusted(-1, -1, 1, 1)));
                if (option.exposedRect.contains(brect))
                    break;
            }
            option.exposedRect &= brect;
        } else {
            // The whole item is exposed
            option.exposedRect = brect;
        }
    }
}

/*!
    Constructs a QGraphicsView. \a parent is passed to QWidget's constructor.
*/
QGraphicsView::QGraphicsView(QWidget *parent)
    : QAbstractScrollArea(*new QGraphicsViewPrivate, parent)
{
    setViewport(0);
    setAcceptDrops(true);
    setBackgroundRole(QPalette::Base);

    // ### Ideally this would be enabled/disabled depending on whether any
    // widgets in the current scene enabled input methods. We could do that
    // using a simple reference count. The same goes for acceptDrops and mouse
    // tracking.
    setAttribute(Qt::WA_InputMethodEnabled);
}

/*!
    Constructs a QGraphicsView and sets the visualized scene to \a
    scene. \a parent is passed to QWidget's constructor.
*/
QGraphicsView::QGraphicsView(QGraphicsScene *scene, QWidget *parent)
    : QAbstractScrollArea(*new QGraphicsViewPrivate, parent)
{
    setScene(scene);
    setViewport(0);
    setAcceptDrops(true);
    setBackgroundRole(QPalette::Base);
    setAttribute(Qt::WA_InputMethodEnabled);
}

/*!
  \internal
 */
QGraphicsView::QGraphicsView(QGraphicsViewPrivate &dd, QWidget *parent)
  : QAbstractScrollArea(dd, parent)
{
    setViewport(0);
    setAcceptDrops(true);
    setBackgroundRole(QPalette::Base);
    setAttribute(Qt::WA_InputMethodEnabled);
}

/*!
    Destructs the QGraphicsView object.
*/
QGraphicsView::~QGraphicsView()
{
    Q_D(QGraphicsView);
    if (d->scene)
        d->scene->d_func()->views.removeAll(this);
    delete d->lastDragDropEvent;
}

/*!
    \reimp
*/
QSize QGraphicsView::sizeHint() const
{
    Q_D(const QGraphicsView);
    if (d->scene) {
        QSizeF baseSize = d->matrix.mapRect(sceneRect()).size();
        baseSize += QSizeF(d->frameWidth * 2, d->frameWidth * 2);
        return baseSize.boundedTo((3 * QApplication::desktop()->size()) / 4).toSize();
    }
    return QAbstractScrollArea::sizeHint();
}

/*!
    \property QGraphicsView::renderHints
    \brief the default render hints for the view

    These hints are
    used to initialize QPainter before each visible item is drawn. QPainter
    uses render hints to toggle rendering features such as antialiasing and
    smooth pixmap transformation.

    QPainter::TextAntialiasing is enabled by default.

    Example:

    \snippet doc/src/snippets/code/src_gui_graphicsview_qgraphicsview.cpp 1
*/
QPainter::RenderHints QGraphicsView::renderHints() const
{
    Q_D(const QGraphicsView);
    return d->renderHints;
}
void QGraphicsView::setRenderHints(QPainter::RenderHints hints)
{
    Q_D(QGraphicsView);
    if (hints == d->renderHints)
        return;
    d->renderHints = hints;
    viewport()->update();
}

/*!
    If \a enabled is true, the render hint \a hint is enabled; otherwise it
    is disabled.

    \sa renderHints
*/
void QGraphicsView::setRenderHint(QPainter::RenderHint hint, bool enabled)
{
    Q_D(QGraphicsView);
    QPainter::RenderHints oldHints = d->renderHints;
    if (enabled)
        d->renderHints |= hint;
    else
        d->renderHints &= ~hint;
    if (oldHints != d->renderHints)
        viewport()->update();
}

/*!
    \property QGraphicsView::alignment
    \brief the alignment of the scene in the view when the whole
    scene is visible.

    If the whole scene is visible in the view, (i.e., there are no visible
    scroll bars,) the view's alignment will decide where the scene will be
    rendered in the view. For example, if the alignment is Qt::AlignCenter,
    which is default, the scene will be centered in the view, and if the
    alignment is (Qt::AlignLeft | Qt::AlignTop), the scene will be rendered in
    the top-left corner of the view.
*/
Qt::Alignment QGraphicsView::alignment() const
{
    Q_D(const QGraphicsView);
    return d->alignment;
}
void QGraphicsView::setAlignment(Qt::Alignment alignment)
{
    Q_D(QGraphicsView);
    if (d->alignment != alignment) {
        d->alignment = alignment;
        d->recalculateContentSize();
    }
}

/*!
    \property QGraphicsView::transformationAnchor
    \brief how the view should position the scene during transformations.

    QGraphicsView uses this property to decide how to position the scene in
    the viewport when the transformation matrix changes, and the coordinate
    system of the view is transformed. The default behavior, AnchorViewCenter,
    ensures that the scene point at the center of the view remains unchanged
    during transformations (e.g., when rotating, the scene will appear to
    rotate around the center of the view).

    Note that the effect of this property is noticeable when only a part of the
    scene is visible (i.e., when there are scroll bars). Otherwise, if the
    whole scene fits in the view, QGraphicsScene uses the view \l alignment to
    position the scene in the view.

    \sa alignment, resizeAnchor
*/
QGraphicsView::ViewportAnchor QGraphicsView::transformationAnchor() const
{
    Q_D(const QGraphicsView);
    return d->transformationAnchor;
}
void QGraphicsView::setTransformationAnchor(ViewportAnchor anchor)
{
    Q_D(QGraphicsView);
    d->transformationAnchor = anchor;
}

/*!
    \property QGraphicsView::resizeAnchor
    \brief how the view should position the scene when the view is resized.

    QGraphicsView uses this property to decide how to position the scene in
    the viewport when the viewport widget's size changes. The default
    behavior, NoAnchor, leaves the scene's position unchanged during a resize;
    the top-left corner of the view will appear to be anchored while resizing.

    Note that the effect of this property is noticeable when only a part of the
    scene is visible (i.e., when there are scroll bars). Otherwise, if the
    whole scene fits in the view, QGraphicsScene uses the view \l alignment to
    position the scene in the view.

    \sa alignment, transformationAnchor, Qt::WNorthWestGravity
*/
QGraphicsView::ViewportAnchor QGraphicsView::resizeAnchor() const
{
    Q_D(const QGraphicsView);
    return d->resizeAnchor;
}
void QGraphicsView::setResizeAnchor(ViewportAnchor anchor)
{
    Q_D(QGraphicsView);
    d->resizeAnchor = anchor;
}

/*!
    \property QGraphicsView::viewportUpdateMode
    \brief how the viewport should update its contents.

    \since 4.3

    QGraphicsView uses this property to decide how to update areas of the
    scene that have been reexposed or changed. Usually you do not need to
    modify this property, but there are some cases where doing so can improve
    rendering performance. See the ViewportUpdateMode documentation for
    specific details.

    The default value is MinimalViewportUpdate, where QGraphicsView will
    update as small an area of the viewport as possible when the contents
    change.

    \sa ViewportUpdateMode, cacheMode
*/
QGraphicsView::ViewportUpdateMode QGraphicsView::viewportUpdateMode() const
{
    Q_D(const QGraphicsView);
    return d->viewportUpdateMode;
}
void QGraphicsView::setViewportUpdateMode(ViewportUpdateMode mode)
{
    Q_D(QGraphicsView);
    d->viewportUpdateMode = mode;
}

/*!
    \property QGraphicsView::optimizationFlags
    \brief flags that can be used to tune QGraphicsView's performance.

    \since 4.3

    QGraphicsView uses clipping, extra bounding rect adjustments, and certain
    other aids to improve rendering quality and performance for the common
    case graphics scene. However, depending on the target platform, the scene,
    and the viewport in use, some of these operations can degrade performance.

    The effect varies from flag to flag; see the OptimizationFlags
    documentation for details.

    By default, no optimization flags are enabled.

    \sa setOptimizationFlag()
*/
QGraphicsView::OptimizationFlags QGraphicsView::optimizationFlags() const
{
    Q_D(const QGraphicsView);
    return d->optimizationFlags;
}
void QGraphicsView::setOptimizationFlags(OptimizationFlags flags)
{
    Q_D(QGraphicsView);
    d->optimizationFlags = flags;
}

/*!
    Enables \a flag if \a enabled is true; otherwise disables \a flag.

    \sa optimizationFlags
*/
void QGraphicsView::setOptimizationFlag(OptimizationFlag flag, bool enabled)
{
    Q_D(QGraphicsView);
    if (enabled)
        d->optimizationFlags |= flag;
    else
        d->optimizationFlags &= ~flag;
}

/*!
    \property QGraphicsView::dragMode
    \brief the behavior for dragging the mouse over the scene while
    the left mouse button is pressed.

    This property defines what should happen when the user clicks on the scene
    background and drags the mouse (e.g., scrolling the viewport contents
    using a pointing hand cursor, or selecting multiple items with a rubber
    band). The default value, NoDrag, does nothing.

    This behavior only affects mouse clicks that are not handled by any item.
    You can define a custom behavior by creating a subclass of QGraphicsView
    and reimplementing mouseMoveEvent().
*/
QGraphicsView::DragMode QGraphicsView::dragMode() const
{
    Q_D(const QGraphicsView);
    return d->dragMode;
}
void QGraphicsView::setDragMode(DragMode mode)
{
    Q_D(QGraphicsView);
    if (d->dragMode == mode)
        return;

#ifndef QT_NO_CURSOR
    if (d->dragMode == ScrollHandDrag)
        viewport()->unsetCursor();
#endif

    // If dragMode is unset while dragging, e.g. via a keyEvent, we
    // don't unset the handScrolling state. When enabling scrolling
    // again the mouseMoveEvent will automatically start scrolling,
    // without a mousePress
    if (d->dragMode == ScrollHandDrag && mode == NoDrag && d->handScrolling)
        d->handScrolling = false;

    d->dragMode = mode;

#ifndef QT_NO_CURSOR
    if (d->dragMode == ScrollHandDrag) {
        // Forget the stored viewport cursor when we enter scroll hand drag mode.
        d->hasStoredOriginalCursor = false;
        viewport()->setCursor(Qt::OpenHandCursor);
    }
#endif
}

#ifndef QT_NO_RUBBERBAND
/*!
    \property QGraphicsView::rubberBandSelectionMode
    \brief the behavior for selecting items with a rubber band selection rectangle.
    \since 4.3

    This property defines how items are selected when using the RubberBandDrag
    drag mode.

    The default value is Qt::IntersectsItemShape; all items whose shape
    intersects with or is contained by the rubber band are selected.

    \sa dragMode, items()
*/
Qt::ItemSelectionMode QGraphicsView::rubberBandSelectionMode() const
{
    Q_D(const QGraphicsView);
    return d->rubberBandSelectionMode;
}
void QGraphicsView::setRubberBandSelectionMode(Qt::ItemSelectionMode mode)
{
    Q_D(QGraphicsView);
    d->rubberBandSelectionMode = mode;
}
#endif

/*!
    \property QGraphicsView::cacheMode
    \brief which parts of the view are cached

    QGraphicsView can cache pre-rendered content in a QPixmap, which is then
    drawn onto the viewport. The purpose of such caching is to speed up the
    total rendering time for areas that are slow to render.  Texture, gradient
    and alpha blended backgrounds, for example, can be notibly slow to render;
    especially with a transformed view. The CacheBackground flag enables
    caching of the view's background. For example:

    \snippet doc/src/snippets/code/src_gui_graphicsview_qgraphicsview.cpp 2

    The cache is invalidated every time the view is transformed. However, when
    scrolling, only partial invalidation is required.

    By default, nothing is cached.

    \sa resetCachedContent(), QPixmapCache
*/
QGraphicsView::CacheMode QGraphicsView::cacheMode() const
{
    Q_D(const QGraphicsView);
    return d->cacheMode;
}
void QGraphicsView::setCacheMode(CacheMode mode)
{
    Q_D(QGraphicsView);
    if (mode == d->cacheMode)
        return;
    d->cacheMode = mode;
    resetCachedContent();
}

/*!
    Resets any cached content. Calling this function will clear
    QGraphicsView's cache. If the current cache mode is \l CacheNone, this
    function does nothing.

    This function is called automatically for you when the backgroundBrush or
    QGraphicsScene::backgroundBrush properties change; you only need to call
    this function if you have reimplemented QGraphicsScene::drawBackground()
    or QGraphicsView::drawBackground() to draw a custom background, and need
    to trigger a full redraw.

    \sa cacheMode()
*/
void QGraphicsView::resetCachedContent()
{
    Q_D(QGraphicsView);
    if (d->cacheMode == CacheNone)
        return;

    if (d->cacheMode & CacheBackground) {
        // Background caching is enabled.
        d->mustResizeBackgroundPixmap = true;
        viewport()->update();
    } else if (d->mustResizeBackgroundPixmap) {
        // Background caching is disabled.
        // Cleanup, free some resources.
        d->mustResizeBackgroundPixmap = false;
        d->backgroundPixmap = QPixmap();
        d->backgroundPixmapExposed = QRegion();
    }
}

/*!
    Invalidates and schedules a redraw of \a layers inside \a rect. \a rect is
    in scene coordinates. Any cached content for \a layers inside \a rect is
    unconditionally invalidated and redrawn.

    You can call this function to notify QGraphicsView of changes to the
    background or the foreground of the scene. It is commonly used for scenes
    with tile-based backgrounds to notify changes when QGraphicsView has
    enabled background caching.

    Note that QGraphicsView currently supports background caching only (see
    QGraphicsView::CacheBackground). This function is equivalent to calling update() if any
    layer but QGraphicsScene::BackgroundLayer is passed.

    \sa QGraphicsScene::invalidate(), update()
*/
void QGraphicsView::invalidateScene(const QRectF &rect, QGraphicsScene::SceneLayers layers)
{
    Q_D(QGraphicsView);
    if ((layers & QGraphicsScene::BackgroundLayer) && !d->mustResizeBackgroundPixmap) {
        QRect viewRect = mapFromScene(rect).boundingRect();
        if (viewport()->rect().intersects(viewRect)) {
            // The updated background area is exposed; schedule this area for
            // redrawing.
            d->backgroundPixmapExposed += viewRect;
            if (d->scene)
                d->scene->update(rect);
        }
    }
}

/*!
    \property QGraphicsView::interactive
    \brief whether the view allowed scene interaction.

    If enabled, this view is set to allow scene interaction. Otherwise, this
    view will not allow interaction, and any mouse or key events are ignored
    (i.e., it will act as a read-only view).

    By default, this property is true.
*/
bool QGraphicsView::isInteractive() const
{
    Q_D(const QGraphicsView);
    return d->sceneInteractionAllowed;
}
void QGraphicsView::setInteractive(bool allowed)
{
    Q_D(QGraphicsView);
    d->sceneInteractionAllowed = allowed;
}

/*!
    Returns a pointer to the scene that is currently visualized in the
    view. If no scene is currently visualized, 0 is returned.

    \sa setScene()
*/
QGraphicsScene *QGraphicsView::scene() const
{
    Q_D(const QGraphicsView);
    return d->scene;
}

/*!
    Sets the current scene to \a scene. If \a scene is already being
    viewed, this function does nothing.

    When a scene is set on a view, the QGraphicsScene::changed() signal
    is automatically connected to this view's updateScene() slot, and the
    view's scroll bars are adjusted to fit the size of the scene.
*/
void QGraphicsView::setScene(QGraphicsScene *scene)
{
    Q_D(QGraphicsView);
    if (d->scene == scene)
        return;

    // Always update the viewport when the scene changes.
    viewport()->update();

    // Remove the previously assigned scene.
    if (d->scene) {
        disconnect(d->scene, SIGNAL(changed(QList<QRectF>)),
                   this, SLOT(updateScene(QList<QRectF>)));
        disconnect(d->scene, SIGNAL(sceneRectChanged(QRectF)),
                   this, SLOT(updateSceneRect(QRectF)));
        d->scene->d_func()->views.removeAll(this);
    }

    // Assign the new scene and update the contents (scrollbars, etc.)).
    if ((d->scene = scene)) {
        connect(d->scene, SIGNAL(sceneRectChanged(QRectF)),
                this, SLOT(updateSceneRect(QRectF)));
        d->updateSceneSlotReimplementedChecked = false;
        d->scene->d_func()->views << this;
        d->recalculateContentSize();
        d->lastCenterPoint = sceneRect().center();
        d->keepLastCenterPoint = true;
    } else {
        d->recalculateContentSize();
    }
}

/*!
    \property QGraphicsView::sceneRect
    \brief the area of the scene visualized by this view.

    The scene rectangle defines the extent of the scene, and in the view's case,
    this means the area of the scene that you can navigate using the scroll
    bars.

    If unset, or if a null QRectF is set, this property has the same value as
    QGraphicsScene::sceneRect, and it changes with
    QGraphicsScene::sceneRect. Otherwise, the view's scene rect is unaffected
    by the scene.

    Note that, although the scene supports a virtually unlimited size, the
    range of the scroll bars will never exceed the range of an integer
    (INT_MIN, INT_MAX). When the scene is larger than the scroll bars' values,
    you can choose to use translate() to navigate the scene instead.

    By default, this property contains a rectangle at the origin with zero
    width and height.

    \sa QGraphicsScene::sceneRect
*/
QRectF QGraphicsView::sceneRect() const
{
    Q_D(const QGraphicsView);
    if (d->hasSceneRect)
        return d->sceneRect;
    if (d->scene)
        return d->scene->sceneRect();
    return QRectF();
}
void QGraphicsView::setSceneRect(const QRectF &rect)
{
    Q_D(QGraphicsView);
    d->hasSceneRect = !rect.isNull();
    d->sceneRect = rect;
    d->recalculateContentSize();
}

/*!
    Returns the current transformation matrix for the view. If no current
    transformation is set, the identity matrix is returned.

    \sa setMatrix(), rotate(), scale(), shear(), translate()
*/
QMatrix QGraphicsView::matrix() const
{
    Q_D(const QGraphicsView);
    return d->matrix.toAffine();
}

/*!
    Sets the view's current transformation matrix to \a matrix.

    If \a combine is true, then \a matrix is combined with the current matrix;
    otherwise, \a matrix \e replaces the current matrix. \a combine is false
    by default.

    The transformation matrix tranforms the scene into view coordinates. Using
    the default transformation, provided by the identity matrix, one pixel in
    the view represents one unit in the scene (e.g., a 10x10 rectangular item
    is drawn using 10x10 pixels in the view). If a 2x2 scaling matrix is
    applied, the scene will be drawn in 1:2 (e.g., a 10x10 rectangular item is
    then drawn using 20x20 pixels in the view).

    Example:

    \snippet doc/src/snippets/code/src_gui_graphicsview_qgraphicsview.cpp 3

    To simplify interation with items using a transformed view, QGraphicsView
    provides mapTo... and mapFrom... functions that can translate between
    scene and view coordinates. For example, you can call mapToScene() to map
    a view coordinate to a floating point scene coordinate, or mapFromScene()
    to map from floating point scene coordinates to view coordinates.

    \sa matrix(), rotate(), scale(), shear(), translate()
*/
void QGraphicsView::setMatrix(const QMatrix &matrix, bool combine)
{
    setTransform(QTransform(matrix), combine);
}

/*!
    Resets the view transformation matrix to the identity matrix.
*/
void QGraphicsView::resetMatrix()
{
    resetTransform();
}

/*!
    Rotates the current view transformation \a angle degrees clockwise.

    \sa setMatrix(), matrix(), scale(), shear(), translate()
*/
void QGraphicsView::rotate(qreal angle)
{
    Q_D(QGraphicsView);
    QTransform matrix = d->matrix;
    matrix.rotate(angle);
    setTransform(matrix);
}

/*!
    Scales the current view transformation by (\a sx, \a sy).

    \sa setMatrix(), matrix(), rotate(), shear(), translate()
*/
void QGraphicsView::scale(qreal sx, qreal sy)
{
    Q_D(QGraphicsView);
    QTransform matrix = d->matrix;
    matrix.scale(sx, sy);
    setTransform(matrix);
}

/*!
    Shears the current view transformation by (\a sh, \a sv).

    \sa setMatrix(), matrix(), rotate(), scale(), translate()
*/
void QGraphicsView::shear(qreal sh, qreal sv)
{
    Q_D(QGraphicsView);
    QTransform matrix = d->matrix;
    matrix.shear(sh, sv);
    setTransform(matrix);
}

/*!
    Translates the current view transformation by (\a dx, \a dy).

    \sa setMatrix(), matrix(), rotate(), shear()
*/
void QGraphicsView::translate(qreal dx, qreal dy)
{
    Q_D(QGraphicsView);
    QTransform matrix = d->matrix;
    matrix.translate(dx, dy);
    setTransform(matrix);
}

/*!
    Scrolls the contents of the viewport to ensure that the scene
    coordinate \a pos, is centered in the view.

    Because \a pos is a floating point coordinate, and the scroll bars operate
    on integer coordinates, the centering is only an approximation.

    \note If the item is close to or outside the border, it will be visible
    in the view, but not centered.

    \sa ensureVisible()
*/
void QGraphicsView::centerOn(const QPointF &pos)
{
    Q_D(QGraphicsView);
    qreal width = viewport()->width();
    qreal height = viewport()->height();
    QPointF viewPoint = d->matrix.map(pos);
    QPointF oldCenterPoint = pos;

    if (!d->leftIndent) {
        if (isRightToLeft()) {
            qint64 horizontal = 0;
            horizontal += horizontalScrollBar()->minimum();
            horizontal += horizontalScrollBar()->maximum();
            horizontal -= int(viewPoint.x() - width / 2.0);
            horizontalScrollBar()->setValue(horizontal);
        } else {
            horizontalScrollBar()->setValue(int(viewPoint.x() - width / 2.0));
        }
    }
    if (!d->topIndent)
        verticalScrollBar()->setValue(int(viewPoint.y() - height / 2.0));
    d->lastCenterPoint = oldCenterPoint;
}

/*!
    \fn QGraphicsView::centerOn(qreal x, qreal y)
    \overload

    This function is provided for convenience. It's equivalent to calling
    centerOn(QPointF(\a x, \a y)).
*/

/*!
    \overload

    Scrolls the contents of the viewport to ensure that \a item
    is centered in the view.

    \sa ensureVisible()
*/
void QGraphicsView::centerOn(const QGraphicsItem *item)
{
    centerOn(item->sceneBoundingRect().center());
}

/*!
    Scrolls the contents of the viewport so that the scene rectangle \a rect
    is visible, with margins specified in pixels by \a xmargin and \a
    ymargin. If the specified rect cannot be reached, the contents are
    scrolled to the nearest valid position. The default value for both margins
    is 50 pixels.

    \sa centerOn()
*/
void QGraphicsView::ensureVisible(const QRectF &rect, int xmargin, int ymargin)
{
    Q_D(QGraphicsView);
    Q_UNUSED(xmargin);
    Q_UNUSED(ymargin);
    qreal width = viewport()->width();
    qreal height = viewport()->height();
    QRectF viewRect = d->matrix.mapRect(rect);

    qreal left = d->horizontalScroll();
    qreal right = left + width;
    qreal top = d->verticalScroll();
    qreal bottom = top + height;

    if (viewRect.left() <= left + xmargin) {
        // need to scroll from the left
        if (!d->leftIndent)
            horizontalScrollBar()->setValue(int(viewRect.left() - xmargin - 0.5));
    }
    if (viewRect.right() >= right - xmargin) {
        // need to scroll from the right
        if (!d->leftIndent)
            horizontalScrollBar()->setValue(int(viewRect.right() - width + xmargin + 0.5));
    }
    if (viewRect.top() <= top + ymargin) {
        // need to scroll from the top
        if (!d->topIndent)
            verticalScrollBar()->setValue(int(viewRect.top() - ymargin - 0.5));
    }
    if (viewRect.bottom() >= bottom - ymargin) {
        // need to scroll from the bottom
        if (!d->topIndent)
            verticalScrollBar()->setValue(int(viewRect.bottom() - height + ymargin + 0.5));
    }
}

/*!
    \fn QGraphicsView::ensureVisible(qreal x, qreal y, qreal w, qreal h,
    int xmargin, int ymargin)
    \overload

    This function is provided for convenience. It's equivalent to calling
    ensureVisible(QRectF(\a x, \a y, \a w, \a h), \a xmargin, \a ymargin).
*/

/*!
    \overload

    Scrolls the contents of the viewport so that the center of item \a item is
    visible, with margins specified in pixels by \a xmargin and \a ymargin. If
    the specified point cannot be reached, the contents are scrolled to the
    nearest valid position. The default value for both margins is 50 pixels.

    \sa centerOn()
*/
void QGraphicsView::ensureVisible(const QGraphicsItem *item, int xmargin, int ymargin)
{
    ensureVisible(item->sceneBoundingRect(), xmargin, ymargin);
}

/*!
    Scales the view matrix and scrolls the scroll bars to ensure that the
    scene rectangle \a rect fits inside the viewport. \a rect must be inside
    the scene rect; otherwise, fitInView() cannot guarantee that the whole
    rect is visible.

    This function keeps the view's rotation, translation, or shear. The view
    is scaled according to \a aspectRatioMode. \a rect will be centered in the
    view if it does not fit tightly.

    It's common to call fitInView() from inside a reimplementation of
    resizeEvent(), to ensure that the whole scene, or parts of the scene,
    scales automatically to fit the new size of the viewport as the view is
    resized. Note though, that calling fitInView() from inside resizeEvent()
    can lead to unwanted resize recursion, if the new transformation toggles
    the automatic state of the scrollbars. You can toggle the scrollbar
    policies to always on or always off to prevent this (see
    horizontalScrollBarPolicy() and verticalScrollBarPolicy()).

    If \a rect is empty, or if the viewport is too small, this
    function will do nothing.

    \sa setMatrix(), ensureVisible(), centerOn()
*/
void QGraphicsView::fitInView(const QRectF &rect, Qt::AspectRatioMode aspectRatioMode)
{
    Q_D(QGraphicsView);
    if (!d->scene || rect.isNull())
        return;

    // Reset the view scale to 1:1.
    QRectF unity = d->matrix.mapRect(QRectF(0, 0, 1, 1));
    if (unity.isEmpty())
        return;
    scale(1 / unity.width(), 1 / unity.height());

    // Find the ideal x / y scaling ratio to fit \a rect in the view.
    int margin = 2;
    QRectF viewRect = viewport()->rect().adjusted(margin, margin, -margin, -margin);
    if (viewRect.isEmpty())
        return;
    QRectF sceneRect = d->matrix.mapRect(rect);
    if (sceneRect.isEmpty())
        return;
    qreal xratio = viewRect.width() / sceneRect.width();
    qreal yratio = viewRect.height() / sceneRect.height();

    // Respect the aspect ratio mode.
    switch (aspectRatioMode) {
    case Qt::KeepAspectRatio:
        xratio = yratio = qMin(xratio, yratio);
        break;
    case Qt::KeepAspectRatioByExpanding:
        xratio = yratio = qMax(xratio, yratio);
        break;
    case Qt::IgnoreAspectRatio:
        break;
    }

    // Scale and center on the center of \a rect.
    scale(xratio, yratio);
    centerOn(rect.center());
}

/*!
    \fn void QGraphicsView::fitInView(qreal x, qreal y, qreal w, qreal h,
    Qt::AspectRatioMode aspectRatioMode = Qt::IgnoreAspectRatio)

    \overload

    This convenience function is equivalent to calling
    fitInView(QRectF(\a x, \a y, \a w, \a h), \a aspectRatioMode).

    \sa ensureVisible(), centerOn()
*/

/*!
    \overload

    Ensures that \a item fits tightly inside the view, scaling the view
    according to \a aspectRatioMode.

    \sa ensureVisible(), centerOn()
*/
void QGraphicsView::fitInView(const QGraphicsItem *item, Qt::AspectRatioMode aspectRatioMode)
{
    QPainterPath path = item->isClipped() ? item->clipPath() : item->shape();
    fitInView(item->sceneTransform().map(path).boundingRect(), aspectRatioMode);
}

/*!
    Renders the \a source rect, which is in view coordinates, from the scene
    into \a target, which is in paint device coordinates, using \a
    painter. This function is useful for capturing the contents of the view
    onto a paint device, such as a QImage (e.g., to take a screenshot), or for
    printing to QPrinter. For example:

    \snippet doc/src/snippets/code/src_gui_graphicsview_qgraphicsview.cpp 4

    If \a source is a null rect, this function will use viewport()->rect() to
    determine what to draw. If \a target is a null rect, the full dimensions
    of \a painter's paint device (e.g., for a QPrinter, the page size) will be
    used.

    The source rect contents will be transformed according to \a
    aspectRatioMode to fit into the target rect. By default, the aspect ratio
    is kept, and \a source is scaled to fit in \a target.

    \sa QGraphicsScene::render()
*/
void QGraphicsView::render(QPainter *painter, const QRectF &target, const QRect &source,
                           Qt::AspectRatioMode aspectRatioMode)
{
    Q_D(QGraphicsView);
    if (!d->scene || !(painter && painter->isActive()))
        return;

    // Default source rect = viewport rect
    QRect sourceRect = source;
    if (source.isNull())
        sourceRect = viewport()->rect();

    // Default target rect = device rect
    QRectF targetRect = target;
    if (target.isNull()) {
        if (painter->device()->devType() == QInternal::Picture)
            targetRect = sourceRect;
        else
            targetRect.setRect(0, 0, painter->device()->width(), painter->device()->height());
    }

    // Find the ideal x / y scaling ratio to fit \a source into \a target.
    qreal xratio = targetRect.width() / sourceRect.width();
    qreal yratio = targetRect.height() / sourceRect.height();

    // Scale according to the aspect ratio mode.
    switch (aspectRatioMode) {
    case Qt::KeepAspectRatio:
        xratio = yratio = qMin(xratio, yratio);
        break;
    case Qt::KeepAspectRatioByExpanding:
        xratio = yratio = qMax(xratio, yratio);
        break;
    case Qt::IgnoreAspectRatio:
        break;
    }

    // Find all items to draw, and reverse the list (we want to draw
    // in reverse order).
    QPolygonF sourceScenePoly = mapToScene(sourceRect.adjusted(-1, -1, 1, 1));
    QList<QGraphicsItem *> itemList = d->scene->items(sourceScenePoly,
                                                      Qt::IntersectsItemBoundingRect);
    QGraphicsItem **itemArray = new QGraphicsItem *[itemList.size()];
    int numItems = itemList.size();
    for (int i = 0; i < numItems; ++i)
        itemArray[numItems - i - 1] = itemList.at(i);
    itemList.clear();

    // Setup painter matrix.
    QTransform moveMatrix;
    moveMatrix.translate(-d->horizontalScroll(), -d->verticalScroll());
    QTransform painterMatrix = d->matrix * moveMatrix;
    painterMatrix *= QTransform()
                     .translate(targetRect.left(), targetRect.top())
                     .scale(xratio, yratio)
                     .translate(-sourceRect.left(), -sourceRect.top());

    // Two unit vectors.
    QLineF v1(0, 0, 1, 0);
    QLineF v2(0, 0, 0, 1);

    // Generate the style options
    QStyleOptionGraphicsItem *styleOptionArray = d->allocStyleOptionsArray(numItems);
    QStyleOptionGraphicsItem* option = styleOptionArray;
    for (int i = 0; i < numItems; ++i, ++option) {
        QGraphicsItem *item = itemArray[i];

        option->state = QStyle::State_None;
        option->rect = item->boundingRect().toRect();
        if (item->isSelected())
            option->state |= QStyle::State_Selected;
        if (item->isEnabled())
            option->state |= QStyle::State_Enabled;
        if (item->hasFocus())
            option->state |= QStyle::State_HasFocus;
        if (d->scene->d_func()->hoverItems.contains(item))
            option->state |= QStyle::State_MouseOver;
        if (item == d->scene->mouseGrabberItem())
            option->state |= QStyle::State_Sunken;

        // Calculate a simple level-of-detail metric.
        // ### almost identical code in QGraphicsScene::render()
        //     and QGraphicsView::paintEvent() - consider refactoring
        QTransform itemToViewportTransform;
        if (item->d_ptr->itemIsUntransformable()) {
            itemToViewportTransform = item->deviceTransform(painterMatrix);
        } else {
            itemToViewportTransform = item->sceneTransform() * painterMatrix;
        }

        option->levelOfDetail = qSqrt(itemToViewportTransform.map(v1).length() * itemToViewportTransform.map(v2).length());
        option->matrix = itemToViewportTransform.toAffine();

        option->exposedRect = item->boundingRect();
        option->exposedRect &= itemToViewportTransform.inverted().mapRect(targetRect);
    }

    painter->save();

    // Clip in device coordinates to avoid QRegion transformations.
    painter->setClipRect(targetRect);
    QPainterPath path;
    path.addPolygon(sourceScenePoly);
    path.closeSubpath();
    painter->setClipPath(painterMatrix.map(path), Qt::IntersectClip);

    // Transform the painter.
    painter->setTransform(painterMatrix, true);

    // Render the scene.
    QRectF sourceSceneRect = sourceScenePoly.boundingRect();
    drawBackground(painter, sourceSceneRect);
    drawItems(painter, numItems, itemArray, styleOptionArray);
    drawForeground(painter, sourceSceneRect);

    delete [] itemArray;
    d->freeStyleOptionsArray(styleOptionArray);

    painter->restore();
}

/*!
    Returns a list of all the items in the associated scene.

    \sa QGraphicsScene::items()
*/
QList<QGraphicsItem *> QGraphicsView::items() const
{
    Q_D(const QGraphicsView);
    if (!d->scene)
        return QList<QGraphicsItem *>();
    return d->scene->items();
}

/*!
    Returns all items in the area \a path, which is in viewport coordinates,
    also taking untransformable items into consideration. This function is
    considerably slower than just checking the scene directly. There is
    certainly room for improvement.
*/
QList<QGraphicsItem *> QGraphicsViewPrivate::itemsInArea(const QPainterPath &path,
                                                         Qt::ItemSelectionMode mode) const
{
    Q_Q(const QGraphicsView);

    // Determine the size of the largest untransformable subtree of children
    // mapped to scene coordinates.
    QRectF untr = scene->d_func()->largestUntransformableItem;
    QRectF ltri = matrix.inverted().mapRect(untr);
    ltri.adjust(-untr.width(), -untr.height(), untr.width(), untr.height());

    QRectF rect = path.controlPointRect();

    // Find all possible items in the relevant area.
    // ### Improve this algorithm; it might be searching a too large area.
    QRectF adjustedRect = q->mapToScene(rect.adjusted(-1, -1, 1, 1).toRect()).boundingRect();
    adjustedRect.adjust(-ltri.width(), -ltri.height(), ltri.width(), ltri.height());

    // First build a (potentially large) list of all items in the vicinity
    // that might be untransformable.
    QList<QGraphicsItem *> allCandidates = scene->d_func()->estimateItemsInRect(adjustedRect);

    // Then find the minimal list of items that are inside \a path, and
    // convert it to a set.
    QList<QGraphicsItem *> regularCandidates = scene->items(q->mapToScene(path), mode);
    QSet<QGraphicsItem *> candSet = QSet<QGraphicsItem *>::fromList(regularCandidates);

    QTransform viewMatrix = q->viewportTransform();

    QList<QGraphicsItem *> result;

    // Run through all candidates and keep all items that are in candSet, or
    // are untransformable and collide with \a path. ### We can improve this
    // algorithm.
    QList<QGraphicsItem *>::Iterator it = allCandidates.begin();
    while (it != allCandidates.end()) {
        QGraphicsItem *item = *it;
        if (item->d_ptr->itemIsUntransformable()) {
            // Check if this untransformable item collides with the
            // original selection rect.
            QTransform itemTransform = item->deviceTransform(viewMatrix);
            if (QGraphicsScenePrivate::itemCollidesWithPath(item, itemTransform.inverted().map(path), mode))
                result << item;
        } else {
            if (candSet.contains(item))
                result << item;
        }
        ++it;
    }

    // ### Insertion sort would be faster.
    QGraphicsScenePrivate::sortItems(&result, Qt::AscendingOrder,
                                     scene->d_func()->sortCacheEnabled);
    return result;
}

/*!
    Returns a list of all the items at the position \a pos in the view. The
    items are listed in descending Z order (i.e., the first item in the list
    is the top-most item, and the last item is the bottom-most item). \a pos
    is in viewport coordinates.

    This function is most commonly called from within mouse event handlers in
    a subclass in QGraphicsView. \a pos is in untransformed viewport
    coordinates, just like QMouseEvent::pos().

    \snippet doc/src/snippets/code/src_gui_graphicsview_qgraphicsview.cpp 5

    \sa QGraphicsScene::items(), QGraphicsItem::zValue()
*/
QList<QGraphicsItem *> QGraphicsView::items(const QPoint &pos) const
{
    Q_D(const QGraphicsView);
    if (!d->scene)
        return QList<QGraphicsItem *>();
    if (d->scene->d_func()->largestUntransformableItem.isNull()) {
        if ((d->identityMatrix || d->matrix.type() <= QTransform::TxScale)) {
            QTransform xinv = viewportTransform().inverted();
            return d->scene->items(xinv.mapRect(QRectF(pos.x(), pos.y(), 1, 1)));
        }
        return d->scene->items(mapToScene(pos.x(), pos.y(), 2, 2));
    }

    QPainterPath path;
    path.addRect(QRectF(pos.x(), pos.y(), 1, 1));
    return d->itemsInArea(path);
}

/*!
    \fn QGraphicsView::items(int x, int y) const

    This function is provided for convenience. It's equivalent to calling
    items(QPoint(\a x, \a y)).
*/

/*!
    \overload

    Returns a list of all the items that, depending on \a mode, are either
    contained by or intersect with \a rect. \a rect is in viewport
    coordinates.

    The default value for \a mode is Qt::IntersectsItemShape; all items whose
    exact shape intersects with or is contained by \a rect are returned.

    \sa itemAt(), items(), mapToScene()
*/
QList<QGraphicsItem *> QGraphicsView::items(const QRect &rect, Qt::ItemSelectionMode mode) const
{
    Q_D(const QGraphicsView);
    if (!d->scene)
        return QList<QGraphicsItem *>();
    if (d->scene->d_func()->largestUntransformableItem.isNull())
        return d->scene->items(mapToScene(rect), mode);

    QPainterPath path;
    path.addRect(rect);
    return d->itemsInArea(path);
}

/*!
    \fn QList<QGraphicsItem *> QGraphicsView::items(int x, int y, int w, int h, Qt::ItemSelectionMode mode) const
    \since 4.3

    This convenience function is equivalent to calling items(QRectF(\a x, \a
    y, \a w, \a h), \a mode).
*/

/*!
    \overload

    Returns a list of all the items that, depending on \a mode, are either
    contained by or intersect with \a polygon. \a polygon is in viewport
    coordinates.

    The default value for \a mode is Qt::IntersectsItemShape; all items whose
    exact shape intersects with or is contained by \a polygon are returned.

    \sa itemAt(), items(), mapToScene()
*/
QList<QGraphicsItem *> QGraphicsView::items(const QPolygon &polygon, Qt::ItemSelectionMode mode) const
{
    Q_D(const QGraphicsView);
    if (!d->scene)
        return QList<QGraphicsItem *>();
    if (d->scene->d_func()->largestUntransformableItem.isNull())
        return d->scene->items(mapToScene(polygon), mode);

    QPainterPath path;
    path.addPolygon(polygon);
    path.closeSubpath();
    return d->itemsInArea(path);
}

/*!
    \overload

    Returns a list of all the items that, depending on \a mode, are either
    contained by or intersect with \a path. \a path is in viewport
    coordinates.

    The default value for \a mode is Qt::IntersectsItemShape; all items whose
    exact shape intersects with or is contained by \a path are returned.

    \sa itemAt(), items(), mapToScene()
*/
QList<QGraphicsItem *> QGraphicsView::items(const QPainterPath &path, Qt::ItemSelectionMode mode) const
{
    Q_D(const QGraphicsView);
    if (!d->scene)
        return QList<QGraphicsItem *>();
    if (d->scene->d_func()->largestUntransformableItem.isNull())
        return d->scene->items(mapToScene(path), mode);
    return d->itemsInArea(path);
}

/*!
    Returns the item at position \a pos, which is in viewport coordinates.
    If there are several items at this position, this function returns
    the topmost item.

    Example:

    \snippet doc/src/snippets/code/src_gui_graphicsview_qgraphicsview.cpp 6

    \sa items()
*/
QGraphicsItem *QGraphicsView::itemAt(const QPoint &pos) const
{
    Q_D(const QGraphicsView);
    if (!d->scene)
        return 0;
    QList<QGraphicsItem *> itemsAtPos = items(pos);
    return itemsAtPos.isEmpty() ? 0 : itemsAtPos.first();
}

/*!
    \overload
    \fn QGraphicsItem *QGraphicsView::itemAt(int x, int y) const

    This function is provided for convenience. It's equivalent to
    calling itemAt(QPoint(\a x, \a y)).
*/

/*!
    Returns the viewport coordinate \a point mapped to scene coordinates.

    Note: It can be useful to map the whole rectangle covered by the pixel at
    \a point instead of the point itself. To do this, you can call
    mapToScene(QRect(\a point, QSize(2, 2))).

    \sa mapFromScene()
*/
QPointF QGraphicsView::mapToScene(const QPoint &point) const
{
    Q_D(const QGraphicsView);
    QPointF p = point;
    p.rx() += d->horizontalScroll();
    p.ry() += d->verticalScroll();
    return d->identityMatrix ? p : d->matrix.inverted().map(p);
}

/*!
    \fn QGraphicsView::mapToScene(int x, int y) const

    This function is provided for convenience. It's equivalent to calling
    mapToScene(QPoint(\a x, \a y)).
*/

/*!
    Returns the viewport rectangle \a rect mapped to a scene coordinate
    polygon.

    \sa mapFromScene()
*/
QPolygonF QGraphicsView::mapToScene(const QRect &rect) const
{
    Q_D(const QGraphicsView);
    if (!rect.isValid())
        return QPolygonF();

    QPointF scrollOffset(d->horizontalScroll(), d->verticalScroll());
    QPointF tl = scrollOffset + rect.topLeft();
    QPointF tr = scrollOffset + rect.topRight();
    QPointF br = scrollOffset + rect.bottomRight();
    QPointF bl = scrollOffset + rect.bottomLeft();

    QPolygonF poly;
    poly.resize(4);
    if (!d->identityMatrix) {
        QTransform x = d->matrix.inverted();
        poly[0] = x.map(tl);
        poly[1] = x.map(tr);
        poly[2] = x.map(br);
        poly[3] = x.map(bl);
    } else {
        poly[0] = tl;
        poly[1] = tr;
        poly[2] = br;
        poly[3] = bl;
    }
    return poly;
}

/*!
    \fn QGraphicsView::mapToScene(int x, int y, int w, int h) const

    This function is provided for convenience. It's equivalent to calling
    mapToScene(QRect(\a x, \a y, \a w, \a h)).
*/

/*!
    Returns the viewport polygon \a polygon mapped to a scene coordinate
    polygon.

    \sa mapFromScene()
*/
QPolygonF QGraphicsView::mapToScene(const QPolygon &polygon) const
{
    QPolygonF poly;
    foreach (const QPoint &point, polygon)
        poly << mapToScene(point);
    return poly;
}

/*!
    Returns the viewport painter path \a path mapped to a scene coordinate
    painter path.

    \sa mapFromScene()
*/
QPainterPath QGraphicsView::mapToScene(const QPainterPath &path) const
{
    Q_D(const QGraphicsView);
    QTransform moveMatrix;
    moveMatrix.translate(d->horizontalScroll(), d->verticalScroll());
    return (moveMatrix * d->matrix.inverted()).map(path);
}

/*!
    Returns the scene coordinate \a point to viewport coordinates.

    \sa mapToScene()
*/
QPoint QGraphicsView::mapFromScene(const QPointF &point) const
{
    Q_D(const QGraphicsView);
    QPointF p = d->identityMatrix ? point : d->matrix.map(point);
    p.rx() -= d->horizontalScroll();
    p.ry() -= d->verticalScroll();
    return p.toPoint();
}

/*!
    \fn QGraphicsView::mapFromScene(qreal x, qreal y) const

    This function is provided for convenience. It's equivalent to
    calling mapFromScene(QPointF(\a x, \a y)).
*/

/*!
    Returns the scene rectangle \a rect to a viewport coordinate
    polygon.

    \sa mapToScene()
*/
QPolygon QGraphicsView::mapFromScene(const QRectF &rect) const
{
    Q_D(const QGraphicsView);
    QPointF tl;
    QPointF tr;
    QPointF br;
    QPointF bl;
    if (!d->identityMatrix) {
        const QTransform &x = d->matrix;
        tl = x.map(rect.topLeft());
        tr = x.map(rect.topRight());
        br = x.map(rect.bottomRight());
        bl = x.map(rect.bottomLeft());
    } else {
        tl = rect.topLeft();
        tr = rect.topRight();
        br = rect.bottomRight();
        bl = rect.bottomLeft();
    }
    QPointF scrollOffset(d->horizontalScroll(), d->verticalScroll());
    tl -= scrollOffset;
    tr -= scrollOffset;
    br -= scrollOffset;
    bl -= scrollOffset;

    QPolygon poly;
    poly.resize(4);
    poly[0] = tl.toPoint();
    poly[1] = tr.toPoint();
    poly[2] = br.toPoint();
    poly[3] = bl.toPoint();
    return poly;
}

/*!
    \fn QGraphicsView::mapFromScene(qreal x, qreal y, qreal w, qreal h) const

    This function is provided for convenience. It's equivalent to
    calling mapFromScene(QRectF(\a x, \a y, \a w, \a h)).
*/

/*!
    Returns the scene coordinate polygon \a polygon to a viewport coordinate
    polygon.

    \sa mapToScene()
*/
QPolygon QGraphicsView::mapFromScene(const QPolygonF &polygon) const
{
    QPolygon poly;
    foreach (const QPointF &point, polygon)
        poly << mapFromScene(point);
    return poly;
}

/*!
    Returns the scene coordinate painter path \a path to a viewport coordinate
    painter path.

    \sa mapToScene()
*/
QPainterPath QGraphicsView::mapFromScene(const QPainterPath &path) const
{
    Q_D(const QGraphicsView);
    QTransform moveMatrix;
    moveMatrix.translate(-d->horizontalScroll(), -d->verticalScroll());
    return (d->matrix * moveMatrix).map(path);
}

/*!
    \reimp
*/
QVariant QGraphicsView::inputMethodQuery(Qt::InputMethodQuery query) const
{
    Q_D(const QGraphicsView);
    if (!d->scene)
        return QVariant();

    QVariant value = d->scene->inputMethodQuery(query);
    if (value.type() == QVariant::RectF)
        value = mapFromScene(value.toRectF()).boundingRect();
    else if (value.type() == QVariant::PointF)
        value = mapFromScene(value.toPointF());
    else if (value.type() == QVariant::Rect)
        value = mapFromScene(value.toRect()).boundingRect();
    else if (value.type() == QVariant::Point)
        value = mapFromScene(value.toPoint());
    return value;
}

/*!
    \property QGraphicsView::backgroundBrush
    \brief the background brush of the scene.

    This property sets the background brush for the scene in this view. It is
    used to override the scene's own background, and defines the behavior of
    drawBackground(). To provide custom background drawing for this view, you
    can reimplement drawBackground() instead.

    By default, this property contains a brush with the Qt::NoBrush pattern.

    \sa QGraphicsScene::backgroundBrush, foregroundBrush
*/
QBrush QGraphicsView::backgroundBrush() const
{
    Q_D(const QGraphicsView);
    return d->backgroundBrush;
}
void QGraphicsView::setBackgroundBrush(const QBrush &brush)
{
    Q_D(QGraphicsView);
    d->backgroundBrush = brush;
    viewport()->update();

    if (d->cacheMode & CacheBackground) {
        // Invalidate the background pixmap
        d->mustResizeBackgroundPixmap = true;
    }
}

/*!
    \property QGraphicsView::foregroundBrush
    \brief the foreground brush of the scene.

    This property sets the foreground brush for the scene in this view. It is
    used to override the scene's own foreground, and defines the behavior of
    drawForeground(). To provide custom foreground drawing for this view, you
    can reimplement drawForeground() instead.

    By default, this property contains a brush with the Qt::NoBrush pattern.

    \sa QGraphicsScene::foregroundBrush, backgroundBrush
*/
QBrush QGraphicsView::foregroundBrush() const
{
    Q_D(const QGraphicsView);
    return d->foregroundBrush;
}
void QGraphicsView::setForegroundBrush(const QBrush &brush)
{
    Q_D(QGraphicsView);
    d->foregroundBrush = brush;
    viewport()->update();
}

/*!
    Schedules an update of the scene rectangles \a rects.

    \sa QGraphicsScene::changed()
*/
void QGraphicsView::updateScene(const QList<QRectF> &rects)
{
    // ### Note: Since 4.5, this slot is only called if the user explicitly
    // establishes a connection between the scene and the view, as the scene
    // and view are no longer connected. We need to keep it working (basically
    // leave it as it is), but the new delivery path is through
    // QGraphicsScenePrivate::itemUpdate().
    Q_D(QGraphicsView);
    if (d->fullUpdatePending || d->viewportUpdateMode == QGraphicsView::NoViewportUpdate)
        return;

    // Extract and reset dirty scene rect info.
    QVector<QRect> dirtyViewportRects;
    for (int i = 0; i < d->dirtyRegions.size(); ++i)
        dirtyViewportRects += d->dirtyRegions.at(i).rects();
    d->dirtyRegions.clear();

    bool fullUpdate = !d->accelerateScrolling || d->viewportUpdateMode == QGraphicsView::FullViewportUpdate;
    bool boundingRectUpdate = (d->viewportUpdateMode == QGraphicsView::BoundingRectViewportUpdate)
                              || (d->viewportUpdateMode == QGraphicsView::SmartViewportUpdate
                                  && ((dirtyViewportRects.size() + rects.size()) >= QGRAPHICSVIEW_REGION_RECT_THRESHOLD));

    QRegion updateRegion;
    QRect boundingRect;
    QRect viewportRect = viewport()->rect();
    bool redraw = false;
    QTransform transform = viewportTransform();

    // Convert scene rects to viewport rects.
    foreach (const QRectF &rect, rects) {
        QRect xrect = transform.mapRect(rect).toRect();
        if (!(d->optimizationFlags & DontAdjustForAntialiasing))
            xrect.adjust(-2, -2, 2, 2);
        if (!viewportRect.intersects(xrect))
            continue;
        dirtyViewportRects << xrect;
    }

    foreach (const QRect &rect, dirtyViewportRects) {
        // Add the exposed rect to the update region. In rect update
        // mode, we only count the bounding rect of items.
        if (!boundingRectUpdate) {
            updateRegion += rect;
        } else {
            boundingRect |= rect;
        }
        redraw = true;
        if (fullUpdate) {
            // If fullUpdate is true and we found a visible dirty rect,
            // we're done.
            break;
        }
    }

    if (!redraw)
        return;

    if (fullUpdate)
        viewport()->update();
    else if (boundingRectUpdate)
        viewport()->update(boundingRect);
    else
        viewport()->update(updateRegion);
}

/*!
    Notifies QGraphicsView that the scene's scene rect has changed.  \a rect
    is the new scene rect. If the view already has an explicitly set scene
    rect, this function does nothing.

    \sa sceneRect, QGraphicsScene::sceneRectChanged()
*/
void QGraphicsView::updateSceneRect(const QRectF &rect)
{
    Q_D(QGraphicsView);
    if (!d->hasSceneRect) {
        d->sceneRect = rect;
        d->recalculateContentSize();
    }
}

/*!
    This slot is called by QAbstractScrollArea after setViewport() has been
    called. Reimplement this function in a subclass of QGraphicsView to
    initialize the new viewport \a widget before it is used.

    \sa setViewport()
*/
void QGraphicsView::setupViewport(QWidget *widget)
{
    Q_D(QGraphicsView);

    if (!widget) {
        qWarning("QGraphicsView::setupViewport: cannot initialize null widget");
        return;
    }

    const bool isGLWidget = widget->inherits("QGLWidget");

    d->accelerateScrolling = !(isGLWidget
                               || widget->testAttribute(Qt::WA_MSWindowsUseDirect3D)
                               || qApp->testAttribute(Qt::AA_MSWindowsUseDirect3DByDefault));

    widget->setFocusPolicy(Qt::StrongFocus);

    if (!isGLWidget) {
        // autoFillBackground enables scroll acceleration.
        widget->setAutoFillBackground(true);
    }

    widget->setMouseTracking(true);
    widget->setAcceptDrops(acceptDrops());
}

/*!
    \reimp
*/
bool QGraphicsView::event(QEvent *event)
{
    Q_D(QGraphicsView);

    if (d->sceneInteractionAllowed) {
        switch (event->type()) {
        case QEvent::ShortcutOverride:
            if (d->scene)
                return QApplication::sendEvent(d->scene, event);
            break;
        case QEvent::KeyPress:
            if (d->scene) {
                QKeyEvent *k = static_cast<QKeyEvent *>(event);
                if (k->key() == Qt::Key_Tab || k->key() == Qt::Key_Backtab) {
                    // Send the key events to the scene. This will invoke the
                    // scene's tab focus handling, and if the event is
                    // accepted, we return (prevent further event delivery),
                    // and the base implementation will call QGraphicsView's
                    // focusNextPrevChild() function. If the event is ignored,
                    // we fall back to standard tab focus handling.
                    QApplication::sendEvent(d->scene, event);
                    if (event->isAccepted())
                        return true;
                    // Ensure the event doesn't propagate just because the
                    // scene ignored it. If the event propagates, then tab
                    // handling will be called twice (this and parent).
                    event->accept();
                }
            }
            break;
        default:
            break;
        }
    }

    return QAbstractScrollArea::event(event);
}

/*!
    \reimp
*/
bool QGraphicsView::viewportEvent(QEvent *event)
{
    Q_D(QGraphicsView);

    if (!d->scene)
        return QAbstractScrollArea::viewportEvent(event);

    switch (event->type()) {
    case QEvent::Enter:
        QApplication::sendEvent(d->scene, event);
        break;
    case QEvent::WindowActivate:
        QApplication::sendEvent(d->scene, event);
        break;
    case QEvent::WindowDeactivate:
        // ### This is a temporary fix for until we get proper mouse
        // grab events. mouseGrabberItem should be set to 0 if we lose
        // the mouse grab.
        // Remove all popups when the scene loses focus.
        if (!d->scene->d_func()->popupWidgets.isEmpty())
            d->scene->d_func()->removePopup(d->scene->d_func()->popupWidgets.first());
        QApplication::sendEvent(d->scene, event);
        break;
    case QEvent::Leave:
        // ### This is a temporary fix for until we get proper mouse grab
        // events. activeMouseGrabberItem should be set to 0 if we lose the
        // mouse grab.
        if ((QApplication::activePopupWidget() && QApplication::activePopupWidget() != window())
            || (QApplication::activeModalWidget() && QApplication::activeModalWidget() != window())
            || (QApplication::activeWindow() != window())) {
            if (!d->scene->d_func()->popupWidgets.isEmpty())
                d->scene->d_func()->removePopup(d->scene->d_func()->popupWidgets.first());
        }
        d->useLastMouseEvent = false;
        QApplication::sendEvent(d->scene, event);
        break;
#ifndef QT_NO_TOOLTIP
    case QEvent::ToolTip: {
        QHelpEvent *toolTip = static_cast<QHelpEvent *>(event);
        QGraphicsSceneHelpEvent helpEvent(QEvent::GraphicsSceneHelp);
        helpEvent.setWidget(viewport());
        helpEvent.setScreenPos(toolTip->globalPos());
        helpEvent.setScenePos(mapToScene(toolTip->pos()));
        QApplication::sendEvent(d->scene, &helpEvent);
        toolTip->setAccepted(helpEvent.isAccepted());
        return true;
    }
#endif
    case QEvent::Paint:
        // Reset full update
        d->fullUpdatePending = false;
        if (d->scene) {
            // Check if this view reimplements the updateScene slot; if it
            // does, we can't do direct update delivery and have to fall back
            // to connecting the changed signal.
            if (!d->updateSceneSlotReimplementedChecked) {
                d->updateSceneSlotReimplementedChecked = true;
                const QMetaObject *mo = metaObject();
                if (mo != &QGraphicsView::staticMetaObject) {
                    if (mo->indexOfSlot("updateScene(QList<QRectF>)")
                        != QGraphicsView::staticMetaObject.indexOfSlot("updateScene(QList<QRectF>)")) {
                        connect(d->scene, SIGNAL(changed(QList<QRectF>)),
                                this, SLOT(updateScene(QList<QRectF>)));
                    }
                }
            }
            d->scene->d_func()->updateAll = false;
        }
        break;
    default:
        break;
    }

    return QAbstractScrollArea::viewportEvent(event);
}

#ifndef QT_NO_CONTEXTMENU
/*!
    \reimp
*/
void QGraphicsView::contextMenuEvent(QContextMenuEvent *event)
{
    Q_D(QGraphicsView);
    if (!d->scene || !d->sceneInteractionAllowed)
        return;

    d->mousePressViewPoint = event->pos();
    d->mousePressScenePoint = mapToScene(d->mousePressViewPoint);
    d->mousePressScreenPoint = event->globalPos();
    d->lastMouseMoveScenePoint = d->mousePressScenePoint;
    d->lastMouseMoveScreenPoint = d->mousePressScreenPoint;

    QGraphicsSceneContextMenuEvent contextEvent(QEvent::GraphicsSceneContextMenu);
    contextEvent.setWidget(viewport());
    contextEvent.setScenePos(d->mousePressScenePoint);
    contextEvent.setScreenPos(d->mousePressScreenPoint);
    contextEvent.setModifiers(event->modifiers());
    contextEvent.setReason((QGraphicsSceneContextMenuEvent::Reason)(event->reason()));
    contextEvent.setAccepted(event->isAccepted());
    QApplication::sendEvent(d->scene, &contextEvent);
    event->setAccepted(contextEvent.isAccepted());
}
#endif // QT_NO_CONTEXTMENU

/*!
    \reimp
*/
void QGraphicsView::dropEvent(QDropEvent *event)
{
#ifndef QT_NO_DRAGANDDROP
    Q_D(QGraphicsView);
    if (!d->scene || !d->sceneInteractionAllowed)
        return;

    // Generate a scene event.
    QGraphicsSceneDragDropEvent sceneEvent(QEvent::GraphicsSceneDrop);
    d->populateSceneDragDropEvent(&sceneEvent, event);

    // Send it to the scene.
    QApplication::sendEvent(d->scene, &sceneEvent);

    // Accept the originating event if the scene accepted the scene event.
    event->setAccepted(sceneEvent.isAccepted());
    if (sceneEvent.isAccepted())
        event->setDropAction(sceneEvent.dropAction());

    delete d->lastDragDropEvent;
    d->lastDragDropEvent = 0;

#else
    Q_UNUSED(event)
#endif
}

/*!
    \reimp
*/
void QGraphicsView::dragEnterEvent(QDragEnterEvent *event)
{
#ifndef QT_NO_DRAGANDDROP
    Q_D(QGraphicsView);
    if (!d->scene || !d->sceneInteractionAllowed)
        return;

    // Disable replaying of mouse move events.
    d->useLastMouseEvent = false;

    // Generate a scene event.
    QGraphicsSceneDragDropEvent sceneEvent(QEvent::GraphicsSceneDragEnter);
    d->populateSceneDragDropEvent(&sceneEvent, event);

    // Store it for later use.
    d->storeDragDropEvent(&sceneEvent);

    // Send it to the scene.
    QApplication::sendEvent(d->scene, &sceneEvent);

    // Accept the originating event if the scene accepted the scene event.
    if (sceneEvent.isAccepted()) {
        event->setAccepted(true);
        event->setDropAction(sceneEvent.dropAction());
    }
#else
    Q_UNUSED(event)
#endif
}

/*!
    \reimp
*/
void QGraphicsView::dragLeaveEvent(QDragLeaveEvent *event)
{
#ifndef QT_NO_DRAGANDDROP
    Q_D(QGraphicsView);
    if (!d->scene || !d->sceneInteractionAllowed)
        return;
    if (!d->lastDragDropEvent) {
        qWarning("QGraphicsView::dragLeaveEvent: drag leave received before drag enter");
        return;
    }

    // Generate a scene event.
    QGraphicsSceneDragDropEvent sceneEvent(QEvent::GraphicsSceneDragLeave);
    sceneEvent.setScenePos(d->lastDragDropEvent->scenePos());
    sceneEvent.setScreenPos(d->lastDragDropEvent->screenPos());
    sceneEvent.setButtons(d->lastDragDropEvent->buttons());
    sceneEvent.setModifiers(d->lastDragDropEvent->modifiers());
    sceneEvent.setPossibleActions(d->lastDragDropEvent->possibleActions());
    sceneEvent.setProposedAction(d->lastDragDropEvent->proposedAction());
    sceneEvent.setDropAction(d->lastDragDropEvent->dropAction());
    sceneEvent.setMimeData(d->lastDragDropEvent->mimeData());
    sceneEvent.setWidget(d->lastDragDropEvent->widget());
    sceneEvent.setSource(d->lastDragDropEvent->source());
    delete d->lastDragDropEvent;
    d->lastDragDropEvent = 0;

    // Send it to the scene.
    QApplication::sendEvent(d->scene, &sceneEvent);

    // Accept the originating event if the scene accepted the scene event.
    if (sceneEvent.isAccepted())
        event->setAccepted(true);
#else
    Q_UNUSED(event)
#endif
}

/*!
    \reimp
*/
void QGraphicsView::dragMoveEvent(QDragMoveEvent *event)
{
#ifndef QT_NO_DRAGANDDROP
    Q_D(QGraphicsView);
    if (!d->scene || !d->sceneInteractionAllowed)
        return;

    // Generate a scene event.
    QGraphicsSceneDragDropEvent sceneEvent(QEvent::GraphicsSceneDragMove);
    d->populateSceneDragDropEvent(&sceneEvent, event);

    // Store it for later use.
    d->storeDragDropEvent(&sceneEvent);

    // Send it to the scene.
    QApplication::sendEvent(d->scene, &sceneEvent);

    // Ignore the originating event if the scene ignored the scene event.
    event->setAccepted(sceneEvent.isAccepted());
    if (sceneEvent.isAccepted())
        event->setDropAction(sceneEvent.dropAction());
#else
    Q_UNUSED(event)
#endif
}

/*!
    \reimp
*/
void QGraphicsView::focusInEvent(QFocusEvent *event)
{
    Q_D(QGraphicsView);
    QAbstractScrollArea::focusInEvent(event);
    if (d->scene)
        QApplication::sendEvent(d->scene, event);
    // Pass focus on if the scene cannot accept focus.
    if (!d->scene || !event->isAccepted())
        QAbstractScrollArea::focusInEvent(event);
}

/*!
    \reimp
*/
bool QGraphicsView::focusNextPrevChild(bool next)
{
    return QAbstractScrollArea::focusNextPrevChild(next);
}

/*!
    \reimp
*/
void QGraphicsView::focusOutEvent(QFocusEvent *event)
{
    Q_D(QGraphicsView);
    QAbstractScrollArea::focusOutEvent(event);
    if (d->scene)
        QApplication::sendEvent(d->scene, event);
}

/*!
    \reimp
*/
void QGraphicsView::keyPressEvent(QKeyEvent *event)
{
    Q_D(QGraphicsView);
    if (!d->scene || !d->sceneInteractionAllowed) {
        QAbstractScrollArea::keyPressEvent(event);
        return;
    }
    QApplication::sendEvent(d->scene, event);
    if (!event->isAccepted())
        QAbstractScrollArea::keyPressEvent(event);
}

/*!
    \reimp
*/
void QGraphicsView::keyReleaseEvent(QKeyEvent *event)
{
    Q_D(QGraphicsView);
    if (!d->scene || !d->sceneInteractionAllowed)
        return;
    QApplication::sendEvent(d->scene, event);
    if (!event->isAccepted())
        QAbstractScrollArea::keyReleaseEvent(event);
}

/*!
    \reimp
*/
void QGraphicsView::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_D(QGraphicsView);
    if (!d->scene || !d->sceneInteractionAllowed)
        return;

    d->storeMouseEvent(event);
    d->mousePressViewPoint = event->pos();
    d->mousePressScenePoint = mapToScene(d->mousePressViewPoint);
    d->mousePressScreenPoint = event->globalPos();
    d->lastMouseMoveScenePoint = d->mousePressScenePoint;
    d->lastMouseMoveScreenPoint = d->mousePressScreenPoint;
    d->mousePressButton = event->button();

    QGraphicsSceneMouseEvent mouseEvent(QEvent::GraphicsSceneMouseDoubleClick);
    mouseEvent.setWidget(viewport());
    mouseEvent.setButtonDownScenePos(d->mousePressButton, d->mousePressScenePoint);
    mouseEvent.setButtonDownScreenPos(d->mousePressButton, d->mousePressScreenPoint);
    mouseEvent.setScenePos(mapToScene(d->mousePressViewPoint));
    mouseEvent.setScreenPos(d->mousePressScreenPoint);
    mouseEvent.setLastScenePos(d->lastMouseMoveScenePoint);
    mouseEvent.setLastScreenPos(d->lastMouseMoveScreenPoint);
    mouseEvent.setButtons(event->buttons());
    mouseEvent.setButtons(event->buttons());
    mouseEvent.setAccepted(false);
    mouseEvent.setButton(event->button());
    mouseEvent.setModifiers(event->modifiers());
    QApplication::sendEvent(d->scene, &mouseEvent);
}

/*!
    \reimp
*/
void QGraphicsView::mousePressEvent(QMouseEvent *event)
{
    Q_D(QGraphicsView);

    // Store this event for replaying, finding deltas, and for
    // scroll-dragging; even in non-interactive mode, scroll hand dragging is
    // allowed, so we store the event at the very top of this function.
    d->storeMouseEvent(event);
    d->lastMouseEvent.setAccepted(false);

    if (d->sceneInteractionAllowed) {
        // Store some of the event's button-down data.
        d->mousePressViewPoint = event->pos();
        d->mousePressScenePoint = mapToScene(d->mousePressViewPoint);
        d->mousePressScreenPoint = event->globalPos();
        d->lastMouseMoveScenePoint = d->mousePressScenePoint;
        d->lastMouseMoveScreenPoint = d->mousePressScreenPoint;
        d->mousePressButton = event->button();

        if (d->scene) {
            // Convert and deliver the mouse event to the scene.
            QGraphicsSceneMouseEvent mouseEvent(QEvent::GraphicsSceneMousePress);
            mouseEvent.setWidget(viewport());
            mouseEvent.setButtonDownScenePos(d->mousePressButton, d->mousePressScenePoint);
            mouseEvent.setButtonDownScreenPos(d->mousePressButton, d->mousePressScreenPoint);
            mouseEvent.setScenePos(d->mousePressScenePoint);
            mouseEvent.setScreenPos(d->mousePressScreenPoint);
            mouseEvent.setLastScenePos(d->lastMouseMoveScenePoint);
            mouseEvent.setLastScreenPos(d->lastMouseMoveScreenPoint);
            mouseEvent.setButtons(event->buttons());
            mouseEvent.setButton(event->button());
            mouseEvent.setModifiers(event->modifiers());
            mouseEvent.setAccepted(false);
            QApplication::sendEvent(d->scene, &mouseEvent);

            // Update the original mouse event accepted state.
            bool isAccepted = mouseEvent.isAccepted();
            event->setAccepted(isAccepted);

            // Update the last mouse event accepted state.
            d->lastMouseEvent.setAccepted(isAccepted);

            if (isAccepted)
                return;
        }
    }

#ifndef QT_NO_RUBBERBAND
    if (d->dragMode == QGraphicsView::RubberBandDrag && !d->rubberBanding) {
        if (d->sceneInteractionAllowed) {
            // Rubberbanding is only allowed in interactive mode.
            event->accept();
            d->rubberBanding = true;
            d->rubberBandRect = QRect();
            if (d->scene) {
                // Initiating a rubber band always clears the selection.
                d->scene->clearSelection();
            }
        }
    } else
#endif
        if (d->dragMode == QGraphicsView::ScrollHandDrag && event->button() == Qt::LeftButton) {
            // Left-button press in scroll hand mode initiates hand scrolling.
            event->accept();
            d->handScrolling = true;
            d->handScrollMotions = 0;
#ifndef QT_NO_CURSOR
            viewport()->setCursor(Qt::ClosedHandCursor);
#endif
        }
}

/*!
    \reimp
*/
void QGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    Q_D(QGraphicsView);

#ifndef QT_NO_RUBBERBAND
    if (d->dragMode == QGraphicsView::RubberBandDrag && d->sceneInteractionAllowed) {
        d->storeMouseEvent(event);
        if (d->rubberBanding) {
            // Check for enough drag distance
            if ((d->mousePressViewPoint - event->pos()).manhattanLength()
                < QApplication::startDragDistance()) {
                return;
            }

            // Update old rubberband
            if (d->viewportUpdateMode != QGraphicsView::NoViewportUpdate && !d->rubberBandRect.isNull()) {
                if (d->viewportUpdateMode != FullViewportUpdate)
                    viewport()->update(d->rubberBandRegion(viewport(), d->rubberBandRect));
                else
                    viewport()->update();
            }

            // Stop rubber banding if the user has let go of all buttons (even
            // if we didn't get the release events).
            if (!event->buttons()) {
                d->rubberBanding = false;
                d->rubberBandRect = QRect();
                return;
            }

            // Update rubberband position
            const QPoint &mp = d->mousePressViewPoint;
            QPoint ep = event->pos();
            d->rubberBandRect = QRect(qMin(mp.x(), ep.x()), qMin(mp.y(), ep.y()),
                                      qAbs(mp.x() - ep.x()) + 1, qAbs(mp.y() - ep.y()) + 1);

            // Update new rubberband
            if (d->viewportUpdateMode != QGraphicsView::NoViewportUpdate){
                if (d->viewportUpdateMode != FullViewportUpdate)
                    viewport()->update(d->rubberBandRegion(viewport(), d->rubberBandRect));
                else
                    viewport()->update();
            }
            // Set the new selection area
            QPainterPath selectionArea;
            selectionArea.addPolygon(mapToScene(d->rubberBandRect));
            selectionArea.closeSubpath();
            if (d->scene)
                d->scene->setSelectionArea(selectionArea, d->rubberBandSelectionMode);
            return;
        }
    } else
#endif // QT_NO_RUBBERBAND
    if (d->dragMode == QGraphicsView::ScrollHandDrag) {
        if (d->handScrolling) {
            QScrollBar *hBar = horizontalScrollBar();
            QScrollBar *vBar = verticalScrollBar();
            QPoint delta = event->pos() - d->lastMouseEvent.pos();
            hBar->setValue(hBar->value() + (isRightToLeft() ? delta.x() : -delta.x()));
            vBar->setValue(vBar->value() - delta.y());

            // Detect how much we've scrolled to disambiguate scrolling from
            // clicking.
            ++d->handScrollMotions;
        }
    }

    d->mouseMoveEventHandler(event);
}

/*!
    \reimp
*/
void QGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    Q_D(QGraphicsView);

#ifndef QT_NO_RUBBERBAND
    if (d->dragMode == QGraphicsView::RubberBandDrag && d->sceneInteractionAllowed && !event->buttons()) {
        if (d->rubberBanding) {
            if (d->viewportUpdateMode != QGraphicsView::NoViewportUpdate){
                if (d->viewportUpdateMode != FullViewportUpdate)
                    viewport()->update(d->rubberBandRegion(viewport(), d->rubberBandRect));
                else
                    viewport()->update();
            }
            d->rubberBanding = false;
            d->rubberBandRect = QRect();
        }
    } else
#endif
    if (d->dragMode == QGraphicsView::ScrollHandDrag) {
#ifndef QT_NO_CURSOR
        // Restore the open hand cursor. ### There might be items
        // under the mouse that have a valid cursor at this time, so
        // we could repeat the steps from mouseMoveEvent().
        viewport()->setCursor(Qt::OpenHandCursor);
#endif
        d->handScrolling = false;

        if (d->scene && d->sceneInteractionAllowed && !d->lastMouseEvent.isAccepted() && d->handScrollMotions <= 6) {
            // If we've detected very little motion during the hand drag, and
            // no item accepted the last event, we'll interpret that as a
            // click to the scene, and reset the selection.
            d->scene->clearSelection();
        }
    }

    d->storeMouseEvent(event);

    if (!d->sceneInteractionAllowed)
        return;

    if (!d->scene)
        return;

    QGraphicsSceneMouseEvent mouseEvent(QEvent::GraphicsSceneMouseRelease);
    mouseEvent.setWidget(viewport());
    mouseEvent.setButtonDownScenePos(d->mousePressButton, d->mousePressScenePoint);
    mouseEvent.setButtonDownScreenPos(d->mousePressButton, d->mousePressScreenPoint);
    mouseEvent.setScenePos(mapToScene(event->pos()));
    mouseEvent.setScreenPos(event->globalPos());
    mouseEvent.setLastScenePos(d->lastMouseMoveScenePoint);
    mouseEvent.setLastScreenPos(d->lastMouseMoveScreenPoint);
    mouseEvent.setButtons(event->buttons());
    mouseEvent.setButton(event->button());
    mouseEvent.setModifiers(event->modifiers());
    mouseEvent.setAccepted(false);
    QApplication::sendEvent(d->scene, &mouseEvent);

    // Update the last mouse event selected state.
    d->lastMouseEvent.setAccepted(mouseEvent.isAccepted());

#ifndef QT_NO_CURSOR
    if (mouseEvent.isAccepted() && mouseEvent.buttons() == 0 && viewport()->testAttribute(Qt::WA_SetCursor)) {
        // The last mouse release on the viewport will trigger clearing the cursor.
        d->_q_unsetViewportCursor();
    }
#endif
}

#ifndef QT_NO_WHEELEVENT
/*!
    \reimp
*/
void QGraphicsView::wheelEvent(QWheelEvent *event)
{
    Q_D(QGraphicsView);
    if (!d->scene || !d->sceneInteractionAllowed) {
        QAbstractScrollArea::wheelEvent(event);
        return;
    }

    event->ignore();

    QGraphicsSceneWheelEvent wheelEvent(QEvent::GraphicsSceneWheel);
    wheelEvent.setWidget(viewport());
    wheelEvent.setScenePos(mapToScene(event->pos()));
    wheelEvent.setScreenPos(event->globalPos());
    wheelEvent.setButtons(event->buttons());
    wheelEvent.setModifiers(event->modifiers());
    wheelEvent.setDelta(event->delta());
    wheelEvent.setOrientation(event->orientation());
    wheelEvent.setAccepted(false);
    QApplication::sendEvent(d->scene, &wheelEvent);
    event->setAccepted(wheelEvent.isAccepted());
    if (!event->isAccepted())
        QAbstractScrollArea::wheelEvent(event);
}
#endif // QT_NO_WHEELEVENT

/*!
    \reimp
*/
void QGraphicsView::paintEvent(QPaintEvent *event)
{
    Q_D(QGraphicsView);
    if (!d->scene) {
        QAbstractScrollArea::paintEvent(event);
        return;
    }

    // Set up painter state protection.
    d->scene->d_func()->painterStateProtection = !(d->optimizationFlags & DontSavePainterState);

    // Determine the exposed region
    QRegion exposedRegion = event->region();
    if (!d->accelerateScrolling)
        exposedRegion = viewport()->rect();
    else if (d->viewportUpdateMode == BoundingRectViewportUpdate)
        exposedRegion = event->rect();
    QRectF exposedSceneRect = mapToScene(exposedRegion.boundingRect().adjusted(0, 0, 1, 1)).boundingRect();

    // Set up the painter
    QPainter painter(viewport());
    QTransform original = painter.worldTransform();
#ifndef QT_NO_RUBBERBAND
    if (d->rubberBanding && !d->rubberBandRect.isNull())
        painter.save();
#endif
    // Set up render hints
    painter.setRenderHints(painter.renderHints(), false);
    painter.setRenderHints(d->renderHints, true);

    // Set up viewport transform
    const QTransform viewTransform = viewportTransform();
    painter.setTransform(viewTransform, true);

#ifdef QGRAPHICSVIEW_DEBUG
    QTime stopWatch;
    stopWatch.start();
    qDebug() << "QGraphicsView::paintEvent(" << exposedRegion << ")";
#endif

    // Find all exposed items
    bool allItems = false;
    QList<QGraphicsItem *> itemList = d->findItems(exposedRegion, viewTransform, &allItems);

#ifdef QGRAPHICSVIEW_DEBUG
    int exposedTime = stopWatch.elapsed();
#endif

    if ((d->cacheMode & CacheBackground)
#ifdef Q_WS_X11
        && X11->use_xrender
#endif
        ) {
        // Recreate the background pixmap, and flag the whole background as
        // exposed.
        if (d->mustResizeBackgroundPixmap) {
            d->backgroundPixmap = QPixmap(viewport()->size());
            QBrush bgBrush = viewport()->palette().brush(viewport()->backgroundRole());
            if (!bgBrush.isOpaque())
                d->backgroundPixmap.fill(Qt::transparent);
            QPainter p(&d->backgroundPixmap);
            p.fillRect(0, 0, d->backgroundPixmap.width(), d->backgroundPixmap.height(), bgBrush);
            d->backgroundPixmapExposed = QRegion(viewport()->rect());
            d->mustResizeBackgroundPixmap = false;
        }

        // Redraw exposed areas
        if (!d->backgroundPixmapExposed.isEmpty()) {
            QPainter backgroundPainter(&d->backgroundPixmap);
            backgroundPainter.setClipRegion(d->backgroundPixmapExposed, Qt::ReplaceClip);
            backgroundPainter.setTransform(viewportTransform());
            drawBackground(&backgroundPainter, exposedSceneRect);
            d->backgroundPixmapExposed = QRegion();
        }

        // Blit the background from the background pixmap
        QTransform oldMatrix = painter.worldTransform();
        painter.setWorldTransform(original);
        painter.drawPixmap(QPoint(), d->backgroundPixmap);
        painter.setWorldTransform(oldMatrix);
    } else {
        if (!(d->optimizationFlags & DontSavePainterState))
            painter.save();
        drawBackground(&painter, exposedSceneRect);
        if (!(d->optimizationFlags & DontSavePainterState))
            painter.restore();
    }

#ifdef QGRAPHICSVIEW_DEBUG
    int backgroundTime = stopWatch.elapsed() - exposedTime;
#endif

    // Generate the style options
    QGraphicsItem **itemArray = new QGraphicsItem *[itemList.size()];
    QStyleOptionGraphicsItem *styleOptionArray = d->allocStyleOptionsArray(itemList.size());

    d->generateStyleOptions(itemList, itemArray, styleOptionArray, viewTransform,
                            allItems, exposedRegion);

    // Items
    drawItems(&painter, itemList.size(), itemArray, styleOptionArray);

#ifdef QGRAPHICSVIEW_DEBUG
    int itemsTime = stopWatch.elapsed() - exposedTime - backgroundTime;
#endif

    // Foreground
    drawForeground(&painter, exposedSceneRect);

    delete [] itemArray;
    d->freeStyleOptionsArray(styleOptionArray);

#ifdef QGRAPHICSVIEW_DEBUG
    int foregroundTime = stopWatch.elapsed() - exposedTime - backgroundTime - itemsTime;
#endif

#ifndef QT_NO_RUBBERBAND
    // Rubberband
    if (d->rubberBanding && !d->rubberBandRect.isNull()) {
        painter.restore();
        QStyleOptionRubberBand option;
        option.initFrom(viewport());
        option.rect = d->rubberBandRect;
        option.shape = QRubberBand::Rectangle;

        QStyleHintReturnMask mask;
        if (viewport()->style()->styleHint(QStyle::SH_RubberBand_Mask, &option, viewport(), &mask)) {
            // painter clipping for masked rubberbands
            painter.setClipRegion(mask.region, Qt::IntersectClip);
        }

        viewport()->style()->drawControl(QStyle::CE_RubberBand, &option, &painter, viewport());
    }
#endif

    painter.end();

#ifdef QGRAPHICSVIEW_DEBUG
    qDebug() << "\tItem discovery....... " << exposedTime << "msecs (" << itemList.size() << "items,"
             << (exposedTime > 0 ? (itemList.size() * 1000.0 / exposedTime) : -1) << "/ sec )";
    qDebug() << "\tDrawing background... " << backgroundTime << "msecs (" << exposedRegion.numRects() << "segments )";
    qDebug() << "\tDrawing items........ " << itemsTime << "msecs ("
             << (itemsTime > 0 ? (itemList.size() * 1000.0 / itemsTime) : -1) << "/ sec )";
    qDebug() << "\tDrawing foreground... " << foregroundTime << "msecs (" << exposedRegion.numRects() << "segments )";
    qDebug() << "\tTotal rendering time: " << stopWatch.elapsed() << "msecs ("
             << (stopWatch.elapsed() > 0 ? (1000.0 / stopWatch.elapsed()) : -1.0) << "fps )";
#endif

    // Restore painter state protection.
    d->scene->d_func()->painterStateProtection = true;
}

/*!
    \reimp
*/
void QGraphicsView::resizeEvent(QResizeEvent *event)
{
    Q_D(QGraphicsView);
    // Save the last center point - the resize may scroll the view, which
    // changes the center point.
    QPointF oldLastCenterPoint = d->lastCenterPoint;

    QAbstractScrollArea::resizeEvent(event);
    d->recalculateContentSize();

    // Restore the center point again.
    if (d->resizeAnchor == NoAnchor && !d->keepLastCenterPoint) {
        d->updateLastCenterPoint();
    } else {
        d->lastCenterPoint = oldLastCenterPoint;
    }
    d->centerView(d->resizeAnchor);
    d->keepLastCenterPoint = false;

    if (d->cacheMode & CacheBackground) {
        // Invalidate the background pixmap
        d->mustResizeBackgroundPixmap = true;
    }
}

/*!
    \reimp
*/
void QGraphicsView::scrollContentsBy(int dx, int dy)
{
    Q_D(QGraphicsView);
    d->dirtyScroll = true;
    if (d->transforming)
        return;
    if (isRightToLeft())
        dx = -dx;

#ifndef QT_NO_RUBBERBAND
    // Update old rubberband
    if (d->viewportUpdateMode != QGraphicsView::NoViewportUpdate && !d->rubberBandRect.isNull()) {
        if (d->viewportUpdateMode != FullViewportUpdate)
            viewport()->update(d->rubberBandRegion(viewport(), d->rubberBandRect));
        else
            viewport()->update();
    }
#endif

    if (d->viewportUpdateMode != QGraphicsView::NoViewportUpdate){
        if (d->accelerateScrolling && d->viewportUpdateMode != FullViewportUpdate)
            viewport()->scroll(dx, dy);
        else
            viewport()->update();
    }
    d->updateLastCenterPoint();

    if ((d->cacheMode & CacheBackground)
#ifdef Q_WS_X11
        && X11->use_xrender
#endif
        ) {
        // Invalidate the background pixmap
        d->backgroundPixmapExposed.translate(dx, 0);
        if (dx > 0) {
            d->backgroundPixmapExposed += QRect(0, 0, dx, viewport()->height());
        } else if (dx < 0) {
            d->backgroundPixmapExposed += QRect(viewport()->width() + dx, 0,
                                                -dx, viewport()->height());
        }
        d->backgroundPixmapExposed.translate(0, dy);
        if (dy > 0) {
            d->backgroundPixmapExposed += QRect(0, 0, viewport()->width(), dy);
        } else if (dy < 0) {
            d->backgroundPixmapExposed += QRect(0, viewport()->height() + dy,
                                                viewport()->width(), -dy);
        }

        // Scroll the background pixmap
        if (!d->backgroundPixmap.isNull()) {
            QPixmap tmp = d->backgroundPixmap.copy();
            QBrush bgBrush = viewport()->palette().brush(viewport()->backgroundRole());
            if (!bgBrush.isOpaque())
                d->backgroundPixmap.fill(Qt::transparent);
            QPainter painter(&d->backgroundPixmap);
            painter.drawPixmap(dx, dy, tmp);
        }
    }

    // Always replay on scroll.
    if (d->sceneInteractionAllowed)
        d->replayLastMouseEvent();
}

/*!
    \reimp
*/
void QGraphicsView::showEvent(QShowEvent *event)
{
    Q_D(QGraphicsView);
    d->recalculateContentSize();
    d->centerView(d->transformationAnchor);
    QAbstractScrollArea::showEvent(event);
}

/*!
    \reimp
*/
void QGraphicsView::inputMethodEvent(QInputMethodEvent *event)
{
    Q_D(QGraphicsView);
    if (d->scene)
        QApplication::sendEvent(d->scene, event);
}

/*!
    Draws the background of the scene using \a painter, before any items and
    the foreground are drawn. Reimplement this function to provide a custom
    background for this view.

    If all you want is to define a color, texture or gradient for the
    background, you can call setBackgroundBrush() instead.

    All painting is done in \e scene coordinates. \a rect is the exposed
    rectangle.

    The default implementation fills \a rect using the view's backgroundBrush.
    If no such brush is defined (the default), the scene's drawBackground()
    function is called instead.

    \sa drawForeground(), QGraphicsScene::drawBackground()
*/
void QGraphicsView::drawBackground(QPainter *painter, const QRectF &rect)
{
    Q_D(QGraphicsView);
    if (d->scene && d->backgroundBrush.style() == Qt::NoBrush) {
        d->scene->drawBackground(painter, rect);
        return;
    }

    painter->fillRect(rect, d->backgroundBrush);
}

/*!
    Draws the foreground of the scene using \a painter, after the background
    and all items are drawn. Reimplement this function to provide a custom
    foreground for this view.

    If all you want is to define a color, texture or gradient for the
    foreground, you can call setForegroundBrush() instead.

    All painting is done in \e scene coordinates. \a rect is the exposed
    rectangle.

    The default implementation fills \a rect using the view's foregroundBrush.
    If no such brush is defined (the default), the scene's drawForeground()
    function is called instead.

    \sa drawBackground(), QGraphicsScene::drawForeground()
*/
void QGraphicsView::drawForeground(QPainter *painter, const QRectF &rect)
{
    Q_D(QGraphicsView);
    if (d->scene && d->foregroundBrush.style() == Qt::NoBrush) {
        d->scene->drawForeground(painter, rect);
        return;
    }

    painter->fillRect(rect, d->foregroundBrush);
}

/*!
    Draws the items \a items in the scene using \a painter, after the
    background and before the foreground are drawn. \a numItems is the number
    of items in \a items and options in \a options. \a options is a list of
    styleoptions; one for each item. Reimplement this function to provide
    custom item drawing for this view.

    The default implementation calls the scene's drawItems() function.

    \sa drawForeground(), drawBackground(), QGraphicsScene::drawItems()
*/
void QGraphicsView::drawItems(QPainter *painter, int numItems,
                              QGraphicsItem *items[],
                              const QStyleOptionGraphicsItem options[])
{
    Q_D(QGraphicsView);
    if (d->scene)
        d->scene->drawItems(painter, numItems, items, options, viewport());
}

/*!
    Returns the current transformation matrix for the view. If no current
    transformation is set, the identity matrix is returned.

    \sa setTransform(), rotate(), scale(), shear(), translate()
*/
QTransform QGraphicsView::transform() const
{
    Q_D(const QGraphicsView);
    return d->matrix;
}

/*!
    Returns a matrix that maps viewport coordinates to scene coordinates.

    \sa mapToScene(), mapFromScene()
*/
QTransform QGraphicsView::viewportTransform() const
{
    Q_D(const QGraphicsView);
    QTransform moveMatrix;
    moveMatrix.translate(-d->horizontalScroll(), -d->verticalScroll());
    return d->identityMatrix ? moveMatrix : d->matrix * moveMatrix;
}

/*!
    Sets the view's current transformation matrix to \a matrix.

    If \a combine is true, then \a matrix is combined with the current matrix;
    otherwise, \a matrix \e replaces the current matrix. \a combine is false
    by default.

    The transformation matrix tranforms the scene into view coordinates. Using
    the default transformation, provided by the identity matrix, one pixel in
    the view represents one unit in the scene (e.g., a 10x10 rectangular item
    is drawn using 10x10 pixels in the view). If a 2x2 scaling matrix is
    applied, the scene will be drawn in 1:2 (e.g., a 10x10 rectangular item is
    then drawn using 20x20 pixels in the view).

    Example:

    \snippet doc/src/snippets/code/src_gui_graphicsview_qgraphicsview.cpp 7

    To simplify interation with items using a transformed view, QGraphicsView
    provides mapTo... and mapFrom... functions that can translate between
    scene and view coordinates. For example, you can call mapToScene() to map
    a view coordiate to a floating point scene coordinate, or mapFromScene()
    to map from floating point scene coordinates to view coordinates.

    \sa transform(), rotate(), scale(), shear(), translate()
*/
void QGraphicsView::setTransform(const QTransform &matrix, bool combine )
{
    Q_D(QGraphicsView);
    QTransform oldMatrix = d->matrix;
    if (!combine)
        d->matrix = matrix;
    else
        d->matrix = matrix * d->matrix;
    if (oldMatrix == d->matrix)
        return;

    d->identityMatrix = d->matrix.isIdentity();
    d->transforming = true;
    if (d->scene) {
        d->recalculateContentSize();
        d->centerView(d->transformationAnchor);
    } else {
        d->updateLastCenterPoint();
    }

    if (d->sceneInteractionAllowed)
        d->replayLastMouseEvent();
    d->transforming = false;

    // Any matrix operation requires a full update.
    viewport()->update();
}

/*!
    Resets the view transformation to the identity matrix.

    \sa transform(), setTransform()
*/
void QGraphicsView::resetTransform()
{
    setTransform(QTransform());
}

QT_END_NAMESPACE

#include "moc_qgraphicsview.cpp"

#endif // QT_NO_GRAPHICSVIEW
