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

#ifndef QGRAPHICSVIEW_P_H
#define QGRAPHICSVIEW_P_H

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

#include "qgraphicsview.h"

#if !defined(QT_NO_GRAPHICSVIEW) || (QT_EDITION & QT_MODULE_GRAPHICSVIEW) != QT_MODULE_GRAPHICSVIEW

#include <QtGui/qevent.h>
#include "qgraphicssceneevent.h"
#include <private/qabstractscrollarea_p.h>

QT_BEGIN_NAMESPACE

class Q_GUI_EXPORT QGraphicsViewPrivate : public QAbstractScrollAreaPrivate
{
    Q_DECLARE_PUBLIC(QGraphicsView)
public:
    QGraphicsViewPrivate();

    void recalculateContentSize();
    void centerView(QGraphicsView::ViewportAnchor anchor);

    QPainter::RenderHints renderHints;

    QGraphicsView::DragMode dragMode;
    bool sceneInteractionAllowed;
    QRectF sceneRect;
    bool hasSceneRect;
    void updateLastCenterPoint();
    bool connectedToScene;

    qint64 horizontalScroll() const;
    qint64 verticalScroll() const;

    QList<QGraphicsItem *> itemsInArea(const QPainterPath &path,
                                       Qt::ItemSelectionMode mode = Qt::IntersectsItemShape) const;

    QPointF mousePressItemPoint;
    QPointF mousePressScenePoint;
    QPoint mousePressViewPoint;
    QPoint mousePressScreenPoint;
    QPointF lastMouseMoveScenePoint;
    QPoint lastMouseMoveScreenPoint;
    Qt::MouseButton mousePressButton;
    QTransform matrix;
    bool identityMatrix;
    qint64 scrollX, scrollY;
    bool dirtyScroll;
    void updateScroll();

    bool accelerateScrolling;
    qreal leftIndent;
    qreal topIndent;

    // Replaying mouse events
    QMouseEvent lastMouseEvent;
    bool useLastMouseEvent;
    void replayLastMouseEvent();
    void storeMouseEvent(QMouseEvent *event);
    void mouseMoveEventHandler(QMouseEvent *event);

    QPointF lastCenterPoint;
    bool keepLastCenterPoint;
    Qt::Alignment alignment;
    bool transforming;

    QGraphicsView::ViewportAnchor transformationAnchor;
    QGraphicsView::ViewportAnchor resizeAnchor;
    QGraphicsView::ViewportUpdateMode viewportUpdateMode;
    QGraphicsView::OptimizationFlags optimizationFlags;

    QPointer<QGraphicsScene> scene;
#ifndef QT_NO_RUBBERBAND
    QRect rubberBandRect;
    QRegion rubberBandRegion(const QWidget *widget, const QRect &rect) const;
    bool rubberBanding;
    Qt::ItemSelectionMode rubberBandSelectionMode;
#endif
    bool handScrolling;
    int handScrollMotions;

    QGraphicsView::CacheMode cacheMode;

    QVector<QStyleOptionGraphicsItem> styleOptions;
    bool mustAllocateStyleOptions;
    QStyleOptionGraphicsItem *allocStyleOptionsArray(int numItems);
    void freeStyleOptionsArray(QStyleOptionGraphicsItem *array);

    QBrush backgroundBrush;
    QBrush foregroundBrush;
    QPixmap backgroundPixmap;
    bool mustResizeBackgroundPixmap;
    QRegion backgroundPixmapExposed;

#ifndef QT_NO_CURSOR
    QCursor originalCursor;
    bool hasStoredOriginalCursor;
    void _q_setViewportCursor(const QCursor &cursor);
    void _q_unsetViewportCursor();
#endif

    QGraphicsSceneDragDropEvent *lastDragDropEvent;
    void storeDragDropEvent(const QGraphicsSceneDragDropEvent *event);
    void populateSceneDragDropEvent(QGraphicsSceneDragDropEvent *dest,
                                    QDropEvent *source);

    QRect mapToViewRect(const QGraphicsItem *item, const QRectF &rect) const;
    QRegion mapToViewRegion(const QGraphicsItem *item, const QRectF &rect) const;
    void itemUpdated(QGraphicsItem *item, const QRectF &rect);
    bool fullUpdatePending;
    QList<QRect> dirtyRects;
    QList<QRegion> dirtyRegions;
    int dirtyRectCount;
    QRect dirtyBoundingRect;
    void updateLater();
    bool updatingLater;
    void _q_updateLaterSlot();
    void updateAll();
    void updateRect(const QRect &rect);
    void updateRegion(const QRegion &region);
    bool updateSceneSlotReimplementedChecked;

    QList<QGraphicsItem *> findItems(const QRegion &exposedRegion,
				     const QTransform &worldTransform,
                                     bool *allItems) const;

    void generateStyleOptions(const QList<QGraphicsItem *> &itemList,
			      QGraphicsItem **itemArray,
			      QStyleOptionGraphicsItem *styleOptionArray,
			      const QTransform &worldTransform,
			      bool allItems,
			      const QRegion &exposedRegion) const;
};

QT_END_NAMESPACE

#endif // QT_NO_GRAPHICSVIEW

#endif
