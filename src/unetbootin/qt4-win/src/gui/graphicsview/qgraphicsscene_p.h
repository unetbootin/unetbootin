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

#ifndef QGRAPHICSSCENE_P_H
#define QGRAPHICSSCENE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of qapplication_*.cpp, qwidget*.cpp and qfiledialog.cpp.  This header
// file may change from version to version without notice, or even be removed.
//
// We mean it.
//

#include "qgraphicsscene.h"

#if !defined(QT_NO_GRAPHICSVIEW) || (QT_EDITION & QT_MODULE_GRAPHICSVIEW) != QT_MODULE_GRAPHICSVIEW

#include "qgraphicsscene_bsp_p.h"

#include <private/qobject_p.h>
#include <QtCore/qlist.h>
#include <QtCore/qmap.h>
#include <QtCore/qset.h>
#include <QtGui/qfont.h>
#include <QtGui/qpalette.h>
#include <QtGui/qstyle.h>

QT_BEGIN_NAMESPACE

class QGraphicsView;
class QGraphicsWidget;

class QGraphicsScenePrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QGraphicsScene)
public:
    QGraphicsScenePrivate();
    void init();

    QGraphicsScene::ItemIndexMethod indexMethod;
    int bspTreeDepth;

    QList<QGraphicsItem *> estimateItemsInRect(const QRectF &rect) const;
    void addToIndex(QGraphicsItem *item);
    void removeFromIndex(QGraphicsItem *item);
    void resetIndex();

    QGraphicsSceneBspTree bspTree;
    void _q_updateIndex();
    int lastItemCount;

    QRectF sceneRect;
    bool hasSceneRect;
    QRectF growingItemsBoundingRect;
    QRectF largestUntransformableItem;

    void _q_emitUpdated();
    QList<QRectF> updatedRects;
    bool updateAll;
    bool calledEmitUpdated;

    QPainterPath selectionArea;
    int selectionChanging;
    QSet<QGraphicsItem *> selectedItems;
    QList<QGraphicsItem *> unindexedItems;
    QList<QGraphicsItem *> indexedItems;
    QList<QGraphicsItem *> pendingUpdateItems;
    QList<QGraphicsItem *> unpolishedItems;
    QMap<QGraphicsItem *, QPointF> movingItemsInitialPositions;
    void _q_updateLater();
    void _q_polishItems();

    QList<int> freeItemIndexes;
    bool regenerateIndex;

    bool purgePending;
    void _q_removeItemLater(QGraphicsItem *item);
    QSet<QGraphicsItem *> removedItems;
    void purgeRemovedItems();

    QBrush backgroundBrush;
    QBrush foregroundBrush;

    int indexTimerId;
    bool restartIndexTimer;
    void startIndexTimer();

    bool hasFocus;
    QGraphicsItem *focusItem;
    QGraphicsItem *lastFocusItem;
    QGraphicsWidget *tabFocusFirst;
    QGraphicsWidget *activeWindow;
    int activationRefCount;

    QList<QGraphicsWidget *> popupWidgets;
    void addPopup(QGraphicsWidget *widget);
    void removePopup(QGraphicsWidget *widget, bool itemIsDying = false);

    QGraphicsItem *lastMouseGrabberItem;
    bool lastMouseGrabberItemHasImplicitMouseGrab;
    QList<QGraphicsItem *> mouseGrabberItems;
    void grabMouse(QGraphicsItem *item, bool implicit = false);
    void ungrabMouse(QGraphicsItem *item, bool itemIsDying = false);
    void clearMouseGrabber();

    QList<QGraphicsItem *> keyboardGrabberItems;
    void grabKeyboard(QGraphicsItem *item);
    void ungrabKeyboard(QGraphicsItem *item, bool itemIsDying = false);
    void clearKeyboardGrabber();
    
    QGraphicsItem *dragDropItem;
    QGraphicsWidget *enterWidget;
    Qt::DropAction lastDropAction;
    QList<QGraphicsItem *> cachedItemsUnderMouse;
    QList<QGraphicsItem *> hoverItems;
    QMap<Qt::MouseButton, QPointF> mouseGrabberButtonDownPos;
    QMap<Qt::MouseButton, QPointF> mouseGrabberButtonDownScenePos;
    QMap<Qt::MouseButton, QPoint> mouseGrabberButtonDownScreenPos;
    QList<QGraphicsItem *> itemsAtPosition(const QPoint &screenPos,
                                           const QPointF &scenePos,
                                           QWidget *widget) const;
    static bool itemCollidesWithPath(QGraphicsItem *item, const QPainterPath &path, Qt::ItemSelectionMode mode);
    void storeMouseButtonsForMouseGrabber(QGraphicsSceneMouseEvent *event);

    QList<QGraphicsView *> views;
    bool painterStateProtection(const QPainter *painter) const;

    QMultiMap<QGraphicsItem *, QGraphicsItem *> sceneEventFilters;
    void installSceneEventFilter(QGraphicsItem *watched, QGraphicsItem *filter);
    void removeSceneEventFilter(QGraphicsItem *watched, QGraphicsItem *filter);
    bool filterEvent(QGraphicsItem *item, QEvent *event);
    bool sendEvent(QGraphicsItem *item, QEvent *event);

    bool dispatchHoverEvent(QGraphicsSceneHoverEvent *hoverEvent);
    bool itemAcceptsHoverEvents_helper(const QGraphicsItem *item) const;
    void leaveScene();

    void cloneDragDropEvent(QGraphicsSceneDragDropEvent *dest,
                           QGraphicsSceneDragDropEvent *source);
    void sendDragDropEvent(QGraphicsItem *item,
                           QGraphicsSceneDragDropEvent *dragDropEvent);
    void sendHoverEvent(QEvent::Type type, QGraphicsItem *item,
                        QGraphicsSceneHoverEvent *hoverEvent);
    void sendMouseEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mousePressEventHandler(QGraphicsSceneMouseEvent *mouseEvent);
    QGraphicsWidget *windowForItem(const QGraphicsItem *item) const;

    static bool closestItemFirst(const QGraphicsItem *item1, const QGraphicsItem *item2);
    static void sortItems(QList<QGraphicsItem *> *itemList);

    static void drawItemHelper(QGraphicsItem *item, QPainter *painter,
                               const QStyleOptionGraphicsItem *option, QWidget *widget);

    QStyle *style;
    QFont font;
    QPalette palette;
};

QT_END_NAMESPACE

#endif // QT_NO_GRAPHICSVIEW

#endif
