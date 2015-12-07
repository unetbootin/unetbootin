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

static const int QGRAPHICSSCENE_INDEXTIMER_TIMEOUT = 2000;

/*!
    \class QGraphicsScene
    \brief The QGraphicsScene class provides a surface for managing a large
    number of 2D graphical items.
    \since 4.2
    \ingroup multimedia
    \ingroup graphicsview-api
    \mainclass

    The class serves as a container for QGraphicsItems. It is used together
    with QGraphicsView for visualizing graphical items, such as lines,
    rectangles, text, or even custom items, on a 2D surface. QGraphicsScene is
    part of \l{The Graphics View Framework}.

    QGraphicsScene also provides functionality that lets you efficiently
    determine both the location of items, and for determining what items are
    visible within an arbitrary area on the scene. With the QGraphicsView
    widget, you can either visualize the whole scene, or zoom in and view only
    parts of the scene.

    Example:

    \snippet doc/src/snippets/code/src_gui_graphicsview_qgraphicsscene.cpp 0

    Note that QGraphicsScene has no visual appearance of its own; it only
    manages the items. You need to create a QGraphicsView widget to visualize
    the scene.

    To add items to a scene, you start off by constructing a QGraphicsScene
    object. Then, you have two options: either add your existing QGraphicsItem
    objects by calling addItem(), or you can call one of the convenience
    functions addEllipse(), addLine(), addPath(), addPixmap(), addPolygon(),
    addRect(), or addText(), which all return a pointer to the newly added item.
    The dimensions of the items added with these functions are relative to the
    item's coordinate system, and the items position is initialized to (0,
    0) in the scene.

    You can then visualize the scene using QGraphicsView. When the scene
    changes, (e.g., when an item moves or is transformed) QGraphicsScene
    emits the changed() signal. To remove an item, call removeItem().

    QGraphicsScene uses an indexing algorithm to manage the location of items
    efficiently. By default, a BSP (Binary Space Partitioning) tree is used; an
    algorithm suitable for large scenes where most items remain static (i.e.,
    do not move around). You can choose to disable this index by calling
    setItemIndexMethod(). For more information about the available indexing
    algorithms, see the itemIndexMethod property.

    The scene's bounding rect is set by calling setSceneRect(). Items can be
    placed at any position on the scene, and the size of the scene is by
    default unlimited. The scene rect is used only for internal bookkeeping,
    maintaining the scene's item index. If the scene rect is unset,
    QGraphicsScene will use the bounding area of all items, as returned by
    itemsBoundingRect(), as the scene rect. However, itemsBoundingRect() is a
    relatively time consuming function, as it operates by collecting
    positional information for every item on the scene. Because of this, you
    should always set the scene rect when operating on large scenes.

    One of QGraphicsScene's greatest strengths is its ability to efficiently
    determine the location of items. Even with millions of items on the scene,
    the items() functions can determine the location of an item within few
    milliseconds. There are several overloads to items(): one that finds items
    at a certain position, one that finds items inside or intersecting with a
    polygon or a rectangle, and more. The list of returned items is sorted by
    stacking order, with the topmost item being the first item in the list.
    For convenience, there is also an itemAt() function that returns the
    topmost item at a given position.

    QGraphicsScene maintains selection information for the scene. To select
    items, call setSelectionArea(), and to clear the current selection, call
    clearSelection(). Call selectedItems() to get the list of all selected
    items.

    \section1 Event Handling and Propagation

    Another responsibility that QGraphicsScene has, is to propagate events
    from QGraphicsView. To send an event to a scene, you construct an event
    that inherits QEvent, and then send it using, for example,
    QApplication::sendEvent(). event() is responsible for dispatching
    the event to the individual items. Some common events are handled by
    convenience event handlers. For example, key press events are handled by
    keyPressEvent(), and mouse press events are handled by mousePressEvent().

    Key events are delivered to the \e {focus item}. To set the focus item,
    you can either call setFocusItem(), passing an item that accepts focus, or
    the item itself can call QGraphicsItem::setFocus().  Call focusItem() to
    get the current focus item. For compatibility with widgets, the scene also
    maintains its own focus information. By default, the scene does not have
    focus, and all key events are discarded. If setFocus() is called, or if an
    item on the scene gains focus, the scene automatically gains focus. If the
    scene has focus, hasFocus() will return true, and key events will be
    forwarded to the focus item, if any. If the scene loses focus, (i.e.,
    someone calls clearFocus(),) while an item has focus, the scene will
    maintain its item focus information, and once the scene regains focus, it
    will make sure the last focus item regains focus.

    For mouse-over effects, QGraphicsScene dispatches \e {hover
    events}. If an item accepts hover events (see
    QGraphicsItem::acceptHoverEvents()), it will receive a \l
    {QEvent::}{GraphicsSceneHoverEnter} event when the mouse enters
    its area. As the mouse continues moving inside the item's area,
    QGraphicsScene will send it \l {QEvent::}{GraphicsSceneHoverMove}
    events. When the mouse leaves the item's area, the item will
    receive a \l {QEvent::}{GraphicsSceneHoverLeave} event.

    All mouse events are delivered to the current \e {mouse grabber}
    item. An item becomes the scene's mouse grabber if it accepts
    mouse events (see QGraphicsItem::acceptedMouseButtons()) and it
    receives a mouse press. It stays the mouse grabber until it
    receives a mouse release when no other mouse buttons are
    pressed. You can call mouseGrabberItem() to determine what item is
    currently grabbing the mouse.

    \sa QGraphicsItem, QGraphicsView
*/

/*!
    \enum QGraphicsScene::SceneLayer
    \since 4.3

    This enum describes the rendering layers in a QGraphicsScene. When
    QGraphicsScene draws the scene contents, it renders each of these layers
    separately, in order.

    Each layer represents a flag that can be OR'ed together when calling
    functions such as invalidate() or QGraphicsView::invalidateScene().

    \value ItemLayer The item layer. QGraphicsScene renders all items are in
    this layer by calling the virtual function drawItems(). The item layer is
    drawn after the background layer, but before the foreground layer.

    \value BackgroundLayer The background layer. QGraphicsScene renders the
    scene's background in this layer by calling the virtual function
    drawBackground(). The background layer is drawn first of all layers.

    \value ForegroundLayer The foreground layer. QGraphicsScene renders the
    scene's foreground in this layer by calling the virtual function
    drawForeground().  The foreground layer is drawn last of all layers.

    \value AllLayers All layers; this value represents a combination of all
    three layers.

    \sa invalidate(), QGraphicsView::invalidateScene()
*/

/*!
    \enum QGraphicsScene::ItemIndexMethod

    This enum describes the indexing algorithms QGraphicsScene provides for
    managing positional information about items on the scene.

    \value BspTreeIndex A Binary Space Partitioning tree is applied. All
    QGraphicsScene's item location algorithms are of an order close to
    logarithmic complexity, by making use of binary search. Adding, moving and
    removing items is logarithmic. This approach is best for static scenes
    (i.e., scenes where most items do not move).

    \value NoIndex No index is applied. Item location is of linear complexity,
    as all items on the scene are searched. Adding, moving and removing items,
    however, is done in constant time. This approach is ideal for dynamic
    scenes, where many items are added, moved or removed continuously.

    \sa setItemIndexMethod(), bspTreeDepth
*/

#include "qgraphicsscene.h"

#ifndef QT_NO_GRAPHICSVIEW

#include "qgraphicsitem.h"
#include "qgraphicsitem_p.h"
#include "qgraphicslayout.h"
#include "qgraphicsscene_p.h"
#include "qgraphicssceneevent.h"
#include "qgraphicsview.h"
#include "qgraphicsview_p.h"
#include "qgraphicswidget.h"
#include "qgraphicswidget_p.h"

#include <QtCore/qdebug.h>
#include <QtCore/qlist.h>
#include <QtCore/qmath.h>
#include <QtCore/qrect.h>
#include <QtCore/qset.h>
#include <QtCore/qstack.h>
#include <QtCore/qtimer.h>
#include <QtCore/qvarlengtharray.h>
#include <QtGui/qapplication.h>
#include <QtGui/qdesktopwidget.h>
#include <QtGui/qevent.h>
#include <QtGui/qgraphicslayout.h>
#include <QtGui/qgraphicsproxywidget.h>
#include <QtGui/qgraphicswidget.h>
#include <QtGui/qmatrix.h>
#include <QtGui/qpaintengine.h>
#include <QtGui/qpainter.h>
#include <QtGui/qpixmapcache.h>
#include <QtGui/qpolygon.h>
#include <QtGui/qstyleoption.h>
#include <QtGui/qtooltip.h>
#include <QtGui/qtransform.h>
#include <private/qapplication_p.h>
#include <private/qobject_p.h>
#ifdef Q_WS_X11
#include <private/qt_x11_p.h>
#endif

QT_BEGIN_NAMESPACE

static inline bool QRectF_intersects(const QRectF &s, const QRectF &r)
{
    qreal xp = s.left();
    qreal yp = s.top();
    qreal w = s.width();
    qreal h = s.height();
    qreal l1 = xp;
    qreal r1 = xp;
    if (w < 0)
        l1 += w;
    else
        r1 += w;

    qreal l2 = r.left();
    qreal r2 = r.left();
    if (w < 0)
        l2 += r.width();
    else
        r2 += r.width();

    if (l1 >= r2 || l2 >= r1)
        return false;

    qreal t1 = yp;
    qreal b1 = yp;
    if (h < 0)
        t1 += h;
    else
        b1 += h;

    qreal t2 = r.top();
    qreal b2 = r.top();
    if (r.height() < 0)
        t2 += r.height();
    else
        b2 += r.height();

    return !(t1 >= b2 || t2 >= b1);
}

// QRectF::intersects() returns false always if either the source or target
// rectangle's width or height are 0. This works around that problem.
static QRectF _q_adjustedRect(const QRectF &rect)
{
    static const qreal p = (qreal)0.00001;
    QRectF r = rect;
    if (!r.width())
        r.adjust(-p, 0, p, 0);
    if (!r.height())
        r.adjust(0, -p, 0, p);
    return r;
}

static void _q_hoverFromMouseEvent(QGraphicsSceneHoverEvent *hover, const QGraphicsSceneMouseEvent *mouseEvent)
{
    hover->setWidget(mouseEvent->widget());
    hover->setPos(mouseEvent->pos());
    hover->setScenePos(mouseEvent->scenePos());
    hover->setScreenPos(mouseEvent->screenPos());
    hover->setLastPos(mouseEvent->lastPos());
    hover->setLastScenePos(mouseEvent->lastScenePos());
    hover->setLastScreenPos(mouseEvent->lastScreenPos());
    hover->setModifiers(mouseEvent->modifiers());
    hover->setAccepted(mouseEvent->isAccepted());
}

/*!
    \internal
*/
QGraphicsScenePrivate::QGraphicsScenePrivate()
    : changedSignalMask(0),
      indexMethod(QGraphicsScene::BspTreeIndex),
      bspTreeDepth(0),
      lastItemCount(0),
      hasSceneRect(false),
      updateAll(false),
      calledEmitUpdated(false),
      selectionChanging(0),
      dirtyItemResetPending(false),
      regenerateIndex(true),
      purgePending(false),
      indexTimerId(0),
      restartIndexTimer(false),
      stickyFocus(false),
      hasFocus(false),
      focusItem(0),
      lastFocusItem(0),
      tabFocusFirst(0),
      activeWindow(0),
      activationRefCount(0),
      lastMouseGrabberItem(0),
      lastMouseGrabberItemHasImplicitMouseGrab(false),
      dragDropItem(0),
      enterWidget(0),
      lastDropAction(Qt::IgnoreAction),
      painterStateProtection(true),
      sortCacheEnabled(false),
      updatingSortCache(false),
      style(0)
{
}

/*!
    \internal
*/
void QGraphicsScenePrivate::init()
{
    Q_Q(QGraphicsScene);

    // Keep this index so we can check for connected slots later on.
    changedSignalMask = (1 << q->metaObject()->indexOfSignal("changed(QList<QRectF>)"));
    qApp->d_func()->scene_list.append(q);
    q->update();
}

/*!
    \internal
*/
QList<QGraphicsItem *> QGraphicsScenePrivate::estimateItemsInRect(const QRectF &rect) const
{
    const_cast<QGraphicsScenePrivate *>(this)->purgeRemovedItems();
    const_cast<QGraphicsScenePrivate *>(this)->_q_updateSortCache();

    if (indexMethod == QGraphicsScene::BspTreeIndex) {
        // ### Only do this once in a while.
        QGraphicsScenePrivate *that = const_cast<QGraphicsScenePrivate *>(this);

        // Get items from BSP tree
        QList<QGraphicsItem *> items = that->bspTree.items(rect);

        // Fill in with any unindexed items
        for (int i = 0; i < unindexedItems.size(); ++i) {
            if (QGraphicsItem *item = unindexedItems.at(i)) {
                if (!item->d_ptr->itemDiscovered && item->d_ptr->visible && !(item->d_ptr->ancestorFlags & QGraphicsItemPrivate::AncestorClipsChildren)) {
                    QRectF boundingRect = item->sceneBoundingRect();
                    if (QRectF_intersects(boundingRect, rect)) {
                        item->d_ptr->itemDiscovered = 1;
                        items << item;
                    }
                }
            }
        }

        // Reset the discovered state of all discovered items
        for (int i = 0; i < items.size(); ++i)
            items.at(i)->d_func()->itemDiscovered = 0;
        return items;
    }

    QList<QGraphicsItem *> itemsInRect;
    for (int i = 0; i < unindexedItems.size(); ++i) {
        if (QGraphicsItem *item = unindexedItems.at(i)) {
            if (item->d_ptr->ancestorFlags & QGraphicsItemPrivate::AncestorClipsChildren)
                continue;
            if (item->d_ptr->visible && item->effectiveOpacity() > qreal(0.0))
                itemsInRect << item;
        }
    }
    for (int i = 0; i < indexedItems.size(); ++i) {
        if (QGraphicsItem *item = indexedItems.at(i)) {
            if (item->d_ptr->ancestorFlags & QGraphicsItemPrivate::AncestorClipsChildren)
                continue;
            if (item->d_ptr->visible && item->effectiveOpacity() > qreal(0.0))
                itemsInRect << item;
        }
    }

    return itemsInRect;
}

/*!
    \internal
*/
void QGraphicsScenePrivate::addToIndex(QGraphicsItem *item)
{
    if (indexMethod == QGraphicsScene::BspTreeIndex) {
        if (item->d_func()->index != -1) {
            bspTree.insertItem(item, item->sceneBoundingRect());
            foreach (QGraphicsItem *child, item->children())
                child->addToIndex();
        } else {
            // The BSP tree is regenerated if the number of items grows to a
            // certain threshold, or if the bounding rect of the graph doubles in
            // size.
            startIndexTimer();
        }
    }
}

/*!
    \internal
*/
void QGraphicsScenePrivate::removeFromIndex(QGraphicsItem *item)
{
    if (indexMethod == QGraphicsScene::BspTreeIndex) {
        int index = item->d_func()->index;
        if (index != -1) {
            bspTree.removeItem(item, item->sceneBoundingRect());
            freeItemIndexes << index;
            indexedItems[index] = 0;
            item->d_func()->index = -1;
            unindexedItems << item;

            foreach (QGraphicsItem *child, item->children())
                child->removeFromIndex();
        }

        startIndexTimer();
    }
}

/*!
    \internal
*/
void QGraphicsScenePrivate::resetIndex()
{
    purgeRemovedItems();
    if (indexMethod == QGraphicsScene::BspTreeIndex) {
        for (int i = 0; i < indexedItems.size(); ++i) {
            if (QGraphicsItem *item = indexedItems.at(i)) {
                item->d_ptr->index = -1;
                unindexedItems << item;
            }
        }
        indexedItems.clear();
        freeItemIndexes.clear();
        regenerateIndex = true;
        startIndexTimer();
    }
}

static inline int intmaxlog(int n)
{
    return  (n > 0 ? qMax(qCeil(qLn(qreal(n)) / qLn(qreal(2))), 5) : 0);
}

/*!
    \internal
*/
void QGraphicsScenePrivate::_q_updateIndex()
{
    if (!indexTimerId)
        return;

    Q_Q(QGraphicsScene);
    q->killTimer(indexTimerId);
    indexTimerId = 0;

    purgeRemovedItems();

    // Add unindexedItems to indexedItems
    QRectF unindexedItemsBoundingRect;
    for (int i = 0; i < unindexedItems.size(); ++i) {
        if (QGraphicsItem *item = unindexedItems.at(i)) {
            unindexedItemsBoundingRect |= item->sceneBoundingRect();
            if (!freeItemIndexes.isEmpty()) {
                int freeIndex = freeItemIndexes.takeFirst();
                item->d_func()->index = freeIndex;
                indexedItems[freeIndex] = item;
            } else {
                item->d_func()->index = indexedItems.size();
                indexedItems << item;
            }
        }
    }

    // Update growing scene rect.
    QRectF oldGrowingItemsBoundingRect = growingItemsBoundingRect;
    growingItemsBoundingRect |= unindexedItemsBoundingRect;

    // Determine whether we should regenerate the BSP tree.
    if (indexMethod == QGraphicsScene::BspTreeIndex) {
        int depth = bspTreeDepth;
        if (depth == 0) {
            int oldDepth = intmaxlog(lastItemCount);
            depth = intmaxlog(indexedItems.size());
            static const int slack = 100;
            if (bspTree.leafCount() == 0 || (oldDepth != depth && qAbs(lastItemCount - indexedItems.size()) > slack)) {
                // ### Crude algorithm.
                regenerateIndex = true;
            }
        }

        // Regenerate the tree.
        if (regenerateIndex) {
            regenerateIndex = false;
            bspTree.initialize(q->sceneRect(), depth);
            unindexedItems = indexedItems;
            lastItemCount = indexedItems.size();
            q->update();

            // Take this opportunity to reset our largest-item counter for
            // untransformable items. When the items are inserted into the BSP
            // tree, we'll get an accurate calculation.
            largestUntransformableItem = QRectF();
        }
    }

    // Insert all unindexed items into the tree.
    for (int i = 0; i < unindexedItems.size(); ++i) {
        if (QGraphicsItem *item = unindexedItems.at(i)) {
            QRectF rect = item->sceneBoundingRect();
            if (item->d_ptr->ancestorFlags & QGraphicsItemPrivate::AncestorClipsChildren)
                continue;
            if (indexMethod == QGraphicsScene::BspTreeIndex)
                bspTree.insertItem(item, rect);

            // If the item ignores view transformations, update our
            // largest-item-counter to ensure that the view can accurately
            // discover untransformable items when drawing.
            if (item->d_ptr->itemIsUntransformable()) {
                QGraphicsItem *topmostUntransformable = item;
                while (topmostUntransformable && (topmostUntransformable->d_ptr->ancestorFlags
                                                  & QGraphicsItemPrivate::AncestorIgnoresTransformations)) {
                    topmostUntransformable = topmostUntransformable->parentItem();
                }
                // ### Verify that this is the correct largest untransformable rectangle.
                largestUntransformableItem |= item->mapToItem(topmostUntransformable, item->boundingRect()).boundingRect();
            }
        }
    }
    unindexedItems.clear();

    // Notify scene rect changes.
    if (!hasSceneRect && growingItemsBoundingRect != oldGrowingItemsBoundingRect)
        emit q->sceneRectChanged(growingItemsBoundingRect);
}

/*!
    \internal
*/
void QGraphicsScenePrivate::_q_emitUpdated()
{
    Q_Q(QGraphicsScene);
    calledEmitUpdated = false;

    // Ensure all views are connected if anything is connected. This disables
    // the optimization that items send updates directly to the views, but it
    // needs to happen in order to keep compatibility with the behavior from
    // Qt 4.4 and backward.
    if (!views.isEmpty() && (connectedSignals & changedSignalMask)) {
        for (int i = 0; i < views.size(); ++i) {
            QGraphicsView *view = views.at(i);
            if (!view->d_func()->connectedToScene) {
                view->d_func()->connectedToScene = true;
                q->connect(q, SIGNAL(changed(QList<QRectF>)),
                           views.at(i), SLOT(updateScene(QList<QRectF>)));
            }
        }
    }

    // Ensure all dirty items's current positions are recorded in the list of
    // updated rects.
    for (int i = 0; i < dirtyItems.size(); ++i)
        updatedRects += dirtyItems.at(i)->sceneBoundingRect();

    // Notify the changes to anybody interested.
    QList<QRectF> oldUpdatedRects;
    oldUpdatedRects = updateAll ? (QList<QRectF>() << q->sceneRect()) : updatedRects;
    updateAll = false;
    updatedRects.clear();
    emit q->changed(oldUpdatedRects);
}

/*!
    \internal

    Updates all items in the pending update list. At this point, the list is
    unlikely to contain partially constructed items.
*/
void QGraphicsScenePrivate::_q_updateLater()
{
    foreach (QGraphicsItem *item, pendingUpdateItems)
        item->update();
    pendingUpdateItems.clear();
}

/*!
    \internal
*/
void QGraphicsScenePrivate::_q_polishItems()
{
    foreach (QGraphicsItem *item, unpolishedItems) {
        if (!item->d_ptr->explicitlyHidden) {
            item->itemChange(QGraphicsItem::ItemVisibleChange, true);
            item->itemChange(QGraphicsItem::ItemVisibleHasChanged, true);
        }
        if (item->isWidget()) {
            QEvent event(QEvent::Polish);
            QApplication::sendEvent((QGraphicsWidget *)item, &event);
        }
    }
    unpolishedItems.clear();
}

/*!
    \internal
*/
void QGraphicsScenePrivate::_q_resetDirtyItems()
{
    for (int i = 0; i < dirtyItems.size(); ++i) {
        QGraphicsItem *item = dirtyItems.at(i);
        item->d_ptr->dirty = 0;
        item->d_ptr->dirtyChildren = 0;
    }
    dirtyItems.clear();
    dirtyItemResetPending = false;
}

/*!
    \internal
*/
void QGraphicsScenePrivate::resetDirtyItemsLater()
{
    Q_Q(QGraphicsScene);
    if (dirtyItemResetPending)
        return;
    // dirtyItems.reserve(indexedItems.size() + unindexedItems.size());
    dirtyItemResetPending = true;
    QMetaObject::invokeMethod(q, "_q_resetDirtyItems", Qt::QueuedConnection);
}

/*!
    \internal

    Schedules an item for removal. This function leaves some stale indexes
    around in the BSP tree; these will be cleaned up the next time someone
    triggers purgeRemovedItems().

    Note: This function is called from QGraphicsItem's destructor. \a item is
    being destroyed, so we cannot call any pure virtual functions on it (such
    as boundingRect()). Also, it is unnecessary to update the item's own state
    in any way.

    ### Refactoring: This function shares much functionality with removeItem()
*/
void QGraphicsScenePrivate::_q_removeItemLater(QGraphicsItem *item)
{
    Q_Q(QGraphicsScene);

    if (QGraphicsItem *parent = item->d_func()->parent) {
        QVariant variant;
        qVariantSetValue<QGraphicsItem *>(variant, item);
        parent->itemChange(QGraphicsItem::ItemChildRemovedChange, variant);
        parent->d_func()->children.removeAll(item);
    }

    // Clear focus on the item to remove any reference in the focusWidget
    // chain.
    item->clearFocus();

    int index = item->d_func()->index;
    if (index != -1) {
        // Important: The index is useless until purgeRemovedItems() is
        // called.
        indexedItems[index] = (QGraphicsItem *)0;
        if (!purgePending) {
            purgePending = true;
            q->update();
        }
        removedItems << item;
    } else {
        // Recently added items are purged immediately. unindexedItems() never
        // contains stale items.
        unindexedItems.removeAll(item);
        q->update();
    }

    // Reset the mouse grabber and focus item data.
    if (item == focusItem)
        focusItem = 0;
    if (item == lastFocusItem)
        lastFocusItem = 0;
    if (item == activeWindow) {
        // ### deactivate...
        activeWindow = 0;
    }

    // Disable selectionChanged() for individual items
    ++selectionChanging;
    int oldSelectedItemsSize = selectedItems.size();

    // Update selected & hovered item bookkeeping
    selectedItems.remove(item);
    hoverItems.removeAll(item);
    pendingUpdateItems.removeAll(item);
    cachedItemsUnderMouse.removeAll(item);
    unpolishedItems.removeAll(item);
    dirtyItems.removeAll(item);

    // Remove from scene transform cache
    int transformIndex = item->d_func()->sceneTransformIndex;
    if (transformIndex != -1) {
        validTransforms.setBit(transformIndex, 0);
        freeSceneTransformSlots.append(transformIndex);
    }

    // Remove all children recursively.
    foreach (QGraphicsItem *child, item->children())
        _q_removeItemLater(child);

    // Reset the mouse grabber
    if (mouseGrabberItems.contains(item))
        ungrabMouse(item, /* item is dying */ true);

    // Reset the keyboard grabber
    if (keyboardGrabberItems.contains(item))
        ungrabKeyboard(item, /* item is dying */ true);

    // Reset the last mouse grabber item
    if (item == lastMouseGrabberItem)
        lastMouseGrabberItem = 0;

    // Reenable selectionChanged() for individual items
    --selectionChanging;
    if (!selectionChanging && selectedItems.size() != oldSelectedItemsSize)
        emit q->selectionChanged();
}

/*!
    \internal

    Removes stale pointers from all data structures.
*/
void QGraphicsScenePrivate::purgeRemovedItems()
{
    Q_Q(QGraphicsScene);

    if (!purgePending && removedItems.isEmpty())
        return;

    // Remove stale items from the BSP tree.
    if (indexMethod != QGraphicsScene::NoIndex)
        bspTree.removeItems(removedItems);

    // Purge this list.
    removedItems.clear();
    freeItemIndexes.clear();
    for (int i = 0; i < indexedItems.size(); ++i) {
        if (!indexedItems.at(i))
            freeItemIndexes << i;
    }
    purgePending = false;

    // No locality info for the items; update the whole scene.
    q->update();
}

/*!
    \internal

    Starts or restarts the timer used for reindexing unindexed items.
*/
void QGraphicsScenePrivate::startIndexTimer()
{
    Q_Q(QGraphicsScene);
    if (indexTimerId) {
        restartIndexTimer = true;
    } else {
        indexTimerId = q->startTimer(QGRAPHICSSCENE_INDEXTIMER_TIMEOUT);
    }
}

/*!
    \internal
*/
void QGraphicsScenePrivate::addPopup(QGraphicsWidget *widget)
{
    Q_ASSERT(widget);
    Q_ASSERT(!popupWidgets.contains(widget));
    popupWidgets << widget;
    if (QGraphicsWidget *focusWidget = widget->focusWidget()) {
        focusWidget->setFocus(Qt::PopupFocusReason);
    } else {
        grabKeyboard((QGraphicsItem *)widget);
        if (focusItem && popupWidgets.size() == 1) {
            QFocusEvent event(QEvent::FocusOut, Qt::PopupFocusReason);
            sendEvent(focusItem, &event);
        }
    }
    grabMouse((QGraphicsItem *)widget);
}

/*!
    \internal

    Remove \a widget from the popup list. Important notes:

    \a widget is guaranteed to be in the list of popups, but it might not be
    the last entry; you can hide any item in the pop list before the others,
    and this must cause all later mouse grabbers to lose the grab.
*/
void QGraphicsScenePrivate::removePopup(QGraphicsWidget *widget, bool itemIsDying)
{
    Q_ASSERT(widget);
    int index = popupWidgets.indexOf(widget);
    Q_ASSERT(index != -1);

    for (int i = popupWidgets.size() - 1; i >= index; --i) {
        QGraphicsWidget *widget = popupWidgets.takeLast();
        ungrabMouse(widget, itemIsDying);
        if (focusItem && popupWidgets.isEmpty()) {
            QFocusEvent event(QEvent::FocusIn, Qt::PopupFocusReason);
            sendEvent(focusItem, &event);
        } else {
            ungrabKeyboard((QGraphicsItem *)widget, itemIsDying);
        }
        if (!itemIsDying && widget->isVisible()) {
            widget->hide();
            widget->QGraphicsItem::d_ptr->explicitlyHidden = 0;
        }
    }
}

/*!
    \internal
*/
void QGraphicsScenePrivate::grabMouse(QGraphicsItem *item, bool implicit)
{
    // Append to list of mouse grabber items, and send a mouse grab event.
    if (mouseGrabberItems.contains(item)) {
        if (mouseGrabberItems.last() == item)
            qWarning("QGraphicsItem::grabMouse: already a mouse grabber");
        else
            qWarning("QGraphicsItem::grabMouse: already blocked by mouse grabber: %p",
                     mouseGrabberItems.last());
        return;
    }

    // Send ungrab event to the last grabber.
    if (!mouseGrabberItems.isEmpty()) {
        QGraphicsItem *last = mouseGrabberItems.last();
        if (lastMouseGrabberItemHasImplicitMouseGrab) {
            // Implicit mouse grab is immediately lost.
            last->ungrabMouse();
        } else {
            // Just send ungrab event to current grabber.
            QEvent ungrabEvent(QEvent::UngrabMouse);
            sendEvent(last, &ungrabEvent);
        }
    }

    mouseGrabberItems << item;
    lastMouseGrabberItemHasImplicitMouseGrab = implicit;

    // Send grab event to current grabber.
    QEvent grabEvent(QEvent::GrabMouse);
    sendEvent(item, &grabEvent);
}

/*!
    \internal
*/
void QGraphicsScenePrivate::ungrabMouse(QGraphicsItem *item, bool itemIsDying)
{
    int index = mouseGrabberItems.indexOf(item);
    if (index == -1) {
        qWarning("QGraphicsItem::ungrabMouse: not a mouse grabber");
        return;
    }

    if (item != mouseGrabberItems.last()) {
        // Recursively ungrab the next mouse grabber until we reach this item
        // to ensure state consistency.
        ungrabMouse(mouseGrabberItems.at(index + 1), itemIsDying);
    }
    if (!popupWidgets.isEmpty() && item == popupWidgets.last()) {
        // If the item is a popup, go via removePopup to ensure state
        // consistency and that it gets hidden correctly - beware that
        // removePopup() reenters this function to continue removing the grab.
        removePopup((QGraphicsWidget *)item, itemIsDying);
        return;
    }

    // Send notification about mouse ungrab.
    if (!itemIsDying) {
        QEvent event(QEvent::UngrabMouse);
        sendEvent(item, &event);
    }

    // Remove the item from the list of grabbers. Whenever this happens, we
    // reset the implicitGrab (there can be only ever be one implicit grabber
    // in a scene, and it is always the latest grabber; if the implicit grab
    // is lost, it is not automatically regained.
    mouseGrabberItems.takeLast();
    lastMouseGrabberItemHasImplicitMouseGrab = false;

    // Send notification about mouse regrab. ### It's unfortunate that all the
    // items get a GrabMouse event, but this is a rare case with a simple
    // implementation and it does ensure a consistent state.
    if (!itemIsDying && !mouseGrabberItems.isEmpty()) {
        QGraphicsItem *last = mouseGrabberItems.last();
        QEvent event(QEvent::GrabMouse);
        sendEvent(last, &event);
    }
}

/*!
    \internal
*/
void QGraphicsScenePrivate::clearMouseGrabber()
{
    if (!mouseGrabberItems.isEmpty())
        mouseGrabberItems.first()->ungrabMouse();
    lastMouseGrabberItem = 0;
}

/*!
    \internal
*/
void QGraphicsScenePrivate::grabKeyboard(QGraphicsItem *item)
{
    if (keyboardGrabberItems.contains(item)) {
        if (keyboardGrabberItems.last() == item)
            qWarning("QGraphicsItem::grabKeyboard: already a keyboard grabber");
        else
            qWarning("QGraphicsItem::grabKeyboard: already blocked by keyboard grabber: %p",
                     keyboardGrabberItems.last());
        return;
    }

    // Send ungrab event to the last grabber.
    if (!keyboardGrabberItems.isEmpty()) {
        // Just send ungrab event to current grabber.
        QEvent ungrabEvent(QEvent::UngrabKeyboard);
        sendEvent(keyboardGrabberItems.last(), &ungrabEvent);
    }

    keyboardGrabberItems << item;

    // Send grab event to current grabber.
    QEvent grabEvent(QEvent::GrabKeyboard);
    sendEvent(item, &grabEvent);
}

/*!
    \internal
*/
void QGraphicsScenePrivate::ungrabKeyboard(QGraphicsItem *item, bool itemIsDying)
{
    int index = keyboardGrabberItems.lastIndexOf(item);
    if (index == -1) {
        qWarning("QGraphicsItem::ungrabKeyboard: not a keyboard grabber");
        return;
    }
    if (item != keyboardGrabberItems.last()) {
        // Recursively ungrab the topmost keyboard grabber until we reach this
        // item to ensure state consistency.
        ungrabKeyboard(keyboardGrabberItems.at(index + 1), itemIsDying);
    }

    // Send notification about keyboard ungrab.
    if (!itemIsDying) {
        QEvent event(QEvent::UngrabKeyboard);
        sendEvent(item, &event);
    }

    // Remove the item from the list of grabbers.
    keyboardGrabberItems.takeLast();

    // Send notification about mouse regrab.
    if (!itemIsDying && !keyboardGrabberItems.isEmpty()) {
        QGraphicsItem *last = keyboardGrabberItems.last();
        QEvent event(QEvent::GrabKeyboard);
        sendEvent(last, &event);
    }
}

/*!
    \internal
*/
void QGraphicsScenePrivate::clearKeyboardGrabber()
{
    if (!keyboardGrabberItems.isEmpty())
        ungrabKeyboard(keyboardGrabberItems.first());
}

/*!
    Returns all items for the screen position in \a event.
*/
QList<QGraphicsItem *> QGraphicsScenePrivate::itemsAtPosition(const QPoint &screenPos,
                                                              const QPointF &scenePos,
                                                              QWidget *widget) const
{
    Q_Q(const QGraphicsScene);
    QGraphicsView *view = widget ? qobject_cast<QGraphicsView *>(widget->parentWidget()) : 0;
    QList<QGraphicsItem *> items;
    if (view)
        items = view->items(view->viewport()->mapFromGlobal(screenPos));
    else
        items = q->items(scenePos);
    return items;
}

/*!
    \internal

    Checks if item collides with the path and mode, but also checks that if it
    doesn't collide, maybe its frame rect will.
*/
bool QGraphicsScenePrivate::itemCollidesWithPath(QGraphicsItem *item,
                                                 const QPainterPath &path,
                                                 Qt::ItemSelectionMode mode)
{
    if (item->collidesWithPath(path, mode))
        return true;
    if (item->isWidget()) {
        // Check if this is a window, and if its frame rect collides.
        QGraphicsWidget *widget = static_cast<QGraphicsWidget *>(item);
        if (widget->isWindow()) {
            QRectF frameRect = widget->windowFrameRect();
            QPainterPath framePath;
            framePath.addRect(frameRect);
            bool intersects = path.intersects(frameRect);
            if (mode == Qt::IntersectsItemShape || mode == Qt::IntersectsItemBoundingRect)
                return intersects || path.contains(frameRect.topLeft())
                    || framePath.contains(path.elementAt(0));
            return !intersects && path.contains(frameRect.topLeft());
        }
    }
    return false;
}

/*!
    \internal
*/
void QGraphicsScenePrivate::storeMouseButtonsForMouseGrabber(QGraphicsSceneMouseEvent *event)
{
    for (int i = 0x1; i <= 0x10; i <<= 1) {
        if (event->buttons() & i) {
            mouseGrabberButtonDownPos.insert(Qt::MouseButton(i),
                                             mouseGrabberItems.last()->d_ptr->genericMapFromScene(event->scenePos(),
                                                                                          event->widget()));
            mouseGrabberButtonDownScenePos.insert(Qt::MouseButton(i), event->scenePos());
            mouseGrabberButtonDownScreenPos.insert(Qt::MouseButton(i), event->screenPos());
        }
    }
}

/*!
    \internal
*/
void QGraphicsScenePrivate::installSceneEventFilter(QGraphicsItem *watched, QGraphicsItem *filter)
{
    sceneEventFilters.insert(watched, filter);
}

/*!
    \internal
*/
void QGraphicsScenePrivate::removeSceneEventFilter(QGraphicsItem *watched, QGraphicsItem *filter)
{
    if (!sceneEventFilters.contains(watched))
        return;

    QMultiMap<QGraphicsItem *, QGraphicsItem *>::Iterator it = sceneEventFilters.lowerBound(watched);
    QMultiMap<QGraphicsItem *, QGraphicsItem *>::Iterator end = sceneEventFilters.upperBound(watched);
    do {
        if (it.value() == filter)
            it = sceneEventFilters.erase(it);
        else
            ++it;
    } while (it != end);
}

/*!
    \internal
*/
bool QGraphicsScenePrivate::filterEvent(QGraphicsItem *item, QEvent *event)
{
    if (item && !sceneEventFilters.contains(item))
        return false;

    QMultiMap<QGraphicsItem *, QGraphicsItem *>::Iterator it = sceneEventFilters.lowerBound(item);
    QMultiMap<QGraphicsItem *, QGraphicsItem *>::Iterator end = sceneEventFilters.upperBound(item);
    while (it != end) {
        // ### The filterer and filteree might both be deleted.
        if (it.value()->sceneEventFilter(it.key(), event))
            return true;
        ++it;
    }
    return false;
}

/*!
    \internal

    This is the final dispatch point for any events from the scene to the
    item. It filters the event first - if the filter returns true, the event
    is considered to have been eaten by the filter, and is therefore stopped
    (the default filter returns false). Then/otherwise, if the item is
    enabled, the event is sent; otherwise it is stopped.
*/
bool QGraphicsScenePrivate::sendEvent(QGraphicsItem *item, QEvent *event)
{
    if (filterEvent(item, event))
        return false;
    return (item && item->isEnabled()) ? item->sceneEvent(event) : false;
}

/*!
    \internal
*/
void QGraphicsScenePrivate::cloneDragDropEvent(QGraphicsSceneDragDropEvent *dest,
                                               QGraphicsSceneDragDropEvent *source)
{
    dest->setWidget(source->widget());
    dest->setPos(source->pos());
    dest->setScenePos(source->scenePos());
    dest->setScreenPos(source->screenPos());
    dest->setButtons(source->buttons());
    dest->setModifiers(source->modifiers());
    dest->setPossibleActions(source->possibleActions());
    dest->setProposedAction(source->proposedAction());
    dest->setDropAction(source->dropAction());
    dest->setSource(source->source());
    dest->setMimeData(source->mimeData());
}

/*!
    \internal
*/
void QGraphicsScenePrivate::sendDragDropEvent(QGraphicsItem *item,
                                              QGraphicsSceneDragDropEvent *dragDropEvent)
{
    dragDropEvent->setPos(item->d_ptr->genericMapFromScene(dragDropEvent->scenePos(), dragDropEvent->widget()));
    sendEvent(item, dragDropEvent);
}

/*!
    \internal
*/
void QGraphicsScenePrivate::sendHoverEvent(QEvent::Type type, QGraphicsItem *item,
                                           QGraphicsSceneHoverEvent *hoverEvent)
{
    QGraphicsSceneHoverEvent event(type);
    event.setWidget(hoverEvent->widget());
    event.setPos(item->d_ptr->genericMapFromScene(hoverEvent->scenePos(), hoverEvent->widget()));
    event.setScenePos(hoverEvent->scenePos());
    event.setScreenPos(hoverEvent->screenPos());
    event.setLastPos(item->d_ptr->genericMapFromScene(hoverEvent->lastScenePos(), hoverEvent->widget()));
    event.setLastScenePos(hoverEvent->lastScenePos());
    event.setLastScreenPos(hoverEvent->lastScreenPos());
    event.setModifiers(hoverEvent->modifiers());
    sendEvent(item, &event);
}

/*!
    \internal
*/
void QGraphicsScenePrivate::sendMouseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (mouseEvent->button() == 0 && mouseEvent->buttons() == 0 && lastMouseGrabberItemHasImplicitMouseGrab) {
        // ### This is a temporary fix for until we get proper mouse
        // grab events.
        clearMouseGrabber();
        return;
    }

    QGraphicsItem *item = mouseGrabberItems.last();
    for (int i = 0x1; i <= 0x10; i <<= 1) {
        Qt::MouseButton button = Qt::MouseButton(i);
        mouseEvent->setButtonDownPos(button, mouseGrabberButtonDownPos.value(button, item->d_ptr->genericMapFromScene(mouseEvent->scenePos(), mouseEvent->widget())));
        mouseEvent->setButtonDownScenePos(button, mouseGrabberButtonDownScenePos.value(button, mouseEvent->scenePos()));
        mouseEvent->setButtonDownScreenPos(button, mouseGrabberButtonDownScreenPos.value(button, mouseEvent->screenPos()));
    }
    mouseEvent->setPos(item->d_ptr->genericMapFromScene(mouseEvent->scenePos(), mouseEvent->widget()));
    mouseEvent->setLastPos(item->d_ptr->genericMapFromScene(mouseEvent->lastScenePos(), mouseEvent->widget()));
    sendEvent(item, mouseEvent);
}

/*!
    \internal
*/
void QGraphicsScenePrivate::mousePressEventHandler(QGraphicsSceneMouseEvent *mouseEvent)
{
    Q_Q(QGraphicsScene);

    // Ignore by default, unless we find a mouse grabber that accepts it.
    mouseEvent->ignore();

    // Deliver to any existing mouse grabber.
    if (!mouseGrabberItems.isEmpty()) {
        // The event is ignored by default, but we disregard the event's
        // accepted state after delivery; the mouse is grabbed, after all.
        sendMouseEvent(mouseEvent);
        return;
    }

    // Start by determining the number of items at the current position.
    // Reuse value from earlier calculations if possible.
    if (cachedItemsUnderMouse.isEmpty()) {
        cachedItemsUnderMouse = itemsAtPosition(mouseEvent->screenPos(),
                                                mouseEvent->scenePos(),
                                                mouseEvent->widget());
    }

    // Update window activation.
    QGraphicsWidget *newActiveWindow = windowForItem(cachedItemsUnderMouse.value(0));
    if (newActiveWindow != activeWindow)
        q->setActiveWindow(newActiveWindow);

    // Set focus on the topmost enabled item that can take focus.
    bool setFocus = false;
    foreach (QGraphicsItem *item, cachedItemsUnderMouse) {
        if (item->isEnabled() && (item->flags() & QGraphicsItem::ItemIsFocusable)) {
            if (!item->isWidget() || ((QGraphicsWidget *)item)->focusPolicy() & Qt::ClickFocus) {
                setFocus = true;
                if (item != q->focusItem())
                    q->setFocusItem(item, Qt::MouseFocusReason);
                break;
            }
        }
    }

    // If nobody could take focus, clear it.
    if (!stickyFocus && !setFocus)
        q->setFocusItem(0, Qt::MouseFocusReason);

    // Find a mouse grabber by sending mouse press events to all mouse grabber
    // candidates one at a time, until the event is accepted. It's accepted by
    // default, so the receiver has to explicitly ignore it for it to pass
    // through.
    foreach (QGraphicsItem *item, cachedItemsUnderMouse) {
        if (!(item->acceptedMouseButtons() & mouseEvent->button())) {
            // Skip items that don't accept the event's mouse button.
            continue;
        }

        grabMouse(item, /* implicit = */ true);
        mouseEvent->accept();

        // check if the item we are sending to are disabled (before we send the event)
        bool disabled = !item->isEnabled();
        bool isWindow = item->isWindow();
        if (mouseEvent->type() == QEvent::GraphicsSceneMouseDoubleClick && item != lastMouseGrabberItem) {
            // If this item is different from the item that received the last
            // mouse event, and mouseEvent is a doubleclick event, then the
            // event is converted to a press. Known limitation:
            // Triple-clicking will not generate a doubleclick, though.
            QGraphicsSceneMouseEvent mousePress(QEvent::GraphicsSceneMousePress);
            mousePress.accept();
            mousePress.setButton(mouseEvent->button());
            mousePress.setButtons(mouseEvent->buttons());
            mousePress.setScreenPos(mouseEvent->screenPos());
            mousePress.setScenePos(mouseEvent->scenePos());
            mousePress.setModifiers(mouseEvent->modifiers());
            mousePress.setWidget(mouseEvent->widget());
            mousePress.setButtonDownPos(mouseEvent->button(),
                                        mouseEvent->buttonDownPos(mouseEvent->button()));
            mousePress.setButtonDownScenePos(mouseEvent->button(),
                                             mouseEvent->buttonDownScenePos(mouseEvent->button()));
            mousePress.setButtonDownScreenPos(mouseEvent->button(),
                                              mouseEvent->buttonDownScreenPos(mouseEvent->button()));
            sendMouseEvent(&mousePress);
            mouseEvent->setAccepted(mousePress.isAccepted());
        } else {
            sendMouseEvent(mouseEvent);
        }

        bool dontSendUngrabEvents = mouseGrabberItems.isEmpty() || mouseGrabberItems.last() != item;
        if (disabled) {
            ungrabMouse(item, /* itemIsDying = */ dontSendUngrabEvents);
            break;
        }
        if (mouseEvent->isAccepted()) {
            if (!mouseGrabberItems.isEmpty())
                storeMouseButtonsForMouseGrabber(mouseEvent);
            lastMouseGrabberItem = item;
            return;
        }
        ungrabMouse(item, /* itemIsDying = */ dontSendUngrabEvents);

        // Don't propagate through windows.
        if (isWindow)
            break;
    }

    // Is the event still ignored? Then the mouse press goes to the scene.
    // Reset the mouse grabber, clear the selection, clear focus, and leave
    // the event ignored so that it can propagate through the originating
    // view.
    if (!mouseEvent->isAccepted()) {
        clearMouseGrabber();

        QGraphicsView *view = mouseEvent->widget() ? qobject_cast<QGraphicsView *>(mouseEvent->widget()->parentWidget()) : 0;
        bool dontClearSelection = view && view->dragMode() == QGraphicsView::ScrollHandDrag;
        if (!dontClearSelection) {
            // Clear the selection if the originating view isn't in scroll
            // hand drag mode. The view will clear the selection if no drag
            // happened.
            q->clearSelection();
        }
    }
}

QGraphicsWidget *QGraphicsScenePrivate::windowForItem(const QGraphicsItem *item) const
{
    if (!item)
        return 0;
    do {
        if (item->isWidget())
            return static_cast<const QGraphicsWidget *>(item)->window();
        item = item->parentItem();
    } while (item);
    return 0;
}

QList<QGraphicsItem *> QGraphicsScenePrivate::items_helper(const QRectF &rect,
                                                           Qt::ItemSelectionMode mode,
                                                           Qt::SortOrder order) const
{
    QList<QGraphicsItem *> items;

    QPainterPath path;

    // The index returns a rough estimate of what items are inside the rect.
    // Refine it by iterating through all returned items.
    QRectF adjustedRect = _q_adjustedRect(rect);
    foreach (QGraphicsItem *item, estimateItemsInRect(adjustedRect)) {
        // Find the item's scene transform in a clever way.
        QTransform x = item->sceneTransform();
        bool keep = false;

        // ### _q_adjustedRect is only needed because QRectF::intersects,
        // QRectF::contains and QTransform::map() and friends don't work with
        // flat rectangles.
        QRectF br = _q_adjustedRect(item->boundingRect());
        if (mode >= Qt::ContainsItemBoundingRect) {
            // Rect intersects/contains item's bounding rect
            QRectF mbr = x.mapRect(br);
            if ((mode == Qt::IntersectsItemBoundingRect && QRectF_intersects(rect, mbr))
                || (mode == Qt::ContainsItemBoundingRect && rect != mbr && rect.contains(mbr))) {
                items << item;
                keep = true;
            }
        } else {
            // Rect intersects/contains item's shape
            if (QRectF_intersects(adjustedRect, x.mapRect(br))) {
                bool ok;
                QTransform xinv = x.inverted(&ok);
                if (ok) {
                    if (path == QPainterPath())
                        path.addRect(rect);
                    if (itemCollidesWithPath(item, xinv.map(path), mode)) {
                        items << item;
                        keep = true;
                    }
                }
            }
        }

        if (keep && (item->flags() & QGraphicsItem::ItemClipsChildrenToShape)) {
            // Recurse into children that clip children.
            bool ok;
            QTransform xinv = x.inverted(&ok);
            if (ok) {
                if (x.type() <= QTransform::TxScale) {
                    // Rect
                    childItems_helper(&items, item, xinv.mapRect(rect), mode);
                } else {
                    // Polygon
                    childItems_helper(&items, item, xinv.map(rect), mode);
                }
            }
        }
    }

    if (order != Qt::SortOrder(-1))
        sortItems(&items, order, sortCacheEnabled);
    return items;
}

QList<QGraphicsItem *> QGraphicsScenePrivate::items_helper(const QPolygonF &polygon,
                                                           Qt::ItemSelectionMode mode,
                                                           Qt::SortOrder order) const
{
    QList<QGraphicsItem *> items;

    QRectF polyRect = _q_adjustedRect(polygon.boundingRect());
    QPainterPath path;

    // The index returns a rough estimate of what items are inside the rect.
    // Refine it by iterating through all returned items.
    foreach (QGraphicsItem *item, estimateItemsInRect(polyRect)) {
        // Find the item's scene transform in a clever way.
        QTransform x = item->sceneTransform();
        bool keep = false;

        // ### _q_adjustedRect is only needed because QRectF::intersects,
        // QRectF::contains and QTransform::map() and friends don't work with
        // flat rectangles.
        QRectF br = _q_adjustedRect(item->boundingRect());
        if (mode >= Qt::ContainsItemBoundingRect) {
            // Polygon contains/intersects item's bounding rect
            if (path == QPainterPath())
                path.addPolygon(polygon);
            if ((mode == Qt::IntersectsItemBoundingRect && path.intersects(x.mapRect(br)))
                || (mode == Qt::ContainsItemBoundingRect && path.contains(x.mapRect(br)))) {
                items << item;
                keep = true;
            }
        } else {
            // Polygon contains/intersects item's shape
            if (QRectF_intersects(polyRect, x.mapRect(br))) {
                bool ok;
                QTransform xinv = x.inverted(&ok);
                if (ok) {
                    if (path == QPainterPath())
                        path.addPolygon(polygon);
                    if (itemCollidesWithPath(item, xinv.map(path), mode)) {
                        items << item;
                        keep = true;
                    }
                }
            }
        }

        if (keep && (item->flags() & QGraphicsItem::ItemClipsChildrenToShape)) {
            // Recurse into children that clip children.
            bool ok;
            QTransform xinv = x.inverted(&ok);
            if (ok)
                childItems_helper(&items, item, xinv.map(polygon), mode);
        }
    }

    if (order != Qt::SortOrder(-1))
        sortItems(&items, order, sortCacheEnabled);
    return items;
}

QList<QGraphicsItem *> QGraphicsScenePrivate::items_helper(const QPainterPath &path,
                                                           Qt::ItemSelectionMode mode,
                                                           Qt::SortOrder order) const
{
    QList<QGraphicsItem *> items;
    // The index returns a rough estimate of what items are inside the rect.
    // Refine it by iterating through all returned items.
    foreach (QGraphicsItem *item, estimateItemsInRect(_q_adjustedRect(path.controlPointRect()))) {
        // Find the item's scene transform in a clever way.
        QTransform x = item->sceneTransform();
        bool ok;
        QTransform xinv = x.inverted(&ok);
        if (ok) {
            QPainterPath mappedPath = xinv.map(path);
            if (itemCollidesWithPath(item, mappedPath, mode)) {
                items << item;
                if (item->flags() & QGraphicsItem::ItemClipsChildrenToShape)
                    childItems_helper(&items, item, mappedPath, mode);
            }
        }
    }

    if (order != Qt::SortOrder(-1))
        sortItems(&items, order, sortCacheEnabled);
    return items;
}

void QGraphicsScenePrivate::childItems_helper(QList<QGraphicsItem *> *items,
                                              const QGraphicsItem *parent,
                                              const QRectF &rect,
                                              Qt::ItemSelectionMode mode) const
{
    QPainterPath path;
    bool parentClip = (parent->flags() & QGraphicsItem::ItemClipsChildrenToShape);
    QRectF r = !parentClip ? _q_adjustedRect(rect) : _q_adjustedRect(rect).intersected(_q_adjustedRect(parent->boundingRect()));
    if (r.isEmpty())
        return;

    QList<QGraphicsItem *> &children = parent->d_ptr->children;
    for (int i = 0; i < children.size(); ++i) {
        QGraphicsItem *item = children.at(i);
        if (item->d_ptr->hasTransform && !item->transform().isInvertible())
            continue;

        // Skip invisible items and all their children.
        if (!item->d_ptr->visible || qFuzzyCompare(item->effectiveOpacity(), qreal(0.0)))
            continue;

        // ### _q_adjustedRect is only needed because QRectF::intersects,
        // QRectF::contains and QTransform::map() and friends don't work with
        // flat rectangles.
        QRectF br = _q_adjustedRect(item->boundingRect());
        QRectF mbr = item->mapRectToParent(br);
        bool keep = false;
        if (mode >= Qt::ContainsItemBoundingRect) {
            // Rect intersects/contains item's bounding rect
            if ((mode == Qt::IntersectsItemBoundingRect && QRectF_intersects(rect, mbr))
                || (mode == Qt::ContainsItemBoundingRect && rect != mbr && rect.contains(br))) {
                items->append(item);
                keep = true;
            }
        } else {
            // Rect intersects/contains item's shape
            if (QRectF_intersects(rect, mbr)) {
                if (path == QPainterPath())
                    path.addRect(rect);
                if (itemCollidesWithPath(item, item->mapFromParent(path), mode)) {
                    items->append(item);
                    keep = true;
                }
            }
        }

        if ((keep || !(item->flags() & QGraphicsItem::ItemClipsChildrenToShape)) && !item->d_ptr->children.isEmpty()) {
            // Recurse into children.
            if (!item->d_ptr->hasTransform || item->transform().type() <= QTransform::TxScale) {
                // Rect
                childItems_helper(items, item, item->mapRectFromParent(rect), mode);
            } else {
                // Polygon
                childItems_helper(items, item, item->mapFromParent(rect), mode);
            }
        }
    }
}

void QGraphicsScenePrivate::childItems_helper(QList<QGraphicsItem *> *items,
                                              const QGraphicsItem *parent,
                                              const QPolygonF &polygon,
                                              Qt::ItemSelectionMode mode) const
{
    QPainterPath path;
    bool parentClip = (parent->flags() & QGraphicsItem::ItemClipsChildrenToShape);
    QRectF polyRect = _q_adjustedRect(polygon.boundingRect());
    QRectF r = !parentClip ? polyRect : polyRect.intersected(_q_adjustedRect(parent->boundingRect()));
    if (r.isEmpty())
        return;

    QList<QGraphicsItem *> &children = parent->d_ptr->children;
    for (int i = 0; i < children.size(); ++i) {
        QGraphicsItem *item = children.at(i);
        if (item->d_ptr->hasTransform && !item->transform().isInvertible())
            continue;

        // Skip invisible items.
        if (!item->d_ptr->visible || qFuzzyCompare(item->effectiveOpacity() + 1, qreal(1.0)))
            continue;

        // ### _q_adjustedRect is only needed because QRectF::intersects,
        // QRectF::contains and QTransform::map() and friends don't work with
        // flat rectangles.
        QRectF br = _q_adjustedRect(item->boundingRect());
        bool keep = false;
        if (mode >= Qt::ContainsItemBoundingRect) {
            // Polygon contains/intersects item's bounding rect
            if (path == QPainterPath())
                path.addPolygon(polygon);
            if ((mode == Qt::IntersectsItemBoundingRect && path.intersects(item->mapRectToParent(br)))
                || (mode == Qt::ContainsItemBoundingRect && path.contains(item->mapRectToParent(br)))) {
                items->append(item);
                keep = true;
            }
        } else {
            // Polygon contains/intersects item's shape
            if (QRectF_intersects(polyRect, item->mapRectToParent(br))) {
                if (path == QPainterPath())
                    path.addPolygon(polygon);
                if (itemCollidesWithPath(item, item->mapFromParent(path), mode)) {
                    items->append(item);
                    keep = true;
                }
            }
        }

        if ((keep || !(item->flags() & QGraphicsItem::ItemClipsChildrenToShape)) && !item->d_ptr->children.isEmpty()) {
            // Recurse into children that clip children.
            childItems_helper(items, item, item->mapFromParent(polygon), mode);
        }
    }
}

void QGraphicsScenePrivate::childItems_helper(QList<QGraphicsItem *> *items,
                                              const QGraphicsItem *parent,
                                              const QPainterPath &path,
                                              Qt::ItemSelectionMode mode) const
{
    bool parentClip = (parent->flags() & QGraphicsItem::ItemClipsChildrenToShape);
    QPainterPath intersectedPath = !parentClip ? path : path.intersected(parent->shape());
    if (intersectedPath.isEmpty())
        return;

    QList<QGraphicsItem *> &children = parent->d_ptr->children;
    for (int i = 0; i < children.size(); ++i) {
        QGraphicsItem *item = children.at(i);

        // Skip invisible items.
        if (!item->d_ptr->visible || qFuzzyCompare(item->effectiveOpacity(), qreal(0.0)))
            continue;

        QTransform x = item->sceneTransform();

        bool ok;
        QTransform xinv = x.inverted(&ok);
        if (ok) {
            QPainterPath mappedPath = xinv.map(path);
            if (itemCollidesWithPath(item, mappedPath, mode)) {
                items->append(item);
                if (!item->d_ptr->children.isEmpty())
                    childItems_helper(items, item, mappedPath, mode);
            }
        }
    }
}

void QGraphicsScenePrivate::invalidateSortCache()
{
    Q_Q(QGraphicsScene);
    if (!sortCacheEnabled || updatingSortCache)
        return;

    updatingSortCache = true;
    QMetaObject::invokeMethod(q, "_q_updateSortCache", Qt::QueuedConnection);
}

/*!
    \internal

    Should not be exported, but we can't change that now.
    ### Qt 5: Remove symbol / make static
*/
inline bool qt_closestLeaf(const QGraphicsItem *item1, const QGraphicsItem *item2)
{
    // Return true if sibling item1 is on top of item2.
    const QGraphicsItemPrivate *d1 = item1->d_ptr;
    const QGraphicsItemPrivate *d2 = item2->d_ptr;
    bool f1 = d1->flags & QGraphicsItem::ItemStacksBehindParent;
    bool f2 = d2->flags & QGraphicsItem::ItemStacksBehindParent;
    if (f1 != f2) return f2;
    qreal z1 = d1->z;
    qreal z2 = d2->z;
    return z1 != z2 ? z1 > z2 : item1 > item2;
}

/*!
    \internal

    Should not be exported, but we can't change that now.
*/
inline bool qt_closestItemFirst(const QGraphicsItem *item1, const QGraphicsItem *item2)
{
    return QGraphicsScenePrivate::closestItemFirst_withoutCache(item1, item2);
}

/*!
    Returns true if \a item1 is on top of \a item2.

    \internal
*/
bool QGraphicsScenePrivate::closestItemFirst_withoutCache(const QGraphicsItem *item1, const QGraphicsItem *item2)
{
    // Siblings? Just check their z-values.
    const QGraphicsItemPrivate *d1 = item1->d_ptr;
    const QGraphicsItemPrivate *d2 = item2->d_ptr;
    if (d1->parent == d2->parent)
        return qt_closestLeaf(item1, item2);

    // Find common ancestor, and each item's ancestor closest to the common
    // ancestor.
    int item1Depth = d1->depth;
    int item2Depth = d2->depth;
    const QGraphicsItem *p = item1;
    const QGraphicsItem *t1 = item1;
    while (item1Depth > item2Depth && (p = p->d_ptr->parent)) {
        if (p == item2) {
            // item2 is one of item1's ancestors; item1 is on top
            return !(t1->d_ptr->flags & QGraphicsItem::ItemStacksBehindParent);
        }
        t1 = p;
        --item1Depth;
    }
    p = item2;
    const QGraphicsItem *t2 = item2;
    while (item2Depth > item1Depth && (p = p->d_ptr->parent)) {
        if (p == item1) {
            // item1 is one of item2's ancestors; item1 is not on top
            return (t2->d_ptr->flags & QGraphicsItem::ItemStacksBehindParent);
        }
        t2 = p;
        --item2Depth;
    }

    // item1Ancestor is now at the same level as item2Ancestor, but not the same.
    const QGraphicsItem *a1 = t1;
    const QGraphicsItem *a2 = t2;
    while (a1) {
        const QGraphicsItem *p1 = a1;
        const QGraphicsItem *p2 = a2;
        a1 = a1->parentItem();
        a2 = a2->parentItem();
        if (a1 && a1 == a2)
            return qt_closestLeaf(p1, p2);
    }

    // No common ancestor? Then just compare the items' toplevels directly.
    return qt_closestLeaf(t1->topLevelItem(), t2->topLevelItem());
}

/*!
    Returns true if \a item2 is on top of \a item1.

    \internal
*/
bool QGraphicsScenePrivate::closestItemLast_withoutCache(const QGraphicsItem *item1, const QGraphicsItem *item2)
{
    return closestItemFirst_withoutCache(item2, item1);
}

void QGraphicsScenePrivate::climbTree(QGraphicsItem *item, int *stackingOrder)
{
    if (!item->d_ptr->children.isEmpty()) {
        QList<QGraphicsItem *> childList = item->d_ptr->children;
        qSort(childList.begin(), childList.end(), qt_closestLeaf);
        for (int i = 0; i < childList.size(); ++i) {
            QGraphicsItem *item = childList.at(i);
            if (!(item->flags() & QGraphicsItem::ItemStacksBehindParent))
                climbTree(childList.at(i), stackingOrder);
        }
        item->d_ptr->globalStackingOrder = (*stackingOrder)++;
        for (int i = 0; i < childList.size(); ++i) {
            QGraphicsItem *item = childList.at(i);
            if (item->flags() & QGraphicsItem::ItemStacksBehindParent)
                climbTree(childList.at(i), stackingOrder);
        }
    } else {
        item->d_ptr->globalStackingOrder = (*stackingOrder)++;
    }
}

void QGraphicsScenePrivate::_q_updateSortCache()
{
    _q_updateIndex();

    if (!sortCacheEnabled || !updatingSortCache)
        return;

    updatingSortCache = false;
    int stackingOrder = 0;

    QList<QGraphicsItem *> topLevels;

    for (int i = 0; i < indexedItems.size(); ++i) {
        QGraphicsItem *item = indexedItems.at(i);
        if (item && item->parentItem() == 0)
            topLevels << item;
    }
    for (int i = 0; i < unindexedItems.size(); ++i) {
        QGraphicsItem *item = unindexedItems.at(i);
        if (item->parentItem() == 0)
            topLevels << item;
    }

    qSort(topLevels.begin(), topLevels.end(), qt_closestLeaf);
    for (int i = 0; i < topLevels.size(); ++i)
        climbTree(topLevels.at(i), &stackingOrder);
}

void QGraphicsScenePrivate::sortItems(QList<QGraphicsItem *> *itemList, Qt::SortOrder order,
                                      bool sortCacheEnabled)
{
    if (sortCacheEnabled) {
        if (order == Qt::AscendingOrder) {
            qSort(itemList->begin(), itemList->end(), closestItemFirst_withCache);
        } else if (order == Qt::DescendingOrder) {
            qSort(itemList->begin(), itemList->end(), closestItemLast_withCache);
        }
    } else {
        if (order == Qt::AscendingOrder) {
            qSort(itemList->begin(), itemList->end(), closestItemFirst_withoutCache);
        } else if (order == Qt::DescendingOrder) {
            qSort(itemList->begin(), itemList->end(), closestItemLast_withoutCache);
        }
    }
}

/*!
    \internal

    Set the font and propagate the changes if the font is different from the
    current font.
*/
void QGraphicsScenePrivate::setFont_helper(const QFont &font)
{
    if (this->font == font && this->font.resolve() == font.resolve())
        return;
    updateFont(font);
}

/*!
    \internal

    Resolve the scene's font against the application font, and propagate the
    changes too all items in the scene.
*/
void QGraphicsScenePrivate::resolveFont()
{
    QFont naturalFont = qApp->font();
    naturalFont.resolve(0);
    QFont resolvedFont = font.resolve(naturalFont);
    updateFont(resolvedFont);
}

/*!
    \internal

    Update the font, and whether or not it has changed, reresolve all fonts in
    the scene.
*/
void QGraphicsScenePrivate::updateFont(const QFont &font)
{
    Q_Q(QGraphicsScene);

    // Update local font setting.
    this->font = font;

    // Resolve the fonts of all top-level widget items, or widget items
    // whose parent is not a widget.
    foreach (QGraphicsItem *item, q->items()) {
        if (!item->parentItem()) {
            // Resolvefont for an item is a noop operation, but
            // every item can be a widget, or can have a widget
            // childre.
            item->d_ptr->resolveFont(font.resolve());
        }
    }

    // Send the scene a FontChange event.
    QEvent event(QEvent::FontChange);
    QApplication::sendEvent(q, &event);
}

/*!
    \internal

    Set the palette and propagate the changes if the palette is different from
    the current palette.
*/
void QGraphicsScenePrivate::setPalette_helper(const QPalette &palette)
{
    if (this->palette == palette && this->palette.resolve() == palette.resolve())
        return;
    updatePalette(palette);
}

/*!
    \internal

    Resolve the scene's palette against the application palette, and propagate
    the changes too all items in the scene.
*/
void QGraphicsScenePrivate::resolvePalette()
{
    QPalette naturalPalette = qApp->palette();
    naturalPalette.resolve(0);
    QPalette resolvedPalette = palette.resolve(naturalPalette);
    updatePalette(resolvedPalette);
}

/*!
    \internal

    Update the palette, and whether or not it has changed, reresolve all
    palettes in the scene.
*/
void QGraphicsScenePrivate::updatePalette(const QPalette &palette)
{
    Q_Q(QGraphicsScene);

    // Update local palette setting.
    this->palette = palette;

    // Resolve the palettes of all top-level widget items, or widget items
    // whose parent is not a widget.
    foreach (QGraphicsItem *item, q->items()) {
        if (!item->parentItem()) {
            // Resolvefont for an item is a noop operation, but
            // every item can be a widget, or can have a widget
            // childre.
            item->d_ptr->resolvePalette(palette.resolve());
        }
    }

    // Send the scene a PaletteChange event.
    QEvent event(QEvent::PaletteChange);
    QApplication::sendEvent(q, &event);
}

/*!
    Constructs a QGraphicsScene object. The \a parent parameter is
    passed to QObject's constructor.
*/
QGraphicsScene::QGraphicsScene(QObject *parent)
    : QObject(*new QGraphicsScenePrivate, parent)
{
    d_func()->init();
}

/*!
    Constructs a QGraphicsScene object, using \a sceneRect for its
    scene rectangle. The \a parent parameter is passed to QObject's
    constructor.

    \sa sceneRect
*/
QGraphicsScene::QGraphicsScene(const QRectF &sceneRect, QObject *parent)
    : QObject(*new QGraphicsScenePrivate, parent)
{
    setSceneRect(sceneRect);
    d_func()->init();
}

/*!
    Constructs a QGraphicsScene object, using the rectangle specified
    by (\a x, \a y), and the given \a width and \a height for its
    scene rectangle. The \a parent parameter is passed to QObject's
    constructor.

    \sa sceneRect
*/
QGraphicsScene::QGraphicsScene(qreal x, qreal y, qreal width, qreal height, QObject *parent)
    : QObject(*new QGraphicsScenePrivate, parent)
{
    setSceneRect(x, y, width, height);
    d_func()->init();
}

/*!
    Destroys the QGraphicsScene object.
*/
QGraphicsScene::~QGraphicsScene()
{
    Q_D(QGraphicsScene);
    // Remove this scene from qApp's global scene list.
    qApp->d_func()->scene_list.removeAll(this);

    clear();

    // Remove this scene from all associated views.
    for (int j = 0; j < d->views.size(); ++j)
        d->views.at(j)->setScene(0);
}

/*!
    \property QGraphicsScene::sceneRect
    \brief the scene rectangle; the bounding rectangle of the scene

    The scene rectangle defines the extent of the scene. It is
    primarily used by QGraphicsView to determine the view's default
    scrollable area, and by QGraphicsScene to manage item indexing.

    If unset, or if set to a null QRectF, sceneRect() will return the largest
    bounding rect of all items on the scene since the scene was created (i.e.,
    a rectangle that grows when items are added to or moved in the scene, but
    never shrinks).

    \sa width(), height(), QGraphicsView::sceneRect
*/
QRectF QGraphicsScene::sceneRect() const
{
    Q_D(const QGraphicsScene);
    const_cast<QGraphicsScenePrivate *>(d)->_q_updateIndex();
    return d->hasSceneRect ? d->sceneRect : d->growingItemsBoundingRect;
}
void QGraphicsScene::setSceneRect(const QRectF &rect)
{
    Q_D(QGraphicsScene);
    if (rect != d->sceneRect) {
        d->hasSceneRect = !rect.isNull();
        d->sceneRect = rect;
        d->resetIndex();
        emit sceneRectChanged(rect);
    }
}

/*!
     \fn qreal QGraphicsScene::width() const

     This convenience function is equivalent to calling sceneRect().width().

     \sa height()
*/

/*!
     \fn qreal QGraphicsScene::height() const

     This convenience function is equivalent to calling \c sceneRect().height().

     \sa width()
*/

/*!
    Renders the \a source rect from scene into \a target, using \a painter. This
    function is useful for capturing the contents of the scene onto a paint
    device, such as a QImage (e.g., to take a screenshot), or for printing
    with QPrinter. For example:

    \snippet doc/src/snippets/code/src_gui_graphicsview_qgraphicsscene.cpp 1

    If \a source is a null rect, this function will use sceneRect() to
    determine what to render. If \a target is a null rect, the dimensions of \a
    painter's paint device will be used.

    The source rect contents will be transformed according to \a
    aspectRatioMode to fit into the target rect. By default, the aspect ratio
    is kept, and \a source is scaled to fit in \a target.

    \sa QGraphicsView::render()
*/
void QGraphicsScene::render(QPainter *painter, const QRectF &target, const QRectF &source,
                            Qt::AspectRatioMode aspectRatioMode)
{
    Q_D(QGraphicsScene);

    // Default source rect = scene rect
    QRectF sourceRect = source;
    if (sourceRect.isNull())
        sourceRect = sceneRect();

    // Default target rect = device rect
    QRectF targetRect = target;
    if (targetRect.isNull()) {
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
    QList<QGraphicsItem *> itemList = items(sourceRect, Qt::IntersectsItemBoundingRect);
    QGraphicsItem **itemArray = new QGraphicsItem *[itemList.size()];
    int numItems = itemList.size();
    for (int i = 0; i < numItems; ++i)
        itemArray[numItems - i - 1] = itemList.at(i);
    itemList.clear();

    painter->save();

    // Transform the painter.
    painter->setClipRect(targetRect);
    QTransform painterTransform;
    painterTransform *= QTransform()
                        .translate(targetRect.left(), targetRect.top())
                        .scale(xratio, yratio)
                        .translate(-sourceRect.left(), -sourceRect.top());
    painter->setWorldTransform(painterTransform, true);

    // Two unit vectors.
    QLineF v1(0, 0, 1, 0);
    QLineF v2(0, 0, 0, 1);

    // Generate the style options
    QStyleOptionGraphicsItem *styleOptionArray = new QStyleOptionGraphicsItem[numItems];
    for (int i = 0; i < numItems; ++i) {
        QGraphicsItem *item = itemArray[i];

        QStyleOptionGraphicsItem option;
        option.state = QStyle::State_None;
        option.rect = item->boundingRect().toRect();
        if (item->isSelected())
            option.state |= QStyle::State_Selected;
        if (item->isEnabled())
            option.state |= QStyle::State_Enabled;
        if (item->hasFocus())
            option.state |= QStyle::State_HasFocus;
        if (d->hoverItems.contains(item))
            option.state |= QStyle::State_MouseOver;
        if (item == mouseGrabberItem())
            option.state |= QStyle::State_Sunken;

        // Calculate a simple level-of-detail metric.
        // ### almost identical code in QGraphicsView::paintEvent()
        //     and QGraphicsView::render() - consider refactoring
        QTransform itemToDeviceTransform;
        if (item->d_ptr->itemIsUntransformable()) {
            itemToDeviceTransform = item->deviceTransform(painterTransform);
        } else {
            itemToDeviceTransform = item->sceneTransform() * painterTransform;
        }

        option.levelOfDetail = qSqrt(itemToDeviceTransform.map(v1).length() * itemToDeviceTransform.map(v2).length());
        option.matrix = itemToDeviceTransform.toAffine(); //### discards perspective

        option.exposedRect = item->boundingRect();
        option.exposedRect &= itemToDeviceTransform.inverted().mapRect(targetRect);

        styleOptionArray[i] = option;
    }

    // Render the scene.
    drawBackground(painter, sourceRect);
    drawItems(painter, numItems, itemArray, styleOptionArray);
    drawForeground(painter, sourceRect);

    delete [] itemArray;
    delete [] styleOptionArray;

    painter->restore();
}

/*!
    \property QGraphicsScene::itemIndexMethod
    \brief the item indexing method.

    QGraphicsScene applies an indexing algorithm to the scene, to speed up
    item discovery functions like items() and itemAt(). Indexing is most
    efficient for static scenes (i.e., where items don't move around). For
    dynamic scenes, or scenes with many animated items, the index bookkeeping
    can outweight the fast lookup speeds.

    For the common case, the default index method BspTreeIndex works fine.  If
    your scene uses many animations and you are experiencing slowness, you can
    disable indexing by calling \c setItemIndexMethod(NoIndex).

    \sa bspTreeDepth
*/
QGraphicsScene::ItemIndexMethod QGraphicsScene::itemIndexMethod() const
{
    Q_D(const QGraphicsScene);
    return d->indexMethod;
}
void QGraphicsScene::setItemIndexMethod(ItemIndexMethod method)
{
    Q_D(QGraphicsScene);
    d->resetIndex();
    d->indexMethod = method;
}

/*!
    \property QGraphicsScene::bspTreeDepth
    \brief the depth of QGraphicsScene's BSP index tree
    \since 4.3

    This property has no effect when NoIndex is used.

    This value determines the depth of QGraphicsScene's BSP tree. The depth
    directly affects QGraphicsScene's performance and memory usage; the latter
    growing exponentially with the depth of the tree. With an optimal tree
    depth, QGraphicsScene can instantly determine the locality of items, even
    for scenes with thousands or millions of items. This also greatly improves
    rendering performance.

    By default, the value is 0, in which case Qt will guess a reasonable
    default depth based on the size, location and number of items in the
    scene. If these parameters change frequently, however, you may experience
    slowdowns as QGraphicsScene retunes the depth internally. You can avoid
    potential slowdowns by fixating the tree depth through setting this
    property.

    The depth of the tree and the size of the scene rectangle decide the
    granularity of the scene's partitioning. The size of each scene segment is
    determined by the following algorithm:

    \snippet doc/src/snippets/code/src_gui_graphicsview_qgraphicsscene.cpp 2

    The BSP tree has an optimal size when each segment contains between 0 and
    10 items.

    \sa itemIndexMethod
*/
int QGraphicsScene::bspTreeDepth() const
{
    Q_D(const QGraphicsScene);
    return d->bspTreeDepth;
}
void QGraphicsScene::setBspTreeDepth(int depth)
{
    Q_D(QGraphicsScene);
    if (d->bspTreeDepth == depth)
        return;

    if (depth < 0) {
        qWarning("QGraphicsScene::setBspTreeDepth: invalid depth %d ignored; must be >= 0", depth);
        return;
    }

    d->bspTreeDepth = depth;
    d->resetIndex();
}

/*!
    \property QGraphicsScene::sortCacheEnabled
    \brief whether sort caching is enabled
    \since 4.5

    When enabled, this property adds a cache that speeds up sorting and
    transformations for scenes with deep hierarchies (i.e., items with many
    levels of descendents), at the cost of using more memory (approx. 100 more
    bytes of memory per item).

    Items that are not part of a deep hierarchy suffer no penalty from this
    cache.
*/
bool QGraphicsScene::isSortCacheEnabled() const
{
    Q_D(const QGraphicsScene);
    return d->sortCacheEnabled;
}
void QGraphicsScene::setSortCacheEnabled(bool enabled)
{
    Q_D(QGraphicsScene);
    if (enabled == d->sortCacheEnabled)
        return;
    if ((d->sortCacheEnabled = enabled))
        d->invalidateSortCache();
}

/*!
    Calculates and returns the bounding rect of all items on the scene. This
    function works by iterating over all items, and because if this, it can
    be slow for large scenes.

    \sa sceneRect()
*/
QRectF QGraphicsScene::itemsBoundingRect() const
{
    QRectF boundingRect;
    foreach (QGraphicsItem *item, items())
        boundingRect |= item->sceneBoundingRect();
    return boundingRect;
}

/*!
    Returns a list of all items on the scene, in no particular order.

    \sa addItem(), removeItem()
*/
QList<QGraphicsItem *> QGraphicsScene::items() const
{
    Q_D(const QGraphicsScene);
    const_cast<QGraphicsScenePrivate *>(d)->purgeRemovedItems();

    // If freeItemIndexes is empty, we know there are no holes in indexedItems and
    // unindexedItems.
    if (d->freeItemIndexes.isEmpty()) {
        if (d->unindexedItems.isEmpty())
            return d->indexedItems;
        return d->indexedItems + d->unindexedItems;
    }

    // Rebuild the list of items to avoid holes. ### We could also just
    // compress the item lists at this point.
    QList<QGraphicsItem *> itemList;
    foreach (QGraphicsItem *item, d->indexedItems + d->unindexedItems) {
        if (item)
            itemList << item;
    }
    return itemList;
}

/*!
    Returns all visible items at position \a pos in the scene. The items are
    listed in descending Z order (i.e., the first item in the list is the
    top-most item, and the last item is the bottom-most item).

    \sa itemAt()
*/
QList<QGraphicsItem *> QGraphicsScene::items(const QPointF &pos) const
{
    QList<QGraphicsItem *> itemsAtPoint;

    // Find all items within a 1x1 rect area starting at pos. This can be
    // inefficient for scenes that use small coordinates (like unity
    // coordinates), or for detailed graphs. ### The index should support
    // fetching items at a pos to avoid this limitation.
    foreach (QGraphicsItem *item, items(QRectF(pos, QSizeF(1, 1)), Qt::IntersectsItemBoundingRect)) {
        if (item->contains(item->mapFromScene(pos)))
            itemsAtPoint << item;
    }
    return itemsAtPoint;
}


/*!
    \fn QList<QGraphicsItem *> QGraphicsScene::items(const QRectF &rectangle, Qt::ItemSelectionMode mode) const

    \overload

    Returns all visible items that, depending on \a mode, are either inside or
    intersect with the specified \a rectangle.

    The default value for \a mode is Qt::IntersectsItemShape; all items whose
    exact shape intersects with or is contained by \a rectangle are returned.

    \sa itemAt()
*/
QList<QGraphicsItem *> QGraphicsScene::items(const QRectF &rect, Qt::ItemSelectionMode mode) const
{
    Q_D(const QGraphicsScene);
    return d->items_helper(rect, mode, Qt::AscendingOrder);
}

/*!
    \fn QList<QGraphicsItem *> QGraphicsScene::items(qreal x, qreal y, qreal w, qreal h, Qt::ItemSelectionMode mode) const
    \since 4.3

    This convenience function is equivalent to calling items(QRectF(\a x, \a y, \a w, \a h), \a mode).
*/

/*!
    \overload

    Returns all visible items that, depending on \a mode, are either inside or
    intersect with the polygon \a polygon.

    The default value for \a mode is Qt::IntersectsItemShape; all items whose
    exact shape intersects with or is contained by \a polygon are returned.

    \sa itemAt()
*/
QList<QGraphicsItem *> QGraphicsScene::items(const QPolygonF &polygon, Qt::ItemSelectionMode mode) const
{
    Q_D(const QGraphicsScene);
    return d->items_helper(polygon, mode, Qt::AscendingOrder);
}

/*!
    \overload

    Returns all visible items that, depending on \a path, are either inside or
    intersect with the path \a path.

    The default value for \a mode is Qt::IntersectsItemShape; all items whose
    exact shape intersects with or is contained by \a path are returned.

    \sa itemAt()
*/
QList<QGraphicsItem *> QGraphicsScene::items(const QPainterPath &path, Qt::ItemSelectionMode mode) const
{
    Q_D(const QGraphicsScene);
    return d->items_helper(path, mode, Qt::AscendingOrder);
}

/*!
    Returns a list of all items that collide with \a item. Collisions are
    determined by calling QGraphicsItem::collidesWithItem(); the collision
    detection is determined by \a mode. By default, all items whose shape
    intersects \a item or is contained inside \a item's shape are returned.

    The items are returned in descending Z order (i.e., the first item in the
    list is the top-most item, and the last item is the bottom-most item).

    \sa items(), itemAt(), QGraphicsItem::collidesWithItem()
*/
QList<QGraphicsItem *> QGraphicsScene::collidingItems(const QGraphicsItem *item,
                                                      Qt::ItemSelectionMode mode) const
{
    Q_D(const QGraphicsScene);
    if (!item) {
        qWarning("QGraphicsScene::collidingItems: cannot find collisions for null item");
        return QList<QGraphicsItem *>();
    }

    QList<QGraphicsItem *> tmp;
    foreach (QGraphicsItem *itemInVicinity, d->estimateItemsInRect(item->sceneBoundingRect())) {
        if (item != itemInVicinity && item->collidesWithItem(itemInVicinity, mode))
            tmp << itemInVicinity;
    }
    d->sortItems(&tmp, Qt::AscendingOrder, d->sortCacheEnabled);
    return tmp;
}

/*!
    \fn QGraphicsItem *QGraphicsScene::itemAt(const QPointF &position) const

    Returns the topmost visible item at the specified \a position, or 0 if
    there are no items at this position.

    \note The topmost item is the one with the highest Z-value.

    \sa items(), collidingItems(), QGraphicsItem::setZValue()
*/
QGraphicsItem *QGraphicsScene::itemAt(const QPointF &pos) const
{
    QList<QGraphicsItem *> itemsAtPoint = items(pos);
    return itemsAtPoint.isEmpty() ? 0 : itemsAtPoint.first();
}

/*!
    \fn QGraphicsScene::itemAt(qreal x, qreal y) const
    \overload

    Returns the topmost item at the position specified by (\a x, \a
    y), or 0 if there are no items at this position.

    This convenience function is equivalent to calling \c
    {itemAt(QPointF(x, y))}.

    \note The topmost item is the one with the highest Z-value.
*/

/*!
    Returns a list of all currently selected items. The items are
    returned in no particular order.

    \sa setSelectionArea()
*/
QList<QGraphicsItem *> QGraphicsScene::selectedItems() const
{
    Q_D(const QGraphicsScene);

    // Optimization: Lazily removes items that are not selected.
    QGraphicsScene *that = const_cast<QGraphicsScene *>(this);
    QSet<QGraphicsItem *> actuallySelectedSet;
    foreach (QGraphicsItem *item, that->d_func()->selectedItems) {
        if (item->isSelected())
            actuallySelectedSet << item;
    }

    that->d_func()->selectedItems = actuallySelectedSet;

    return d->selectedItems.values();
}

/*!
    Returns the selection area that was previously set with
    setSelectionArea(), or an empty QPainterPath if no selection area has been
    set.

    \sa setSelectionArea()
*/
QPainterPath QGraphicsScene::selectionArea() const
{
    Q_D(const QGraphicsScene);
    return d->selectionArea;
}

/*!
    Sets the selection area to \a path. All items within this area are
    immediately selected, and all items outside are unselected. You can get
    the list of all selected items by calling selectedItems().

    For an item to be selected, it must be marked as \e selectable
    (QGraphicsItem::ItemIsSelectable).

    \sa clearSelection(), selectionArea()
*/
void QGraphicsScene::setSelectionArea(const QPainterPath &path)
{
    setSelectionArea(path, Qt::IntersectsItemShape);
}

/*!
    \overload
    \since 4.3

    Sets the selection area to \a path using \a mode to determine if items are
    included in the selection area.

    \sa clearSelection(), selectionArea()
*/
void QGraphicsScene::setSelectionArea(const QPainterPath &path, Qt::ItemSelectionMode mode)
{
    Q_D(QGraphicsScene);

    // Note: with boolean path operations, we can improve performance here
    // quite a lot by "growing" the old path instead of replacing it. That
    // allows us to only check the intersect area for changes, instead of
    // reevaluating the whole path over again.
    d->selectionArea = path;

    QSet<QGraphicsItem *> unselectItems = d->selectedItems;

    // Disable emitting selectionChanged() for individual items.
    ++d->selectionChanging;
    bool changed = false;

    // Set all items in path to selected.
    foreach (QGraphicsItem *item, items(path, mode)) {
        if (item->flags() & QGraphicsItem::ItemIsSelectable) {
            if (!item->isSelected())
                changed = true;
            unselectItems.remove(item);
            item->setSelected(true);
        }
    }

    // Unselect all items outside path.
    foreach (QGraphicsItem *item, unselectItems) {
        item->setSelected(false);
        changed = true;
    }

    // Reenable emitting selectionChanged() for individual items.
    --d->selectionChanging;

    if (!d->selectionChanging && changed)
        emit selectionChanged();
}

/*!
   Clears the current selection.

   \sa setSelectionArea(), selectedItems()
*/
void QGraphicsScene::clearSelection()
{
    Q_D(QGraphicsScene);

    // Disable emitting selectionChanged
    ++d->selectionChanging;
    bool changed = !d->selectedItems.isEmpty();

    foreach (QGraphicsItem *item, d->selectedItems)
        item->setSelected(false);
    d->selectedItems.clear();

    // Reenable emitting selectionChanged() for individual items.
    --d->selectionChanging;

    if (!d->selectionChanging && changed)
        emit selectionChanged();
}

/*!
    \since 4.4

    Removes and deletes all items from the scene, but otherwise leaves the
    state of the scene unchanged.

    \sa addItem()
*/
void QGraphicsScene::clear()
{
    Q_D(QGraphicsScene);
    // Recursive descent delete
    for (int i = 0; i < d->indexedItems.size(); ++i) {
        if (QGraphicsItem *item = d->indexedItems.at(i)) {
            if (!item->parentItem())
                delete item;
        }
    }
    QList<QGraphicsItem *> unindexedParents;
    for (int i = 0; i < d->unindexedItems.size(); ++i) {
        QGraphicsItem *item = d->unindexedItems.at(i);
        if (!item->parentItem())
            unindexedParents << item;
    }
    d->unindexedItems.clear();
    qDeleteAll(unindexedParents);

    d->indexedItems.clear();
    d->freeItemIndexes.clear();
    d->lastItemCount = 0;
    d->bspTree.clear();
    d->largestUntransformableItem = QRectF();
}

/*!
    Groups all items in \a items into a new QGraphicsItemGroup, and returns a
    pointer to the group. The group is created with the common ancestor of \a
    items as its parent, and with position (0, 0). The items are all
    reparented to the group, and their positions and transformations are
    mapped to the group. If \a items is empty, this function will return an
    empty top-level QGraphicsItemGroup.

    QGraphicsScene has ownership of the group item; you do not need to delete
    it. To dismantle (ungroup) a group, call destroyItemGroup().

    \sa destroyItemGroup(), QGraphicsItemGroup::addToGroup()
*/
QGraphicsItemGroup *QGraphicsScene::createItemGroup(const QList<QGraphicsItem *> &items)
{
    // Build a list of the first item's ancestors
    QList<QGraphicsItem *> ancestors;
    int n = 0;
    if (!items.isEmpty()) {
        QGraphicsItem *parent = items.at(n++);
        while ((parent = parent->parentItem()))
            ancestors.append(parent);
    }

    // Find the common ancestor for all items
    QGraphicsItem *commonAncestor = 0;
    if (!ancestors.isEmpty()) {
        while (n < items.size()) {
            int commonIndex = -1;
            QGraphicsItem *parent = items.at(n++);
            do {
                int index = ancestors.indexOf(parent, qMax(0, commonIndex));
                if (index != -1) {
                    commonIndex = index;
                    break;
                }
            } while ((parent = parent->parentItem()));

            if (commonIndex == -1) {
                commonAncestor = 0;
                break;
            }

            commonAncestor = ancestors.at(commonIndex);
        }
    }

    // Create a new group at that level
    QGraphicsItemGroup *group = new QGraphicsItemGroup(commonAncestor);
    if (!commonAncestor)
        addItem(group);
    foreach (QGraphicsItem *item, items)
        group->addToGroup(item);
    return group;
}

/*!
    Reparents all items in \a group to \a group's parent item, then removes \a
    group from the scene, and finally deletes it. The items' positions and
    transformations are mapped from the group to the group's parent.

    \sa createItemGroup(), QGraphicsItemGroup::removeFromGroup()
*/
void QGraphicsScene::destroyItemGroup(QGraphicsItemGroup *group)
{
    foreach (QGraphicsItem *item, group->children())
        group->removeFromGroup(item);
    removeItem(group);
    delete group;
}

/*!
    Adds or moves the item \a item and all its childen to the scene.

    If the item is visible (i.e., QGraphicsItem::isVisible() returns
    true), QGraphicsScene will emit changed() once control goes back
    to the event loop.

    If the item is already in a different scene, it will first be removed from
    its old scene, and then added to this scene as a top-level.

    QGraphicsScene will send ItemSceneChange notifications to \a item while
    it is added to the scene. If item does not currently belong to a scene, only one
    notification is sent. If it does belong to scene already (i.e., it is
    moved to this scene), QGraphicsScene will send an addition notification as
    the item is removed from its previous scene.

    \sa removeItem(), addEllipse(), addLine(), addPath(), addPixmap(),
    addRect(), addText(), addWidget()
*/
void QGraphicsScene::addItem(QGraphicsItem *item)
{
    Q_D(QGraphicsScene);
    if (!item) {
        qWarning("QGraphicsScene::addItem: cannot add null item");
        return;
    }
    if (item->scene() == this) {
        qWarning("QGraphicsScene::addItem: item has already been added to this scene");
        return;
    }

    // Remove this item from its existing scene
    if (QGraphicsScene *oldScene = item->scene())
        oldScene->removeItem(item);

    // Notify the item that its scene is changing, and allow the item to
    // react.
    QGraphicsScene *targetScene = qVariantValue<QGraphicsScene *>(item->itemChange(QGraphicsItem::ItemSceneChange,
                                                                                   qVariantFromValue<QGraphicsScene *>(this)));
    if (targetScene != this) {
        if (targetScene && item->scene() != targetScene)
            targetScene->addItem(item);
        return;
    }

    // Prevent reusing a recently deleted pointer: purge all removed items
    // from our lists.
    d->purgeRemovedItems();

    // Invalidate any sort caching; arrival of a new item means we need to
    // resort.
    d->invalidateSortCache();

    // Detach this item from its parent if the parent's scene is different
    // from this scene.
    if (QGraphicsItem *itemParent = item->parentItem()) {
        if (itemParent->scene() != this)
            item->setParentItem(0);
    }

    // Add the item to this scene
    item->d_func()->scene = targetScene;

    // Indexing requires sceneBoundingRect(), but because \a item might
    // not be completely constructed at this point, we need to store it in
    // a temporary list and schedule an indexing for later.
    d->unindexedItems << item;
    item->d_func()->index = -1;
    d->startIndexTimer();

    // Update the scene's sort cache settings.
    item->d_ptr->globalStackingOrder = -1;
    d->invalidateSortCache();

    // Add to list of items that require an update. We cannot assume that the
    // item is fully constructed, so calling item->update() can lead to a pure
    // virtual function call to boundingRect().
    if (!d->updateAll) {
        if (d->pendingUpdateItems.isEmpty())
            QMetaObject::invokeMethod(this, "_q_updateLater", Qt::QueuedConnection);
        d->pendingUpdateItems << item;
    }

    // Disable selectionChanged() for individual items
    ++d->selectionChanging;
    int oldSelectedItemSize = d->selectedItems.size();

    // Update selection lists
    if (item->isSelected())
        d->selectedItems << item;
    if (item->isWidget() && item->isVisible() && static_cast<QGraphicsWidget *>(item)->windowType() == Qt::Popup)
        d->addPopup(static_cast<QGraphicsWidget *>(item));

    // Update creation order focus chain. Make sure to leave the widget's
    // internal tab order intact.
    if (item->isWidget()) {
        QGraphicsWidget *widget = static_cast<QGraphicsWidget *>(item);
        if (!d->tabFocusFirst) {
            // No first tab focus widget - make this the first tab focus
            // widget.
            d->tabFocusFirst = widget;
        } else if (!widget->parentWidget()) {
            // Adding a widget that is not part of a tab focus chain.
            QGraphicsWidget *last = d->tabFocusFirst->d_func()->focusPrev;
            QGraphicsWidget *lastNew = widget->d_func()->focusPrev;
            last->d_func()->focusNext = widget;
            widget->d_func()->focusPrev = last;
            d->tabFocusFirst->d_func()->focusPrev = lastNew;
            lastNew->d_func()->focusNext = d->tabFocusFirst;
        }
    }

    // Add all children recursively
    foreach (QGraphicsItem *child, item->children())
        addItem(child);

    // Resolve font and palette.
    item->d_ptr->resolveFont(d->font.resolve());
    item->d_ptr->resolvePalette(d->palette.resolve());

    if (!item->d_ptr->explicitlyHidden) {
       if (d->unpolishedItems.isEmpty())
           QMetaObject::invokeMethod(this, "_q_polishItems", Qt::QueuedConnection);
       d->unpolishedItems << item;
    }

    // Reenable selectionChanged() for individual items
    --d->selectionChanging;
    if (!d->selectionChanging && d->selectedItems.size() != oldSelectedItemSize)
        emit selectionChanged();

    // Deliver post-change notification
    item->itemChange(QGraphicsItem::ItemSceneHasChanged, qVariantFromValue<QGraphicsScene *>(this));
}

/*!
    Creates and adds an ellipse item to the scene, and returns the item
    pointer. The geometry of the ellipse is defined by \a rect, and its pen
    and brush are initialized to \a pen and \a brush.

    Note that the item's geometry is provided in item coordinates, and its
    position is initialized to (0, 0).

    If the item is visible (i.e., QGraphicsItem::isVisible() returns true),
    QGraphicsScene will emit changed() once control goes back to the event
    loop.

    \sa addLine(), addPath(), addPixmap(), addRect(), addText(), addItem(),
    addWidget()
*/
QGraphicsEllipseItem *QGraphicsScene::addEllipse(const QRectF &rect, const QPen &pen, const QBrush &brush)
{
    QGraphicsEllipseItem *item = new QGraphicsEllipseItem(rect);
    item->setPen(pen);
    item->setBrush(brush);
    addItem(item);
    return item;
}

/*!
    \fn QGraphicsEllipseItem *QGraphicsScene::addEllipse(qreal x, qreal y, qreal w, qreal h, const QPen &pen, const QBrush &brush)
    \since 4.3

    This convenience function is equivalent to calling addEllipse(QRectF(\a x,
    \a y, \a w, \a h), \a pen, \a brush).
*/

/*!
    Creates and adds a line item to the scene, and returns the item
    pointer. The geometry of the line is defined by \a line, and its pen
    is initialized to \a pen.

    Note that the item's geometry is provided in item coordinates, and its
    position is initialized to (0, 0).

    If the item is visible (i.e., QGraphicsItem::isVisible() returns true),
    QGraphicsScene will emit changed() once control goes back to the event
    loop.

    \sa addEllipse(), addPath(), addPixmap(), addRect(), addText(), addItem(),
    addWidget()
*/
QGraphicsLineItem *QGraphicsScene::addLine(const QLineF &line, const QPen &pen)
{
    QGraphicsLineItem *item = new QGraphicsLineItem(line);
    item->setPen(pen);
    addItem(item);
    return item;
}

/*!
    \fn QGraphicsLineItem *QGraphicsScene::addLine(qreal x1, qreal y1, qreal x2, qreal y2, const QPen &pen)
    \since 4.3

    This convenience function is equivalent to calling addLine(QLineF(\a x1,
    \a y1, \a x2, \a y2), \a pen).
*/

/*!
    Creates and adds a path item to the scene, and returns the item
    pointer. The geometry of the path is defined by \a path, and its pen and
    brush are initialized to \a pen and \a brush.

    Note that the item's geometry is provided in item coordinates, and its
    position is initialized to (0, 0).

    If the item is visible (i.e., QGraphicsItem::isVisible() returns true),
    QGraphicsScene will emit changed() once control goes back to the event
    loop.

    \sa addEllipse(), addLine(), addPixmap(), addRect(), addText(), addItem(),
    addWidget()
*/
QGraphicsPathItem *QGraphicsScene::addPath(const QPainterPath &path, const QPen &pen, const QBrush &brush)
{
    QGraphicsPathItem *item = new QGraphicsPathItem(path);
    item->setPen(pen);
    item->setBrush(brush);
    addItem(item);
    return item;
}

/*!
    Creates and adds a pixmap item to the scene, and returns the item
    pointer. The pixmap is defined by \a pixmap.

    Note that the item's geometry is provided in item coordinates, and its
    position is initialized to (0, 0).

    If the item is visible (i.e., QGraphicsItem::isVisible() returns true),
    QGraphicsScene will emit changed() once control goes back to the event
    loop.

    \sa addEllipse(), addLine(), addPath(), addRect(), addText(), addItem(),
    addWidget()
*/
QGraphicsPixmapItem *QGraphicsScene::addPixmap(const QPixmap &pixmap)
{
    QGraphicsPixmapItem *item = new QGraphicsPixmapItem(pixmap);
    addItem(item);
    return item;
}

/*!
    Creates and adds a polygon item to the scene, and returns the item
    pointer. The polygon is defined by \a polygon, and its pen and
    brush are initialized to \a pen and \a brush.

    Note that the item's geometry is provided in item coordinates, and its
    position is initialized to (0, 0).

    If the item is visible (i.e., QGraphicsItem::isVisible() returns true),
    QGraphicsScene will emit changed() once control goes back to the event
    loop.

    \sa addEllipse(), addLine(), addPath(), addRect(), addText(), addItem(),
    addWidget()
*/
QGraphicsPolygonItem *QGraphicsScene::addPolygon(const QPolygonF &polygon,
                                                 const QPen &pen, const QBrush &brush)
{
    QGraphicsPolygonItem *item = new QGraphicsPolygonItem(polygon);
    item->setPen(pen);
    item->setBrush(brush);
    addItem(item);
    return item;
}

/*!
    Creates and adds a rectangle item to the scene, and returns the item
    pointer. The geometry of the rectangle is defined by \a rect, and its pen
    and brush are initialized to \a pen and \a brush.

    Note that the item's geometry is provided in item coordinates, and its
    position is initialized to (0, 0). For example, if a QRect(50, 50, 100,
    100) is added, its top-left corner will be at (50, 50) relative to the
    origin in the items coordinate system.

    If the item is visible (i.e., QGraphicsItem::isVisible() returns true),
    QGraphicsScene will emit changed() once control goes back to the event
    loop.

    \sa addEllipse(), addLine(), addPixmap(), addPixmap(), addText(),
    addItem(), addWidget()
*/
QGraphicsRectItem *QGraphicsScene::addRect(const QRectF &rect, const QPen &pen, const QBrush &brush)
{
    QGraphicsRectItem *item = new QGraphicsRectItem(rect);
    item->setPen(pen);
    item->setBrush(brush);
    addItem(item);
    return item;
}

/*!
    \fn QGraphicsRectItem *QGraphicsScene::addRect(qreal x, qreal y, qreal w, qreal h, const QPen &pen, const QBrush &brush)
    \since 4.3

    This convenience function is equivalent to calling addRect(QRectF(\a x,
    \a y, \a w, \a h), \a pen, \a brush).
*/

/*!
    Creates and adds a text item to the scene, and returns the item
    pointer. The text string is initialized to \a text, and its font
    is initialized to \a font.

    The item's position is initialized to (0, 0).

    If the item is visible (i.e., QGraphicsItem::isVisible() returns true),
    QGraphicsScene will emit changed() once control goes back to the event
    loop.

    \sa addEllipse(), addLine(), addPixmap(), addPixmap(), addRect(),
    addItem(), addWidget()
*/
QGraphicsTextItem *QGraphicsScene::addText(const QString &text, const QFont &font)
{
    QGraphicsTextItem *item = new QGraphicsTextItem(text);
    item->setFont(font);
    addItem(item);
    return item;
}

/*!
    Creates and adds a QGraphicsSimpleTextItem to the scene, and returns the
    item pointer. The text string is initialized to \a text, and its font is
    initialized to \a font.

    The item's position is initialized to (0, 0).

    If the item is visible (i.e., QGraphicsItem::isVisible() returns true),
    QGraphicsScene will emit changed() once control goes back to the event
    loop.

    \sa addEllipse(), addLine(), addPixmap(), addPixmap(), addRect(),
    addItem(), addWidget()
*/
QGraphicsSimpleTextItem *QGraphicsScene::addSimpleText(const QString &text, const QFont &font)
{
    QGraphicsSimpleTextItem *item = new QGraphicsSimpleTextItem(text);
    item->setFont(font);
    addItem(item);
    return item;
}

/*!
    Creates a new QGraphicsProxyWidget for \a widget, adds it to the scene,
    and returns a pointer to the proxy. \a wFlags set the default window flags
    for the embedding proxy widget.

    The item's position is initialized to (0, 0).

    If the item is visible (i.e., QGraphicsItem::isVisible() returns true),
    QGraphicsScene will emit changed() once control goes back to the event
    loop.

    Note that widgets with the Qt::WA_PaintOnScreen widget attribute
    set and widgets that wrap an external application or controller
    are not supported. Examples are QGLWidget and QAxWidget.

    \sa addEllipse(), addLine(), addPixmap(), addPixmap(), addRect(),
    addText(), addSimpleText(), addItem()
*/
QGraphicsProxyWidget *QGraphicsScene::addWidget(QWidget *widget, Qt::WindowFlags wFlags)
{
    QGraphicsProxyWidget *proxy = new QGraphicsProxyWidget(0, wFlags);
    proxy->setWidget(widget);
    addItem(proxy);
    return proxy;
}

/*!
    Removes the item \a item and all its children from the scene.  The
    ownership of \a item is passed on to the caller (i.e.,
    QGraphicsScene will no longer delete \a item when destroyed).

    \sa addItem()
*/
void QGraphicsScene::removeItem(QGraphicsItem *item)
{
    // ### Refactoring: This function shares much functionality with _q_removeItemLater()
    Q_D(QGraphicsScene);
    if (!item) {
        qWarning("QGraphicsScene::removeItem: cannot remove 0-item");
        return;
    }
    if (item->scene() != this) {
        qWarning("QGraphicsScene::removeItem: item %p's scene (%p)"
                 " is different from this scene (%p)",
                 item, item->scene(), this);
        return;
    }

    // Notify the item that it's scene is changing to 0, allowing the item to
    // react.
    QGraphicsScene *targetScene = qVariantValue<QGraphicsScene *>(item->itemChange(QGraphicsItem::ItemSceneChange,
                                                                                   qVariantFromValue<QGraphicsScene *>(0)));
    if (targetScene != 0 && targetScene != this) {
        targetScene->addItem(item);
        return;
    }

    // If the item has focus, remove it (and any focusWidget reference).
    item->clearFocus();

    // Clear its background
    item->update();

    // Note: This will access item's sceneBoundingRect(), which (as this is
    // C++) is why we cannot call removeItem() from QGraphicsItem's
    // destructor.
    d->removeFromIndex(item);

    if (item == d->tabFocusFirst) {
        QGraphicsWidget *widget = static_cast<QGraphicsWidget *>(item);
        widget->d_func()->fixFocusChainBeforeReparenting(0, 0);
    }
    // Set the item's scene ptr to 0.
    item->d_func()->scene = 0;

    // Detach the item from its parent.
    if (QGraphicsItem *parentItem = item->parentItem()) {
        if (parentItem->scene()) {
            Q_ASSERT_X(parentItem->scene() == this, "QGraphicsScene::removeItem",
                       "Parent item's scene is different from this item's scene");
            item->setParentItem(0);
        }
    }

    // Remove from our item lists.
    int index = item->d_func()->index;
    if (index != -1) {
        d->freeItemIndexes << index;
        d->indexedItems[index] = 0;
    } else {
        d->unindexedItems.removeAll(item);
    }

    // Remove from scene transform cache
    int transformIndex = item->d_func()->sceneTransformIndex;
    if (transformIndex != -1) {
        d->validTransforms.setBit(transformIndex, 0);
        d->freeSceneTransformSlots.append(transformIndex);
        item->d_func()->sceneTransformIndex = -1;
    }

    if (item == d->focusItem)
        d->focusItem = 0;
    if (item == d->lastFocusItem)
        d->lastFocusItem = 0;
    if (item == d->activeWindow) {
        // ### deactivate...
        d->activeWindow = 0;
    }

    // Disable selectionChanged() for individual items
    ++d->selectionChanging;
    int oldSelectedItemsSize = d->selectedItems.size();

    // Update selected & hovered item bookkeeping
    d->selectedItems.remove(item);
    d->hoverItems.removeAll(item);
    d->pendingUpdateItems.removeAll(item);
    d->cachedItemsUnderMouse.removeAll(item);
    d->unpolishedItems.removeAll(item);
    d->dirtyItems.removeAll(item);

    //Ensure dirty flag have the correct default value so the next time it will be added it will receive updates
    item->d_func()->dirty = 0;
    item->d_func()->dirtyChildren = 0;

    // Remove all children recursively
    foreach (QGraphicsItem *child, item->children())
        removeItem(child);

    // Reset the mouse grabber and focus item data.
    if (d->mouseGrabberItems.contains(item))
        d->ungrabMouse(item);

    // Reset the keyboard grabber
    if (d->keyboardGrabberItems.contains(item))
        item->ungrabKeyboard();

    // Reset the last mouse grabber item
    if (item == d->lastMouseGrabberItem)
        d->lastMouseGrabberItem = 0;

    // Reenable selectionChanged() for individual items
    --d->selectionChanging;

    if (!d->selectionChanging && d->selectedItems.size() != oldSelectedItemsSize)
        emit selectionChanged();

    // Deliver post-change notification
    item->itemChange(QGraphicsItem::ItemSceneHasChanged, qVariantFromValue<QGraphicsScene *>(0));
}

/*!
    Returns the scene's current focus item, or 0 if no item currently has
    focus.

    The focus item receives keyboard input when the scene receives a
    key event.

    \sa setFocusItem(), QGraphicsItem::hasFocus()
*/
QGraphicsItem *QGraphicsScene::focusItem() const
{
    Q_D(const QGraphicsScene);
    return d->focusItem;
}

/*!
    Sets the scene's focus item to \a item, with the focus reason \a
    focusReason, after removing focus from any previous item that may have had
    focus.

    If \a item is 0, or if it either does not accept focus (i.e., it does not
    have the QGraphicsItem::ItemIsFocusable flag enabled), or is not visible
    or not enabled, this function only removes focus from any previous
    focusitem.

    If item is not 0, and the scene does not currently have focus (i.e.,
    hasFocus() returns false), this function will call setFocus()
    automatically.

    \sa focusItem(), hasFocus(), setFocus()
*/
void QGraphicsScene::setFocusItem(QGraphicsItem *item, Qt::FocusReason focusReason)
{
    Q_D(QGraphicsScene);
    if (item == d->focusItem)
        return;
    if (item && (!(item->flags() & QGraphicsItem::ItemIsFocusable)
                 || !item->isVisible() || !item->isEnabled())) {
        item = 0;
    }

    if (item) {
        setFocus(focusReason);
        if (item == d->focusItem)
            return;
    }

    if (d->focusItem) {
        QFocusEvent event(QEvent::FocusOut, focusReason);
        d->lastFocusItem = d->focusItem;
        d->focusItem = 0;
        d->sendEvent(d->lastFocusItem, &event);
    }

    if (item) {
        if (item->isWidget()) {
            // Update focus child chain.
            static_cast<QGraphicsWidget *>(item)->d_func()->setFocusWidget();
        }

        d->focusItem = item;
        QFocusEvent event(QEvent::FocusIn, focusReason);
        d->sendEvent(item, &event);
    }
}

/*!
    Returns true if the scene has focus; otherwise returns false. If the scene
    has focus, it will will forward key events from QKeyEvent to any item that
    has focus.

    \sa setFocus(), setFocusItem()
*/
bool QGraphicsScene::hasFocus() const
{
    Q_D(const QGraphicsScene);
    return d->hasFocus;
}

/*!
    Sets focus on the scene by sending a QFocusEvent to the scene, passing \a
    focusReason as the reason. If the scene regains focus after having
    previously lost it while an item had focus, the last focus item will
    receive focus with \a focusReason as the reason.

    If the scene already has focus, this function does nothing.

    \sa hasFocus(), clearFocus(), setFocusItem()
*/
void QGraphicsScene::setFocus(Qt::FocusReason focusReason)
{
    Q_D(QGraphicsScene);
    if (d->hasFocus)
        return;
    QFocusEvent event(QEvent::FocusIn, focusReason);
    QCoreApplication::sendEvent(this, &event);
}

/*!
    Clears focus from the scene. If any item has focus when this function is
    called, it will lose focus, and regain focus again once the scene regains
    focus.

    A scene that does not have focus ignores key events.

    \sa hasFocus(), setFocus(), setFocusItem()
*/
void QGraphicsScene::clearFocus()
{
    Q_D(QGraphicsScene);
    if (d->hasFocus) {
        d->hasFocus = false;
        setFocusItem(0, Qt::OtherFocusReason);
    }
}

/*!
    \property QGraphicsScene::stickyFocus
    \brief whether or not clicking the scene will clear focus

    If this property is false (the default), then clicking on the scene
    background or on an item that does not accept focus, will clear
    focus. Otherwise, focus will remain unchanged.

    The focus change happens in response to a mouse press. You can reimplement
    mousePressEvent() in a subclass of QGraphicsScene to toggle this property
    based on where the user has clicked.

    \sa clearFocus(), setFocusItem()
*/
void QGraphicsScene::setStickyFocus(bool enabled)
{
    Q_D(QGraphicsScene);
    d->stickyFocus = enabled;
}
bool QGraphicsScene::stickyFocus() const
{
    Q_D(const QGraphicsScene);
    return d->stickyFocus;
}

/*!
    Returns the current mouse grabber item, or 0 if no item is currently
    grabbing the mouse. The mouse grabber item is the item that receives all
    mouse events sent to the scene.

    An item becomes a mouse grabber when it receives and accepts a
    mouse press event, and it stays the mouse grabber until either of
    the following events occur:

    \list
    \o If the item receives a mouse release event when there are no other
    buttons pressed, it loses the mouse grab.
    \o If the item becomes invisible (i.e., someone calls \c {item->setVisible(false))},
    or if it becomes disabled (i.e., someone calls \c {item->setEnabled(false))},
    it loses the mouse grab.
    \o If the item is removed from the scene, it loses the mouse grab.
    \endlist

    If the item loses its mouse grab, the scene will ignore all mouse events
    until a new item grabs the mouse (i.e., until a new item receives a mouse
    press event).
*/
QGraphicsItem *QGraphicsScene::mouseGrabberItem() const
{
    Q_D(const QGraphicsScene);
    return !d->mouseGrabberItems.isEmpty() ? d->mouseGrabberItems.last() : 0;
}

/*!
    \property QGraphicsScene::backgroundBrush
    \brief the background brush of the scene.

    Set this property to changes the scene's background to a different color,
    gradient or texture. The default background brush is Qt::NoBrush. The
    background is drawn before (behind) the items.

    Example:

    \snippet doc/src/snippets/code/src_gui_graphicsview_qgraphicsscene.cpp 3

    QGraphicsScene::render() calls drawBackground() to draw the scene
    background. For more detailed control over how the background is drawn,
    you can reimplement drawBackground() in a subclass of QGraphicsScene.
*/
QBrush QGraphicsScene::backgroundBrush() const
{
    Q_D(const QGraphicsScene);
    return d->backgroundBrush;
}
void QGraphicsScene::setBackgroundBrush(const QBrush &brush)
{
    Q_D(QGraphicsScene);
    d->backgroundBrush = brush;
    foreach (QGraphicsView *view, d->views) {
        view->resetCachedContent();
        view->viewport()->update();
    }
    update();
}

/*!
    \property QGraphicsScene::foregroundBrush
    \brief the foreground brush of the scene.

    Change this property to set the scene's foreground to a different
    color, gradient or texture.

    The foreground is drawn after (on top of) the items. The default
    foreground brush is Qt::NoBrush ( i.e. the foreground is not
    drawn).

    Example:

    \snippet doc/src/snippets/code/src_gui_graphicsview_qgraphicsscene.cpp 4

    QGraphicsScene::render() calls drawForeground() to draw the scene
    foreground. For more detailed control over how the foreground is
    drawn, you can reimplement the drawForeground() function in a
    QGraphicsScene subclass.
*/
QBrush QGraphicsScene::foregroundBrush() const
{
    Q_D(const QGraphicsScene);
    return d->foregroundBrush;
}
void QGraphicsScene::setForegroundBrush(const QBrush &brush)
{
    Q_D(QGraphicsScene);
    d->foregroundBrush = brush;
    foreach (QGraphicsView *view, views())
        view->viewport()->update();
    update();
}

/*!
    This method is used by input methods to query a set of properties of
    the scene to be able to support complex input method operations as support
    for surrounding text and reconversions.

    The \a query parameter specifies which property is queried.

    \sa QWidget::inputMethodQuery()
*/
QVariant QGraphicsScene::inputMethodQuery(Qt::InputMethodQuery query) const
{
    Q_D(const QGraphicsScene);
    if (!d->focusItem)
        return QVariant();
    const QTransform matrix = d->focusItem->sceneTransform();
    QVariant value = d->focusItem->inputMethodQuery(query);
    if (value.type() == QVariant::RectF)
        value = matrix.mapRect(value.toRectF());
    else if (value.type() == QVariant::PointF)
        value = matrix.map(value.toPointF());
    else if (value.type() == QVariant::Rect)
        value = matrix.mapRect(value.toRect());
    else if (value.type() == QVariant::Point)
        value = matrix.map(value.toPoint());
    return value;
}

/*!
    \fn void QGraphicsScene::update(const QRectF &rect)
    Schedules a redraw of the area \a rect on the scene.

    \sa sceneRect(), changed()
*/
void QGraphicsScene::update(const QRectF &rect)
{
    Q_D(QGraphicsScene);
    if (d->updateAll)
        return;

    // Check if anyone's connected; if not, we can send updates directly to
    // the views. Otherwise or if there are no views, use old behavior.
    bool directUpdates = !(d->connectedSignals & d->changedSignalMask) && !d->views.isEmpty();
    if (rect.isNull()) {
        d->updateAll = true;
        d->updatedRects.clear();
        if (directUpdates) {
            // Update all views.
            for (int i = 0; i < d->views.size(); ++i)
                d->views.at(i)->d_func()->updateAll();
        }
    } else {
        if (directUpdates) {
            // Update all views.
            for (int i = 0; i < d->views.size(); ++i) {
                QGraphicsView *view = d->views.at(i);
                view->d_func()->updateRegion(QRegion(view->mapFromScene(rect).boundingRect()));
            }
        } else {
            d->updatedRects << rect;
        }
    }

    if (!directUpdates && !d->calledEmitUpdated) {
        d->calledEmitUpdated = true;
        QMetaObject::invokeMethod(this, "_q_emitUpdated", Qt::QueuedConnection);
    }
}

/*!
    \fn void QGraphicsScene::update(qreal x, qreal y, qreal w, qreal h)
    \overload
    \since 4.3

    This function is equivalent to calling update(QRectF(\a x, \a y, \a w,
    \a h));
*/

/*!
    Invalidates and schedules a redraw of the \a layers in \a rect on the
    scene. Any cached content in \a layers is unconditionally invalidated and
    redrawn.

    You can use this function overload to notify QGraphicsScene of changes to
    the background or the foreground of the scene. This function is commonly
    used for scenes with tile-based backgrounds to notify changes when
    QGraphicsView has enabled
    \l{QGraphicsView::CacheBackground}{CacheBackground}.

    Example:

    \snippet doc/src/snippets/code/src_gui_graphicsview_qgraphicsscene.cpp 5

    Note that QGraphicsView currently supports background caching only (see
    QGraphicsView::CacheBackground). This function is equivalent to calling
    update() if any layer but BackgroundLayer is passed.

    \sa QGraphicsView::resetCachedContent()
*/
void QGraphicsScene::invalidate(const QRectF &rect, SceneLayers layers)
{
    foreach (QGraphicsView *view, views())
        view->invalidateScene(rect, layers);
    update(rect);
}

/*!
    \fn void QGraphicsScene::invalidate(qreal x, qreal y, qreal w, qreal h, SceneLayers layers)
    \overload
    \since 4.3

    This convenience function is equivalent to calling invalidate(QRectF(\a x, \a
    y, \a w, \a h), \a layers);
*/

/*!
    Returns a list of all the views that display this scene.

    \sa QGraphicsView::scene()
*/
QList <QGraphicsView *> QGraphicsScene::views() const
{
    Q_D(const QGraphicsScene);
    return d->views;
}

/*!
    This slot \e advances the scene by one step, by calling
    QGraphicsItem::advance() for all items on the scene. This is done in two
    phases: in the first phase, all items are notified that the scene is about
    to change, and in the second phase all items are notified that they can
    move. In the first phase, QGraphicsItem::advance() is called passing a
    value of 0 as an argument, and 1 is passed in the second phase.

    \sa QGraphicsItem::advance(), QGraphicsItemAnimation, QTimeLine
*/
void QGraphicsScene::advance()
{
    for (int i = 0; i < 2; ++i) {
        foreach (QGraphicsItem *item, items())
            item->advance(i);
    }
}

/*!
    Processes the event \a event, and dispatches it to the respective
    event handlers.

    In addition to calling the convenience event handlers, this
    function is responsible for converting mouse move events to hover
    events for when there is no mouse grabber item. Hover events are
    delivered directly to items; there is no convenience function for
    them.

    Unlike QWidget, QGraphicsScene does not have the convenience functions
    \l{QWidget::}{enterEvent()} and \l{QWidget::}{leaveEvent()}. Use this
    function to obtain those events instead.

    \sa contextMenuEvent(), keyPressEvent(), keyReleaseEvent(),
    mousePressEvent(), mouseMoveEvent(), mouseReleaseEvent(),
    mouseDoubleClickEvent(), focusInEvent(), focusOutEvent()
*/
bool QGraphicsScene::event(QEvent *event)
{
    Q_D(QGraphicsScene);

    switch (event->type()) {
    case QEvent::GraphicsSceneMousePress:
    case QEvent::GraphicsSceneMouseMove:
    case QEvent::GraphicsSceneMouseRelease:
    case QEvent::GraphicsSceneMouseDoubleClick:
    case QEvent::GraphicsSceneHoverEnter:
    case QEvent::GraphicsSceneHoverLeave:
    case QEvent::GraphicsSceneHoverMove:
        // Reset the under-mouse list to ensure that this event gets fresh
        // item-under-mouse data. Be careful about this list; if people delete
        // items from inside event handlers, this list can quickly end up
        // having stale pointers in it. We need to clear it before dispatching
        // events that use it.
        d->cachedItemsUnderMouse.clear();
    default:
        break;
    }

    switch (event->type()) {
    case QEvent::GraphicsSceneDragEnter:
        dragEnterEvent(static_cast<QGraphicsSceneDragDropEvent *>(event));
        break;
    case QEvent::GraphicsSceneDragMove:
        dragMoveEvent(static_cast<QGraphicsSceneDragDropEvent *>(event));
        break;
    case QEvent::GraphicsSceneDragLeave:
        dragLeaveEvent(static_cast<QGraphicsSceneDragDropEvent *>(event));
        break;
    case QEvent::GraphicsSceneDrop:
        dropEvent(static_cast<QGraphicsSceneDragDropEvent *>(event));
        break;
    case QEvent::GraphicsSceneContextMenu:
        contextMenuEvent(static_cast<QGraphicsSceneContextMenuEvent *>(event));
        break;
    case QEvent::KeyPress:
        if (!d->focusItem) {
            QKeyEvent *k = static_cast<QKeyEvent *>(event);
            if (k->key() == Qt::Key_Tab || k->key() == Qt::Key_Backtab) {
                if (!(k->modifiers() & (Qt::ControlModifier | Qt::AltModifier))) {  //### Add MetaModifier?
                    bool res = false;
                    if (k->key() == Qt::Key_Backtab
                        || (k->key() == Qt::Key_Tab && (k->modifiers() & Qt::ShiftModifier))) {
                        res = focusNextPrevChild(false);
                    } else if (k->key() == Qt::Key_Tab) {
                        res = focusNextPrevChild(true);
                    }
                    if (!res)
                        event->ignore();
                    return true;
                }
            }
        }
        keyPressEvent(static_cast<QKeyEvent *>(event));
        break;
    case QEvent::KeyRelease:
        keyReleaseEvent(static_cast<QKeyEvent *>(event));
        break;
    case QEvent::ShortcutOverride: {
            QGraphicsItem *parent = focusItem();
            while (parent) {
                d->sendEvent(parent, event);
                if (event->isAccepted())
                    return true;
                parent = parent->parentItem();
            }
        }
        return false;
    case QEvent::GraphicsSceneMouseMove:
        mouseMoveEvent(static_cast<QGraphicsSceneMouseEvent *>(event));
        break;
    case QEvent::GraphicsSceneMousePress:
        mousePressEvent(static_cast<QGraphicsSceneMouseEvent *>(event));
        break;
    case QEvent::GraphicsSceneMouseRelease:
        mouseReleaseEvent(static_cast<QGraphicsSceneMouseEvent *>(event));
        break;
    case QEvent::GraphicsSceneMouseDoubleClick:
        mouseDoubleClickEvent(static_cast<QGraphicsSceneMouseEvent *>(event));
        break;
    case QEvent::GraphicsSceneWheel:
        wheelEvent(static_cast<QGraphicsSceneWheelEvent *>(event));
        break;
    case QEvent::FocusIn:
        focusInEvent(static_cast<QFocusEvent *>(event));
        break;
    case QEvent::FocusOut:
        focusOutEvent(static_cast<QFocusEvent *>(event));
        break;
    case QEvent::GraphicsSceneHoverEnter:
    case QEvent::GraphicsSceneHoverLeave:
    case QEvent::GraphicsSceneHoverMove:
        d->dispatchHoverEvent(static_cast<QGraphicsSceneHoverEvent *>(event));
        break;
    case QEvent::Leave:
        d->leaveScene();
        break;
    case QEvent::GraphicsSceneHelp:
        helpEvent(static_cast<QGraphicsSceneHelpEvent *>(event));
        break;
    case QEvent::InputMethod:
        inputMethodEvent(static_cast<QInputMethodEvent *>(event));
        break;
    case QEvent::WindowActivate: {
        if (!d->activationRefCount++) {
            // Notify all non-window widgets.
            foreach (QGraphicsItem *item, items()) {
                if (item->isWidget() && item->isVisible() && !item->isWindow() && !item->parentWidget()) {
                    QEvent event(QEvent::WindowActivate);
                    QApplication::sendEvent(static_cast<QGraphicsWidget *>(item), &event);
                }
            }

            // Restore window activation.
            QGraphicsItem *nextFocusItem = d->focusItem ? d->focusItem : d->lastFocusItem;
            if (nextFocusItem && nextFocusItem->window())
                setActiveWindow(static_cast<QGraphicsWidget *>(nextFocusItem));
            else if (d->tabFocusFirst && d->tabFocusFirst->isWindow())
                setActiveWindow(d->tabFocusFirst);
        }
        break;
    }
    case QEvent::WindowDeactivate: {
        if (!--d->activationRefCount) {
            // Remove window activation.
            setActiveWindow(0);

            // Notify all non-window widgets.
            foreach (QGraphicsItem *item, items()) {
                if (item->isWidget() && item->isVisible() && !item->isWindow() && !item->parentWidget()) {
                    QEvent event(QEvent::WindowDeactivate);
                    QApplication::sendEvent(static_cast<QGraphicsWidget *>(item), &event);
                }
            }
        }
        break;
    }
    case QEvent::ApplicationFontChange: {
        // Resolve the existing scene font.
        d->resolveFont();
        break;
    }
    case QEvent::FontChange:
        // Update the entire scene when the font changes.
        update();
        break;
    case QEvent::ApplicationPaletteChange: {
        // Resolve the existing scene palette.
        d->resolvePalette();
        break;
    }
    case QEvent::PaletteChange:
        // Update the entire scene when the palette changes.
        update();
        break;
    case QEvent::StyleChange:
        // Reresolve all widgets' styles. Update all top-level widgets'
        // geometries that do not have an explicit style set.
        update();
        break;
    case QEvent::Timer:
        if (d->indexTimerId && static_cast<QTimerEvent *>(event)->timerId() == d->indexTimerId) {
            if (d->restartIndexTimer) {
                d->restartIndexTimer = false;
            } else {
                // this call will kill the timer
                d->_q_updateIndex();
            }
        }
        // Fallthrough intended - support timers in subclasses.
    default:
        return QObject::event(event);
    }
    return true;
}

/*!
    \reimp

    QGraphicsScene filters QApplication's events to detect palette and font
    changes.
*/
bool QGraphicsScene::eventFilter(QObject *watched, QEvent *event)
{
    if (watched != qApp)
        return false;

    switch (event->type()) {
    case QEvent::ApplicationPaletteChange:
        qApp->postEvent(this, new QEvent(QEvent::ApplicationPaletteChange));
        break;
    case QEvent::ApplicationFontChange:
        qApp->postEvent(this, new QEvent(QEvent::ApplicationFontChange));
        break;
    default:
        break;
    }
    return false;
}

/*!
    This event handler, for event \a contextMenuEvent, can be reimplemented in
    a subclass to receive context menu events. The default implementation
    forwards the event to the topmost item that accepts context menu events at
    the position of the event. If no items accept context menu events at this
    position, the event is ignored.

    \sa QGraphicsItem::contextMenuEvent()
*/
void QGraphicsScene::contextMenuEvent(QGraphicsSceneContextMenuEvent *contextMenuEvent)
{
    Q_D(QGraphicsScene);
    // Ignore by default.
    contextMenuEvent->ignore();

    // Send the event to all items at this position until one item accepts the
    // event.
    foreach (QGraphicsItem *item, d->itemsAtPosition(contextMenuEvent->screenPos(),
                                                     contextMenuEvent->scenePos(),
                                                     contextMenuEvent->widget())) {
        contextMenuEvent->setPos(item->d_ptr->genericMapFromScene(contextMenuEvent->scenePos(),
                                                                  contextMenuEvent->widget()));
        contextMenuEvent->accept();
        if (!d->sendEvent(item, contextMenuEvent))
            break;

        if (contextMenuEvent->isAccepted())
            break;
    }
}

/*!
    This event handler, for event \a event, can be reimplemented in a subclass
    to receive drag enter events for the scene.

    The default implementation accepts the event and prepares the scene to
    accept drag move events.

    \sa QGraphicsItem::dragEnterEvent(), dragMoveEvent(), dragLeaveEvent(),
    dropEvent()
*/
void QGraphicsScene::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    Q_D(QGraphicsScene);
    d->dragDropItem = 0;
    d->lastDropAction = Qt::IgnoreAction;
    event->accept();
}

/*!
    This event handler, for event \a event, can be reimplemented in a subclass
    to receive drag move events for the scene.

    \sa QGraphicsItem::dragMoveEvent(), dragEnterEvent(), dragLeaveEvent(),
    dropEvent()
*/
void QGraphicsScene::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    Q_D(QGraphicsScene);
    event->ignore();

    if (!d->mouseGrabberItems.isEmpty()) {
        // Mouse grabbers that start drag events lose the mouse grab.
        d->clearMouseGrabber();
        d->mouseGrabberButtonDownPos.clear();
        d->mouseGrabberButtonDownScenePos.clear();
        d->mouseGrabberButtonDownScreenPos.clear();
    }

    bool eventDelivered = false;

    // Find the topmost enabled items under the cursor. They are all
    // candidates for accepting drag & drop events.
    foreach (QGraphicsItem *item, d->itemsAtPosition(event->screenPos(),
                                                     event->scenePos(),
                                                     event->widget())) {
        if (!item->isEnabled() || !item->acceptDrops())
            continue;

        if (item != d->dragDropItem) {
            // Enter the new drag drop item. If it accepts the event, we send
            // the leave to the parent item.
            QGraphicsSceneDragDropEvent dragEnter(QEvent::GraphicsSceneDragEnter);
            d->cloneDragDropEvent(&dragEnter, event);
            dragEnter.setDropAction(event->proposedAction());
            d->sendDragDropEvent(item, &dragEnter);
            event->setAccepted(dragEnter.isAccepted());
            event->setDropAction(dragEnter.dropAction());
            if (!event->isAccepted()) {
                // Propagate to the item under
                continue;
            }

            d->lastDropAction = event->dropAction();

            if (d->dragDropItem) {
                // Leave the last drag drop item. A perfect implementation
                // would set the position of this event to the point where
                // this event and the last event intersect with the item's
                // shape, but that's not easy to do. :-)
                QGraphicsSceneDragDropEvent dragLeave(QEvent::GraphicsSceneDragLeave);
                d->cloneDragDropEvent(&dragLeave, event);
                d->sendDragDropEvent(d->dragDropItem, &dragLeave);
            }

            // We've got a new drag & drop item
            d->dragDropItem = item;
        }

        // Send the move event.
        event->setDropAction(d->lastDropAction);
        event->accept();
        d->sendDragDropEvent(item, event);
        if (event->isAccepted())
            d->lastDropAction = event->dropAction();
        eventDelivered = true;
        break;
    }

    if (!eventDelivered) {
        if (d->dragDropItem) {
            // Leave the last drag drop item
            QGraphicsSceneDragDropEvent dragLeave(QEvent::GraphicsSceneDragLeave);
            d->cloneDragDropEvent(&dragLeave, event);
            d->sendDragDropEvent(d->dragDropItem, &dragLeave);
            d->dragDropItem = 0;
        }
        // Propagate
        event->setDropAction(Qt::IgnoreAction);
    }
}

/*!
    This event handler, for event \a event, can be reimplemented in a subclass
    to receive drag leave events for the scene.

    \sa QGraphicsItem::dragLeaveEvent(), dragEnterEvent(), dragMoveEvent(),
    dropEvent()
*/
void QGraphicsScene::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
{
    Q_D(QGraphicsScene);
    if (d->dragDropItem) {
        // Leave the last drag drop item
        d->sendDragDropEvent(d->dragDropItem, event);
        d->dragDropItem = 0;
    }
}

/*!
    This event handler, for event \a event, can be reimplemented in a subclass
    to receive drop events for the scene.

    \sa QGraphicsItem::dropEvent(), dragEnterEvent(), dragMoveEvent(),
    dragLeaveEvent()
*/
void QGraphicsScene::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    Q_UNUSED(event);
    Q_D(QGraphicsScene);
    if (d->dragDropItem) {
        // Drop on the last drag drop item
        d->sendDragDropEvent(d->dragDropItem, event);
        d->dragDropItem = 0;
    }
}

/*!
    This event handler, for event \a focusEvent, can be reimplemented in a
    subclass to receive focus in events.

    The default implementation sets focus on the scene, and then on the last
    focus item.

    \sa QGraphicsItem::focusOutEvent()
*/
void QGraphicsScene::focusInEvent(QFocusEvent *focusEvent)
{
    Q_D(QGraphicsScene);

    d->hasFocus = true;
    switch (focusEvent->reason()) {
    case Qt::TabFocusReason:
        if (!focusNextPrevChild(true))
            focusEvent->ignore();
        break;
    case Qt::BacktabFocusReason:
        if (!focusNextPrevChild(false))
            focusEvent->ignore();
        break;
    default:
        if (d->lastFocusItem) {
            // Set focus on the last focus item
            setFocusItem(d->lastFocusItem, focusEvent->reason());
        }
        break;
    }
}

/*!
    This event handler, for event \a focusEvent, can be reimplemented in a
    subclass to receive focus out events.

    The default implementation removes focus from any focus item, then removes
    focus from the scene.

    \sa QGraphicsItem::focusInEvent()
*/
void QGraphicsScene::focusOutEvent(QFocusEvent *focusEvent)
{
    Q_D(QGraphicsScene);
    d->hasFocus = false;
    setFocusItem(0, focusEvent->reason());

    // Remove all popups when the scene loses focus.
    if (!d->popupWidgets.isEmpty())
        d->removePopup(d->popupWidgets.first());
}

/*!
    This event handler, for event \a helpEvent, can be
    reimplemented in a subclass to receive help events. The events
    are of type QEvent::ToolTip, which are created when a tooltip is
    requested.

    The default implementation shows the tooltip of the topmost
    item, i.e., the item with the highest z-value, at the mouse
    cursor position. If no item has a tooltip set, this function
    does nothing.

   \sa QGraphicsItem::toolTip(), QGraphicsSceneHelpEvent
*/
void QGraphicsScene::helpEvent(QGraphicsSceneHelpEvent *helpEvent)
{
#ifdef QT_NO_TOOLTIP
    Q_UNUSED(helpEvent);
#else
    // Find the first item that does tooltips
    Q_D(QGraphicsScene);
    QList<QGraphicsItem *> itemsAtPos = d->itemsAtPosition(helpEvent->screenPos(),
                                                           helpEvent->scenePos(),
                                                           helpEvent->widget());
    QGraphicsItem *toolTipItem = 0;
    for (int i = 0; i < itemsAtPos.size(); ++i) {
        QGraphicsItem *tmp = itemsAtPos.at(i);
        if (!tmp->toolTip().isEmpty()) {
            toolTipItem = tmp;
            break;
        }
    }

    // Show or hide the tooltip
    QString text;
    QPoint point;
    if (toolTipItem && !toolTipItem->toolTip().isEmpty()) {
        text = toolTipItem->toolTip();
        point = helpEvent->screenPos();
    }
    QToolTip::showText(point, text);
    helpEvent->setAccepted(!text.isEmpty());
#endif
}

bool QGraphicsScenePrivate::itemAcceptsHoverEvents_helper(const QGraphicsItem *item) const
{
    return item->acceptHoverEvents()
        || (item->isWidget() && static_cast<const QGraphicsWidget *>(item)->d_func()->hasDecoration());
}

/*!
    This event handler, for event \a hoverEvent, can be reimplemented in a
    subclass to receive hover enter events. The default implementation
    forwards the event to the topmost item that accepts hover events at the
    scene position from the event.

    \sa QGraphicsItem::hoverEvent(), QGraphicsItem::setAcceptHoverEvents()
*/
bool QGraphicsScenePrivate::dispatchHoverEvent(QGraphicsSceneHoverEvent *hoverEvent)
{
    // Find the first item that accepts hover events, reusing earlier
    // calculated data is possible.
    if (cachedItemsUnderMouse.isEmpty()) {
        cachedItemsUnderMouse = itemsAtPosition(hoverEvent->screenPos(),
                                                hoverEvent->scenePos(),
                                                hoverEvent->widget());
    }

    QGraphicsItem *item = 0;
    for (int i = 0; i < cachedItemsUnderMouse.size(); ++i) {
        QGraphicsItem *tmp = cachedItemsUnderMouse.at(i);
        if (itemAcceptsHoverEvents_helper(tmp)) {
            item = tmp;
            break;
        }
    }

    // Find the common ancestor item for the new topmost hoverItem and the
    // last item in the hoverItem list.
    QGraphicsItem *commonAncestorItem = (item && !hoverItems.isEmpty()) ? item->commonAncestorItem(hoverItems.last()) : 0;
    while (commonAncestorItem && !itemAcceptsHoverEvents_helper(commonAncestorItem))
        commonAncestorItem = commonAncestorItem->parentItem();
    if (commonAncestorItem && commonAncestorItem->window() != item->window()) {
        // The common ancestor isn't in the same window as the two hovered
        // items.
        commonAncestorItem = 0;
    }

    // Check if the common ancestor item is known.
    int index = commonAncestorItem ? hoverItems.indexOf(commonAncestorItem) : -1;
    // Send hover leaves to any existing hovered children of the common
    // ancestor item.
    for (int i = hoverItems.size() - 1; i > index; --i) {
        QGraphicsItem *lastItem = hoverItems.takeLast();
        if (itemAcceptsHoverEvents_helper(lastItem))
            sendHoverEvent(QEvent::GraphicsSceneHoverLeave, lastItem, hoverEvent);
    }

    // Item is a child of a known item. Generate enter events for the
    // missing links.
    QList<QGraphicsItem *> parents;
    QGraphicsItem *parent = item;
    while (parent && parent != commonAncestorItem) {
        parents.prepend(parent);
        if (parent->isWindow()) {
            // Stop at the window - we don't deliver beyond this point.
            break;
        }
        parent = parent->parentItem();
    }
    for (int i = 0; i < parents.size(); ++i) {
        parent = parents.at(i);
        hoverItems << parent;
        if (itemAcceptsHoverEvents_helper(parent))
            sendHoverEvent(QEvent::GraphicsSceneHoverEnter, parent, hoverEvent);
    }

    // Generate a move event for the item itself
    if (item && !hoverItems.isEmpty() && item == hoverItems.last()) {
        sendHoverEvent(QEvent::GraphicsSceneHoverMove, item, hoverEvent);
        return true;
    }
    return false;
}

/*!
    \internal

    Handles all actions necessary to clean up the scene when the mouse leaves
    the view.
*/
void QGraphicsScenePrivate::leaveScene()
{
    Q_Q(QGraphicsScene);
#ifndef QT_NO_TOOLTIP
    // Remove any tooltips
    QToolTip::showText(QPoint(), QString());
#endif
    // Send HoverLeave events to all existing hover items, topmost first.
    QGraphicsView *senderWidget = qobject_cast<QGraphicsView *>(q->sender());
    QGraphicsSceneHoverEvent hoverEvent;
    hoverEvent.setWidget(senderWidget);

    if (senderWidget) {
        QPoint cursorPos = QCursor::pos();
        hoverEvent.setScenePos(senderWidget->mapToScene(senderWidget->mapFromGlobal(cursorPos)));
        hoverEvent.setLastScenePos(hoverEvent.scenePos());
        hoverEvent.setScreenPos(cursorPos);
        hoverEvent.setLastScreenPos(hoverEvent.screenPos());
    }

    while (!hoverItems.isEmpty()) {
        QGraphicsItem *lastItem = hoverItems.takeLast();
        if (lastItem->acceptHoverEvents()
            || (lastItem->isWidget() && static_cast<QGraphicsWidget*>(lastItem)->d_func()->hasDecoration()))
            sendHoverEvent(QEvent::GraphicsSceneHoverLeave, lastItem, &hoverEvent);
    }
}

/*!
    This event handler, for event \a keyEvent, can be reimplemented in a
    subclass to receive keypress events. The default implementation forwards
    the event to current focus item.

    \sa QGraphicsItem::keyPressEvent(), focusItem()
*/
void QGraphicsScene::keyPressEvent(QKeyEvent *keyEvent)
{
    // ### Merge this function with keyReleaseEvent; they are identical
    // ### (except this comment).
    Q_D(QGraphicsScene);
    QGraphicsItem *item = !d->keyboardGrabberItems.isEmpty() ? d->keyboardGrabberItems.last() : 0;
    if (!item)
        item = focusItem();
    if (item) {
        QGraphicsItem *p = item;
        do {
            // Accept the event by default
            keyEvent->accept();
            // Send it; QGraphicsItem::keyPressEvent ignores it.  If the event
            // is filtered out, stop propagating it.
            if (!d->sendEvent(p, keyEvent))
                break;
        } while (!keyEvent->isAccepted() && !p->isWindow() && (p = p->parentItem()));
    } else {
        keyEvent->ignore();
    }
}

/*!
    This event handler, for event \a keyEvent, can be reimplemented in a
    subclass to receive key release events. The default implementation
    forwards the event to current focus item.

    \sa QGraphicsItem::keyReleaseEvent(), focusItem()
*/
void QGraphicsScene::keyReleaseEvent(QKeyEvent *keyEvent)
{
    // ### Merge this function with keyPressEvent; they are identical (except
    // ### this comment).
    Q_D(QGraphicsScene);
    QGraphicsItem *item = !d->keyboardGrabberItems.isEmpty() ? d->keyboardGrabberItems.last() : 0;
    if (!item)
        item = focusItem();
    if (item) {
        QGraphicsItem *p = item;
        do {
            // Accept the event by default
            keyEvent->accept();
            // Send it; QGraphicsItem::keyPressEvent ignores it.  If the event
            // is filtered out, stop propagating it.
            if (!d->sendEvent(p, keyEvent))
                break;
        } while (!keyEvent->isAccepted() && !p->isWindow() && (p = p->parentItem()));
    } else {
        keyEvent->ignore();
    }
}

/*!
    This event handler, for event \a mouseEvent, can be reimplemented
    in a subclass to receive mouse press events for the scene.

    The default implementation depends on the state of the scene. If
    there is a mouse grabber item, then the event is sent to the mouse
    grabber. Otherwise, it is forwarded to the topmost item that
    accepts mouse events at the scene position from the event, and
    that item promptly becomes the mouse grabber item.

    If there is no item at the given position on the scene, the
    selection area is reset, any focus item loses its input focus, and
    the event is then ignored.

    \sa QGraphicsItem::mousePressEvent(),
    QGraphicsItem::setAcceptedMouseButtons()
*/
void QGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    Q_D(QGraphicsScene);
    d->mousePressEventHandler(mouseEvent);
}

/*!
    This event handler, for event \a mouseEvent, can be reimplemented
    in a subclass to receive mouse move events for the scene.

    The default implementation depends on the mouse grabber state. If there is
    a mouse grabber item, the event is sent to the mouse grabber.  If there
    are any items that accept hover events at the current position, the event
    is translated into a hover event and accepted; otherwise it's ignored.

    \sa QGraphicsItem::mousePressEvent(), QGraphicsItem::mouseReleaseEvent(),
    QGraphicsItem::mouseDoubleClickEvent(), QGraphicsItem::setAcceptedMouseButtons()
*/
void QGraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    Q_D(QGraphicsScene);
    if (d->mouseGrabberItems.isEmpty()) {
        if (mouseEvent->buttons())
            return;
        QGraphicsSceneHoverEvent hover;
        _q_hoverFromMouseEvent(&hover, mouseEvent);
        mouseEvent->setAccepted(d->dispatchHoverEvent(&hover));
        return;
    }

    // Forward the event to the mouse grabber
    d->sendMouseEvent(mouseEvent);
    mouseEvent->accept();
}

/*!
    This event handler, for event \a mouseEvent, can be reimplemented
    in a subclass to receive mouse release events for the scene.

    The default implementation depends on the mouse grabber state.  If
    there is no mouse grabber, the event is ignored.  Otherwise, if
    there is a mouse grabber item, the event is sent to the mouse
    grabber. If this mouse release represents the last pressed button
    on the mouse, the mouse grabber item then loses the mouse grab.

    \sa QGraphicsItem::mousePressEvent(), QGraphicsItem::mouseMoveEvent(),
    QGraphicsItem::mouseDoubleClickEvent(), QGraphicsItem::setAcceptedMouseButtons()
*/
void QGraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    Q_D(QGraphicsScene);
    if (d->mouseGrabberItems.isEmpty()) {
        mouseEvent->ignore();
        return;
    }

    // Forward the event to the mouse grabber
    d->sendMouseEvent(mouseEvent);
    mouseEvent->accept();

    // Reset the mouse grabber when the last mouse button has been released.
    if (!mouseEvent->buttons()) {
        if (!d->mouseGrabberItems.isEmpty()) {
            d->lastMouseGrabberItem = d->mouseGrabberItems.last();
            if (d->lastMouseGrabberItemHasImplicitMouseGrab)
                d->mouseGrabberItems.last()->ungrabMouse();
        } else {
            d->lastMouseGrabberItem = 0;
        }

        // Generate a hoverevent
        QGraphicsSceneHoverEvent hoverEvent;
        _q_hoverFromMouseEvent(&hoverEvent, mouseEvent);
        d->dispatchHoverEvent(&hoverEvent);
    }
}

/*!
    This event handler, for event \a mouseEvent, can be reimplemented
    in a subclass to receive mouse doubleclick events for the scene.

    If someone doubleclicks on the scene, the scene will first receive
    a mouse press event, followed by a release event (i.e., a click),
    then a doubleclick event, and finally a release event. If the
    doubleclick event is delivered to a different item than the one
    that received the first press and release, it will be delivered as
    a press event. However, tripleclick events are not delivered as
    doubleclick events in this case.

    The default implementation is similar to mousePressEvent().

    \sa QGraphicsItem::mousePressEvent(), QGraphicsItem::mouseMoveEvent(),
    QGraphicsItem::mouseReleaseEvent(), QGraphicsItem::setAcceptedMouseButtons()
*/
void QGraphicsScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    Q_D(QGraphicsScene);
    d->mousePressEventHandler(mouseEvent);
}

/*!
    This event handler, for event \a wheelEvent, can be reimplemented in a
    subclass to receive mouse wheel events for the scene.

    By default, the event is delivered to the topmost visible item under the
    cursor. If ignored, the event propagates to the item beneath, and again
    until the event is accepted, or it reaches the scene. If no items accept
    the event, it is ignored.

    \sa QGraphicsItem::wheelEvent()
*/
void QGraphicsScene::wheelEvent(QGraphicsSceneWheelEvent *wheelEvent)
{
    Q_D(QGraphicsScene);
    QList<QGraphicsItem *> wheelCandidates = d->itemsAtPosition(wheelEvent->screenPos(),
                                                                wheelEvent->scenePos(),
                                                                wheelEvent->widget());

    bool hasSetFocus = false;
    foreach (QGraphicsItem *item, wheelCandidates) {
        if (!hasSetFocus && item->isEnabled() && (item->flags() & QGraphicsItem::ItemIsFocusable)) {
            if (item->isWidget() && static_cast<QGraphicsWidget *>(item)->focusPolicy() == Qt::WheelFocus) {
                hasSetFocus = true;
                if (item != focusItem())
                    setFocusItem(item, Qt::MouseFocusReason);
            }
        }

        wheelEvent->setPos(item->d_ptr->genericMapFromScene(wheelEvent->scenePos(),
                                                            wheelEvent->widget()));
        wheelEvent->accept();
        bool isWindow = item->isWindow();
        d->sendEvent(item, wheelEvent);
        if (isWindow || wheelEvent->isAccepted())
            break;
    }
}

/*!
    This event handler, for event \a event, can be reimplemented in a
    subclass to receive input method events for the scene.

    The default implementation forwards the event to the focusItem().
    If no item currently has focus, this function does nothing.

    \sa QGraphicsItem::inputMethodEvent()
*/
void QGraphicsScene::inputMethodEvent(QInputMethodEvent *event)
{
    Q_D(QGraphicsScene);
    if (!d->focusItem)
        return;
    d->sendEvent(d->focusItem, event);
}

/*!
    Draws the background of the scene using \a painter, before any items and
    the foreground are drawn. Reimplement this function to provide a custom
    background for the scene.

    All painting is done in \e scene coordinates. The \a rect
    parameter is the exposed rectangle.

    If all you want is to define a color, texture, or gradient for the
    background, you can call setBackgroundBrush() instead.

    \sa drawForeground(), drawItems()
*/
void QGraphicsScene::drawBackground(QPainter *painter, const QRectF &rect)
{
    Q_D(QGraphicsScene);

    if (d->backgroundBrush.style() != Qt::NoBrush) {
        if (d->painterStateProtection)
            painter->save();
        painter->setBrushOrigin(0, 0);
        painter->fillRect(rect, backgroundBrush());
        if (d->painterStateProtection)
            painter->restore();
    }
}

/*!
    Draws the foreground of the scene using \a painter, after the background
    and all items have been drawn. Reimplement this function to provide a
    custom foreground for the scene.

    All painting is done in \e scene coordinates. The \a rect
    parameter is the exposed rectangle.

    If all you want is to define a color, texture or gradient for the
    foreground, you can call setForegroundBrush() instead.

    \sa drawBackground(), drawItems()
*/
void QGraphicsScene::drawForeground(QPainter *painter, const QRectF &rect)
{
    Q_D(QGraphicsScene);

    if (d->foregroundBrush.style() != Qt::NoBrush) {
        if (d->painterStateProtection)
            painter->save();
        painter->setBrushOrigin(0, 0);
        painter->fillRect(rect, foregroundBrush());
        if (d->painterStateProtection)
            painter->restore();
    }
}

static void _q_paintItem(QGraphicsItem *item, QPainter *painter,
                         const QStyleOptionGraphicsItem *option, QWidget *widget,
                         bool useWindowOpacity, bool painterStateProtection)
{
    if (!item->isWidget()) {
        item->paint(painter, option, widget);
        return;
    }
    QGraphicsWidget *widgetItem = static_cast<QGraphicsWidget *>(item);
    QGraphicsProxyWidget *proxy = qobject_cast<QGraphicsProxyWidget *>(widgetItem);
    const qreal windowOpacity = (proxy && proxy->widget() && useWindowOpacity)
                                ? proxy->widget()->windowOpacity() : 1.0;
    const qreal oldPainterOpacity = painter->opacity();

    if (qFuzzyCompare(windowOpacity + 1, qreal(1.0)))
        return;
    // Set new painter opacity.
    if (windowOpacity < 1.0)
        painter->setOpacity(oldPainterOpacity * windowOpacity);

    // set layoutdirection on the painter
    Qt::LayoutDirection oldLayoutDirection = painter->layoutDirection();
    painter->setLayoutDirection(widgetItem->layoutDirection());

    if (widgetItem->isWindow() && widgetItem->windowType() != Qt::Popup && widgetItem->windowType() != Qt::ToolTip
        && !(widgetItem->windowFlags() & Qt::FramelessWindowHint)) {
        if (painterStateProtection)
            painter->save();
        widgetItem->paintWindowFrame(painter, option, widget);
        if (painterStateProtection)
            painter->restore();
    }

    widgetItem->paint(painter, option, widget);

    // Restore layoutdirection on the painter.
    painter->setLayoutDirection(oldLayoutDirection);
    // Restore painter opacity.
    if (windowOpacity < 1.0)
        painter->setOpacity(oldPainterOpacity);
}

static void _q_paintIntoCache(QPixmap *pix, QGraphicsItem *item, const QRegion &pixmapExposed,
                              const QTransform &itemToPixmap, QPainter::RenderHints renderHints,
                              const QStyleOptionGraphicsItem *option, bool painterStateProtection)
{
    QPixmap subPix;
    QPainter pixmapPainter;
    QRect br = pixmapExposed.boundingRect();

    // Don't use subpixmap if we get a full update.
    if (pixmapExposed.isEmpty() || (pixmapExposed.numRects() == 1 && br.contains(pix->rect()))) {
        pix->fill(Qt::transparent);
        pixmapPainter.begin(pix);
    } else {
        subPix = QPixmap(br.size());
        subPix.fill(Qt::transparent);
        pixmapPainter.begin(&subPix);
        pixmapPainter.translate(-br.topLeft());
        if (!pixmapExposed.isEmpty()) {
            // Applied to subPix; paint is adjusted to the coordinate space is
            // correct.
            pixmapPainter.setClipRegion(pixmapExposed);
        }
    }

    pixmapPainter.setRenderHints(pixmapPainter.renderHints(), false);
    pixmapPainter.setRenderHints(renderHints, true);
    pixmapPainter.setWorldTransform(itemToPixmap, true);

    // Render.
    _q_paintItem(item, &pixmapPainter, option, 0, false, painterStateProtection);
    pixmapPainter.end();

    if (!subPix.isNull()) {
        // Blit the subpixmap into the main pixmap.
        pixmapPainter.begin(pix);
        pixmapPainter.setCompositionMode(QPainter::CompositionMode_Source);
        pixmapPainter.setClipRegion(pixmapExposed);
        pixmapPainter.drawPixmap(br.topLeft(), subPix);
        pixmapPainter.end();
    }
}

/*!
    \internal

    Draws items directly, or using cache.
*/
void QGraphicsScenePrivate::drawItemHelper(QGraphicsItem *item, QPainter *painter,
                                           const QStyleOptionGraphicsItem *option, QWidget *widget,
                                           bool painterStateProtection)
{
    QGraphicsItemPrivate *itemd = item->d_ptr;
    QGraphicsItem::CacheMode cacheMode = QGraphicsItem::CacheMode(itemd->cacheMode);

    // Render directly, using no cache.
    if (cacheMode == QGraphicsItem::NoCache
#ifdef Q_WS_X11
        || !X11->use_xrender
#endif
        ) {
        _q_paintItem(static_cast<QGraphicsWidget *>(item), painter, option, widget, true, painterStateProtection);
        return;
    }

    const qreal oldPainterOpacity = painter->opacity();
    qreal newPainterOpacity = oldPainterOpacity;
    QGraphicsProxyWidget *proxy = item->isWidget() ? qobject_cast<QGraphicsProxyWidget *>(static_cast<QGraphicsWidget *>(item)) : 0;
    if (proxy && proxy->widget()) {
        const qreal windowOpacity = proxy->widget()->windowOpacity();
        if (windowOpacity < 1.0)
            newPainterOpacity *= windowOpacity;
    }

    // Item's (local) bounding rect
    QRectF brect = item->boundingRect();
    if (_q_adjustedRect(brect).isEmpty())
        return;

    // Fetch the off-screen transparent buffer and exposed area info.
    QString pixmapKey;
    QPixmap pix;
    QGraphicsItemCache *itemCache = itemd->extraItemCache();
    if (cacheMode == QGraphicsItem::ItemCoordinateCache) {
        if (itemCache->boundingRect != brect.toRect()) {
            itemCache->boundingRect = brect.toRect();
            itemCache->allExposed = true;
            itemCache->exposed.clear();
        }
        pixmapKey = itemCache->key;
    } else {
        if ((pixmapKey = itemCache->deviceData.value(widget).key).isEmpty()) {
            pixmapKey.sprintf("qgv-%p-%p", item, widget);
            QGraphicsItemCache::DeviceData data;
            data.key = pixmapKey;
            itemCache->deviceData.insert(widget, data);
        }
    }

    // Find pixmap in cache.
    if (!itemCache->allExposed)
        QPixmapCache::find(pixmapKey, pix);

    // Render using item coordinate cache mode.
    if (cacheMode == QGraphicsItem::ItemCoordinateCache) {
        QSize pixmapSize;
        bool fixedCacheSize = false;
        if ((fixedCacheSize = itemCache->fixedSize.isValid())) {
            pixmapSize = itemCache->fixedSize;
        } else {
            pixmapSize = brect.toAlignedRect().size();
        }

        // Create or recreate the pixmap.
        int adjust = itemCache->fixedSize.isValid() ? 0 : 2;
        QSize adjustSize(adjust*2, adjust*2);
        QRectF br = brect.adjusted(-adjust, -adjust, adjust, adjust);
        if (pix.isNull() || (!fixedCacheSize && (pixmapSize + adjustSize) != pix.size())) {
            pix = QPixmap(pixmapSize + adjustSize);
            itemCache->exposed.clear();
            itemCache->allExposed = true;
        }

        // Redraw any newly exposed areas.
        if (itemCache->allExposed || !itemCache->exposed.isEmpty()) {
            // Fit the item's bounding rect into the pixmap's coordinates.
            const QPointF scale(pixmapSize.width() / brect.width(), pixmapSize.height() / brect.height());
            QTransform itemToPixmap;
            itemToPixmap.scale(scale.x(), scale.y());
            itemToPixmap.translate(-br.x(), -br.y());

            // Generate the item's exposedRect and map its list of expose
            // rects to device coordinates.
            QStyleOptionGraphicsItem cacheOption = *option;
            QRegion pixmapExposed;
            QRectF exposedRect;
            if (!itemCache->allExposed) {
                for (int i = 0; i < itemCache->exposed.size(); ++i) {
                    QRectF r = itemCache->exposed.at(i);
                    exposedRect |= r;
                    pixmapExposed += itemToPixmap.mapRect(r).toAlignedRect();
                }
            } else {
                exposedRect = brect;
            }
            cacheOption.exposedRect = exposedRect;

            // Render.
            _q_paintIntoCache(&pix, item, pixmapExposed, itemToPixmap, painter->renderHints(),
                              &cacheOption, painterStateProtection);

            // Reinsert this pixmap into the cache.
            QPixmapCache::insert(pixmapKey, pix);

            // Reset expose data.
            itemCache->allExposed = false;
            itemCache->exposed.clear();
        }

        // Redraw the exposed area using the transformed painter. Depending on
        // the hardware, this may be a server-side operation, or an expensive
        // qpixmap-image-transform-pixmap roundtrip.
        if (newPainterOpacity != oldPainterOpacity) {
            painter->setOpacity(newPainterOpacity);
            painter->drawPixmap(br, pix, QRectF(QPointF(), pix.size()));
            painter->setOpacity(oldPainterOpacity);
        } else {
            painter->drawPixmap(br, pix, QRectF(QPointF(), pix.size()));
        }
        return;
    }

    // Render using device coordinate cache mode.
    if (cacheMode == QGraphicsItem::DeviceCoordinateCache) {
        // Find the item's bounds in device coordinates.
        QRectF deviceBounds = painter->worldTransform().mapRect(brect);
        QRect deviceRect = deviceBounds.toRect().adjusted(-1, -1, 1, 1);
        if (deviceRect.isEmpty())
            return;
        QRect viewRect = widget ? widget->rect() : QRect();
        if (widget && !viewRect.intersects(deviceRect))
            return;

        // Resort to direct rendering if the device rect exceeds the
        // (optional) maximum bounds. (QGraphicsSvgItem uses this).
        QSize maximumCacheSize =
            itemd->extra(QGraphicsItemPrivate::ExtraMaxDeviceCoordCacheSize).toSize();
        if (!maximumCacheSize.isEmpty()
            && (deviceRect.width() > maximumCacheSize.width()
                || deviceRect.height() > maximumCacheSize.height())) {
            _q_paintItem(static_cast<QGraphicsWidget *>(item), painter, option, widget,
                         oldPainterOpacity != newPainterOpacity, painterStateProtection);
            return;
        }

        // Create or reuse offscreen pixmap, possibly scroll/blit from the old one.
        bool pixModified = false;
        QGraphicsItemCache::DeviceData *deviceData = &itemCache->deviceData[widget];
        bool invertable = true;
        QTransform diff = deviceData->lastTransform.inverted(&invertable);
        if (invertable)
            diff *= painter->worldTransform();
        deviceData->lastTransform = painter->worldTransform();
        if (!invertable || diff.type() > QTransform::TxTranslate) {
            pixModified = true;
            itemCache->allExposed = true;
            itemCache->exposed.clear();
            pix = QPixmap();
        }

        // ### This is a pretty bad way to determine when to start partial
        // exposure for DeviceCoordinateCache but it's the least intrusive
        // approach for now.
#if 0
        // Only if the device rect isn't fully contained.
        bool allowPartialCacheExposure = !viewRect.contains(deviceRect);
#else
        // Only if deviceRect is 20% taller or wider than the desktop.
        QRect desktopRect = qApp->desktop()->availableGeometry(widget);
        bool allowPartialCacheExposure = (desktopRect.width() * 1.2 < deviceRect.width()
                                          || desktopRect.height() * 1.2 < deviceRect.height());
#endif
        QRegion scrollExposure;
        if (deviceData->cacheIndent != QPoint() || allowPartialCacheExposure) {
            // Part of pixmap is drawn. Either device contains viewrect (big
            // item covers whole screen) or parts of device are outside the
            // viewport. In either case the device rect must be the intersect
            // between the two.
            int dx = deviceRect.left() < viewRect.left() ? viewRect.left() - deviceRect.left() : 0;
            int dy = deviceRect.top() < viewRect.top() ? viewRect.top() - deviceRect.top() : 0;
            QPoint newCacheIndent(dx, dy);
            deviceRect &= viewRect;

            if (pix.isNull()) {
                deviceData->cacheIndent = QPoint();
                itemCache->allExposed = true;
                itemCache->exposed.clear();
                pixModified = true;
            }

            // Copy / "scroll" the old pixmap onto the new ole and calculate
            // scrolled exposure.
            if (newCacheIndent != deviceData->cacheIndent || deviceRect.size() != pix.size()) {
                QPoint diff = newCacheIndent - deviceData->cacheIndent;
                QPixmap newPix(deviceRect.size());
                // ### Investigate removing this fill (test with Plasma and
                // graphicssystem raster).
                newPix.fill(Qt::transparent);
                if (!pix.isNull()) {
                    QPainter newPixPainter(&newPix);
                    newPixPainter.drawPixmap(-diff, pix);
                    newPixPainter.end();
                }
                QRegion exposed;
                exposed += newPix.rect();
                if (!pix.isNull())
                    exposed -= QRect(-diff, pix.size());
                scrollExposure = exposed;

                pix = newPix;
                pixModified = true;
            }
            deviceData->cacheIndent = newCacheIndent;
        } else {
            // Full pixmap is drawn.
            deviceData->cacheIndent = QPoint();

            // Auto-adjust the pixmap size.
            if (deviceRect.size() != pix.size()) {
                // exposed needs to cover the whole pixmap
                pix = QPixmap(deviceRect.size());
                pixModified = true;
                itemCache->allExposed = true;
                itemCache->exposed.clear();
            }
        }

        // Check for newly invalidated areas.
        if (itemCache->allExposed || !itemCache->exposed.isEmpty() || !scrollExposure.isEmpty()) {
            // Construct an item-to-pixmap transform.
            QPointF p = deviceRect.topLeft();
            QTransform itemToPixmap = QTransform::fromTranslate(-p.x(), -p.y());
            itemToPixmap = painter->worldTransform() * itemToPixmap;

            // Map the item's logical expose to pixmap coordinates.
            QRegion pixmapExposed = scrollExposure;
            if (!itemCache->allExposed) {
                const QVector<QRectF> &exposed = itemCache->exposed;
                for (int i = 0; i < exposed.size(); ++i)
                    pixmapExposed += itemToPixmap.mapRect(exposed.at(i)).toRect().adjusted(-1, -1, 1, 1);
            }

            // Calculate the style option's exposedRect.
            QRectF br;
            if (itemCache->allExposed) {
                br = item->boundingRect();
            } else {
                const QVector<QRectF> &exposed = itemCache->exposed;
                for (int i = 0; i < exposed.size(); ++i)
                    br |= exposed.at(i);
                QTransform pixmapToItem = itemToPixmap.inverted();
                foreach (QRect r, scrollExposure.rects())
                    br |= pixmapToItem.mapRect(r);
            }
            QStyleOptionGraphicsItem cacheOption = *option;
            cacheOption.exposedRect = br.adjusted(-1, -1, 1, 1);

            // Render the exposed areas.
            _q_paintIntoCache(&pix, item, pixmapExposed, itemToPixmap, painter->renderHints(),
                              &cacheOption, painterStateProtection);

            // Reset expose data.
            pixModified = true;
            itemCache->allExposed = false;
            itemCache->exposed.clear();
        }

        if (pixModified) {
            // Reinsert this pixmap into the cache
            QPixmapCache::insert(pixmapKey, pix);
        }

        // Redraw the exposed area using an untransformed painter. This
        // effectively becomes a bitblit that does not transform the cache.
        QTransform restoreTransform = painter->worldTransform();
        painter->setWorldTransform(QTransform());
        if (newPainterOpacity != oldPainterOpacity) {
            painter->setOpacity(newPainterOpacity);
            painter->drawPixmap(deviceRect.topLeft(), pix);
            painter->setOpacity(oldPainterOpacity);
        } else {
            painter->drawPixmap(deviceRect.topLeft(), pix);
        }
        painter->setWorldTransform(restoreTransform);
        return;
    }
}

/*!
    Paints the given \a items using the provided \a painter, after the
    background has been drawn, and before the foreground has been
    drawn.  All painting is done in \e scene coordinates. Before
    drawing each item, the painter must be transformed using
    QGraphicsItem::sceneMatrix().

    The \a options parameter is the list of style option objects for
    each item in \a items. The \a numItems parameter is the number of
    items in \a items and options in \a options. The \a widget
    parameter is optional; if specified, it should point to the widget
    that is being painted on.

    The default implementation prepares the painter matrix, and calls
    QGraphicsItem::paint() on all items. Reimplement this function to
    provide custom painting of all items for the scene; gaining
    complete control over how each item is drawn. In some cases this
    can increase drawing performance significantly.

    Example:

    \snippet doc/src/snippets/graphicssceneadditemsnippet.cpp 0

    \sa drawBackground(), drawForeground()
*/
void QGraphicsScene::drawItems(QPainter *painter,
                               int numItems,
                               QGraphicsItem *items[],
                               const QStyleOptionGraphicsItem options[], QWidget *widget)
{
    Q_D(QGraphicsScene);

    // Detect if painter state protection is disabled.
    QTransform viewTransform = painter->worldTransform();
    QVarLengthArray<QGraphicsItem *, 16> childClippers;

    for (int i = 0; i < numItems; ++i) {
        QGraphicsItem *item = items[i];
        if (!(item->d_ptr->ancestorFlags & QGraphicsItemPrivate::AncestorClipsChildren)) {
            if (!childClippers.isEmpty()) {
                // Item is not clipped to any ancestor: pop all current clippers.
                for (int i = 0; i < childClippers.size(); ++i)
                    painter->restore();
                childClippers.clear();
            }
        } else {
            // Item is clipped to an ancestor, which may or may not be in our
            // child clipper list. Let's start by finding the item's closest
            // clipping ancestor.
            QGraphicsItem *clipParent = item->parentItem();
            while (clipParent && !(clipParent->d_ptr->flags & QGraphicsItem::ItemClipsChildrenToShape))
                clipParent = clipParent->parentItem();

            // Pop any in-between clippers. If the clipper is unknown, pop
            // them all.  ### QVarLengthArray::lastIndexOf().
            int index = -1;
            for (int n = childClippers.size() - 1; n >= 0; --n) {
                if (childClippers[n] == clipParent) {
                    index = n;
                    break;
                }
            }
            if (index != -1) {
                int toPop = childClippers.size() - index - 1;
                if (toPop > 0) {
                    for (int i = 0; i < toPop; ++i)
                        painter->restore();
                    childClippers.resize(index + 1);
                }
            }

            // Sanity check
            if (!childClippers.isEmpty())
                Q_ASSERT(childClippers[childClippers.size() - 1] == clipParent);

            // If the clipper list is empty at this point, but we're still
            // clipped to an ancestor, then we need to build the clip chain
            // ourselves. There is only one case that can produce this issue:
            // This item is stacked behind an ancestor:
            // ItemStacksBehindParent.
            if (childClippers.isEmpty()) {
                Q_ASSERT(clipParent != 0);
                // Build a stack of clippers.
                QVarLengthArray<QGraphicsItem *, 16> clippers;
                QGraphicsItem *p = clipParent;
                do {
                    if (p->d_ptr->flags & QGraphicsItem::ItemClipsChildrenToShape)
                        clippers.append(p);
                } while ((p = p->parentItem()) && (p->d_ptr->ancestorFlags & QGraphicsItemPrivate::AncestorClipsChildren));

                // ### This code path can also use the itemTransform
                // optimization, but it's hit very rarely.
                for (int i = clippers.size() - 1; i >= 0; --i) {
                    QGraphicsItem *clipper = clippers[i];
                    if (clipper->d_ptr->itemIsUntransformable()) {
                        painter->setWorldTransform(clipper->deviceTransform(viewTransform), false);
                    } else {
                        painter->setWorldTransform(clipper->sceneTransform() * viewTransform, false);
                    }

                    childClippers.append(clipper);
                    painter->save();
                    painter->setClipPath(clipper->shape(), Qt::IntersectClip);
                }
                Q_ASSERT(childClippers[childClippers.size() - 1] == clipParent);
            }
        }

        // Set up the painter transform
        if (item->d_ptr->itemIsUntransformable()) {
            painter->setWorldTransform(item->deviceTransform(viewTransform), false);
        } else {
            painter->setWorldTransform(item->sceneTransform() * viewTransform, false);
        }
       
        // Save painter
        bool saveState = (d->painterStateProtection || (item->flags() & QGraphicsItem::ItemClipsToShape));
        if (saveState)
            painter->save();

        // Set local clip
        if (item->flags() & QGraphicsItem::ItemClipsToShape)
            painter->setClipPath(item->shape(), Qt::IntersectClip);

        // Setup opacity
        painter->setOpacity(item->effectiveOpacity());

        // Draw the item
        d->drawItemHelper(item, painter, &options[i], widget, d->painterStateProtection);

        if (saveState)
            painter->restore();

        if (item->flags() & QGraphicsItem::ItemClipsChildrenToShape) {
            // Clip descendents to this item's shape, and keep the painter
            // saved.
            childClippers.append(item);
            painter->save();
            painter->setClipPath(item->shape(), Qt::IntersectClip);
        }
    }

    for (int i = 0; i < childClippers.size(); ++i)
        painter->restore();

    painter->setWorldTransform(viewTransform);
}

/*!
    \since 4.4

    Finds a new widget to give the keyboard focus to, as appropriate for Tab
    and Shift+Tab, and returns true if it can find a new widget, or false if
    it cannot. If \a next is true, this function searches forward; if \a next
    is false, it searches backward.

    You can reimplement this function in a subclass of QGraphicsScene to
    provide fine-grained control over how tab focus passes inside your
    scene. The default implementation is based on the tab focus chain defined
    by QGraphicsWidget::setTabOrder().
*/
bool QGraphicsScene::focusNextPrevChild(bool next)
{
    Q_D(QGraphicsScene);

    QGraphicsItem *item = focusItem();
    if (item && !item->isWidget()) {
        // Tab out of the scene.
        return false;
    }
    if (!item) {
        if (d->lastFocusItem && !d->lastFocusItem->isWidget()) {
            // Restore focus to the last focusable non-widget item that had
            // focus.
            setFocusItem(d->lastFocusItem, next ? Qt::TabFocusReason : Qt::BacktabFocusReason);
            return true;
        }
    }
    if (!d->tabFocusFirst) {
        // No widgets...
        return false;
    }

    // The item must be a widget.
    QGraphicsWidget *widget = 0;
    if (!item) {
        widget = next ? d->tabFocusFirst : d->tabFocusFirst->d_func()->focusPrev;
    } else {
        QGraphicsWidget *test = static_cast<QGraphicsWidget *>(item);
        widget = next ? test->d_func()->focusNext : test->d_func()->focusPrev;
        if ((next && widget == d->tabFocusFirst) || (!next && widget == d->tabFocusFirst->d_func()->focusPrev))
            return false;
    }
    QGraphicsWidget *widgetThatHadFocus = widget;

    // Run around the focus chain until we find a widget that can take tab focus.
    do {
        if (widget->flags() & QGraphicsItem::ItemIsFocusable
            && widget->isEnabled() && widget->isVisibleTo(0)
            && (widget->focusPolicy() & Qt::TabFocus)
            && (!item || !item->isWindow() || item->isAncestorOf(widget))
            ) {
            setFocusItem(widget, next ? Qt::TabFocusReason : Qt::BacktabFocusReason);
            return true;
        }
        widget = next ? widget->d_func()->focusNext : widget->d_func()->focusPrev;
        if ((next && widget == d->tabFocusFirst) || (!next && widget == d->tabFocusFirst->d_func()->focusPrev))
            return false;
    } while (widget != widgetThatHadFocus);

    return false;
}

/*!
    \fn QGraphicsScene::changed(const QList<QRectF> &region)

    This signal is emitted by QGraphicsScene when control reaches the
    event loop, if the scene content changes. The \a region parameter
    contains a list of scene rectangles that indicate the area that
    has been changed.

    \sa QGraphicsView::updateScene()
*/

/*!
    \fn QGraphicsScene::sceneRectChanged(const QRectF &rect)

    This signal is emitted by QGraphicsScene whenever the scene rect changes.
    The \a rect parameter is the new scene rectangle.

    \sa QGraphicsView::updateSceneRect()
*/

/*!
    \fn QGraphicsScene::selectionChanged()
    \since 4.3

    This signal is emitted by QGraphicsScene whenever the selection
    changes. You can call selectedItems() to get the new list of selected
    items.

    The selection changes whenever an item is selected or unselected, a
    selection area is set, cleared or otherwise changed, if a preselected item
    is added to the scene, or if a selected item is removed from the scene.

    QGraphicsScene emits this signal only once for group selection operations.
    For example, if you set a selection area, select or unselect a
    QGraphicsItemGroup, or if you add or remove from the scene a parent item
    that contains several selected items, selectionChanged() is emitted only
    once after the operation has completed (instead of once for each item).

    \sa setSelectionArea(), selectedItems(), QGraphicsItem::setSelected()
*/

/*!
    \internal

    This private function is called by QGraphicsItem, which is a friend of
    QGraphicsScene. It is used by QGraphicsScene to record the rectangles that
    need updating. It also launches a single-shot timer to ensure that
    updated() will be emitted later.

    The \a item parameter is the item that changed, and \a rect is the
    area of the item that changed given in item coordinates.
*/
void QGraphicsScene::itemUpdated(QGraphicsItem *item, const QRectF &rect)
{
    Q_D(QGraphicsScene);
    // Deliver the actual update.
    if (!d->updateAll) {
        if (d->views.isEmpty() || ((d->connectedSignals & d->changedSignalMask) && !item->d_ptr->itemIsUntransformable()
                                   && qFuzzyCompare(item->boundingRegionGranularity(), qreal(0.0)))) {
            // This block of code is kept for compatibility. Since 4.5, by default
            // QGraphicsView does not connect the signal and we use the below
            // method of delivering updates.
            update(item->sceneBoundingRect());
        } else {
            // ### Remove _q_adjustedRects().
            QRectF boundingRect = _q_adjustedRect(item->boundingRect());
            if (!rect.isNull())
                boundingRect &= _q_adjustedRect(rect);

            // Update each view directly.
            for (int i = 0; i < d->views.size(); ++i)
                d->views.at(i)->d_func()->itemUpdated(item, boundingRect);
        }
    }
    if (item->d_ptr->dirty) {
        d->dirtyItems << item;
        d->resetDirtyItemsLater();
    }

    // Update d->largestUntransformableItem by mapping this item's bounding
    // rect back to the topmost untransformable item's untransformed
    // coordinate system (which sort of equals the 1:1 coordinate system of an
    // untransformed view).
    if (item->d_ptr->itemIsUntransformable()) {
        QGraphicsItem *parent = item;
        while (parent && (parent->d_ptr->ancestorFlags & QGraphicsItemPrivate::AncestorIgnoresTransformations))
            parent = parent->parentItem();
        d->largestUntransformableItem |= item->mapToItem(parent, item->boundingRect()).boundingRect();
    }

    // Only track the automatically growing scene rect if the scene has no
    // defined scene rect.
    if (!d->hasSceneRect) {
        QRectF oldGrowingItemsBoundingRect = d->growingItemsBoundingRect;
        d->growingItemsBoundingRect |= _q_adjustedRect(item->sceneBoundingRect());
        if (d->growingItemsBoundingRect != oldGrowingItemsBoundingRect)
            emit sceneRectChanged(d->growingItemsBoundingRect);
    }
}

/*!
    \since 4.4

    Returns the scene's style, or the same as QApplication::style() if the
    scene has not been explicitly assigned a style.

    \sa setStyle()
*/
QStyle *QGraphicsScene::style() const
{
    Q_D(const QGraphicsScene);
    // ### This function, and the use of styles in general, is non-reentrant.
    return d->style ? d->style : qApp->style();
}

/*!
    \since 4.4

    Sets or replaces the style of the scene to \a style, and reparents the
    style to this scene. Any previously assigned style is deleted. The scene's
    style defaults to QApplication::style(), and serves as the default for all
    QGraphicsWidget items in the scene.

    Changing the style, either directly by calling this function, or
    indirectly by calling QApplication::setStyle(), will automatically update
    the style for all widgets in the scene that do not have a style explicitly
    assigned to them.

    If \a style is 0, QGraphicsScene will revert to QApplication::style().

    \sa style()
*/
void QGraphicsScene::setStyle(QStyle *style)
{
    Q_D(QGraphicsScene);
    // ### This function, and the use of styles in general, is non-reentrant.
    if (style == d->style)
        return;

    // Delete the old style,
    delete d->style;
    if ((d->style = style))
        d->style->setParent(this);

    // Notify the scene.
    QEvent event(QEvent::StyleChange);
    QApplication::sendEvent(this, &event);

    // Notify all widgets that don't have a style explicitly set.
    foreach (QGraphicsItem *item, items()) {
        if (item->isWidget()) {
            QGraphicsWidget *widget = static_cast<QGraphicsWidget *>(item);
            if (!widget->testAttribute(Qt::WA_SetStyle))
                QApplication::sendEvent(widget, &event);
        }
    }
}

/*!
    \property QGraphicsScene::font
    \since 4.4
    \brief the scene's default font

    This property provides the scene's font. The scene font defaults to,
    and resolves all its entries from, QApplication::font.

    If the scene's font changes, either directly through setFont() or
    indirectly when the application font changes, QGraphicsScene first
    sends itself a \l{QEvent::FontChange}{FontChange} event, and it then
    sends \l{QEvent::FontChange}{FontChange} events to all top-level
    widget items in the scene. These items respond by resolving their own
    fonts to the scene, and they then notify their children, who again
    notify their children, and so on, until all widget items have updated
    their fonts.

    Changing the scene font, (directly or indirectly through
    QApplication::setFont(),) automatically schedules a redraw the entire
    scene.

    \sa QWidget::font, QApplication::setFont(), palette, style()
*/
QFont QGraphicsScene::font() const
{
    Q_D(const QGraphicsScene);
    return d->font;
}
void QGraphicsScene::setFont(const QFont &font)
{
    Q_D(QGraphicsScene);
    QFont naturalFont = qApp->font();
    naturalFont.resolve(0);
    QFont resolvedFont = font.resolve(naturalFont);
    d->setFont_helper(resolvedFont);
}

/*!
    \property QGraphicsScene::palette
    \since 4.4
    \brief the scene's default palette

    This property provides the scene's palette. The scene palette defaults to,
    and resolves all its entries from, QApplication::palette.

    If the scene's palette changes, either directly through setPalette() or
    indirectly when the application palette changes, QGraphicsScene first
    sends itself a \l{QEvent::PaletteChange}{PaletteChange} event, and it then
    sends \l{QEvent::PaletteChange}{PaletteChange} events to all top-level
    widget items in the scene. These items respond by resolving their own
    palettes to the scene, and they then notify their children, who again
    notify their children, and so on, until all widget items have updated
    their palettes.

    Changing the scene palette, (directly or indirectly through
    QApplication::setPalette(),) automatically schedules a redraw the entire
    scene.

    \sa QWidget::palette, QApplication::setPalette(), font, style()
*/
QPalette QGraphicsScene::palette() const
{
    Q_D(const QGraphicsScene);
    return d->palette;
}
void QGraphicsScene::setPalette(const QPalette &palette)
{
    Q_D(QGraphicsScene);
    QPalette naturalPalette = qApp->palette();
    naturalPalette.resolve(0);
    QPalette resolvedPalette = palette.resolve(naturalPalette);
    d->setPalette_helper(resolvedPalette);
}

/*!
    \since 4.4

    Returns the current active window, or 0 if there is no window is currently
    active.

    \sa QGraphicsScene::setActiveWindow()
*/
QGraphicsWidget *QGraphicsScene::activeWindow() const
{
    Q_D(const QGraphicsScene);
    return d->activeWindow;
}

/*!
    \since 4.4
    Activates \a widget, which must be a widget in this scene. You can also
    pass 0 for \a widget, in which case QGraphicsScene will deactivate any
    currently active window.

    \sa activeWindow(), QGraphicsWidget::isActiveWindow()
*/
void QGraphicsScene::setActiveWindow(QGraphicsWidget *widget)
{
    Q_D(QGraphicsScene);
    if (widget && widget->scene() != this) {
        qWarning("QGraphicsScene::setActiveWindow: widget %p must be part of this scene",
                 widget);
        return;
    }

    // Activate the widget's window.
    QGraphicsWidget *window = widget ? widget->window() : 0;
    if (window == d->activeWindow)
        return;

    // Deactivate the last active window.
    if (d->activeWindow) {
        if (QGraphicsWidget *fw = d->activeWindow->focusWidget()) {
            // Remove focus from the current focus item.
            if (fw == focusItem())
                setFocusItem(0, Qt::ActiveWindowFocusReason);
        }

        QEvent event(QEvent::WindowDeactivate);
        QApplication::sendEvent(d->activeWindow, &event);
    }

    // Update activate state.
    d->activeWindow = window;
    QEvent event(QEvent::ActivationChange);
    QApplication::sendEvent(this, &event);

    // Activate
    if (window) {
        QEvent event(QEvent::WindowActivate);
        QApplication::sendEvent(window, &event);

        QList<QGraphicsItem *> siblingWindows;
        QGraphicsItem *parent = window->parentItem();
        // Raise ### inefficient for toplevels
        foreach (QGraphicsItem *sibling, parent ? parent->children() : items()) {
            if (sibling != window && sibling->isWidget()
                && static_cast<QGraphicsWidget *>(sibling)->isWindow()) {
                siblingWindows << sibling;
            }
        }

        // Find the highest z value.
        qreal z = window->zValue();
        for (int i = 0; i < siblingWindows.size(); ++i)
            z = qMax(z, siblingWindows.at(i)->zValue());

        // This will probably never overflow.
        const qreal litt = qreal(0.001);
        window->setZValue(z + litt);

        if (QGraphicsWidget *focusChild = window->focusWidget())
            focusChild->setFocus(Qt::ActiveWindowFocusReason);
    }
}

QT_END_NAMESPACE

#include "moc_qgraphicsscene.cpp"

#endif // QT_NO_GRAPHICSVIEW
