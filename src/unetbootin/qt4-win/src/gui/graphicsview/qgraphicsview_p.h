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

class QGraphicsViewPrivate : public QAbstractScrollAreaPrivate
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
    bool accelerateScrolling;
    qreal leftIndent;
    qreal topIndent;

    // Replaying mouse events
    QMouseEvent lastMouseEvent;
    bool useLastMouseEvent;
    void replayLastMouseEvent();
    void storeMouseEvent(QMouseEvent *event);

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

    void markDirtyRegion(const QRegion &region);
    void markAllDirty();
    bool fullUpdatePending;
    QList<QRect> dirtyRegion;
};

QT_END_NAMESPACE

#endif // QT_NO_GRAPHICSVIEW

#endif
