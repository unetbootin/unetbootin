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

#include <QtCore/qglobal.h>

#define QT_FT_BEGIN_HEADER
#define QT_FT_END_HEADER
#include <private/qrasterdefs_p.h>
#include <private/qgrayraster_p.h>
#include <private/qblackraster_p.h>

#include <qpainterpath.h>
#include <qdebug.h>
#include <qhash.h>
#include <qlabel.h>
#include <qbitmap.h>
#include <qmath.h>

#include <private/qdatabuffer_p.h>
#include <private/qpainter_p.h>
#include <private/qmath_p.h>
#include <private/qtextengine_p.h>
#include <private/qfontengine_p.h>
#include <private/qpixmap_raster_p.h>
#include <private/qpolygonclipper_p.h>
#include <private/qrasterizer_p.h>
#include <private/qimage_p.h>

#include "qpaintengine_raster_p.h"
#include "qbezier_p.h"

#if defined(Q_WS_X11)
#  include <private/qfontengine_ft_p.h>
#  include <qwidget.h>
#  include <qx11info_x11.h>
#  include <X11/Xlib.h>
#  include <X11/Xutil.h>
#  undef None
#elif defined(Q_WS_WIN)
#  include <qt_windows.h>
#  include <qvarlengtharray.h>
#  include <private/qfontengine_p.h>
#  if defined(Q_OS_WINCE)
#    include "qguifunctions_wince.h"
#  endif
#elif defined(Q_WS_MAC)
#  include <private/qt_mac_p.h>
#  include <private/qpixmap_mac_p.h>
#  include <private/qpaintengine_mac_p.h>
#  if Q_BYTE_ORDER == Q_BIG_ENDIAN
#    define BITMAPS_ARE_MSB
#  endif
#elif defined(Q_WS_QWS)
#  if !defined(QT_NO_FREETYPE)
#    include <private/qfontengine_ft_p.h>
#  endif
#  if !defined(QT_NO_QWS_QPF2)
#    include <private/qfontengine_qpf_p.h>
#  endif
#  include <private/qabstractfontengine_p.h>
#endif

#if defined(Q_WS_WIN64)
#  include <malloc.h>
#endif
#include <limits.h>

#if defined(Q_WS_WIN)
#  ifndef SPI_GETFONTSMOOTHINGTYPE
#    define SPI_GETFONTSMOOTHINGTYPE 0x200A
#  endif

#  ifndef FE_FONTSMOOTHINGCLEARTYPE
#    define FE_FONTSMOOTHINGCLEARTYPE 0x0002
#  endif
#endif

#if defined(QT_NO_FPU) || (_MSC_VER >= 1300 && _MSC_VER < 1400)
#  define FLOATING_POINT_BUGGY_OR_NO_FPU
#endif

QT_BEGIN_NAMESPACE

#define qreal_to_fixed_26_6(f) (int(f * 64))
#define qt_swap_int(x, y) { int tmp = (x); (x) = (y); (y) = tmp; }
#define qt_swap_qreal(x, y) { qreal tmp = (x); (x) = (y); (y) = tmp; }

#ifdef Q_WS_WIN
void qt_draw_text_item(const QPointF &point, const QTextItemInt &ti, HDC hdc,
                       bool convertToText, const QTransform &xform, const QPointF &topLeft);
static bool qt_enable_16bit_colors = false;
#endif

// #define QT_DEBUG_DRAW
// #define QT_DEBUG_CONVERT

#define QT_FAST_SPANS

static const qreal aliasedCoordinateDelta = 0.5 - 1e-6;

/********************************************************************************
 * Span functions
 */
static void qt_span_fill_clipRect(int count, const QSpan *spans, void *userData);
static void qt_span_fill_clipRegion(int count, const QSpan *spans, void *userData);
static void qt_span_fill_clipped(int count, const QSpan *spans, void *userData);
static void qt_span_clip(int count, const QSpan *spans, void *userData);

struct ClipData
{
    QClipData *oldClip;
    QClipData *newClip;
    Qt::ClipOperation operation;
};

enum LineDrawMode {
    LineDrawClipped,
    LineDrawNormal,
    LineDrawIncludeLastPixel
};

static void drawLine_midpoint_i(int x1, int y1, int x2, int y2, ProcessSpans span_func, QSpanData *data,
                                LineDrawMode style, const QRect &devRect);
static void drawLine_midpoint_dashed_i(int x1, int y1, int x2, int y2,
                                       QPen *pen, ProcessSpans span_func, QSpanData *data,
                                       LineDrawMode style, const QRect &devRect,
                                       int *patternOffset);
// static void drawLine_midpoint_f(qreal x1, qreal y1, qreal x2, qreal y2,
//                                 ProcessSpans span_func, QSpanData *data,
//                                 LineDrawMode style, const QRect &devRect);

static void drawEllipse_midpoint_i(const QRect &rect, const QRect &clip,
                                   ProcessSpans pen_func, ProcessSpans brush_func,
                                   QSpanData *pen_data, QSpanData *brush_data);

// This limitations comes from qgrayraster.c. Any higher and
// rasterization of shapes will produce incorrect results.
const int QT_RASTER_COORD_LIMIT = 32767;

struct QRasterFloatPoint {
    qreal x;
    qreal y;
};

static const QRectF boundingRect(const QPointF *points, int pointCount)
{
    const QPointF *e = points;
    const QPointF *last = points + pointCount;
    qreal minx, maxx, miny, maxy;
    minx = maxx = e->x();
    miny = maxy = e->y();
    while (++e < last) {
        if (e->x() < minx)
            minx = e->x();
        else if (e->x() > maxx)
            maxx = e->x();
        if (e->y() < miny)
            miny = e->y();
        else if (e->y() > maxy)
            maxy = e->y();
    }
    return QRectF(QPointF(minx, miny), QPointF(maxx, maxy));
}

/********************************************************************************
 * class QFTOutlineMapper
 *
 * Used to map between QPainterPath and the QT_FT_Outline structure used by the
 * freetype scanconvertor.
 *
 * The outline mapper uses a path iterator to get points from the path,
 * so that it is possible to transform the points as they are converted. The
 * callback can be a noop, translate or full-fledged xform. (Tests indicated
 * that using a C callback was low cost).
 */
class QFTOutlineMapper
{
public:

    /*!
      Sets up the matrix to be used for conversion. This also
      sets up the qt_path_iterator function that is used as a callback
      to get points.
    */
    void setMatrix(const QTransform &m, uint txop)
    {
        m_m11 = m.m11();
        m_m12 = m.m12();
        m_m13 = m.m13();
        m_m21 = m.m21();
        m_m22 = m.m22();
        m_m23 = m.m23();
        m_dx = m.dx();
        m_dy = m.dy();
        m_txop = txop;
    }

    void beginOutline(Qt::FillRule fillRule)
    {
#ifdef QT_DEBUG_CONVERT
        printf("QFTOutlineMapper::beginOutline rule=%d\n", fillRule);
#endif
        m_valid = true;
        m_elements.reset();
        m_elements_dev.reset();
        m_element_types.reset();
        m_points.reset();
        m_tags.reset();
        m_contours.reset();
        m_outline.flags = fillRule == Qt::WindingFill
                          ? QT_FT_OUTLINE_NONE
                          : QT_FT_OUTLINE_EVEN_ODD_FILL;
        m_subpath_start = 0;
    }

    void endOutline();

    void clipElements(const QPointF *points, const QPainterPath::ElementType *types, int count);

    void convertElements(const QPointF *points, const QPainterPath::ElementType *types, int count);

    inline void moveTo(const QPointF &pt) {
#ifdef QT_DEBUG_CONVERT
        printf("QFTOutlineMapper::moveTo() (%f, %f)\n", pt.x(), pt.y());
#endif
        closeSubpath();
        m_subpath_start = m_elements.size();
        m_elements << pt;
        m_element_types << QPainterPath::MoveToElement;
    }

    inline void lineTo(const QPointF &pt) {
#ifdef QT_DEBUG_CONVERT
        printf("QFTOutlineMapper::lineTo() (%f, %f)\n", pt.x(), pt.y());
#endif
        m_elements.add(pt);
        m_element_types << QPainterPath::LineToElement;
    }

    inline void curveTo(const QPointF &cp1, const QPointF &cp2, const QPointF &ep) {
#ifdef QT_DEBUG_CONVERT
        printf("QFTOutlineMapper::curveTo() (%f, %f)\n", ep.x(), ep.y());
#endif
        m_elements << cp1 << cp2 << ep;
        m_element_types << QPainterPath::CurveToElement
                        << QPainterPath::CurveToDataElement
                        << QPainterPath::CurveToDataElement;
    }

    inline void closeSubpath() {
        int element_count = m_elements.size();
        if (element_count > 0) {
            if (m_elements.at(element_count-1) != m_elements.at(m_subpath_start)) {
#ifdef QT_DEBUG_CONVERT
                printf(" - implicitly closing\n");
#endif
                // Put the object on the stack to avoid the odd case where
                // lineTo reallocs the databuffer and the QPointF & will
                // be invalidated.
                QPointF pt = m_elements.at(m_subpath_start);
                lineTo(pt);
            }
        }
    }

    QT_FT_Outline *outline() {
        if (m_valid)
            return &m_outline;
        return 0;
    }

    QT_FT_Outline *convertPath(const QPainterPath &path)
    {
        Q_ASSERT(!path.isEmpty());
        int elmCount = path.elementCount();
#ifdef QT_DEBUG_CONVERT
        printf("QFTOutlineMapper::convertPath(), size=%d\n", elmCount);
#endif
        beginOutline(path.fillRule());

        for (int index=0; index<elmCount; ++index) {
            const QPainterPath::Element &elm = path.elementAt(index);

            switch (elm.type) {

            case QPainterPath::MoveToElement:
                if (index == elmCount - 1)
                    continue;
                moveTo(elm);
                break;

            case QPainterPath::LineToElement:
                lineTo(elm);
                break;

            case QPainterPath::CurveToElement:
                curveTo(elm, path.elementAt(index + 1), path.elementAt(index + 2));
                index += 2;
                break;

            default:
                break; // This will never hit..
            }
        }

        endOutline();
        return outline();
    }

public:
    QDataBuffer<QPainterPath::ElementType> m_element_types;
    QDataBuffer<QPointF> m_elements;
    QDataBuffer<QPointF> m_elements_dev;
    QDataBuffer<QT_FT_Vector> m_points;
    QDataBuffer<char> m_tags;
    QDataBuffer<int> m_contours;

    QRect m_clip_rect;
    QDataBuffer<QPointF> m_polygon_dev;

    QRectF controlPointRect; // only valid after endOutline()

    QT_FT_Outline m_outline;
    uint m_txop;

    int m_subpath_start;

    // Matrix
    qreal m_m11;
    qreal m_m12;
    qreal m_m13;
    qreal m_m21;
    qreal m_m22;
    qreal m_m23;
    qreal m_dx;
    qreal m_dy;

    bool m_valid;
};

void QFTOutlineMapper::endOutline()
{
    closeSubpath();

    int element_count = m_elements.size();
    const QPointF *elements;

    // Transform the outline
    if (m_txop == QTransform::TxNone) {
        elements = m_elements.data();
    } else {
        if (m_txop == QTransform::TxTranslate) {
            for (int i=0; i<m_elements.size(); ++i) {
                const QPointF &e = m_elements.at(i);
                m_elements_dev << QPointF(e.x() + m_dx, e.y() + m_dy);
            }
        } else if (m_txop == QTransform::TxScale) {
            for (int i=0; i<m_elements.size(); ++i) {
                const QPointF &e = m_elements.at(i);
                m_elements_dev << QPointF(m_m11 * e.x() + m_dx, m_m22 * e.y() + m_dy);
            }
        } else if (m_txop < QTransform::TxProject) {
            for (int i=0; i<m_elements.size(); ++i) {
                const QPointF &e = m_elements.at(i);
                m_elements_dev << QPointF(m_m11 * e.x() + m_m21 * e.y() + m_dx,
                                          m_m22 * e.y() + m_m12 * e.x() + m_dy);
            }
        } else {
            for (int i=0; i<m_elements.size(); ++i) {
                const QPointF &e = m_elements.at(i);
                qreal x = m_m11 * e.x() + m_m21 * e.y() + m_dx;
                qreal y = m_m22 * e.y() + m_m12 * e.x() + m_dy;
                qreal w = m_m13*e.x() + m_m23*e.y() + 1.;
                w = 1/w;
                x *= w;
                y *= w;
                m_elements_dev << QPointF(x, y);
            }
        }
        elements = m_elements_dev.data();
    }

    controlPointRect = boundingRect(elements, element_count);

    // Check for out of dev bounds...
    const bool do_clip = (controlPointRect.left() < -QT_RASTER_COORD_LIMIT
                          || controlPointRect.right() > QT_RASTER_COORD_LIMIT
                          || controlPointRect.top() < -QT_RASTER_COORD_LIMIT
                          || controlPointRect.bottom() > QT_RASTER_COORD_LIMIT);

    if (do_clip) {
        clipElements(elements, m_element_types.data(), element_count);
    } else {
        convertElements(elements, m_element_types.data(), element_count);
    }
}

void QFTOutlineMapper::convertElements(const QPointF *elements,
                                       const QPainterPath::ElementType *types,
                                       int element_count)
{
    // Translate into FT coords
    const QPointF *e = elements;
    for (int i=0; i<element_count; ++i) {
        switch (*types) {
        case QPainterPath::MoveToElement:
            {
                QT_FT_Vector pt_fixed = { qreal_to_fixed_26_6(e->x()),
                                          qreal_to_fixed_26_6(e->y()) };
                if (i != 0)
                    m_contours << m_points.size() - 1;
                m_points << pt_fixed;
                m_tags <<  QT_FT_CURVE_TAG_ON;
            }
            break;

        case QPainterPath::LineToElement:
            {
                QT_FT_Vector pt_fixed = { qreal_to_fixed_26_6(e->x()),
                                          qreal_to_fixed_26_6(e->y()) };
                m_points << pt_fixed;
                m_tags << QT_FT_CURVE_TAG_ON;
            }
            break;

        case QPainterPath::CurveToElement:
            {
                QT_FT_Vector cp1_fixed = { qreal_to_fixed_26_6(e->x()),
                                           qreal_to_fixed_26_6(e->y()) };
                ++e;
                QT_FT_Vector cp2_fixed = { qreal_to_fixed_26_6((e)->x()),
                                           qreal_to_fixed_26_6((e)->y()) };
                ++e;
                QT_FT_Vector ep_fixed = { qreal_to_fixed_26_6((e)->x()),
                                          qreal_to_fixed_26_6((e)->y()) };

                m_points << cp1_fixed << cp2_fixed << ep_fixed;
                m_tags << QT_FT_CURVE_TAG_CUBIC
                       << QT_FT_CURVE_TAG_CUBIC
                       << QT_FT_CURVE_TAG_ON;

                types += 2;
                i += 2;
            }
            break;
        default:
            break;
        }
        ++types;
        ++e;
    }

    // close the very last subpath
    m_contours << m_points.size() - 1;

    m_outline.n_contours = m_contours.size();
    m_outline.n_points = m_points.size();

    m_outline.points = m_points.data();
    m_outline.tags = m_tags.data();
    m_outline.contours = m_contours.data();

#ifdef QT_DEBUG_CONVERT
    printf("QFTOutlineMapper::endOutline\n");

    printf(" - contours: %d\n", m_outline.n_contours);
    for (int i=0; i<m_outline.n_contours; ++i) {
        printf("   - %d\n", m_outline.contours[i]);
    }

    printf(" - points: %d\n", m_outline.n_points);
    for (int i=0; i<m_outline.n_points; ++i) {
        printf("   - %d -- %.2f, %.2f, (%d, %d)\n", i,
               m_outline.points[i].x / 64.0,
               m_outline.points[i].y / 64.0,
               m_outline.points[i], m_outline.points[i]);
    }
#endif
}

void QFTOutlineMapper::clipElements(const QPointF *elements,
                                    const QPainterPath::ElementType *types,
                                    int element_count)
{
    // We could save a bit of time by actually implementing them fully
    // instead of going through convenience functionallity, but since
    // this part of code hardly every used, it shouldn't matter.

    QPainterPath path;
    for (int i=0; i<element_count; ++i) {
        switch (types[i]) {
        case QPainterPath::MoveToElement:
            path.moveTo(elements[i]);
            break;

        case QPainterPath::LineToElement:
            path.lineTo(elements[i]);
            break;

        case QPainterPath::CurveToElement:
            path.cubicTo(elements[i], elements[i+1], elements[i+2]);
            i += 2;
            break;
        default:
            break;
        }
    }

    QPainterPath clipPath;
    clipPath.addRect(m_clip_rect);
    QPainterPath clippedPath = path.intersected(clipPath);
    uint old_txop = m_txop;
    m_txop = QTransform::TxNone;
    if (clippedPath.isEmpty())
        m_valid = false;
    else
        convertPath(clippedPath);
    m_txop = old_txop;
}


static void qt_ft_outline_move_to(qfixed x, qfixed y, void *data)
{
    ((QFTOutlineMapper *) data)->moveTo(QPointF(qt_fixed_to_real(x), qt_fixed_to_real(y)));
}

static void qt_ft_outline_line_to(qfixed x, qfixed y, void *data)
{
    ((QFTOutlineMapper *) data)->lineTo(QPointF(qt_fixed_to_real(x), qt_fixed_to_real(y)));
}

static void qt_ft_outline_cubic_to(qfixed c1x, qfixed c1y,
                             qfixed c2x, qfixed c2y,
                             qfixed ex, qfixed ey,
                             void *data)
{
    ((QFTOutlineMapper *) data)->curveTo(QPointF(qt_fixed_to_real(c1x), qt_fixed_to_real(c1y)),
                                         QPointF(qt_fixed_to_real(c2x), qt_fixed_to_real(c2y)),
                                         QPointF(qt_fixed_to_real(ex), qt_fixed_to_real(ey)));
}


#if !defined(QT_NO_DEBUG) && 0
static void qt_debug_path(const QPainterPath &path)
{
    const char *names[] = {
        "MoveTo     ",
        "LineTo     ",
        "CurveTo    ",
        "CurveToData"
    };

    printf("\nQPainterPath: elementCount=%d\n", path.elementCount());
    for (int i=0; i<path.elementCount(); ++i) {
        const QPainterPath::Element &e = path.elementAt(i);
        Q_ASSERT(e.type >= 0 && e.type <= QPainterPath::CurveToDataElement);
        printf(" - %3d:: %s, (%.2f, %.2f)\n", i, names[e.type], e.x, e.y);
    }
}
#endif

/*!
    \class QRasterPaintEngine
    \preliminary
    \ingroup qws
    \since 4.2

    \brief The QRasterPaintEngine class enables hardware acceleration
    of painting operations in Qt for Embedded Linux.

    Note that this functionality is only available in
    \l{Qt for Embedded Linux}.

    In \l{Qt for Embedded Linux}, painting is a pure software
    implementation. But starting with Qt 4.2, it is
    possible to add an accelerated graphics driver to take advantage
    of available hardware resources.

    Hardware acceleration is accomplished by creating a custom screen
    driver, accelerating the copying from memory to the screen, and
    implementing a custom paint engine accelerating the various
    painting operations. Then a custom paint device (derived from the
    QCustomRasterPaintDevice class) and a custom window surface
    (derived from QWSWindowSurface) must be implemented to make
    \l{Qt for Embedded Linux} aware of the accelerated driver.

    \note The QRasterPaintEngine class does not support 8-bit images.
    Instead, they need to be converted to a supported format, such as
    QImage::Format_ARGB32_Premultiplied.

    See the \l {Adding an Accelerated Graphics Driver to Qt for Embedded Linux}
    documentation for details.

    \sa QCustomRasterPaintDevice, QPaintEngine
*/

/*!
    \fn Type QRasterPaintEngine::type() const
    \reimp
*/

/*!
    \typedef QSpan
    \relates QRasterPaintEngine

    A struct equivalent to QT_FT_Span, containing a position (x,
    y), the span's length in pixels and its color/coverage (a value
    ranging from 0 to 255).
*/

/*!
    Creates a raster based paint engine with the complete set of \l
    {QPaintEngine::PaintEngineFeature}{paint engine features and
    capabilities}.
*/
QRasterPaintEngine::QRasterPaintEngine()
    : QPaintEngine(*(new QRasterPaintEnginePrivate),
                   QPaintEngine::PaintEngineFeatures(AllFeatures & ~ObjectBoundingModeGradients)
        )
{
    init();
}

/*!
    \internal
*/
QRasterPaintEngine::QRasterPaintEngine(QRasterPaintEnginePrivate &dd)
    : QPaintEngine(dd, QPaintEngine::PaintEngineFeatures(AllFeatures & ~ObjectBoundingModeGradients))
{
    init();
}

void QRasterPaintEngine::init()
{
    Q_D(QRasterPaintEngine);

    d->rasterPoolSize = 8192;
    d->rasterPoolBase =
#if defined(Q_WS_WIN64)
        // We make use of setjmp and longjmp in qgrayraster.c which requires
        // 16-byte alignment, hence we hardcode this requirement here..
        (unsigned char *) _aligned_malloc(d->rasterPoolSize, sizeof(void*) * 2);
#else
        (unsigned char *) malloc(d->rasterPoolSize);
#endif

    // The antialiasing raster.
    d->grayRaster = new QT_FT_Raster;
    qt_ft_grays_raster.raster_new(0, d->grayRaster);
    qt_ft_grays_raster.raster_reset(*d->grayRaster, d->rasterPoolBase, d->rasterPoolSize);

    // Initialize the standard raster.
    d->blackRaster = new QT_FT_Raster;
    qt_ft_standard_raster.raster_new(0, d->blackRaster);
    qt_ft_standard_raster.raster_reset(*d->blackRaster, d->rasterPoolBase, d->rasterPoolSize);

    d->rasterBuffer = new QRasterBuffer();
#ifdef Q_WS_WIN
    d->fontRasterBuffer = new QRasterBuffer();
    d->fontRasterBuffer->setTextBuffer(true);
#endif
    d->outlineMapper = new QFTOutlineMapper;

    d->dashStroker = 0;

    d->flushOnEnd = true;

    d->basicStroker.setMoveToHook(qt_ft_outline_move_to);
    d->basicStroker.setLineToHook(qt_ft_outline_line_to);
    d->basicStroker.setCubicToHook(qt_ft_outline_cubic_to);
    d->dashStroker = 0;
}

/*!
    Destroys this paint engine.
*/
QRasterPaintEngine::~QRasterPaintEngine()
{
    Q_D(QRasterPaintEngine);

#if defined(Q_WS_WIN64)
    _aligned_free(d->rasterPoolBase);
#else
    free(d->rasterPoolBase);
#endif

    qt_ft_grays_raster.raster_done(*d->grayRaster);
    delete d->grayRaster;

    qt_ft_standard_raster.raster_done(*d->blackRaster);
    delete d->blackRaster;


    delete d->rasterBuffer;
    delete d->outlineMapper;
#ifdef Q_WS_WIN
    delete d->fontRasterBuffer;
#endif

    delete d->dashStroker;
}

/*!
    \reimp
*/
bool QRasterPaintEngine::begin(QPaintDevice *device)
{
    Q_D(QRasterPaintEngine);

    // ####### move to QApp
    qInitDrawhelperAsm();
    d->deviceDepth = device->depth();
    d->antialiased = false;
    d->bilinear = false;
    d->mono_surface = false;
    d->fast_pen = true;
    d->int_xform = true;
    d->user_clip_enabled = false;
    d->opacity = 256;
    d->fast_text = true;
    d->paint_unclipped = false;
    d->tx_noshear = true;

    const QPaintDevice *originalDevice = painter()->d_ptr->original_device;
    if (originalDevice->devType() == QInternal::Widget) {
        const QWidget *widget = static_cast<const QWidget *>(originalDevice);
        if (widget->testAttribute(Qt::WA_PaintUnclipped))
            d->paint_unclipped = true;
    }

    painter()->d_func()->fillrect_func = static_cast<QPainterPrivate::FillRectBackdoor>(&QRasterPaintEngine::fastFillRect);
    d->inverseScale = qreal(1);

#if defined(Q_WS_WIN)
    d->isPlain45DegreeRotation = true;
    d->clear_type_text = false;
    QT_WA({
        UINT result;
        BOOL ok;
        ok = SystemParametersInfoW(SPI_GETFONTSMOOTHINGTYPE, 0, &result, 0);
        if (ok)
            d->clear_type_text = (result == FE_FONTSMOOTHINGCLEARTYPE);
    }, {
        UINT result;
        BOOL ok;
        ok = SystemParametersInfoA(SPI_GETFONTSMOOTHINGTYPE, 0, &result, 0);
        if (ok)
            d->clear_type_text = (result == FE_FONTSMOOTHINGCLEARTYPE);
    });
#endif

    d->rasterBuffer->init();

    d->deviceRect = QRect(0, 0, device->width(), device->height());


    gccaps &= ~PorterDuff;

    // reset paintevent clip
    d->baseClip = QPainterPath();
    if (device->devType() == QInternal::Widget) {
        if (!d->paint_unclipped) {
            QRegion sysClip = systemClip();
            if (!sysClip.isEmpty()) {
                d->baseClip.addRegion(sysClip);
                if (!static_cast<QWidget *>(device)->testAttribute(Qt::WA_PaintOnScreen)) {
                    d->deviceRect = sysClip.boundingRect();
                    // Shift the baseclip to absolute
                    d->baseClip = d->baseClip * QMatrix(1, 0, 0, 1,
                                                        -d->deviceRect.x(),
                                                        -d->deviceRect.y());
                }
            }
        }
        gccaps &= ~PaintOutsidePaintEvent;
    }
#if defined(Q_WS_QWS)
    else if (device->devType() == QInternal::Pixmap) {
        if (!d->paint_unclipped) {
            // Only embedded uses system clipping on pixmaps
            QRegion sysClip = systemClip();
            if (!sysClip.isEmpty())
                d->baseClip.addRegion(sysClip);
        }
    }
#endif
    else if (!d->paint_unclipped) {
        QRegion sysClip = systemClip();
        if (!sysClip.isEmpty()) {
            d->baseClip.addRegion(sysClip);
        }
    }

    QImage *image = 0;
    if (device->devType() == QInternal::Pixmap) {
        QPixmap *pixmap = static_cast<QPixmap *>(device);
        if (pixmap->isNull()) {
            qWarning("Cannot paint on a null pixmap");
            return false;
        }
        d->rasterBuffer->prepare(pixmap);
        image = static_cast<QPixmap*>(device)->pixmapData()->buffer();
    } else if (device->devType() == QInternal::Image) {
        image = static_cast<QImage *>(device);
        d->flushOnEnd = false;

        d->rasterBuffer->prepare(image);
#ifdef Q_WS_QWS
    } else if (device->devType() == QInternal::CustomRaster) {
        QCustomRasterPaintDevice *dev = static_cast<QCustomRasterPaintDevice*>(device);
        d->rasterBuffer->prepare(dev);
#endif
    } else {
        d->rasterBuffer->prepare(d->deviceRect.width(), d->deviceRect.height());
    }

    if (image) {
        QImage::Format format = image->format();

        switch (format) {
        case QImage::Format_MonoLSB:
        case QImage::Format_Mono:
            d->mono_surface = true;
            break;
        case QImage::Format_ARGB8565_Premultiplied:
        case QImage::Format_ARGB8555_Premultiplied:
        case QImage::Format_ARGB6666_Premultiplied:
        case QImage::Format_ARGB4444_Premultiplied:
        case QImage::Format_ARGB32_Premultiplied:
        case QImage::Format_ARGB32:
            gccaps |= PorterDuff;
            break;
        case QImage::Format_RGB32:
        case QImage::Format_RGB444:
        case QImage::Format_RGB555:
        case QImage::Format_RGB666:
        case QImage::Format_RGB888:
        case QImage::Format_RGB16:
            break;
        default:
            qWarning("QRasterPaintEngine::begin: Unsupported image format (%d)", format);
            return false;
        }
    }

    d->matrix = QTransform();
    d->txop = QTransform::TxNone;
    d->txscale = 1;

    d->outlineMapper->setMatrix(d->matrix, d->txop);
    d->outlineMapper->m_clip_rect = d->deviceRect.adjusted(-10, -10, 10, 10);
    QRect bounds(-QT_RASTER_COORD_LIMIT, -QT_RASTER_COORD_LIMIT,
                 2*QT_RASTER_COORD_LIMIT, 2*QT_RASTER_COORD_LIMIT);
    d->outlineMapper->m_clip_rect = bounds.intersected(d->outlineMapper->m_clip_rect);


    d->rasterizer.setClipRect(d->deviceRect);

    if (device->depth() == 1) {
        d->pen = QPen(Qt::color1);
        d->brush = QBrush(Qt::color0);
    } else {
        d->pen = QPen(Qt::black);
        d->brush = QBrush(Qt::NoBrush);
    }

    d->penData.init(d->rasterBuffer, this);
    d->penData.setup(d->pen.brush(), d->opacity);
    d->stroker = &d->basicStroker;
    d->basicStroker.setClipRect(d->deviceRect);

    d->brushData.init(d->rasterBuffer, this);
    d->brushData.setup(d->brush, d->opacity);

    if (!d->paint_unclipped)
        updateClipRegion(QRegion(), Qt::NoClip);

    setDirty(DirtyBrushOrigin);

    setActive(true);
    return true;
}

/*!
    \reimp
*/
bool QRasterPaintEngine::end()
{
    Q_D(QRasterPaintEngine);

    if (d->flushOnEnd)
        flush(d->pdev, QPoint());

    if (d->rasterBuffer->disabled_clip) {
        delete d->rasterBuffer->disabled_clip;
        d->rasterBuffer->disabled_clip = 0;
    }

    setActive(false);

    return true;
}

/*!
    \internal
*/
void QRasterPaintEngine::releaseBuffer()
{
    Q_D(QRasterPaintEngine);
    delete d->rasterBuffer;
    d->rasterBuffer = new QRasterBuffer;
}

/*!
    \internal
*/
QSize QRasterPaintEngine::size() const
{
    Q_D(const QRasterPaintEngine);
    return QSize(d->rasterBuffer->width(), d->rasterBuffer->height());
}

/*!
    \internal
*/
#ifndef QT_NO_DEBUG
void QRasterPaintEngine::saveBuffer(const QString &s) const
{
    Q_D(const QRasterPaintEngine);
    d->rasterBuffer->bufferImage().save(s, "PNG");
}
#endif

/*!
    \internal
*/
void QRasterPaintEngine::setFlushOnEnd(bool flushOnEnd)
{
    Q_D(QRasterPaintEngine);

    d->flushOnEnd = flushOnEnd;
}


/*!
    \internal

    Force the contents of the buffer out on the underlying device.
*/
void QRasterPaintEngine::flush(QPaintDevice *device, const QPoint &offset)
{
    Q_D(QRasterPaintEngine);
    Q_ASSERT(device);

#if defined(Q_WS_WIN)
    if (!d->rasterBuffer->hdc())
        return;

    if (device->devType() == QInternal::Widget) {
        HDC hdc = device->getDC();

        QRegion sysClip = systemClip();
        if (sysClip.isEmpty()) {
            BitBlt(hdc, d->deviceRect.x() + offset.x(), d->deviceRect.y() + offset.y(),
                   d->deviceRect.width(), d->deviceRect.height(),
                   d->rasterBuffer->hdc(), 0, 0, SRCCOPY);
        } else {
            QVector<QRect> rects = sysClip.rects();
            for (int i=0; i<rects.size(); ++i) {
                QRect r = rects.at(i);
                BitBlt(hdc,
                       r.x() + offset.x(), r.y() + offset.y(), r.width(), r.height(),
                       d->rasterBuffer->hdc(), r.x() - d->deviceRect.x(), r.y() - d->deviceRect.y(),
                       SRCCOPY);
            }
        }

        device->releaseDC(hdc);
    }
#elif defined(Q_WS_MAC)
    extern CGContextRef qt_mac_cg_context(const QPaintDevice *); //qpaintdevice_mac.cpp
    extern void qt_mac_clip_cg(CGContextRef, const QRegion &, const QPoint *, CGAffineTransform *); //qpaintengine_mac.cpp
    if(CGContextRef ctx = qt_mac_cg_context(device)) {
        qt_mac_clip_cg(ctx, systemClip(), 0, 0);
        const CGRect source = CGRectMake(0, 0, d->deviceRect.width(), d->deviceRect.height());
        QCFType<CGImageRef> subimage;
#if (MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_4)
        if (QSysInfo::MacintoshVersion >= QSysInfo::MV_10_4) {
            subimage = CGImageCreateWithImageInRect(d->rasterBuffer->m_data, source);
        } else
#endif
        {
            int dw = d->deviceRect.width();
            int dh = d->deviceRect.height();
            QCFType<CGDataProviderRef> provider = CGDataProviderCreateWithData(
                                                            d->rasterBuffer->buffer(),
                                                            d->rasterBuffer->buffer(),
                                                            dw*dh*sizeof(uint), 0);
            subimage = CGImageCreate(dw, dh, 8, 32, d->rasterBuffer->width() * sizeof(uint),
                                     QCoreGraphicsPaintEngine::macGenericColorSpace(),
                                     kCGImageAlphaPremultipliedFirst, provider, 0, 0,
                                     kCGRenderingIntentDefault);
        }
        const CGRect dest = CGRectMake(offset.x(), offset.y(),
                                       d->deviceRect.width(), d->deviceRect.height());
        HIViewDrawCGImage(ctx, &dest, subimage); //top left
        CGContextRelease(ctx);
        CGImageRelease(subimage);
    }
#else
    Q_UNUSED(d);
    Q_UNUSED(device);
    Q_UNUSED(offset);
#endif
}

/*!
    \internal
*/
void QRasterPaintEngine::updateMatrix(const QTransform &matrix)
{
    Q_D(QRasterPaintEngine);

    if (d->matrix == matrix)
        return;

    d->matrix = matrix;
    d->int_xform = false;
    d->txop = static_cast<int>(d->matrix.type());
    switch (d->txop) {
    case QTransform::TxScale:
        d->int_xform = qreal(int(d->matrix.dx())) == d->matrix.dx()
                            && qreal(int(d->matrix.dy())) == d->matrix.dy()
                            && qreal(int(d->matrix.m11())) == d->matrix.m11()
                            && qreal(int(d->matrix.m22())) == d->matrix.m22();
        break;
    case QTransform::TxTranslate:
        d->int_xform = qreal(int(d->matrix.dx())) == d->matrix.dx()
                            && qreal(int(d->matrix.dy())) == d->matrix.dy();
        break;
    case QTransform::TxNone:
        d->int_xform = true;
        break;
    }

    if (d->txop < QTransform::TxScale) {
        d->tx_noshear = true;
    } else if (d->txop < QTransform::TxRotate) {
        d->tx_noshear = qFuzzyCompare(qAbs(d->matrix.m11()), qAbs(d->matrix.m22()));
    } else if (d->txop < QTransform::TxShear) {
        const qreal xAxis = d->matrix.m11() * d->matrix.m11() +
                            d->matrix.m21() * d->matrix.m21();
        const qreal yAxis = d->matrix.m12() * d->matrix.m12() +
                            d->matrix.m22() * d->matrix.m22();

        d->tx_noshear = qFuzzyCompare(xAxis, yAxis);
    } else {
        d->tx_noshear = false;
    }

    d->txscale = d->txop > QTransform::TxTranslate ?
        qSqrt(qMax(d->matrix.m11() * d->matrix.m11()
                  + d->matrix.m21() * d->matrix.m21(),
                  d->matrix.m12() * d->matrix.m12()
                  + d->matrix.m22() * d->matrix.m22()))
        : 1.0;

    // 1/10000 == 0.0001, so we have good enough res to cover curves
    // that span the entire widget...
    d->inverseScale = d->txop <= QTransform::TxTranslate ? 1
                      : qMax(1 / qMax( qMax(qAbs(matrix.m11()), qAbs(matrix.m22())),
                                       qMax(qAbs(matrix.m12()), qAbs(matrix.m21())) ),
                             qreal(0.0001));

    d->outlineMapper->setMatrix(d->matrix, d->txop);
    d->updateMatrixData(&d->penData, d->pen.brush(), matrix);
    d->updateMatrixData(&d->brushData, d->brush, d->brushMatrix());

#ifdef Q_WS_WIN
    d->isPlain45DegreeRotation = false;
    if (d->txop >= QTransform::TxRotate) {
        d->isPlain45DegreeRotation =
            (qFuzzyCompare(matrix.m11() + 1, qreal(1))
             && qFuzzyCompare(matrix.m12(), qreal(1))
             && qFuzzyCompare(matrix.m21(), qreal(-1))
             && qFuzzyCompare(matrix.m22() + 1, qreal(1))
                )
            ||
            (qFuzzyCompare(matrix.m11(), qreal(-1))
             && qFuzzyCompare(matrix.m12() + 1, qreal(1))
             && qFuzzyCompare(matrix.m21() + 1, qreal(1))
             && qFuzzyCompare(matrix.m22(), qreal(-1))
                )
            ||
            (qFuzzyCompare(matrix.m11() + 1, qreal(1))
             && qFuzzyCompare(matrix.m12(), qreal(-1))
             && qFuzzyCompare(matrix.m21(), qreal(1))
             && qFuzzyCompare(matrix.m22() + 1, qreal(1))
                )
            ;
    }
#endif

}

/*!
    \reimp
*/
void QRasterPaintEngine::updateState(const QPaintEngineState &state)
{
    Q_D(QRasterPaintEngine);

    QPaintEngine::DirtyFlags flags = state.state();

    bool update_fast_pen = false;
    bool update_fast_text = false;

    if (flags & DirtyTransform) {
        update_fast_pen = true;
        updateMatrix(state.transform());

        // the cliprect set on the dash stroker needs to be updated when the
        // transform changes, if we have a pen that needs stroking
        Qt::PenStyle pen_style = d->pen.style();
        if (d->dashStroker && pen_style != Qt::SolidLine &&
            pen_style != Qt::NoPen && d->pen.widthF() != 0.0f) {
            QRectF clipRect = d->matrix.inverted().mapRect(QRectF(d->deviceRect));
            d->dashStroker->setClipRect(clipRect);
        }
    }

    if (flags & DirtyOpacity) {
        int newOpacity = qRound(state.opacity() * qreal(256));
        if (d->opacity != newOpacity) {
            d->opacity = newOpacity;
            if (d->opacity > 256)
                d->opacity = 256;
            if (d->opacity < 0)
                d->opacity = 0;

            // Force update pen/brush as to get proper alpha colors propagated
            if (!(flags & DirtyPen) && d->pen.style() != Qt::NoPen) {
                update_fast_text = true;
                if (d->pen.brush().style() == Qt::SolidPattern) {
                    d->penData.solid.color = PREMUL(ARGB_COMBINE_ALPHA(d->pen.brush().color().rgba(), d->opacity));
                } else {
                    d->penData.setup(d->pen.brush(), d->opacity);
                }
            }

            if (!(flags & DirtyBrush) && d->brush.style() != Qt::NoBrush) {
                if (d->brush.style() == Qt::SolidPattern) {
                    d->brushData.solid.color = PREMUL(ARGB_COMBINE_ALPHA(d->brush.color().rgba(), d->opacity));
                } else {
                    d->brushData.setup(d->brush, d->opacity);
                }
            }
        }
    }

    if ((flags & DirtyPen) && (d->pen != state.pen())) {
        update_fast_pen = true;
        update_fast_text = true;
        d->pen = state.pen();

        if (d->pen.style() == Qt::CustomDashLine && d->pen.dashPattern().size() == 0)
            d->pen.setStyle(Qt::SolidLine);

        d->basicStroker.setJoinStyle(d->pen.joinStyle());
        d->basicStroker.setCapStyle(d->pen.capStyle());
        d->basicStroker.setMiterLimit(d->pen.miterLimit());

        qreal penWidth = d->pen.widthF();
        if (penWidth == 0)
            d->basicStroker.setStrokeWidth(1);
        else
            d->basicStroker.setStrokeWidth(penWidth);

        Qt::PenStyle pen_style = d->pen.style();
        if(pen_style == Qt::SolidLine) {
            d->stroker = &d->basicStroker;
        } else if (pen_style != Qt::NoPen) {
            if (!d->dashStroker)
                d->dashStroker = new QDashStroker(&d->basicStroker);
            if (penWidth == 0) {
                d->dashStroker->setClipRect(d->deviceRect);
            } else {
                QRectF clipRect = d->matrix.inverted().mapRect(QRectF(d->deviceRect));
                d->dashStroker->setClipRect(clipRect);
            }
            d->dashStroker->setDashPattern(d->pen.dashPattern());
            d->dashStroker->setDashOffset(d->pen.dashOffset());
            d->stroker = d->dashStroker;
        } else {
            d->stroker = 0;
        }
        d->penData.setup(pen_style == Qt::NoPen ? QBrush() : d->pen.brush(), d->opacity);

        d->updateMatrixData(&d->penData, d->pen.brush(), d->matrix);
    }

    if (flags & (DirtyBrush|DirtyBrushOrigin)) {
        QBrush brush = state.brush();
        d->brush = brush;
        d->brushOffset = state.brushOrigin();
        d->brushData.setup(d->brush, d->opacity);
        d->updateMatrixData(&d->brushData, d->brush, d->brushMatrix());
    }

    if (!d->paint_unclipped && (flags & (DirtyClipPath | DirtyClipRegion))) {
        d->user_clip_enabled = true;
        // If we're setting a clip, we kill the old clip
        if (d->rasterBuffer->disabled_clip) {
            delete d->rasterBuffer->disabled_clip;
            d->rasterBuffer->disabled_clip = 0;
        }
    }

    if (d->paint_unclipped) {
    } else if (flags & DirtyClipPath) {
        if (state.clipOperation() == Qt::NoClip)
            updateClipRegion(QRegion(), Qt::NoClip);
        else
            updateClipPath(state.clipPath(), state.clipOperation());
    } else if (flags & DirtyClipRegion) {
        updateClipRegion(state.clipRegion(), state.clipOperation());
    } else if (flags & DirtyClipEnabled) {

        if (state.isClipEnabled() != d->user_clip_enabled) {
            d->user_clip_enabled = state.isClipEnabled();

            // The tricky case... When we disable clipping we still do
            // system clip so we need to rasterize the system clip and
            // replace the current clip with it. Since people might
            // choose to set clipping to true later on we have to save the
            // current one (in disabled_clip).
            if (!state.isClipEnabled()) { // save current clip for later
                Q_ASSERT(!d->rasterBuffer->disabled_clip);
                d->rasterBuffer->disabled_clip = d->rasterBuffer->clip;
                d->rasterBuffer->disabledClipRegion = d->rasterBuffer->clipRegion;
                d->rasterBuffer->disabledClipRect = d->rasterBuffer->clipRect;
                d->rasterBuffer->clip = 0;
                d->rasterBuffer->clipRegion = QRegion();
                d->rasterBuffer->clipRect = QRect();
                d->disabledClipRegion = d->clipRegion;
                updateClipRegion(QRegion(), Qt::NoClip);
            } else { // re-enable old clip
                d->rasterBuffer->resetClip();
                d->rasterBuffer->clip = d->rasterBuffer->disabled_clip;
                d->rasterBuffer->clipRegion = d->rasterBuffer->disabledClipRegion;
                d->rasterBuffer->clipRect = d->rasterBuffer->disabledClipRect;
                d->rasterBuffer->disabled_clip = 0;
                d->rasterBuffer->disabledClipRegion = QRegion();
                d->rasterBuffer->disabledClipRect = QRect();
                d->clipRegion = d->disabledClipRegion;
                d->disabledClipRegion = QRegion();
            }
            d->penData.adjustSpanMethods();
            d->brushData.adjustSpanMethods();
        }
    }

    if (!d->mono_surface) {
        if (flags & DirtyHints) {
            update_fast_pen = true;
            d->antialiased = bool(state.renderHints() & QPainter::Antialiasing);
            d->bilinear = bool(state.renderHints() & QPainter::SmoothPixmapTransform);
            // propegate state to data's
            d->brushData.bilinear = d->penData.bilinear = d->bilinear;
            d->penData.adjustSpanMethods();
            d->brushData.adjustSpanMethods();
        }

        if (flags & DirtyCompositionMode) {
            update_fast_text = true;
            d->rasterBuffer->compositionMode = state.compositionMode();
        }
    }

    if (update_fast_pen) {
        d->fast_pen = !d->antialiased
                      && (d->pen.widthF() == 0
                          || (d->pen.widthF() <= 1
                              && (d->txop <= QTransform::TxTranslate || d->pen.isCosmetic())));
    }

    if (update_fast_text) {
        const QPainter::CompositionMode mode = d->rasterBuffer->compositionMode;
        d->fast_text = d->rasterBuffer->buffer()
                       && (d->penData.type == QSpanData::Solid)
                       && (mode == QPainter::CompositionMode_Source
                           || (mode == QPainter::CompositionMode_SourceOver
                               && qAlpha(d->penData.solid.color) == 255));
    }
}

void QRasterPaintEnginePrivate::updateMatrixData(QSpanData *spanData, const QBrush &b, const QTransform &m)
{
    if (b.d->style == Qt::NoBrush || b.d->style == Qt::SolidPattern)
        return;
    if (b.d->hasTransform) {
        spanData->setupMatrix(b.transform() * m, bilinear);
    } else {
        if (txop <= QTransform::TxTranslate) {
            // specialize setupMatrix for translation matrices
            // to avoid needless matrix inversion
            spanData->m11 = 1;
            spanData->m12 = 0;
            spanData->m13 = 0;
            spanData->m21 = 0;
            spanData->m22 = 1;
            spanData->m23 = 0;
            spanData->dx = -m.dx();
            spanData->dy = -m.dy();
            spanData->txop = txop;
            spanData->bilinear = bilinear;
            spanData->fast_matrix = qAbs(m.dx()) < 1e4 && qAbs(m.dy()) < 1e4;
            spanData->adjustSpanMethods();
        } else {
            spanData->setupMatrix(m, bilinear);
        }
    }
}

/*!
    \internal
*/
void QRasterPaintEngine::updateClipRegion(const QRegion &r, Qt::ClipOperation op)
{
#ifdef QT_DEBUG_DRAW
    qDebug() << " - QRasterPaintEngine::updateClipRegion() op=" << op << r;
#endif

    Q_D(QRasterPaintEngine);

    if (d->paint_unclipped)
        return;

    if (d->txop <= QTransform::TxScale
        && (!d->rasterBuffer->clip || op == Qt::NoClip || op == Qt::ReplaceClip)) {
        switch (op) {
        case Qt::NoClip:
            d->clipRegion = d->deviceRect;
            break;
        case Qt::IntersectClip:
            d->clipRegion &= d->matrix.map(r);
            break;
        case Qt::ReplaceClip:
            d->clipRegion = d->matrix.map(r) & d->deviceRect;
            break;
        case Qt::UniteClip:
            d->clipRegion |= d->matrix.map(r);
            break;
        default:
            break;
        }

        const QRegion sysClip = systemClip();
        if (!sysClip.isEmpty())
            d->clipRegion &= sysClip;

        if (!d->clipRegion.isEmpty())
            if (d->clipRegion.numRects() == 1) {
                d->setClipRect(d->clipRegion.boundingRect());
                return;
            }
#ifndef Q_OS_WINCE
            else {
                d->setClipRegion(d->clipRegion);
                return;
            }
#endif
    }

    QPainterPath p;
    p.addRegion(r);
    updateClipPath(p, op);
}

void QRasterPaintEnginePrivate::setClipRect(const QRect &rect)
{
    rasterBuffer->resetClip();
    rasterBuffer->clipRect = rect;
    rasterBuffer->clipRegion = QRegion();
    rasterBuffer->clipEnabled = true;

    penData.adjustSpanMethods();
    brushData.adjustSpanMethods();
}

void QRasterPaintEnginePrivate::setClipRegion(const QRegion &region)
{
    rasterBuffer->resetClip();
    rasterBuffer->clipRect = QRect();
    rasterBuffer->clipRegion = region;
    rasterBuffer->clipEnabled = true;

    penData.adjustSpanMethods();
    brushData.adjustSpanMethods();
}

/*!
    \internal
*/
void QRasterPaintEngine::updateClipPath(const QPainterPath &path, Qt::ClipOperation op)
{
    Q_D(QRasterPaintEngine);
#ifdef QT_DEBUG_DRAW
    qDebug() << " - QRasterPaintEngine::updateClipPath(), op="
             << op
             << path.boundingRect();
#endif

    if (d->paint_unclipped)
        return;

    // Convert old clip if necessary
    if (d->rasterBuffer->clip) {
        // nothing
    } else if (!d->rasterBuffer->clipRect.isEmpty()) {
        if (op == Qt::UniteClip || op == Qt::IntersectClip) {
            d->rasterBuffer->clip = new QClipData(d->rasterBuffer->height());
            d->rasterBuffer->clip->setClipRect(d->rasterBuffer->clipRect);
        }
        d->rasterBuffer->clipRect = QRect();
    } else if (!d->rasterBuffer->clipRegion.isEmpty()) {
        if (op == Qt::UniteClip || op == Qt::IntersectClip) {
            d->rasterBuffer->clip = new QClipData(d->rasterBuffer->height());
            d->rasterBuffer->clip->setClipRegion(d->rasterBuffer->clipRegion);
        }
        d->rasterBuffer->clipRegion = QRegion();
    }

    d->updateClip_helper(path, op);

    // Reset the baseClip if the operation requires it.
    if (!d->baseClip.isEmpty()) {
        switch (op) {
        case Qt::UniteClip:
        case Qt::ReplaceClip:
        case Qt::NoClip:
            d->outlineMapper->setMatrix(QTransform(), QTransform::TxNone);
            d->updateClip_helper(d->baseClip, Qt::IntersectClip);
            d->outlineMapper->setMatrix(d->matrix, d->txop);
            break;
        default:
            break;
        }
    }
}

/*!
    \internal
*/
void QRasterPaintEngine::fillPath(const QPainterPath &path, QSpanData *fillData)
{
#ifdef QT_DEBUG_DRAW
    qDebug() << " --- fillPath, bounds=" << path.boundingRect();
#endif

    if (!fillData->blend)
        return;

    Q_D(QRasterPaintEngine);

    const QRectF controlPointRect = path.controlPointRect();

    ProcessSpans blend = d->getBrushFunc(d->matrix.mapRect(controlPointRect), fillData);

    const QRectF deviceRect = d->matrix.mapRect(controlPointRect);
    const bool do_clip = (deviceRect.left() < -QT_RASTER_COORD_LIMIT
                          || deviceRect.right() > QT_RASTER_COORD_LIMIT
                          || deviceRect.top() < -QT_RASTER_COORD_LIMIT
                          || deviceRect.bottom() > QT_RASTER_COORD_LIMIT);

#ifdef QT_FAST_SPANS
    if (!d->antialiased && !do_clip) {
        d->initializeRasterizer(fillData);
        d->rasterizer.rasterize(path * d->matrix, path.fillRule());
        return;
    }
#endif
    d->rasterize(d->outlineMapper->convertPath(path), blend, fillData, d->rasterBuffer);
}

static void fillRect(const QRect &r, QSpanData *data,
                     QRasterPaintEnginePrivate *pe)
{
    QRect rect = r.normalized();
    int x1 = qMax(rect.x(), 0);
    int x2 = qMin(rect.width() + rect.x(), data->rasterBuffer->width());
    int y1 = qMax(rect.y(), 0);
    int y2 = qMin(rect.height() + rect.y(), data->rasterBuffer->height());
    QClipData *clip = data->rasterBuffer->clipEnabled ? data->rasterBuffer->clip : 0;

    if (clip) {
        x1 = qMax(x1, clip->xmin);
        x2 = qMin(x2, clip->xmax);
        y1 = qMax(y1, clip->ymin);
        y2 = qMin(y2, clip->ymax);
    }

    const int width = x2 - x1;
    const int height = y2 - y1;

    rect = QRect(x1, y1, width, height);
    if (rect.isEmpty())
        return;

    if (pe && data->fillRect && pe->isUnclipped_normalized(rect)) {
        const QPainter::CompositionMode mode = pe->rasterBuffer->compositionMode;
        if (mode == QPainter::CompositionMode_Source
            || (mode == QPainter::CompositionMode_SourceOver
                && qAlpha(data->solid.color) == 255))
        {
            data->fillRect(data->rasterBuffer, x1, y1, width, height,
                           data->solid.color);
            return;
        }
    }

    ProcessSpans blend = pe ? pe->getBrushFunc(rect, data) : data->blend;

    const int nspans = 256;
    QT_FT_Span spans[nspans];

    Q_ASSERT(data->blend);
    int y = y1;
    while (y < y2) {
        int n = qMin(nspans, y2 - y);
        int i = 0;
        while (i < n) {
            spans[i].x = x1;
            spans[i].len = width;
            spans[i].y = y + i;
            spans[i].coverage = 255;
            ++i;
        }

        blend(n, spans, data);
        y += n;
    }
}

/*!
    \reimp
*/
void QRasterPaintEngine::drawRects(const QRect *rects, int rectCount)
{
#ifdef QT_DEBUG_DRAW
    qDebug(" - QRasterPaintEngine::drawRect(), rectCount=%d", rectCount);
#endif
    Q_D(QRasterPaintEngine);
    if (!d->antialiased && d->txop <= QTransform::TxTranslate) {
        int offset_x = int(d->matrix.dx());
        int offset_y = int(d->matrix.dy());

        const QRect *lastRect = rects + rectCount;

        while (rects < lastRect) {
            QRect rect = rects->normalized();
            if (d->brushData.blend) {
                QRect r = rect.translated(offset_x, offset_y);
                fillRect(r, &d->brushData, d);
            }

            if (d->penData.blend) {
                ProcessSpans brush_blend = d->brushData.blend;
                d->brushData.blend = 0;
                int left = rect.x();
                int right = rect.x() + rect.width();
                int top = rect.y();
                int bottom = rect.y() + rect.height();
                QPoint pts[] = { QPoint(left, top),
                                 QPoint(right, top),
                                 QPoint(right, bottom),
                                 QPoint(left, bottom) };
                QRasterPaintEngine::drawPolygon(pts, 4, WindingMode);
                d->brushData.blend = brush_blend;
            }

            ++rects;
        }
    } else {
        QPaintEngine::drawRects(rects, rectCount);
    }
}

/*!
    \internal
*/
void QRasterPaintEngine::fastFillRect(const QRect &rect, const QBrush &brush)
{
    Q_D(QRasterPaintEngine);
    Q_ASSERT(!d->antialiased && d->txop <= QTransform::TxTranslate);

    int offset_x = int(d->matrix.dx());
    int offset_y = int(d->matrix.dy());

    QSpanData brushData;
    brushData.init(d->rasterBuffer, this);
    brushData.setup(brush, d->opacity);
    d->updateMatrixData(&brushData, brush, d->brushMatrix());

    if (!brushData.blend)
        return;

    QRect r = rect.translated(offset_x, offset_y);

    fillRect(r, &brushData, d);
}

/*!
    \reimp
*/
void QRasterPaintEngine::drawRects(const QRectF *rects, int rectCount)
{
#ifdef QT_DEBUG_DRAW
    qDebug(" - QRasterPaintEngine::drawRect(), rectCount=%d", rectCount);
#endif
#ifdef QT_FAST_SPANS
    Q_D(QRasterPaintEngine);
    if (d->tx_noshear) {

        if (d->brushData.blend) {
            d->initializeRasterizer(&d->brushData);
            for (int i = 0; i < rectCount; ++i) {
                const QRectF &rect = rects[i].normalized();
                const QPointF a = d->matrix.map((rect.topLeft() + rect.bottomLeft()) * 0.5f);
                const QPointF b = d->matrix.map((rect.topRight() + rect.bottomRight()) * 0.5f);
                d->rasterizer.rasterizeLine(a, b, rect.height() / rect.width());
            }
        }

        if (d->penData.blend) {
            qreal width = d->pen.isCosmetic()
                          ? (d->pen.widthF() == 0 ? 1 : d->pen.widthF())
                          : d->pen.widthF() * d->txscale;

            if (!d->fast_pen && width <= 1 && d->pen.style() == Qt::SolidLine) {
                d->initializeRasterizer(&d->penData);

                for (int i = 0; i < rectCount; ++i) {
                    const QRectF &rect = rects[i].normalized();
                    const QPointF tl = d->matrix.map(rect.topLeft());
                    const QPointF tr = d->matrix.map(rect.topRight());
                    const QPointF bl = d->matrix.map(rect.bottomLeft());
                    const QPointF br = d->matrix.map(rect.bottomRight());
                    const qreal w = width / (rect.width() * d->txscale);
                    const qreal h = width / (rect.height() * d->txscale);
                    d->rasterizer.rasterizeLine(tl, tr, w); // top
                    d->rasterizer.rasterizeLine(bl, br, w); // bottom
                    d->rasterizer.rasterizeLine(bl, tl, h); // left
                    d->rasterizer.rasterizeLine(br, tr, h); // right
                }
            } else {
                ProcessSpans brush_blend = d->brushData.blend;
                d->brushData.blend = 0;
                for (int i = 0; i < rectCount; ++i) {
                    const QRectF &rf = rects[i].normalized();
                    QPointF pts[4] = { QPointF(rf.x(), rf.y()),
                                       QPointF(rf.x() + rf.width(), rf.y()),
                                       QPointF(rf.x() + rf.width(), rf.y() + rf.height()),
                                       QPointF(rf.x(), rf.y() + rf.height()) };
                    drawPolygon(pts, 4, ConvexMode);
                }
                d->brushData.blend = brush_blend;
            }
        }

        return;
    }
#endif // QT_FAST_SPANS
    for (int i=0; i<rectCount; ++i) {
        const QRectF &rf = rects[i];
        QPointF pts[4] = { QPointF(rf.x(), rf.y()),
                           QPointF(rf.x() + rf.width(), rf.y()),
                           QPointF(rf.x() + rf.width(), rf.y() + rf.height()),
                           QPointF(rf.x(), rf.y() + rf.height()) };
        drawPolygon(pts, 4, ConvexMode);
    }
}

/*!
    \reimp
*/
void QRasterPaintEngine::drawPath(const QPainterPath &path)
{
#ifdef QT_DEBUG_DRAW
    QRectF bounds = path.boundingRect();
    qDebug(" - QRasterPaintEngine::drawPath(), [%.2f, %.2f, %.2f, %.2f]",
           bounds.x(), bounds.y(), bounds.width(), bounds.height());
#endif
    if (path.isEmpty())
        return;

    Q_D(QRasterPaintEngine);

    if (d->brushData.blend) {
        d->outlineMapper->setMatrix(d->matrix, d->txop);
        fillPath(path, &d->brushData);
    }

    if (!d->penData.blend)
        return;

    {
        Q_ASSERT(d->stroker);
        d->outlineMapper->beginOutline(Qt::WindingFill);

        if (d->pen.isCosmetic()) {
            d->outlineMapper->setMatrix(QTransform(), QTransform::TxNone);
            d->stroker->strokePath(path, d->outlineMapper, d->matrix);
        } else {
            d->outlineMapper->setMatrix(d->matrix, d->txop);
            d->stroker->strokePath(path, d->outlineMapper, QTransform());
        }
        d->outlineMapper->endOutline();

        ProcessSpans blend = d->getPenFunc(d->outlineMapper->controlPointRect,
                                           &d->penData);
        d->rasterize(d->outlineMapper->outline(), blend, &d->penData, d->rasterBuffer);
        d->outlineMapper->setMatrix(d->matrix, d->txop);
    }

}

static inline bool isAbove(const QPointF *a, const QPointF *b)
{
    return a->y() < b->y();
}

static bool splitPolygon(const QPointF *points, int pointCount, QVector<QPointF> *upper, QVector<QPointF> *lower)
{
    Q_ASSERT(upper);
    Q_ASSERT(lower);

    Q_ASSERT(pointCount >= 2);

    QVector<const QPointF *> sorted;
    sorted.reserve(pointCount);

    upper->reserve(pointCount * 3 / 4);
    lower->reserve(pointCount * 3 / 4);

    for (int i = 0; i < pointCount; ++i)
        sorted << points + i;

    qSort(sorted.begin(), sorted.end(), isAbove);

    qreal splitY = sorted.at(sorted.size() / 2)->y();

    const QPointF *end = points + pointCount;
    const QPointF *last = end - 1;

    QVector<QPointF> *bin[2] = { upper, lower };

    for (const QPointF *p = points; p < end; ++p) {
        int side = p->y() < splitY;
        int lastSide = last->y() < splitY;

        if (side != lastSide) {
            if (qFuzzyCompare(p->y(), splitY)) {
                bin[!side]->append(*p);
            } else if (qFuzzyCompare(last->y(), splitY)) {
                bin[side]->append(*last);
            } else {
                QPointF delta = *p - *last;
                QPointF intersection(p->x() + delta.x() * (splitY - p->y()) / delta.y(), splitY);

                bin[0]->append(intersection);
                bin[1]->append(intersection);
            }
        }

        bin[side]->append(*p);

        last = p;
    }

    // give up if we couldn't reduce the point count
    return upper->size() < pointCount && lower->size() < pointCount;
}

/*!
  \internal
 */
void QRasterPaintEngine::fillPolygon(const QPointF *points, int pointCount, PolygonDrawMode mode)
{
    Q_D(QRasterPaintEngine);

    const int maxPoints = 0xffff;

    // max amount of points that raster engine can reliably handle
    if (pointCount > maxPoints) {
        QVector<QPointF> upper, lower;

        if (splitPolygon(points, pointCount, &upper, &lower)) {
            fillPolygon(upper.constData(), upper.size(), mode);
            fillPolygon(lower.constData(), lower.size(), mode);
        } else
            qWarning("Polygon too complex for filling.");

        return;
    }

    // Compose polygon fill..,
    d->outlineMapper->beginOutline(mode == WindingMode ? Qt::WindingFill : Qt::OddEvenFill);
    d->outlineMapper->moveTo(*points);
    const QPointF *p = points;
    const QPointF *ep = points + pointCount - 1;
    do {
        d->outlineMapper->lineTo(*(++p));
    } while (p < ep);
    d->outlineMapper->endOutline();

    // scanconvert.
    ProcessSpans brushBlend = d->getBrushFunc(d->outlineMapper->controlPointRect,
                                              &d->brushData);
    d->rasterize(d->outlineMapper->outline(), brushBlend, &d->brushData, d->rasterBuffer);
}

/*!
    \reimp
*/
void QRasterPaintEngine::drawPolygon(const QPointF *points, int pointCount, PolygonDrawMode mode)
{
    Q_D(QRasterPaintEngine);
#ifdef QT_DEBUG_DRAW
    qDebug(" - QRasterPaintEngine::drawPolygon(), pointCount=%d", pointCount);
    for (int i=0; i<pointCount; ++i)
        qDebug() << "   - " << points[i];
#endif
    Q_ASSERT(pointCount >= 2);

    // Do the fill
    if (d->brushData.blend && mode != PolylineMode)
        fillPolygon(points, pointCount, mode);

    // Do the outline...
    if (d->penData.blend) {
        bool needs_closing = mode != PolylineMode && points[0] != points[pointCount-1];

        if (d->fast_pen && d->pen.brush().isOpaque()) {
            // Use fast path for 0 width /  trivial pens.
            QRect devRect(0, 0, d->deviceRect.width(), d->deviceRect.height());

            LineDrawMode mode_for_last = (d->pen.capStyle() != Qt::FlatCap
                                          ? LineDrawIncludeLastPixel
                                          : LineDrawNormal);
            int dashOffset = int(d->pen.dashOffset());

            const QPointF offs(aliasedCoordinateDelta, aliasedCoordinateDelta);

            // Draw all the line segments.
            for (int i=1; i<pointCount; ++i) {
                QPointF lp1 = points[i-1] * d->matrix + offs;
                QPointF lp2 = points[i] * d->matrix + offs;

                const QRectF brect(lp1, lp2);
                ProcessSpans penBlend = d->getPenFunc(brect, &d->penData);
                if (d->pen.style() == Qt::SolidLine) {
                    drawLine_midpoint_i(qFloor(lp1.x()), qFloor(lp1.y()),
                                        qFloor(lp2.x()), qFloor(lp2.y()),
                                        penBlend, &d->penData,
                                        i == pointCount - 1 ? mode_for_last : LineDrawIncludeLastPixel,
                                        devRect);
                } else {
                    drawLine_midpoint_dashed_i(qFloor(lp1.x()), qFloor(lp1.y()),
                                               qFloor(lp2.x()), qFloor(lp2.y()),
                                               &d->pen,
                                               penBlend, &d->penData,
                                               i == pointCount - 1 ? mode_for_last : LineDrawIncludeLastPixel,
                                               devRect, &dashOffset);
                }
            }

            // Polygons are implicitly closed.
            if (needs_closing) {
                QPointF lp1 = points[pointCount-1] * d->matrix + offs;
                QPointF lp2 = points[0] * d->matrix + offs;

                const QRectF brect(lp1, lp2);
                ProcessSpans penBlend = d->getPenFunc(brect, &d->penData);
                if (d->pen.style() == Qt::SolidLine) {
                    drawLine_midpoint_i(qFloor(lp1.x()), qFloor(lp1.y()),
                                        qFloor(lp2.x()), qFloor(lp2.y()),
                                        penBlend, &d->penData,
                                        LineDrawIncludeLastPixel,
                                        devRect);
                } else {
                    drawLine_midpoint_dashed_i(qFloor(lp1.x()), qFloor(lp1.y()),
                                               qFloor(lp2.x()), qFloor(lp2.y()),
                                               &d->pen,
                                               penBlend, &d->penData,
                                               LineDrawIncludeLastPixel,
                                               devRect, &dashOffset);
                }
            }

        } else {
            // fallback case for complex or transformed pens.
            d->outlineMapper->beginOutline(Qt::WindingFill);
            if (d->pen.isCosmetic()) {
                d->outlineMapper->setMatrix(QTransform(),
                                            QTransform::TxNone);
                d->stroker->strokePolygon(points, pointCount, needs_closing,
                                          d->outlineMapper, d->matrix);
            } else {
                d->outlineMapper->setMatrix(d->matrix, d->txop);
                d->stroker->strokePolygon(points, pointCount, needs_closing,
                                          d->outlineMapper, QTransform());
            }
            d->outlineMapper->endOutline();

            ProcessSpans penBlend = d->getPenFunc(d->outlineMapper->controlPointRect,
                                                  &d->penData);
            d->rasterize(d->outlineMapper->outline(), penBlend, &d->penData, d->rasterBuffer);

            d->outlineMapper->setMatrix(d->matrix, d->txop);
        }
    }

}

/*!
    \reimp
*/
void QRasterPaintEngine::drawPolygon(const QPoint *points, int pointCount, PolygonDrawMode mode)
{
    Q_D(QRasterPaintEngine);
    if (!(d->int_xform && d->fast_pen)) {
        // this calls the float version
        QPaintEngine::drawPolygon(points, pointCount, mode);
        return;
    }

#ifdef QT_DEBUG_DRAW
    qDebug(" - QRasterPaintEngine::drawPolygon(), pointCount=%d", pointCount);
    for (int i=0; i<pointCount; ++i)
        qDebug() << "   - " << points[i];
#endif
    Q_ASSERT(pointCount >= 2);

    // Do the fill
    if (d->brushData.blend && mode != PolylineMode) {

        // Compose polygon fill..,
        d->outlineMapper->beginOutline(mode == WindingMode ? Qt::WindingFill : Qt::OddEvenFill);
        d->outlineMapper->moveTo(*points);
        const QPoint *p = points;
        const QPoint *ep = points + pointCount - 1;
        do {
            d->outlineMapper->lineTo(*(++p));
        } while (p < ep);
        d->outlineMapper->endOutline();

        // scanconvert.
        ProcessSpans brushBlend = d->getBrushFunc(d->outlineMapper->controlPointRect,
                                                  &d->brushData);
        d->rasterize(d->outlineMapper->outline(), brushBlend, &d->brushData, d->rasterBuffer);
    }

    // Do the outline...
    if (d->penData.blend) {

        bool needs_closing = mode != PolylineMode && points[0] != points[pointCount-1];

        QRect devRect(0, 0, d->deviceRect.width(), d->deviceRect.height());

        LineDrawMode mode_for_last = (d->pen.capStyle() != Qt::FlatCap
                                      ? LineDrawIncludeLastPixel
                                      : LineDrawNormal);

        int m11 = int(d->matrix.m11());
        int m22 = int(d->matrix.m22());
        int dx = int(d->matrix.dx());
        int dy = int(d->matrix.dy());
        int m13 = int(d->matrix.m13());
        int m23 = int(d->matrix.m23());
        bool affine = !m13 && !m23;

        int dashOffset = int(d->pen.dashOffset());

        if (affine) {
            // Draw all the line segments.
            for (int i=1; i<pointCount; ++i) {
                const QPoint lp1 = points[i-1] * d->matrix;
                const QPoint lp2 = points[i] * d->matrix;
                const QRect brect(lp1, lp2);
                ProcessSpans penBlend = d->getPenFunc(brect, &d->penData);

                if (d->pen.style() == Qt::SolidLine)
                    drawLine_midpoint_i(lp1.x(), lp1.y(),
                                        lp2.x(), lp2.y(),
                                        penBlend, &d->penData,
                                        i == pointCount - 1 ? mode_for_last : LineDrawIncludeLastPixel,
                                        devRect);
                else
                    drawLine_midpoint_dashed_i(lp1.x(), lp1.y(),
                                               lp2.x(), lp2.y(),
                                               &d->pen,
                                               penBlend, &d->penData,
                                               i == pointCount - 1 ? mode_for_last : LineDrawIncludeLastPixel,
                                               devRect, &dashOffset);

            }

            // Polygons are implicitly closed.
            if (needs_closing) {
                const QPoint lp1 = points[pointCount - 1] * d->matrix;
                const QPoint lp2 = points[0] * d->matrix;
                const QRect brect(lp1, lp2);
                ProcessSpans penBlend = d->getPenFunc(brect, &d->penData);

                if (d->pen.style() == Qt::SolidLine)
                    drawLine_midpoint_i(lp1.x(), lp1.y(),
                                        lp2.x(), lp2.y(),
                                        penBlend, &d->penData, LineDrawIncludeLastPixel,
                                        devRect);
                else
                    drawLine_midpoint_dashed_i(lp1.x(), lp1.y(),
                                               lp2.x(), lp2.y(),
                                               &d->pen,
                                               penBlend, &d->penData, LineDrawIncludeLastPixel,
                                               devRect, &dashOffset);
            }
        } else {
            // Draw all the line segments.
            for (int i=1; i<pointCount; ++i) {
                int x1 = points[i-1].x() * m11 + dx;
                int y1 = points[i-1].y() * m22 + dy;
                qreal w = m13*points[i-1].x() + m23*points[i-1].y() + 1.;
                w = 1/w;
                x1 = int(x1*w);
                y1 = int(y1*w);
                int x2 = points[i].x() * m11 + dx;
                int y2 = points[i].y() * m22 + dy;
                w = m13*points[i].x() + m23*points[i].y() + 1.;
                w = 1/w;
                x2 = int(x2*w);
                y2 = int(y2*w);

                const QRect brect(x1, y1, x2 - x1 + 1, y2 - y1 + 1);
                ProcessSpans penBlend = d->getPenFunc(brect, &d->penData);
                if (d->pen.style() == Qt::SolidLine)
                    drawLine_midpoint_i(x1, y1, x2, y2,
                                        penBlend, &d->penData,
                                        i == pointCount - 1 ? mode_for_last : LineDrawIncludeLastPixel,
                                        devRect);
                else
                    drawLine_midpoint_dashed_i(x1, y1, x2, y2,
                                               &d->pen,
                                               penBlend, &d->penData,
                                               i == pointCount - 1 ? mode_for_last : LineDrawIncludeLastPixel,
                                               devRect, &dashOffset);

            }

            int x1 = points[pointCount-1].x() * m11 + dx;
            int y1 = points[pointCount-1].y() * m22 + dy;
            qreal w = m13*points[pointCount-1].x() + m23*points[pointCount-1].y() + 1.;
            w = 1/w;
            x1 = int(x1*w);
            y1 = int(y1*w);
            int x2 = points[0].x() * m11 + dx;
            int y2 = points[0].y() * m22 + dy;
            w = m13*points[0].x() + m23*points[0].y() + 1.;
            w = 1/w;
            x2 = int(x2 * w);
            y2 = int(y2 * w);
            // Polygons are implicitly closed.

            if (needs_closing) {
                const QRect brect(x1, y1, x2 - x1 + 1, y2 - y1 + 1);
                ProcessSpans penBlend = d->getPenFunc(brect, &d->penData);
                if (d->pen.style() == Qt::SolidLine)
                    drawLine_midpoint_i(x1, y1, x2, y2,
                                        penBlend, &d->penData, LineDrawIncludeLastPixel,
                                        devRect);
                else
                    drawLine_midpoint_dashed_i(x1, y1, x2, y2,
                                               &d->pen,
                                               penBlend, &d->penData, LineDrawIncludeLastPixel,
                                               devRect, &dashOffset);
            }
        }

    }

}

/*!
    \reimp
*/
void QRasterPaintEngine::drawPixmap(const QRectF &r, const QPixmap &pixmap, const QRectF &sr)
{
#ifdef QT_DEBUG_DRAW
    qDebug() << " - QRasterPaintEngine::drawPixmap(), r=" << r << " sr=" << sr << " pixmap=" << pixmap.size() << "depth=" << pixmap.depth();
#endif

    Q_D(QRasterPaintEngine);

    if (pixmap.depth() == 1) {
        if (d->txop <= QTransform::TxTranslate
            && r.size() == sr.size()
            && r.size() == pixmap.size()) {
            d->drawBitmap(r.topLeft() + QPointF(d->matrix.dx(), d->matrix.dy()), pixmap, &d->penData);
            return;
        } else {
            drawImage(r, d->rasterBuffer->colorizeBitmap(pixmap.toImage(), d->pen.color()), sr);
        }
    } else {
#if defined(Q_WS_MAC)
        if(CGContextRef ctx = macCGContext()) {
            const CGRect dest = CGRectMake(r.x(), r.y(), r.width(), r.height());
            QCFType<CGImageRef> subimage;
#if (MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_4)
            if (QSysInfo::MacintoshVersion >= QSysInfo::MV_10_4) {
                const CGRect source = CGRectMake(sr.x(), sr.y(), sr.width(), sr.height());
                QMacPixmapData *macData = static_cast<QMacPixmapData*>(pixmap.data);
                subimage = CGImageCreateWithImageInRect(macData->cg_data, source);
            } else
#endif
            {
                int sx = qRound(sr.x());
                int sy = qRound(sr.y());
                int sw = qRound(sr.width());
                int sh = qRound(sr.height());
                QMacPixmapData *data = static_cast<QMacPixmapData*>(pixmap.data);
                quint32 *pantherData = data->pixels + (sy * pixmap.width() + sx);
                QCFType<CGDataProviderRef> provider = CGDataProviderCreateWithData(0,
                                                                   pantherData, sw*sh*sizeof(uint),
                                                                   0);
                subimage = CGImageCreate(sw, sh, 8, 32, pixmap.width() * sizeof(uint),
                                         QCoreGraphicsPaintEngine::macGenericColorSpace(),
                                         kCGImageAlphaPremultipliedFirst, provider, 0, 0,
                                         kCGRenderingIntentDefault);
            }
            HIViewDrawCGImage(ctx, &dest, subimage); //top left
        } else
#endif
            drawImage(r, pixmap.toImage(), sr);
    }

}

// assumes that rect has positive width and height
static inline const QRect toRect_normalized(const QRectF &rect)
{
    const int x = qRound(rect.x());
    const int y = qRound(rect.y());
    const int w = int(rect.width() + qreal(0.5));
    const int h = int(rect.height() + qreal(0.5));

    return QRect(x, y, w, h);
}

static inline int fast_ceil_positive(const qreal &v)
{
    const int iv = int(v);
    if (v - iv == 0)
        return iv;
    else
        return iv + 1;
}

static inline const QRect toAlignedRect_positive(const QRectF &rect)
{
    const int xmin = int(rect.x());
    const int xmax = int(fast_ceil_positive(rect.right()));
    const int ymin = int(rect.y());
    const int ymax = int(fast_ceil_positive(rect.bottom()));
    return QRect(xmin, ymin, xmax - xmin, ymax - ymin);
}

/*!
    \reimp
*/
void QRasterPaintEngine::drawImage(const QRectF &r, const QImage &img, const QRectF &sr,
                                   Qt::ImageConversionFlags)
{
#ifdef QT_DEBUG_DRAW
    qDebug() << " - QRasterPaintEngine::drawImage(), r=" << r << " sr=" << sr << " image=" << img.size() << "depth=" << img.depth();
#endif

    Q_D(QRasterPaintEngine);
    QSpanData textureData;
    textureData.init(d->rasterBuffer, this);
    textureData.type = QSpanData::Texture;
    textureData.initTexture(&img, d->opacity, QTextureData::Plain, toAlignedRect_positive(sr));

    bool stretch_sr = r.width() != sr.width() || r.height() != sr.height();

    if (d->txop > QTransform::TxTranslate || stretch_sr) {
        QTransform copy = d->matrix;
        copy.translate(r.x(), r.y());
        if (stretch_sr)
            copy.scale(r.width() / sr.width(), r.height() / sr.height());
        copy.translate(-sr.x(), -sr.y());
        textureData.setupMatrix(copy, d->bilinear);

#ifdef QT_FAST_SPANS
        if (d->tx_noshear || d->txop == QTransform::TxScale) {
            d->initializeRasterizer(&textureData);
            d->rasterizer.setAntialiased(d->antialiased || d->bilinear);

            const QRectF &rect = r.normalized();
            const QPointF a = d->matrix.map((rect.topLeft() + rect.bottomLeft()) * 0.5f);
            const QPointF b = d->matrix.map((rect.topRight() + rect.bottomRight()) * 0.5f);
            if (d->tx_noshear)
                d->rasterizer.rasterizeLine(a, b, rect.height() / rect.width());
            else
                d->rasterizer.rasterizeLine(a, b, qAbs((d->matrix.m22() * rect.height()) / (d->matrix.m11() * rect.width())));
            return;
        }
#endif
        bool wasAntialiased = d->antialiased;
        if (!d->antialiased)
            d->antialiased = d->bilinear;
        QPainterPath path;
        path.addRect(r);
        fillPath(path, &textureData);
        d->antialiased = wasAntialiased;
    } else {
        textureData.dx = -(r.x() + d->matrix.dx()) + sr.x();
        textureData.dy = -(r.y() + d->matrix.dy()) + sr.y();

        QRectF rr = r;
        rr.translate(d->matrix.dx(), d->matrix.dy());
        fillRect(toRect_normalized(rr), &textureData, d);
    }
}

/*!
    \reimp
*/
void QRasterPaintEngine::drawTiledPixmap(const QRectF &r, const QPixmap &pixmap, const QPointF &sr)
{
#ifdef QT_DEBUG_DRAW
    qDebug() << " - QRasterPaintEngine::drawTiledPixmap(), r=" << r << "pixmap=" << pixmap.size();
#endif
    Q_D(QRasterPaintEngine);

    QImage image;
    if (pixmap.depth() == 1)
        image = d->rasterBuffer->colorizeBitmap(pixmap.toImage(), d->pen.color());
    else
        image = pixmap.toImage();

    QSpanData textureData;
    textureData.init(d->rasterBuffer, this);
    textureData.type = QSpanData::Texture;
    textureData.initTexture(&image, d->opacity, QTextureData::Tiled);

    if (d->txop > QTransform::TxTranslate) {
        QTransform copy = d->matrix;
        copy.translate(r.x(), r.y());
        copy.translate(-sr.x(), -sr.y());
        textureData.setupMatrix(copy, d->bilinear);

#ifdef QT_FAST_SPANS
        if (d->tx_noshear || d->txop == QTransform::TxScale) {
            d->initializeRasterizer(&textureData);
            d->rasterizer.setAntialiased(d->antialiased || d->bilinear);

            const QRectF &rect = r.normalized();
            const QPointF a = d->matrix.map((rect.topLeft() + rect.bottomLeft()) * 0.5f);
            const QPointF b = d->matrix.map((rect.topRight() + rect.bottomRight()) * 0.5f);
            if (d->tx_noshear)
                d->rasterizer.rasterizeLine(a, b, rect.height() / rect.width());
            else
                d->rasterizer.rasterizeLine(a, b, qAbs((d->matrix.m22() * rect.height()) / (d->matrix.m11() * rect.width())));
            return;
        }
#endif
        bool wasAntialiased = d->antialiased;
        if (!d->antialiased)
            d->antialiased = d->bilinear;
        QPainterPath path;
        path.addRect(r);
        fillPath(path, &textureData);
        d->antialiased = wasAntialiased;
    } else {
        textureData.dx = -(r.x() + d->matrix.dx()) + sr.x();
        textureData.dy = -(r.y() + d->matrix.dy()) + sr.y();

        QRectF rr = r;
        rr.translate(d->matrix.dx(), d->matrix.dy());
        fillRect(rr.toRect(), &textureData, d);
    }
}


//QWS hack
static inline bool monoVal(const uchar* s, int x)
{
    return  (s[x>>3] << (x&7)) & 0x80;
}

/*!
    \internal
*/
void QRasterPaintEngine::alphaPenBlt(const void* src, int bpl, bool mono, int rx,int ry,int w,int h)
{
    Q_D(QRasterPaintEngine);

    if (!d->penData.blend)
        return;

    QRasterBuffer *rb = d->rasterBuffer;

    const QRect rect(rx, ry, w, h);
    const QClipData *clip = rb->clip;
    bool unclipped = false;
    if (clip) {
        // inlined QRect::intersects
        const bool intersects = qMax(clip->xmin, rect.left()) <= qMin(clip->xmax - 1, rect.right())
                                && qMax(clip->ymin, rect.top()) <= qMin(clip->ymax - 1, rect.bottom());
        if (!intersects)
            return;
    } else {
        // inlined QRect::intersects
        const bool intersects = qMax(0, rect.left()) <= qMin(rb->width() - 1, rect.right())
                                && qMax(0, rect.top()) <= qMin(rb->height() - 1, rect.bottom());
        if (!intersects)
            return;

        // inlined QRect::contains
        const bool contains = rect.left() >= 0 && rect.right() < rb->width()
                              && rect.top() >= 0 && rect.bottom() < rb->height();

        unclipped = contains && d->isUnclipped_normalized(rect);
    }

    ProcessSpans blend = unclipped ? d->penData.unclipped_blend : d->penData.blend;
    const uchar * scanline = static_cast<const uchar *>(src);

    if (unclipped && d->fast_text) {
        if (mono) {
            if (d->penData.bitmapBlit) {
                d->penData.bitmapBlit(rb, rx, ry, d->penData.solid.color,
                                      scanline, w, h, bpl);
                return;
            }
        } else {
            if (d->penData.alphamapBlit) {
                d->penData.alphamapBlit(rb, rx, ry, d->penData.solid.color,
                                        scanline, w, h, bpl);
                return;
            }
        }
    }

    int x0 = 0;
    if (rx < 0) {
        x0 = -rx;
        w -= x0;
    }

    int y0 = 0;
    if (ry < 0) {
        y0 = -ry;
        scanline += bpl * y0;
        h -= y0;
    }

    w = qMin(w, rb->width() - qMax(0, rx));
    h = qMin(h, rb->height() - qMax(0, ry));

    if (w <= 0 || h <= 0)
        return;

    const int NSPANS = 256;
    QSpan spans[NSPANS];
    int current = 0;

    const int x1 = x0 + w;
    const int y1 = y0 + h;

    if (mono) {
        for (int y = y0; y < y1; ++y) {
            for (int x = x0; x < x1; ) {
                if (!monoVal(scanline, x)) {
                    ++x;
                    continue;
                }

                if (current == NSPANS) {
                    blend(current, spans, &d->penData);
                    current = 0;
                }
                spans[current].x = x + rx;
                spans[current].y = y + ry;
                spans[current].coverage = 255;
                int len = 1;
                ++x;
                // extend span until we find a different one.
                while (x < x1 && monoVal(scanline, x)) {
                    ++x;
                    ++len;
                }
                spans[current].len = len;
                ++current;
            }
            scanline += bpl;
        }
    } else {
        for (int y = y0; y < y1; ++y) {
            for (int x = x0; x < x1; ) {
                // Skip those with 0 coverage
                if (scanline[x] == 0) {
                    ++x;
                    continue;
                }

                if (current == NSPANS) {
                    blend(current, spans, &d->penData);
                    current = 0;
                }
                int coverage = scanline[x];
                spans[current].x = x + rx;
                spans[current].y = y + ry;
                spans[current].coverage = coverage;
                int len = 1;
                ++x;

                // extend span until we find a different one.
                while (x < x1 && scanline[x] == coverage) {
                    ++x;
                    ++len;
                }
                spans[current].len = len;
                ++current;
            }
            scanline += bpl;
        }
    }
//     qDebug() << "alphaPenBlt: num spans=" << current
//              << "span:" << spans->x << spans->y << spans->len << spans->coverage;
        // Call span func for current set of spans.
    if (current != 0)
        blend(current, spans, &d->penData);
}

#ifdef Q_WS_QWS
/*!
    \internal

     Fills a rect with the current pen
*/
void QRasterPaintEngine::qwsFillRect(int x, int y, int w, int h)
{
    Q_D(QRasterPaintEngine);
    int x1 = qMax(x,0);
    int x2 = qMin(x+w, d->rasterBuffer->width());
    int y1 = qMax(y, 0);
    int y2 = qMin(y+h, d->rasterBuffer->height());;

    int len = x2 - x1;

    if (d->penData.blend && len > 0) {
        QT_FT_Span span;
        span.x = x1;
        span.len = x2 - x1;
        span.y = y;
        span.coverage = 255;

        // draw the fill
        for (int y=y1; y<y2; ++y) {
            // hw: why isn't span.y adjusted???
            d->penData.blend(1, &span, &d->penData);
        }
    }
}
#endif

#if defined(Q_WS_WIN)
bool QRasterPaintEngine::drawTextInFontBuffer(const QRect &devRect, int xmin, int ymin, int xmax,
                                              int ymax, const QTextItem &textItem, bool clearType,
                                              qreal leftBearingReserve, const QPointF &topLeft)
{
    Q_D(QRasterPaintEngine);
    const QTextItemInt &ti = static_cast<const QTextItemInt &>(textItem);

    if (d->mono_surface) {
        // Some extra work to get proper rasterization of text on monochrome targets
        HDC displayDC = GetDC(0);
        HDC hdc = CreateCompatibleDC(displayDC);
        ReleaseDC(0, displayDC);

        HBITMAP bitmap = CreateBitmap(devRect.width(), devRect.height(), 1, 1, 0);
        HGDIOBJ null_bitmap = SelectObject(hdc, bitmap);
        SelectObject(hdc, GetStockObject(WHITE_BRUSH));
        SelectObject(hdc, GetStockObject(NULL_PEN));
        Rectangle(hdc, 0, 0, devRect.width() + 1, devRect.height() + 1);

        // Fill buffer with stuff
        SelectObject(hdc, GetStockObject(BLACK_BRUSH));
        SelectObject(hdc, GetStockObject(BLACK_PEN));
        SetTextColor(hdc, RGB(0,0,0));
        qt_draw_text_item(QPointF(leftBearingReserve, ti.ascent.toReal()), ti, hdc,
                          false, QTransform(d->matrix.m11(), d->matrix.m12(),
                                            d->matrix.m21(), d->matrix.m22(),
                                            0, 0), topLeft);

        BitBlt(d->fontRasterBuffer->hdc(), 0, 0, devRect.width(), devRect.height(),
               hdc, 0, 0, SRCCOPY);
        SelectObject(hdc, null_bitmap);
        DeleteObject(bitmap);
        DeleteDC(hdc);

        return false;
    } else {
        // Let Windows handle the composition of background and foreground for cleartype text
        QRgb penColor = 0;
        bool brokenRasterBufferAlpha = false;
        if (clearType) {
            penColor = d->penData.solid.color;

            // Copy background from raster buffer
            for (int y=ymin; y<ymax; ++y) {
                QRgb *sourceScanline = (QRgb *) d->rasterBuffer->scanLine(y);
                QRgb *destScanline = (QRgb *) d->fontRasterBuffer->scanLine(y - devRect.y());
                for (int x=xmin; x<xmax; ++x) {
                    uint color = sourceScanline[x];

                    // If the alpha channel of the raster buffer has previously been
                    // broken by GDI, we assume there are valid colors there and
                    // use clear type anyway (this is for XP style)
                    int alpha = qAlpha(color);
                    if (d->rasterBuffer->format == QImage::Format_ARGB32_Premultiplied
                        && (qRed(color) > alpha || qGreen(color) > alpha || qBlue(color) > alpha)) {
                        brokenRasterBufferAlpha = true;
                    }

                    // If the background is transparent, set it to completely opaque so we will
                    // recognize it after Windows screws up the alpha channel of font buffer.
                    // Otherwise, just copy the contents.
                    if (alpha == 0x00)
                        destScanline[x - devRect.x()] = color | 0xff000000;
                    else
                        destScanline[x - devRect.x()] = color;
                }
            }
        } else {
            uint startX = xmin - devRect.x();
            uint startY = ymin - devRect.y();
            uint width = xmax - xmin;
            uint height = ymax - ymin;

            if (startX == 0
                && startY == 0
                && width == d->fontRasterBuffer->width()
                && height == d->fontRasterBuffer->height()) {
                d->fontRasterBuffer->resetBuffer(255);
            } else {
                uint bpp = d->fontRasterBuffer->bytesPerPixel();
                uchar *span = d->fontRasterBuffer->scanLine(startY) + startX * bpp;
                for (int y = ymin; y < ymax; ++y) {
                    memset(span, 255, width * bpp);
                    span += d->fontRasterBuffer->bytesPerLine();
                }
            }
        }

        // Draw the text item
        if (clearType) {
            COLORREF cf = RGB(qRed(penColor), qGreen(penColor), qBlue(penColor));
            SelectObject(d->fontRasterBuffer->hdc(), CreateSolidBrush(cf));
            SelectObject(d->fontRasterBuffer->hdc(), CreatePen(PS_SOLID, 1, cf));
            SetTextColor(d->fontRasterBuffer->hdc(), cf);
        }


        qt_draw_text_item(QPointF(leftBearingReserve, ti.ascent.toReal()), ti,
                          d->fontRasterBuffer->hdc(), false,
                          QTransform(d->matrix.m11(), d->matrix.m12(), d->matrix.m21(),
                                     d->matrix.m22(), 0, 0), topLeft);

        if (clearType) {
            DeleteObject(SelectObject(d->fontRasterBuffer->hdc(),GetStockObject(NULL_BRUSH)));
            DeleteObject(SelectObject(d->fontRasterBuffer->hdc(),GetStockObject(BLACK_PEN)));
            SetTextColor(d->fontRasterBuffer->hdc(), RGB(0, 0, 0));
        }

        // Clean up alpha channel
        if (clearType) {
            for (int y=ymin; y<ymax; ++y) {
                QRgb *scanline = (QRgb *) d->fontRasterBuffer->scanLine(y - devRect.y());
                QRgb *rbScanline = (QRgb *) d->rasterBuffer->scanLine(y);
                for (int x=xmin; x<xmax; ++x) {
                    // If alpha is 0, then Windows has drawn text on top of the pixel, so set
                    // the pixel to opaque. Otherwise, Windows has not touched the pixel, so
                    // we can set it to transparent so the background shines through instead.
                    switch (qAlpha(scanline[x - devRect.x()])) {
                    case 0x0:
                        // Special case: If Windows has drawn on top of a translucent pixel, then
                        // we bail out. This is an attempt at avoiding the problem where Windows
                        // has no background to use for composition, but also minimizing the
                        // number of cases hit by the fall back.
                        //
                        // This test could be done in the loop above, but because the bounding
                        // box is very large (probably too big, because of kerning & italics)
                        // we need to check only the "touched" pixels. If the kerning and
                        // italics problems where fixed this code could be simplfied and
                        // sped up by moving this check to the upper loop...
                        if (!brokenRasterBufferAlpha && qAlpha(rbScanline[x]) < 255) {
                            return drawTextInFontBuffer(devRect, xmin, ymin, xmax, ymax, textItem,
                                false, leftBearingReserve, topLeft);
                        }
                        scanline[x - devRect.x()] |= 0xff000000;
                        break ;
                    default: scanline[x - devRect.x()] = 0x0; break ;
                    };
                }
            }
        }
    }

    return clearType;
}
#endif // Q_WS_WIN

bool QRasterPaintEnginePrivate::isUnclipped_normalized(const QRect &r) const
{
    if (paint_unclipped)
        return true;

    if (rasterBuffer->clip)
        return false;

    // currently all painting functions clips to deviceRect internally
    if (rasterBuffer->clipRect == deviceRect)
        return true;

    if (!rasterBuffer->clipRect.isEmpty()) {
        // return rasterBuffer->clipRect.contains(r);

        // inline for performance (we know the rects are normalized)
        const QRect &r1 = rasterBuffer->clipRect;
        return (r.left() >= r1.left() && r.right() <= r1.right()
                && r.top() >= r1.top() && r.bottom() <= r1.bottom());
    } else {
        return qt_region_strictContains(clipRegion, r);
    }
}

bool QRasterPaintEnginePrivate::isUnclipped(const QRect &rect,
                                            int penWidth) const
{
    if (paint_unclipped)
        return true;

    if (rasterBuffer->clip)
        return false;

    // currently all painting functions that call this function clip to deviceRect internally
    if (rasterBuffer->clipRect == deviceRect)
        return true;

    if (antialiased)
        ++penWidth;

    QRect r = rect.normalized();
    if (penWidth > 0) {
        r.setX(r.x() - penWidth);
        r.setY(r.y() - penWidth);
        r.setWidth(r.width() + 2 * penWidth);
        r.setHeight(r.height() + 2 * penWidth);
    }

    if (!rasterBuffer->clipRect.isEmpty()) {
        // return rasterBuffer->clipRect.contains(r);

        // inline for performance (we know the rects are normalized)
        const QRect &r1 = rasterBuffer->clipRect;
        return (r.left() >= r1.left() && r.right() <= r1.right()
                && r.top() >= r1.top() && r.bottom() <= r1.bottom());
    } else {
        return qt_region_strictContains(clipRegion, r);
    }
}

inline bool QRasterPaintEnginePrivate::isUnclipped(const QRectF &rect,
                                                   int penWidth) const
{
    const QRectF rn = rect.normalized();
    const QRect r(qFloor(rn.x()), qFloor(rn.y()),
                  qCeil(rn.width()), qCeil(rn.height()));
    return isUnclipped(r, penWidth);
}

inline ProcessSpans
QRasterPaintEnginePrivate::getBrushFunc(const QRect &rect,
                                        const QSpanData *data) const
{
    return isUnclipped(rect, 0) ? data->unclipped_blend : data->blend;
}

inline ProcessSpans
QRasterPaintEnginePrivate::getBrushFunc(const QRectF &rect,
                                        const QSpanData *data) const
{
    return isUnclipped(rect, 0) ? data->unclipped_blend : data->blend;
}

inline ProcessSpans
QRasterPaintEnginePrivate::getPenFunc(const QRect &rect,
                                      const QSpanData *data) const
{
    if (!fast_pen && txop > QTransform::TxTranslate)
        return data->blend;
    const int penWidth = fast_pen ? 1 : qCeil(pen.widthF());
    return isUnclipped(rect, penWidth) ? data->unclipped_blend : data->blend;
}

inline ProcessSpans
QRasterPaintEnginePrivate::getPenFunc(const QRectF &rect,
                                      const QSpanData *data) const
{
    if (!fast_pen && txop > QTransform::TxTranslate)
        return data->blend;
    const int penWidth = fast_pen ? 1 : qCeil(pen.widthF());
    return isUnclipped(rect, penWidth) ? data->unclipped_blend : data->blend;
}

/*!
    \reimp
*/
void QRasterPaintEngine::drawTextItem(const QPointF &p, const QTextItem &textItem)
{
    const QTextItemInt &ti = static_cast<const QTextItemInt &>(textItem);

#ifdef QT_DEBUG_DRAW
    printf(" - QRasterPaintEngine::drawTextItem(), (%.2f,%.2f), string=%s\n",
           p.x(), p.y(), QString::fromRawData(ti.chars, ti.num_chars).toLatin1().data());
#endif
    Q_D(QRasterPaintEngine);

#if defined(Q_WS_WIN)

    if (!d->penData.blend)
        return;

    // Only support cleartype for solid pens, 32 bit target buffers and when the pen color is
    // opaque
    bool clearType = d->clear_type_text
                     && d->penData.type == QSpanData::Solid
                     && d->deviceDepth == 32
                     && d->fontRasterBuffer->format != QImage::Format_RGB16
                     &&  qAlpha(d->penData.solid.color) == 255;

    bool usefallback = false;

    QFontInfo fi(textItem.font());

#ifdef Q_OS_WINCE
    usefallback = (d->txop >= QTransform::TxScale);
#else
    usefallback = ((d->txop >= QTransform::TxRotate) && fi.pointSize() < 14
                   && (painter()->renderHints() & QPainter::TextAntialiasing) && !clearType
                   && !d->isPlain45DegreeRotation)
                  || d->txop >= QTransform::TxProject;
#endif

    if (QT_WA_INLINE(false, d->txop >= QTransform::TxScale)
        || (d->txscale * fi.pointSize() > 64) || usefallback)
    {
        QPaintEngine::drawTextItem(p, textItem);
        return;
    }

    QFixed x_buffering = ti.ascent;

    // Hack to reserve some space on the left side of the string in case
    // the character has a large negative bearing (e.g. it should be drawn on top
    // of the previous character)
    qreal leftBearingReserve = ti.fontEngine->maxCharWidth();
    qreal bufferWidth = (ti.width + x_buffering).toReal() + leftBearingReserve;
    qreal bufferHeight = (ti.ascent + ti.descent + 1).toReal();

    QTransform m(d->matrix.m11(), d->matrix.m12(), d->matrix.m13(),
                 d->matrix.m21(), d->matrix.m22(), d->matrix.m23(),
                 0, 0, 1);
    QRectF logRect(0, 0, bufferWidth, bufferHeight);
    QPointF topLeft = m.mapRect(logRect).topLeft();

    logRect.moveTo(p.x() - leftBearingReserve, p.y() - ti.ascent.toReal());
    QRect devRect = d->matrix.mapRect(logRect).toRect();
    if(devRect.width() == 0 || devRect.height() == 0)
        return;

    d->fontRasterBuffer->prepare(devRect.width(), devRect.height());

    // Boundaries
    int ymax = qMin(devRect.y() + devRect.height(), d->rasterBuffer->height());
    int ymin = qMax(devRect.y(), 0);
    int xmax = qMin(devRect.x() + devRect.width(), d->rasterBuffer->width());
    int xmin = qMax(devRect.x(), 0);

    QClipData *clip = d->rasterBuffer->clipEnabled ? d->rasterBuffer->clip : 0;
    if (clip) {
        xmin = qMax(xmin, clip->xmin);
        xmax = qMin(xmax, clip->xmax);
        ymin = qMax(ymin, clip->ymin);
        ymax = qMin(ymax, clip->ymax);
    }

    if (xmax - xmin <= 0 || ymax - ymin <= 0)
        return;

    // Fill the font raster buffer with text
    clearType = drawTextInFontBuffer(devRect, xmin, ymin, xmax, ymax, textItem,
                                     clearType, leftBearingReserve,
                                     topLeft);

    const int NSPANS = 256;
    QSpan spans[NSPANS];
    int current = 0;

    if (d->mono_surface) {
        for (int y=ymin; y<ymax; ++y) {
            if (d->fontRasterBuffer->format != QImage::Format_RGB16) {
                QRgb *scanline = (QRgb *) d->fontRasterBuffer->scanLine(y - devRect.y()) - devRect.x();
                // Generate spans for this y coord
                for (int x = xmin; x<xmax; ) {
                    // Skip those with 0 coverage (black on white so inverted)
                    while (x < xmax && qBlue(scanline[x]) > 0x80) ++x;
                    if (x >= xmax) break;

                    QT_FT_Span span = { x, 0, y, 255 };

                    // extend span until we find a different one.
                    while (x < xmax && qBlue(scanline[x]) < 0x80) ++x;
                    span.len = x - span.x;
                    if (current == NSPANS) {
                        d->penData.blend(current, spans, &d->penData);
                        current = 0;
                    }
                    spans[current++] = span;
                }
            } else {
                quint16 *scanline = (quint16 *) d->fontRasterBuffer->scanLine(y - devRect.y()) - devRect.x();
                // Generate spans for this y coord
                for (int x = xmin; x<xmax; ) {
                    // Skip those with 0 coverage (black on white so inverted)
                    while (x < xmax && qBlue565(scanline[x]) > 0x80) ++x;
                    if (x >= xmax) break;

                    QT_FT_Span span = { x, 0, y, 255 };

                    // extend span until we find a different one.
                    while (x < xmax && qBlue565(scanline[x]) < 0x80) ++x;
                    span.len = x - span.x;
                    if (current == NSPANS) {
                        d->penData.blend(current, spans, &d->penData);
                        current = 0;
                    }
                    spans[current++] = span;
                }

            }
        }
    } else if (clearType) {
        QSpanData data;
        data.init(d->rasterBuffer, this);
        data.type = QSpanData::Texture;
        data.texture.type = QTextureData::Plain;
        data.texture.imageData = d->fontRasterBuffer->buffer();
        data.texture.width = d->fontRasterBuffer->bytesPerLine() / 4;
        data.texture.height = d->fontRasterBuffer->height();
        data.texture.x1 = 0;
        data.texture.y1 = 0;
        data.texture.x2 = data.texture.width;
        data.texture.y2 = data.texture.height;
        data.texture.bytesPerLine = d->fontRasterBuffer->bytesPerLine();
        data.texture.hasAlpha = true;
        data.bilinear = false;
        data.texture.const_alpha = 255;
        data.texture.format = QImage::Format_ARGB32_Premultiplied;
        data.texture.colorTable = 0;

        data.dx = -devRect.x();
        data.dy = -devRect.y();
        data.adjustSpanMethods();
        fillRect(QRect(xmin, ymin, xmax - xmin, ymax - ymin), &data, d);
    } else if (d->clear_type_text && d->fontRasterBuffer->format != QImage::Format_RGB16) {
        for (int y=ymin; y<ymax; ++y) {
            QRgb *scanline = (QRgb *) d->fontRasterBuffer->scanLine(y - devRect.y()) - devRect.x();
            // Generate spans for this y coord
            for (int x = xmin; x<xmax; ) {
                // Skip those with 0 coverage (black on white so inverted)
                while (x < xmax && qGray(scanline[x]) == 255) ++x;
                if (x >= xmax) break;

                int prev = qGray(scanline[x]);
                QT_FT_Span span = { x, 0, y, 255 - prev };

                // extend span until we find a different one.
                while (x < xmax && qGray(scanline[x]) == prev) ++x;
                span.len = x - span.x;

                if (current == NSPANS) {
                    d->penData.blend(current, spans, &d->penData);
                    current = 0;
                }
                spans[current++] = span;
            }
        }
    } else {
        // For the noncleartype/grayscale text we can look at only one color component,
        // and save a bit of qGray effort...
        for (int y=ymin; y<ymax; ++y) {
            if (d->fontRasterBuffer->format != QImage::Format_RGB16) {
                QRgb *scanline = (QRgb *) d->fontRasterBuffer->scanLine(y - devRect.y()) - devRect.x();
                // Generate spans for this y coord
                for (int x = xmin; x<xmax; ) {
                    // Skip those with 0 coverage (black on white so inverted)
                    while (x < xmax && qBlue(scanline[x]) == 255) ++x;
                    if (x >= xmax) break;

                    int prev = qBlue(scanline[x]);
                    QT_FT_Span span = { x, 0, y, 255 - prev };

                    // extend span until we find a different one.
                    while (x < xmax && qBlue(scanline[x]) == prev) ++x;
                    span.len = x - span.x;

                    if (current == NSPANS) {
                        d->penData.blend(current, spans, &d->penData);
                        current = 0;
                    }
                    spans[current++] = span;
                }
            } else {
                quint16 *scanline = (quint16 *) d->fontRasterBuffer->scanLine(y - devRect.y()) - devRect.x();
                // Generate spans for this y coord
                for (int x = xmin; x<xmax; ) {
                    // Skip those with 0 coverage (black on white so inverted)
                    while (x < xmax && qBlue565(scanline[x]) == 255) ++x;
                    if (x >= xmax) break;

                    int prev = qBlue565(scanline[x]);
                    QT_FT_Span span = { x, 0, y, 255 - prev };

                    // extend span until we find a different one.
                    while (x < xmax && qBlue565(scanline[x]) == prev) ++x;
                    span.len = x - span.x;

                    if (current == NSPANS) {
                        d->penData.blend(current, spans, &d->penData);
                        current = 0;
                    }
                    spans[current++] = span;
                }
            }
        }
    }

    if (current != 0)
        d->penData.blend(current, spans, &d->penData);

    return;

#else

    QFontEngine *fontEngine = ti.fontEngine;
#if defined(Q_WS_QWS)
    if (fontEngine->type() == QFontEngine::Box) {
        fontEngine->draw(this, qFloor(p.x() + aliasedCoordinateDelta), qFloor(p.y() + aliasedCoordinateDelta), ti);
        return;
    }

    if (d->txop < QTransform::TxScale
        && (fontEngine->type() == QFontEngine::QPF1 || fontEngine->type() == QFontEngine::QPF2
            || (fontEngine->type() == QFontEngine::Proxy
                && !(static_cast<QProxyFontEngine *>(fontEngine)->drawAsOutline()))
            )) {
        fontEngine->draw(this, qFloor(p.x() + aliasedCoordinateDelta), qFloor(p.y() + aliasedCoordinateDelta), ti);
        return;
    }
#endif // Q_WS_QWS

#if (defined(Q_WS_X11) || defined(Q_WS_QWS)) && !defined(QT_NO_FREETYPE)

#if defined(Q_WS_QWS) && !defined(QT_NO_QWS_QPF2)
    if (fontEngine->type() == QFontEngine::QPF2) {
        QFontEngine *renderingEngine = static_cast<QFontEngineQPF *>(fontEngine)->renderingEngine();
        if (renderingEngine)
            fontEngine = renderingEngine;
    }
#endif

    if (fontEngine->type() == QFontEngine::Freetype
        && !static_cast<QFontEngineFT *>(fontEngine)->drawAsOutline()) {

        QFontEngineFT *fe = static_cast<QFontEngineFT *>(fontEngine);

        QTransform matrix = d->matrix;
        matrix.translate(p.x(), p.y());

        QVarLengthArray<QFixedPoint> positions;
        QVarLengthArray<glyph_t> glyphs;
        fe->getGlyphPositions(ti.glyphs, ti.num_glyphs, matrix, ti.flags, glyphs, positions);
        if (glyphs.size() == 0)
            return;

        QFontEngineFT::GlyphFormat neededFormat = QFontEngineFT::Format_A8;
        if (d->mono_surface
            || fe->isBitmapFont() // alphaPenBlt can handle mono, too
            )
            neededFormat = QFontEngineFT::Format_Mono;

        QFontEngineFT::QGlyphSet *gset = fe->defaultGlyphs();
        if (d->txop >= QTransform::TxScale) {
            if (d->matrix.isAffine())
                gset = fe->loadTransformedGlyphSet(glyphs.data(), glyphs.size(), d->matrix, neededFormat);
            else
                gset = 0;

            if (!gset) {
                QPaintEngine::drawTextItem(p, ti);
                return;
            }
        }

        QFixed offs = QFixed::fromReal(aliasedCoordinateDelta);
        FT_Face lockedFace = 0;
        for(int i = 0; i < glyphs.size(); i++) {
            QFontEngineFT::Glyph *glyph = gset->glyph_data.value(glyphs[i]);

            if (!glyph || glyph->format != neededFormat) {
                if (!lockedFace)
                    lockedFace = fe->lockFace();
                glyph = fe->loadGlyph(gset, glyphs[i], neededFormat);
            }

            if (!glyph || !glyph->data)
                continue;

            const int pitch = (neededFormat == QFontEngineFT::Format_Mono ? ((glyph->width + 31) & ~31) >> 3
                               : (glyph->width + 3) & ~3);

            alphaPenBlt(glyph->data, pitch, neededFormat == QFontEngineFT::Format_Mono,
                        qFloor(positions[i].x + offs) + glyph->x,
                        qFloor(positions[i].y + offs) - glyph->y,
                        glyph->width, glyph->height);
        }
        if (lockedFace)
            fe->unlockFace();
        return;
    }
#elif defined(Q_WS_MAC)
    uchar *myBits = d->rasterBuffer->buffer();
    QImage image;
    if (d->rasterBuffer->bytesPerPixel() < 4) {
        image = QImage(d->rasterBuffer->width(), d->rasterBuffer->height(), QImage::Format_ARGB32_Premultiplied);
        image.fill(0);
        myBits = image.bits();
    }
    int width = d->rasterBuffer->width();
    int height = d->rasterBuffer->height();
    int bytesPerLine = image.isNull() ? d->rasterBuffer->bytesPerLine() : image.bytesPerLine();
#if (MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_4)
    uint cgflags = kCGImageAlphaPremultipliedFirst;
#endif
#if (MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_4) && defined(kCGBitmapByteOrder32Host) //only needed because CGImage.h added symbols in the minor version
    if (QSysInfo::MacintoshVersion >= QSysInfo::MV_10_4)
        cgflags |= kCGBitmapByteOrder32Host;
#endif

    CGContextRef cg = CGBitmapContextCreate(myBits, width, height, 8, bytesPerLine,
                                            QCoreGraphicsPaintEngine::macGenericColorSpace(), cgflags);
    CGContextTranslateCTM(cg, 0, height);
    CGContextScaleCTM(cg, 1, -1);
    CGContextSetShouldAntialias(cg, (d->rasterBuffer->format == QImage::Format_Mono
                                     || d->rasterBuffer->format == QImage::Format_MonoLSB));

    QMacQuartzPaintDevice pd(cg, width, height, bytesPerLine);
    QCoreGraphicsPaintEngine pe;
    reinterpret_cast<QRasterPaintEngine *>(&pe)->state = state;
    pe.begin(&pd);
    pe.setDirty(QPaintEngine::DirtyFlags(QPaintEngine::AllDirty & ~QPaintEngine::DirtyHints));
    pe.updateState(*state);
    pe.drawTextItem(p, textItem);
    pe.end();

    if (!image.isNull()) {
        QTransform old_transform = d->matrix;
        updateMatrix(QTransform());
        drawImage(QRectF(0, 0, image.width(), image.height()), image, QRectF(0, 0, image.width(), image.height()));
        updateMatrix(old_transform);
    }
    return;
#endif
    // Fallthrough for embedded
    Q_UNUSED(fontEngine);
    bool aa = d->antialiased;
    d->antialiased = !d->mono_surface;
    QPaintEngine::drawTextItem(p, ti);
    d->antialiased = aa;
    return;
#endif
}

/*!
    \reimp
*/
void QRasterPaintEngine::drawPoints(const QPointF *points, int pointCount)
{
    Q_D(QRasterPaintEngine);

    double pw = d->pen.widthF();

    if (!d->fast_pen && (d->txop > QTransform::TxTranslate || pw > 1)) {
        QBrush oldBrush = d->brush;
        d->brush = Qt::NoBrush;

        bool flat_pen = d->pen.capStyle() == Qt::FlatCap;
        if (flat_pen)
            d->basicStroker.setCapStyle(Qt::SquareCap);

        const QPointF *end = points + pointCount;
        while (points < end) {
            QPainterPath path;
            path.moveTo(*points);
            path.lineTo(points->x() + 0.001, points->y());
            drawPath(path);
            ++points;
        }

        d->brush = oldBrush;

        if (flat_pen)
            d->basicStroker.setCapStyle(Qt::FlatCap);

    } else {
        if (!d->penData.blend)
            return;

        QVarLengthArray<QT_FT_Span, 4096> array(pointCount);

        QT_FT_Span span = { 0, 1, 0, 255 };
        const QPointF *end = points + pointCount;
        qreal trans_x, trans_y;
        int x, y;
        int left = 0;
        int right = d->deviceRect.width();
        int top = 0;
        int bottom = d->deviceRect.height();
        int count = 0;
        while (points < end) {
            d->matrix.map(points->x(), points->y(), &trans_x, &trans_y);
            x = qFloor(trans_x);
            y = qFloor(trans_y);
            if (x >= left && x < right && y >= top && y < bottom) {
                if (count > 0) {
                    const QT_FT_Span &last = array[count - 1];
                    // spans must be sorted on y (primary) and x (secondary)
                    if (y < last.y || y == last.y && x < last.x) {
                        d->penData.blend(count, array.constData(), &d->penData);
                        count = 0;
                    }
                }

                span.x = x;
                span.y = y;
                array[count++] = span;
            }
            ++points;
        }

        if (count > 0)
            d->penData.blend(count, array.constData(), &d->penData);
    }
}

/*!
    \reimp
*/
void QRasterPaintEngine::drawLines(const QLine *lines, int lineCount)
{
#ifdef QT_DEBUG_DRAW
    qDebug() << " - QRasterPaintEngine::drawLine()";
#endif
    Q_D(QRasterPaintEngine);
    if (!d->penData.blend)
        return;
    if (d->fast_pen) {
        QRect bounds(0, 0, d->deviceRect.width(), d->deviceRect.height());
        LineDrawMode mode = d->pen.capStyle() == Qt::FlatCap
                            ? LineDrawNormal
                            : LineDrawIncludeLastPixel;

        int m11 = int(d->matrix.m11());
        int m22 = int(d->matrix.m22());
        int dx = int(d->matrix.dx() + aliasedCoordinateDelta);
        int dy = int(d->matrix.dy() + aliasedCoordinateDelta);
        int dashOffset = int(d->pen.dashOffset());
        for (int i=0; i<lineCount; ++i) {
            if (d->int_xform) {
                const QLine &l = lines[i];
                int x1 = l.x1() * m11 + dx;
                int y1 = l.y1() * m22 + dy;
                int x2 = l.x2() * m11 + dx;
                int y2 = l.y2() * m22 + dy;

                const QRect brect(QPoint(x1, y1), QPoint(x2, y2));
                ProcessSpans penBlend = d->getPenFunc(brect, &d->penData);
                if (d->pen.style() == Qt::SolidLine)
                    drawLine_midpoint_i(x1, y1, x2, y2,
                                        penBlend, &d->penData, mode, bounds);
                else
                    drawLine_midpoint_dashed_i(x1, y1, x2, y2,
                                               &d->pen, penBlend,
                                               &d->penData, mode, bounds,
                                               &dashOffset);
            } else {
                QLineF line = lines[i] * d->matrix;
                const QRectF brect(QPointF(line.x1(), line.y1()),
                                   QPointF(line.x2(), line.y2()));
                ProcessSpans penBlend = d->getPenFunc(brect, &d->penData);
                if (d->pen.style() == Qt::SolidLine)
                    drawLine_midpoint_i(int(line.x1()), int(line.y1()),
                                        int(line.x2()), int(line.y2()),
                                        penBlend, &d->penData, mode, bounds);
                else
                    drawLine_midpoint_dashed_i(int(line.x1()), int(line.y1()),
                                               int(line.x2()), int(line.y2()),
                                               &d->pen, penBlend,
                                               &d->penData, mode, bounds,
                                               &dashOffset);
            }
        }
    } else {
        QPaintEngine::drawLines(lines, lineCount);
    }
}

/*!
    \reimp
*/
void QRasterPaintEngine::drawLines(const QLineF *lines, int lineCount)
{
#ifdef QT_DEBUG_DRAW
    qDebug() << " - QRasterPaintEngine::drawLine()";
#endif
    Q_D(QRasterPaintEngine);
    if (!d->penData.blend)
        return;
    if (d->fast_pen) {
        QRect bounds(0, 0, d->deviceRect.width(), d->deviceRect.height());
        LineDrawMode mode = d->pen.capStyle() == Qt::FlatCap
                            ? LineDrawNormal
                            : LineDrawIncludeLastPixel;

        int dashOffset = int(d->pen.dashOffset());
        for (int i=0; i<lineCount; ++i) {
            QLineF line = (lines[i] * d->matrix).translated(aliasedCoordinateDelta, aliasedCoordinateDelta);
            const QRectF brect(QPointF(line.x1(), line.y1()),
                               QPointF(line.x2(), line.y2()));
            ProcessSpans penBlend = d->getPenFunc(brect, &d->penData);
            if (d->pen.style() == Qt::SolidLine)
                drawLine_midpoint_i(int(line.x1()), int(line.y1()),
                                    int(line.x2()), int(line.y2()),
                                    penBlend, &d->penData, mode, bounds);
            else
                drawLine_midpoint_dashed_i(int(line.x1()), int(line.y1()),
                                           int(line.x2()), int(line.y2()),
                                           &d->pen,
                                           penBlend, &d->penData, mode,
                                           bounds, &dashOffset);
        }
    } else {
#ifdef QT_FAST_SPANS
        if (d->pen.style() == Qt::SolidLine
            && d->pen.capStyle() <= Qt::SquareCap
            && d->tx_noshear)
        {
            qreal width = d->pen.isCosmetic()
                          ? (d->pen.widthF() == 0 ? 1 : d->pen.widthF())
                          : d->pen.widthF() * d->txscale;

            d->initializeRasterizer(&d->penData);

            for (int i = 0; i < lineCount; ++i) {
                if (lines[i].p1() == lines[i].p2()) {
                    if (d->pen.capStyle() != Qt::FlatCap) {
                        QPointF p = lines[i].p1();
                        QLineF line = d->matrix.map(QLineF(QPointF(p.x() - width*0.5, p.y()),
                                                           QPointF(p.x() + width*0.5, p.y())));
                        d->rasterizer.rasterizeLine(line.p1(), line.p2(), 1);
                    }
                    continue;
                }

                QLineF line = d->matrix.map(lines[i]);
                d->rasterizer.rasterizeLine(line.p1(), line.p2(), width / line.length(), d->pen.capStyle() == Qt::SquareCap);
            }

            return;
        }
#endif // QT_FAST_SPANS
        QPaintEngine::drawLines(lines, lineCount);
    }
}


// A little helper macro to get a better approximation of dimensions.
// If we have a rect that starting at 0.5 of width 3.5 it should span
// 4 pixels.
#define int_dim(pos, dim) (int(pos+dim) - int(pos))

/*!
    \reimp
*/
void QRasterPaintEngine::drawEllipse(const QRectF &rect)
{
    Q_D(QRasterPaintEngine);
    if (!d->brushData.blend && !d->penData.blend)
        return;

    const QRectF r = d->matrix.mapRect(rect);
    ProcessSpans penBlend = d->getPenFunc(r, &d->penData);
    ProcessSpans brushBlend = d->getBrushFunc(r, &d->brushData);

    if ((d->pen.style() == Qt::SolidLine && d->fast_pen
         || d->pen.style() == Qt::NoPen && !d->antialiased)
#ifdef FLOATING_POINT_BUGGY_OR_NO_FPU
        && qMax(r.width(), r.height()) < 128 // integer math breakdown
#endif
        && d->txop <= QTransform::TxScale) // no shear
    {
        const QRect devRect(0, 0, d->deviceRect.width(), d->deviceRect.height());
        const QRect brect = QRect(int(r.x()), int(r.y()),
                                  int_dim(r.x(), r.width()),
                                  int_dim(r.y(), r.height()));
        if (brect == r) {
            drawEllipse_midpoint_i(brect, devRect, penBlend, brushBlend,
                                   &d->penData, &d->brushData);
            return;
        }
    }

    if (d->brushData.blend) {
        QPointF controlPoints[12];
        int point_count = 0;
        QPointF start = qt_curves_for_arc(rect, 0, -360, controlPoints, &point_count);

        Q_ASSERT(point_count == 12); // a perfect circle...

        d->outlineMapper->beginOutline(Qt::WindingFill);
        d->outlineMapper->moveTo(start);
        for (int i=0; i<point_count; i+=3) {
            d->outlineMapper->curveTo(controlPoints[i], controlPoints[i+1], controlPoints[i+2]);
        }
        d->outlineMapper->endOutline();

        d->rasterize(d->outlineMapper->outline(), brushBlend, &d->brushData, d->rasterBuffer);
    }

    if (d->penData.blend) {
        d->outlineMapper->beginOutline(Qt::WindingFill);
        if (d->pen.isCosmetic()) {
            d->outlineMapper->setMatrix(QTransform(), QTransform::TxNone);
            d->stroker->strokeEllipse(rect, d->outlineMapper, d->matrix);
        } else {
            d->outlineMapper->setMatrix(d->matrix, d->txop);
            d->stroker->strokeEllipse(rect, d->outlineMapper, QTransform());
        }
        d->outlineMapper->endOutline();

        d->rasterize(d->outlineMapper->outline(), penBlend, &d->penData, d->rasterBuffer);

        d->outlineMapper->setMatrix(d->matrix, d->txop);
    }
}

/*!
    \internal
*/
#ifdef Q_WS_MAC
CGContextRef
QRasterPaintEngine::macCGContext() const
{
    Q_D(const QRasterPaintEngine);
    return d->rasterBuffer->macCGContext();
}
#endif

/*!
    \internal
*/
#ifdef Q_WS_WIN
HDC QRasterPaintEngine::getDC() const
{
    Q_D(const QRasterPaintEngine);
    return d->rasterBuffer->hdc();
}

/*!
    \internal
*/
void QRasterPaintEngine::releaseDC(HDC) const
{
}

/*!
    \internal
*/
void QRasterPaintEngine::disableClearType()
{
    Q_D(QRasterPaintEngine);
    d->clear_type_text = false;
    d->fontRasterBuffer->setupHDC(d->clear_type_text);
}

#endif

/*!
    \internal
*/
QPoint QRasterPaintEngine::coordinateOffset() const
{
    Q_D(const QRasterPaintEngine);
    return QPoint(d->deviceRect.x(), d->deviceRect.y());
}

/*!
    Draws the given color \a spans with the specified \a color. The \a
    count parameter specifies the number of spans.

    The default implementation does nothing; reimplement this function
    to draw the given color \a spans with the specified \a color. Note
    that this function \e must be reimplemented if the framebuffer is
    not memory-mapped.

    \sa drawBufferSpan()
*/
#ifdef Q_WS_QWS
void QRasterPaintEngine::drawColorSpans(const QSpan *spans, int count, uint color)
{
    Q_UNUSED(spans);
    Q_UNUSED(count);
    Q_UNUSED(color);
    qFatal("QRasterPaintEngine::drawColorSpans must be reimplemented on "
           "a non memory-mapped device");
}

/*!
    \fn void QRasterPaintEngine::drawBufferSpan(const uint *buffer, int size, int x, int y, int length, uint alpha)

    Draws the given \a buffer.

    The default implementation does nothing; reimplement this function
    to draw a buffer that contains more than one color. Note that this
    function \e must be reimplemented if the framebuffer is not
    memory-mapped.

    The \a size parameter specifies the total size of the given \a
    buffer, while the \a length parameter specifies the number of
    pixels to draw. The buffer's position is given by (\a x, \a
    y). The provided \a alpha value is added to each pixel in the
    buffer when drawing.

    \sa drawColorSpans()
*/
void QRasterPaintEngine::drawBufferSpan(const uint *buffer, int bufsize,
                                        int x, int y, int length, uint const_alpha)
{
    Q_UNUSED(buffer);
    Q_UNUSED(bufsize);
    Q_UNUSED(x);
    Q_UNUSED(y);
    Q_UNUSED(length);
    Q_UNUSED(const_alpha);
    qFatal("QRasterPaintEngine::drawBufferSpan must be reimplemented on "
           "a non memory-mapped device");
}
#endif // Q_WS_QWS

void QRasterPaintEnginePrivate::drawBitmap(const QPointF &pos, const QPixmap &pm, QSpanData *fg)
{
    Q_ASSERT(fg);
    if (!fg->blend)
        return;

    const QImage image = pm.toImage();
    Q_ASSERT(image.depth() == 1);

    const int spanCount = 256;
    QT_FT_Span spans[spanCount];
    int n = 0;

    // Boundaries
    int w = pm.width();
    int h = pm.height();
    int ymax = qMin(qRound(pos.y() + h), rasterBuffer->height());
    int ymin = qMax(qRound(pos.y()), 0);
    int xmax = qMin(qRound(pos.x() + w), rasterBuffer->width());
    int xmin = qMax(qRound(pos.x()), 0);

    int x_offset = xmin - qRound(pos.x());

#if defined (BITMAPS_ARE_MSB)
    QImage::Format format = image.format();
#endif
    for (int y = ymin; y < ymax; ++y) {
        const uchar *src = image.scanLine(y - qRound(pos.y()));
#if defined (BITMAPS_ARE_MSB)
        if (format == QImage::Format_MonoLSB) {
#endif
            for (int x = 0; x < xmax - xmin; ++x) {
                int src_x = x + x_offset;
                uchar pixel = src[src_x >> 3];
                if (!pixel) {
                    x += 7 - (src_x%8);
                    continue;
                }
                if (pixel & (0x1 << (src_x & 7))) {
                    spans[n].x = xmin + x;
                    spans[n].y = y;
                    spans[n].coverage = 255;
                    int len = 1;
                    while (src_x < w-1 && src[(src_x+1) >> 3] & (0x1 << ((src_x+1) & 7))) {
                        ++src_x;
                        ++len;
                    }
                    spans[n].len = ((len + spans[n].x) > xmax) ? (xmax - spans[n].x) : len;
                    x += len;
                    ++n;
                    if (n == spanCount) {
                        fg->blend(n, spans, fg);
                        n = 0;
                    }
                }
            }
#if defined (BITMAPS_ARE_MSB)
        } else {
            for (int x = 0; x < xmax - xmin; ++x) {
                int src_x = x + x_offset;
                uchar pixel = src[src_x >> 3];
                if (!pixel) {
                    x += 7 - (src_x%8);
                    continue;
                }
                if (pixel & (0x80 >> (x & 7))) {
                    spans[n].x = xmin + x;
                    spans[n].y = y;
                    spans[n].coverage = 255;
                    int len = 1;
                    while (src_x < w-1 && src[(src_x+1) >> 3] & (0x80 >> ((src_x+1) & 7))) {
                        ++src_x;
                        ++len;
                    }
                    spans[n].len = ((len + spans[n].x) > xmax) ? (xmax - spans[n].x) : len;
                    x += len;
                    ++n;
                    if (n == spanCount) {
                        fg->blend(n, spans, fg);
                        n = 0;
                    }
                }
            }
        }
#endif
    }
    if (n) {
        fg->blend(n, spans, fg);
        n = 0;
    }
}

static void qt_merge_clip(const QClipData *c1, const QClipData *c2, QClipData *result)
{
    Q_ASSERT(c1->clipSpanHeight == c2->clipSpanHeight && c1->clipSpanHeight == result->clipSpanHeight);

    // ### buffer overflow possible
    const int BUFFER_SIZE = 4096;
    int buffer[BUFFER_SIZE];
    int *b = buffer;
    int bsize = BUFFER_SIZE;

    for (int y = 0; y < c1->clipSpanHeight; ++y) {
        const QSpan *c1_spans = c1->clipLines[y].spans;
        int c1_count = c1->clipLines[y].count;
        const QSpan *c2_spans = c2->clipLines[y].spans;
        int c2_count = c2->clipLines[y].count;

        if (c1_count == 0 && c2_count == 0)
            continue;
        if (c1_count == 0) {
            result->appendSpans(c2_spans, c2_count);
            continue;
        } else if (c2_count == 0) {
            result->appendSpans(c1_spans, c1_count);
            continue;
        }

        // we need to merge the two

        // find required length
        int max = qMax(c1_spans[c1_count - 1].x + c1_spans[c1_count - 1].len,
                       c2_spans[c2_count - 1].x + c2_spans[c2_count - 1].len);
        if (max > bsize) {
            b = (int *)realloc(bsize == BUFFER_SIZE ? 0 : b, max*sizeof(int));
            bsize = max;
        }
        memset(buffer, 0, BUFFER_SIZE * sizeof(int));

        // Fill with old spans.
        for (int i = 0; i < c1_count; ++i) {
            const QSpan *cs = c1_spans + i;
            for (int j=cs->x; j<cs->x + cs->len; ++j)
                buffer[j] = cs->coverage;
        }

        // Fill with new spans
        for (int i = 0; i < c2_count; ++i) {
            const QSpan *cs = c2_spans + i;
            for (int j = cs->x; j < cs->x + cs->len; ++j) {
                buffer[j] += cs->coverage;
                if (buffer[j] > 255)
                    buffer[j] = 255;
            }
        }

        int x = 0;
        while (x<max) {

            // Skip to next span
            while (x < max && buffer[x] == 0) ++x;
            if (x >= max) break;

            int sx = x;
            int coverage = buffer[x];

            // Find length of span
            while (x < max && buffer[x] == coverage)
                ++x;

            result->appendSpan(sx, x - sx, y, coverage);
        }
    }
    if (b != buffer)
        free(b);
}

void QRasterPaintEnginePrivate::initializeRasterizer(QSpanData *data)
{
    rasterizer.setAntialiased(antialiased);

    QRect clipRect;
    ProcessSpans blend;
    if (rasterBuffer->clipRect.isEmpty()) {
        clipRect = deviceRect;
        const QClipData *clip = rasterBuffer->clip;
        if (rasterBuffer->clipEnabled && clip) {
            const QRect r(QPoint(clip->xmin, clip->ymin),
                          QPoint(clip->xmax, clip->ymax));

            clipRect = clipRect.intersected(r);
        }
        blend = data->blend;
    } else {
        clipRect = rasterBuffer->clipRect;
        blend = data->unclipped_blend;
    }

    rasterizer.setClipRect(clipRect);
    rasterizer.initialize(blend, data);
}

void QRasterPaintEnginePrivate::rasterize(QT_FT_Outline *outline,
                                          ProcessSpans callback,
                                          QSpanData *spanData, QRasterBuffer *rasterBuffer)
{
    if (!callback || !outline)
        return;

#ifdef QT_FAST_SPANS
    if (!antialiased) {
        initializeRasterizer(spanData);

        const Qt::FillRule fillRule = outline->flags == QT_FT_OUTLINE_NONE
                                      ? Qt::WindingFill
                                      : Qt::OddEvenFill;

        rasterizer.rasterize(outline, fillRule);
        return;
    }
#endif
    rasterize(outline, callback, (void *)spanData, rasterBuffer);
}

void QRasterPaintEnginePrivate::rasterize(QT_FT_Outline *outline,
                                          ProcessSpans callback,
                                          void *userData, QRasterBuffer *rasterBuffer)
{
    if (!callback || !outline)
        return;

#ifdef QT_FAST_SPANS
    if (!antialiased) {
        rasterizer.setAntialiased(antialiased);
        rasterizer.setClipRect(deviceRect);
        rasterizer.initialize(callback, userData);

        const Qt::FillRule fillRule = outline->flags == QT_FT_OUTLINE_NONE
                                      ? Qt::WindingFill
                                      : Qt::OddEvenFill;

        rasterizer.rasterize(outline, fillRule);
        return;
    }
#endif

    void *data = userData;

    QT_FT_BBox clip_box = { 0, 0, deviceRect.width(), deviceRect.height() };
    if (rasterBuffer && rasterBuffer->clipEnabled && rasterBuffer->clip) {
        Q_ASSERT(((QSpanData *)userData)->rasterBuffer == rasterBuffer);
        clip_box.xMin = qMax((int)clip_box.xMin, rasterBuffer->clip->xmin);
        clip_box.xMax = qMin((int)clip_box.xMax, rasterBuffer->clip->xmax);
        if (antialiased)
            // ### Fixme: The black raster gives drawing errors when you try to
            // move ymin to something greater 0.
            clip_box.yMin = qMax((int)clip_box.yMin, rasterBuffer->clip->ymin);
        clip_box.yMax = qMin((int)clip_box.yMax, rasterBuffer->clip->ymax);
    }

    QT_FT_Raster_Params rasterParams;
    rasterParams.target = 0;
    rasterParams.source = outline;
    rasterParams.flags = QT_FT_RASTER_FLAG_CLIP;
    rasterParams.gray_spans = 0;
    rasterParams.black_spans = 0;
    rasterParams.bit_test = 0;
    rasterParams.bit_set = 0;
    rasterParams.user = data;
    rasterParams.clip_box = clip_box;

    bool done = false;
    int error;

    while (!done) {

        if (antialiased) {
            rasterParams.flags |= (QT_FT_RASTER_FLAG_AA | QT_FT_RASTER_FLAG_DIRECT);
            rasterParams.gray_spans = callback;
            error = qt_ft_grays_raster.raster_render(*grayRaster, &rasterParams);
        } else {
            rasterParams.flags |= QT_FT_RASTER_FLAG_DIRECT;
            rasterParams.black_spans = callback;
            error = qt_ft_standard_raster.raster_render(*blackRaster, &rasterParams);
        }

        // Out of memory, reallocate some more and try again...
        if (error == -6) { // -6 is Result_err_OutOfMemory
            int new_size = rasterPoolSize * 2;
            if (new_size > 1024 * 1024) {
                qWarning("QPainter: Rasterization of primitive failed");
                return;
            }

#if defined(Q_WS_WIN64)
            _aligned_free(rasterPoolBase);
#else
            free(rasterPoolBase);
#endif

            rasterPoolSize = new_size;
            rasterPoolBase =
#if defined(Q_WS_WIN64)
                // We make use of setjmp and longjmp in qgrayraster.c which requires
                // 16-byte alignment, hence we hardcode this requirement here..
                (unsigned char *) _aligned_malloc(rasterPoolSize, sizeof(void*) * 2);
#else
                (unsigned char *) malloc(rasterPoolSize);
#endif

            qt_ft_grays_raster.raster_done(*grayRaster);
            qt_ft_grays_raster.raster_new(0, grayRaster);
            qt_ft_grays_raster.raster_reset(*grayRaster, rasterPoolBase, rasterPoolSize);

            qt_ft_standard_raster.raster_done(*blackRaster);
            qt_ft_standard_raster.raster_new(0, blackRaster);
            qt_ft_standard_raster.raster_reset(*blackRaster, rasterPoolBase, rasterPoolSize);

        } else {
            done = true;
        }
    }
}


void QRasterPaintEnginePrivate::updateClip_helper(const QPainterPath &path, Qt::ClipOperation op)
{
#ifdef QT_DEBUG_DRAW
    QRectF bounds = path.boundingRect();
    qDebug() << " --- updateClip_helper(), op=" << op << ", bounds=" << bounds
             << rasterBuffer->clipEnabled << rasterBuffer->clip;
#endif
    if (op == Qt::IntersectClip && !rasterBuffer->clipEnabled)
        op = Qt::ReplaceClip;

    if (op == Qt::NoClip) {
        rasterBuffer->resetClip();
        rasterBuffer->clipEnabled = false;
        goto end;
    } else if (path.isEmpty()) {
        if (op == Qt::ReplaceClip || op == Qt::IntersectClip) {
            rasterBuffer->resetClip();
            rasterBuffer->clip = new QClipData(rasterBuffer->height());
        }
    } else if (op == Qt::ReplaceClip) {
        rasterBuffer->resetClip();
    } else if (op == Qt::IntersectClip && !rasterBuffer->clip) {
        return;
    }

    rasterBuffer->clipEnabled = true;
    if (!path.isEmpty()) {
        QClipData *newClip = new QClipData(rasterBuffer->height());
        ClipData clipData = { rasterBuffer->clip, newClip, op };
        rasterize(outlineMapper->convertPath(path), qt_span_clip, &clipData, 0);
        newClip->fixup();

        if (op == Qt::UniteClip) {
            // merge clips
            QClipData *result = new QClipData(rasterBuffer->height());
            qt_merge_clip(rasterBuffer->clip, newClip, result);
            result->fixup();
            delete newClip;
            newClip = result;
        }

        delete rasterBuffer->clip;
        rasterBuffer->clip = newClip;
    }
end:
    penData.adjustSpanMethods();
    brushData.adjustSpanMethods();
}

QImage QRasterBuffer::colorizeBitmap(const QImage &image, const QColor &color)
{
    Q_ASSERT(image.depth() == 1);

    QImage sourceImage = image.convertToFormat(QImage::Format_MonoLSB);
    QImage dest = QImage(sourceImage.size(), QImage::Format_ARGB32_Premultiplied);

    QRgb fg = PREMUL(color.rgba());
    QRgb bg = 0;

    int height = sourceImage.height();
    int width = sourceImage.width();
    for (int y=0; y<height; ++y) {
        uchar *source = sourceImage.scanLine(y);
        QRgb *target = reinterpret_cast<QRgb *>(dest.scanLine(y));
        for (int x=0; x < width; ++x)
            target[x] = (source[x>>3] >> (x&7)) & 1 ? fg : bg;
    }
    return dest;
}

QRasterBuffer::~QRasterBuffer()
{
    delete clip;

#if defined (Q_WS_MAC)
    if(m_ctx) {
        CGContextRelease(m_ctx);
        m_ctx = 0;
    }
    if(m_data) {
        CGImageRelease(m_data);
        m_data = 0;
    }
#endif

#if defined (Q_WS_WIN)
    if (m_bitmap || m_hdc) {
        Q_ASSERT(m_hdc);
        Q_ASSERT(m_bitmap);
        if (m_null_bitmap)
            SelectObject(m_hdc, m_null_bitmap);
        DeleteDC(m_hdc);
        DeleteObject(m_bitmap);
    }
#endif
}

void QRasterBuffer::init()
{
    clipEnabled = false;
    disabled_clip = 0;

    compositionMode = QPainter::CompositionMode_SourceOver;
    clipRect = QRect();
    delete clip;
    clip = 0;
#ifdef Q_WS_WIN
    static int nativeBitCount = 0;
    if (nativeBitCount == 0) {
        HDC handle = GetDC(0);
        if (handle) {
            nativeBitCount = GetDeviceCaps(handle, BITSPIXEL);
            if (nativeBitCount == 16)
                qt_enable_16bit_colors = true;
            else
                qt_enable_16bit_colors = false;
            ReleaseDC(0, handle);
        }
    }

    if (qt_enable_16bit_colors)
        format = QImage::Format_RGB16;
    else
#endif
        format = QImage::Format_ARGB32_Premultiplied;
    drawHelper = qDrawHelper + format;
    monoDestinationWithClut = false;
    destColor0 = 0;
    destColor1 = 0;
}


#if defined(Q_WS_MAC)
CGContextRef
QRasterBuffer::macCGContext() const
{
    if(!m_ctx && m_data) {
#if (MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_4)
        uint flags = CGImageGetAlphaInfo(m_data);
        CGBitmapInfo (*CGImageGetBitmapInfo_ptr)(CGImageRef) = CGImageGetBitmapInfo;
        if(CGImageGetBitmapInfo_ptr)
            flags |= (*CGImageGetBitmapInfo_ptr)(m_data);
#else
        CGImageAlphaInfo flags = CGImageGetAlphaInfo(m_data);
#endif
        CGColorSpaceRef colorspace = QCoreGraphicsPaintEngine::macGenericColorSpace();
        m_ctx = CGBitmapContextCreate(m_buffer, m_width, m_height, 8, m_width * 4, colorspace,
                                      flags);
        if(!m_ctx)
            qWarning("QPaintDevice: Unable to create context for rasterbuffer (%d/%d)",
                     m_width, m_height);
        CGContextTranslateCTM(m_ctx, 0, m_height);
        CGContextScaleCTM(m_ctx, 1, -1);
    }
    return m_ctx;
}
#endif

#if defined(Q_WS_WIN)
void QRasterBuffer::setupHDC(bool clear_type)
{
    if (!clear_type) {
        SelectObject(m_hdc, GetStockObject(BLACK_BRUSH));
        SelectObject(m_hdc, GetStockObject(BLACK_PEN));
        SetTextColor(m_hdc, RGB(0, 0, 0));
    }
}
#endif

void QRasterBuffer::prepare(int w, int h)
{
    if (w<=m_width && h<=m_height) {
#ifdef Q_WS_MAC
        memset(m_buffer, 0, m_width*h*sizeof(uint));
#endif
        return;
    }

    prepareBuffer(w, h);

    m_width = w;
    m_height = h;
    bytes_per_pixel = (format == QImage::Format_RGB16) ? 2 : 4;
    bytes_per_line = ((m_width * bytes_per_pixel*8 + 31) >> 5) << 2; // bytes per scanline (must be multiple of 4)
}


void QRasterBuffer::prepare(QImage *image)
{
    m_buffer = (uchar *)image->bits();
    m_width = image->width();
    m_height = image->height();
    bytes_per_pixel = image->depth()/8;
    bytes_per_line = image->bytesPerLine();

    format = image->format();
    drawHelper = qDrawHelper + format;
    if (image->depth() == 1 && image->colorTable().size() == 2) {
        monoDestinationWithClut = true;
        destColor0 = PREMUL(image->colorTable()[0]);
        destColor1 = PREMUL(image->colorTable()[1]);
    }
}

void QRasterBuffer::resetBuffer(int val)
{
    memset(m_buffer, val, m_height*bytes_per_line);
}

void QRasterBuffer::prepare(QPixmap *pixmap)
{
    QPixmapData *data = pixmap->data_ptr();
    m_width = data->width();
    m_height = data->height();
    bytes_per_pixel = pixmap->depth() / 8;

#if defined(Q_WS_MAC)
    QMacPixmapData *macData = static_cast<QMacPixmapData*>(data);
    m_buffer = (uchar *)macData->pixels;
    bytes_per_line = macData->bytesPerRow;
    m_data = macData->cg_data;
    CGImageRetain(m_data);
#else
    QImage *image = data->buffer();
    format = image->format();
    m_buffer = image->bits();
    bytes_per_line = image->bytesPerLine();
    drawHelper = qDrawHelper + format;
#endif
#ifdef Q_WS_QWS
    if (!m_buffer)
        drawHelper = qDrawHelperCallback + format;
#endif
}

#if defined(Q_WS_WIN)
typedef struct {
    BITMAPINFOHEADER bmiHeader;
    DWORD redMask;
    DWORD greenMask;
    DWORD blueMask;
} BITMAPINFO_MASK;

void QRasterBuffer::prepareBuffer(int width, int height)
{
    BITMAPINFO_MASK bmi;
    memset(&bmi, 0, sizeof(bmi));
    bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth       = width;
    bmi.bmiHeader.biHeight      = -height;
    bmi.bmiHeader.biPlanes      = 1;
    bmi.bmiHeader.biSizeImage   = 0;
    if (format != QImage::Format_RGB16) {
        bmi.bmiHeader.biBitCount    = 32;
        bmi.bmiHeader.biCompression = BI_RGB;
        bmi.redMask = 0;
        bmi.greenMask = 0;
        bmi.blueMask = 0;
    } else {
        bmi.bmiHeader.biBitCount    = 16;
        if (m_text_buffer) {
            bmi.bmiHeader.biCompression = BI_RGB;
            bmi.redMask = 0;
            bmi.greenMask = 0;
            bmi.blueMask = 0;
        } else {
            bmi.bmiHeader.biCompression = BI_BITFIELDS;
            bmi.redMask = 0xF800;
            bmi.greenMask = 0x07E0;
            bmi.blueMask = 0x001F;
        }
    }

    HDC displayDC = GetDC(0);

    // a little bit of cleanup...
    if (m_bitmap || m_hdc) {
        Q_ASSERT(m_hdc);
        Q_ASSERT(m_bitmap);
        if (m_null_bitmap)
            SelectObject(m_hdc, m_null_bitmap);
        DeleteDC(m_hdc);
        DeleteObject(m_bitmap);
    }

    m_hdc = CreateCompatibleDC(displayDC);
    Q_ASSERT(m_hdc);

    m_buffer = 0;
    m_bitmap = CreateDIBSection(m_hdc, reinterpret_cast<BITMAPINFO *>(&bmi), DIB_RGB_COLORS, (void**) &m_buffer, 0, 0);
    Q_ASSERT(m_bitmap);
    Q_ASSERT(m_buffer);
#if !defined(Q_OS_WINCE)
    GdiFlush();
#endif

    m_null_bitmap = (HBITMAP)SelectObject(m_hdc, m_bitmap);
    ReleaseDC(0, displayDC);
}
#elif defined(Q_WS_X11)
void QRasterBuffer::prepareBuffer(int width, int height)
{
    delete[] m_buffer;
    m_buffer = new uchar[width*height*sizeof(uint)];
    memset(m_buffer, 255, width*height*sizeof(uint));
}
#elif defined(Q_WS_MAC)
static void qt_mac_raster_data_free(void *memory, const void *, size_t)
{
    free(memory);
}

void QRasterBuffer::prepareBuffer(int width, int height)
{
    m_buffer = new uchar[width*height*sizeof(uint)];
    memset(m_buffer, 0, width*height*sizeof(uint));

    if (m_data) {
        CGImageRelease(m_data);
        m_data = 0;
    }
    if (m_ctx) {
        CGContextRelease(m_ctx);
        m_ctx = 0;
    }
    CGColorSpaceRef colorspace = QCoreGraphicsPaintEngine::macGenericColorSpace();
    CGDataProviderRef provider = CGDataProviderCreateWithData(m_buffer, m_buffer, width*height*sizeof(uint),
                                                              qt_mac_raster_data_free);
#if (MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_4)
    uint cgflags = kCGImageAlphaPremultipliedFirst;
#ifdef kCGBitmapByteOrder32Host //only needed because CGImage.h added symbols in the minor version
    if(QSysInfo::MacintoshVersion >= QSysInfo::MV_10_4)
        cgflags |= kCGBitmapByteOrder32Host;
#endif
#else
    CGImageAlphaInfo cgflags = kCGImageAlphaPremultipliedFirst;
#endif
    m_data = CGImageCreate(width, height, 8, 32, width*4, colorspace,
                           cgflags, provider, 0, 0, kCGRenderingIntentDefault);
    CGColorSpaceRelease(colorspace);
    CGDataProviderRelease(provider);
}
#elif defined(Q_WS_QWS)

void QRasterBuffer::prepare(QCustomRasterPaintDevice *device)
{
    m_buffer = reinterpret_cast<uchar*>(device->memory());
    m_width = device->width();
    m_height = device->height();
    bytes_per_pixel = device->depth() / 8;
    bytes_per_line = device->bytesPerLine();
    format = device->format();
    drawHelper = (m_buffer ? qDrawHelper : qDrawHelperCallback) + format;
}

void QRasterBuffer::prepareBuffer(int /*width*/, int /*height*/)
{
    qFatal("QRasterBuffer::prepareBuffer not implemented on embedded");
    m_buffer = 0;
}

class MetricAccessor : public QWidget {
public:
    int metric(PaintDeviceMetric m) {
        return QWidget::metric(m);
    }
};

int QCustomRasterPaintDevice::metric(PaintDeviceMetric m) const
{
    switch (m) {
    case PdmWidth:
        return widget->frameGeometry().width();
    case PdmHeight:
        return widget->frameGeometry().height();
    default:
        break;
    }

    return (static_cast<MetricAccessor*>(widget)->metric(m));
}

int QCustomRasterPaintDevice::bytesPerLine() const
{
    return (width() * depth() + 7) / 8;
}
#endif // Q_WS_QWS


/*!
    \class QCustomRasterPaintDevice
    \preliminary
    \ingroup qws
    \since 4.2

    \brief The QCustomRasterPaintDevice class is provided to activate
    hardware accelerated paint engines in Qt for Embedded Linux.

    Note that this class is only available in \l{Qt for Embedded Linux}.

    In \l{Qt for Embedded Linux}, painting is a pure software
    implementation. But starting with Qt 4.2, it is
    possible to add an accelerated graphics driver to take advantage
    of available hardware resources.

    Hardware acceleration is accomplished by creating a custom screen
    driver, accelerating the copying from memory to the screen, and
    implementing a custom paint engine accelerating the various
    painting operations. Then a custom paint device (derived from the
    QCustomRasterPaintDevice class) and a custom window surface
    (derived from QWSWindowSurface) must be implemented to make
    \l{Qt for Embedded Linux} aware of the accelerated driver.

    See the \l {Adding an Accelerated Graphics Driver to Qt for Embedded Linux}
    documentation for details.

    \sa QRasterPaintEngine, QPaintDevice
*/

/*!
    \fn QCustomRasterPaintDevice::QCustomRasterPaintDevice(QWidget *widget)

    Constructs a custom raster based paint device for the given
    top-level \a widget.
*/

/*!
    \fn int QCustomRasterPaintDevice::bytesPerLine() const

    Returns the number of bytes per line in the framebuffer. Note that
    this number might be larger than the framebuffer width.
*/

/*!
    \fn int QCustomRasterPaintDevice::devType() const
    \internal
*/

/*!
    \fn QImage::Format QCustomRasterPaintDevice::format() const

    Returns the format of the device's memory buffet.

    The default format is QImage::Format_ARGB32_Premultiplied. The
    only other valid format is QImage::Format_RGB16.
*/

/*!
    \fn void * QCustomRasterPaintDevice::memory () const

    Returns a pointer to the paint device's memory buffer, or 0 if no
    such buffer exists.
*/

/*!
    \fn int QCustomRasterPaintDevice::metric ( PaintDeviceMetric m ) const
    \reimp
*/

/*!
    \fn QSize QCustomRasterPaintDevice::size () const
    \internal
*/


QClipData::QClipData(int height)
{
    clipSpanHeight = height;
    clipLines = (ClipLine *)calloc(sizeof(ClipLine), height);

    allocated = height;
    spans = (QSpan *)malloc(height*sizeof(QSpan));
    xmin = xmax = ymin = ymax = 0;
    count = 0;
}

QClipData::~QClipData()
{
    free(clipLines);
    free(spans);
}

void QClipData::fixup()
{
    if (count == 0) {
        ymin = ymax = xmin = xmax = 0;
        return;
    }

//      qDebug("QClipData::fixup: count=%d",count);
    int y = -1;
    ymin = spans[0].y;
    ymax = spans[count-1].y + 1;
    xmin = INT_MAX;
    xmax = 0;
    for (int i = 0; i < count; ++i) {
//           qDebug() << "    " << spans[i].x << spans[i].y << spans[i].len << spans[i].coverage;
        if (spans[i].y != y) {
            y = spans[i].y;
            clipLines[y].spans = spans+i;
            clipLines[y].count = 0;
//              qDebug() << "        new line: y=" << y;
        }
        ++clipLines[y].count;
        xmin = qMin(xmin, (int)spans[i].x);
        xmax = qMax(xmax, (int)spans[i].x + spans[i].len);
    }
    ++xmax;
//     qDebug("xmin=%d,xmax=%d,ymin=%d,ymax=%d", xmin, xmax, ymin, ymax);
}

/*
    Convert \a rect to clip spans.
 */
void QClipData::setClipRect(const QRect &rect)
{
//    qDebug() << "setClipRect" << clipSpanHeight << count << allocated << rect;

    xmin = rect.x();
    xmax = rect.x() + rect.width() + 1;
    ymin = rect.y();
    ymax = rect.y() + rect.height();

//    qDebug() << xmin << xmax << ymin << ymax;

    int y = 0;

    while (y < ymin) {
        clipLines[y].spans = 0;
        clipLines[y].count = 0;
        ++y;
    }

    const int len = rect.width();
    count = 0;
    while (y < ymax) {
        QSpan *span = spans + count;
        span->x = xmin;
        span->len = len;
        span->y = y;
        span->coverage = 255;
        ++count;

        clipLines[y].spans = span;
        clipLines[y].count = 1;
        ++y;
    }

    while (y < clipSpanHeight) {
        clipLines[y].spans = 0;
        clipLines[y].count = 0;
        ++y;
    }
}

/*
    Convert \a region to clip spans.
 */
void QClipData::setClipRegion(const QRegion &region)
{
    { // set bounding rect
        const QRect rect = region.boundingRect();
        xmin = rect.x();
        xmax = rect.x() + rect.width() + 1;
        ymin = rect.y();
        ymax = rect.y() + rect.height();
    }

    const QVector<QRect> rects = region.rects();
    const int numRects = rects.size();

    { // resize
        const int maxSpans = (ymax - ymin) * numRects;
        if (maxSpans > allocated) {
            spans = (QSpan *)realloc(spans, maxSpans * sizeof(QSpan));
            allocated = maxSpans;
        }
    }

    int y = 0;
    int firstInBand = 0;
    while (firstInBand < numRects) {
        const int currMinY = rects.at(firstInBand).y();
        const int currMaxY = currMinY + rects.at(firstInBand).height();

        while (y < currMinY) {
            clipLines[y].spans = 0;
            clipLines[y].count = 0;
            ++y;
        }

        int lastInBand = firstInBand;
        while (lastInBand + 1 < numRects && rects.at(lastInBand+1).top() == y)
            ++lastInBand;

        while (y < currMaxY) {

            clipLines[y].spans = spans + count;
            clipLines[y].count = lastInBand - firstInBand + 1;

            for (int r = firstInBand; r <= lastInBand; ++r) {
                const QRect &currRect = rects.at(r);
                QSpan *span = spans + count;
                span->x = currRect.x();
                span->len = currRect.width();
                span->y = y;
                span->coverage = 255;
                ++count;
            }
            ++y;
        }

        firstInBand = lastInBand + 1;
    }

    Q_ASSERT(count <= allocated);

    while (y < clipSpanHeight) {
        clipLines[y].spans = 0;
        clipLines[y].count = 0;
        ++y;
    }
}

/*!
    \internal
    spans must be sorted on y
*/
static const QSpan *qt_intersect_spans(const QClipData *clip, int *currentClip,
                                       const QSpan *spans, const QSpan *end,
                                       QSpan **outSpans, int available)
{
    QSpan *out = *outSpans;

    const QSpan *clipSpans = clip->spans + *currentClip;
    const QSpan *clipEnd = clip->spans + clip->count;

    while (available && spans < end ) {
        if (clipSpans >= clipEnd) {
            spans = end;
            break;
        }
        if (clipSpans->y > spans->y) {
            ++spans;
            continue;
        }
        if (spans->y != clipSpans->y) {
            if (spans->y < clip->count && clip->clipLines[spans->y].spans)
                clipSpans = clip->clipLines[spans->y].spans;
            else
                ++clipSpans;
            continue;
        }
        Q_ASSERT(spans->y == clipSpans->y);

        int sx1 = spans->x;
        int sx2 = sx1 + spans->len;
        int cx1 = clipSpans->x;
        int cx2 = cx1 + clipSpans->len;

        if (cx1 < sx1 && cx2 < sx1) {
            ++clipSpans;
            continue;
        } else if (sx1 < cx1 && sx2 < cx1) {
            ++spans;
            continue;
        }
        int x = qMax(sx1, cx1);
        int len = qMin(sx2, cx2) - x;
        if (len) {
            out->x = qMax(sx1, cx1);
            out->len = qMin(sx2, cx2) - out->x;
            out->y = spans->y;
            out->coverage = qt_div_255(spans->coverage * clipSpans->coverage);
            ++out;
            --available;
        }
        if (sx2 < cx2) {
            ++spans;
        } else {
            ++clipSpans;
        }
    }

    *outSpans = out;
    *currentClip = clipSpans - clip->spans;
    return spans;
}

static void qt_span_fill_clipped(int spanCount, const QSpan *spans, void *userData)
{
//     qDebug() << "qt_span_fill_clipped" << spanCount;
    QSpanData *fillData = reinterpret_cast<QSpanData *>(userData);

    Q_ASSERT(fillData->blend && fillData->unclipped_blend);

    QRasterBuffer *rb = fillData->rasterBuffer;
    Q_ASSERT(rb->clip);

    const int NSPANS = 256;
    QSpan cspans[NSPANS];
    int currentClip = 0;
    const QSpan *end = spans + spanCount;
    while (spans < end) {
        QSpan *clipped = cspans;
        spans = qt_intersect_spans(rb->clip, &currentClip, spans, end, &clipped, NSPANS);
//         qDebug() << "processed " << processed << "clipped" << clipped-cspans
//                  << "span:" << cspans->x << cspans->y << cspans->len << spans->coverage;

        if (clipped - cspans)
            fillData->unclipped_blend(clipped - cspans, cspans, fillData);
    }
}

/*
    \internal
    Clip spans to \a{clip}-rectangle.
    Returns number of unclipped spans
*/
static int qt_intersect_spans(QT_FT_Span *spans, int numSpans,
                              const QRect &clip)
{
    const short minx = clip.left();
    const short miny = clip.top();
    const short maxx = clip.right();
    const short maxy = clip.bottom();

    int n = 0;
    for (int i = 0; i < numSpans; ++i) {
        if (spans[i].y > maxy)
            break;
        if (spans[i].y < miny
            || spans[i].x > maxx
            || spans[i].x + spans[i].len <= minx) {
            continue;
        }
        if (spans[i].x < minx) {
            spans[n].len = qMin(spans[i].len - (minx - spans[i].x), maxx - minx + 1);
            spans[n].x = minx;
        } else {
            spans[n].x = spans[i].x;
            spans[n].len = qMin(spans[i].len, ushort(maxx - spans[n].x + 1));
        }
        if (spans[n].len == 0)
            continue;
        spans[n].y = spans[i].y;
        spans[n].coverage = spans[i].coverage;

        ++n;
    }
    return n;
}

/*
    \internal
    Clip spans to \a{clip}-region.
    Returns number of unclipped spans
*/
static int qt_intersect_spans(QT_FT_Span *spans, int numSpans,
                              int *currSpan,
                              QT_FT_Span *outSpans, int maxOut,
                              const QRegion &clip)
{
    const QVector<QRect> rects = clip.rects();
    const int numRects = rects.size();

    int r = 0;
    short miny, minx, maxx, maxy;
    {
        const QRect &rect = rects[0];
        miny = rect.top();
        minx = rect.left();
        maxx = rect.right();
        maxy = rect.bottom();
    }

    // TODO: better mapping of currY and startRect

    int n = 0;
    int i = *currSpan;
    int currY = spans[i].y;
    while (i < numSpans) {

        if (spans[i].y != currY && r != 0) {
            currY = spans[i].y;
            r = 0;
            const QRect &rect = rects[r];
            miny = rect.top();
            minx = rect.left();
            maxx = rect.right();
            maxy = rect.bottom();
        }

        if (spans[i].y < miny) {
            ++i;
            continue;
        }

        if (spans[i].y > maxy || spans[i].x > maxx) {
            if (++r >= numRects) {
                ++i;
                continue;
            }

            const QRect &rect = rects[r];
            miny = rect.top();
            minx = rect.left();
            maxx = rect.right();
            maxy = rect.bottom();
            continue;
        }

        if (spans[i].x + spans[i].len <= minx) {
            ++i;
            continue;
        }

        outSpans[n].y = spans[i].y;
        outSpans[n].coverage = spans[i].coverage;

        if (spans[i].x < minx) {
            const ushort cutaway = minx - spans[i].x;
            outSpans[n].len = qMin(spans[i].len - cutaway, maxx - minx + 1);
            outSpans[n].x = minx;
            if (outSpans[n].len == spans[i].len - cutaway) {
                ++i;
            } else {
                // span wider than current rect
                spans[i].len -= outSpans[n].len + cutaway;
                spans[i].x = maxx + 1;
            }
        } else { // span starts inside current rect
            outSpans[n].x = spans[i].x;
            outSpans[n].len = qMin(spans[i].len,
                                   ushort(maxx - spans[i].x + 1));
            if (outSpans[n].len == spans[i].len) {
                ++i;
            } else {
                // span wider than current rect
                spans[i].len -= outSpans[n].len;
                spans[i].x = maxx + 1;
            }
        }

        if (++n >= maxOut)
            break;
    }

    *currSpan = i;
    return n;
}

static void qt_span_fill_clipRect(int count, const QSpan *spans,
                                  void *userData)
{
    QSpanData *fillData = reinterpret_cast<QSpanData *>(userData);
    Q_ASSERT(fillData->blend && fillData->unclipped_blend);

    QRasterBuffer *rb = fillData->rasterBuffer;
    Q_ASSERT(!rb->clipRect.isEmpty());

    // hw: check if this const_cast<> is safe!!!
    count = qt_intersect_spans(const_cast<QSpan*>(spans), count,
                               rb->clipRect);
    if (count > 0)
        fillData->unclipped_blend(count, spans, fillData);
}

static void qt_span_fill_clipRegion(int count, const QSpan *spans,
                                    void *userData)
{
    QSpanData *fillData = reinterpret_cast<QSpanData *>(userData);
    Q_ASSERT(fillData->blend && fillData->unclipped_blend);

    QRasterBuffer *rb = fillData->rasterBuffer;
    Q_ASSERT(!rb->clipRegion.isEmpty());

    const int NSPANS = 256;
    QSpan cspans[NSPANS];
    int currentClip = 0;
    while (currentClip < count) {
        const int unclipped = qt_intersect_spans(const_cast<QSpan*>(spans),
                                                 count, &currentClip,
                                                 &cspans[0], NSPANS,
                                                 rb->clipRegion);
        if (unclipped > 0)
            fillData->unclipped_blend(unclipped, cspans, fillData);
    }

}

static void qt_span_clip(int count, const QSpan *spans, void *userData)
{
    ClipData *clipData = reinterpret_cast<ClipData *>(userData);
//     qDebug() << " qt_span_clip: " << count << clipData->operation;
//      for (int i = 0; i < count; ++i) {
//           qDebug() << "    " << spans[i].x << spans[i].y << spans[i].len << spans[i].coverage;
//      }

    switch (clipData->operation) {

    case Qt::IntersectClip:
        {
            QClipData *newClip = clipData->newClip;
            int currentClip = 0;
            const QSpan *end = spans + count;
            while (spans < end) {
                QSpan *newspans = newClip->spans + newClip->count;
                spans = qt_intersect_spans(clipData->oldClip, &currentClip, spans, end,
                                           &newspans, newClip->allocated - newClip->count);
                newClip->count = newspans - newClip->spans;
                if (spans < end) {
                    newClip->allocated *= 2;
                    newClip->spans = (QSpan *)realloc(newClip->spans, newClip->allocated*sizeof(QSpan));
                }
            }
        }
        break;

    case Qt::UniteClip:
    case Qt::ReplaceClip:
        clipData->newClip->appendSpans(spans, count);
        break;
    case Qt::NoClip:
        break;
    }
}

#ifndef QT_NO_DEBUG
QImage QRasterBuffer::clipImage() const
{
    QImage image(m_width, m_height, QImage::Format_ARGB32_Premultiplied);
    image.fill(qRgb(0, 0, 0));

    for (int y = 0; y < m_height; ++y) {
        QSpan *spans = clip->clipLines[y].spans;
        int count = clip->clipLines[y].count;

        while (count--) {
            for (int x=spans->x; x<spans->x + spans->len; ++x) {
                QRgb pixel = image.pixel(x, y);
                image.setPixel(x, y, qRgb(spans->coverage, qGreen(pixel) + 10, 0));
            }
            ++spans;
        }
    }
    return image;
}

QImage QRasterBuffer::bufferImage() const
{
    QImage image(m_width, m_height, QImage::Format_ARGB32_Premultiplied);

    for (int y = 0; y < m_height; ++y) {
        uint *span = (uint *)const_cast<QRasterBuffer *>(this)->scanLine(y);

        for (int x=0; x<m_width; ++x) {
            uint argb = span[x];
            image.setPixel(x, y, argb);
        }
    }
    return image;
}
#endif


void QRasterBuffer::flushToARGBImage(QImage *target) const
{
    int w = qMin(m_width, target->width());
    int h = qMin(m_height, target->height());

    for (int y=0; y<h; ++y) {
        uint *sourceLine = (uint *)const_cast<QRasterBuffer *>(this)->scanLine(y);
        QRgb *dest = (QRgb *) target->scanLine(y);
        for (int x=0; x<w; ++x) {
            QRgb pixel = sourceLine[x];
            int alpha = qAlpha(pixel);
            if (!alpha) {
                dest[x] = 0;
            } else {
                dest[x] = (alpha << 24)
                        | ((255*qRed(pixel)/alpha) << 16)
                        | ((255*qGreen(pixel)/alpha) << 8)
                        | ((255*qBlue(pixel)/alpha) << 0);
            }
        }
    }
}


class QGradientCache
{
    struct CacheInfo
    {
        inline CacheInfo(QGradientStops s, int op) :
            stops(s), opacity(op) {}
        uint buffer[GRADIENT_STOPTABLE_SIZE];
        QGradientStops stops;
        int opacity;
    };

    typedef QMultiHash<quint64, CacheInfo> QGradientColorTableHash;

public:
    inline const uint *getBuffer(const QGradientStops &stops, int opacity) {
        quint64 hash_val = 0;

        for (int i = 0; i < stops.size() && i <= 2; i++)
            hash_val += stops[i].second.rgba();

        QGradientColorTableHash::const_iterator it = cache.constFind(hash_val);

        if (it == cache.constEnd())
            return addCacheElement(hash_val, stops, opacity);
        else {
            do {
                const CacheInfo &cache_info = it.value();
                if (cache_info.stops == stops && cache_info.opacity == opacity)
                    return cache_info.buffer;
                ++it;
            } while (it != cache.constEnd() && it.key() == hash_val);
            // an exact match for these stops and opacity was not found, create new cache
            return addCacheElement(hash_val, stops, opacity);
        }
    }

    inline int paletteSize() const { return GRADIENT_STOPTABLE_SIZE; }
protected:
    inline int maxCacheSize() const { return 60; }
    inline void generateGradientColorTable(const QGradientStops& s,
                                           uint *colorTable,
                                           int size, int opacity) const;
    uint *addCacheElement(quint64 hash_val, const QGradientStops &stops, int opacity) {
        if (cache.size() == maxCacheSize()) {
            int elem_to_remove = qrand() % maxCacheSize();
            cache.remove(cache.keys()[elem_to_remove]); // may remove more than 1, but OK
        }
        CacheInfo cache_entry(stops, opacity);
        generateGradientColorTable(stops, cache_entry.buffer, paletteSize(), opacity);
        return cache.insert(hash_val, cache_entry).value().buffer;
    }

    QGradientColorTableHash cache;
};

void QGradientCache::generateGradientColorTable(const QGradientStops& stops, uint *colorTable, int size, int opacity) const
{
    int stopCount = stops.count();
    Q_ASSERT(stopCount > 0);

    // The position where the gradient begins and ends
    int begin_pos = int(stops[0].first * size);
    int end_pos = int(stops[stopCount-1].first * size);

    int pos = 0; // The position in the color table.

    uint current_color;
    uint next_color;

    // adjust begin position in case when stops.size == 2 and
    //  stops[0].pos = 1 and stops[1].pos == 1
    if (begin_pos == end_pos && begin_pos > 0)
        --begin_pos;

     // Up to first point
    current_color = PREMUL(ARGB_COMBINE_ALPHA(stops[0].second.rgba(), opacity));
    while (pos <= begin_pos) {
        colorTable[pos] = current_color;
        ++pos;
    }

    qreal incr = 1 / qreal(size); // the double increment.
    qreal dpos = incr * pos; // The position in terms of 0-1.
    qreal diff;

    int current_stop = 0; // We always interpolate between current and current + 1.

    // Gradient area
    if (pos < end_pos) {
        next_color = PREMUL(ARGB_COMBINE_ALPHA(stops[1].second.rgba(), opacity));
        diff = stops[1].first - stops[0].first;
    }
    while (pos < end_pos) {

        Q_ASSERT(current_stop < stopCount);

        int dist;
        if (diff != 0.0)
            dist = (int)(256*(dpos - stops[current_stop].first) / diff);
        else
            dist = 0;
        int idist = 256 - dist;

        colorTable[pos] = INTERPOLATE_PIXEL_256(current_color, idist, next_color, dist);

        ++pos;
        dpos += incr;

        if (dpos > stops[current_stop+1].first) {
            ++current_stop;
            if (pos >= end_pos)
                break;
            current_color = next_color;
            next_color = PREMUL(ARGB_COMBINE_ALPHA(stops[current_stop+1].second.rgba(), opacity));
            diff = (stops[current_stop+1].first - stops[current_stop].first);
        }
    }

    // After last point
    current_color = PREMUL(ARGB_COMBINE_ALPHA(stops[stopCount - 1].second.rgba(), opacity));
    while (pos < size) {
        colorTable[pos] = current_color;
        ++pos;
    }

    // Make sure the last color stop is represented at the end of the table
    colorTable[size - 1] = current_color;
}

Q_GLOBAL_STATIC(QGradientCache, qt_gradient_cache)


void QSpanData::init(QRasterBuffer *rb, QRasterPaintEngine *pe)
{
    rasterBuffer = rb;
#ifdef Q_WS_QWS
    rasterEngine = pe;
#else
    Q_UNUSED(pe);
#endif
    type = None;
    txop = 0;
    bilinear = false;
    m11 = m22 = 1.;
    m12 = m13 = m21 = m23 = dx = dy = 0.;
}

extern QImage qt_imageForBrush(int brushStyle, bool invert);

void QSpanData::setup(const QBrush &brush, int alpha)
{
    Qt::BrushStyle brushStyle = brush.style();
    switch (brushStyle) {
    case Qt::SolidPattern:
        type = Solid;
        solid.color = PREMUL(ARGB_COMBINE_ALPHA(brush.color().rgba(), alpha));
        break;

    case Qt::LinearGradientPattern:
        {
            type = LinearGradient;
            const QLinearGradient *g = static_cast<const QLinearGradient *>(brush.gradient());
            gradient.alphaColor = !brush.isOpaque() || alpha != 256;
            gradient.colorTable = const_cast<uint*>(qt_gradient_cache()->getBuffer(g->stops(), alpha));
            gradient.spread = g->spread();

            QLinearGradientData &linearData = gradient.linear;

            linearData.origin.x = g->start().x();
            linearData.origin.y = g->start().y();
            linearData.end.x = g->finalStop().x();
            linearData.end.y = g->finalStop().y();
            break;
        }

    case Qt::RadialGradientPattern:
        {
            type = RadialGradient;
            const QRadialGradient *g = static_cast<const QRadialGradient *>(brush.gradient());
            gradient.alphaColor = !brush.isOpaque() || alpha != 256;
            gradient.colorTable = const_cast<uint*>(qt_gradient_cache()->getBuffer(g->stops(), alpha));
            gradient.spread = g->spread();

            QRadialGradientData &radialData = gradient.radial;

            QPointF center = g->center();
            radialData.center.x = center.x();
            radialData.center.y = center.y();
            QPointF focal = g->focalPoint();
            radialData.focal.x = focal.x();
            radialData.focal.y = focal.y();
            radialData.radius = g->radius();
        }
        break;

    case Qt::ConicalGradientPattern:
        {
            type = ConicalGradient;
            const QConicalGradient *g = static_cast<const QConicalGradient *>(brush.gradient());
            gradient.alphaColor = !brush.isOpaque() || alpha != 256;
            gradient.colorTable = const_cast<uint*>(qt_gradient_cache()->getBuffer(g->stops(), alpha));
            gradient.spread = QGradient::RepeatSpread;

            QConicalGradientData &conicalData = gradient.conical;

            QPointF center = g->center();
            conicalData.center.x = center.x();
            conicalData.center.y = center.y();
            conicalData.angle = g->angle() * 2 * Q_PI / 360.0;
        }
        break;

    case Qt::Dense1Pattern:
    case Qt::Dense2Pattern:
    case Qt::Dense3Pattern:
    case Qt::Dense4Pattern:
    case Qt::Dense5Pattern:
    case Qt::Dense6Pattern:
    case Qt::Dense7Pattern:
    case Qt::HorPattern:
    case Qt::VerPattern:
    case Qt::CrossPattern:
    case Qt::BDiagPattern:
    case Qt::FDiagPattern:
    case Qt::DiagCrossPattern:
        type = Texture;
        if (!tempImage)
            tempImage = new QImage();
        *tempImage = rasterBuffer->colorizeBitmap(qt_imageForBrush(brushStyle, true), brush.color());
        initTexture(tempImage, alpha, QTextureData::Tiled);
        break;
    case Qt::TexturePattern:
        type = Texture;
        if (!tempImage)
            tempImage = new QImage();

        if (qHasPixmapTexture(brush) && brush.texture().isQBitmap())
            *tempImage = rasterBuffer->colorizeBitmap(brush.textureImage(), brush.color());
        else
            *tempImage = brush.textureImage();
        initTexture(tempImage, alpha, !brush.d->sourceRect.isNull() ? QTextureData::Plain : QTextureData::Tiled, brush.d->sourceRect);
        break;

    case Qt::NoBrush:
    default:
        type = None;
        break;
    }
    adjustSpanMethods();
}

void QSpanData::adjustSpanMethods()
{
    bitmapBlit = 0;
    alphamapBlit = 0;
    fillRect = 0;

    switch(type) {
    case None:
        unclipped_blend = 0;
        break;
    case Solid:
        unclipped_blend = rasterBuffer->drawHelper->blendColor;
        bitmapBlit = rasterBuffer->drawHelper->bitmapBlit;
        alphamapBlit = rasterBuffer->drawHelper->alphamapBlit;
        fillRect = rasterBuffer->drawHelper->fillRect;
        break;
    case LinearGradient:
    case RadialGradient:
    case ConicalGradient:
        unclipped_blend = rasterBuffer->drawHelper->blendGradient;
        break;
    case Texture:
#ifdef Q_WS_QWS
        if (!rasterBuffer->buffer())
            unclipped_blend = qBlendTextureCallback;
        else
            unclipped_blend = qBlendTexture;
#else
        unclipped_blend = qBlendTexture;
#endif
        break;
    }
    // setup clipping
    if (!unclipped_blend)
        blend = 0;
    else if (!rasterBuffer->clipEnabled)
        blend = unclipped_blend;
    else if (!rasterBuffer->clipRect.isEmpty())
        blend = qt_span_fill_clipRect;
    else if (!rasterBuffer->clipRegion.isEmpty())
        blend = qt_span_fill_clipRegion;
    else if (rasterBuffer->clip)
        blend = qt_span_fill_clipped;
    else
        blend = unclipped_blend;
}

void QSpanData::setupMatrix(const QTransform &matrix, int bilin)
{
    QTransform inv = matrix.inverted();
    m11 = inv.m11();
    m12 = inv.m12();
    m13 = inv.m13();
    m21 = inv.m21();
    m22 = inv.m22();
    m23 = inv.m23();
    dx = inv.dx();
    dy = inv.dy();
    txop = inv.type();
    bilinear = bilin;

    const bool affine = !m13 && !m23;
    fast_matrix = affine
        && m11 * m11 + m21 * m21 < 1e4
        && m12 * m12 + m22 * m22 < 1e4
        && qAbs(dx) < 1e4
        && qAbs(dy) < 1e4;

    adjustSpanMethods();
}

extern const QVector<QRgb> *qt_image_colortable(const QImage &image);

void QSpanData::initTexture(const QImage *image, int alpha, QTextureData::Type _type, const QRect &sourceRect)
{
    const QImageData *d = const_cast<QImage *>(image)->data_ptr();
    if (!d || d->height == 0) {
        texture.imageData = 0;
        texture.width = 0;
        texture.height = 0;
        texture.x1 = 0;
        texture.y1 = 0;
        texture.x2 = 0;
        texture.y2 = 0;
        texture.bytesPerLine = 0;
        texture.format = QImage::Format_Invalid;
        texture.colorTable = 0;
        texture.hasAlpha = alpha != 256;
    } else {
        texture.imageData = d->data;
        texture.width = d->width;
        texture.height = d->height;

        if (sourceRect.isNull()) {
            texture.x1 = 0;
            texture.y1 = 0;
            texture.x2 = texture.width;
            texture.y2 = texture.height;
        } else {
            texture.x1 = sourceRect.x();
            texture.y1 = sourceRect.y();
            texture.x2 = qMin(texture.x1 + sourceRect.width(), d->width);
            texture.y2 = qMin(texture.y1 + sourceRect.height(), d->height);
        }

        texture.bytesPerLine = d->nbytes / d->height;
        texture.format = d->format;
        texture.colorTable = (d->format <= QImage::Format_Indexed8 && !d->colortable.isEmpty()) ? &d->colortable : 0;
        texture.hasAlpha = image->hasAlphaChannel() || alpha != 256;
    }
    texture.const_alpha = alpha;
    texture.type = _type;

    adjustSpanMethods();
}

#ifdef Q_WS_WIN
static void draw_text_item_win(const QPointF &_pos, const QTextItemInt &ti, HDC hdc,
                               bool convertToText, const QTransform &xform, const QPointF &topLeft)
{

    // Make sure we translate for systems that can't handle world transforms
    QPointF pos(QT_WA_INLINE(_pos, _pos + QPointF(xform.dx(), xform.dy())));
    QFontEngine *fe = ti.fontEngine;
    QPointF baseline_pos = xform.inverted().map(xform.map(pos) - topLeft);

    SetTextAlign(hdc, TA_BASELINE);
    SetBkMode(hdc, TRANSPARENT);

    bool has_kerning = ti.f && ti.f->kerning();
    QFontEngineWin *winfe = (fe->type() == QFontEngine::Win) ? static_cast<QFontEngineWin *>(fe) : 0;

    HFONT hfont;
    bool ttf = false;
    bool useTextOutA = false;

    if (winfe) {
        hfont = winfe->hfont;
        ttf = winfe->ttf;
        useTextOutA = winfe->useTextOutA;
    } else {
        hfont = (HFONT)GetStockObject(ANSI_VAR_FONT);
    }

    HGDIOBJ old_font = SelectObject(hdc, hfont);
    unsigned int options = (ttf && !convertToText) ? ETO_GLYPH_INDEX : 0;
    wchar_t *convertedGlyphs = (wchar_t *)ti.chars;
    QGlyphLayout *glyphs = ti.glyphs;

    if (!(ti.flags & QTextItem::RightToLeft) && useTextOutA) {
        qreal x = pos.x();
        qreal y = pos.y();

        // hack to get symbol fonts working on Win95. See also QFontEngine constructor
        // can only happen if !ttf
        for(int i = 0; i < ti.num_glyphs; i++) {
            QString str(QChar(glyphs->glyph));
            QT_WA({
                TextOutW(hdc, qRound(x + glyphs->offset.x.toReal()),
                        qRound(y + glyphs->offset.y.toReal()),
                        (LPWSTR)str.utf16(), str.length());
            } , {
                QByteArray cstr = str.toLocal8Bit();
                TextOutA(hdc, qRound(x + glyphs->offset.x.toReal()),
                        qRound(y + glyphs->offset.y.toReal()),
                        cstr.data(), cstr.length());
            });
            x += glyphs->effectiveAdvance().toReal();
            glyphs++;
        }
    } else {
        bool fast = !has_kerning;
        for(int i = 0; i < ti.num_glyphs; i++) {
            if (glyphs[i].offset.x != 0 || glyphs[i].offset.y != 0 || glyphs[i].justification.space_18d6 != 0
                || glyphs[i].attributes.dontPrint) {
                fast = false;
                break;
            }
        }

#if !defined(Q_OS_WINCE)
        // Scale, rotate and translate here. This is only valid for systems > Windows Me.
        // We should never get here on Windows Me or lower if the transformation specifies
        // scaling or rotation.
        QT_WA({
            XFORM win_xform;
            win_xform.eM11 = xform.m11();
            win_xform.eM12 = xform.m12();
            win_xform.eM21 = xform.m21();
            win_xform.eM22 = xform.m22();
            win_xform.eDx = xform.dx();
            win_xform.eDy = xform.dy();
            SetGraphicsMode(hdc, GM_ADVANCED);
            SetWorldTransform(hdc, &win_xform);
        }, {
            // nothing
        });
#endif

        if (fast) {
            // fast path
            QVarLengthArray<wchar_t> g(ti.num_glyphs);
            for (int i = 0; i < ti.num_glyphs; ++i)
                g[i] = glyphs[i].glyph;
            ExtTextOutW(hdc,
                        qRound(baseline_pos.x() + glyphs->offset.x.toReal()),
                        qRound(baseline_pos.y() + glyphs->offset.y.toReal()),
                        options, 0, convertToText ? convertedGlyphs : g.data(), ti.num_glyphs, 0);
        } else {
            QVarLengthArray<QFixedPoint> positions;
            QVarLengthArray<glyph_t> _glyphs;

            QTransform matrix;
            matrix.translate(baseline_pos.x(), baseline_pos.y());
            ti.fontEngine->getGlyphPositions(ti.glyphs, ti.num_glyphs, matrix, ti.flags,
                _glyphs, positions);
            if (_glyphs.size() == 0) {
                SelectObject(hdc, old_font);
                return;
            }

            convertToText = convertToText && ti.num_glyphs == _glyphs.size();

            bool outputEntireItem = (QSysInfo::WindowsVersion & QSysInfo::WV_NT_based)
                && QSysInfo::WindowsVersion != QSysInfo::WV_NT
                && _glyphs.size() > 0;

            if (outputEntireItem) {
                options |= ETO_PDY;
                QVarLengthArray<INT> glyphDistances(_glyphs.size() * 2);
                QVarLengthArray<wchar_t> g(_glyphs.size());
                for (int i=0; i<_glyphs.size() - 1; ++i) {
                    glyphDistances[i * 2] = qRound(positions[i + 1].x) - qRound(positions[i].x);
                    glyphDistances[i * 2 + 1] = qRound(positions[i + 1].y) - qRound(positions[i].y);
                    g[i] = _glyphs[i];
                }
                glyphDistances[(_glyphs.size() - 1) * 2] = 0;
                glyphDistances[(_glyphs.size() - 1) * 2 + 1] = 0;
                g[_glyphs.size() - 1] = _glyphs[_glyphs.size() - 1];
                ExtTextOutW(hdc, qRound(positions[0].x), qRound(positions[0].y), options, 0,
                            convertToText ? convertedGlyphs : g.data(), _glyphs.size(),
                            glyphDistances.data());
            } else {
                int i = 0;
                while(i < _glyphs.size()) {
                    wchar_t g = _glyphs[i];

                    ExtTextOutW(hdc, qRound(positions[i].x),
                        qRound(positions[i].y), options, 0,
                                convertToText ? convertedGlyphs + i : &g, 1, 0);
                    ++i;
                }
            }
        }

#if !defined(Q_OS_WINCE)
        QT_WA({
            XFORM win_xform;
            win_xform.eM11 = win_xform.eM22 = 1.0;
            win_xform.eM12 = win_xform.eM21 = win_xform.eDx = win_xform.eDy = 0.0;
            SetWorldTransform(hdc, &win_xform);
        }, {
            // nothing
        });
#endif
    }
    SelectObject(hdc, old_font);
}

void qt_draw_text_item(const QPointF &pos, const QTextItemInt &ti, HDC hdc,
                       bool convertToText, const QTransform &xform, const QPointF &topLeft)
{
    Q_ASSERT(ti.fontEngine->type() != QFontEngine::Multi);
    draw_text_item_win(pos, ti, hdc, convertToText, xform, topLeft);
}



#endif


/*!
    \internal

    Draws a line using the floating point midpoint algorithm. The line
    \a line is already in device coords at this point.
*/

static void drawLine_midpoint_i(int x1, int y1, int x2, int y2, ProcessSpans span_func, QSpanData *data,
                                LineDrawMode style, const QRect &devRect)
{
#ifdef QT_DEBUG_DRAW
    qDebug() << "   - drawLine_midpoint_i" << QLine(QPoint(x1, y1), QPoint(x2, y2));
#endif

    int x, y;
    int dx, dy, d, incrE, incrNE;

    dx = x2 - x1;
    dy = y2 - y1;

    const int NSPANS = 256;
    QT_FT_Span spans[NSPANS];
    int current = 0;
    bool ordered = true;

    if (dy == 0) {
        // specialcase horizontal lines
        if (y1 >= 0 && y1 < devRect.height()) {
            int start = qMax(0, qMin(x1, x2));
            int stop = qMax(x1, x2) + 1;
            int stop_clipped = qMin(devRect.width(), stop);
            int len = stop_clipped - start;
            if (style == LineDrawNormal && stop == stop_clipped)
                len--;
            if (len > 0) {
                spans[0].x = ushort(start);
                spans[0].len = ushort(len);
                spans[0].y = y1;
                spans[0].coverage = 255;
                span_func(1, spans, data);
            }
        }
        return;
    } else if (dx == 0) {
        // specialcase vertical lines
        if (x1 >= 0 && x1 < devRect.width()) {
            int start = qMax(0, qMin(y1, y2));
            int stop = qMax(y1, y2) + 1;
            int stop_clipped = qMin(devRect.height(), stop);
            int len = stop_clipped - start;
            if (style == LineDrawNormal && stop == stop_clipped)
                len--;
            // hw: create spans directly instead to possibly avoid clipping
            if (len > 0)
                fillRect(QRect(x1, start, 1, len), data, 0);
        }
        return;
    }


    if (qAbs(dx) >= qAbs(dy)) {       /* if x is the major axis: */

        if (x2 < x1) {  /* if coordinates are out of order */
            qt_swap_int(x1, x2);
            dx = -dx;

            qt_swap_int(y1, y2);
            dy = -dy;
        }

        if (style == LineDrawNormal)
            --x2;

        // In the loops below we increment before call the span function so
        // we need to stop one pixel before
        x2 = qMin(x2, devRect.width() - 1);

        // completely clipped, so abort
        if (x2 <= x1) {
            return;
        }

        int x = x1;
        int y = y1;

        if (y2 <= y1)
            ordered = false;

        {
            const int index = (ordered ? current : NSPANS - 1 - current);
            spans[index].coverage = 255;
            spans[index].x = x;
            spans[index].y = y;

            if (x >= 0 && y >= 0 && y < devRect.height())
                spans[index].len = 1;
            else
                spans[index].len = 0;
        }

        if (y2 > y1) { // 315 -> 360 and 135 -> 180 (unit circle degrees)
            y2 = qMin(y2, devRect.height() - 1);

            incrE = dy * 2;
            d = incrE - dx;
            incrNE = (dy - dx) * 2;

            if (y > y2)
                goto flush_and_return;

            while (x < x2) {
                ++x;
                if (d > 0) {
                    if (spans[current].len > 0)
                        ++current;
                    if (current == NSPANS) {
                        span_func(NSPANS, spans, data);
                        current = 0;
                    }

                    ++y;
                    d += incrNE;
                    if (y > y2)
                        goto flush_and_return;

                    spans[current].len = 0;
                    spans[current].coverage = 255;
                    spans[current].x = x;
                    spans[current].y = y;
                } else {
                    d += incrE;
                    if (x == 0)
                        spans[current].x = 0;
                }

                if (x < 0 || y < 0)
                    continue;

                Q_ASSERT(x<devRect.width());
                Q_ASSERT(y<devRect.height());
                Q_ASSERT(spans[current].y == y);
                spans[current].len++;
            }
            if (spans[current].len > 0) {
                ++current;
            }
        } else {  // 0-45 and 180->225 (unit circle degrees)

            y1 = qMin(y1, devRect.height() - 1);

            incrE = dy * 2;
            d = incrE + dx;
            incrNE = (dy + dx) * 2;

            if (y < 0)
                goto flush_and_return;

            while (x < x2) {
                ++x;
                if (d < 0) {
                    if (spans[NSPANS - 1 - current].len > 0)
                        ++current;
                    if (current == NSPANS) {
                        span_func(NSPANS, spans, data);
                        current = 0;
                    }

                    --y;
                    d += incrNE;
                    if (y < 0)
                        goto flush_and_return;

                    const int index = NSPANS - 1 - current;
                    spans[index].len = 0;
                    spans[index].coverage = 255;
                    spans[index].x = x;
                    spans[index].y = y;
                } else {
                    d += incrE;
                    if (x == 0)
                        spans[NSPANS - 1 - current].x = 0;
                }

                if (x < 0 || y > y1)
                    continue;

                Q_ASSERT(x<devRect.width() && y<devRect.height());
                Q_ASSERT(spans[NSPANS - 1 - current].y == y);
                spans[NSPANS - 1 - current].len++;
            }
            if (spans[NSPANS - 1 - current].len > 0) {
                ++current;
            }
        }

    } else {

        // if y is the major axis:

        if (y2 < y1) {      /* if coordinates are out of order */
            qt_swap_int(y1, y2);
            dy = -dy;

            qt_swap_int(x1, x2);
            dx = -dx;
        }

        if (style == LineDrawNormal)
            --y2;

        // In the loops below we increment before call the span function so
        // we need to stop one pixel before
        y2 = qMin(y2, devRect.height() - 1);

        // completely clipped, so abort
        if (y2 <= y1) {
            return;
        }

        x = x1;
        y = y1;

        if (x>=0 && y>=0 && x < devRect.width()) {
            Q_ASSERT(x >= 0 && y >= 0 && x < devRect.width() && y < devRect.height());
            if (current == NSPANS) {
                span_func(NSPANS, spans, data);
                current = 0;
            }
            spans[current].len = 1;
            spans[current].coverage = 255;
            spans[current].x = x;
            spans[current].y = y;
            ++current;
        }

        if (x2 > x1) { // 90 -> 135 and 270 -> 315 (unit circle degrees)
            x2 = qMin(x2, devRect.width() - 1);
            incrE = dx * 2;
            d = incrE - dy;
            incrNE = (dx - dy) * 2;

            if (x > x2)
                goto flush_and_return;

            while (y < y2) {
                if (d > 0) {
                    ++x;
                    d += incrNE;
                    if (x > x2)
                        goto flush_and_return;
                } else {
                    d += incrE;
                }
                ++y;
                if (x < 0 || y < 0)
                    continue;
                Q_ASSERT(x<devRect.width() && y<devRect.height());
                if (current == NSPANS) {
                    span_func(NSPANS, spans, data);
                    current = 0;
                }
                spans[current].len = 1;
                spans[current].coverage = 255;
                spans[current].x = x;
                spans[current].y = y;
                ++current;
            }
        } else { // 45 -> 90 and 225 -> 270 (unit circle degrees)
            x1 = qMin(x1, devRect.width() - 1);
            incrE = dx * 2;
            d = incrE + dy;
            incrNE = (dx + dy) * 2;

            if (x < 0)
                goto flush_and_return;

            while (y < y2) {
                if (d < 0) {
                    --x;
                    d += incrNE;
                    if (x < 0)
                        goto flush_and_return;
                } else {
                    d += incrE;
                }
                ++y;
                if (y < 0 || x > x1)
                    continue;
                Q_ASSERT(x>=0 && x<devRect.width() && y>=0 && y<devRect.height());
                if (current == NSPANS) {
                    span_func(NSPANS, spans, data);
                    current = 0;
                }
                spans[current].len = 1;
                spans[current].coverage = 255;
                spans[current].x = x;
                spans[current].y = y;
                ++current;
            }
        }
    }
flush_and_return:
    if (current > 0)
        span_func(current, ordered ? spans : spans + (NSPANS - current), data);
}

static void offset_pattern(int offset, bool *inDash, int *dashIndex, int *currentOffset, const QVarLengthArray<qreal> &pattern)
{
    while (offset--) {
        if (--*currentOffset == 0) {
            *inDash = !*inDash;
            *dashIndex = ((*dashIndex + 1) % pattern.size());
            *currentOffset = int(pattern[*dashIndex]);
        }
    }
}

static void drawLine_midpoint_dashed_i(int x1, int y1, int x2, int y2,
                                       QPen *pen,
                                       ProcessSpans span_func, QSpanData *data,
                                       LineDrawMode style, const QRect &devRect,
                                       int *patternOffset)
{
#ifdef QT_DEBUG_DRAW
    qDebug() << "   - drawLine_midpoint_dashed_i" << x1 << y1 << x2 << y2 << *patternOffset;
#endif

    int x, y;
    int dx, dy, d, incrE, incrNE;

    dx = x2 - x1;
    dy = y2 - y1;

    Q_ASSERT(*patternOffset >= 0);

    const QVector<qreal> penPattern = pen->dashPattern();
    QVarLengthArray<qreal> pattern(penPattern.size());

    int patternLength = 0;
    for (int i = 0; i < penPattern.size(); ++i)
        patternLength += int(penPattern.at(i));

    // pattern must be reversed if coordinates are out of order
    int reverseLength = -1;
    if (dy == 0 && x1 > x2)
        reverseLength = x1 - x2;
    else if (dx == 0 && y1 > y2)
        reverseLength = y1 - y2;
    else if (qAbs(dx) >= qAbs(dy) && x2 < x1) // x major axis
        reverseLength = qAbs(dx);
    else if (qAbs(dy) >= qAbs(dx) && y2 < y1) // y major axis
        reverseLength = qAbs(dy);

    const bool reversed = (reverseLength > -1);
    if (reversed) { // reverse pattern
        for (int i = 0; i < penPattern.size(); ++i)
            pattern[penPattern.size() - 1 - i] = penPattern.at(i);

        *patternOffset = (patternLength - 1 - *patternOffset);
        *patternOffset += patternLength - (reverseLength % patternLength);
        *patternOffset = *patternOffset % patternLength;
    } else {
        for (int i = 0; i < penPattern.size(); ++i)
            pattern[i] = penPattern.at(i);
    }

    int dashIndex = 0;
    bool inDash = !reversed;
    int currPattern = int(pattern[dashIndex]);

    // adjust pattern for offset
    offset_pattern(*patternOffset, &inDash, &dashIndex, &currPattern, pattern);

    const int NSPANS = 256;
    QT_FT_Span spans[NSPANS];
    int current = 0;
    bool ordered = true;

    if (dy == 0) {
        // specialcase horizontal lines
        if (y1 >= 0 && y1 < devRect.height()) {
            int start_unclipped = qMin(x1, x2);
            int start = qMax(0, start_unclipped);
            int stop = qMax(x1, x2) + 1;
            int stop_clipped = qMin(devRect.width(), stop);
            int len = stop_clipped - start;
            if (style == LineDrawNormal && stop == stop_clipped)
                len--;

            // adjust pattern for starting offset
            offset_pattern(start - start_unclipped, &inDash, &dashIndex, &currPattern, pattern);

            if (len > 0) {
                int x = start;
                while (x < stop_clipped) {
                    if (current == NSPANS) {
                        span_func(NSPANS, spans, data);
                        current = 0;
                    }
                    const int dash = qMin(currPattern, stop_clipped - x);
                    if (inDash) {
                        spans[current].x = ushort(x);
                        spans[current].len = ushort(dash);
                        spans[current].y = y1;
                        spans[current].coverage = 255;
                        ++current;
                    }
                    if (dash < currPattern) {
                        currPattern -= dash;
                    } else {
                        dashIndex = (dashIndex + 1) % pattern.size();
                        currPattern = int(pattern[dashIndex]);
                        inDash = !inDash;
                    }
                    x += dash;
                }
            }
        }
        goto flush_and_return;
    } else if (dx == 0) {
        if (x1 >= 0 && x1 < devRect.width()) {
            int start_unclipped = qMin(y1, y2);
            int start = qMax(0, start_unclipped);
            int stop = qMax(y1, y2) + 1;
            int stop_clipped = qMin(devRect.height(), stop);
            if (style == LineDrawNormal && stop == stop_clipped)
                --stop;
            else
                stop = stop_clipped;

            // adjust pattern for starting offset
            offset_pattern(start - start_unclipped, &inDash, &dashIndex, &currPattern, pattern);

            // loop over dashes
            int y = start;
            while (y < stop) {
                const int dash = qMin(currPattern, stop - y);
                if (inDash) {
                    for (int i = 0; i < dash; ++i) {
                        if (current == NSPANS) {
                            span_func(NSPANS, spans, data);
                            current = 0;
                        }
                        spans[current].x = x1;
                        spans[current].len = 1;
                        spans[current].coverage = 255;
                        spans[current].y = ushort(y + i);
                        ++current;
                    }
                }
                if (dash < currPattern) {
                    currPattern -= dash;
                } else {
                    dashIndex = (dashIndex + 1) % pattern.size();
                    currPattern = int(pattern[dashIndex]);
                    inDash = !inDash;
                }
                y += dash;
            }
        }
        goto flush_and_return;
    }

    if (qAbs(dx) >= qAbs(dy)) {       /* if x is the major axis: */

        if (x2 < x1) {  /* if coordinates are out of order */
            qt_swap_int(x1, x2);
            dx = -dx;

            qt_swap_int(y1, y2);
            dy = -dy;
        }

        if (style == LineDrawNormal)
            --x2;

        // In the loops below we increment before call the span function so
        // we need to stop one pixel before
        x2 = qMin(x2, devRect.width() - 1);

        // completely clipped, so abort
        if (x2 <= x1)
            goto flush_and_return;

        int x = x1;
        int y = y1;

        if (x >= 0 && y >= 0 && y < devRect.height()) {
            Q_ASSERT(x >= 0 && y >= 0 && x < devRect.width() && y < devRect.height());
            if (inDash) {
                if (current == NSPANS) {
                    span_func(NSPANS, spans, data);
                    current = 0;
                }
                spans[current].len = 1;
                spans[current].coverage = 255;
                spans[current].x = x;
                spans[current].y = y;
                ++current;
            }
            if (--currPattern <= 0) {
                inDash = !inDash;
                dashIndex = (dashIndex + 1) % pattern.size();
                currPattern = int(pattern[dashIndex]);
            }
        }

        if (y2 > y1) { // 315 -> 360 and 135 -> 180 (unit circle degrees)
            y2 = qMin(y2, devRect.height() - 1);

            incrE = dy * 2;
            d = incrE - dx;
            incrNE = (dy - dx) * 2;

            if (y > y2)
                goto flush_and_return;

            while (x < x2) {
                if (d > 0) {
                    ++y;
                    d += incrNE;
                    if (y > y2)
                        goto flush_and_return;
                } else {
                    d += incrE;
                }
                ++x;

                const bool skip = x < 0 || y < 0;
                Q_ASSERT(skip || x < devRect.width() && y < devRect.height());
                if (inDash && !skip) {
                    if (current == NSPANS) {
                        span_func(NSPANS, spans, data);
                        current = 0;
                    }
                    spans[current].len = 1;
                    spans[current].coverage = 255;
                    spans[current].x = x;
                    spans[current].y = y;
                    ++current;
                }
                if (--currPattern <= 0) {
                    inDash = !inDash;
                    dashIndex = (dashIndex + 1) % pattern.size();
                    currPattern = int(pattern[dashIndex]);
                }
            }
        } else {  // 0-45 and 180->225 (unit circle degrees)
            y1 = qMin(y1, devRect.height() - 1);

            incrE = dy * 2;
            d = incrE + dx;
            incrNE = (dy + dx) * 2;

            if (y < 0)
                goto flush_and_return;

            while (x < x2) {
                if (d < 0) {
                    if (current > 0) {
                        span_func(current, spans, data);
                        current = 0;
                    }

                    --y;
                    d += incrNE;
                    if (y < 0)
                        goto flush_and_return;
                } else {
                    d += incrE;
                }
                ++x;

                const bool skip = x < 0 || y > y1;
                Q_ASSERT(skip || x < devRect.width() && y < devRect.height());
                if (inDash && !skip) {
                    if (current == NSPANS) {
                        span_func(NSPANS, spans, data);
                        current = 0;
                    }
                    spans[current].len = 1;
                    spans[current].coverage = 255;
                    spans[current].x = x;
                    spans[current].y = y;
                    ++current;
                }
                if (--currPattern <= 0) {
                    inDash = !inDash;
                    dashIndex = (dashIndex + 1) % pattern.size();
                    currPattern = int(pattern[dashIndex]);
                }
            }
        }
    } else {

        // if y is the major axis:

        if (y2 < y1) {      /* if coordinates are out of order */
            qt_swap_int(y1, y2);
            dy = -dy;

            qt_swap_int(x1, x2);
            dx = -dx;
        }

        if (style == LineDrawNormal)
            --y2;

        // In the loops below we increment before call the span function so
        // we need to stop one pixel before
        y2 = qMin(y2, devRect.height() - 1);

        // completely clipped, so abort
        if (y2 <= y1)
            goto flush_and_return;

        x = x1;
        y = y1;

        if (x>=0 && y>=0 && x < devRect.width()) {
            Q_ASSERT(x >= 0 && y >= 0 && x < devRect.width() && y < devRect.height());
            if (inDash) {
                if (current == NSPANS) {
                    span_func(NSPANS, spans, data);
                    current = 0;
                }
                spans[current].len = 1;
                spans[current].coverage = 255;
                spans[current].x = x;
                spans[current].y = y;
                ++current;
            }
            if (--currPattern <= 0) {
                inDash = !inDash;
                dashIndex = (dashIndex + 1) % pattern.size();
                currPattern = int(pattern[dashIndex]);
            }
        }

        if (x2 > x1) { // 90 -> 135 and 270 -> 315 (unit circle degrees)
            x2 = qMin(x2, devRect.width() - 1);
            incrE = dx * 2;
            d = incrE - dy;
            incrNE = (dx - dy) * 2;

            if (x > x2)
                goto flush_and_return;

            while (y < y2) {
                if (d > 0) {
                    ++x;
                    d += incrNE;
                    if (x > x2)
                        goto flush_and_return;
                } else {
                    d += incrE;
                }
                ++y;
                const bool skip = x < 0 || y < 0;
                Q_ASSERT(skip || x < devRect.width() && y < devRect.height());
                if (inDash && !skip) {
                    if (current == NSPANS) {
                        span_func(NSPANS, spans, data);
                        current = 0;
                    }
                    spans[current].len = 1;
                    spans[current].coverage = 255;
                    spans[current].x = x;
                    spans[current].y = y;
                    ++current;
                }
                if (--currPattern <= 0) {
                    inDash = !inDash;
                    dashIndex = (dashIndex + 1) % pattern.size();
                    currPattern = int(pattern[dashIndex]);
                }
            }
        } else { // 45 -> 90 and 225 -> 270 (unit circle degrees)
            x1 = qMin(x1, devRect.width() - 1);
            incrE = dx * 2;
            d = incrE + dy;
            incrNE = (dx + dy) * 2;

            if (x < 0)
                goto flush_and_return;

            while (y < y2) {
                if (d < 0) {
                    --x;
                    d += incrNE;
                    if (x < 0)
                        goto flush_and_return;
                } else {
                    d += incrE;
                }
                ++y;
                const bool skip = y < 0 || x > x1;
                Q_ASSERT(skip || x >= 0 && x < devRect.width() && y < devRect.height());
                if (inDash && !skip) {
                    if (current == NSPANS) {
                        span_func(NSPANS, spans, data);
                        current = 0;
                    }
                    spans[current].len = 1;
                    spans[current].coverage = 255;
                    spans[current].x = x;
                    spans[current].y = y;
                    ++current;
                }
                if (--currPattern <= 0) {
                    inDash = !inDash;
                    dashIndex = (dashIndex + 1) % pattern.size();
                    currPattern = int(pattern[dashIndex]);
                }
            }
        }
    }
flush_and_return:
    if (current > 0)
        span_func(current, ordered ? spans : spans + (NSPANS - current), data);

    // adjust offset
    if (reversed) {
        *patternOffset = (patternLength - 1 - *patternOffset);
    } else {
        *patternOffset = 0;
        for (int i = 0; i <= dashIndex; ++i)
            *patternOffset += int(pattern[i]);
        *patternOffset += patternLength - currPattern - 1;
        *patternOffset = (*patternOffset % patternLength);
    }
}

/*!
    \internal
    \a x and \a y is relative to the midpoint of \a rect.
*/
static inline void drawEllipsePoints(int x, int y, int length,
                                     const QRect &rect,
                                     const QRect &clip,
                                     ProcessSpans pen_func, ProcessSpans brush_func,
                                     QSpanData *pen_data, QSpanData *brush_data)
{
    if (length == 0)
        return;

    QT_FT_Span outline[4];
    const int midx = rect.x() + (rect.width() + 1) / 2;
    const int midy = rect.y() + (rect.height() + 1) / 2;

    x = x + midx;
    y = midy - y;

    // topleft
    outline[0].x = midx + (midx - x) - (length - 1) - (rect.width() & 0x1);
    outline[0].len = qMin(length, x - outline[0].x);
    outline[0].y = y;
    outline[0].coverage = 255;

    // topright
    outline[1].x = x;
    outline[1].len = length;
    outline[1].y = y;
    outline[1].coverage = 255;

    // bottomleft
    outline[2].x = outline[0].x;
    outline[2].len = outline[0].len;
    outline[2].y = midy + (midy - y) - (rect.height() & 0x1);
    outline[2].coverage = 255;

    // bottomright
    outline[3].x = x;
    outline[3].len = length;
    outline[3].y = outline[2].y;
    outline[3].coverage = 255;

    if (brush_func && outline[0].x + outline[0].len < outline[1].x) {
        QT_FT_Span fill[2];

        // top fill
        fill[0].x = outline[0].x + outline[0].len - 1;
        fill[0].len = qMax(0, outline[1].x - fill[0].x);
        fill[0].y = outline[1].y;
        fill[0].coverage = 255;

        // bottom fill
        fill[1].x = outline[2].x + outline[2].len - 1;
        fill[1].len = qMax(0, outline[3].x - fill[1].x);
        fill[1].y = outline[3].y;
        fill[1].coverage = 255;

        int n = (fill[0].y >= fill[1].y ? 1 : 2);
        n = qt_intersect_spans(fill, n, clip);
        if (n > 0)
            brush_func(n, fill, brush_data);
    }
    if (pen_func) {
        int n = (outline[1].y >= outline[2].y ? 2 : 4);
        n = qt_intersect_spans(outline, n, clip);
        if (n > 0)
            pen_func(n, outline, pen_data);
    }
}

/*!
    \internal
    Draws an ellipse using the integer point midpoint algorithm.
*/
static void drawEllipse_midpoint_i(const QRect &rect, const QRect &clip,
                                   ProcessSpans pen_func, ProcessSpans brush_func,
                                   QSpanData *pen_data, QSpanData *brush_data)
{
#ifdef FLOATING_POINT_BUGGY_OR_NO_FPU // no fpu, so use fixed point
    const QFixed a = QFixed(rect.width()) >> 1;
    const QFixed b = QFixed(rect.height()) >> 1;
    QFixed d = b*b - (a*a*b) + ((a*a) >> 2);
#else
    const qreal a = qreal(rect.width()) / 2;
    const qreal b = qreal(rect.height()) / 2;
    qreal d = b*b - (a*a*b) + 0.25*a*a;
#endif

    int x = 0;
    int y = (rect.height() + 1) / 2;
    int startx = x;

    // region 1
    while (a*a*(2*y - 1) > 2*b*b*(x + 1)) {
        if (d < 0) { // select E
            d += b*b*(2*x + 3);
            ++x;
        } else {     // select SE
            d += b*b*(2*x + 3) + a*a*(-2*y + 2);
            drawEllipsePoints(startx, y, x - startx + 1, rect, clip,
                              pen_func, brush_func, pen_data, brush_data);
            startx = ++x;
            --y;
        }
    }
    drawEllipsePoints(startx, y, x - startx + 1, rect, clip,
                      pen_func, brush_func, pen_data, brush_data);

    // region 2
#ifdef FLOATING_POINT_BUGGY_OR_NO_FPU
    d = b*b*(x + (QFixed(1) >> 1))*(x + (QFixed(1) >> 1))
        + a*a*((y - 1)*(y - 1) - b*b);
#else
    d = b*b*(x + 0.5)*(x + 0.5) + a*a*((y - 1)*(y - 1) - b*b);
#endif
    const int miny = rect.height() & 0x1;
    while (y > miny) {
        if (d < 0) { // select SE
            d += b*b*(2*x + 2) + a*a*(-2*y + 3);
            ++x;
        } else {     // select S
            d += a*a*(-2*y + 3);
        }
        --y;
        drawEllipsePoints(x, y, 1, rect, clip,
                          pen_func, brush_func, pen_data, brush_data);
    }
}

/*!
    \fn void QRasterPaintEngine::drawPoints(const QPoint *points, int pointCount)
    \overload

    Draws the first \a pointCount points in the buffer \a points

    The default implementation converts the first \a pointCount QPoints in \a points
    to QPointFs and calls the floating point version of drawPoints.
*/

/*!
    \fn void QRasterPaintEngine::drawEllipse(const QRect &rect)
    \overload

    Reimplement this function to draw the largest ellipse that can be
    contained within rectangle \a rect.
*/

QT_END_NAMESPACE
