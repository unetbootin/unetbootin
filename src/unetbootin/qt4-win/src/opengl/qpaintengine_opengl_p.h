/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtOpenGL module of the Qt Toolkit.
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

#ifndef QPAINTENGINE_OPENGL_P_H
#define QPAINTENGINE_OPENGL_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of the QLibrary class.  This header file may change from
// version to version without notice, or even be removed.
//
// We mean it.
//

#include "QtGui/qpaintengine.h"

QT_BEGIN_NAMESPACE

class QOpenGLPaintEnginePrivate;

class QOpenGLPaintEngine : public QPaintEngine
{
    Q_DECLARE_PRIVATE(QOpenGLPaintEngine)
public:
    QOpenGLPaintEngine();
    ~QOpenGLPaintEngine();

    bool begin(QPaintDevice *pdev);
    bool end();

    void updateState(const QPaintEngineState &state);

    void updatePen(const QPen &pen);
    void updateBrush(const QBrush &brush, const QPointF &pt);
    void updateFont(const QFont &font);
    void updateMatrix(const QTransform &matrix);
    void updateClipRegion(const QRegion &region, Qt::ClipOperation op);
    void updateRenderHints(QPainter::RenderHints hints);
    void updateCompositionMode(QPainter::CompositionMode composition_mode);

    void drawRects(const QRectF *r, int rectCount);
    void drawLines(const QLineF *lines, int lineCount);
    void drawPoints(const QPointF *p, int pointCount);

#ifdef Q_NO_USING_KEYWORD
    inline void drawPoints(const QPoint *points, int pointCount) { QPaintEngine::drawPoints(points, pointCount); }
    inline void drawPolygon(const QPoint *points, int pointCount, PolygonDrawMode mode)
        { QPaintEngine::drawPolygon(points, pointCount, mode); }
    inline void drawLines(const QLine *lines, int lineCount) { QPaintEngine::drawLines(lines, lineCount); }
    inline void drawRects(const QRect *r, int rectCount) { QPaintEngine::drawRects(r, rectCount); }
#else
    using QPaintEngine::drawRects;
    using QPaintEngine::drawPolygon;
    using QPaintEngine::drawPoints;
    using QPaintEngine::drawLines;
#endif

    void drawPixmap(const QRectF &r, const QPixmap &pm, const QRectF &sr);

    void drawPath(const QPainterPath &path);
    void drawPolygon(const QPointF *points, int pointCount, PolygonDrawMode mode);
    void drawTiledPixmap(const QRectF &r, const QPixmap &pixmap, const QPointF &s);
    void drawImage(const QRectF &r, const QImage &image, const QRectF &sr,
                   Qt::ImageConversionFlags conversionFlags);
    void drawTextItem(const QPointF &p, const QTextItem &ti);

    void drawEllipse(const QRectF &rect);

#ifdef Q_WS_WIN
    HDC handle() const;
#else
    Qt::HANDLE handle() const;
#endif
    inline Type type() const { return QPaintEngine::OpenGL; }

private:
    void drawPolyInternal(const QPolygonF &pa, bool close = true);
    void drawTextureRect(int tx_width, int tx_height, const QRectF &r, const QRectF &sr, GLenum target);
    Q_DISABLE_COPY(QOpenGLPaintEngine)
};

QT_END_NAMESPACE

#endif // QPAINTENGINE_OPENGL_P_H
