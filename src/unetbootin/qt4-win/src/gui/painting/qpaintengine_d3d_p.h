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

#ifndef QPAINTENGINE_D3D_P_H
#define QPAINTENGINE_D3D_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "QtGui/qpaintengine.h"
#include <d3d9.h>

QT_BEGIN_NAMESPACE

class QDirect3DPaintEnginePrivate;
class QDirect3DPaintEngine : public QPaintEngine
{
    Q_DECLARE_PRIVATE(QDirect3DPaintEngine)
public:
    QDirect3DPaintEngine();
    ~QDirect3DPaintEngine();
    bool begin(QPaintDevice *device);

    void drawEllipse(const QRectF &rect);
    void drawEllipse(const QRect &rect);

    void drawImage(const QRectF &rectangle, const QImage &image, const QRectF &sr,
        Qt::ImageConversionFlags flags = Qt::AutoColor);

    void drawLines(const QLineF *lines, int lineCount);
    void drawLines(const QLine *lines, int lineCount);

    void drawPath(const QPainterPath &path);

    void drawPixmap(const QRectF &r, const QPixmap &pm, const QRectF &sr);

    void drawPoints(const QPointF *points, int pointCount);
    void drawPoints(const QPoint *points, int pointCount);

    void drawPolygon(const QPointF *points, int pointCount, PolygonDrawMode mode);
    void drawPolygon(const QPoint *points, int pointCount, PolygonDrawMode mode);

    void drawRects(const QRectF *rects, int rectCount);
    void drawRects(const QRect * rects, int rectCount);

    void drawTextItem(const QPointF &p, const QTextItem &textItem);

    void drawTiledPixmap(const QRectF &r, const QPixmap &pm, const QPointF &sr);

    bool end();

    Type type() const { return Direct3D; }
    void updateState(const QPaintEngineState &state);

    void cleanup();

    HDC getDC() const;
    void setFlushOnEnd(bool flushOnEnd);
    bool hasDirect3DSupport();

public:
    void scroll(QPaintDevice *pd, const RECT &srcrect, const RECT &destrect);
    LPDIRECT3DSWAPCHAIN9 swapChain(QPaintDevice *pd);
    void releaseSwapChain(QPaintDevice *pd);

private:
    Q_DISABLE_COPY(QDirect3DPaintEngine)
    friend class QPixmap;
    friend class QD3DGlyphCache;
};

QT_END_NAMESPACE

#endif
