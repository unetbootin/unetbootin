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

#ifndef QPAINTENGINE_RASTER_P_H
#define QPAINTENGINE_RASTER_P_H

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
#include "QtGui/qpainterpath.h"
#include "private/qdatabuffer_p.h"
#include "private/qdrawhelper_p.h"
#include "private/qpaintengine_p.h"
#include "private/qrasterizer_p.h"
#include "private/qstroker_p.h"

#include <stdlib.h>

QT_BEGIN_NAMESPACE

class QFTOutlineMapper;
class QRasterPaintEnginePrivate;
class QRasterBuffer;
class QClipData;

/*******************************************************************************
 * QRasterPaintEngine
 */
class
#ifdef Q_WS_QWS
Q_GUI_EXPORT
#endif
QRasterPaintEngine : public QPaintEngine
{
    Q_DECLARE_PRIVATE(QRasterPaintEngine)
public:
    QRasterPaintEngine();
    ~QRasterPaintEngine();
    bool begin(QPaintDevice *device);
    bool end();

    void updateState(const QPaintEngineState &state);
    void updateMatrix(const QTransform &matrix);

    void updateClipRegion(const QRegion &region, Qt::ClipOperation op);
    void updateClipPath(const QPainterPath &path, Qt::ClipOperation op);

    void drawPath(const QPainterPath &path);
    void drawPolygon(const QPointF *points, int pointCount, PolygonDrawMode mode);
    void drawPolygon(const QPoint *points, int pointCount, PolygonDrawMode mode);
    void fillPath(const QPainterPath &path, QSpanData *fillData);
    void fillPolygon(const QPointF *points, int pointCount, PolygonDrawMode mode);

    void drawEllipse(const QRectF &rect);

    void drawRects(const QRect  *rects, int rectCount);
    void drawRects(const QRectF *rects, int rectCount);
    void fastFillRect(const QRect &rect, const QBrush &brush);

    void drawPixmap(const QRectF &r, const QPixmap &pm, const QRectF &sr);
    void drawImage(const QRectF &r, const QImage &pm, const QRectF &sr,
                   Qt::ImageConversionFlags falgs = Qt::AutoColor);
    void drawTiledPixmap(const QRectF &r, const QPixmap &pm, const QPointF &sr);
    void drawTextItem(const QPointF &p, const QTextItem &textItem);

    void drawLines(const QLine *line, int lineCount);
    void drawLines(const QLineF *line, int lineCount);

    void drawPoints(const QPointF *points, int pointCount);

#ifdef Q_NO_USING_KEYWORD
    inline void drawPoints(const QPoint *points, int pointCount) { QPaintEngine::drawPoints(points, pointCount); }
    inline void drawEllipse(const QRect &rect) { QPaintEngine::drawEllipse(rect); }
#else
    using QPaintEngine::drawPolygon;
    using QPaintEngine::drawPoints;
    using QPaintEngine::drawEllipse;
#endif

    void setFlushOnEnd(bool flush);
    void flush(QPaintDevice *device, const QPoint &offset);

    void releaseBuffer();

    QSize size() const;

#ifndef QT_NO_DEBUG
    void saveBuffer(const QString &s) const;
#endif

#ifdef Q_WS_MAC
    CGContextRef macCGContext() const;
#endif

#ifdef Q_WS_WIN
    HDC getDC() const;
    void releaseDC(HDC hdc) const;

    void disableClearType();
#endif
    //QWS hack
    void alphaPenBlt(const void* src, int bpl, bool mono, int rx,int ry,int w,int h);
#ifdef Q_WS_QWS
    void qwsFillRect(int x, int y, int w, int h);
#endif

    Type type() const { return Raster; }

    QPoint coordinateOffset() const;

#ifdef Q_WS_QWS
    virtual void drawColorSpans(const QSpan *spans, int count, uint color);
    virtual void drawBufferSpan(const uint *buffer, int bufsize,
                                int x, int y, int length, uint const_alpha);
#endif

protected:
    QRasterPaintEngine(QRasterPaintEnginePrivate &d);
private:
    void init();

#if defined(Q_WS_WIN)
    bool drawTextInFontBuffer(const QRect &devRect, int xmin, int ymin, int xmax,
        int ymax, const QTextItem &textItem, bool clearType, qreal leftBearingReserve,
        const QPointF &topLeft);
#endif
};


/*******************************************************************************
 * QRasterPaintEnginePrivate
 */
class QRasterPaintEnginePrivate : public QPaintEnginePrivate
{
    Q_DECLARE_PUBLIC(QRasterPaintEngine)
public:

    void updateClip_helper(const QPainterPath &path, Qt::ClipOperation);

    void drawBitmap(const QPointF &pos, const QPixmap &image, QSpanData *fill);

    void rasterize(QT_FT_Outline *outline, ProcessSpans callback, QSpanData *spanData, QRasterBuffer *rasterBuffer);
    void rasterize(QT_FT_Outline *outline, ProcessSpans callback, void *userData, QRasterBuffer *rasterBuffer);
    void setClipRect(const QRect &rect);
    void setClipRegion(const QRegion &region);
    void updateMatrixData(QSpanData *spanData, const QBrush &brush, const QTransform &brushMatrix);

    QTransform brushMatrix() const {
        QTransform m(matrix);
        m.translate(brushOffset.x(), brushOffset.y());
        return m;
    }

    bool isUnclipped_normalized(const QRect &rect) const;
    bool isUnclipped(const QRect &rect, int penWidth) const;
    bool isUnclipped(const QRectF &rect, int penWidth) const;
    ProcessSpans getPenFunc(const QRect &rect, const QSpanData *data) const;
    ProcessSpans getPenFunc(const QRectF &rect, const QSpanData *data) const;
    ProcessSpans getBrushFunc(const QRect &rect, const QSpanData *data) const;
    ProcessSpans getBrushFunc(const QRectF &rect, const QSpanData *data) const;

    void initializeRasterizer(QSpanData *data);

    QPointF brushOffset;
    QBrush brush;
    QPen pen;
    QTransform matrix;
    int opacity;

    QPaintDevice *device;
    QFTOutlineMapper *outlineMapper;
    QRasterBuffer *rasterBuffer;
#ifdef Q_WS_WIN
    QRasterBuffer *fontRasterBuffer;
    uint clear_type_text : 1;
#endif

    QPainterPath baseClip;
    QRect deviceRect;
    QRegion clipRegion;
    QRegion disabledClipRegion;

    QSpanData penData;
    QSpanData brushData;

    QStroker basicStroker;
    QDashStroker *dashStroker;
    QStrokerOps *stroker;


    QT_FT_Raster *blackRaster;
    QT_FT_Raster *grayRaster;
    unsigned long rasterPoolSize;
    unsigned char *rasterPoolBase;

    qreal inverseScale;
    QDataBuffer<QLineF> cachedLines;

    int deviceDepth;

    qreal txscale;

    uint txop;

    uint fast_pen : 1;
    uint antialiased : 1;
    uint bilinear : 1;
    uint flushOnEnd : 1;
    uint mono_surface : 1;
    uint int_xform : 1;
    uint user_clip_enabled : 1;
    uint fast_text : 1;
    uint paint_unclipped : 1;
    uint tx_noshear : 1;
#ifdef Q_WS_WIN
    uint isPlain45DegreeRotation : 1;
#endif

    QRasterizer rasterizer;
};

class QClipData {
public:
    QClipData(int height);
    ~QClipData();
    int clipSpanHeight;
    struct ClipLine {
        int count;
        QSpan *spans;
    } *clipLines;

    int allocated;
    int count;
    QSpan *spans;
    int xmin, xmax, ymin, ymax;

    void appendSpan(int x, int length, int y, int coverage);
    void appendSpans(const QSpan *s, int num);
    void setClipRect(const QRect &rect);
    void setClipRegion(const QRegion &region);
    void fixup();
};

inline void QClipData::appendSpan(int x, int length, int y, int coverage)
{
    if (count == allocated) {
        allocated *= 2;
        spans = (QSpan *)realloc(spans, allocated*sizeof(QSpan));
    }
    spans[count].x = x;
    spans[count].len = length;
    spans[count].y = y;
    spans[count].coverage = coverage;
    ++count;
}

inline void QClipData::appendSpans(const QSpan *s, int num)
{
    if (count + num > allocated) {
        do {
            allocated *= 2;
        } while (count + num > allocated);
        spans = (QSpan *)realloc(spans, allocated*sizeof(QSpan));
    }
    memcpy(spans+count, s, num*sizeof(QSpan));
    count += num;
}

#ifdef Q_WS_QWS
class Q_GUI_EXPORT QCustomRasterPaintDevice : public QPaintDevice
{
public:
    QCustomRasterPaintDevice(QWidget *w) : widget(w) {}

    int devType() const { return QInternal::CustomRaster; }

    virtual int metric(PaintDeviceMetric m) const;

    virtual void* memory() const { return 0; }

    virtual QImage::Format format() const {
        return QImage::Format_ARGB32_Premultiplied;
    }

    virtual int bytesPerLine() const;

    virtual QSize size() const {
        return static_cast<QRasterPaintEngine*>(paintEngine())->size();
    }

private:
    QWidget *widget;
};
#endif // Q_WS_QWS

/*******************************************************************************
 * QRasterBuffer
 */
class QRasterBuffer
{
public:
#if defined(Q_WS_WIN)
    QRasterBuffer()
        : clip(0),
          m_hdc(0),
          m_bitmap(0),
          m_null_bitmap(0),
          m_text_buffer(false),
          m_width(0),
          m_height(0),
          m_buffer(0)
    {
        init();
    }

    HDC hdc() const { return m_hdc; }
#elif defined(Q_WS_X11)
    QRasterBuffer() : clip(0), m_width(0), m_height(0), m_buffer(0) { init(); }
#elif defined(Q_WS_QWS)
    QRasterBuffer() : clip(0), m_width(0), m_height(0), m_buffer(0) { init(); }
#elif defined(Q_WS_MAC)
    QRasterBuffer() : m_ctx(0), m_data(0), clip(0), m_width(0), m_height(0), m_buffer(0) { init(); }
    CGContextRef macCGContext() const;
    mutable CGContextRef m_ctx;
    CGImageRef m_data;
#endif
    ~QRasterBuffer();

    void init();

    void prepare(QImage *image);
    void prepare(QPixmap *pix);
#ifdef Q_WS_QWS
    void prepare(QCustomRasterPaintDevice *device);
#endif
    void prepare(int w, int h);
    void prepareBuffer(int w, int h);

#ifdef Q_WS_WIN
    void setupHDC(bool clear_type);
    inline void setTextBuffer(bool t);
#endif

    void resetBuffer(int val=0);

    uchar *scanLine(int y) { Q_ASSERT(y>=0); Q_ASSERT(y<m_height); return m_buffer + y * bytes_per_line; }

#ifndef QT_NO_DEBUG
    QImage clipImage() const;
    QImage bufferImage() const;
#endif

    void flushToARGBImage(QImage *image) const;

    int width() const { return m_width; }
    int height() const { return m_height; }
    int bytesPerLine() const { return bytes_per_line; }
    int bytesPerPixel() const { return bytes_per_pixel; }

    uchar *buffer() const { return m_buffer; }

    QRect clipRect;
    QRect disabledClipRect;
    QRegion clipRegion;
    QRegion disabledClipRegion;
    QClipData *clip;
    QClipData *disabled_clip;
    bool clipEnabled;

    bool monoDestinationWithClut;
    QRgb destColor0;
    QRgb destColor1;

    QPainter::CompositionMode compositionMode;
    QImage::Format format;
    DrawHelper *drawHelper;
    QImage colorizeBitmap(const QImage &image, const QColor &color);

    void resetClip() { delete clip; clip = 0; }
private:
#if defined(Q_WS_WIN)
    HDC m_hdc;
    HBITMAP m_bitmap;
    HBITMAP m_null_bitmap;
    bool m_text_buffer;
#endif
    int m_width;
    int m_height;
    int bytes_per_line;
    int bytes_per_pixel;
    uchar *m_buffer;
};

#ifdef Q_WS_WIN
inline void QRasterBuffer::setTextBuffer(bool t) { m_text_buffer = t; }
#endif

QT_END_NAMESPACE
#endif // QPAINTENGINE_RASTER_P_H
