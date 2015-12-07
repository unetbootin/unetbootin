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
// QtCore
#include <qdebug.h>
#include <qmath.h>
#include <qmutex.h>

// QtGui
#include "qbitmap.h"
#include "qimage.h"
#include "qpaintdevice.h"
#include "qpaintengine.h"
#include "qpainter.h"
#include "qpainter_p.h"
#include "qpainterpath.h"
#include "qpicture.h"
#include "qpixmapcache.h"
#include "qpolygon.h"
#include "qtextlayout.h"
#include "qwidget.h"
#include "qapplication.h"
#include "qstyle.h"
#include "qthread.h"
#include "qvarlengtharray.h"

#include <private/qfontengine_p.h>
#include <private/qpaintengine_p.h>
#include <private/qpainterpath_p.h>
#include <private/qtextengine_p.h>
#include <private/qwidget_p.h>
#include <private/qpaintengine_raster_p.h>
#include <private/qmath_p.h>

QT_BEGIN_NAMESPACE

#define QGradient_StretchToDevice 0x10000000
#define QPaintEngine_OpaqueBackground 0x40000000

// #define QT_DEBUG_DRAW
#ifdef QT_DEBUG_DRAW
bool qt_show_painter_debug_output = true;
#endif

extern QPixmap qt_pixmapForBrush(int style, bool invert);

void qt_format_text(const QFont &font,
                    const QRectF &_r, int tf, const QTextOption *option, const QString& str, QRectF *brect,
                    int tabstops, int* tabarray, int tabarraylen,
                    QPainter *painter);

static inline QGradient::CoordinateMode coordinateMode(const QBrush &brush)
{
    switch (brush.style()) {
    case Qt::LinearGradientPattern:
    case Qt::RadialGradientPattern:
    case Qt::ConicalGradientPattern:
        return brush.gradient()->coordinateMode();
    default:
        ;
    }
    return QGradient::LogicalMode;
}

/* Returns true if the gradient requires stretch to device...*/
static inline bool check_gradient(const QBrush &brush)
{
    return coordinateMode(brush) == QGradient::StretchToDeviceMode;
}

extern bool qHasPixmapTexture(const QBrush &);

static inline bool is_brush_transparent(const QBrush &brush) {
    Qt::BrushStyle s = brush.style();
    bool brushBitmap = qHasPixmapTexture(brush)
                       ? brush.texture().isQBitmap()
                       : (brush.textureImage().depth() == 1);
    return ((s >= Qt::Dense1Pattern && s <= Qt::DiagCrossPattern)
            || (s == Qt::TexturePattern && brushBitmap));
}

static inline bool is_pen_transparent(const QPen &pen) {
    return pen.style() > Qt::SolidLine || is_brush_transparent(pen.brush());
}

/* Discards the emulation flags that are not relevant for line drawing
   and returns the result
*/
static inline uint line_emulation(uint emulation)
{
    return emulation & (QPaintEngine::PrimitiveTransform
                        | QPaintEngine::AlphaBlend
                        | QPaintEngine::Antialiasing
                        | QPaintEngine::BrushStroke
                        | QPaintEngine::ConstantOpacity
                        | QGradient_StretchToDevice
                        | QPaintEngine::ObjectBoundingModeGradients
                        | QPaintEngine_OpaqueBackground);
}

static bool qt_painter_thread_test(int devType, const char *what, bool extraCondition = false)
{
    switch (devType) {
    case QInternal::Image:
    case QInternal::Printer:
    case QInternal::Picture:
        // can be drawn onto these devices safely from any thread
        if (extraCondition)
            break;
    default:
        if (!extraCondition && QThread::currentThread() != qApp->thread()) {
            qWarning("QPainter: It is not safe to use %s outside the GUI thread", what);
            return false;
        }
        break;
    }
    return true;
}

QTransform QPainterPrivate::viewTransform() const
{
    QTransform m;
    if (state->VxF) {
        qreal scaleW = qreal(state->vw)/qreal(state->ww);
        qreal scaleH = qreal(state->vh)/qreal(state->wh);
        m.setMatrix(scaleW, 0, 0,
                    0, scaleH, 0,
                    state->vx - state->wx*scaleW, state->vy - state->wy*scaleH, 1);
    }
    return m;
}


/*
   \internal
   Returns true if using a shared painter; otherwise false.
*/
bool QPainterPrivate::attachPainterPrivate(QPainter *q, QPaintDevice *pdev)
{
    Q_ASSERT(q);
    Q_ASSERT(pdev);

    if (pdev->devType() != QInternal::Widget)
        return false;

    QWidget *widget = static_cast<QWidget *>(pdev);
    Q_ASSERT(widget);

    // Someone either called QPainter::setRedirected in the widget's paint event
    // right before this painter was created (or begin was called) or
    // sent a paint event directly to the widget.
    if (!widget->d_func()->redirectDev)
        return false;

    QWidget *window = widget->isWindow() ? widget : widget->window();
    Q_ASSERT(window);

    QTLWExtra *extra = window->d_func()->maybeTopData();
    QPainter *sp = extra ? extra->sharedPainter : 0;
    if (!sp || !sp->isActive())
        return false;

    if ((sp->d_ptr->refcount == 1) || (extra->inRenderWithPainter && sp->d_ptr->refcount == 2)) {

        if (sp->paintEngine()->paintDevice() != widget->d_func()->redirectDev)
            return false;

        // Check if we're attempting to paint outside a paint event.
        if (!sp->d_ptr->engine->hasFeature(QPaintEngine::PaintOutsidePaintEvent)
            && !widget->testAttribute(Qt::WA_PaintOutsidePaintEvent)
            && !widget->testAttribute(Qt::WA_WState_InPaintEvent)) {

            qWarning("QPainter::begin: Widget painting can only begin as a result of a paintEvent");
            return false;
        }

        // Save the current state of the shared painter and assign
        // the current d_ptr to the shared painter's d_ptr.
        sp->save();
        if (!sp->d_ptr->d_ptrs)
            sp->d_ptr->d_ptrs = new QPainterPrivate*[2];
        sp->d_ptr->d_ptrs[++sp->d_ptr->refcount - 2] = q->d_ptr;
        q->d_ptr = sp->d_ptr;

        // Now initialize the painter with correct widget properties.
        q->d_ptr->initSharedPainter(widget, extra);
        return true;
    }

    qWarning("QPainter::begin: A paint device can only be painted by one painter at a time.");
    return false;
}

void QPainterPrivate::detachPainterPrivate(QPainter *q)
{
    Q_ASSERT(refcount > 1);
    Q_ASSERT(refcount <= 3);
    Q_ASSERT(q);

    QPainterPrivate *original = d_ptrs[--refcount - 1];
    if (inDestructor) {
        inDestructor = false;
        if (original)
            original->inDestructor = true;
    } else if (!original) {
        original = new QPainterPrivate(q);
    }

    d_ptrs[refcount - 1] = 0;
    q->restore();
    q->d_ptr = original;
}


void QPainterPrivate::draw_helper(const QPainterPath &originalPath, DrawOperation op)
{
#ifdef QT_DEBUG_DRAW
    if (qt_show_painter_debug_output) {
        printf("QPainter::drawHelper\n");
    }
#endif

    if (originalPath.isEmpty())
        return;

    QPaintEngine::PaintEngineFeatures gradientStretch =
        QPaintEngine::PaintEngineFeatures(QGradient_StretchToDevice
                                          | QPaintEngine::ObjectBoundingModeGradients);

    const bool mustEmulateObjectBoundingModeGradients =
        (state->emulationSpecifier & QPaintEngine::ObjectBoundingModeGradients)
         && !engine->hasFeature(QPaintEngine::PatternTransform);

    if (!(state->emulationSpecifier & ~gradientStretch)
        && !mustEmulateObjectBoundingModeGradients) {
        drawStretchedGradient(originalPath, op);
        return;
    } else if (state->emulationSpecifier & QPaintEngine_OpaqueBackground) {
        drawOpaqueBackground(originalPath, op);
        return;
    }

    Q_Q(QPainter);
    int devMinX = 0, devMaxX = 0, devMinY = 0, devMaxY = 0;

    qreal strokeOffsetX = 0, strokeOffsetY = 0;

    QPainterPath path = originalPath * state->matrix;
    QRectF pathBounds = path.boundingRect();
    QRectF strokeBounds;
    bool doStroke = (op & StrokeDraw) && (state->pen.style() != Qt::NoPen);
    if (doStroke) {
        qreal penWidth = state->pen.widthF();
        if (penWidth == 0) {
            strokeOffsetX = 1;
            strokeOffsetY = 1;
        } else {
            // In case of complex xform
            if (state->txop > QTransform::TxScale) {
                QPainterPathStroker stroker;
                stroker.setWidth(penWidth);
                stroker.setJoinStyle(state->pen.joinStyle());
                stroker.setCapStyle(state->pen.capStyle());
                QPainterPath stroke = stroker.createStroke(originalPath);
                strokeBounds = (stroke * state->matrix).boundingRect();
            } else {
                strokeOffsetX = qAbs(penWidth * state->matrix.m11() / 2.0);
                strokeOffsetY = qAbs(penWidth * state->matrix.m22() / 2.0);
            }
        }
    }

    const qreal ROUND_UP_TRICK = qreal(0.9999);
    if (!strokeBounds.isEmpty()) {
        devMinX = int(strokeBounds.left());
        devMaxX = int(strokeBounds.right() + ROUND_UP_TRICK);
        devMinY = int(strokeBounds.top());
        devMaxY = int(strokeBounds.bottom() + ROUND_UP_TRICK);
    } else {
        devMinX = int(pathBounds.left() - strokeOffsetX);
        devMaxX = int(pathBounds.right() + strokeOffsetX + ROUND_UP_TRICK);
        devMinY = int(pathBounds.top() - strokeOffsetY);
        devMaxY = int(pathBounds.bottom() + strokeOffsetY + ROUND_UP_TRICK);
    }

    QRect absPathRect(devMinX, devMinY, devMaxX - devMinX, devMaxY - devMinY);

    if (state->clipInfo.size() != 0) {
        QPainterPath clipPath = q->clipPath() * q->deviceTransform();
        QRectF r = clipPath.boundingRect().intersected(absPathRect);
        absPathRect = r.toAlignedRect();
    }
    absPathRect = absPathRect.intersected(QRect(0, 0, device->width(), device->height()));


//     qDebug("\nQPainterPrivate::draw_helper(), x=%d, y=%d, w=%d, h=%d",
//            devMinX, devMinY, device->width(), device->height());
//     qDebug() << " - matrix" << state->matrix;
//     qDebug() << " - originalPath.bounds" << originalPath.boundingRect();
//     qDebug() << " - path.bounds" << path.boundingRect();

    if (absPathRect.width() <= 0 || absPathRect.height() <= 0)
        return;

    QImage image(absPathRect.width(), absPathRect.height(), QImage::Format_ARGB32_Premultiplied);
    image.fill(0);

    QPainter p(&image);

    p.d_ptr->helper_device = helper_device;

    p.setOpacity(state->opacity);
    p.translate(-absPathRect.x(), -absPathRect.y());
    p.setTransform(state->matrix, true);
    p.setPen(doStroke ? state->pen : QPen(Qt::NoPen));
    p.setBrush((op & FillDraw) ? state->brush : QBrush(Qt::NoBrush));
    p.setBackground(state->bgBrush);
    p.setBackgroundMode(state->bgMode);
    p.setBrushOrigin(state->bgOrigin);

    p.setRenderHint(QPainter::Antialiasing, state->renderHints & QPainter::Antialiasing);
    p.setRenderHint(QPainter::SmoothPixmapTransform,
                    state->renderHints & QPainter::SmoothPixmapTransform);

    p.drawPath(originalPath);

    p.end();

    q->save();
    q->resetMatrix();
    updateState(state);
    engine->drawImage(absPathRect,
                 image,
                 QRectF(0, 0, absPathRect.width(), absPathRect.height()),
                 Qt::OrderedDither | Qt::OrderedAlphaDither);
    q->restore();
}

void QPainterPrivate::drawOpaqueBackground(const QPainterPath &path, DrawOperation op)
{
    Q_Q(QPainter);

    q->setBackgroundMode(Qt::TransparentMode);

    if (op & FillDraw && state->brush.style() != Qt::NoBrush) {
        q->fillPath(path, state->bgBrush.color());
        q->fillPath(path, state->brush);
    }

    if (op & StrokeDraw && state->pen.style() != Qt::NoPen) {
        q->strokePath(path, QPen(state->bgBrush.color(), state->pen.width()));
        q->strokePath(path, state->pen);
    }

    q->setBackgroundMode(Qt::OpaqueMode);
}

static inline QBrush stretchGradientToUserSpace(const QBrush &brush, const QRectF &boundingRect)
{
    Q_ASSERT(brush.style() >= Qt::LinearGradientPattern
             && brush.style() <= Qt::ConicalGradientPattern);

    QTransform gradientToUser(boundingRect.width(), 0, 0, boundingRect.height(),
                              boundingRect.x(), boundingRect.y());

    QGradient g = *brush.gradient();
    g.setCoordinateMode(QGradient::LogicalMode);

    QBrush b(g);
    b.setTransform(gradientToUser * b.transform());
    return b;
}

void QPainterPrivate::drawStretchedGradient(const QPainterPath &path, DrawOperation op)
{
    Q_Q(QPainter);

    const qreal sw = helper_device->width();
    const qreal sh = helper_device->height();

    bool changedPen = false;
    bool changedBrush = false;
    bool needsFill = false;

    const QPen pen = state->pen;
    const QBrush brush = state->brush;

    const QGradient::CoordinateMode penMode = coordinateMode(pen.brush());
    const QGradient::CoordinateMode brushMode = coordinateMode(brush);

    QRectF boundingRect;

    // Draw the xformed fill if the brush is a stretch gradient.
    if ((op & FillDraw) && brush.style() != Qt::NoBrush) {
        if (brushMode == QGradient::StretchToDeviceMode) {
            q->setPen(Qt::NoPen);
            changedPen = pen.style() != Qt::NoPen;
            q->scale(sw, sh);
            updateState(state);

            const qreal isw = 1.0 / sw;
            const qreal ish = 1.0 / sh;
            QTransform inv(isw, 0, 0, ish, 0, 0);
            engine->drawPath(path * inv);
            q->scale(isw, ish);
        } else {
            needsFill = true;

            if (brushMode == QGradient::ObjectBoundingMode) {
                Q_ASSERT(engine->hasFeature(QPaintEngine::PatternTransform));
                boundingRect = path.boundingRect();
                q->setBrush(stretchGradientToUserSpace(brush, boundingRect));
                changedBrush = true;
            }
        }
    }

    if ((op & StrokeDraw) && pen.style() != Qt::NoPen) {
        // Draw the xformed outline if the pen is a stretch gradient.
        if (penMode == QGradient::StretchToDeviceMode) {
            q->setPen(Qt::NoPen);
            changedPen = true;

            if (needsFill) {
                updateState(state);
                engine->drawPath(path);
            }

            q->scale(sw, sh);
            q->setBrush(pen.brush());
            changedBrush = true;
            updateState(state);

            QPainterPathStroker stroker;
            stroker.setDashPattern(pen.style());
            stroker.setWidth(pen.widthF());
            stroker.setJoinStyle(pen.joinStyle());
            stroker.setCapStyle(pen.capStyle());
            stroker.setMiterLimit(pen.miterLimit());
            QPainterPath stroke = stroker.createStroke(path);

            const qreal isw = 1.0 / sw;
            const qreal ish = 1.0 / sh;
            QTransform inv(isw, 0, 0, ish, 0, 0);
            engine->drawPath(stroke * inv);
            q->scale(isw, ish);
        } else {
            if (!needsFill && brush.style() != Qt::NoBrush) {
                q->setBrush(Qt::NoBrush);
                changedBrush = true;
            }

            if (penMode == QGradient::ObjectBoundingMode) {
                Q_ASSERT(engine->hasFeature(QPaintEngine::PatternTransform));

                // avoid computing the bounding rect twice
                if (!needsFill || brushMode != QGradient::ObjectBoundingMode)
                    boundingRect = path.boundingRect();

                QPen p = pen;
                p.setBrush(stretchGradientToUserSpace(pen.brush(), boundingRect));
                q->setPen(p);
                changedPen = true;
            } else if (changedPen) {
                q->setPen(pen);
                changedPen = false;
            }

            updateState(state);
            engine->drawPath(path);
        }
    } else if (needsFill) {
        if (pen.style() != Qt::NoPen) {
            q->setPen(Qt::NoPen);
            changedPen = true;
        }

        updateState(state);
        engine->drawPath(path);
    }

    if (changedPen)
        q->setPen(pen);
    if (changedBrush)
        q->setBrush(brush);
}

void QPainterPrivate::init()
{
    Q_Q(QPainter);
    state->painter = q;
}

void QPainterPrivate::updateMatrix()
{
    state->matrix = (state->WxF ? state->worldMatrix : QTransform())
                    * (state->VxF ? viewTransform() : QTransform());

    txinv = false;                                // no inverted matrix
    state->txop = static_cast<int>(state->matrix.type());
    if (!state->redirection_offset.isNull()) {
        state->txop |= QTransform::TxTranslate;
        // We want to translate in dev space so we do the adding of the redirection
        // offset manually.
        if (state->matrix.isAffine()) {
            state->matrix = QTransform(state->matrix.m11(), state->matrix.m12(),
                                       state->matrix.m21(), state->matrix.m22(),
                                       state->matrix.dx()-state->redirection_offset.x(),
                                       state->matrix.dy()-state->redirection_offset.y());
        } else {
            QTransform temp;
            temp.translate(-state->redirection_offset.x(), -state->redirection_offset.y());
            state->matrix *= temp;
        }
    }
    state->dirtyFlags |= QPaintEngine::DirtyTransform;

//     printf("VxF=%d, WxF=%d\n", state->VxF, state->WxF);
//     qDebug() << " --- using matrix" << state->matrix << redirection_offset;
}

/*! \internal */
void QPainterPrivate::updateInvMatrix()
{
    Q_ASSERT(txinv == false);
    txinv = true;                                // creating inverted matrix
    bool invertible;
    QTransform m;
    if (state->VxF) {
        m.translate(state->vx, state->vy);
        m.scale(1.0*state->vw/state->ww, 1.0*state->vh/state->wh);
        m.translate(-state->wx, -state->wy);
    }
    if (state->WxF) {
        if (state->VxF)
            m = state->worldMatrix * m;
        else
            m = state->worldMatrix;
    }
    invMatrix = m.inverted(&invertible);                // invert matrix
}

void QPainterPrivate::updateCombinedOpacity()
{
    Q_Q(QPainter);
    QPaintDevice *currentDevice = q->device();
    if (currentDevice && currentDevice->devType() == QInternal::Widget) {
        QTLWExtra *extra = static_cast<QWidget *>(currentDevice)->window()->d_func()->maybeTopData();
        if (extra && extra->inRenderWithPainter && extra->globalOpacity < 1.0)
            state->opacity = state->localOpacity * extra->globalOpacity;
    }
}

void QPainterPrivate::initSharedPainter(QWidget *widget, QTLWExtra *extra)
{
    if (!widget || !extra)
        return;

    Q_Q(QPainter);
    Q_ASSERT(engine);

    q->initFrom(widget);
    QPoint offset;
    widget->d_func()->redirected(&offset);
    offset += engine->coordinateOffset();
    QPaintEnginePrivate *enginePrivate = engine->d_func();

    // System rect.
    state->ww = state->vw = widget->width();
    state->wh = state->vh = widget->height();
    updateCombinedOpacity();

    if (enginePrivate->currentClipWidget == widget) {
        if (extra->inRenderWithPainter && state->WxF)
            state->worldMatrix.translate(-offset.x(), -offset.y());
        else
            state->redirection_offset = offset;
        updateMatrix();
        return;
    }

    QPainterPath clipPath;
    const bool usingRasterEngine = engine->type() == QPaintEngine::Raster;

    if (extra->inRenderWithPainter && state->WxF) {
        // Translate world matrix.
        state->worldMatrix.translate(-offset.x(), -offset.y());
        updateMatrix();

        // Calculate new system clip.
        clipPath.addRegion(enginePrivate->systemClip);
        clipPath = state->matrix.map(clipPath);

        // Intersect with current viewport.
        const QPainterPath viewportClipPath = enginePrivate->oldSystemClipPath;
        if (!viewportClipPath.isEmpty()) {
            clipPath = clipPath.intersected(viewportClipPath);
            if (clipPath.isEmpty())
                clipPath = viewportClipPath;
        }

        enginePrivate->systemClip = clipPath.toFillPolygon().toPolygon();
    } else {
        if (usingRasterEngine)
            clipPath.addRegion(enginePrivate->systemClip);
        state->redirection_offset = offset;
        updateMatrix();
    }

    if (usingRasterEngine)
        static_cast<QRasterPaintEnginePrivate *>(enginePrivate)->baseClip = clipPath;
    static_cast<QPainterState*>(state)->clipEnabled = false;
    state->dirtyFlags |= QPaintEngine::DirtyClipEnabled | QPaintEngine::DirtyClipRegion;

    updateState(state);
    enginePrivate->currentClipWidget = widget;
}

void QPainterPrivate::updateEmulationSpecifier(QPainterState *s)
{
    bool alpha = false;
    bool linearGradient = false;
    bool radialGradient = false;
    bool conicalGradient = false;
    bool patternBrush = false;
    bool xform = false;
    bool complexXform = false;

    bool skip = true;

    // Pen and brush properties (we have to check both if one changes because the
    // one that's unchanged can still be in a state which requires emulation)
    if (s->state() & QPaintEngine::DirtyPen ||
        s->state() & QPaintEngine::DirtyBrush) {
        // Check Brush stroke emulation
        if (!s->pen.isSolid() && !engine->hasFeature(QPaintEngine::BrushStroke))
            s->emulationSpecifier |= QPaintEngine::BrushStroke;
        else
            s->emulationSpecifier &= ~QPaintEngine::BrushStroke;

        skip = false;

        QBrush penBrush = s->pen.brush();
        Qt::BrushStyle brushStyle = s->brush.style();
        Qt::BrushStyle penBrushStyle = penBrush.style();
        alpha = (penBrushStyle != Qt::NoBrush
                 && (penBrushStyle < Qt::LinearGradientPattern && penBrush.color().alpha() != 255)
                 && !penBrush.isOpaque())
                || (brushStyle != Qt::NoBrush
                    && (brushStyle < Qt::LinearGradientPattern && s->brush.color().alpha() != 255)
                    && !s->brush.isOpaque());
        linearGradient = ((penBrushStyle == Qt::LinearGradientPattern) ||
                           (brushStyle == Qt::LinearGradientPattern));
        radialGradient = ((penBrushStyle == Qt::RadialGradientPattern) ||
                           (brushStyle == Qt::RadialGradientPattern));
        conicalGradient = ((penBrushStyle == Qt::ConicalGradientPattern) ||
                            (brushStyle == Qt::ConicalGradientPattern));
        patternBrush = (((penBrushStyle > Qt::SolidPattern
                           && penBrushStyle < Qt::LinearGradientPattern)
                          || penBrushStyle == Qt::TexturePattern) ||
                         ((brushStyle > Qt::SolidPattern
                           && brushStyle < Qt::LinearGradientPattern)
                          || brushStyle == Qt::TexturePattern));

        bool penTextureAlpha = qHasPixmapTexture(penBrush)
                               ? penBrush.texture().hasAlpha()
                               : penBrush.textureImage().hasAlphaChannel();
        bool brushTextureAlpha = qHasPixmapTexture(s->brush)
                                 ? s->brush.texture().hasAlpha()
                                 : s->brush.textureImage().hasAlphaChannel();
        if (((penBrush.style() == Qt::TexturePattern && penTextureAlpha)
             || (s->brush.style() == Qt::TexturePattern && brushTextureAlpha))
            && !engine->hasFeature(QPaintEngine::MaskedBrush))
            s->emulationSpecifier |= QPaintEngine::MaskedBrush;
        else
            s->emulationSpecifier &= ~QPaintEngine::MaskedBrush;
    }

    if (s->state() & (QPaintEngine::DirtyHints
                      | QPaintEngine::DirtyOpacity
                      | QPaintEngine::DirtyBackgroundMode)) {
        skip = false;
    }

    if (skip)
        return;

#if 0
    qDebug("QPainterPrivate::updateEmulationSpecifier, state=%p\n"
           " - alpha: %d\n"
           " - linearGradient: %d\n"
           " - radialGradient: %d\n"
           " - conicalGradient: %d\n"
           " - patternBrush: %d\n"
           " - hints: %x\n"
           " - xform: %d\n",
           s,
           alpha,
           linearGradient,
           radialGradient,
           conicalGradient,
           patternBrush,
           uint(s->renderHints),
           xform);
#endif

    // XForm properties
    if (s->state() & QPaintEngine::DirtyTransform) {
        xform = !s->matrix.isIdentity();
        complexXform = !s->matrix.isAffine();
    } else if (s->txop >= QTransform::TxTranslate) {
        xform = true;
        complexXform = !s->matrix.isAffine();
    }

    const bool brushXform = (!s->brush.transform().type() == QTransform::TxNone);
    const bool penXform = (!s->pen.brush().transform().type() == QTransform::TxNone);

    const bool patternXform = patternBrush && (xform || brushXform || penXform);

    // Check alphablending
    if (alpha && !engine->hasFeature(QPaintEngine::AlphaBlend))
        s->emulationSpecifier |= QPaintEngine::AlphaBlend;
    else
        s->emulationSpecifier &= ~QPaintEngine::AlphaBlend;

    // Linear gradient emulation
    if (linearGradient && !engine->hasFeature(QPaintEngine::LinearGradientFill))
        s->emulationSpecifier |= QPaintEngine::LinearGradientFill;
    else
        s->emulationSpecifier &= ~QPaintEngine::LinearGradientFill;

    // Radial gradient emulation
    if (radialGradient && !engine->hasFeature(QPaintEngine::RadialGradientFill))
        s->emulationSpecifier |= QPaintEngine::RadialGradientFill;
    else
        s->emulationSpecifier &= ~QPaintEngine::RadialGradientFill;

    // Conical gradient emulation
    if (conicalGradient && !engine->hasFeature(QPaintEngine::ConicalGradientFill))
        s->emulationSpecifier |= QPaintEngine::ConicalGradientFill;
    else
        s->emulationSpecifier &= ~QPaintEngine::ConicalGradientFill;

    // Pattern brushes
    if (patternBrush && !engine->hasFeature(QPaintEngine::PatternBrush))
        s->emulationSpecifier |= QPaintEngine::PatternBrush;
    else
        s->emulationSpecifier &= ~QPaintEngine::PatternBrush;

    // Pattern XForms
    if (patternXform && !engine->hasFeature(QPaintEngine::PatternTransform))
        s->emulationSpecifier |= QPaintEngine::PatternTransform;
    else
        s->emulationSpecifier &= ~QPaintEngine::PatternTransform;

    // Primitive XForms
    if (xform && !engine->hasFeature(QPaintEngine::PrimitiveTransform))
        s->emulationSpecifier |= QPaintEngine::PrimitiveTransform;
    else
        s->emulationSpecifier &= ~QPaintEngine::PrimitiveTransform;

    // Perspective XForms
    if (complexXform && !engine->hasFeature(QPaintEngine::PerspectiveTransform))
        s->emulationSpecifier |= QPaintEngine::PerspectiveTransform;
    else
        s->emulationSpecifier &= ~QPaintEngine::PerspectiveTransform;

    // Constant opacity
    if (state->opacity != 1 && !engine->hasFeature(QPaintEngine::ConstantOpacity))
        s->emulationSpecifier |= QPaintEngine::ConstantOpacity;
    else
        s->emulationSpecifier &= ~QPaintEngine::ConstantOpacity;

    bool gradientStretch = false;
    bool objectBoundingMode = false;
    if (linearGradient || conicalGradient || radialGradient) {
        QGradient::CoordinateMode brushMode = coordinateMode(s->brush);
        QGradient::CoordinateMode penMode = coordinateMode(s->pen.brush());

        gradientStretch |= (brushMode == QGradient::StretchToDeviceMode);
        gradientStretch |= (penMode == QGradient::StretchToDeviceMode);

        objectBoundingMode |= (brushMode == QGradient::ObjectBoundingMode);
        objectBoundingMode |= (penMode == QGradient::ObjectBoundingMode);
    }
    if (gradientStretch)
        s->emulationSpecifier |= QGradient_StretchToDevice;
    else
        s->emulationSpecifier &= ~QGradient_StretchToDevice;

    if (objectBoundingMode && !engine->hasFeature(QPaintEngine::ObjectBoundingModeGradients))
        s->emulationSpecifier |= QPaintEngine::ObjectBoundingModeGradients;
    else
        s->emulationSpecifier &= ~QPaintEngine::ObjectBoundingModeGradients;

    // Opaque backgrounds...
    if (s->bgMode == Qt::OpaqueMode &&
        (is_pen_transparent(s->pen) || is_brush_transparent(s->brush)))
        s->emulationSpecifier |= QPaintEngine_OpaqueBackground;
    else
        s->emulationSpecifier &= ~QPaintEngine_OpaqueBackground;

#if 0
    //won't be correct either way because the device can already have
    // something rendered to it in which case subsequent emulation
    // on a fully transparent qimage and then blitting the results
    // won't produce correct results
    // Blend modes
    if (state->composition_mode > QPainter::CompositionMode_Xor &&
        !engine->hasFeature(QPaintEngine::BlendModes))
        s->emulationSpecifier |= QPaintEngine::BlendModes;
    else
        s->emulationSpecifier &= ~QPaintEngine::BlendModes;
#endif
}

void QPainterPrivate::updateStateImpl(QPainterState *newState)
{
    // ### we might have to call QPainter::begin() here...
    if (!engine->state) {
        engine->state = newState;
        engine->setDirty(QPaintEngine::AllDirty);
    }

    if (engine->state->painter() != newState->painter)
        // ### this could break with clip regions vs paths.
        engine->setDirty(QPaintEngine::AllDirty);

    // Upon restore, revert all changes since last save
    else if (engine->state != newState)
        newState->dirtyFlags |= QPaintEngine::DirtyFlags(static_cast<QPainterState *>(engine->state)->changeFlags);

    // We need to store all changes made so that restore can deal with them
    else
        newState->changeFlags |= newState->dirtyFlags;

    updateEmulationSpecifier(newState);

    // Unset potential dirty background mode
    newState->dirtyFlags &= ~(QPaintEngine::DirtyBackgroundMode
            | QPaintEngine::DirtyBackground);

    engine->state = newState;
    engine->updateState(*newState);
    engine->clearDirty(QPaintEngine::AllDirty);

}

void QPainterPrivate::updateState(QPainterState *newState)
{

    if (!newState) {
        engine->state = newState;

    } else if (newState->state() || engine->state!=newState) {
        bool setNonCosmeticPen = (newState->renderHints & QPainter::NonCosmeticDefaultPen)
                                 && newState->pen.widthF() == 0;
        if (setNonCosmeticPen) {
            // Override the default pen's cosmetic state if the
            // NonCosmeticDefaultPen render hint is used.
            QPen oldPen = newState->pen;
            newState->pen.setWidth(1);
            newState->pen.setCosmetic(false);
            newState->dirtyFlags |= QPaintEngine::DirtyPen;

            updateStateImpl(newState);

            // Restore the state pen back to its default to preserve visible
            // state.
            newState->pen = oldPen;
        } else {
            updateStateImpl(newState);
        }
    }
}


/*!
    \class QPainter
    \brief The QPainter class performs low-level painting on widgets and
    other paint devices.

    \ingroup multimedia
    \mainclass
    \reentrant

    QPainter provides highly optimized functions to do most of the
    drawing GUI programs require. It can draw everything from simple
    lines to complex shapes like pies and chords. It can also draw
    aligned text and pixmaps. Normally, it draws in a "natural"
    coordinate system, but it can also do view and world
    transformation. QPainter can operate on any object that inherits
    the QPaintDevice class.

    The common use of QPainter is inside a widget's paint event:
    Construct and customize (e.g. set the pen or the brush) the
    painter. Then draw. Remember to destroy the QPainter object after
    drawing. For example:

    \snippet doc/src/snippets/code/src.gui.painting.qpainter.cpp 0

    The core functionality of QPainter is drawing, but the class also
    provide several functions that allows you to customize QPainter's
    settings and its rendering quality, and others that enable
    clipping. In addition you can control how different shapes are
    merged together by specifying the painter's composition mode.

    The isActive() function indicates whether the painter is active. A
    painter is activated by the begin() function and the constructor
    that takes a QPaintDevice argument. The end() function, and the
    destructor, deactivates it.

    Together with the QPaintDevice and QPaintEngine classes, QPainter
    form the basis for Qt's paint system. QPainter is the class used
    to perform drawing operations. QPaintDevice represents a device
    that can be painted on using a QPainter. QPaintEngine provides the
    interface that the painter uses to draw onto different types of
    devices. If the painter is active, device() returns the paint
    device on which the painter paints, and paintEngine() returns the
    paint engine that the painter is currently operating on. For more
    information, see \l {The Paint System} documentation.

    Sometimes it is desirable to make someone else paint on an unusual
    QPaintDevice. QPainter supports a static function to do this,
    setRedirected().

    \warning When the paintdevice is a widget, QPainter can only be
    used inside a paintEvent() function or in a function called by
    paintEvent(); that is unless the Qt::WA_PaintOutsidePaintEvent
    widget attribute is set. On Mac OS X and Windows, you can only
    paint in a paintEvent() function regardless of this attribute's
    setting.

    \tableofcontents

    \section1 Settings

    There are several settings that you can customize to make QPainter
    draw according to your preferences:

    \list

    \o font() is the font used for drawing text. If the painter
        isActive(), you can retrieve information about the currently set
        font, and its metrics, using the fontInfo() and fontMetrics()
        functions respectively.

    \o brush() defines the color or pattern that is used for filling
       shapes.

    \o pen() defines the color or stipple that is used for drawing
       lines or boundaries.

    \o backgroundMode() defines whether there is a background() or
       not, i.e it is either Qt::OpaqueMode or Qt::TransparentMode.

    \o background() only applies when backgroundMode() is \l
       Qt::OpaqueMode and pen() is a stipple. In that case, it
       describes the color of the background pixels in the stipple.

    \o brushOrigin() defines the origin of the tiled brushes, normally
       the origin of widget's background.

    \o viewport(), window(), worldMatrix() make up the painter's coordinate
        transformation system. For more information, see the \l
        {Coordinate Transformations} section and the \l {The Coordinate
        System} documentation.

    \o hasClipping() tells whether the painter clips at all. (The paint
       device clips, too.) If the painter clips, it clips to clipRegion().

    \o layoutDirection() defines the layout direction used by the
       painter when drawing text.

    \o matrixEnabled() tells whether world transformation is enabled.

    \o viewTransformEnabled() tells whether view transformation is
        enabled.

    \endlist

    Note that some of these settings mirror settings in some paint
    devices, e.g.  QWidget::font(). The QPainter::begin() function (or
    equivalently the QPainter constructor) copies these attributes
    from the paint device.

    You can at any time save the QPainter's state by calling the
    save() function which saves all the available settings on an
    internal stack. The restore() function pops them back.

    \section1 Drawing

    QPainter provides functions to draw most primitives: drawPoint(),
    drawPoints(), drawLine(), drawRect(), drawRoundedRect(),
    drawEllipse(), drawArc(), drawPie(), drawChord(), drawPolyline(),
    drawPolygon(), drawConvexPolygon() and drawCubicBezier().  The two
    convenience functions, drawRects() and drawLines(), draw the given
    number of rectangles or lines in the given array of \l
    {QRect}{QRects} or \l {QLine}{QLines} using the current pen and
    brush.

    The QPainter class also provides the fillRect() function which
    fills the given QRect, with the given QBrush, and the eraseRect()
    function that erases the area inside the given rectangle.

    All of these functions have both integer and floating point
    versions.

    \table 100%
    \row
    \o \inlineimage qpainter-basicdrawing.png
    \o
    \bold {Basic Drawing Example}

    The \l {painting/basicdrawing}{Basic Drawing} example shows how to
    display basic graphics primitives in a variety of styles using the
    QPainter class.

    \endtable

    If you need to draw a complex shape, especially if you need to do
    so repeatedly, consider creating a QPainterPath and drawing it
    using drawPath().

    \table 100%
    \row
    \o
    \bold {Painter Paths example}

    The QPainterPath class provides a container for painting
    operations, enabling graphical shapes to be constructed and
    reused.

    The \l {painting/painterpaths}{Painter Paths} example shows how
    painter paths can be used to build complex shapes for rendering.

    \o \inlineimage qpainter-painterpaths.png
    \endtable

    QPainter also provides the fillPath() function which fills the
    given QPainterPath with the given QBrush, and the strokePath()
    function that draws the outline of the given path (i.e. strokes
    the path).

    See also the \l {demos/deform}{Vector Deformation} demo which
    shows how to use advanced vector techniques to draw text using a
    QPainterPath, the \l {demos/gradients}{Gradients} demo which shows
    the different types of gradients that are available in Qt, and the \l
    {demos/pathstroke}{Path Stroking} demo which shows Qt's built-in
    dash patterns and shows how custom patterns can be used to extend
    the range of available patterns.

    \table
    \row
    \o \inlineimage qpainter-vectordeformation.png
    \o \inlineimage qpainter-gradients.png
    \o \inlineimage qpainter-pathstroking.png
    \header
    \o \l {demos/deform}{Vector Deformation}
    \o \l {demos/gradients}{Gradients}
    \o \l {demos/pathstroke}{Path Stroking}
    \endtable


    There are functions to draw pixmaps/images, namely drawPixmap(),
    drawImage() and drawTiledPixmap(). Both drawPixmap() and drawImage()
    produce the same result, except that drawPixmap() is faster
    on-screen while drawImage() may be faster on a QPrinter or other
    devices.

    Text drawing is done using drawText(). When you need
    fine-grained positioning, boundingRect() tells you where a given
    drawText() command will draw.

    There is a drawPicture() function that draws the contents of an
    entire QPicture. The drawPicture() function is the only function
    that disregards all the painter's settings as QPicture has its own
    settings.

    \section1 Rendering Quality

    To get the optimal rendering result using QPainter, you should use
    the platform independent QImage as paint device; i.e. using QImage
    will ensure that the result has an identical pixel representation
    on any platform.

    The QPainter class also provides a means of controlling the
    rendering quality through its RenderHint enum and the support for
    floating point precision: All the functions for drawing primitives
    has a floating point version. These are often used in combination
    with the \l {RenderHint}{QPainter::Antialiasing} render hint.

    \table 100%
    \row
    \o \inlineimage qpainter-concentriccircles.png
    \o
    \bold {Concentric Circles Example}

    The \l {painting/concentriccircles}{Concentric Circles} example
    shows the improved rendering quality that can be obtained using
    floating point precision and anti-aliasing when drawing custom
    widgets.

    The application's main window displays several widgets which are
    drawn using the various combinations of precision and
    anti-aliasing.

    \endtable

    The RenderHint enum specifies flags to QPainter that may or may
    not be respected by any given engine.  \l
    {RenderHint}{QPainter::Antialiasing} indicates that the engine
    should antialias edges of primitives if possible, \l
    {RenderHint}{QPainter::TextAntialiasing} indicates that the engine
    should antialias text if possible, and the \l
    {RenderHint}{QPainter::SmoothPixmapTransform} indicates that the
    engine should use a smooth pixmap transformation algorithm.
    \l {RenderHint}{HighQualityAntialiasing} is an OpenGL-specific rendering hint
    indicating that the engine should use fragment programs and offscreen
    rendering for antialiasing.

    The renderHints() function returns a flag that specifies the
    rendering hints that are set for this painter.  Use the
    setRenderHint() function to set or clear the currently set
    RenderHints.

    \section1 Coordinate Transformations

    Normally, the QPainter operates on the device's own coordinate
    system (usually pixels), but QPainter has good support for
    coordinate transformations.

    \table
    \row
    \o \inlineimage qpainter-clock.png
    \o \inlineimage qpainter-rotation.png
    \o \inlineimage qpainter-scale.png
    \o \inlineimage qpainter-translation.png
    \header
    \o  nop \o rotate() \o scale() \o translate()
    \endtable

    The most commonly used transformations are scaling, rotation,
    translation and shearing. Use the scale() function to scale the
    coordinate system by a given offset, the rotate() function to
    rotate it clockwise and translate() to translate it (i.e. adding a
    given offset to the points). You can also twist the coordinate
    system around the origin using the shear() function. See the \l
    {demos/affine}{Affine Transformations} demo for a visualization of
    a sheared coordinate system.

    See also the \l {painting/transformations}{Transformations}
    example which shows how transformations influence the way that
    QPainter renders graphics primitives. In particular it shows how
    the order of transformations affects the result.

    \table 100%
    \row
    \o
    \bold {Affine Transformations Demo}

    The \l {demos/affine}{Affine Transformations} demo show Qt's
    ability to perform affine transformations on painting
    operations. The demo also allows the user to experiment with the
    transformation operations and see the results immediately.

    \o \inlineimage qpainter-affinetransformations.png
    \endtable

    All the tranformation operations operate on the transformation
    worldMatrix(). A matrix transforms a point in the plane to another
    point. For more information about the transformation matrix, see
    the \l {The Coordinate System} and QMatrix documentation.

    The setWorldMatrix() function can replace or add to the currently
    set worldMatrix(). The resetMatrix() function resets any
    transformations that were made using translate(), scale(),
    shear(), rotate(), setWorldMatrix(), setViewport() and setWindow()
    functions. The deviceMatrix() returns the matrix that transforms
    from logical coordinates to device coordinates of the platform
    dependent paint device. The latter function is only needed when
    using platform painting commands on the platform dependent handle,
    and the platform does not do transformations nativly.

    When drawing with QPainter, we specify points using logical
    coordinates which then are converted into the physical coordinates
    of the paint device. The mapping of the logical coordinates to the
    physical coordinates are handled by QPainter's combinedMatrix(), a
    combination of viewport() and window() and worldMatrix().  The
    viewport() represents the physical coordinates specifying an
    arbitrary rectangle, the window() describes the same rectangle in
    logical coordinates, and the worldMatrix() is identical with the
    transformation matrix.

    See also \l {The Coordinate System} documentation.

    \section1 Clipping

    QPainter can clip any drawing operation to a rectangle, a region,
    or a vector path. The current clip is available using the
    functions clipRegion() and clipPath(). Whether paths or regions are
    preferred (faster) depends on the underlying paintEngine(). For
    example, the QImage paint engine prefers paths while the X11 paint
    engine prefers regions. Setting a clip is done in the painters
    logical coordinates.

    After QPainter's clipping, the paint device may also clip. For
    example, most widgets clip away the pixels used by child widgets,
    and most printers clip away an area near the edges of the paper.
    This additional clipping is not reflected by the return value of
    clipRegion() or hasClipping().

    \section1 Composition Modes
    \target Composition Modes

    QPainter provides the CompositionMode enum which defines the
    Porter-Duff rules for digital image compositing; it describes a
    model for combining the pixels in one image, the source, with the
    pixels in another image, the destination.

    The two most common forms of composition are \l
    {QPainter::CompositionMode}{Source} and \l
    {QPainter::CompositionMode}{SourceOver}.  \l
    {QPainter::CompositionMode}{Source} is used to draw opaque objects
    onto a paint device. In this mode, each pixel in the source
    replaces the corresponding pixel in the destination. In \l
    {QPainter::CompositionMode}{SourceOver} composition mode, the
    source object is transparent and is drawn on top of the
    destination.

    Note that composition transformation operates pixelwise. For that
    reason, there is a difference between using the grahic primitive
    itself and its bounding rectangle: The bounding rect contains
    pixels with alpha == 0 (i.e the pixels surrounding the
    primitive). These pixels will overwrite the other image's pixels,
    affectively clearing those, while the primitive only overwrites
    its own area.

    \table 100%
    \row
    \o \inlineimage qpainter-compositiondemo.png

    \o
    \bold {Composition Modes Demo}

    The \l {demos/composition}{Composition Modes} demo, available in
    Qt's demo directory, allows you to experiment with the various
    composition modes and see the results immediately.

    \endtable

    \section1 Limitations
    \target Limitations

    If you are using coordinates with Qt's raster-based paint engine, it is
    important to note that coordinates greater than +/- 2\sup 15 are not
    valid due to the use of \c{short int} in the implementation.

    \sa QPaintDevice, QPaintEngine, {QtSvg Module}, {Basic Drawing Example}
*/

/*!
    \enum QPainter::RenderHint

    Renderhints are used to specify flags to QPainter that may or
    may not be respected by any given engine.

    \value Antialiasing Indicates that the engine should antialias
    edges of primitives if possible.

    \value TextAntialiasing Indicates that the engine should antialias
    text if possible.

    \value SmoothPixmapTransform Indicates that the engine should use
    a smooth pixmap transformation algorithm (such as bilinear) rather
    than nearest neighbor.

    \value HighQualityAntialiasing An OpenGL-specific rendering hint
    indicating that the engine should use fragment programs and offscreen
    rendering for antialiasing.

    \value NonCosmeticDefaultPen The engine should interpret pens with a width
    of 0 (which otherwise enables QPen::isCosmetic()) as being a non-cosmetic
    pen with a width of 1.

    \sa renderHints(), setRenderHint(), {QPainter#Rendering
    Quality}{Rendering Quality}, {Concentric Circles Example}

*/

/*!
    Constructs a painter.

    \sa begin(), end()
*/

QPainter::QPainter()
{
    d_ptr = new QPainterPrivate(this);
    d_ptr->init();
}

/*!
    \fn QPainter::QPainter(QPaintDevice *device)

    Constructs a painter that begins painting the paint \a device
    immediately.

    This constructor is convenient for short-lived painters, e.g. in a
    QWidget::paintEvent() and should be used only once. The
    constructor calls begin() for you and the QPainter destructor
    automatically calls end().

    Here's an example using begin() and end():
    \snippet doc/src/snippets/code/src.gui.painting.qpainter.cpp 1

    The same example using this constructor:
    \snippet doc/src/snippets/code/src.gui.painting.qpainter.cpp 2

    Since the constructor cannot provide feedback when the initialization
    of the painter failed you should rather use begin() and end() to paint
    on external devices, e.g. printers.

    \sa begin(), end()
*/

QPainter::QPainter(QPaintDevice *pd)
    : d_ptr(0)
{
    Q_ASSERT(pd != 0);
    if (!QPainterPrivate::attachPainterPrivate(this, pd)) {
        d_ptr = new QPainterPrivate(this);
        d_ptr->init();
        begin(pd);
    }
    Q_ASSERT(d_ptr);
}

/*!
    Destroys the painter.
*/
QPainter::~QPainter()
{
    d_ptr->inDestructor = true;
    if (isActive())
        end();
    else if (d_ptr->refcount > 1)
        d_ptr->detachPainterPrivate(this);

    if (d_ptr) {
        // Make sure we haven't messed things up.
        Q_ASSERT(d_ptr->inDestructor);
        d_ptr->inDestructor = false;
        Q_ASSERT(d_ptr->refcount == 1);
        if (d_ptr->d_ptrs)
            delete [] d_ptr->d_ptrs;
        delete d_ptr;
    }
}

/*!
    Returns the paint device on which this painter is currently
    painting, or 0 if the painter is not active.

    \sa isActive()
*/

QPaintDevice *QPainter::device() const
{
    Q_D(const QPainter);
    if (isActive() && d->engine->d_func()->currentClipWidget)
        return d->engine->d_func()->currentClipWidget;
    return d->original_device;
}

/*!
    Returns true if begin() has been called and end() has not yet been
    called; otherwise returns false.

    \sa begin(), QPaintDevice::paintingActive()
*/

bool QPainter::isActive() const
{
    Q_D(const QPainter);
    return d->engine && d->engine->isActive();
}

/*!
    Initializes the painters pen, background and font to the same as
    the given \a widget. Call this function after begin() while the
    painter is active.

    \sa begin(), {QPainter#Settings}{Settings}
*/
void QPainter::initFrom(const QWidget *widget)
{
    Q_ASSERT_X(widget, "QPainter::initFrom(const QWidget *widget)", "Widget cannot be 0");
    if (!isActive()) {
        qWarning("QPainter::initFrom: Painter not active, aborted");
        return;
    }
    const QPalette &pal = widget->palette();
    Q_D(QPainter);
    d->state->pen = QPen(pal.brush(widget->foregroundRole()), 0);
    d->state->bgBrush = pal.brush(widget->backgroundRole());
    d->state->deviceFont = QFont(widget->font(), d->device);
    d->state->font = d->state->deviceFont;
    if (d->engine) {
        d->engine->setDirty(QPaintEngine::DirtyPen);
        d->engine->setDirty(QPaintEngine::DirtyBrush);
        d->engine->setDirty(QPaintEngine::DirtyFont);
    }
    d->state->layoutDirection = widget->layoutDirection();
}


/*!
    Saves the current painter state (pushes the state onto a stack). A
    save() must be followed by a corresponding restore(); the end()
    function unwinds the stack.

    \sa restore()
*/

void QPainter::save()
{
#ifdef QT_DEBUG_DRAW
    if (qt_show_painter_debug_output)
        printf("QPainter::save()\n");
#endif
    if (!isActive()) {
        qWarning("QPainter::save: Painter not active");
        return;
    }

    Q_D(QPainter);
    d->updateState(d->state);

    d->state = new QPainterState(d->states.back());
    d->states.push_back(d->state);
    d->engine->state = d->state;
}

/*!
    Restores the current painter state (pops a saved state off the
    stack).

    \sa save()
*/

void QPainter::restore()
{
#ifdef QT_DEBUG_DRAW
    if (qt_show_painter_debug_output)
        printf("QPainter::restore()\n");
#endif
    Q_D(QPainter);
    if (d->states.size()<=1) {
        qWarning("QPainter::restore: Unbalanced save/restore");
        return;
    } else if (!isActive()) {
        qWarning("QPainter::restore: Painter not active");
        return;
    }

    QPainterState *tmp = d->state;
    d->states.pop_back();
    d->state = d->states.back();
    d->txinv = false;

    // trigger clip update if the clip path/region has changed since
    // last save
    if (!d->state->clipInfo.isEmpty()
        && (tmp->changeFlags & (QPaintEngine::DirtyClipRegion | QPaintEngine::DirtyClipPath))) {
        // reuse the tmp state to avoid any extra allocs...
        tmp->dirtyFlags = QPaintEngine::DirtyClipPath;
        tmp->clipOperation = Qt::NoClip;
        tmp->clipPath = QPainterPath();
        d->engine->updateState(*tmp);
        // replay the list of clip states,
        for (int i=0; i<d->state->clipInfo.size(); ++i) {
            const QPainterClipInfo &info = d->state->clipInfo.at(i);
            tmp->matrix.setMatrix(info.matrix.m11(), info.matrix.m12(), info.matrix.m13(),
                                  info.matrix.m21(), info.matrix.m22(), info.matrix.m23(),
                                  info.matrix.dx() - d->state->redirection_offset.x(),
                                  info.matrix.dy() - d->state->redirection_offset.y(), info.matrix.m33());
            tmp->clipOperation = info.operation;
            if (info.clipType == QPainterClipInfo::RegionClip) {
                tmp->dirtyFlags = QPaintEngine::DirtyClipRegion | QPaintEngine::DirtyTransform;
                tmp->clipRegion = info.region;
            } else { // clipType == QPainterClipInfo::PathClip
                tmp->dirtyFlags = QPaintEngine::DirtyClipPath | QPaintEngine::DirtyTransform;
                tmp->clipPath = info.path;
            }
            d->engine->updateState(*tmp);
        }


        //Since we've updated the clip region anyway, pretend that the clip path hasn't changed:
        d->state->dirtyFlags &= ~(QPaintEngine::DirtyClipPath | QPaintEngine::DirtyClipRegion);
        tmp->changeFlags &= ~(QPaintEngine::DirtyClipPath | QPaintEngine::DirtyClipRegion);
        tmp->changeFlags |= QPaintEngine::DirtyTransform;
    }

    d->updateState(d->state);
    delete tmp;
}


/*!

    \fn bool QPainter::begin(QPaintDevice *device)

    Begins painting the paint \a device and returns true if
    successful; otherwise returns false.

    Notice that all painter settings (setPen(), setBrush() etc.) are reset
    to default values when begin() is called.

    The errors that can occur are serious problems, such as these:

    \snippet doc/src/snippets/code/src.gui.painting.qpainter.cpp 3

    Note that most of the time, you can use one of the constructors
    instead of begin(), and that end() is automatically done at
    destruction.

    \warning A paint device can only be painted by one painter at a
    time.

    \sa end(), QPainter()
*/

bool QPainter::begin(QPaintDevice *pd)
{
    Q_ASSERT(pd);

    if (pd->painters > 0) {
        qWarning("QPainter::begin: A paint device can only be painted by one painter at a time.");
        return false;
    }

    if (d_ptr->engine) {
        qWarning("QPainter::begin: Painter already active");
        return false;
    }

    if (QPainterPrivate::attachPainterPrivate(this, pd))
        return true;

    Q_D(QPainter);
    // Ensure fresh painter state
    if (d->emptyState && !d->state->dirtyFlags)
        d->emptyState = false;
    else
        d->state->init(d->state->painter);

    d->helper_device = pd;
    d->original_device = pd;
    QPaintDevice *rpd = 0;

    // We know for sure that redirection is broken when the widget is inside
    // its paint event, so it's safe to use our hard-coded redirection. However,
    // there IS one particular case we still need to support, and that's
    // when people call QPainter::setRedirected in the widget's paint event right
    // before any painter is created (or QPainter::begin is called). In that
    // particular case our hard-coded redirection is restored and the redirection
    // is retrieved from QPainter::redirected (as before).
    if (pd->devType() == QInternal::Widget)
        rpd = static_cast<QWidget *>(pd)->d_func()->redirected(&d->state->redirection_offset);

    if (!rpd)
        rpd = redirected(pd, &d->state->redirection_offset);

    if (rpd) {
        pd = rpd;
    }

#ifdef QT_DEBUG_DRAW
    if (qt_show_painter_debug_output)
        printf("QPainter::begin(), device=%p, type=%d\n", pd, pd->devType());
#endif


    d->state->bgOrigin = QPointF();

    d->device = pd;
    if (pd->devType() == QInternal::Pixmap)
        static_cast<QPixmap *>(pd)->detach();
    else if (pd->devType() == QInternal::Image)
        static_cast<QImage *>(pd)->detach();

    d->engine = pd->paintEngine();

    if (!d->engine) {
        qWarning("QPainter::begin: Paint device returned engine == 0, type: %d", pd->devType());
        return true;
    }
    switch (pd->devType()) {
        case QInternal::Widget:
        {
            const QWidget *widget = static_cast<const QWidget *>(pd);
            Q_ASSERT(widget);

            const bool paintOutsidePaintEvent = widget->testAttribute(Qt::WA_PaintOutsidePaintEvent);
            const bool inPaintEvent = widget->testAttribute(Qt::WA_WState_InPaintEvent);
            if(!d->engine->hasFeature(QPaintEngine::PaintOutsidePaintEvent)
                && !paintOutsidePaintEvent && !inPaintEvent) {
                qWarning("QPainter::begin: Widget painting can only begin as a "
                         "result of a paintEvent");
                return false;
            }

            // Adjust offset for alien widgets painting outside the paint event.
            if (!inPaintEvent && paintOutsidePaintEvent && !widget->internalWinId()
                && widget->testAttribute(Qt::WA_WState_Created)) {
                d->state->redirection_offset -= widget->mapTo(widget->nativeParentWidget(), QPoint());
            }
            break;
        }
        case QInternal::Pixmap:
        {
            const QPixmap *pm = static_cast<const QPixmap *>(pd);
            Q_ASSERT(pm);
            if (pm->isNull()) {
                qWarning("QPainter::begin: Cannot paint on a null pixmap");
                return false;
            }

            if (pm->depth() == 1) {
                d->state->pen = QPen(Qt::color1);
                d->state->brush = QBrush(Qt::color0);
            }
            break;
        }
        case QInternal::Image:
        {
            const QImage *img = static_cast<const QImage *>(pd);
            Q_ASSERT(img);
            if (img->isNull()) {
                qWarning("QPainter::begin: Cannot paint on a null image");
                return false;
            }
            if (img->depth() == 1) {
                d->state->pen = QPen(Qt::color1);
                d->state->brush = QBrush(Qt::color0);
            }
            break;
        }
        default:
            break;
    }
    if (d->state->ww == 0) // For compat with 3.x painter defaults
        d->state->ww = d->state->wh = d->state->vw = d->state->vh = 1024;

    // Slip a painter state into the engine before we do any other operations
    d->engine->state = d->state;

    d->engine->setPaintDevice(pd);

    bool begun = d->engine->begin(pd);
    if (!begun) {
        qWarning("QPainter::begin(): Returned false");
        if (isActive())
            end();
        d->engine = 0;
        d->device = 0;
        return false;
    } else {
        d->engine->setActive(begun);
    }

    // Copy painter properties from original paint device,
    // required for QPixmap::grabWidget()
    if (d->original_device->devType() == QInternal::Widget) {
        QWidget *widget = static_cast<QWidget *>(d->original_device);
        initFrom(widget);
    } else {
        d->state->layoutDirection = QApplication::layoutDirection();
        // make sure we have a font compatible with the paintdevice
        d->state->deviceFont = d->state->font = QFont(d->state->deviceFont, d->device);
    }

    QRect systemRect = d->engine->systemRect();
    if (!systemRect.isEmpty()) {
        d->state->ww = d->state->vw = systemRect.width();
        d->state->wh = d->state->vh = systemRect.height();
    } else {
        d->state->ww = d->state->vw = pd->metric(QPaintDevice::PdmWidth);
        d->state->wh = d->state->vh = pd->metric(QPaintDevice::PdmHeight);
    }

    d->state->redirection_offset += d->engine->coordinateOffset();

    Q_ASSERT(d->engine->isActive());

    if (!d->state->redirection_offset.isNull())
        d->updateMatrix();

    Q_ASSERT(d->engine->isActive());
    d->state->renderHints = QPainter::TextAntialiasing;
    ++d->device->painters;

    d->state->emulationSpecifier = 0;

    return true;
}

/*!
    Ends painting. Any resources used while painting are released. You
    don't normally need to call this since it is called by the
    destructor.

    Returns true if the painter is no longer active; otherwise returns false.

    \sa begin(), isActive()
*/

bool QPainter::end()
{
#ifdef QT_DEBUG_DRAW
    if (qt_show_painter_debug_output)
        printf("QPainter::end()\n");
#endif

    if (!isActive()) {
        qWarning("QPainter::end: Painter not active, aborted");
        return false;
    }

    if (d_ptr->refcount > 1) {
        d_ptr->detachPainterPrivate(this);
        return true;
    }

    Q_D(QPainter);
    if (d->states.size()>1) {
        qWarning("QPainter::end: Painter ended with %d saved states",
                 d->states.size());
    }

    bool ended = true;

    d->fillrect_func = 0;

    if (d->engine->isActive()) {
        ended = d->engine->end();
        d->updateState(0);

        --d->device->painters;
        if (d->device->painters == 0) {
            d->engine->setPaintDevice(0);
            d->engine->setActive(false);
        }
    }

    if (d->engine->autoDestruct()) {
        delete d->engine;
    }

    d->engine = 0;

    d->device = 0;
    return ended;
}


/*!
    Returns the paint engine that the painter is currently operating
    on if the painter is active; otherwise 0.

    \sa isActive()
*/
QPaintEngine *QPainter::paintEngine() const
{
    Q_D(const QPainter);
    return d->engine;
}


/*!
    Returns the font metrics for the painter if the painter is
    active. Otherwise, the return value is undefined.

    \sa font(), isActive(), {QPainter#Settings}{Settings}
*/

QFontMetrics QPainter::fontMetrics() const
{
    Q_D(const QPainter);
    return QFontMetrics(d->state->font);
}


/*!
    Returns the font info for the painter if the painter is
    active. Otherwise, the return value is undefined.

    \sa font(), isActive(), {QPainter#Settings}{Settings}
*/

QFontInfo QPainter::fontInfo() const
{
    Q_D(const QPainter);
    return QFontInfo(d->state->font);
}

/*!
    \since 4.2

    Returns the opacity of the painter. The default value is
    1.
*/

qreal QPainter::opacity() const
{
    Q_D(const QPainter);
    return d->state->localOpacity;
}

/*!
    \since 4.2

    Sets the opacity of the painter to \a opacity. The value should
    be in the range 0.0 to 1.0, where 0.0 is fully transparent and
    1.0 is fully opaque.

    Opacity set on the painter will apply to all drawing operations
    individually.
*/

void QPainter::setOpacity(qreal opacity)
{
    Q_D(QPainter);
    d->state->opacity = qMin(qreal(1), qMax(qreal(0), opacity));
    d->state->dirtyFlags |= QPaintEngine::DirtyOpacity;
    d->state->localOpacity = d->state->opacity;
    d->updateCombinedOpacity();
}


/*!
    Returns the currently set brush origin.

    \sa setBrushOrigin(), {QPainter#Settings}{Settings}
*/

QPoint QPainter::brushOrigin() const
{
    Q_D(const QPainter);
    return QPointF(d->state->bgOrigin).toPoint();
}

/*!
    \fn void QPainter::setBrushOrigin(const QPointF &position)

    Sets the brush origin to \a position.

    The brush origin specifies the (0, 0) coordinate of the painter's
    brush. This setting only applies to pattern brushes and pixmap
    brushes.

    Note that while the brushOrigin() was necessary to adopt the
    parent's background for a widget in Qt 3, this is no longer the
    case since the Qt 4 painter doesn't paint the background unless
    you explicitly tell it to do so by setting the widget's \l
    {QWidget::autoFillBackground}{autoFillBackground} property to
    true.

    \sa brushOrigin(), {QPainter#Settings}{Settings}
*/

void QPainter::setBrushOrigin(const QPointF &p)
{
    Q_D(QPainter);
#ifdef QT_DEBUG_DRAW
    if (qt_show_painter_debug_output)
        printf("QPainter::setBrushOrigin(), (%.2f,%.2f)\n", p.x(), p.y());
#endif
    d->state->bgOrigin = p;
    d->state->dirtyFlags |= QPaintEngine::DirtyBrushOrigin;
}

/*!
    \fn void QPainter::setBrushOrigin(const QPoint &position)
    \overload

    Sets the brush's origin to the given \a position.
*/

/*!
    \fn void QPainter::setBrushOrigin(int x, int y)

    \overload

    Sets the brush's origin to point (\a x, \a y).
*/

/*!
    \enum QPainter::CompositionMode

    Defines the Porter-Duff rules for digital image
    compositing. Composition modes are used to specify how the pixels
    in one image, the source, are merged with the pixel in another
    image, the destination.

     \image qpainter-compositionmode1.png
     \image qpainter-compositionmode2.png

    The most common type is SourceOver (often referred to as just
    alpha blending) where the source pixel is blended on top of the
    destination pixel in such a way that the alpha component of the
    source defines the translucency of the pixel.

    When the paint device is a QImage, the image format must be set to
    \l {QImage::Format}{Format_ARGB32Premultiplied} or
    \l {QImage::Format}{Format_ARGB32} for the composition modes to have
    any effect. For performance the premultiplied version is the preferred
    format.

    When a composition mode is set it applies to all painting
    operator, pens, brushes, gradients and pixmap/image drawing.

    \value CompositionMode_SourceOver This is the default mode. The
    alpha of the source is used to blend the pixel on top of the
    destination.

    \value CompositionMode_DestinationOver The alpha of the
    destination is used to blend it on top of the source pixels. This
    mode is the inverse of CompositionMode_SourceOver.

    \value CompositionMode_Clear The pixels in the destination are
    cleared (set to fully transparent) independent of the source.

    \value CompositionMode_Source The output is the source
    pixel. (This means a basic copy operation and is identical to
    SourceOver when the source pixel is opaque).

    \value CompositionMode_Destination The output is the destination
    pixel. This means that the blending has no effect. This mode is
    the inverse of CompositionMode_Source.

    \value CompositionMode_SourceIn The output is the source, where
    the alpha is reduced by that of the destination.

    \value CompositionMode_DestinationIn The output is the
    destination, where the alpha is reduced by that of the
    source. This mode is the inverse of CompositionMode_SourceIn.

    \value CompositionMode_SourceOut The output is the source, where
    the alpha is reduced by the inverse of destination.

    \value CompositionMode_DestinationOut The output is the
    destination, where the alpha is reduced by the inverse of the
    source. This mode is the inverse of CompositionMode_SourceOut.

    \value CompositionMode_SourceAtop The source pixel is blended on
    top of the destination, with the alpha of the source pixel reduced
    by the alpha of the destination pixel.

    \value CompositionMode_DestinationAtop The destination pixel is
    blended on top of the source, with the alpha of the destination
    pixel is reduced by the alpha of the destination pixel. This mode
    is the inverse of CompositionMode_SourceAtop.

    \value CompositionMode_Xor The source, whose alpha is reduced with
    the inverse of the destination alpha, is merged with the
    destination, whose alpha is reduced by the inverse of the source
    alpha. CompositionMode_Xor is not the same as the bitwise Xor.

    \value CompositionMode_Plus Both the alpha and color of the source
    and destination pixels are added together.

    \value CompositionMode_Multiply The output is the source color
    multiplied by the destination. Multiplying a color with white
    leaves the color unchanged, while multiplying a color
    with black produces black.

    \value CompositionMode_Screen The source and destination colors
    are inverted and then multiplied. Screening a color with white
    produces white, whereas screening a color with black leaves the
    color unchanged.

    \value CompositionMode_Overlay Multiplies or screens the colors
    depending on the destination color. The destination color is mixed
    with the source color to reflect the lightness or darkness of the
    destination.

    \value CompositionMode_Darken The darker of the source and destination
    colors is selected.

    \value CompositionMode_Lighten The lighter of the source and destination
    colors is selected.

    \value CompositionMode_ColorDodge The destination color is brightened
    to reflect the source color. A black source color leaves the destination
    color unchanged.

    \value CompositionMode_ColorBurn The destination color is darkened to
    reflect the source color. A white source color leaves the destination color
    unchanged.

    \value CompositionMode_HardLight Multiplies or screens the colors depending
    on the source color. A light source color will lighten the destination color,
    whereas a dark source color will darken the destination color.

    \value CompositionMode_SoftLight Darkens or lightens the colors depending on the
    source color. Similar to CompositionMode_HardLight.

    \value CompositionMode_Difference Subtracts the darker of the colors from the lighter.
    Painting with white inverts the destination color, whereas painting with black leaves the
    destination color unchanged.

    \value CompositionMode_Exclusion Similar to CompositionMode_Difference,
    but with a lower contrast. Painting with white inverts the destination color, whereas painting
    with black leaves the destination color unchanged.

    \sa compositionMode(), setCompositionMode(), {QPainter#Composition
    Modes}{Composition Modes}, {Image Composition Example}
*/

/*!
    Sets the composition mode to the given \a mode.

    \warning You can only set the composition mode for QPainter
    objects that operates on a QImage.

    \sa compositionMode()
*/
void QPainter::setCompositionMode(CompositionMode mode)
{
    Q_D(QPainter);
    if (!isActive()) {
        qWarning("QPainter::setCompositionMode: Painter not active");
        return;
    } else if (!d->engine->hasFeature(QPaintEngine::PorterDuff)) {
        if (mode != CompositionMode_Source && mode != CompositionMode_SourceOver) {
            qWarning("QPainter::setCompositionMode: PorterDuff modes not supported on device");
            return;
        }
    }

    if (mode > QPainter::CompositionMode_Xor &&
        !d->engine->hasFeature(QPaintEngine::BlendModes)) {
        qWarning("QPainter::setCompositionMode: Blend modes not supported on device");
        return;
    }

    d->state->composition_mode = mode;
    d->state->dirtyFlags |= QPaintEngine::DirtyCompositionMode;
}

/*!
  Returns the current composition mode.

  \sa CompositionMode, setCompositionMode()
*/
QPainter::CompositionMode QPainter::compositionMode() const
{
    Q_D(const QPainter);
    return d->state->composition_mode;
}

/*!
    Returns the current background brush.

    \sa setBackground(), {QPainter#Settings}{Settings}
*/

const QBrush &QPainter::background() const
{
    Q_D(const QPainter);
    return d->state->bgBrush;
}


/*!
    Returns true if clipping has been set; otherwise returns false.

    \sa setClipping(), {QPainter#Clipping}{Clipping}
*/

bool QPainter::hasClipping() const
{
    Q_D(const QPainter);
    return d->state->clipEnabled && d->state->clipOperation != Qt::NoClip;
}


/*!
    Enables clipping if  \a enable is true, or disables clipping if  \a
    enable is false.

    \sa hasClipping(), {QPainter#Clipping}{Clipping}
*/

void QPainter::setClipping(bool enable)
{
    Q_D(QPainter);
#ifdef QT_DEBUG_DRAW
    if (qt_show_painter_debug_output)
        printf("QPainter::setClipping(), enable=%s, was=%s\n",
               enable ? "on" : "off",
               hasClipping() ? "on" : "off");
#endif
    if (!isActive()) {
        qWarning("QPainter::setClipping: Painter not active, state will be reset by begin");
        return;
    }

    if (hasClipping() == enable)
        return;

    // we can't enable clipping if we don't have a clip
    if (enable
        && (d->state->clipInfo.isEmpty() || d->state->clipInfo.last().operation == Qt::NoClip))
        return;

    if (enable)
        d->state->clipEnabled = true;
    else
        d->state->clipEnabled = false;

    d->state->dirtyFlags |= QPaintEngine::DirtyClipEnabled;
    d->updateState(d->state);
}


/*!
    Returns the currently set clip region. Note that the clip region
    is given in logical coordinates.

    \sa setClipRegion(), clipPath(), setClipping()
*/

QRegion QPainter::clipRegion() const
{
    if (!isActive()) {
        qWarning("QPainter::clipRegion: Painter not active");
        return QRegion();
    }

    Q_D(const QPainter);
    QRegion region;
    bool lastWasNothing = true;

    if (!d->txinv)
        const_cast<QPainter *>(this)->d_ptr->updateInvMatrix();

    for (int i=0; i<d->state->clipInfo.size(); ++i) {
        const QPainterClipInfo &info = d->state->clipInfo.at(i);
        QRegion other;
        switch (info.clipType) {

        case QPainterClipInfo::RegionClip: {
            QTransform matrix = (info.matrix * d->invMatrix);
            if (lastWasNothing) {
                region = info.region * matrix;
                lastWasNothing = false;
                continue;
            }
            if (info.operation == Qt::IntersectClip)
                region &= info.region * matrix;
            else if (info.operation == Qt::UniteClip)
                region |= info.region * matrix;
            else if (info.operation == Qt::NoClip) {
                lastWasNothing = true;
                region = QRegion();
            } else
                region = info.region * matrix;
            break;
        }

        case QPainterClipInfo::PathClip: {
            QTransform matrix = (info.matrix * d->invMatrix);
            if (lastWasNothing) {
                region = QRegion((info.path * matrix).toFillPolygon().toPolygon(),
                                 info.path.fillRule());
                lastWasNothing = false;
                continue;
            }
            if (info.operation == Qt::IntersectClip) {
                region &= QRegion((info.path * matrix).toFillPolygon().toPolygon(),
                                  info.path.fillRule());
            } else if (info.operation == Qt::UniteClip) {
                region |= QRegion((info.path * matrix).toFillPolygon().toPolygon(),
                                  info.path.fillRule());
            } else if (info.operation == Qt::NoClip) {
                lastWasNothing = true;
                region = QRegion();
            } else {
                region = QRegion((info.path * matrix).toFillPolygon().toPolygon(),
                                 info.path.fillRule());
            }
            break;
        }
        }
    }

    return region;
}

/*!
    Returns the currently clip as a path. Note that the clip path is
    given in logical coordinates.

    \sa setClipPath(), clipRegion(), setClipping()
*/
QPainterPath QPainter::clipPath() const
{
    // ### Since we do not support path intersections and path unions yet,
    // we just use clipRegion() here...
    if (!isActive()) {
        qWarning("QPainter::clipPath: Painter not active");
        return QPainterPath();
    }

    Q_D(const QPainter);
    // No clip, return empty
    if (d->state->clipInfo.size() == 0) {
        return QPainterPath();
    } else {

        // Update inverse matrix, used below.
        if (!d->txinv)
            const_cast<QPainter *>(this)->d_ptr->updateInvMatrix();

        // For the simple case avoid conversion.
        if (d->state->clipInfo.size() == 1
            && d->state->clipInfo.at(0).clipType == QPainterClipInfo::PathClip) {
            QTransform matrix = (d->state->clipInfo.at(0).matrix * d->invMatrix);
            return d->state->clipInfo.at(0).path * matrix;

        // Fallback to clipRegion() for now, since we don't have isect/unite for paths
        } else {
            QPainterPath path;
            path.addRegion(clipRegion());
            return path;
        }
    }
}

/*!
    \fn void QPainter::setClipRect(const QRectF &rectangle, Qt::ClipOperation operation)

    Enables clipping, and sets the clip region to the given \a
    rectangle using the given clip \a operation. The default operation
    is to replace the current clip rectangle.

    Note that the clip rectangle is specified in logical (painter)
    coordinates.

    \sa clipRegion(), setClipping(), {QPainter#Clipping}{Clipping}
*/
void QPainter::setClipRect(const QRectF &rect, Qt::ClipOperation op)
{
    if (qreal(int(rect.top())) == rect.top()
        && qreal(int(rect.bottom())) == rect.bottom()
        && qreal(int(rect.left())) == rect.left()
        && qreal(int(rect.right())) == rect.right())
    {
        setClipRect(rect.toRect(), op);
        return;
    }

    if (rect.isEmpty()) {
        setClipRegion(QRegion(), op);
        return;
    }

    QPainterPath path;
    path.addRect(rect);
    setClipPath(path, op);
}

/*!
    \fn void QPainter::setClipRect(const QRect &rectangle, Qt::ClipOperation operation)
    \overload

    Enables clipping, and sets the clip region to the given \a rectangle using the given
    clip \a operation.
*/
void QPainter::setClipRect(const QRect &rect, Qt::ClipOperation op)
{
    if (!isActive())
        return;

    Q_D(QPainter);

    if (!hasClipping() && (op == Qt::IntersectClip || op == Qt::UniteClip))
        op = Qt::ReplaceClip;

    d->state->clipRegion = rect;
    d->state->clipOperation = op;
    if (op == Qt::NoClip || op == Qt::ReplaceClip)
        d->state->clipInfo.clear();
    d->state->clipInfo << QPainterClipInfo(rect, op, d->state->worldMatrix);
    d->state->clipEnabled = true;
    d->state->dirtyFlags |= QPaintEngine::DirtyClipRegion | QPaintEngine::DirtyClipEnabled;
    d->updateState(d->state);
}

/*!
    \fn void QPainter::setClipRect(int x, int y, int width, int height, Qt::ClipOperation operation)

    Enables clipping, and sets the clip region to the rectangle beginning at (\a x, \a y)
    with the given \a width and \a height.
*/

/*!
    \fn void QPainter::setClipRegion(const QRegion &region, Qt::ClipOperation operation)

    Sets the clip region to the given \a region using the specified clip
    \a operation. The default clip operation is to replace the current
    clip region.

    Note that the clip region is given in logical coordinates.

    \sa clipRegion(), setClipRect(), {QPainter#Clipping}{Clipping}
*/
void QPainter::setClipRegion(const QRegion &r, Qt::ClipOperation op)
{
    Q_D(QPainter);
#ifdef QT_DEBUG_DRAW
    QRect rect = r.boundingRect();
    if (qt_show_painter_debug_output)
        printf("QPainter::setClipRegion(), size=%d, [%d,%d,%d,%d]\n",
           r.rects().size(), rect.x(), rect.y(), rect.width(), rect.height());
#endif
    if (!isActive()) {
        qWarning("QPainter::setClipRegion: Painter not active");
        return;
    }

    if (!hasClipping() && (op == Qt::IntersectClip || op == Qt::UniteClip))
        op = Qt::ReplaceClip;

    d->state->clipRegion = r;
    d->state->clipOperation = op;
    if (op == Qt::NoClip || op == Qt::ReplaceClip)
        d->state->clipInfo.clear();
    d->state->clipInfo << QPainterClipInfo(r, op, d->state->worldMatrix);
    d->state->clipEnabled = true;
    d->state->dirtyFlags |= QPaintEngine::DirtyClipRegion | QPaintEngine::DirtyClipEnabled;
    d->updateState(d->state);
}

/*!
    \since 4.2

    Sets the transformation matrix to \a matrix and enables transformations.

    \note It is advisable to use setWorldTransform() instead of this function to
    preserve the properties of perspective transformations.

    If \a combine is true, then \a matrix is combined with the current
    transformation matrix; otherwise \a matrix replaces the current
    transformation matrix.

    If \a matrix is the identity matrix and \a combine is false, this
    function calls setWorldMatrixEnabled(false). (The identity matrix is the
    matrix where QMatrix::m11() and QMatrix::m22() are 1.0 and the
    rest are 0.0.)

    The following functions can transform the coordinate system without using
    a QMatrix:
    \list
    \i translate()
    \i scale()
    \i shear()
    \i rotate()
    \endlist

    They operate on the painter's worldMatrix() and are implemented like this:

    \snippet doc/src/snippets/code/src.gui.painting.qpainter.cpp 4

    Note that when using setWorldMatrix() function you should always have
    \a combine be true when you are drawing into a QPicture. Otherwise
    it may not be possible to replay the picture with additional
    transformations; using the translate(), scale(), etc. convenience
    functions is safe.

    For more information about the coordinate system, transformations
    and window-viewport conversion, see \l {The Coordinate System}
    documentation.

    \sa worldMatrixEnabled(), QMatrix
*/

void QPainter::setWorldMatrix(const QMatrix &matrix, bool combine)
{
    setWorldTransform(QTransform(matrix), combine);
}

/*!
    \since 4.2

    Returns the world transformation matrix.

    It is advisable to use worldTransform() because worldMatrix() does not
    preserve the properties of perspective transformations.

    \sa {QPainter#Coordinate Transformations}{Coordinate Transformations},
    {The Coordinate System}
*/

const QMatrix &QPainter::worldMatrix() const
{
    Q_D(const QPainter);
    return d->state->worldMatrix.toAffine();
}

/*!
    \obsolete

    Use setWorldTransform() instead.

    \sa setWorldTransform()
*/

void QPainter::setMatrix(const QMatrix &matrix, bool combine)
{
    setWorldTransform(QTransform(matrix), combine);
}

/*!
    \obsolete

    Use worldTransform() instead.

    \sa worldTransform()
*/

const QMatrix &QPainter::matrix() const
{
    return worldMatrix();
}


/*!
    \since 4.2

    Returns the transformation matrix combining the current
    window/viewport and world transformation.

    It is advisable to use combinedTransform() instead of this
    function to preserve the properties of perspective transformations.

    \sa setWorldMatrix(), setWindow(), setViewport()
*/
QMatrix QPainter::combinedMatrix() const
{
    return combinedTransform().toAffine();
}


/*!
    Returns the matrix that transforms from logical coordinates to
    device coordinates of the platform dependent paint device.

    \note It is advisable to use deviceTransform() instead of this
    function to preserve the properties of perspective transformations.

    This function is \e only needed when using platform painting
    commands on the platform dependent handle (Qt::HANDLE), and the
    platform does not do transformations nativly.

    The QPaintEngine::PaintEngineFeature enum can be queried to
    determine whether the platform performs the transformations or
    not.

    \sa worldMatrix(), QPaintEngine::hasFeature(),
*/
const QMatrix &QPainter::deviceMatrix() const
{
    Q_D(const QPainter);
    return d->state->matrix.toAffine();
}

/*!
    Resets any transformations that were made using translate(), scale(),
    shear(), rotate(), setWorldMatrix(), setViewport() and
    setWindow().

    It is advisable to use resetTransform() instead of this function
    to preserve the properties of perspective transformations.

    \sa {QPainter#Coordinate Transformations}{Coordinate
    Transformations}
*/

void QPainter::resetMatrix()
{
    resetTransform();
}


/*!
    \since 4.2

    Enables transformations if \a enable is true, or disables
    transformations if \a enable is false. The world transformation
    matrix is not changed.

    \sa worldMatrixEnabled(), worldMatrix(), {QPainter#Coordinate
    Transformations}{Coordinate Transformations}
*/

void QPainter::setWorldMatrixEnabled(bool enable)
{
    Q_D(QPainter);
#ifdef QT_DEBUG_DRAW
    if (qt_show_painter_debug_output)
        printf("QPainter::setMatrixEnabled(), enable=%d\n", enable);
#endif

    if (!isActive()) {
        qWarning("QPainter::setMatrixEnabled: Painter not active");
        return;
    }
    if (enable == d->state->WxF)
        return;

    d->state->WxF = enable;
    d->updateMatrix();
}

/*!
    \since 4.2

    Returns true if world transformation is enabled; otherwise returns
    false.

    \sa setWorldMatrixEnabled(), worldMatrix(), {The Coordinate System}
*/

bool QPainter::worldMatrixEnabled() const
{
    Q_D(const QPainter);
    return d->state->WxF;
}

/*!
    \obsolete

    Use setWorldMatrixEnabled() instead.

    \sa setWorldMatrixEnabled()
*/

void QPainter::setMatrixEnabled(bool enable)
{
    setWorldMatrixEnabled(enable);
}

/*!
    \obsolete

    Use worldMatrixEnabled() instead

    \sa worldMatrixEnabled()
*/

bool QPainter::matrixEnabled() const
{
    return worldMatrixEnabled();
}

/*!
    Scales the coordinate system by (\a{sx}, \a{sy}).

    \sa setWorldMatrix() {QPainter#Coordinate Transformations}{Coordinate
    Transformations}
*/

void QPainter::scale(qreal sx, qreal sy)
{
#ifdef QT_DEBUG_DRAW
    if (qt_show_painter_debug_output)
        printf("QPainter::scale(), sx=%f, sy=%f\n", sx, sy);
#endif

    QTransform m;
    m.scale(sx, sy);
    setTransform(m, true);
}

/*!
    Shears the coordinate system by (\a{sh}, \a{sv}).

    \sa setWorldMatrix(), {QPainter#Coordinate Transformations}{Coordinate
    Transformations}
*/

void QPainter::shear(qreal sh, qreal sv)
{
#ifdef QT_DEBUG_DRAW
    if (qt_show_painter_debug_output)
        printf("QPainter::shear(), sh=%f, sv=%f\n", sh, sv);
#endif
    QTransform m;
    m.shear(sv, sh);
    setTransform(m, true);
}

/*!
    \fn void QPainter::rotate(qreal angle)

    Rotates the coordinate system the given \a angle clockwise.

    \sa setWorldMatrix(), {QPainter#Coordinate Transformations}{Coordinate
    Transformations}
*/

void QPainter::rotate(qreal a)
{
#ifdef QT_DEBUG_DRAW
    if (qt_show_painter_debug_output)
        printf("QPainter::rotate(), angle=%f\n", a);
#endif
    QTransform m;
    m.rotate(a);
    setTransform(m, true);
}

/*!
    Translates the coordinate system by the given \a offset; i.e. the
    given \a offset is added to points.

    \sa setWorldMatrix(), {QPainter#Coordinate Transformations}{Coordinate
    Transformations}
*/
void QPainter::translate(const QPointF &offset)
{
    qreal dx = offset.x();
    qreal dy = offset.y();
#ifdef QT_DEBUG_DRAW
    if (qt_show_painter_debug_output)
        printf("QPainter::translate(), dx=%f, dy=%f\n", dx, dy);
#endif

    QTransform m;
    m.translate(dx, dy);
    setTransform(m, true);
}

/*!
    \fn void QPainter::translate(const QPoint &offset)
    \overload

    Translates the coordinate system by the given \a offset.
*/

/*!
    \fn void QPainter::translate(qreal dx, qreal dy)
    \overload

    Translates the coordinate system by the vector (\a dx, \a dy).
*/

/*!
    \fn void QPainter::setClipPath(const QPainterPath &path, Qt::ClipOperation operation)

    Enables clipping, and sets the clip path for the painter to the
    given \a path, with the clip \a operation.

    Note that the clip path is specified in logical (painter)
    coordinates.

    \sa clipPath(), clipRegion(), {QPainter#Clipping}{Clipping}

*/
void QPainter::setClipPath(const QPainterPath &path, Qt::ClipOperation op)
{
#ifdef QT_DEBUG_DRAW
    if (qt_show_painter_debug_output) {
        QRectF b = path.boundingRect();
        printf("QPainter::setClipPath(), size=%d, op=%d, bounds=[%.2f,%.2f,%.2f,%.2f]\n",
               path.elementCount(), op, b.x(), b.y(), b.width(), b.height());
    }
#endif
    if (!isActive())
        return;

    Q_D(QPainter);

    if (!hasClipping() && (op == Qt::IntersectClip || op == Qt::UniteClip))
        op = Qt::ReplaceClip;

    d->state->clipPath = path;
    d->state->clipOperation = op;
    if (op == Qt::NoClip || op == Qt::ReplaceClip)
        d->state->clipInfo.clear();
    d->state->clipInfo << QPainterClipInfo(path, op, d->state->worldMatrix);
    d->state->clipEnabled = true;
    d->state->dirtyFlags |= QPaintEngine::DirtyClipPath | QPaintEngine::DirtyClipEnabled;
    d->updateState(d->state);
}

/*!
    Draws the outline (strokes) the path \a path with the pen specified
    by \a pen

    \sa fillPath(), {QPainter#Drawing}{Drawing}
*/
void QPainter::strokePath(const QPainterPath &path, const QPen &pen)
{
    if (!isActive())
        return;

    Q_D(QPainter);
    QBrush oldBrush = d->state->brush;
    QPen oldPen = d->state->pen;

    d->state->pen = pen;
    d->state->brush = Qt::NoBrush;
    d->engine->setDirty(QPaintEngine::DirtyFlags(QPaintEngine::DirtyPen | QPaintEngine::DirtyBrush));

    drawPath(path);

    // Reset old state
    d->state->pen = oldPen;
    d->state->brush = oldBrush;
    d->engine->setDirty(QPaintEngine::DirtyFlags(QPaintEngine::DirtyPen | QPaintEngine::DirtyBrush));
}

/*!
    Fills the given \a path using the given \a brush. The outline is
    not drawn.

    Alternatively, you can specify a QColor instead of a QBrush; the
    QBrush constructor (taking a QColor argument) will automatically
    create a solid pattern brush.

    \sa drawPath()
*/
void QPainter::fillPath(const QPainterPath &path, const QBrush &brush)
{
    if (!isActive())
        return;

    Q_D(QPainter);
    QBrush oldBrush = d->state->brush;
    QPen oldPen = d->state->pen;

    d->state->pen = Qt::NoPen;
    d->state->brush = brush;
    d->engine->setDirty(QPaintEngine::DirtyFlags(QPaintEngine::DirtyPen | QPaintEngine::DirtyBrush));

    drawPath(path);

    // Reset old state
    d->state->pen = oldPen;
    d->state->brush = oldBrush;
    d->engine->setDirty(QPaintEngine::DirtyFlags(QPaintEngine::DirtyPen | QPaintEngine::DirtyBrush));
}

/*!
    Draws the given painter \a path using the current pen for outline
    and the current brush for filling.

    \table 100%
    \row
    \o \inlineimage qpainter-path.png
    \o
    \snippet doc/src/snippets/code/src.gui.painting.qpainter.cpp 5
    \endtable

    \sa {painting/painterpaths}{the Painter Paths
    example},{demos/deform}{the Vector Deformation demo}
*/
void QPainter::drawPath(const QPainterPath &path)
{
#ifdef QT_DEBUG_DRAW
    QRectF pathBounds = path.boundingRect();
    if (qt_show_painter_debug_output)
        printf("QPainter::drawPath(), size=%d, [%.2f,%.2f,%.2f,%.2f]\n",
               path.elementCount(),
               pathBounds.x(), pathBounds.y(), pathBounds.width(), pathBounds.height());
#endif

    if (!isActive())
        return;

    Q_D(QPainter);
    d->updateState(d->state);

    if (d->engine->hasFeature(QPaintEngine::PainterPaths) && d->state->emulationSpecifier == 0) {
        d->engine->drawPath(path);
    } else {
        d->draw_helper(path);
    }
}

/*!
    \fn void QPainter::drawLine(const QLineF &line)

    Draws a line defined by \a line.

    \table 100%
    \row
    \o \inlineimage qpainter-line.png
    \o
    \snippet doc/src/snippets/code/src.gui.painting.qpainter.cpp 6
    \endtable

    \sa drawLines(), drawPolyline(), {The Coordinate System}
*/

/*!
    \fn void QPainter::drawLine(const QLine &line)
    \overload

    Draws a line defined by \a line.
*/

/*!
    \fn void QPainter::drawLine(const QPoint &p1, const QPoint &p2)
    \overload

    Draws a line from \a p1 to \a p2.
*/

/*!
    \fn void QPainter::drawLine(const QPointF &p1, const QPointF &p2)
    \overload

    Draws a line from \a p1 to \a p2.
*/

/*!
    \fn void QPainter::drawLine(int x1, int y1, int x2, int y2)
    \overload

    Draws a line from (\a x1, \a y1) to (\a x2, \a y2) and sets the
    current pen position to (\a x2, \a y2).
*/

/*!
    \fn void QPainter::drawRect(const QRectF &rectangle)

    Draws the current \a rectangle with the current pen and brush.

    A filled rectangle has a size of \a{rectangle}.size(). A stroked
    rectangle has a size of \a{rectangle}.size() plus the pen width.

    \table 100%
    \row
    \o \inlineimage qpainter-rectangle.png
    \o
    \snippet doc/src/snippets/code/src.gui.painting.qpainter.cpp 7
    \endtable

    \sa drawRects(), drawPolygon(), {The Coordinate System}
*/

/*!
    \fn void QPainter::drawRect(const QRect &rectangle)

    \overload

    Draws the current \a rectangle with the current pen and brush.
*/

/*!
    \fn void QPainter::drawRect(int x, int y, int width, int height)

    \overload

    Draws a rectangle with upper left corner at (\a{x}, \a{y}) and
    with the given \a width and \a height.
*/

/*!
    \fn void QPainter::drawRects(const QRectF *rectangles, int rectCount)

    Draws the first \a rectCount of the given \a rectangles using the
    current pen and brush.

    \sa drawRect()
*/
void QPainter::drawRects(const QRectF *rects, int rectCount)
{
#ifdef QT_DEBUG_DRAW
    if (qt_show_painter_debug_output)
        printf("QPainter::drawRects(), count=%d\n", rectCount);
#endif
    if (!isActive() || rectCount <= 0)
        return;

    Q_D(QPainter);
    d->updateState(d->state);

    if (!d->state->emulationSpecifier) {
        d->engine->drawRects(rects, rectCount);
        return;
    }

    if (d->state->emulationSpecifier == QPaintEngine::PrimitiveTransform
        && d->state->txop == QTransform::TxTranslate) {
        for (int i=0; i<rectCount; ++i) {
            QRectF r(rects[i].x() + d->state->matrix.dx(),
                     rects[i].y() + d->state->matrix.dy(),
                     rects[i].width(),
                     rects[i].height());
            d->engine->drawRects(&r, 1);
        }
    } else {
        if (d->state->brushNeedsResolving() || d->state->penNeedsResolving()) {
            for (int i=0; i<rectCount; ++i) {
                QPainterPath rectPath;
                rectPath.addRect(rects[i]);
                d->draw_helper(rectPath, QPainterPrivate::StrokeAndFillDraw);
            }
        } else {
            QPainterPath rectPath;
            for (int i=0; i<rectCount; ++i)
                rectPath.addRect(rects[i]);
            d->draw_helper(rectPath, QPainterPrivate::StrokeAndFillDraw);
        }
    }
}

/*!
    \fn void QPainter::drawRects(const QRect *rectangles, int rectCount)
    \overload

    Draws the first \a rectCount of the given \a rectangles using the
    current pen and brush.
*/
void QPainter::drawRects(const QRect *rects, int rectCount)
{
#ifdef QT_DEBUG_DRAW
    if (qt_show_painter_debug_output)
        printf("QPainter::drawRects(), count=%d\n", rectCount);
#endif
    if (!isActive() || rectCount <= 0)
        return;

    Q_D(QPainter);
    d->updateState(d->state);

    if (!d->state->emulationSpecifier) {
        d->engine->drawRects(rects, rectCount);
        return;
    }

    if (d->state->emulationSpecifier == QPaintEngine::PrimitiveTransform
        && d->state->txop == QTransform::TxTranslate) {
        for (int i=0; i<rectCount; ++i) {
            QRectF r(rects[i].x() + d->state->matrix.dx(),
                     rects[i].y() + d->state->matrix.dy(),
                     rects[i].width(),
                     rects[i].height());

            d->engine->drawRects(&r, 1);
        }
    } else {
        if (d->state->brushNeedsResolving() || d->state->penNeedsResolving()) {
            for (int i=0; i<rectCount; ++i) {
                QPainterPath rectPath;
                rectPath.addRect(rects[i]);
                d->draw_helper(rectPath, QPainterPrivate::StrokeAndFillDraw);
            }
        } else {
            QPainterPath rectPath;
            for (int i=0; i<rectCount; ++i)
                rectPath.addRect(rects[i]);

            d->draw_helper(rectPath, QPainterPrivate::StrokeAndFillDraw);
        }
    }
}

/*!
    \fn void QPainter::drawRects(const QVector<QRectF> &rectangles)
    \overload

    Draws the given \a rectangles using the current pen and brush.
*/

/*!
    \fn void QPainter::drawRects(const QVector<QRect> &rectangles)

    \overload

    Draws the given \a rectangles using the current pen and brush.
*/

/*!
  \fn void QPainter::drawPoint(const QPointF &position)

    Draws a single point at the given \a position using the current
    pen's color.

    \sa {The Coordinate System}
*/

/*!
    \fn void QPainter::drawPoint(const QPoint &position)
    \overload

    Draws a single point at the given \a position using the current
    pen's color.
*/

/*! \fn void QPainter::drawPoint(int x, int y)

    \overload

    Draws a single point at position (\a x, \a y).
*/

/*!
    Draws the first \a pointCount points in the array \a points using
    the current pen's color.

    \sa {The Coordinate System}
*/
void QPainter::drawPoints(const QPointF *points, int pointCount)
{
#ifdef QT_DEBUG_DRAW
    if (qt_show_painter_debug_output)
        printf("QPainter::drawPoints(), count=%d\n", pointCount);
#endif
    if (!isActive() || pointCount <= 0)
        return;
    Q_D(QPainter);
    d->updateState(d->state);

    if (!d->state->emulationSpecifier) {
        d->engine->drawPoints(points, pointCount);
        return;
    }

    if (d->state->emulationSpecifier == QPaintEngine::PrimitiveTransform
        && d->state->txop == QTransform::TxTranslate) {
        // ### use drawPoints function
        for (int i=0; i<pointCount; ++i) {
            QPointF pt(points[i].x() + d->state->matrix.dx(),
                       points[i].y() + d->state->matrix.dy());
            d->engine->drawPoints(&pt, 1);
        }
    } else {
        QPen pen = d->state->pen;
        bool flat_pen = pen.capStyle() == Qt::FlatCap;
        if (flat_pen) {
            save();
            pen.setCapStyle(Qt::SquareCap);
            setPen(pen);
        }
        QPainterPath path;
        for (int i=0; i<pointCount; ++i) {
            path.moveTo(points[i].x(), points[i].y());
            path.lineTo(points[i].x() + 0.0001, points[i].y());
        }
        d->draw_helper(path, QPainterPrivate::StrokeDraw);
        if (flat_pen)
            restore();
    }
}

/*!
    \overload

    Draws the first \a pointCount points in the array \a points using
    the current pen's color.
*/

void QPainter::drawPoints(const QPoint *points, int pointCount)
{
#ifdef QT_DEBUG_DRAW
    if (qt_show_painter_debug_output)
        printf("QPainter::drawPoints(), count=%d\n", pointCount);
#endif
    if (!isActive() || pointCount <= 0)
        return;
    Q_D(QPainter);
    d->updateState(d->state);

    if (!d->state->emulationSpecifier) {
        d->engine->drawPoints(points, pointCount);
        return;
    }

    if (d->state->emulationSpecifier == QPaintEngine::PrimitiveTransform
        && d->state->txop == QTransform::TxTranslate) {
        // ### use drawPoints function
        for (int i=0; i<pointCount; ++i) {
            QPointF pt(points[i].x() + d->state->matrix.dx(),
                       points[i].y() + d->state->matrix.dy());
            d->engine->drawPoints(&pt, 1);
        }
    } else {
        QPen pen = d->state->pen;
        bool flat_pen = (pen.capStyle() == Qt::FlatCap);
        if (flat_pen) {
            save();
            pen.setCapStyle(Qt::SquareCap);
            setPen(pen);
        }
        QPainterPath path;
        for (int i=0; i<pointCount; ++i) {
            path.moveTo(points[i].x(), points[i].y());
            path.lineTo(points[i].x() + 0.0001, points[i].y());
        }
        d->draw_helper(path, QPainterPrivate::StrokeDraw);
        if (flat_pen)
            restore();
    }
}

/*!
    \fn void QPainter::drawPoints(const QPolygonF &points)

    \overload

    Draws the points in the vector  \a points.
*/

/*!
    \fn void QPainter::drawPoints(const QPolygon &points)

    \overload

    Draws the points in the vector  \a points.
*/

/*!
    \fn void QPainter::drawPoints(const QPolygon &polygon, int index,
    int count)

    \overload
    \compat

    Draws \a count points in the vector \a polygon starting on \a index
    using the current pen.

    Use drawPoints() combined with QPolygon::constData() instead.

    \oldcode
        QPainter painter(this);
        painter.drawPoints(polygon, index, count);
    \newcode
        int pointCount = (count == -1) ?  polygon.size() - index : count;

        QPainter painter(this);
        painter.drawPoints(polygon.constData() + index, pointCount);
    \endcode
*/

/*!
    Sets the background mode of the painter to the given \a mode

    Qt::TransparentMode (the default) draws stippled lines and text
    without setting the background pixels.  Qt::OpaqueMode fills these
    space with the current background color.

    Note that in order to draw a bitmap or pixmap transparently, you
    must use QPixmap::setMask().

    \sa backgroundMode(), setBackground(),
    {QPainter#Settings}{Settings}
*/

void QPainter::setBackgroundMode(Qt::BGMode mode)
{
#ifdef QT_DEBUG_DRAW
    if (qt_show_painter_debug_output)
        printf("QPainter::setBackgroundMode(), mode=%d\n", mode);
#endif

    if (mode != Qt::TransparentMode && mode != Qt::OpaqueMode) {
        qWarning("QPainter::setBackgroundMode: Invalid mode");
        return;
    }
    Q_D(QPainter);
    d->state->bgMode = mode;
    d->state->dirtyFlags |= QPaintEngine::DirtyBackgroundMode;
}

/*!
    Returns the current background mode.

    \sa setBackgroundMode(), {QPainter#Settings}{Settings}
*/
Qt::BGMode QPainter::backgroundMode() const
{
    Q_D(const QPainter);
    return d->state->bgMode;
}


/*!
    \overload

    Sets the painter's pen to have style Qt::SolidLine, width 0 and the
    specified \a color.
*/

void QPainter::setPen(const QColor &color)
{
#ifdef QT_DEBUG_DRAW
    if (qt_show_painter_debug_output)
        printf("QPainter::setPen(), color=%04x\n", color.rgb());
#endif
    Q_D(QPainter);

    if (d->state->pen.style() == Qt::SolidLine
        && d->state->pen.widthF() == 0
        && d->state->pen.isSolid()
        && d->state->pen.color() == color)
        return;
    d->state->pen = QPen(color.isValid() ? color : QColor(Qt::black), 0, Qt::SolidLine);
    d->state->dirtyFlags |= QPaintEngine::DirtyPen;
}

/*!
    Sets the painter's pen to be the given \a pen.

    The \a pen defines how to draw lines and outlines, and it also
    defines the text color.

    \sa pen(), {QPainter#Settings}{Settings}
*/

void QPainter::setPen(const QPen &pen)
{

#ifdef QT_DEBUG_DRAW
    if (qt_show_painter_debug_output)
        printf("QPainter::setPen(), color=%04x, (brushStyle=%d) style=%d, cap=%d, join=%d\n",
           pen.color().rgb(), pen.brush().style(), pen.style(), pen.capStyle(), pen.joinStyle());
#endif
    Q_D(QPainter);

    // Do some checks to see if we are the same pen.
    Qt::PenStyle currentStyle = d->state->pen.style();
    if (currentStyle == pen.style() && currentStyle != Qt::CustomDashLine) {
        if (currentStyle == Qt::NoPen ||
            (d->state->pen.isSolid() && pen.isSolid()
             && d->state->pen.color() == pen.color()
             && d->state->pen.widthF() == pen.widthF()
             && d->state->pen.capStyle() == pen.capStyle()
             && d->state->pen.joinStyle() == pen.joinStyle()
             && d->state->pen.isCosmetic() == pen.isCosmetic()))
            return;
    }

    d->state->pen = pen;
    d->state->dirtyFlags |= QPaintEngine::DirtyPen;
}

/*!
    \overload

    Sets the painter's pen to have the given \a style, width 0 and
    black color.
*/

void QPainter::setPen(Qt::PenStyle style)
{
    Q_D(QPainter);

    if (d->state->pen.style() == style
        && (style == Qt::NoPen || (d->state->pen.widthF() == 0
                                   && d->state->pen.isSolid()
                                   && d->state->pen.color() == QColor(Qt::black))))
        return;
    d->state->pen = QPen(Qt::black, 0, style);
    d->state->dirtyFlags |= QPaintEngine::DirtyPen;

}

/*!
    Returns the painter's current pen.

    \sa setPen(), {QPainter#Settings}{Settings}
*/

const QPen &QPainter::pen() const
{
    Q_D(const QPainter);
    return d->state->pen;
}


/*!
    Sets the painter's brush to the given \a brush.

    The painter's brush defines how shapes are filled.

    \sa brush(), {QPainter#Settings}{Settings}
*/

void QPainter::setBrush(const QBrush &brush)
{
#ifdef QT_DEBUG_DRAW
    if (qt_show_painter_debug_output)
        printf("QPainter::setBrush(), color=%04x, style=%d\n", brush.color().rgb(), brush.style());
#endif
    Q_D(QPainter);

    Qt::BrushStyle currentStyle = d->state->brush.style();
    if (currentStyle == brush.style()) {
        if (currentStyle == Qt::NoBrush
            || (currentStyle == Qt::SolidPattern
                && d->state->brush.color() == brush.color()))
            return;
    }

    d->state->brush = brush;
    d->state->dirtyFlags |= QPaintEngine::DirtyBrush;
}


/*!
    \overload

    Sets the painter's brush to black color and the specified \a
    style.
*/

void QPainter::setBrush(Qt::BrushStyle style)
{
    Q_D(QPainter);
    if (d->state->brush.style() == style &&
        (style == Qt::NoBrush
         || (style == Qt::SolidPattern && d->state->brush.color() == QColor(0, 0, 0))))
        return;
    d->state->brush = QBrush(Qt::black, style);
    d->state->dirtyFlags |= QPaintEngine::DirtyBrush;
}

/*!
    Returns the painter's current brush.

    \sa QPainter::setBrush(), {QPainter#Settings}{Settings}
*/

const QBrush &QPainter::brush() const
{
    Q_D(const QPainter);
    return d->state->brush;
}

/*!
    \fn void QPainter::setBackground(const QBrush &brush)

    Sets the background brush of the painter to the given \a brush.

    The background brush is the brush that is filled in when drawing
    opaque text, stippled lines and bitmaps. The background brush has
    no effect in transparent background mode (which is the default).

    \sa background(), setBackgroundMode(),
    {QPainter#Settings}{Settings}
*/

void QPainter::setBackground(const QBrush &bg)
{
#ifdef QT_DEBUG_DRAW
    if (qt_show_painter_debug_output)
        printf("QPainter::setBackground(), color=%04x, style=%d\n", bg.color().rgb(), bg.style());
#endif

    Q_D(QPainter);
    d->state->bgBrush = bg;
    d->state->dirtyFlags |= QPaintEngine::DirtyBackground;
}

/*!
    Sets the painter's font to the given \a font.

    This font is used by subsequent drawText() functions. The text
    color is the same as the pen color.

    If you set a font that isn't available, Qt finds a close match.
    font() will return what you set using setFont() and fontInfo() returns the
    font actually being used (which may be the same).

    \sa font(), drawText(), {QPainter#Settings}{Settings}
*/

void QPainter::setFont(const QFont &font)
{
    if (!isActive()) {
        qWarning("QPainter::setFont: Painter not active");
        return;
    }

#ifdef QT_DEBUG_DRAW
    if (qt_show_painter_debug_output)
        printf("QPainter::setFont(), family=%s, pointSize=%d\n", font.family().toLatin1().constData(), font.pointSize());
#endif

    Q_D(QPainter);
    d->state->font = QFont(font.resolve(d->state->deviceFont), d->device);
    d->state->dirtyFlags |= QPaintEngine::DirtyFont;
}

/*!
    Returns the currently set font used for drawing text.

    \sa setFont(), drawText(), {QPainter#Settings}{Settings}
*/
const QFont &QPainter::font() const
{
    Q_D(const QPainter);
    return d->state->font;
}

/*!
    \since 4.4

    Draws the given rectangle \a rect with rounded corners.

    The \a xRadius and \a yRadius arguments specify the radii
    of the ellipses defining the corners of the rounded rectangle.
    When \a mode is Qt::RelativeSize, \a xRadius and
    \a yRadius are specified in percentage of half the rectangle's
    width and height respectively, and should be in the range
    0.0 to 100.0.

    A filled rectangle has a size of rect.size(). A stroked rectangle
    has a size of rect.size() plus the pen width.

    \table 100%
    \row
    \o \inlineimage qpainter-roundrect.png
    \o
    \snippet doc/src/snippets/code/src.gui.painting.qpainter.cpp 8
    \endtable

    \sa drawRect(), QPen
*/
void QPainter::drawRoundedRect(const QRectF &rect, qreal xRadius, qreal yRadius, Qt::SizeMode mode)
{
#ifdef QT_DEBUG_DRAW
    if (qt_show_painter_debug_output)
        printf("QPainter::drawRoundedRect(), [%.2f,%.2f,%.2f,%.2f]\n", rect.x(), rect.y(), rect.width(), rect.height());
#endif

    if (!isActive())
        return;

    if (xRadius <= 0 || yRadius <= 0) {             // draw normal rectangle
        drawRect(rect);
        return;
    }

    QPainterPath path;
    path.addRoundedRect(rect, xRadius, yRadius, mode);
    drawPath(path);
}

/*!
    \fn void QPainter::drawRoundedRect(const QRect &rect, qreal xRadius, qreal yRadius,
                                       Qt::SizeMode mode = Qt::AbsoluteSize);
    \since 4.4
    \overload

    Draws the given rectangle \a rect with rounded corners.
*/

/*!
    \fn void QPainter::drawRoundedRect(int x, int y, int w, int h, qreal xRadius, qreal yRadius,
                                       Qt::SizeMode mode = Qt::AbsoluteSize);
    \since 4.4
    \overload

    Draws the given rectangle \a x, \a y, \a w, \a h with rounded corners.
*/

/*!
    \obsolete

    Draws a rectangle \a r with rounded corners.

    The \a xRnd and \a yRnd arguments specify how rounded the corners
    should be. 0 is angled corners, 99 is maximum roundedness.

    A filled rectangle has a size of r.size(). A stroked rectangle
    has a size of r.size() plus the pen width.

    \sa drawRoundedRect()
*/
void QPainter::drawRoundRect(const QRectF &r, int xRnd, int yRnd)
{
#ifdef QT_DEBUG_DRAW
    if (qt_show_painter_debug_output)
        printf("QPainter::drawRoundRectangle(), [%.2f,%.2f,%.2f,%.2f]\n", r.x(), r.y(), r.width(), r.height());
#endif

    if (!isActive())
        return;

    if(xRnd <= 0 || yRnd <= 0) {             // draw normal rectangle
        drawRect(r);
        return;
    }

    QPainterPath path;
    path.addRoundRect(r, xRnd, yRnd);
    drawPath(path);
}


/*!
    \fn void QPainter::drawRoundRect(const QRect &r, int xRnd = 25, int yRnd = 25)

    \overload
    \obsolete

    Draws the rectangle \a r with rounded corners.
*/

/*!
    \obsolete

    \fn QPainter::drawRoundRect(int x, int y, int w, int h, int xRnd, int yRnd)

    \overload

    Draws the rectangle \a x, \a y, \a w, \a h with rounded corners.
*/

/*!
    \fn void QPainter::drawEllipse(const QRectF &rectangle)

    Draws the ellipse defined by the given \a rectangle.

    A filled ellipse has a size of \a{rectangle}.\l
    {QRect::size()}{size()}. A stroked ellipse has a size of
    \a{rectangle}.\l {QRect::size()}{size()} plus the pen width.

    \table 100%
    \row
    \o \inlineimage qpainter-ellipse.png
    \o
    \snippet doc/src/snippets/code/src.gui.painting.qpainter.cpp 9
    \endtable

    \sa drawPie(), {The Coordinate System}
*/
void QPainter::drawEllipse(const QRectF &r)
{
#ifdef QT_DEBUG_DRAW
    if (qt_show_painter_debug_output)
        printf("QPainter::drawEllipse(), [%.2f,%.2f,%.2f,%.2f]\n", r.x(), r.y(), r.width(), r.height());
#endif

    if (!isActive())
        return;
    Q_D(QPainter);
    d->updateState(d->state);

    QRectF rect(r.normalized());

    if (rect.isEmpty())
        return;

    if (d->state->emulationSpecifier) {
        if (d->state->emulationSpecifier == QPaintEngine::PrimitiveTransform
            && d->state->txop == QTransform::TxTranslate) {
            rect.translate(QPointF(d->state->matrix.dx(), d->state->matrix.dy()));
        } else {
            QPainterPath path;
            path.addEllipse(rect);
            d->draw_helper(path, QPainterPrivate::StrokeAndFillDraw);
            return;
        }
    }

    d->engine->drawEllipse(rect);
}

/*!
    \fn QPainter::drawEllipse(const QRect &rectangle)

    \overload

    Draws the ellipse defined by the given \a rectangle.
*/
void QPainter::drawEllipse(const QRect &r)
{
#ifdef QT_DEBUG_DRAW
    if (qt_show_painter_debug_output)
        printf("QPainter::drawEllipse(), [%d,%d,%d,%d]\n", r.x(), r.y(), r.width(), r.height());
#endif

    if (!isActive())
        return;
    Q_D(QPainter);
    d->updateState(d->state);

    QRect rect(r.normalized());

    if (rect.isEmpty())
        return;

    if (d->state->emulationSpecifier) {
        if (d->state->emulationSpecifier == QPaintEngine::PrimitiveTransform
            && d->state->txop == QTransform::TxTranslate) {
            rect.translate(QPoint(qRound(d->state->matrix.dx()), qRound(d->state->matrix.dy())));
        } else {
            QPainterPath path;
            path.addEllipse(rect);
            d->draw_helper(path, QPainterPrivate::StrokeAndFillDraw);
            return;
        }
    }

    d->engine->drawEllipse(rect);
}

/*!
    \fn QPainter::drawEllipse(int x, int y, int width, int height)

    \overload

    Draws the ellipse defined by the rectangle beginning at (\a{x},
    \a{y}) with the given \a width and \a height.
*/

/*!
    \since 4.4

    \fn QPainter::drawEllipse(const QPointF &center, qreal rx, qreal ry)

    \overload

    Draws the ellipse positioned at \a{center} with radii \a{rx} and \a{ry}.
*/

/*!
    \since 4.4

    \fn QPainter::drawEllipse(const QPoint &center, int rx, int ry)

    \overload

    Draws the ellipse positioned at \a{center} with radii \a{rx} and \a{ry}.
*/

/*!
    \fn void QPainter::drawArc(const QRectF &rectangle, int startAngle, int spanAngle)

    Draws the arc defined by the given \a rectangle, \a startAngle and
    \a spanAngle.

    The \a startAngle and \a spanAngle must be specified in 1/16th of
    a degree, i.e. a full circle equals 5760 (16 * 360). Positive
    values for the angles mean counter-clockwise while negative values
    mean the clockwise direction. Zero degrees is at the 3 o'clock
    position.

    \table 100%
    \row
    \o \inlineimage qpainter-arc.png
    \o
    \snippet doc/src/snippets/code/src.gui.painting.qpainter.cpp 10
    \endtable

    \sa drawPie(), drawChord(), {The Coordinate System}
*/

void QPainter::drawArc(const QRectF &r, int a, int alen)
{
#ifdef QT_DEBUG_DRAW
    if (qt_show_painter_debug_output)
        printf("QPainter::drawArc(), [%.2f,%.2f,%.2f,%.2f], angle=%d, sweep=%d\n",
           r.x(), r.y(), r.width(), r.height(), a/16, alen/16);
#endif

    if (!isActive())
        return;
    Q_D(QPainter);
    d->updateState(d->state);

    QRectF rect = r.normalized();

    QPainterPath path;
    path.arcMoveTo(rect, a/16.0);
    path.arcTo(rect, a/16.0, alen/16.0);
    strokePath(path, d->state->pen);
}

/*! \fn void QPainter::drawArc(const QRect &rectangle, int startAngle,
                               int spanAngle)

    \overload

    Draws the arc defined by the given \a rectangle, \a startAngle and
    \a spanAngle.
*/

/*!
    \fn void QPainter::drawArc(int x, int y, int width, int height,
                               int startAngle, int spanAngle)

    \overload

    Draws the arc defined by the rectangle beginning at (\a x, \a y)
    with the specified \a width and \a height, and the given \a
    startAngle and \a spanAngle.
*/

/*!
    \fn void QPainter::drawPie(const QRectF &rectangle, int startAngle, int spanAngle)

    Draws a pie defined by the given \a rectangle, \a startAngle and
    and \a spanAngle.

    The pie is filled with the current brush().

    The startAngle and spanAngle must be specified in 1/16th of a
    degree, i.e. a full circle equals 5760 (16 * 360). Positive values
    for the angles mean counter-clockwise while negative values mean
    the clockwise direction. Zero degrees is at the 3 o'clock
    position.

    \table 100%
    \row
    \o \inlineimage qpainter-pie.png
    \o
    \snippet doc/src/snippets/code/src.gui.painting.qpainter.cpp 11
    \endtable

    \sa drawEllipse(), drawChord(), {The Coordinate System}
*/
void QPainter::drawPie(const QRectF &r, int a, int alen)
{
#ifdef QT_DEBUG_DRAW
    if (qt_show_painter_debug_output)
        printf("QPainter::drawPie(), [%.2f,%.2f,%.2f,%.2f], angle=%d, sweep=%d\n",
           r.x(), r.y(), r.width(), r.height(), a/16, alen/16);
#endif

    if (!isActive())
        return;
    Q_D(QPainter);
    d->updateState(d->state);

    if (a > (360*16)) {
        a = a % (360*16);
    } else if (a < 0) {
        a = a % (360*16);
        if (a < 0) a += (360*16);
    }

    QRectF rect = r.normalized();

    QPainterPath path;
    path.moveTo(rect.center());
    path.arcTo(rect.x(), rect.y(), rect.width(), rect.height(), a/16.0, alen/16.0);
    path.closeSubpath();
    drawPath(path);

}

/*!
    \fn void QPainter::drawPie(const QRect &rectangle, int startAngle, int spanAngle)
    \overload

    Draws a pie defined by the given \a rectangle, \a startAngle and
    and \a spanAngle.
*/

/*!
    \fn void QPainter::drawPie(int x, int y, int width, int height, int
    startAngle, int spanAngle)

    \overload

    Draws the pie defined by the rectangle beginning at (\a x, \a y) with
    the specified \a width and \a height, and the given \a startAngle and
    \a spanAngle.
*/

/*!
    \fn void QPainter::drawChord(const QRectF &rectangle, int startAngle, int spanAngle)

    Draws the chord defined by the given \a rectangle, \a startAngle and
    \a spanAngle.  The chord is filled with the current brush().

    The startAngle and spanAngle must be specified in 1/16th of a
    degree, i.e. a full circle equals 5760 (16 * 360). Positive values
    for the angles mean counter-clockwise while negative values mean
    the clockwise direction. Zero degrees is at the 3 o'clock
    position.

    \table 100%
    \row
    \o \inlineimage qpainter-chord.png
    \o
    \snippet doc/src/snippets/code/src.gui.painting.qpainter.cpp 12
    \endtable

    \sa drawArc(), drawPie(), {The Coordinate System}
*/
void QPainter::drawChord(const QRectF &r, int a, int alen)
{
#ifdef QT_DEBUG_DRAW
    if (qt_show_painter_debug_output)
        printf("QPainter::drawChord(), [%.2f,%.2f,%.2f,%.2f], angle=%d, sweep=%d\n",
           r.x(), r.y(), r.width(), r.height(), a/16, alen/16);
#endif

    if (!isActive())
        return;
    Q_D(QPainter);
    d->updateState(d->state);

    QRectF rect = r.normalized();

    QPainterPath path;
    path.arcMoveTo(rect, a/16.0);
    path.arcTo(rect, a/16.0, alen/16.0);
    path.closeSubpath();
    drawPath(path);
}
/*!
    \fn void QPainter::drawChord(const QRect &rectangle, int startAngle, int spanAngle)

    \overload

    Draws the chord defined by the given \a rectangle, \a startAngle and
    \a spanAngle.
*/

/*!
    \fn void QPainter::drawChord(int x, int y, int width, int height, int
    startAngle, int spanAngle)

    \overload

   Draws the chord defined by the rectangle beginning at (\a x, \a y)
   with the specified \a width and \a height, and the given \a
   startAngle and \a spanAngle.
*/

#ifdef QT3_SUPPORT
/*!
    \fn void QPainter::drawLineSegments(const QPolygon &polygon, int
    index, int count)

    Draws \a count separate lines from points defined by the \a
    polygon, starting at \a{polygon}\e{[index]} (\a index defaults to
    0). If \a count is -1 (the default) all points until the end of
    the array are used.

    Use drawLines() combined with QPolygon::constData() instead.

    \oldcode
        QPainter painter(this);
        painter.drawLineSegments(polygon, index, count);
    \newcode
        int lineCount = (count == -1) ?  (polygon.size() - index) / 2  : count;

        QPainter painter(this);
        painter.drawLines(polygon.constData() + index * 2, lineCount);
    \endcode
*/

void QPainter::drawLineSegments(const QPolygon &a, int index, int nlines)
{
#ifdef QT_DEBUG_DRAW
    if (qt_show_painter_debug_output)
        printf("QPainter::drawLineSegments(), count=%d\n", a.size()/2);
#endif

    if (!isActive())
        return;

    if (nlines < 0)
        nlines = a.size()/2 - index/2;
    if (index + nlines*2 > (int)a.size())
        nlines = (a.size() - index)/2;
    if (nlines < 1 || index < 0)
        return;

    Q_D(QPainter);
    d->updateState(d->state);

    QVector<QLineF> lines;
    if (d->state->emulationSpecifier) {
        if (d->state->emulationSpecifier == QPaintEngine::PrimitiveTransform
            && d->state->txop == QTransform::TxTranslate) {
            QPointF offset(d->state->matrix.dx(), d->state->matrix.dy());
            for (int i=index; i<index + nlines*2; i+=2)
                lines << QLineF(a.at(i) + offset, a.at(i+1) + offset);
        } else {
            QPainterPath linesPath;
            for (int i=index; i<index + nlines*2; i+=2) {
                linesPath.moveTo(a.at(i));
                linesPath.lineTo(a.at(i+1));
            }
            d->draw_helper(linesPath, QPainterPrivate::StrokeDraw);
            return;
        }
    } else {
        for (int i=index; i<index + nlines*2; i+=2)
            lines << QLineF(a.at(i), a.at(i+1));
    }

    d->engine->drawLines(lines.data(), lines.size());
}
#endif // QT3_SUPPORT

/*!
    Draws the first \a lineCount lines in the array \a lines
    using the current pen.

    \sa drawLine(), drawPolyline()
*/
void QPainter::drawLines(const QLineF *lines, int lineCount)
{
#ifdef QT_DEBUG_DRAW
    if (qt_show_painter_debug_output)
        printf("QPainter::drawLines(), line count=%d\n", lineCount);
#endif

    Q_ASSERT_X(lines, "QPainter::drawLines", "lines array cannot be 0");

    if (!isActive() || lineCount < 1)
        return;

    Q_D(QPainter);
    d->updateState(d->state);

    uint lineEmulation = line_emulation(d->state->emulationSpecifier);

    if (lineEmulation) {
        if (lineEmulation == QPaintEngine::PrimitiveTransform
            && d->state->txop == QTransform::TxTranslate) {
            for (int i = 0; i < lineCount; ++i) {
                QLineF line = lines[i];
                line.translate(d->state->matrix.dx(), d->state->matrix.dy());
                d->engine->drawLines(&line, 1);
            }
        } else {
            QPainterPath linePath;
            for (int i = 0; i < lineCount; ++i) {
                linePath.moveTo(lines[i].p1());
                linePath.lineTo(lines[i].p2());
            }
            d->draw_helper(linePath, QPainterPrivate::StrokeDraw);
        }
        return;
    }
    d->engine->drawLines(lines, lineCount);
}

/*!
    \fn void QPainter::drawLines(const QLine *lines, int lineCount)
    \overload

    Draws the first \a lineCount lines in the array \a lines
    using the current pen.
*/
void QPainter::drawLines(const QLine *lines, int lineCount)
{
#ifdef QT_DEBUG_DRAW
    if (qt_show_painter_debug_output)
        printf("QPainter::drawLine(), line count=%d\n", lineCount);
#endif

    Q_ASSERT_X(lines, "QPainter::drawLines", "lines array cannot be 0");

    if (!isActive() || lineCount < 1)
        return;

    Q_D(QPainter);
    d->updateState(d->state);

    uint lineEmulation = line_emulation(d->state->emulationSpecifier);

    if (lineEmulation) {
        if (lineEmulation == QPaintEngine::PrimitiveTransform
            && d->state->txop == QTransform::TxTranslate) {
            for (int i = 0; i < lineCount; ++i) {
                QLineF line = lines[i];
                line.translate(d->state->matrix.dx(), d->state->matrix.dy());
                d->engine->drawLines(&line, 1);
            }
        } else {
            QPainterPath linePath;
            for (int i = 0; i < lineCount; ++i) {
                linePath.moveTo(lines[i].p1());
                linePath.lineTo(lines[i].p2());
            }
            d->draw_helper(linePath, QPainterPrivate::StrokeDraw);
        }
        return;
    }
    d->engine->drawLines(lines, lineCount);
}

/*!
    \overload

    Draws the first \a lineCount lines in the array \a pointPairs
    using the current pen.  The lines are specified as pairs of points
    so the number of entries in \a pointPairs must be at least \a
    lineCount * 2.
*/
void QPainter::drawLines(const QPointF *pointPairs, int lineCount)
{
    Q_ASSERT_X(pointPairs, "QPainter::drawLines", "pointPairs array cannot be 0");
    Q_ASSERT(sizeof(QLineF) == 2*sizeof(QPointF));

    drawLines((QLineF*)pointPairs, lineCount);
}

/*!
    \overload

    Draws the first \a lineCount lines in the array \a pointPairs
    using the current pen.
*/
void QPainter::drawLines(const QPoint *pointPairs, int lineCount)
{
    Q_ASSERT_X(pointPairs, "QPainter::drawLines", "pointPairs array cannot be 0");
    Q_ASSERT(sizeof(QLine) == 2*sizeof(QPoint));

    drawLines((QLine*)pointPairs, lineCount);
}


/*!
    \fn void QPainter::drawLines(const QVector<QPointF> &pointPairs)
    \overload

    Draws a line for each pair of points in the vector \a pointPairs
    using the current pen. If there is an odd number of points in the
    array, the last point will be ignored.
*/

/*!
    \fn void QPainter::drawLines(const QVector<QPoint> &pointPairs)
    \overload

    Draws a line for each pair of points in the vector \a pointPairs
    using the current pen.
*/

/*!
    \fn void QPainter::drawLines(const QVector<QLineF> &lines)
    \overload

    Draws the set of lines defined by the list \a lines using the
    current pen and brush.
*/

/*!
    \fn void QPainter::drawLines(const QVector<QLine> &lines)
    \overload

    Draws the set of lines defined by the list \a lines using the
    current pen and brush.
*/

/*!
    Draws the polyline defined by the first \a pointCount points in \a
    points using the current pen.

    Note that unlike the drawPolygon() function the last point is \e
    not connected to the first, neither is the polyline filled.

    \table 100%
    \row
    \o
    \snippet doc/src/snippets/code/src.gui.painting.qpainter.cpp 13
    \endtable

    \sa drawLines(), drawPolygon(), {The Coordinate System}
*/
void QPainter::drawPolyline(const QPointF *points, int pointCount)
{
#ifdef QT_DEBUG_DRAW
    if (qt_show_painter_debug_output)
        printf("QPainter::drawPolyline(), count=%d\n", pointCount);
#endif

    if (!isActive() || pointCount < 2)
        return;

    Q_D(QPainter);
    d->updateState(d->state);

    uint lineEmulation = line_emulation(d->state->emulationSpecifier);

    if (lineEmulation) {
        // ###
//         if (lineEmulation == QPaintEngine::PrimitiveTransform
//             && d->state->txop == QTransform::TxTranslate) {
//         } else {
        QPainterPath polylinePath(points[0]);
        for (int i=1; i<pointCount; ++i)
            polylinePath.lineTo(points[i]);
        d->draw_helper(polylinePath, QPainterPrivate::StrokeDraw);
//         }
    } else {
        d->engine->drawPolygon(points, pointCount, QPaintEngine::PolylineMode);
    }
}

/*!
    \overload

    Draws the polyline defined by the first \a pointCount points in \a
    points using the current pen.
 */
void QPainter::drawPolyline(const QPoint *points, int pointCount)
{
#ifdef QT_DEBUG_DRAW
    if (qt_show_painter_debug_output)
        printf("QPainter::drawPolyline(), count=%d\n", pointCount);
#endif

    if (!isActive() || pointCount < 2)
        return;

    Q_D(QPainter);
    d->updateState(d->state);

    uint lineEmulation = line_emulation(d->state->emulationSpecifier);

    if (lineEmulation) {
        // ###
//         if (lineEmulation == QPaintEngine::PrimitiveTransform
//             && d->state->txop == QTransform::TxTranslate) {
//         } else {
        QPainterPath polylinePath(points[0]);
        for (int i=1; i<pointCount; ++i)
            polylinePath.lineTo(points[i]);
        d->draw_helper(polylinePath, QPainterPrivate::StrokeDraw);
//         }
    } else {
        d->engine->drawPolygon(points, pointCount, QPaintEngine::PolylineMode);
    }
}

/*!
    \fn void QPainter::drawPolyline(const QPolygon &polygon, int index, int
    count)

    \overload
    \compat

    Draws the polyline defined by the \a count lines of the given \a
    polygon starting at \a index (\a index defaults to 0).

    Use drawPolyline() combined with QPolygon::constData() instead.

    \oldcode
        QPainter painter(this);
        painter.drawPolyline(polygon, index, count);
    \newcode
        int pointCount = (count == -1) ?  polygon.size() - index : count;

        QPainter painter(this);
        painter.drawPolyline(polygon.constData() + index, pointCount);
    \endcode
*/

/*!
    \fn void QPainter::drawPolyline(const QPolygonF &points)

    \overload

    Draws the polyline defined by the given \a points using the
    current pen.
*/

/*!
    \fn void QPainter::drawPolyline(const QPolygon &points)

    \overload

    Draws the polyline defined by the given \a points using the
    current pen.
*/

/*!
    Draws the polygon defined by the first \a pointCount points in the
    array \a points using the current pen and brush.

    \table 100%
    \row
    \o \inlineimage qpainter-polygon.png
    \o
    \snippet doc/src/snippets/code/src.gui.painting.qpainter.cpp 14
    \endtable

    The first point is implicitly connected to the last point, and the
    polygon is filled with the current brush().

    If \a fillRule is Qt::WindingFill, the polygon is filled using the
    winding fill algorithm.  If \a fillRule is Qt::OddEvenFill, the
    polygon is filled using the odd-even fill algorithm. See
    \l{Qt::FillRule} for a more detailed description of these fill
    rules.

    \sa  drawConvexPolygon(), drawPolyline(), {The Coordinate System}
*/
void QPainter::drawPolygon(const QPointF *points, int pointCount, Qt::FillRule fillRule)
{
#ifdef QT_DEBUG_DRAW
    if (qt_show_painter_debug_output)
        printf("QPainter::drawPolygon(), count=%d\n", pointCount);
#endif

    if (!isActive() || pointCount < 2)
        return;

    Q_D(QPainter);
    d->updateState(d->state);

    uint emulationSpecifier = d->state->emulationSpecifier;

    if (emulationSpecifier) {
        QPainterPath polygonPath(points[0]);
        for (int i=1; i<pointCount; ++i)
            polygonPath.lineTo(points[i]);
        polygonPath.closeSubpath();
        polygonPath.setFillRule(fillRule);
        d->draw_helper(polygonPath);
        return;
    }

    d->engine->drawPolygon(points, pointCount, QPaintEngine::PolygonDrawMode(fillRule));
}

/*! \overload

    Draws the polygon defined by the first \a pointCount points in the
    array \a points.
*/
void QPainter::drawPolygon(const QPoint *points, int pointCount, Qt::FillRule fillRule)
{
#ifdef QT_DEBUG_DRAW
    if (qt_show_painter_debug_output)
        printf("QPainter::drawPolygon(), count=%d\n", pointCount);
#endif

    if (!isActive() || pointCount < 2)
        return;

    Q_D(QPainter);
    d->updateState(d->state);

    uint emulationSpecifier = d->state->emulationSpecifier;

    if (emulationSpecifier) {
        QPainterPath polygonPath(points[0]);
        for (int i=1; i<pointCount; ++i)
            polygonPath.lineTo(points[i]);
        polygonPath.closeSubpath();
        polygonPath.setFillRule(fillRule);
        d->draw_helper(polygonPath);
        return;
    }

    d->engine->drawPolygon(points, pointCount, QPaintEngine::PolygonDrawMode(fillRule));
}

/*! \fn void QPainter::drawPolygon(const QPolygonF &polygon, bool winding, int index = 0,
                                   int count = -1)
    \compat
    \overload

    Use drawPolygon() combined with QPolygonF::constData() instead.

    \oldcode
        QPainter painter(this);
        painter.drawPolygon(polygon, winding, index, count);
    \newcode
        int pointCount = (count == -1) ?  polygon.size() - index : count;
        int fillRule = winding ? Qt::WindingFill : Qt::OddEvenFill;

        QPainter painter(this);
        painter.drawPolygon( polygon.constData() + index, pointCount, fillRule);
    \endcode
*/

/*! \fn void QPainter::drawPolygon(const QPolygon &polygon, bool winding,
                                   int index = 0, int count = -1)

    \compat
    \overload

    Use drawPolygon() combined with QPolygon::constData() instead.

    \oldcode
        QPainter painter(this);
        painter.drawPolygon(polygon, winding, index, count);
    \newcode
        int pointCount = (count == -1) ?  polygon.size() - index : count;
        int fillRule = winding ? Qt::WindingFill : Qt::OddEvenFill;

        QPainter painter(this);
        painter.drawPolygon( polygon.constData() + index, pointCount, fillRule);
    \endcode
*/

/*! \fn void QPainter::drawPolygon(const QPolygonF &points, Qt::FillRule fillRule)

    \overload

    Draws the polygon defined by the given \a points using the fill
    rule \a fillRule.
*/

/*! \fn void QPainter::drawPolygon(const QPolygon &points, Qt::FillRule fillRule)

    \overload

    Draws the polygon defined by the given \a points using the fill
    rule \a fillRule.
*/

/*!
    \fn void QPainter::drawConvexPolygon(const QPointF *points, int pointCount)

    Draws the convex polygon defined by the first \a pointCount points
    in the array \a points using the current pen.

    \table 100%
    \row
    \o \inlineimage qpainter-polygon.png
    \o
    \snippet doc/src/snippets/code/src.gui.painting.qpainter.cpp 15
    \endtable

    The first point is implicitly connected to the last point, and the
    polygon is filled with the current brush().  If the supplied
    polygon is not convex, i.e. it contains at least one angle larger
    than 180 degrees, the results are undefined.

    On some platforms (e.g. X11), the drawConvexPolygon() function can
    be faster than the drawPolygon() function.

    \sa drawPolygon(), drawPolyline(), {The Coordinate System}
*/

/*!
    \fn void QPainter::drawConvexPolygon(const QPoint *points, int pointCount)
    \overload

    Draws the convex polygon defined by the first \a pointCount points
    in the array \a points using the current pen.
*/

/*!
    \fn void QPainter::drawConvexPolygon(const QPolygonF &polygon)

    \overload

    Draws the convex polygon defined by \a polygon using the current
    pen and brush.
*/

/*!
    \fn void QPainter::drawConvexPolygon(const QPolygon &polygon)
    \overload

    Draws the convex polygon defined by \a polygon using the current
    pen and brush.
*/

/*!
    \fn void QPainter::drawConvexPolygon(const QPolygonF &polygon, int
    index, int count)

    \compat
    \overload

    Use drawConvexPolygon() combined with QPolygonF::constData()
    instead.

    \oldcode
        QPainter painter(this);
        painter.drawConvexPolygon(polygon, index, count);
    \newcode
        int pointCount = (count == -1) ?  polygon.size() - index : count;

        QPainter painter(this);
        painter.drawConvexPolygon(polygon.constData() + index, pointCount);
    \endcode
*/

/*!
    \fn void QPainter::drawConvexPolygon(const QPolygon &polygon, int
    index, int count)

    \compat
    \overload

    Use drawConvexPolygon() combined with QPolygon::constData()
    instead.

    \oldcode
        QPainter painter(this);
        painter.drawConvexPolygon(polygon, index, count);
    \newcode
        int pointCount = (count == -1) ?  polygon.size() - index : count;

        QPainter painter(this);
        painter.drawConvexPolygon(polygon.constData() + index, pointCount);
    \endcode
*/

void QPainter::drawConvexPolygon(const QPoint *points, int pointCount)
{
#ifdef QT_DEBUG_DRAW
    if (qt_show_painter_debug_output)
        printf("QPainter::drawConvexPolygon(), count=%d\n", pointCount);
#endif

    if (!isActive() || pointCount < 2)
        return;

    Q_D(QPainter);
    d->updateState(d->state);

    uint emulationSpecifier = d->state->emulationSpecifier;

    if (emulationSpecifier) {
        QPainterPath polygonPath(points[0]);
        for (int i=1; i<pointCount; ++i)
            polygonPath.lineTo(points[i]);
        polygonPath.closeSubpath();
        polygonPath.setFillRule(Qt::WindingFill);
        d->draw_helper(polygonPath);
        return;
    }

    d->engine->drawPolygon(points, pointCount, QPaintEngine::ConvexMode);
}

void QPainter::drawConvexPolygon(const QPointF *points, int pointCount)
{
#ifdef QT_DEBUG_DRAW
    if (qt_show_painter_debug_output)
        printf("QPainter::drawConvexPolygon(), count=%d\n", pointCount);
#endif

    if (!isActive() || pointCount < 2)
        return;

    Q_D(QPainter);
    d->updateState(d->state);

    uint emulationSpecifier = d->state->emulationSpecifier;

    if (emulationSpecifier) {
        QPainterPath polygonPath(points[0]);
        for (int i=1; i<pointCount; ++i)
            polygonPath.lineTo(points[i]);
        polygonPath.closeSubpath();
        polygonPath.setFillRule(Qt::WindingFill);
        d->draw_helper(polygonPath);
        return;
    }

    d->engine->drawPolygon(points, pointCount, QPaintEngine::ConvexMode);
}

/*!
    \fn void QPainter::drawPixmap(const QRectF &target, const QPixmap &pixmap, const QRectF &source)

    Draws the rectangular portion \a source of the given \a pixmap
    into the given \a target in the paint device.

    \note The pixmap is scaled to fit the rectangle, if both the pixmap and rectangle size disagree.

    \table 100%
    \row
    \o
    \snippet doc/src/snippets/code/src.gui.painting.qpainter.cpp 16
    \endtable

    If \a pixmap is a QBitmap it is drawn with the bits that are "set"
    using the pens color. If backgroundMode is Qt::OpaqueMode, the
    "unset" bits are drawn using the color of the background brush; if
    backgroundMode is Qt::TransparentMode, the "unset" bits are
    transparent. Drawing bitmaps with gradient or texture colors is
    not supported.

    \sa drawImage()
*/
void QPainter::drawPixmap(const QRectF &r, const QPixmap &pm, const QRectF &sr)
{
#if defined QT_DEBUG_DRAW
    if (qt_show_painter_debug_output)
        printf("QPainter::drawPixmap(), target=[%.2f,%.2f,%.2f,%.2f], pix=[%d,%d], source=[%.2f,%.2f,%.2f,%.2f]\n",
               r.x(), r.y(), r.width(), r.height(),
               pm.width(), pm.height(),
               sr.x(), sr.y(), sr.width(), sr.height());
#endif

    Q_D(QPainter);
    if (!isActive() || pm.isNull())
        return;
    if (!qt_painter_thread_test(d->device->devType(), "drawPixmap()"))
        return;

    qreal x = r.x();
    qreal y = r.y();
    qreal w = r.width();
    qreal h = r.height();
    qreal sx = sr.x();
    qreal sy = sr.y();
    qreal sw = sr.width();
    qreal sh = sr.height();

    // Sanity-check clipping
    if (sw <= 0)
        sw = pm.width() - sx;

    if (sh <= 0)
        sh = pm.height() - sy;

    if (sx < 0) {
        qreal w_ratio = sx * w/sw;
        x -= w_ratio;
        w += w_ratio;
        sw += sx;
        sx = 0;
    }

    if (sy < 0) {
        qreal h_ratio = sy * h/sh;
        y -= h_ratio;
        h += h_ratio;
        sh += sy;
        sy = 0;
    }

    if (sw + sx > pm.width()) {
        qreal delta = sw - (pm.width() - sx);
        qreal w_ratio = delta * w/sw;
        sw -= delta;
        w -= w_ratio;
    }

    if (sh + sy > pm.height()) {
        qreal delta = sh - (pm.height() - sy);
        qreal h_ratio = delta * h/sh;
        sh -= delta;
        h -= h_ratio;
    }


    if (w < 0)
        w = sw;
    if (h < 0)
        h = sh;

    if (w == 0 || h == 0 || sw <= 0 || sh <= 0)
        return;

    // Emulate opaque background for bitmaps
    if (d->state->bgMode == Qt::OpaqueMode && pm.isQBitmap()) {
        fillRect(QRectF(x, y, w, h), d->state->bgBrush.color());
    }

    d->updateState(d->state);

    if ((d->state->txop > QTransform::TxTranslate
         && !d->engine->hasFeature(QPaintEngine::PixmapTransform))
        || (!d->state->matrix.isAffine() && !d->engine->hasFeature(QPaintEngine::PerspectiveTransform))
        || (d->state->opacity != 1.0 && !d->engine->hasFeature(QPaintEngine::ConstantOpacity))
        || ((sw != w || sh != h) && !d->engine->hasFeature(QPaintEngine::PixmapTransform)))
    {
        save();
        // If there is no scaling or transformation involved we have to make sure we use the
        // antialiased and not the aliased coordinate system by rounding the coordinates.
        if (d->state->txop <= QTransform::TxTranslate && sw == w && sh == h) {
            x = qRound(x + d->state->matrix.dx()) - d->state->matrix.dx();
            y = qRound(y + d->state->matrix.dy()) - d->state->matrix.dy();
            sx = qRound(sx);
            sy = qRound(sy);
            sw = qRound(sw);
            sh = qRound(sh);
        }
        translate(x, y);
        scale(w / sw, h / sh);
        setBackgroundMode(Qt::TransparentMode);
        setRenderHint(Antialiasing, renderHints() & SmoothPixmapTransform);
        QBrush brush(d->state->pen.color(), pm);
        brush.d->sourceRect = QRectF(sx, sy, sw, sh).toAlignedRect();
        setBrush(brush);
        setPen(Qt::NoPen);
        setBrushOrigin(QPointF(-sx, -sy));

        drawRect(QRectF(0, 0, sw, sh));
        restore();
    } else {
        if (!d->engine->hasFeature(QPaintEngine::PixmapTransform)) {
            x += d->state->matrix.dx();
            y += d->state->matrix.dy();
        }
        d->engine->drawPixmap(QRectF(x, y, w, h), pm, QRectF(sx, sy, sw, sh));
    }
}


/*!
    \fn void QPainter::drawPixmap(const QRect &target, const QPixmap &pixmap,
                                  const QRect &source)
    \overload

    Draws the rectangular portion \a source of the given \a pixmap
    into the given \a target in the paint device.

    \note The pixmap is scaled to fit the rectangle, if both the pixmap and rectangle size disagree.
*/

/*!
    \fn void QPainter::drawPixmap(const QPointF &point, const QPixmap &pixmap,
                                  const QRectF &source)
    \overload

    Draws the rectangular portion \a source of the given \a pixmap
    with its origin at the given \a point.
*/

/*!
    \fn void QPainter::drawPixmap(const QPoint &point, const QPixmap &pixmap,
                                  const QRect &source)

    \overload

    Draws the rectangular portion \a source of the given \a pixmap
    with its origin at the given \a point.
*/

/*!
    \fn void QPainter::drawPixmap(const QPointF &point, const QPixmap &pixmap)
    \overload

    Draws the given \a pixmap with its origin at the given \a point.
*/

/*!
    \fn void QPainter::drawPixmap(const QPoint &point, const QPixmap &pixmap)
    \overload

    Draws the given \a pixmap with its origin at the given \a point.
*/

/*!
    \fn void QPainter::drawPixmap(int x, int y, const QPixmap &pixmap)

    \overload

    Draws the given \a pixmap at position (\a{x}, \a{y}).
*/

/*!
    \fn void QPainter::drawPixmap(const QRect &rectangle, const QPixmap &pixmap)
    \overload

    Draws the given \a  pixmap into the given \a rectangle.

    \note The pixmap is scaled to fit the rectangle, if both the pixmap and rectangle size disagree.
*/

/*!
    \fn void QPainter::drawPixmap(int x, int y, int width, int height,
    const QPixmap &pixmap)

    \overload

    Draws the \a pixmap into the rectangle at position (\a{x}, \a{y})
    with  the given \a width and \a height.
*/

/*!
    \fn void QPainter::drawPixmap(int x, int y, int w, int h, const QPixmap &pixmap,
                                  int sx, int sy, int sw, int sh)

    \overload

    Draws the rectangular portion with the origin (\a{sx}, \a{sy}),
    width \a sw and height \a sh, of the given \a pixmap , at the
    point (\a{x}, \a{y}), with a width of \a w and a height of \a h.
    If sw or sh are equal to zero the width/height of the pixmap
    is used and adjusted by the offset sx/sy;
*/

/*!
    \fn void QPainter::drawPixmap(int x, int y, const QPixmap &pixmap,
                                  int sx, int sy, int sw, int sh)

    \overload

    Draws a pixmap at (\a{x}, \a{y}) by copying a part of the given \a
    pixmap into the paint device.

    (\a{x}, \a{y}) specifies the top-left point in the paint device that is
    to be drawn onto. (\a{sx}, \a{sy}) specifies the top-left point in \a
    pixmap that is to be drawn. The default is (0, 0).

    (\a{sw}, \a{sh}) specifies the size of the pixmap that is to be drawn.
    The default, (0, 0) (and negative) means all the way to the
    bottom-right of the pixmap.
*/

void QPainter::drawImage(const QRectF &targetRect, const QImage &image, const QRectF &sourceRect,
                         Qt::ImageConversionFlags flags)
{
    if (!isActive() || image.isNull())
        return;

    Q_D(QPainter);
    d->updateState(d->state);

    qreal x = targetRect.x();
    qreal y = targetRect.y();
    qreal w = targetRect.width();
    qreal h = targetRect.height();
    qreal sx = sourceRect.x();
    qreal sy = sourceRect.y();
    qreal sw = sourceRect.width();
    qreal sh = sourceRect.height();

    // Sanity-check clipping
    if (sw <= 0)
        sw = image.width() - sx;

    if (sh <= 0)
        sh = image.height() - sy;

    if (sx < 0) {
        qreal w_ratio = sx * w/sw;
        x -= w_ratio;
        w += w_ratio;
        sw += sx;
        sx = 0;
    }

    if (sy < 0) {
        qreal h_ratio = sy * h/sh;
        y -= h_ratio;
        h += h_ratio;
        sh += sy;
        sy = 0;
    }

    if (sw + sx > image.width()) {
        qreal delta = sw - (image.width() - sx);
        qreal w_ratio = delta * w/sw;
        sw -= delta;
        w -= w_ratio;
    }

    if (sh + sy > image.height()) {
        qreal delta = sh - (image.height() - sy);
        qreal h_ratio = delta * h/sh;
        sh -= delta;
        h -= h_ratio;
    }


    if (w < 0)
        w = sw;
    if (h < 0)
        h = sh;

    if (w == 0 || h == 0 || sw <= 0 || sh <= 0)
        return;

    if (((d->state->txop > QTransform::TxTranslate || (sw != w || sh != h))
         && !d->engine->hasFeature(QPaintEngine::PixmapTransform))
        || (!d->state->matrix.isAffine() && !d->engine->hasFeature(QPaintEngine::PerspectiveTransform))
        || (d->state->opacity != 1.0 && !d->engine->hasFeature(QPaintEngine::ConstantOpacity)))
    {
        save();
        // If there is no scaling or transformation involved we have to make sure we use the
        // antialiased and not the aliased coordinate system by rounding the coordinates.
        if (d->state->txop <= QTransform::TxTranslate && sw == w && sh == h) {
            x = qRound(x + d->state->matrix.dx()) - d->state->matrix.dx();
            y = qRound(y + d->state->matrix.dy()) - d->state->matrix.dy();
            sx = qRound(sx);
            sy = qRound(sy);
            sw = qRound(sw);
            sh = qRound(sh);
        }
        translate(x, y);
        scale(w / sw, h / sh);
        setBackgroundMode(Qt::TransparentMode);
        setRenderHint(Antialiasing, renderHints() & SmoothPixmapTransform);
        QBrush brush(image);
        brush.d->sourceRect = QRectF(sx, sy, sw, sh).toAlignedRect();
        setBrush(brush);
        setPen(Qt::NoPen);
        setBrushOrigin(QPointF(-sx, -sy));

        drawRect(QRectF(0, 0, sw, sh));
        restore();
        return;
    }

    if (d->state->txop == QTransform::TxTranslate
        && !d->engine->hasFeature(QPaintEngine::PixmapTransform)) {
        x += d->state->matrix.dx();
        y += d->state->matrix.dy();
    }

    d->engine->drawImage(QRectF(x, y, w, h), image, QRectF(sx, sy, sw, sh), flags);
}

/*!
    \fn void QPainter::drawText(const QPointF &position, const QString &text)

    Draws the given \a text with the currently defined text direction,
    beginning at the given \a position.

    This function does not handle the newline character (\n), as it cannot
    break text into multiple lines, and it cannot display the newline character.
    Use the QPainter::drawText() overload that takes a rectangle instead
    if you want to draw multiple lines of text with the newline character, or
    if you want the text to be wrapped.

    By default, QPainter draws text anti-aliased.

    \note The y-position is used as the baseline of the font.
*/

void QPainter::drawText(const QPointF &p, const QString &str)
{
    drawText(p, str, 0, 0);
}

/*!
   \internal
*/
void QPainter::drawText(const QPointF &p, const QString &str, int tf, int justificationPadding)
{
#ifdef QT_DEBUG_DRAW
    if (qt_show_painter_debug_output)
        printf("QPainter::drawText(), pos=[%.2f,%.2f], str='%s'\n", p.x(), p.y(), str.toLatin1().constData());
#endif

    if (!isActive() || str.isEmpty() || pen().style() == Qt::NoPen)
        return;

    Q_D(QPainter);

    QStackTextEngine engine(str, d->state->font);
    engine.option.setTextDirection(d->state->layoutDirection);
    if (tf & (Qt::TextForceLeftToRight|Qt::TextForceRightToLeft)) {
        engine.ignoreBidi = true;
        engine.option.setTextDirection((tf & Qt::TextForceLeftToRight) ? Qt::LeftToRight : Qt::RightToLeft);
    }
    engine.itemize();
    QScriptLine line;
    line.length = str.length();
    engine.shapeLine(line);

    int nItems = engine.layoutData->items.size();
    QVarLengthArray<int> visualOrder(nItems);
    QVarLengthArray<uchar> levels(nItems);
    for (int i = 0; i < nItems; ++i)
        levels[i] = engine.layoutData->items[i].analysis.bidiLevel;
    QTextEngine::bidiReorder(nItems, levels.data(), visualOrder.data());

    if (justificationPadding > 0) {
        engine.option.setAlignment(Qt::AlignJustify);
        engine.forceJustification = true;
        // this works because justify() is only interested in the difference between width and textWidth
        line.width = justificationPadding;
        engine.justify(line);
    }
    QFixed x = QFixed::fromReal(p.x());
    QFixed ox = x;

    for (int i = 0; i < nItems; ++i) {
        int item = visualOrder[i];
        const QScriptItem &si = engine.layoutData->items.at(item);
        if (si.analysis.flags >= QScriptAnalysis::TabOrObject) {
            x += si.width;
            continue;
        }
        QFont f = engine.font(si);
        QTextItemInt gf(si, &f);
        gf.num_glyphs = si.num_glyphs;
        gf.glyphs = engine.glyphs(&si);
        gf.chars = engine.layoutData->string.unicode() + si.position;
        gf.num_chars = engine.length(item);
        gf.width = si.width;
        gf.logClusters = engine.logClusters(&si);

        drawTextItem(QPointF(x.toReal(), p.y()), gf);

        x += si.width;
    }
}

void QPainter::drawText(const QRect &r, int flags, const QString &str, QRect *br)
{
#ifdef QT_DEBUG_DRAW
    if (qt_show_painter_debug_output)
        printf("QPainter::drawText(), r=[%d,%d,%d,%d], flags=%d, str='%s'\n",
           r.x(), r.y(), r.width(), r.height(), flags, str.toLatin1().constData());
#endif

    if (!isActive() || str.length() == 0 || pen().style() == Qt::NoPen)
        return;

    Q_D(QPainter);
    d->updateState(d->state);

    QRectF bounds;
    qt_format_text(d->state->font, r, flags, 0, str, br ? &bounds : 0, 0, 0, 0, this);
    if (br)
        *br = bounds.toAlignedRect();
}

/*!
    \fn void QPainter::drawText(const QPoint &position, const QString &text)

    \overload

    Draws the given \a text with the currently defined text direction,
    beginning at the given \a position.

    By default, QPainter draws text anti-aliased.

    \note The y-position is used as the baseline of the font.

*/

/*!
    \fn void QPainter::drawText(const QRectF &rectangle, int flags, const QString &text, QRectF *boundingRect)
    \overload

    Draws the given \a text within the provided \a rectangle.

    \table 100%
    \row
    \o \inlineimage qpainter-text.png
    \o
    \snippet doc/src/snippets/code/src.gui.painting.qpainter.cpp 17
    \endtable

    The \a boundingRect (if not null) is set to the what the bounding rectangle
    should be in order to enclose the whole text. The \a flags argument is a bitwise
    OR of the following flags:

    \list
    \o Qt::AlignLeft
    \o Qt::AlignRight
    \o Qt::AlignHCenter
    \o Qt::AlignJustify
    \o Qt::AlignTop
    \o Qt::AlignBottom
    \o Qt::AlignVCenter
    \o Qt::AlignCenter
    \o Qt::TextDontClip
    \o Qt::TextSingleLine
    \o Qt::TextExpandTabs
    \o Qt::TextShowMnemonic
    \o Qt::TextWordWrap
    \endlist

    \sa Qt::AlignmentFlag, Qt::TextFlag, boundingRect(), layoutDirection()

    By default, QPainter draws text anti-aliased.

    \note The y-coordinate of \a rectangle is used as the top of the font.
*/
void QPainter::drawText(const QRectF &r, int flags, const QString &str, QRectF *br)
{
#ifdef QT_DEBUG_DRAW
    if (qt_show_painter_debug_output)
        printf("QPainter::drawText(), r=[%.2f,%.2f,%.2f,%.2f], flags=%d, str='%s'\n",
           r.x(), r.y(), r.width(), r.height(), flags, str.toLatin1().constData());
#endif

    if (!isActive() || str.length() == 0 || pen().style() == Qt::NoPen)
        return;

    Q_D(QPainter);
    d->updateState(d->state);

    qt_format_text(d->state->font, r, flags, 0, str, br, 0, 0, 0, this);
}

/*!
    \fn void QPainter::drawText(const QRect &rectangle, int flags, const QString &text, QRect *boundingRect)
    \overload

    Draws the given \a text within the provided \a rectangle according
    to the specified \a flags. The \a boundingRect (if not null) is set to
    the what the bounding rectangle should be in order to enclose the whole text.

    By default, QPainter draws text anti-aliased.

    \note The y-coordinate of \a rectangle is used as the top of the font.
*/

/*!
    \fn void QPainter::drawText(int x, int y, const QString &text)

    \overload

    Draws the given \a text at position (\a{x}, \a{y}), using the painter's
    currently defined text direction.

    By default, QPainter draws text anti-aliased.

    \note The y-position is used as the baseline of the font.

*/

/*!
    \fn void QPainter::drawText(int x, int y, int width, int height, int flags,
                                const QString &text, QRect *boundingRect)

    \overload

    Draws the given \a text within the rectangle with origin (\a{x},
    \a{y}), \a width and \a height.

    The \a boundingRect (if not null) is set to the actual bounding
    rectangle of the output.  The \a flags argument is a bitwise OR of
    the following flags:

    \list
    \o Qt::AlignLeft
    \o Qt::AlignRight
    \o Qt::AlignHCenter
    \o Qt::AlignJustify
    \o Qt::AlignTop
    \o Qt::AlignBottom
    \o Qt::AlignVCenter
    \o Qt::AlignCenter
    \o Qt::TextSingleLine
    \o Qt::TextExpandTabs
    \o Qt::TextShowMnemonic
    \o Qt::TextWordWrap
    \endlist

    By default, QPainter draws text anti-aliased.

    \note The y-position is used as the baseline of the font.

    \sa Qt::AlignmentFlag, Qt::TextFlag
*/

/*!
    \fn void QPainter::drawText(const QRectF &rectangle, const QString &text,
        const QTextOption &option)
    \overload

    Draws the given \a text in the \a rectangle specified using the \a option
    to control its positioning and orientation.

    By default, QPainter draws text anti-aliased.

    \note The y-coordinate of \a rectangle is used as the top of the font.
*/
void QPainter::drawText(const QRectF &r, const QString &text, const QTextOption &o)
{
#ifdef QT_DEBUG_DRAW
    if (qt_show_painter_debug_output)
        printf("QPainter::drawText(), r=[%.2f,%.2f,%.2f,%.2f], str='%s'\n",
           r.x(), r.y(), r.width(), r.height(), text.toLatin1().constData());
#endif

    if (!isActive() || text.length() == 0 || pen().style() == Qt::NoPen)
        return;

    Q_D(QPainter);
    d->updateState(d->state);

    QTextOption opt = o;
    int flags = opt.alignment();
    if (!(opt.alignment() & Qt::AlignJustify)) { // justify is like left. In that case, don't touch.
        // avoid doing the alignment twice since qt_format_text does it, too
        opt.setAlignment(Qt::AlignLeft);
    }

    if (opt.wrapMode() == QTextOption::WordWrap)
        flags |= Qt::TextWordWrap;
    else if (opt.wrapMode() == QTextOption::WrapAnywhere)
        flags |= Qt::TextWrapAnywhere;

    if (opt.flags() & QTextOption::IncludeTrailingSpaces)
        flags |= Qt::TextIncludeTrailingSpaces;

    if (opt.tabStop() >= 0 || !opt.tabArray().isEmpty())
        flags |= Qt::TextExpandTabs;

    qt_format_text(d->state->font, r, flags, &opt, text, 0, 0, 0, 0, this);
}

/*!
    \fn void QPainter::drawTextItem(int x, int y, const QTextItem &ti)

    \internal
    \overload
*/

/*!
    \fn void QPainter::drawTextItem(const QPoint &p, const QTextItem &ti)

    \internal
    \overload

    Draws the text item \a ti at position \a p.
*/

/*! \internal
    Draws the text item \a ti at position \a p.

    This method ignores the painters background mode and
    color. drawText and qt_format_text have to do it themselves, as
    only they know the extents of the complete string.

    It ignores the font set on the painter as the text item has one of its own.

    The underline and strikeout parameters of the text items font are
    ignored aswell. You'll need to pass in the correct flags to get
    underlining and strikeout.
*/
static QPainterPath generateWavyPath(qreal minWidth, QPaintDevice *device)
{
    extern int qt_defaultDpi();
    QPainterPath path;

    bool up = true;
    const qreal radius = 1.3 * device->logicalDpiY() / qt_defaultDpi();
    qreal xs, ys;
    int i = 0;
    path.moveTo(0, radius);
    do {
        xs = i*(2*radius);
        ys = 0;

        qreal remaining = minWidth - xs;
        qreal angle = 180;

        // cut-off at the last arc segment
        if (remaining < 2 * radius)
            angle = 180 * acos(1 - remaining / radius) / Q_PI;

        path.arcTo(xs, ys, 2*radius, 2*radius, 180, up ? angle : -angle);

        up = !up;
        ++i;
    } while (xs + 2*radius < minWidth);

    return path;
}

static void drawTextItemDecoration(QPainter *painter, const QPointF &pos, const QTextItemInt &ti)
{
    QTextCharFormat::UnderlineStyle underlineStyle = ti.underlineStyle;
    if (underlineStyle == QTextCharFormat::NoUnderline
        && !(ti.flags & (QTextItem::StrikeOut | QTextItem::Overline)))
        return;

    QFontEngine *fe = ti.fontEngine;

    const QPen oldPen = painter->pen();
    const QBrush oldBrush = painter->brush();
    painter->setBrush(Qt::NoBrush);
    QPen pen = oldPen;
    pen.setStyle(Qt::SolidLine);
    pen.setWidthF(fe->lineThickness().toReal());

    QLineF line(pos.x(), pos.y(), pos.x() + ti.width.toReal(), pos.y());
    // deliberately ceil the offset to avoid the underline coming too close to
    // the text above it.
    const int underlinePos = qCeil(pos.y()) + qCeil(fe->underlinePosition().toReal());

    if (underlineStyle == QTextCharFormat::SpellCheckUnderline) {
        underlineStyle = QTextCharFormat::UnderlineStyle(QApplication::style()->styleHint(QStyle::SH_SpellCheckUnderlineStyle));
    }

    if (underlineStyle == QTextCharFormat::WaveUnderline) {
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);
        painter->translate(pos.x(), underlinePos);

        QColor uc = ti.charFormat.underlineColor();
        if (uc.isValid())
            painter->setPen(uc);

        painter->drawPath(generateWavyPath(ti.width.toReal(), painter->device()));
        painter->restore();
    } else if (underlineStyle != QTextCharFormat::NoUnderline) {
        QLineF underLine(line.x1(), underlinePos, line.x2(), underlinePos);

        QColor uc = ti.charFormat.underlineColor();
        if (uc.isValid())
            pen.setColor(uc);

        pen.setStyle((Qt::PenStyle)(underlineStyle));
        painter->setPen(pen);
        painter->drawLine(underLine);
    }

    pen.setStyle(Qt::SolidLine);
    pen.setColor(oldPen.color());

    if (ti.flags & QTextItem::StrikeOut) {
        QLineF strikeOutLine = line;
        strikeOutLine.translate(0., - fe->ascent().toReal() / 3.);
        painter->drawLine(strikeOutLine);
    }

    if (ti.flags & QTextItem::Overline) {
        QLineF overLine = line;
        overLine.translate(0., - fe->ascent().toReal());
        painter->drawLine(overLine);
    }

    painter->setPen(oldPen);
    painter->setBrush(oldBrush);
}

/*!
    \internal
    \since 4.1
*/
void QPainter::drawTextItem(const QPointF &p, const QTextItem &_ti)
{
#ifdef QT_DEBUG_DRAW
    if (qt_show_painter_debug_output)
        printf("QPainter::drawTextItem(), pos=[%.f,%.f], str='%s'\n",
               p.x(), p.y(), qPrintable(_ti.text()));
#endif

    Q_D(QPainter);

    if (!isActive())
        return;
    if (!qt_painter_thread_test(d->device->devType(),
                                "text and fonts",
                                QFontDatabase::supportsThreadedFontRendering())) {
        return;
    }

    QTextItemInt &ti = const_cast<QTextItemInt &>(static_cast<const QTextItemInt &>(_ti));

    if (d->state->bgMode == Qt::OpaqueMode) {
        QRectF rect(p.x(), p.y() - ti.ascent.toReal(),
                    ti.width.toReal(),
                    (ti.ascent + ti.descent + 1).toReal());
        fillRect(rect, d->state->bgBrush);
    }


    if (pen().style() == Qt::NoPen)
        return;

    const RenderHints oldRenderHints = d->state->renderHints;
    if (d->state->txop >= QTransform::TxScale && d->state->txop < QTransform::TxProject) {
        // draw antialias decoration (underline/overline/strikeout) with
        // transformed text

        const QTransform &m = d->state->matrix;
        bool isPlain90DegreeRotation =
            (qFuzzyCompare(m.m11() + 1, qreal(1))
             && qFuzzyCompare(m.m12(), qreal(1))
             && qFuzzyCompare(m.m21(), qreal(-1))
             && qFuzzyCompare(m.m22() + 1, qreal(1))
                )
            ||
            (qFuzzyCompare(m.m11(), qreal(-1))
             && qFuzzyCompare(m.m12() + 1, qreal(1))
             && qFuzzyCompare(m.m21() + 1, qreal(1))
             && qFuzzyCompare(m.m22(), qreal(-1))
                )
            ||
            (qFuzzyCompare(m.m11() + 1, qreal(1))
             && qFuzzyCompare(m.m12(), qreal(-1))
             && qFuzzyCompare(m.m21(), qreal(1))
             && qFuzzyCompare(m.m22() + 1, qreal(1))
                )
            ;
        if (!isPlain90DegreeRotation)
            setRenderHint(QPainter::Antialiasing, true);
    }

    d->updateState(d->state);

    if (!ti.num_glyphs) {
        // nothing to do
    } else if (ti.fontEngine->type() == QFontEngine::Multi) {
        QFontEngineMulti *multi = static_cast<QFontEngineMulti *>(ti.fontEngine);

        QGlyphLayout *glyphs = ti.glyphs;
        int which = glyphs[0].glyph >> 24;

        qreal x = p.x();
        qreal y = p.y();

        int logClusterOffset = ti.logClusters[0];
        int start = 0;
        int end, i;
        for (end = 0; end < ti.num_glyphs; ++end) {
            const int e = glyphs[end].glyph >> 24;
            if (e == which)
                continue;

            // draw the text
            QTextItemInt ti2 = ti;
            ti2.glyphs = ti.glyphs + start;
            ti2.num_glyphs = end - start;
            ti2.fontEngine = multi->engine(which);

            if (ti.logClusters && ti.chars) {
                while (ti.logClusters[ti2.chars - ti.chars] - logClusterOffset < start)
                    ++ti2.chars;

                ti2.logClusters += (ti2.chars - ti.chars);

                ti2.num_chars = 0;
                int char_start = ti2.chars - ti.chars;
                while (char_start + ti2.num_chars < ti.num_chars && ti2.logClusters[ti2.num_chars] - logClusterOffset < end)
                    ++ti2.num_chars;
            }
            ti2.width = 0;
            // set the high byte to zero and calc the width
            for (i = start; i < end; ++i) {
                glyphs[i].glyph = glyphs[i].glyph & 0xffffff;
                ti2.width += ti.glyphs[i].effectiveAdvance();
            }

            d->engine->drawTextItem(QPointF(x, y), ti2);

            // reset the high byte for all glyphs and advance to the next sub-string
            const int hi = which << 24;
            for (i = start; i < end; ++i) {
                glyphs[i].glyph = hi | glyphs[i].glyph;
            }
            x += ti2.width.toReal();

            // change engine
            start = end;
            which = e;
        }

        // draw the text
        QTextItemInt ti2 = ti;
        ti2.glyphs = ti.glyphs + start;
        ti2.num_glyphs = end - start;
        ti2.fontEngine = multi->engine(which);

        if (ti.logClusters && ti.chars) {
            while (ti.logClusters[ti2.chars - ti.chars] - logClusterOffset < start)
                ++ti2.chars;

            ti2.logClusters += (ti2.chars - ti.chars);

            ti2.num_chars = 0;
            int char_start = ti2.chars - ti.chars;
            while (char_start + ti2.num_chars < ti.num_chars && ti2.logClusters[ti2.num_chars] - logClusterOffset < end)
                ++ti2.num_chars;
        }
        ti2.width = 0;
        // set the high byte to zero and calc the width
        for (i = start; i < end; ++i) {
            glyphs[i].glyph = glyphs[i].glyph & 0xffffff;
            ti2.width += ti.glyphs[i].effectiveAdvance();
        }

        d->engine->drawTextItem(QPointF(x,y), ti2);

        // reset the high byte for all glyphs
        const int hi = which << 24;
        for (i = start; i < end; ++i)
            glyphs[i].glyph = hi | glyphs[i].glyph;

    } else {
        d->engine->drawTextItem(p, ti);
    }
    drawTextItemDecoration(this, p, ti);

    if (d->state->renderHints != oldRenderHints) {
        d->state->renderHints = oldRenderHints;
        d->state->dirtyFlags |= QPaintEngine::DirtyHints;
    }
}

/*!
    \fn QRectF QPainter::boundingRect(const QRectF &rectangle, int flags, const QString &text)

    Returns the bounding rectangle of the \a text as it will appear
    when drawn inside the given \a rectangle with the specified \a
    flags using the currently set font(); i.e the function tells you
    where the drawText() function will draw when given the same
    arguments.

    If the \a text does not fit within the given \a rectangle using
    the specified \a flags, the function returns the required
    rectangle.

    The \a flags argument is a bitwise OR of the following flags:
    \list
         \o Qt::AlignLeft
         \o Qt::AlignRight
         \o Qt::AlignHCenter
         \o Qt::AlignTop
         \o Qt::AlignBottom
         \o Qt::AlignVCenter
         \o Qt::AlignCenter
         \o Qt::TextSingleLine
         \o Qt::TextExpandTabs
         \o Qt::TextShowMnemonic
         \o Qt::TextWordWrap
    \endlist
    If several of the horizontal or several of the vertical alignment
    flags are set, the resulting alignment is undefined.

    \sa drawText(), Qt::Alignment, Qt::TextFlag
*/

/*!
    \fn QRect QPainter::boundingRect(const QRect &rectangle, int flags,
                                     const QString &text)

    \overload

    Returns the bounding rectangle of the \a text as it will appear
    when drawn inside the given \a rectangle with the specified \a
    flags using the currently set font().
*/

/*!
    \fn QRect QPainter::boundingRect(int x, int y, int w, int h, int flags,
                                     const QString &text);

    \overload

    Returns the bounding rectangle of the given \a text as it will
    appear when drawn inside the rectangle beginning at the point
    (\a{x}, \a{y}) with width \a w and height \a h.
*/
QRect QPainter::boundingRect(const QRect &rect, int flags, const QString &str)
{
    if (str.isEmpty())
        return QRect(rect.x(),rect.y(), 0,0);
    QRect brect;
    drawText(rect, flags | Qt::TextDontPrint, str, &brect);
    return brect;
}



QRectF QPainter::boundingRect(const QRectF &rect, int flags, const QString &str)
{
    if (str.isEmpty())
        return QRectF(rect.x(),rect.y(), 0,0);
    QRectF brect;
    drawText(rect, flags | Qt::TextDontPrint, str, &brect);
    return brect;
}

/*!
    \fn QRectF QPainter::boundingRect(const QRectF &rectangle,
        const QString &text, const QTextOption &option)

    \overload

    Instead of specifying flags as a bitwise OR of the
    Qt::AlignmentFlag and Qt::TextFlag, this overloaded function takes
    an \a option argument. The QTextOption class provides a
    description of general rich text properties.

    \sa QTextOption
*/
QRectF QPainter::boundingRect(const QRectF &r, const QString &text, const QTextOption &o)
{
    if (!isActive() || text.length() == 0)
        return QRectF(r.x(),r.y(), 0,0);

    Q_D(QPainter);
    d->updateState(d->state);

    QTextOption opt = o;
    int flags = opt.alignment() | Qt::TextDontPrint;
    // avoid doing the alignment twice since qt_format_text does it, too
    opt.setAlignment(Qt::AlignLeft);

    if (opt.wrapMode() == QTextOption::WordWrap)
        flags |= Qt::TextWordWrap;
    else if (opt.wrapMode() == QTextOption::WrapAnywhere)
        flags |= Qt::TextWrapAnywhere;
    if (opt.flags() & QTextOption::IncludeTrailingSpaces)
        flags |= Qt::TextIncludeTrailingSpaces;

    if (opt.tabStop() >= 0 || !opt.tabArray().isEmpty())
        flags |= Qt::TextExpandTabs;

    QRectF br;
    qt_format_text(d->state->font, r, flags, &opt, text, &br, 0, 0, 0, this);
    return br;
}

/*!
    \fn void QPainter::drawTiledPixmap(const QRectF &rectangle, const QPixmap &pixmap, const QPointF &position)

    Draws a tiled \a pixmap, inside the given \a rectangle with its
    origin at the given \a position.

    Calling drawTiledPixmap() is similar to calling drawPixmap()
    several times to fill (tile) an area with a pixmap, but is
    potentially much more efficient depending on the underlying window
    system.

    \sa drawPixmap()
*/
void QPainter::drawTiledPixmap(const QRectF &r, const QPixmap &pixmap, const QPointF &sp)
{
#ifdef QT_DEBUG_DRAW
    if (qt_show_painter_debug_output)
        printf("QPainter::drawTiledPixmap(), target=[%.2f,%.2f,%.2f,%.2f], pix=[%d,%d], offset=[%.2f,%.2f]\n",
               r.x(), r.y(), r.width(), r.height(),
               pixmap.width(), pixmap.height(),
               sp.x(), sp.y());
#endif

    Q_D(QPainter);
    if (!isActive() || pixmap.isNull() || r.isEmpty())
        return;
    if (!qt_painter_thread_test(d->device->devType(), "drawTiledPixmap()"))
        return;

    qreal sw = pixmap.width();
    qreal sh = pixmap.height();
    qreal sx = sp.x();
    qreal sy = sp.y();
    if (sx < 0)
        sx = qRound(sw) - qRound(-sx) % qRound(sw);
    else
        sx = qRound(sx) % qRound(sw);
    if (sy < 0)
        sy = qRound(sh) - -qRound(sy) % qRound(sh);
    else
        sy = qRound(sy) % qRound(sh);

    if (d->state->bgMode == Qt::OpaqueMode)
        fillRect(r, d->state->bgBrush);

    d->updateState(d->state);
    if ((d->state->txop > QTransform::TxTranslate
        && !d->engine->hasFeature(QPaintEngine::PixmapTransform))
        || (d->state->opacity != 1.0 && !d->engine->hasFeature(QPaintEngine::ConstantOpacity)))
    {
        save();
        setBackgroundMode(Qt::TransparentMode);
        setRenderHint(Antialiasing, renderHints() & SmoothPixmapTransform);
        setBrush(QBrush(d->state->pen.color(), pixmap));
        setPen(Qt::NoPen);

        // If there is no scaling or transformation involved we have to make sure we use the
        // antialiased and not the aliased coordinate system by rounding the coordinates.
        if (d->state->txop <= QTransform::TxTranslate) {
            qreal x = qRound(r.x() + d->state->matrix.dx()) - d->state->matrix.dx();
            qreal y = qRound(r.y() + d->state->matrix.dy()) - d->state->matrix.dy();
            qreal w = qRound(r.width());
            qreal h = qRound(r.height());
            sx = qRound(sx);
            sy = qRound(sy);
            setBrushOrigin(QPointF(r.x()-sx, r.y()-sy));
            drawRect(QRectF(x, y, w, h));
        } else {
            setBrushOrigin(QPointF(r.x()-sx, r.y()-sy));
            drawRect(r);
        }
        restore();
        return;
    }

    qreal x = r.x();
    qreal y = r.y();
    if (d->state->txop == QTransform::TxTranslate
        && !d->engine->hasFeature(QPaintEngine::PixmapTransform)) {
        x += d->state->matrix.dx();
        y += d->state->matrix.dy();
    }

    d->engine->drawTiledPixmap(QRectF(x, y, r.width(), r.height()), pixmap, QPointF(sx, sy));
}

/*!
    \fn QPainter::drawTiledPixmap(const QRect &rectangle, const QPixmap &pixmap,
                                  const QPoint &position = QPoint())
    \overload

    Draws a tiled \a pixmap, inside the given \a rectangle with its
    origin at the given \a position.
*/

/*!
    \fn void QPainter::drawTiledPixmap(int x, int y, int width, int height, const
         QPixmap &pixmap, int sx, int sy);
    \overload

    Draws a tiled \a pixmap in the specified rectangle.

    (\a{x}, \a{y}) specifies the top-left point in the paint device
    that is to be drawn onto; with the given \a width and \a
    height. (\a{sx}, \a{sy}) specifies the top-left point in the \a
    pixmap that is to be drawn; this defaults to (0, 0).
*/

#ifndef QT_NO_PICTURE

/*!
    \fn void QPainter::drawPicture(const QPointF &point, const QPicture &picture)

    Replays the given \a picture at the given \a point.

    The QPicture class is a paint device that records and replays
    QPainter commands. A picture serializes the painter commands to an
    IO device in a platform-independent format. Everything that can be
    painted on a widget or pixmap can also be stored in a picture.

    This function does exactly the same as QPicture::play() when
    called with \a point = QPoint(0, 0).

    \table 100%
    \row
    \o
    \snippet doc/src/snippets/code/src.gui.painting.qpainter.cpp 18
    \endtable

    \sa QPicture::play()
*/

void QPainter::drawPicture(const QPointF &p, const QPicture &picture)
{
    if (!isActive())
        return;
    Q_D(QPainter);
    d->updateState(d->state);
    save();
    translate(p);
    const_cast<QPicture *>(&picture)->play(this);
    restore();
}

/*!
    \fn void QPainter::drawPicture(const QPoint &point, const QPicture &picture)
    \overload

    Replays the given \a picture at the given \a point.
*/

/*!
    \fn void QPainter::drawPicture(int x, int y, const QPicture &picture)
    \overload

    Draws the given \a picture at point (\a x, \a y).
*/

#endif // QT_NO_PICTURE

/*!
    \fn void QPainter::eraseRect(const QRectF &rectangle)

    Erases the area inside the given \a rectangle. Equivalent to
    calling
    \snippet doc/src/snippets/code/src.gui.painting.qpainter.cpp 19

    \sa fillRect()
*/
void QPainter::eraseRect(const QRectF &r)
{
    if (!isActive())
        return;
    Q_D(QPainter);
    d->updateState(d->state);

    bool noTexture = qHasPixmapTexture(d->state->bgBrush)
                     ? d->state->bgBrush.texture().isNull()
                     : d->state->bgBrush.textureImage().isNull();
    if (noTexture)
        fillRect(r, d->state->bgBrush);
    else
        drawTiledPixmap(r, d->state->bgBrush.texture(), -d->state->bgOrigin); // ### not thread-safe - we need drawTiledImage()
}

static inline bool needsResolving(const QBrush &brush)
{
    Qt::BrushStyle s = brush.style();
    return ((s == Qt::LinearGradientPattern || s == Qt::RadialGradientPattern ||
             s == Qt::ConicalGradientPattern) &&
            brush.gradient()->coordinateMode() == QGradient::ObjectBoundingMode);
}

/*!
    \fn void QPainter::eraseRect(const QRect &rectangle)
    \overload

    Erases the area inside the given  \a rectangle.
*/

/*!
    \fn void QPainter::eraseRect(int x, int y, int width, int height)
    \overload

    Erases the area inside the rectangle beginning at (\a x, \a y)
    with the given \a width and \a height.
*/

/*!
    \fn void QPainter::fillRect(const QRectF &rectangle, const QBrush &brush)

    Fills the given \a rectangle  with the given  \a brush.

    Alternatively, you can specify a QColor instead of a QBrush; the
    QBrush constructor (taking a QColor argument) will automatically
    create a solid pattern brush.

    \sa drawRect()
*/
void QPainter::fillRect(const QRectF &r, const QBrush &brush)
{
    if (!isActive())
        return;
    QPen oldPen   = pen();
    bool swap = oldPen.style() != Qt::NoPen;
    if (swap)
        setPen(Qt::NoPen);
    QBrush oldBrush = this->brush();
    setBrush(brush);
    drawRect(r);
    setBrush(oldBrush);
    if (swap)
        setPen(oldPen);
}

void QPainter::fillRect(const QRect &r, const QBrush &brush)
{
    if (!isActive())
        return;

    Q_D(QPainter);
    if (d->fillrect_func && !(d->state->renderHints & QPainter::Antialiasing)
        && d->state->txop <= QTransform::TxTranslate)
    {
        if (d->state->dirtyFlags)
            d->updateState(d->state);

        const QRect &rt = r;

        if (d->state->bgMode == Qt::OpaqueMode)
            ((d->engine)->*(d->fillrect_func))(rt, d->state->bgBrush);

        if (needsResolving(brush))
            ((d->engine)->*(d->fillrect_func))(rt, stretchGradientToUserSpace(brush, rt));
        else
            ((d->engine)->*(d->fillrect_func))(rt, brush);

        return;
    }

    QPen oldPen   = pen();
    bool swap = oldPen.style() != Qt::NoPen;
    if (swap)
        setPen(Qt::NoPen);
    QBrush oldBrush = this->brush();
    setBrush(brush);
    drawRect(r);
    setBrush(oldBrush);
    if (swap)
        setPen(oldPen);
}

/*!
    \fn void QPainter::fillRect(const QRect &rectangle, const QBrush &brush)
    \overload

    Fills the given \a rectangle with the given \a brush.
*/

/*!
    \fn void QPainter::fillRect(int x, int y, int width, int height, const QBrush &brush)

    \overload

    Fills the rectangle beginning at (\a{x}, \a{y}) with the given \a
    width and \a height, using the given \a brush.
*/

/*!
    Sets the given render \a hint on the painter if \a on is true;
    otherwise clears the render hint.

    \sa setRenderHints(), renderHints(), {QPainter#Rendering
    Quality}{Rendering Quality}
*/
void QPainter::setRenderHint(RenderHint hint, bool on)
{
#ifdef QT_DEBUG_DRAW
    if (qt_show_painter_debug_output)
        printf("QPainter::setRenderHint: hint=%x, %s\n", hint, on ? "on" : "off");
#endif

#ifndef QT_NO_DEBUG
    static const bool antialiasingDisabled = qgetenv("QT_NO_ANTIALIASING").toInt();
    if (hint == QPainter::Antialiasing && antialiasingDisabled)
        return;
#endif

    setRenderHints(hint, on);
}

/*!
    \since 4.2

    Sets the given render \a hints on the painter if \a on is true;
    otherwise clears the render hints.

    \sa setRenderHint(), renderHints(), {QPainter#Rendering
    Quality}{Rendering Quality}
*/

void QPainter::setRenderHints(RenderHints hints, bool on)
{
    if (!isActive()) {
        qWarning("QPainter::setRenderHint: Painter must be active to set rendering hints");
        return;
    }
    Q_D(QPainter);

    if (on)
        d->state->renderHints |= hints;
    else
        d->state->renderHints &= ~hints;

    d->state->dirtyFlags |= QPaintEngine::DirtyHints;
}

/*!
    Returns a flag that specifies the rendering hints that are set for
    this painter.

    \sa testRenderHint(), {QPainter#Rendering Quality}{Rendering Quality}
*/
QPainter::RenderHints QPainter::renderHints() const
{
    if (!isActive()) {
        qWarning("QPainter::renderHints: Painter must be active to set rendering hints");
        return 0;
    }
    Q_D(const QPainter);
    return d->state->renderHints;
}

/*!
    \fn bool QPainter::testRenderHint(RenderHint hint) const
    \since 4.3

    Returns true if \a hint is set; otherwise returns false.

    \sa renderHints(), setRenderHint()
*/

/*!
    Returns true if view transformation is enabled; otherwise returns
    false.

    \sa setViewTransformEnabled(), worldMatrix()
*/

bool QPainter::viewTransformEnabled() const
{
    Q_D(const QPainter);
    return d->state->VxF;
}

/*!
    \fn void QPainter::setWindow(const QRect &rectangle)

    Sets the painter's window to the given \a rectangle, and enables
    view transformations.

    The window rectangle is part of the view transformation. The
    window specifies the logical coordinate system. Its sister, the
    viewport(), specifies the device coordinate system.

    The default window rectangle is the same as the device's
    rectangle.

    \sa window(), viewTransformEnabled(), {The Coordinate
    System#Window-Viewport Conversion}{Window-Viewport Conversion}
*/

/*!
    \fn void QPainter::setWindow(int x, int y, int width, int height)
    \overload

    Sets the painter's window to the rectangle beginning at (\a x, \a
    y) and the given \a width and \a height.
*/

void QPainter::setWindow(const QRect &r)
{
#ifdef QT_DEBUG_DRAW
    if (qt_show_painter_debug_output)
        printf("QPainter::setWindow(), [%d,%d,%d,%d]\n", r.x(), r.y(), r.width(), r.height());
#endif

    if (!isActive()) {
        qWarning("QPainter::setWindow: Painter not active");
        return;
    }

    Q_D(QPainter);

    d->state->wx = r.x();
    d->state->wy = r.y();
    d->state->ww = r.width();
    d->state->wh = r.height();
    if (d->state->VxF)
        d->updateMatrix();
    else
        setViewTransformEnabled(true);
}

/*!
    Returns the window rectangle.

    \sa setWindow(), setViewTransformEnabled()
*/

QRect QPainter::window() const
{
    Q_D(const QPainter);
    return QRect(d->state->wx, d->state->wy, d->state->ww, d->state->wh);
}

/*!
    \fn void QPainter::setViewport(const QRect &rectangle)

    Sets the painter's viewport rectangle to the given \a rectangle,
    and enables view transformations.

    The viewport rectangle is part of the view transformation. The
    viewport specifies the device coordinate system. Its sister, the
    window(), specifies the logical coordinate system.

    The default viewport rectangle is the same as the device's
    rectangle.

    \sa viewport(), viewTransformEnabled() {The Coordinate
    System#Window-Viewport Conversion}{Window-Viewport Conversion}
*/

/*!
    \fn void QPainter::setViewport(int x, int y, int width, int height)
    \overload

    Sets the painter's viewport rectangle to be the rectangle
    beginning at (\a x, \a y) with the given \a width and \a height.
*/

void QPainter::setViewport(const QRect &r)
{
#ifdef QT_DEBUG_DRAW
    if (qt_show_painter_debug_output)
        printf("QPainter::setViewport(), [%d,%d,%d,%d]\n", r.x(), r.y(), r.width(), r.height());
#endif

    if (!isActive()) {
        qWarning("QPainter::setViewport: Painter not active");
        return;
    }

    Q_D(QPainter);

    d->state->vx = r.x();
    d->state->vy = r.y();
    d->state->vw = r.width();
    d->state->vh = r.height();
    if (d->state->VxF)
        d->updateMatrix();
    else
        setViewTransformEnabled(true);
}

/*!
    Returns the viewport rectangle.

    \sa setViewport(), setViewTransformEnabled()
*/

QRect QPainter::viewport() const
{
    Q_D(const QPainter);
    return QRect(d->state->vx, d->state->vy, d->state->vw, d->state->vh);
}

/*! \fn bool QPainter::hasViewXForm() const
    \compat

    Use viewTransformEnabled() instead.
*/

/*! \fn bool QPainter::hasWorldXForm() const
    \compat

    Use worldMatrixEnabled() instead.
*/

/*! \fn void QPainter::resetXForm()
    \compat

    Use resetMatrix() instead.
*/

/*! \fn void QPainter::setViewXForm(bool enabled)
    \compat

    Use setViewTransformEnabled() instead.
*/

/*! \fn void QPainter::setWorldXForm(bool enabled)
    \compat

    Use setWorldMatrixEnabled() instead.
*/
/*!
    Enables view transformations if \a enable is true, or disables
    view transformations if \a enable is false.

    \sa viewTransformEnabled(), {The Coordinate System#Window-Viewport
    Conversion}{Window-Viewport Conversion}
*/

void QPainter::setViewTransformEnabled(bool enable)
{
#ifdef QT_DEBUG_DRAW
    if (qt_show_painter_debug_output)
        printf("QPainter::setViewTransformEnabled(), enable=%d\n", enable);
#endif

    if (!isActive()) {
        qWarning("QPainter::setViewTransformEnabled: Painter not active");
        return;
    }
    Q_D(QPainter);
    if (enable == d->state->VxF)
        return;

    d->state->VxF = enable;
    d->updateMatrix();
}

#ifdef QT3_SUPPORT

/*!
    Use the worldMatrix() combined with QMatrix::dx() instead.

    \oldcode
        QPainter painter(this);
        qreal x = painter.translationX();
    \newcode
        QPainter painter(this);
        qreal x = painter.worldMatrix().dx();
    \endcode
*/
qreal QPainter::translationX() const
{
    Q_D(const QPainter);
    return d->state->worldMatrix.dx();
}

/*!
    Use the worldMatrix() combined with QMatrix::dy() instead.

    \oldcode
        QPainter painter(this);
        qreal y = painter.translationY();
    \newcode
        QPainter painter(this);
        qreal y = painter.worldMatrix().dy();
    \endcode
*/
qreal QPainter::translationY() const
{
    Q_D(const QPainter);
    return d->state->worldMatrix.dy();
}

/*!
    \fn void QPainter::map(int x, int y, int *rx, int *ry) const

    \internal

    Sets (\a{rx}, \a{ry}) to the point that results from applying the
    painter's current transformation on the point (\a{x}, \a{y}).
*/
void QPainter::map(int x, int y, int *rx, int *ry) const
{
    QPoint p(x, y);
    p = p * combinedMatrix();
    *rx = p.x();
    *ry = p.y();
}

/*!
    \fn QPoint QPainter::xForm(const QPoint &point) const

    Use combinedTransform() instead.
*/

QPoint QPainter::xForm(const QPoint &p) const
{
    Q_D(const QPainter);
    if (d->state->txop == QTransform::TxNone)
        return p;
    return p * combinedMatrix();
}


/*!
    \fn QRect QPainter::xForm(const QRect &rectangle) const
    \overload

    Use combinedTransform() instead of this function and call
    mapRect() on the result to obtain a QRect.
*/

QRect QPainter::xForm(const QRect &r) const
{
    Q_D(const QPainter);
    if (d->state->txop == QTransform::TxNone)
        return r;
    return combinedMatrix().mapRect(r);
}

/*!
    \fn QPolygon QPainter::xForm(const QPolygon &polygon) const
    \overload

    Use combinedTransform() instead.
*/

QPolygon QPainter::xForm(const QPolygon &a) const
{
    Q_D(const QPainter);
    if (d->state->txop == QTransform::TxNone)
        return a;
    return a * combinedMatrix();
}

/*!
    \fn QPolygon QPainter::xForm(const QPolygon &polygon, int index, int count) const
    \overload

    Use combinedTransform() combined with QPolygon::mid() instead.

    \oldcode
        QPainter painter(this);
        QPolygon transformed = painter.xForm(polygon, index, count)
    \newcode
        QPainter painter(this);
        QPolygon transformed = polygon.mid(index, count) * painter.combinedMatrix();
    \endcode
*/

QPolygon QPainter::xForm(const QPolygon &av, int index, int npoints) const
{
    int lastPoint = npoints < 0 ? av.size() : index+npoints;
    QPolygon a(lastPoint-index);
    memcpy(a.data(), av.data()+index, (lastPoint-index)*sizeof(QPoint));
    return a * combinedMatrix();
}

/*!
    \fn QPoint QPainter::xFormDev(const QPoint &point) const
    \overload

    Use combinedTransform() combined with QMatrix::inverted() instead.

    \oldcode
        QPainter painter(this);
        QPoint transformed = painter.xFormDev(point);
    \newcode
        QPainter painter(this);
        QPoint transformed = point * painter.combinedMatrix().inverted();
    \endcode
*/

QPoint QPainter::xFormDev(const QPoint &p) const
{
    Q_D(const QPainter);
    if(d->state->txop == QTransform::TxNone)
        return p;
    return p * combinedMatrix().inverted();
}

/*!
    \fn QRect QPainter::xFormDev(const QRect &rectangle) const
    \overload

    Use  combineMatrix() combined with QMatrix::inverted() instead.

    \oldcode
        QPainter painter(this);
        QRect transformed = painter.xFormDev(rectangle);
    \newcode
        QPainter painter(this);
        QRect transformed = rectangle * painter.combinedMatrix().inverted();
    \endcode
*/

QRect QPainter::xFormDev(const QRect &r)  const
{
    Q_D(const QPainter);
    if (d->state->txop == QTransform::TxNone)
        return r;
    return combinedMatrix().inverted().mapRect(r);
}

/*!
    \overload

    \fn QPoint QPainter::xFormDev(const QPolygon &polygon) const
    \overload

    Use  combinedMatrix() combined with QMatrix::inverted() instead.

    \oldcode
        QPainter painter(this);
        QPolygon transformed = painter.xFormDev(rectangle);
    \newcode
        QPainter painter(this);
        QPolygon transformed = polygon * painter.combinedMatrix().inverted();
    \endcode
*/

QPolygon QPainter::xFormDev(const QPolygon &a) const
{
    Q_D(const QPainter);
    if (d->state->txop == QTransform::TxNone)
        return a;
    return a * combinedMatrix().inverted();
}

/*!
    \fn QPolygon QPainter::xFormDev(const QPolygon &polygon, int index, int count) const
    \overload

    Use combinedMatrix() combined with QPolygon::mid() and QMatrix::inverted() instead.

    \oldcode
        QPainter painter(this);
        QPolygon transformed = painter.xFormDev(polygon, index, count);
    \newcode
        QPainter painter(this);
        QPolygon transformed = polygon.mid(index, count) * painter.combinedMatrix().inverted();
    \endcode
*/

QPolygon QPainter::xFormDev(const QPolygon &ad, int index, int npoints) const
{
    Q_D(const QPainter);
    int lastPoint = npoints < 0 ? ad.size() : index+npoints;
    QPolygon a(lastPoint-index);
    memcpy(a.data(), ad.data()+index, (lastPoint-index)*sizeof(QPoint));
    if (d->state->txop == QTransform::TxNone)
        return a;
    return a * combinedMatrix().inverted();
}

/*!
    \fn void QPainter::drawCubicBezier(const QPolygon &controlPoints, int index)

    Draws a cubic Bezier curve defined by the \a controlPoints,
    starting at \a{controlPoints}\e{[index]} (\a index defaults to 0).
    Points after \a{controlPoints}\e{[index + 3]} are ignored. Nothing
    happens if there aren't enough control points.

    Use strokePath() instead.

    \oldcode
             QPainter painter(this);
             painter.drawCubicBezier(controlPoints, index)
    \newcode
             QPainterPath path;
             path.moveTo(controlPoints.at(index));
             path.cubicTo(controlPoints.at(index+1),
                                 controlPoints.at(index+2),
                                 controlPoints.at(index+3));

             QPainter painter(this);
             painter.strokePath(path, painter.pen());
    \endcode
*/
void QPainter::drawCubicBezier(const QPolygon &a, int index)
{
    if (!isActive())
        return;
    Q_D(QPainter);
    d->updateState(d->state);

    if ((int)a.size() - index < 4) {
        qWarning("QPainter::drawCubicBezier: Cubic Bezier needs 4 control "
                  "points");
        return;
    }

    QPainterPath path;
    path.moveTo(a.at(index));
    path.cubicTo(a.at(index+1), a.at(index+2), a.at(index+3));
    strokePath(path, d->state->pen);
}
#endif

struct QPaintDeviceRedirection
{
    QPaintDeviceRedirection() : device(0), replacement(0) {}
    QPaintDeviceRedirection(const QPaintDevice *device, QPaintDevice *replacement,
                            const QPoint& offset)
        : device(device), replacement(replacement), offset(offset) { }
    const QPaintDevice *device;
    QPaintDevice *replacement;
    QPoint offset;
    bool operator==(const QPaintDevice *pdev) const { return device == pdev; }
    Q_DUMMY_COMPARISON_OPERATOR(QPaintDeviceRedirection)
};

typedef QList<QPaintDeviceRedirection> QPaintDeviceRedirectionList;
Q_GLOBAL_STATIC(QPaintDeviceRedirectionList, globalRedirections)
Q_GLOBAL_STATIC(QMutex, globalRedirectionsMutex)

/*!
    \threadsafe

    Redirects all paint commands for the given paint \a device, to the
    \a replacement device. The optional point \a offset defines an
    offset within the source device.

    The redirection will not be effective until the begin() function
    has been called; make sure to call end() for the given \a
    device's painter (if any) before redirecting. Call
    restoreRedirected() to restore the previous redirection.

    In general, you'll probably find that calling
    QPixmap::grabWidget() or QPixmap::grabWindow() is an easier
    solution.

    \sa redirected(), restoreRedirected()
*/
void QPainter::setRedirected(const QPaintDevice *device,
                             QPaintDevice *replacement,
                             const QPoint &offset)
{
    Q_ASSERT(device != 0);

    if (device->devType() == QInternal::Widget) {
        const QWidgetPrivate *widgetPrivate = static_cast<const QWidget *>(device)->d_func();
        // This is the case when the widget is in a paint event.
        if (widgetPrivate->redirectDev)
            const_cast<QWidgetPrivate *>(widgetPrivate)->restoreRedirected();
    }

    QPoint roffset;
    QPaintDevice *rdev = redirected(replacement, &roffset);

    QMutexLocker locker(globalRedirectionsMutex());
    QPaintDeviceRedirectionList *redirections = globalRedirections();
    Q_ASSERT(redirections != 0);
    *redirections += QPaintDeviceRedirection(device, rdev ? rdev : replacement, offset + roffset);
}


/*!
    \threadsafe

    Restores the previous redirection for the given \a device after a
    call to setRedirected().

    \sa redirected()
 */
void QPainter::restoreRedirected(const QPaintDevice *device)
{
    Q_ASSERT(device != 0);
    QMutexLocker locker(globalRedirectionsMutex());
    QPaintDeviceRedirectionList *redirections = globalRedirections();
    Q_ASSERT(redirections != 0);
    for (int i = redirections->size()-1; i >= 0; --i) {
        if (redirections->at(i) == device) {
            redirections->removeAt(i);
            return;
        }
    }
}

/*!
    \threadsafe

    Returns the replacement for given \a device. The optional out
    parameter \a offset returns the offset within the replaced device.

    \sa setRedirected(), restoreRedirected()
*/
QPaintDevice *QPainter::redirected(const QPaintDevice *device, QPoint *offset)
{
    Q_ASSERT(device != 0);

    if (device->devType() == QInternal::Widget) {
        const QWidgetPrivate *widgetPrivate = static_cast<const QWidget *>(device)->d_func();
        if (widgetPrivate->redirectDev)
            return widgetPrivate->redirected(offset);
    }

    QMutexLocker locker(globalRedirectionsMutex());
    QPaintDeviceRedirectionList *redirections = globalRedirections();
    Q_ASSERT(redirections != 0);
    for (int i = redirections->size()-1; i >= 0; --i)
        if (redirections->at(i) == device) {
            if (offset)
                *offset = redirections->at(i).offset;
            return redirections->at(i).replacement;
        }
    if (offset)
        *offset = QPoint(0, 0);
    return 0;
}


void qt_painter_removePaintDevice(QPaintDevice *dev)
{
    QMutexLocker locker(globalRedirectionsMutex());
    if(QPaintDeviceRedirectionList *redirections = globalRedirections()) {
        for (int i = 0; i < redirections->size(); ) {
            if(redirections->at(i) == dev || redirections->at(i).replacement == dev)
                redirections->removeAt(i);
            else
                ++i;
        }
    }
}

void qt_format_text(const QFont &fnt, const QRectF &_r,
                    int tf, const QString& str, QRectF *brect,
                    int tabstops, int *ta, int tabarraylen,
                    QPainter *painter)
{
    qt_format_text(fnt, _r,
                    tf, 0, str, brect,
                    tabstops, ta, tabarraylen,
                    painter);
}
void qt_format_text(const QFont &fnt, const QRectF &_r,
                    int tf, const QTextOption *option, const QString& str, QRectF *brect,
                    int tabstops, int *, int tabarraylen,
                    QPainter *painter)
{
    // we need to copy r here to protect against the case (&r == brect).
    QRectF r(_r);

    bool dontclip  = (tf & Qt::TextDontClip);
    bool wordwrap  = (tf & Qt::TextWordWrap) || (tf & Qt::TextWrapAnywhere);
    bool singleline = (tf & Qt::TextSingleLine);
    bool showmnemonic = (tf & Qt::TextShowMnemonic);
    bool hidemnmemonic = (tf & Qt::TextHideMnemonic);

    Qt::LayoutDirection layout_direction;
    if(option)
        layout_direction = option->textDirection();
    else if (painter)
        layout_direction = painter->layoutDirection();
    else
        layout_direction = Qt::LeftToRight;

    tf = QStyle::visualAlignment(layout_direction, QFlag(tf));

    bool isRightToLeft = layout_direction == Qt::RightToLeft;
    bool expandtabs = ((tf & Qt::TextExpandTabs) &&
                        (((tf & Qt::AlignLeft) && !isRightToLeft) ||
                          ((tf & Qt::AlignRight) && isRightToLeft)));

    if (!painter)
        tf |= Qt::TextDontPrint;

    int maxUnderlines = 0;
    int numUnderlines = 0;
    int underlinePositionStack[32];
    int *underlinePositions = underlinePositionStack;

    QFontMetricsF fm(fnt);

    QString text = str;
    // compatible behaviour to the old implementation. Replace
    // tabs by spaces
    QChar *chr = text.data();
    const QChar *end = chr + str.length();
    bool has_tab = false;
    while (chr != end) {
        if (*chr == QLatin1Char('\r') || (singleline && *chr == QLatin1Char('\n'))) {
            *chr = QLatin1Char(' ');
        } else if (*chr == QLatin1Char('\n')) {
            *chr = QChar::LineSeparator;
        } else if (*chr == QLatin1Char('&')) {
            ++maxUnderlines;
        } else if (*chr == QLatin1Char('\t')) {
            has_tab = true;
        }
        ++chr;
    }
    if (has_tab) {
        if (!expandtabs) {
            chr = text.data();
            while (chr != end) {
                if (*chr == QLatin1Char('\t'))
                    *chr = QLatin1Char(' ');
                ++chr;
            }
        } else if (!tabarraylen && !tabstops) {
            tabstops = qRound(fm.width(QLatin1Char('x'))*8);
        }
    }

    if (hidemnmemonic || showmnemonic) {
        if (maxUnderlines > 32)
            underlinePositions = new int[maxUnderlines];
        QChar *cout = text.data();
        QChar *cin = cout;
        int l = str.length();
        while (l) {
            if (*cin == QLatin1Char('&')) {
                ++cin;
                --l;
                if (!l)
                    break;
                if (*cin != QLatin1Char('&') && !hidemnmemonic)
                    underlinePositions[numUnderlines++] = cout - text.unicode();
            }
            *cout = *cin;
            ++cout;
            ++cin;
            --l;
        }
        int newlen = cout - text.unicode();
        if (newlen != text.length())
            text.resize(newlen);
    }

    // no need to do extra work for underlines if we don't paint
    if (tf & Qt::TextDontPrint)
        numUnderlines = 0;

    underlinePositions[numUnderlines] = -1;
    qreal height = 0;
    qreal width = 0;

    QStackTextEngine engine(text, fnt);
    if (option)
        engine.option = *option;
    engine.option.setTextDirection(layout_direction);
    if (tf & Qt::AlignJustify)
        engine.option.setAlignment(Qt::AlignJustify);
    if (tf & Qt::TextWrapAnywhere)
        engine.option.setWrapMode(QTextOption::WrapAnywhere);
    if (tf & Qt::TextJustificationForced)
        engine.forceJustification = true;
    QTextLayout textLayout(&engine);
    textLayout.setCacheEnabled(true);
    textLayout.engine()->underlinePositions = underlinePositions;

    if (text.isEmpty()) {
        height = fm.height();
        width = 0;
        tf |= Qt::TextDontPrint;
    } else {
        qreal lineWidth = 0x01000000;
        if (wordwrap || (tf & Qt::TextJustificationForced))
            lineWidth = qMax<qreal>(0, r.width());
        if(!wordwrap)
            tf |= Qt::TextIncludeTrailingSpaces;
        textLayout.engine()->ignoreBidi = bool(tf & Qt::TextDontPrint);
        textLayout.beginLayout();

        qreal leading = fm.leading();
        height = -leading;

        while (1) {
            QTextLine l = textLayout.createLine();
            if (!l.isValid())
                break;

            l.setLineWidth(lineWidth);
            height += leading;
            l.setPosition(QPointF(0., height));
            height += l.height();
            width = qMax(width, l.naturalTextWidth());
            if (!brect && height >= r.height())
                break;
        }
        textLayout.endLayout();
    }

    qreal yoff = 0;
    qreal xoff = 0;
    if (tf & Qt::AlignBottom)
        yoff = r.height() - height;
    else if (tf & Qt::AlignVCenter)
        yoff = (r.height() - height)/2;
    if (tf & Qt::AlignRight)
        xoff = r.width() - width;
    else if (tf & Qt::AlignHCenter)
        xoff = (r.width() - width)/2;
    QRectF bounds = QRectF(r.x() + xoff, r.y() + yoff, width, height);
    if (brect)
        *brect = bounds;

    if (!(tf & Qt::TextDontPrint)) {
        bool restore = false;
        if (!dontclip && !r.contains(bounds)) {
            restore = true;
            painter->save();
            painter->setClipRect(r, Qt::IntersectClip);
        }

        for (int i = 0; i < textLayout.lineCount(); i++) {
            QTextLine line = textLayout.lineAt(i);

            if (tf & Qt::AlignRight)
                xoff = r.width() - line.naturalTextWidth();
            else if (tf & Qt::AlignHCenter)
                xoff = (r.width() - line.naturalTextWidth())/2;

            line.draw(painter, QPointF(r.x() + xoff + line.x(), r.y() + yoff));
        }

        if (restore) {
            painter->restore();
        }
    }

    if (underlinePositions != underlinePositionStack)
        delete [] underlinePositions;
}

/*!
    Sets the layout direction used by the painter when drawing text,
    to the specified \a direction.

    \sa layoutDirection(), drawText(), {QPainter#Settings}{Settings}
*/
void QPainter::setLayoutDirection(Qt::LayoutDirection direction)
{
    Q_D(QPainter);
    d->state->layoutDirection = direction;
}

/*!
    Returns the layout direction used by the painter when drawing text.

    \sa setLayoutDirection(), drawText(), {QPainter#Settings}{Settings}
*/
Qt::LayoutDirection QPainter::layoutDirection() const
{
    Q_D(const QPainter);
    return d->state->layoutDirection;
}

QPainterState::QPainterState(const QPainterState *s)
    : bgOrigin(s->bgOrigin), font(s->font), deviceFont(s->deviceFont),
      pen(s->pen), brush(s->brush), bgBrush(s->bgBrush),
      clipRegion(s->clipRegion), clipPath(s->clipPath),
      clipOperation(s->clipOperation),
      renderHints(s->renderHints), clipInfo(s->clipInfo),
      worldMatrix(s->worldMatrix), matrix(s->matrix), redirection_offset(s->redirection_offset), txop(s->txop),
      wx(s->wx), wy(s->wy), ww(s->ww), wh(s->wh),
      vx(s->vx), vy(s->vy), vw(s->vw), vh(s->vh),
      opacity(s->opacity), localOpacity(s->localOpacity), WxF(s->WxF), VxF(s->VxF),
      clipEnabled(s->clipEnabled), bgMode(s->bgMode), painter(s->painter),
      layoutDirection(s->layoutDirection),
      composition_mode(s->composition_mode),
      emulationSpecifier(s->emulationSpecifier), changeFlags(0)
{
    dirtyFlags = s->dirtyFlags;
}

QPainterState::QPainterState()
    : bgOrigin(0, 0), bgBrush(Qt::white), clipOperation(Qt::NoClip),
      renderHints(0),
      txop(0), wx(0), wy(0), ww(0), wh(0), vx(0), vy(0), vw(0), vh(0),
      opacity(1), localOpacity(1.0), WxF(false), VxF(false), clipEnabled(true),
      bgMode(Qt::TransparentMode), painter(0),
      layoutDirection(QApplication::layoutDirection()),
      composition_mode(QPainter::CompositionMode_SourceOver),
      emulationSpecifier(0), changeFlags(0)
{
    dirtyFlags = 0;
}

QPainterState::~QPainterState()
{
}

void QPainterState::init(QPainter *p) {
    bgBrush = Qt::white;
    bgMode = Qt::TransparentMode;
    WxF = false;
    VxF = false;
    clipEnabled = true;
    wx = wy = ww = wh = 0;
    vx = vy = vw = vh = 0;
    painter = p;
    pen = QPen();
    bgOrigin = QPointF(0, 0);
    brush = QBrush();
    font = deviceFont = QFont();
    clipRegion = QRegion();
    clipPath = QPainterPath();
    clipOperation = Qt::NoClip;
    clipInfo.clear();
    worldMatrix.reset();
    matrix.reset();
    txop = 0;
    layoutDirection = QApplication::layoutDirection();
    composition_mode = QPainter::CompositionMode_SourceOver;
    emulationSpecifier = 0;
    dirtyFlags = 0;
    changeFlags = 0;
    renderHints = 0;
    opacity = 1;
    localOpacity = 1.0;
}

#ifdef QT3_SUPPORT
static void bitBlt_helper(QPaintDevice *dst, const QPoint &dp,
                          const QPaintDevice *src, const QRect &sr, bool)
{
    Q_ASSERT(dst);
    Q_ASSERT(src);

    if (src->devType() == QInternal::Pixmap) {
        const QPixmap *pixmap = static_cast<const QPixmap *>(src);
        QPainter pt(dst);
        pt.drawPixmap(dp, *pixmap, sr);

    } else {
        qWarning("QPainter: bitBlt only works when source is of type pixmap");
    }
}

void bitBlt(QPaintDevice *dst, int dx, int dy,
             const QPaintDevice *src, int sx, int sy, int sw, int sh,
             bool ignoreMask )
{
    bitBlt_helper(dst, QPoint(dx, dy), src, QRect(sx, sy, sw, sh), ignoreMask);
}

void bitBlt(QPaintDevice *dst, const QPoint &dp, const QPaintDevice *src, const QRect &sr, bool ignoreMask)
{
    bitBlt_helper(dst, dp, src, sr, ignoreMask);
}

void bitBlt(QPaintDevice *dst, int dx, int dy,
            const QImage *src, int sx, int sy, int sw, int sh, int fl)
{
    Qt::ImageConversionFlags flags(fl);
    QPixmap srcPixmap = QPixmap::fromImage(*src, flags);
    bitBlt_helper(dst, QPoint(dx, dy), &srcPixmap, QRect(sx, sy, sw, sh), false);
}

#endif // QT3_SUPPORT

/*!
    \fn void QPainter::setBackgroundColor(const QColor &color)

    Use setBackground() instead.
*/

/*!
    \fn const QColor &QPainter::backgroundColor() const

    Use background() and QBrush::color() instead.

    \oldcode
        QColor myColor = backgroundColor();
    \newcode
        QColor myColor = background().color();
    \endcode

    Note that the background can be a complex brush such as a texture
    or a gradient.
*/

/*!
    \fn void QPainter::drawText(int x, int y, const QString &text, int pos, int length)
    \compat

    Use drawText() combined with QString::mid() instead.

    \oldcode
        QPainter painter(this);
        painter.drawText(x, y, text, pos, length);
    \newcode
        QPainter painter(this);
        painter.drawText(x, y, text.mid(pos, length));
    \endcode
*/

/*!
    \fn void QPainter::drawText(const QPoint &point, const QString &text, int pos, int length)
    \compat

    Use drawText() combined with QString::mid() instead.

    \oldcode
        QPainter painter(this);
        painter.drawText(point, text, pos, length);
    \newcode
        QPainter painter(this);
        painter.drawText(point, text.mid(pos, length));
    \endcode
*/

/*!
    \fn void QPainter::drawText(int x, int y, const QString &text, int length)
    \compat

    Use drawText() combined with QString::left() instead.

    \oldcode
        QPainter painter(this);
        painter.drawText(x, y, text, length);
    \newcode
        QPainter painter(this);
        painter.drawText(x, y, text.left(length));
    \endcode
*/

/*!
    \fn void QPainter::drawText(const QPoint &point, const QString &text, int length)
    \compat

    Use drawText() combined with QString::left() instead.

    \oldcode
        QPainter painter(this);
        painter.drawText(point, text, length);
    \newcode
        QPainter painter(this);
        painter.drawText(point, text.left(length));
    \endcode
*/

/*!
    \fn bool QPainter::begin(QPaintDevice *device, const QWidget *init)
    \compat

    Use begin() instead.

    If the paint \a device is a QWidget, QPainter is initialized after
    the widget's settings automatically. Otherwise, you must call the
    initFrom() function to initialize the painters pen, background and
    font to the same as any given widget.

    \oldcode
        QPainter painter(this);
        painter.begin(device, init);
    \newcode
        QPainter painter(this);
        painter.begin(device);
        painter.initFrom(init);
    \endcode
*/

/*!
    \fn void QPainter::drawImage(const QRectF &target, const QImage &image, const QRectF &source,
                         Qt::ImageConversionFlags flags)

    Draws the rectangular portion \a source of the given \a image
    into the \a target rectangle in the paint device.

    \note The image is scaled to fit the rectangle, if both the image and rectangle size disagree.

    If the image needs to be modified to fit in a lower-resolution
    result (e.g. converting from 32-bit to 8-bit), use the \a flags to
    specify how you would prefer this to happen.

    \table 100%
    \row
    \o
    \snippet doc/src/snippets/code/src.gui.painting.qpainter.cpp 20
    \endtable

    \sa drawPixmap()
*/

/*!
    \fn void QPainter::drawImage(const QRect &target, const QImage &image, const QRect &source,
                                 Qt::ImageConversionFlags flags)
    \overload

    Draws the rectangular portion \a source of the given \a image
    into the \a target rectangle in the paint device.

    \note The image is scaled to fit the rectangle, if both the image and rectangle size disagree.
*/

/*!
    \fn void QPainter::drawImage(const QPointF &point, const QImage &image)

    \overload

    Draws the given \a image at the given \a point.
*/

/*!
    \fn void QPainter::drawImage(const QPoint &point, const QImage &image)

    \overload

    Draws the given \a image at the given \a point.
*/

/*!
    \fn void QPainter::drawImage(const QPointF &point, const QImage &image, const QRectF &source,
                                 Qt::ImageConversionFlags flags = 0)

    \overload

    Draws the rectangular portion \a source of the given \a image with
    its origin at the given \a point.
*/

/*!
    \fn void QPainter::drawImage(const QPoint &point, const QImage &image, const QRect &source,
                                 Qt::ImageConversionFlags flags = 0)
    \overload

    Draws the rectangular portion \a source of the given \a image with
    its origin at the given \a point.
*/

/*!
    \fn void QPainter::drawImage(const QRectF &rectangle, const QImage &image)

    \overload

    Draws the given \a image into the given \a rectangle.

    \note The image is scaled to fit the rectangle, if both the image and rectangle size disagree.
*/

/*!
    \fn void QPainter::drawImage(const QRect &rectangle, const QImage &image)

    \overload

    Draws the given \a image into the given \a rectangle.

   \note The image is scaled to fit the rectangle, if both the image and rectangle size disagree.
*/

/*!
    \fn void QPainter::drawImage(int x, int y, const QImage &image,
                                 int sx, int sy, int sw, int sh,
                                 Qt::ImageConversionFlags flags)
    \overload

    Draws an image at (\a{x}, \a{y}) by copying a part of \a image into
    the paint device.

    (\a{x}, \a{y}) specifies the top-left point in the paint device that is
    to be drawn onto. (\a{sx}, \a{sy}) specifies the top-left point in \a
    image that is to be drawn. The default is (0, 0).

    (\a{sw}, \a{sh}) specifies the size of the image that is to be drawn.
    The default, (0, 0) (and negative) means all the way to the
    bottom-right of the image.
*/

/*!
    \fn void QPainter::redirect(QPaintDevice *pdev, QPaintDevice *replacement)

    Use setRedirected() instead.
*/

/*!
    \fn QPaintDevice *QPainter::redirect(QPaintDevice *pdev)

    Use redirected() instead.
*/

/*!
    \fn QRect QPainter::boundingRect(const QRect &rectangle, int flags,
                                     const QString &text, int length)
    \compat

    Returns the bounding rectangle for the given \a length of the \a
    text constrained by the provided \a rectangle.

    Use boundingRect() combined with QString::left() instead.

    \oldcode
        QRect rectangle = boundingRect(rect, flags, text, length);
    \newcode
        QRect rectangle = boundingRect(rect, flags, text.left(length));
    \endcode
*/

/*!
    \fn void QPainter::drawText(const QRect &rectangle, int flags, const QString &text,
                                int length, QRect *br)
    \compat

    Use drawText() combined with QString::left() instead.

    \oldcode
        QPainter painter(this);
        painter.drawText(rectangle, flags, text, length, br );
    \newcode
        QPainter painter(this);
        painter.drawText(rectangle, flags, text.left(length), br );
    \endcode
*/

/*!
    \fn QRect QPainter::boundingRect(int x, int y, int width, int height, int flags,
                                     const QString &text, int length);

    \compat

    Returns the bounding rectangle for the given \a length of the \a
    text constrained by the rectangle that begins at point (\a{x},
    \a{y}) with the given \a width and \a height.

    Use boundingRect() combined with QString::left() instead.

    \oldcode
        QRect rectangle = boundingRect(x, y, width, height, flags, text, length);
    \newcode
        QRect rectangle = boundingRect(x, y, width, height, flags, text.left(length));
    \endcode
*/

/*!
    \fn void QPainter::drawText(int x, int y, int width, int height, int flags,
                                const QString &text, int length, QRect *br)

    \compat

    Use drawText() combined with QString::left() instead.

    \oldcode
        QPainter painter(this);
        painter.drawText(x, y, width, height, flags, text, length, br );
    \newcode
        QPainter painter(this);
        painter.drawText(x, y, width, height, flags, text.left(length), br );
    \endcode
*/


/*!
    \class QPaintEngineState
    \since 4.1

    \brief The QPaintEngineState class provides information about the
    active paint engine's current state.
    \reentrant

    QPaintEngineState records which properties that have changed since
    the last time the paint engine was updated, as well as their
    current value.

    Which properties that have changed can at any time be retrieved
    using the state() function. This function returns an instance of
    the QPaintEngine::DirtyFlags type which stores an OR combination
    of QPaintEngine::DirtyFlag values. The QPaintEngine::DirtyFlag
    enum defines whether a property has changed since the last update
    or not.

    If a property is marked with a dirty flag, its current value can
    be retrieved using the corresponding get function:

    \target GetFunction

    \table
    \header \o Property Flag \o Current Property Value
    \row \o QPaintEngine::DirtyBackground \o backgroundBrush()
    \row \o QPaintEngine::DirtyBackgroundMode \o backgroundMode()
    \row \o QPaintEngine::DirtyBrush \o brush()
    \row \o QPaintEngine::DirtyBrushOrigin \o brushOrigin()
    \row \o QPaintEngine::DirtyClipRegion \e or QPaintEngine::DirtyClipPath
         \o clipOperation()
    \row \o QPaintEngine::DirtyClipPath \o clipPath()
    \row \o QPaintEngine::DirtyClipRegion \o clipRegion()
    \row \o QPaintEngine::DirtyCompositionMode \o compositionMode()
    \row \o QPaintEngine::DirtyFont \o font()
    \row \o QPaintEngine::DirtyTransform \o matrix()
    \row \o QPaintEngine::DirtyClipEnabled \o isClipEnabled()
    \row \o QPaintEngine::DirtyPen \o pen()
    \row \o QPaintEngine::DirtyHints \o renderHints()
    \endtable

    The QPaintEngineState class also provide the painter() function
    which returns a pointer to the painter that is currently updating
    the paint engine.

    An instance of this class, representing the current state of the
    active paint engine, is passed as argument to the
    QPaintEngine::updateState() function. The only situation in which
    you will have to use this class directly is when implementing your
    own paint engine.

    \sa QPaintEngine
*/


/*!
    \fn QPaintEngine::DirtyFlags QPaintEngineState::state() const

    Returns a combination of flags identifying the set of properties
    that need to be updated when updating the paint engine's state
    (i.e. during a call to the QPaintEngine::updateState() function).

    \sa QPaintEngine::updateState()
*/


/*!
    Returns the pen in the current paint engine state.

    This variable should only be used when the state() returns a
    combination which includes the QPaintEngine::DirtyPen flag.

    \sa state(), QPaintEngine::updateState()
*/

QPen QPaintEngineState::pen() const
{
    return static_cast<const QPainterState *>(this)->pen;
}

/*!
    Returns the brush in the current paint engine state.

    This variable should only be used when the state() returns a
    combination which includes the QPaintEngine::DirtyBrush flag.

    \sa state(), QPaintEngine::updateState()
*/

QBrush QPaintEngineState::brush() const
{
    return static_cast<const QPainterState *>(this)->brush;
}

/*!
    Returns the brush origin in the current paint engine state.

    This variable should only be used when the state() returns a
    combination which includes the QPaintEngine::DirtyBrushOrigin flag.

    \sa state(), QPaintEngine::updateState()
*/

QPointF QPaintEngineState::brushOrigin() const
{
    return static_cast<const QPainterState *>(this)->bgOrigin;
}

/*!
    Returns the background brush in the current paint engine state.

    This variable should only be used when the state() returns a
    combination which includes the QPaintEngine::DirtyBackground flag.

    \sa state(), QPaintEngine::updateState()
*/

QBrush QPaintEngineState::backgroundBrush() const
{
    return static_cast<const QPainterState *>(this)->bgBrush;
}

/*!
    Returns the background mode in the current paint engine
    state.

    This variable should only be used when the state() returns a
    combination which includes the QPaintEngine::DirtyBackgroundMode flag.

    \sa state(), QPaintEngine::updateState()
*/

Qt::BGMode QPaintEngineState::backgroundMode() const
{
    return static_cast<const QPainterState *>(this)->bgMode;
}

/*!
    Returns the font in the current paint engine
    state.

    This variable should only be used when the state() returns a
    combination which includes the QPaintEngine::DirtyFont flag.

    \sa state(), QPaintEngine::updateState()
*/

QFont QPaintEngineState::font() const
{
    return static_cast<const QPainterState *>(this)->font;
}

/*!
    \since 4.2

    Returns the matrix in the current paint engine
    state.

    This variable should only be used when the state() returns a
    combination which includes the QPaintEngine::DirtyTransform flag.

    \sa state(), QPaintEngine::updateState()
*/

QMatrix QPaintEngineState::matrix() const
{
    const QPainterState *st = static_cast<const QPainterState *>(this);

    return st->matrix.toAffine();
}

/*!
    \since 4.3

    Returns the matrix in the current paint engine state.

    This variable should only be used when the state() returns a
    combination which includes the QPaintEngine::DirtyTransform flag.

    \sa state(), QPaintEngine::updateState()
*/


QTransform QPaintEngineState::transform() const
{
    const QPainterState *st = static_cast<const QPainterState *>(this);

    return st->matrix;
}


/*!
    Returns the clip operation in the current paint engine
    state.

    This variable should only be used when the state() returns a
    combination which includes either the QPaintEngine::DirtyClipPath
    or the QPaintEngine::DirtyClipRegion flag.

    \sa state(), QPaintEngine::updateState()
*/

Qt::ClipOperation QPaintEngineState::clipOperation() const
{
    return static_cast<const QPainterState *>(this)->clipOperation;
}

/*!
    \since 4.3

    Returns whether the coordinate of the fill have been specified
    as bounded by the current rendering operation and have to be
    resolved (about the currently rendered primitive).
*/
bool QPaintEngineState::brushNeedsResolving() const
{
    const QBrush &brush = static_cast<const QPainterState *>(this)->brush;
    return needsResolving(brush);
}


/*!
    \since 4.3

    Returns whether the coordinate of the stroke have been specified
    as bounded by the current rendering operation and have to be
    resolved (about the currently rendered primitive).
*/
bool QPaintEngineState::penNeedsResolving() const
{
    const QPen &pen = static_cast<const QPainterState *>(this)->pen;
    return needsResolving(pen.brush());
}

/*!
    Returns the clip region in the current paint engine state.

    This variable should only be used when the state() returns a
    combination which includes the QPaintEngine::DirtyClipRegion flag.

    \sa state(), QPaintEngine::updateState()
*/

QRegion QPaintEngineState::clipRegion() const
{
    return static_cast<const QPainterState *>(this)->clipRegion;
}

/*!
    Returns the clip path in the current paint engine state.

    This variable should only be used when the state() returns a
    combination which includes the QPaintEngine::DirtyClipPath flag.

    \sa state(), QPaintEngine::updateState()
*/

QPainterPath QPaintEngineState::clipPath() const
{
    return static_cast<const QPainterState *>(this)->clipPath;
}

/*!
    Returns wether clipping is enabled or not in the current paint
    engine state.

    This variable should only be used when the state() returns a
    combination which includes the QPaintEngine::DirtyClipEnabled
    flag.

    \sa state(), QPaintEngine::updateState()
*/

bool QPaintEngineState::isClipEnabled() const
{
    return static_cast<const QPainterState *>(this)->clipEnabled;
}

/*!
    Returns the render hints in the current paint engine state.

    This variable should only be used when the state() returns a
    combination which includes the QPaintEngine::DirtyHints
    flag.

    \sa state(), QPaintEngine::updateState()
*/

QPainter::RenderHints QPaintEngineState::renderHints() const
{
    return static_cast<const QPainterState *>(this)->renderHints;
}

/*!
    Returns the composition mode in the current paint engine state.

    This variable should only be used when the state() returns a
    combination which includes the QPaintEngine::DirtyCompositionMode
    flag.

    \sa state(), QPaintEngine::updateState()
*/

QPainter::CompositionMode QPaintEngineState::compositionMode() const
{
    return static_cast<const QPainterState *>(this)->composition_mode;
}


/*!
    Returns a pointer to the painter currently updating the paint
    engine.
*/

QPainter *QPaintEngineState::painter() const
{
    return static_cast<const QPainterState *>(this)->painter;
}


/*!
    \since 4.2

    Returns the opacity in the current paint engine state.
*/

qreal QPaintEngineState::opacity() const
{
    return static_cast<const QPainterState *>(this)->opacity;
}

/*!
    \since 4.3

    Sets the world transformation matrix.
    If \a combine is true, the specified \a transform is combined with
    the current matrix; otherwise it replaces the current matrix.

    This function has been added for compatibility with setMatrix(),
    but as with setMatrix() the preferred method of setting a
    transformation on the painter is through setWorldTransform().

    \sa transform()
*/

void QPainter::setTransform(const QTransform &transform, bool combine )
{
    setWorldTransform(transform, combine);
}

/*!
    Returns the world transformation matrix.
*/

const QTransform & QPainter::transform() const
{
    return worldTransform();
}


/*!
    Returns the matrix that transforms from logical coordinates to
    device coordinates of the platform dependent paint device.

    This function is \e only needed when using platform painting
    commands on the platform dependent handle (Qt::HANDLE), and the
    platform does not do transformations nativly.

    The QPaintEngine::PaintEngineFeature enum can be queried to
    determine whether the platform performs the transformations or
    not.

    \sa worldTransform(), QPaintEngine::hasFeature(),
*/

const QTransform & QPainter::deviceTransform() const
{
    Q_D(const QPainter);
    return d->state->matrix;
}


/*!
    Resets any transformations that were made using translate(),
    scale(), shear(), rotate(), setWorldTransform(), setViewport()
    and setWindow().

    \sa {Coordinate Transformations}
*/

void QPainter::resetTransform()
{
     Q_D(QPainter);
#ifdef QT_DEBUG_DRAW
    if (qt_show_painter_debug_output)
        printf("QPainter::resetMatrix()\n");
#endif
    if (!isActive()) {
        qWarning("QPainter::resetMatrix: Painter not active");
        return;
    }

    d->state->wx = d->state->wy = d->state->vx = d->state->vy = 0;                        // default view origins
    d->state->ww = d->state->vw = d->device->metric(QPaintDevice::PdmWidth);
    d->state->wh = d->state->vh = d->device->metric(QPaintDevice::PdmHeight);
    d->state->worldMatrix = QTransform();
    setMatrixEnabled(false);
    setViewTransformEnabled(false);
    if (d->engine)
        d->engine->setDirty(QPaintEngine::DirtyTransform);
}

/*!
    Sets the world transformation matrix.
    If \a combine is true, the specified \a matrix is combined with the current matrix;
    otherwise it replaces the current matrix.

    \sa transform(), setTransform()
*/

void QPainter::setWorldTransform(const QTransform &matrix, bool combine )
{
    Q_D(QPainter);

   if (!isActive()) {
        qWarning("QPainter::setWorldTransform: Painter not active");
        return;
    }

    if (d->refcount > 2)
        combine = true;

    if (combine)
        d->state->worldMatrix = matrix * d->state->worldMatrix;                        // combines
    else
        d->state->worldMatrix = matrix;                                // set new matrix

    if (!d->state->WxF)
        setMatrixEnabled(true);
    else
        d->updateMatrix();
}

/*!
    Returns the world transformation matrix.
*/

const QTransform & QPainter::worldTransform() const
{

    Q_D(const QPainter);
    return d->state->worldMatrix;
}

/*!
    Returns the transformation matrix combining the current
    window/viewport and world transformation.

    \sa setWorldMatrix(), setWindow(), setViewport()
*/

QTransform QPainter::combinedTransform() const
{
    Q_D(const QPainter);
    return d->state->worldMatrix * d->viewTransform();
}

QT_END_NAMESPACE
