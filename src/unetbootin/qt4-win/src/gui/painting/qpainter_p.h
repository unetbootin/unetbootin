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

#ifndef QPAINTER_P_H
#define QPAINTER_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "QtGui/qbrush.h"
#include "QtGui/qfont.h"
#include "QtGui/qpen.h"
#include "QtGui/qregion.h"
#include "QtGui/qmatrix.h"
#include "QtGui/qpainter.h"
#include "QtGui/qpainterpath.h"
#include "QtGui/qpaintengine.h"
#include <QtCore/qhash.h>

QT_BEGIN_NAMESPACE

class QPaintEngine;
struct QTLWExtra;

class QPainterClipInfo
{
public:
    enum ClipType { RegionClip, PathClip };

    QPainterClipInfo(const QPainterPath &p, Qt::ClipOperation op, const QTransform &m) :
        clipType(PathClip), matrix(m), operation(op), path(p) { }

    QPainterClipInfo(const QRegion &r, Qt::ClipOperation op, const QTransform &m) :
        clipType(RegionClip), matrix(m), operation(op), region(r) { }

    ClipType clipType;
    QTransform matrix;
    Qt::ClipOperation operation;
    QPainterPath path;
    QRegion region;
};


class QPainterState : public QPaintEngineState
{
public:
    QPainterState();
    QPainterState(const QPainterState *s);
    ~QPainterState();
    void init(QPainter *p);

    QPointF bgOrigin;
    QFont font;
    QFont deviceFont;
    QPen pen;
    QBrush brush;
    QBrush bgBrush;             // background brush
    QRegion clipRegion;
    QPainterPath clipPath;
    Qt::ClipOperation clipOperation;
    QPainter::RenderHints renderHints;
    QList<QPainterClipInfo> clipInfo;
    QTransform worldMatrix;       // World transformation matrix, not window and viewport
    QTransform matrix;            // Complete transformation matrix,
    QPoint redirection_offset;
    int txop;
    int wx, wy, ww, wh;         // window rectangle
    int vx, vy, vw, vh;         // viewport rectangle
    qreal opacity;
    qreal localOpacity;

    uint WxF:1;                 // World transformation
    uint VxF:1;                 // View transformation
    uint clipEnabled:1;

    Qt::BGMode bgMode;
    QPainter *painter;
    Qt::LayoutDirection layoutDirection;
    QPainter::CompositionMode composition_mode;
    uint emulationSpecifier;
    uint changeFlags;
};


class QPainterPrivate
{
    Q_DECLARE_PUBLIC(QPainter)
public:
    QPainterPrivate(QPainter *painter)
        : q_ptr(painter), d_ptrs(0), txinv(0), emptyState(true), inDestructor(false),
          refcount(1), device(0), original_device(0), helper_device(0), engine(0), fillrect_func(0)
    {
        states.push_back(new QPainterState());
        state = states.back();
    }

    ~QPainterPrivate()
    {
        for (int i=0; i<states.size(); ++i)
            delete states.at(i);
    }

    QPainter *q_ptr;
    QPainterPrivate **d_ptrs;

    QPainterState *state;
    QVector<QPainterState*> states;

    QTransform invMatrix;
    uint txinv:1;
    uint emptyState:1;
    uint inDestructor : 1;
    uint refcount;

    enum DrawOperation { StrokeDraw        = 0x1,
                         FillDraw          = 0x2,
                         StrokeAndFillDraw = 0x3
    };

    void updateEmulationSpecifier(QPainterState *s);
    void updateStateImpl(QPainterState *state);
    void updateState(QPainterState *state);

    void draw_helper(const QPainterPath &path, DrawOperation operation = StrokeAndFillDraw);
    void drawStretchedGradient(const QPainterPath &path, DrawOperation operation);
    void drawOpaqueBackground(const QPainterPath &path, DrawOperation operation);

    void updateMatrix();
    void updateInvMatrix();
    void updateCombinedOpacity();
    void initSharedPainter(QWidget *widget, QTLWExtra *extra);
    void init();

    int rectSubtraction() const {
        return state->pen.style() != Qt::NoPen && state->pen.width() == 0 ? 1 : 0;
    }

    QTransform viewTransform() const;
    static bool attachPainterPrivate(QPainter *q, QPaintDevice *pdev);
    void detachPainterPrivate(QPainter *q);

    QPaintDevice *device;
    QPaintDevice *original_device;
    QPaintDevice *helper_device;
    QPaintEngine *engine;

    typedef void (QPaintEngine::*FillRectBackdoor)(const QRect&, const QBrush&);
    FillRectBackdoor fillrect_func;
};

QString qt_generate_brush_key(const QBrush &brush);

QT_END_NAMESPACE

#endif // QPAINTER_P_H
