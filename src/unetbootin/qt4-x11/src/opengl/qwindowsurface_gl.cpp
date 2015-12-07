/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtOpenGL module of the Qt Toolkit.
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

#include <QtGui/QApplication>
#include <QtGui/QColormap>
#include <QtGui/QDesktopWidget>
#include <QtGui/QPaintDevice>
#include <QtGui/QWidget>

#include <qglframebufferobject.h>
#include <qglpixelbuffer.h>
#include <qcolormap.h>
#include <qdesktopwidget.h>
#include "qdebug.h"

#ifdef Q_WS_X11
#include <private/qt_x11_p.h>
#include <qx11info_x11.h>
#include <private/qwidget_p.h>

#ifndef QT_OPENGL_ES
#include <GL/glx.h>
#include <X11/Xlib.h>
#endif
#endif //Q_WS_X11

#include <private/qglextensions_p.h>
#include <private/qwindowsurface_gl_p.h>

#include <private/qgl_p.h>

#include <private/qglpixelbuffer_p.h>
#include <private/qgraphicssystem_gl_p.h>
#include <private/qpaintengine_opengl_p.h>

#ifndef GLX_ARB_multisample
#define GLX_SAMPLE_BUFFERS_ARB  100000
#define GLX_SAMPLES_ARB         100001
#endif

QT_BEGIN_NAMESPACE

//
// QGLGraphicsSystem
//

QGLGraphicsSystem::QGLGraphicsSystem()
    : QGraphicsSystem()
{
#if defined(Q_WS_X11) && !defined(QT_OPENGL_ES)
    // only override the system defaults if the user hasn't already
    // picked a visual
    if (X11->visual == 0 && X11->visual_id == -1 && X11->visual_class == -1) {
        // find a double buffered, RGBA visual that supports OpenGL
        // and set that as the default visual for windows in Qt
        int i = 0;
        int spec[16];
        spec[i++] = GLX_RGBA;
#if 0
        spec[i++] = GLX_DOUBLEBUFFER;
        spec[i++] = GLX_DEPTH_SIZE;
        spec[i++] = 8;
        spec[i++] = GLX_STENCIL_SIZE;
        spec[i++] = 8;
        spec[i++] = GLX_SAMPLE_BUFFERS_ARB;
        spec[i++] = 1;
        spec[i++] = GLX_SAMPLES_ARB;
        spec[i++] = 4;
#endif
        spec[i++] = XNone;

        XVisualInfo *vi = glXChooseVisual(X11->display, X11->defaultScreen, spec);
        if (vi) {
            X11->visual_id = vi->visualid;
            X11->visual_class = vi->c_class;

            QGLFormat format;
            int res;
            glXGetConfig(X11->display, vi, GLX_LEVEL, &res);
            format.setPlane(res);
            glXGetConfig(X11->display, vi, GLX_DOUBLEBUFFER, &res);
            format.setDoubleBuffer(res);
            glXGetConfig(X11->display, vi, GLX_DEPTH_SIZE, &res);
            format.setDepth(res);
            if (format.depth())
                format.setDepthBufferSize(res);
            glXGetConfig(X11->display, vi, GLX_RGBA, &res);
            format.setRgba(res);
            glXGetConfig(X11->display, vi, GLX_RED_SIZE, &res);
            format.setRedBufferSize(res);
            glXGetConfig(X11->display, vi, GLX_GREEN_SIZE, &res);
            format.setGreenBufferSize(res);
            glXGetConfig(X11->display, vi, GLX_BLUE_SIZE, &res);
            format.setBlueBufferSize(res);
            glXGetConfig(X11->display, vi, GLX_ALPHA_SIZE, &res);
            format.setAlpha(res);
            if (format.alpha())
                format.setAlphaBufferSize(res);
            glXGetConfig(X11->display, vi, GLX_ACCUM_RED_SIZE, &res);
            format.setAccum(res);
            if (format.accum())
                format.setAccumBufferSize(res);
            glXGetConfig(X11->display, vi, GLX_STENCIL_SIZE, &res);
            format.setStencil(res);
            if (format.stencil())
                format.setStencilBufferSize(res);
            glXGetConfig(X11->display, vi, GLX_STEREO, &res);
            format.setStereo(res);
            glXGetConfig(X11->display, vi, GLX_SAMPLE_BUFFERS_ARB, &res);
            format.setSampleBuffers(res);
            if (format.sampleBuffers()) {
                glXGetConfig(X11->display, vi, GLX_SAMPLES_ARB, &res);
                format.setSamples(res);
            }

            QGLWindowSurface::surfaceFormat = format;
            XFree(vi);

            printf("using visual class %x, id %x\n", X11->visual_class, X11->visual_id);
        }
    }
#elif defined(Q_WS_WIN)
    QGLWindowSurface::surfaceFormat.setDoubleBuffer(false);

    Q_GUI_EXPORT bool qt_win_owndc_required;
    qt_win_owndc_required = true;
#endif
}

//
// QGLWindowSurface
//

class QGLGlobalShareWidget
{
public:
    QGLGlobalShareWidget() : widget(0) {}

    QGLWidget *shareWidget() {
        if (!widget && !cleanedUp) {
            widget = new QGLWidget;
        }
        return widget;
    }

    void cleanup() {
        delete widget;
        widget = 0;
        cleanedUp = true;
    }

    static bool cleanedUp;

private:
    QGLWidget *widget;
};

bool QGLGlobalShareWidget::cleanedUp = false;

static void qt_cleanup_gl_share_widget();
Q_GLOBAL_STATIC_WITH_INITIALIZER(QGLGlobalShareWidget, _qt_gl_share_widget,
                                 {
                                     qAddPostRoutine(qt_cleanup_gl_share_widget);
                                 })

static void qt_cleanup_gl_share_widget()
{
    _qt_gl_share_widget()->cleanup();
}

QGLWidget* qt_gl_share_widget()
{
    if (QGLGlobalShareWidget::cleanedUp)
        return 0;
    return _qt_gl_share_widget()->shareWidget();
}

struct QGLWindowSurfacePrivate
{
    QGLFramebufferObject *fbo;
    QGLPixelBuffer *pb;
    GLuint tex_id;
    GLuint pb_tex_id;

    int tried_fbo : 1;
    int tried_pb : 1;

    QGLContext *ctx;

    QList<QGLContext **> contexts;

    QRegion paintedRegion;
    QSize size;

    QList<QImage> buffers;
};

QGLFormat QGLWindowSurface::surfaceFormat;

QGLWindowSurface::QGLWindowSurface(QWidget *window)
    : QWindowSurface(window), d_ptr(new QGLWindowSurfacePrivate)
{
    Q_ASSERT(window->isTopLevel());
    QGLExtensions::init();
    d_ptr->pb = 0;
    d_ptr->fbo = 0;
    d_ptr->ctx = 0;
    d_ptr->tried_fbo = false;
    d_ptr->tried_pb = false;
}

QGLWindowSurface::~QGLWindowSurface()
{
    if (d_ptr->ctx)
        glDeleteTextures(1, &d_ptr->tex_id);
    foreach(QGLContext **ctx, d_ptr->contexts) {
        delete *ctx;
        *ctx = 0;
    }

    delete d_ptr->pb;
    delete d_ptr->fbo;
    delete d_ptr;
}

void QGLWindowSurface::hijackWindow(QWidget *widget)
{
    QWidgetPrivate *widgetPrivate = widget->d_func();
    widgetPrivate->createExtra();
    if (widgetPrivate->extraData()->glContext)
        return;

    QGLContext *ctx = new QGLContext(surfaceFormat, widget);
    ctx->create(qt_gl_share_widget()->context());
#ifdef Q_WS_MAC
    ctx->updatePaintDevice();
#endif
    widgetPrivate->extraData()->glContext = ctx;

    union { QGLContext **ctxPtr; void **voidPtr; };

    voidPtr = &widgetPrivate->extraData()->glContext;
    d_ptr->contexts << ctxPtr;
    qDebug() << "hijackWindow() context created for" << widget << d_ptr->contexts.size();
}

#if !defined(QT_OPENGL_ES_2)
Q_GLOBAL_STATIC(QOpenGLPaintEngine, qt_gl_window_surface_paintengine)
#endif

/*! \reimp */
QPaintEngine *QGLWindowSurface::paintEngine() const
{
#if !defined(QT_OPENGL_ES_2)
    return qt_gl_window_surface_paintengine();
#else
    return 0;
#endif
}

int QGLWindowSurface::metric(PaintDeviceMetric m) const
{
    return window()->metric(m);
}

QGLContext *QGLWindowSurface::context() const
{
    return d_ptr->ctx;
}

QPaintDevice *QGLWindowSurface::paintDevice()
{
    if (d_ptr->pb)
        return d_ptr->pb;

    if (d_ptr->ctx)
        return this;

    QGLContext *ctx = reinterpret_cast<QGLContext *>(window()->d_func()->extraData()->glContext);
    ctx->makeCurrent();
    return d_ptr->fbo;
}

static void drawTexture(const QRectF &rect, GLuint tex_id, const QSize &texSize, const QRectF &src = QRectF());

void QGLWindowSurface::beginPaint(const QRegion &)
{
}

void QGLWindowSurface::endPaint(const QRegion &rgn)
{
    if (context())
        d_ptr->paintedRegion |= rgn;

    d_ptr->buffers.clear();
}

void QGLWindowSurface::flush(QWidget *widget, const QRegion &rgn, const QPoint &offset)
{
    QWidget *parent = widget->internalWinId() ? widget : widget->nativeParentWidget();
    Q_ASSERT(parent);

    hijackWindow(parent);

    QRect br = rgn.boundingRect().translated(offset);
    br = br.intersected(window()->rect());
    QPoint wOffset = qt_qwidget_data(parent)->wrect.topLeft();
    QRect rect = br.translated(-offset - wOffset);

    const GLenum target = qt_gl_preferredTextureTarget();

    if (context()) {
        context()->makeCurrent();

        if (context()->format().doubleBuffer()) {
            glBindTexture(target, d_ptr->tex_id);

            QVector<QRect> rects = d_ptr->paintedRegion.rects();
            for (int i = 0; i < rects.size(); ++i) {
                QRect br = rects.at(i);
                if (br.isEmpty())
                    continue;

                const uint bottom = window()->height() - (br.y() + br.height());
                glCopyTexSubImage2D(target, 0, br.x(), bottom, br.x(), bottom, br.width(), br.height());
            }

            glBindTexture(target, 0);

            QRegion dirtyRegion = QRegion(window()->rect()) - d_ptr->paintedRegion;

            if (!dirtyRegion.isEmpty()) {
                context()->makeCurrent();

                glMatrixMode(GL_MODELVIEW);
                glLoadIdentity();

                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();
#ifndef QT_OPENGL_ES
                glOrtho(0, window()->width(), window()->height(), 0, -999999, 999999);
#else
                glOrthof(0, window()->width(), window()->height(), 0, -999999, 999999);
#endif
                glViewport(0, 0, window()->width(), window()->height());

                QVector<QRect> rects = dirtyRegion.rects();
                glColor4f(1, 1, 1, 1);
                for (int i = 0; i < rects.size(); ++i) {
                    QRect rect = rects.at(i);
                    if (rect.isEmpty())
                        continue;

                    drawTexture(rect, d_ptr->tex_id, window()->size(), rect);
                }
            }
            d_ptr->paintedRegion = QRegion();

            context()->swapBuffers();
        } else {
            glFlush();
        }

        return;
    }

    QGLContext *ctx = reinterpret_cast<QGLContext *>(parent->d_func()->extraData()->glContext);
    GLuint texture;
    if (d_ptr->fbo) {
        texture = d_ptr->fbo->texture();
    } else {
        d_ptr->pb->makeCurrent();
        glBindTexture(target, d_ptr->pb_tex_id);
        const uint bottom = window()->height() - (br.y() + br.height());
        glCopyTexSubImage2D(target, 0, br.x(), bottom, br.x(), bottom, br.width(), br.height());
        texture = d_ptr->pb_tex_id;
        glBindTexture(target, 0);
    }

    QSize size = widget->rect().size();
    if (ctx->format().doubleBuffer()) {
        rect = parent->rect();
        br = rect.translated(wOffset);
        size = parent->size();
    }

    ctx->makeCurrent();
#ifdef Q_WS_MAC
    ctx->updatePaintDevice();
#endif
    if (d_ptr->fbo)
        d_ptr->fbo->release();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
#ifndef QT_OPENGL_ES
    glOrtho(0, size.width(), size.height(), 0, -999999, 999999);
#else
    glOrthof(0, size.width(), size.height(), 0, -999999, 999999);
#endif
    glViewport(0, 0, size.width(), size.height());

    glColor4f(1, 1, 1, 1);
    drawTexture(rect, texture, window()->size(), br);

    if (ctx->format().doubleBuffer())
        ctx->swapBuffers();
    else
        glFlush();

    if (d_ptr->fbo)
        d_ptr->fbo->bind();
}

void QGLWindowSurface::setGeometry(const QRect &rect)
{
    QWindowSurface::setGeometry(rect);

    const GLenum target = qt_gl_preferredTextureTarget();

    if (rect.width() <= 0 || rect.height() <= 0)
        return;

    if (d_ptr->size == rect.size())
        return;

    d_ptr->size = rect.size();

    if (d_ptr->ctx) {
        glBindTexture(target, d_ptr->tex_id);
        glTexImage2D(target, 0, GL_RGBA, rect.width(), rect.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
        glBindTexture(target, 0);
        return;
    }

    if (d_ptr->pb || !d_ptr->tried_pb) {
        d_ptr->tried_pb = true;

        if (d_ptr->pb) {
            d_ptr->pb->makeCurrent();
            glDeleteTextures(1, &d_ptr->pb_tex_id);
        }

        delete d_ptr->pb;

        d_ptr->pb = new QGLPixelBuffer(rect.width(), rect.height(),
                                        QGLFormat(QGL::SampleBuffers | QGL::StencilBuffer | QGL::DepthBuffer),
                                        qt_gl_share_widget());

        if (d_ptr->pb->isValid()) {
            qDebug() << "PB Sample buffers:" << d_ptr->pb->format().sampleBuffers();
            d_ptr->pb->makeCurrent();

            glGenTextures(1, &d_ptr->pb_tex_id);
            glBindTexture(target, d_ptr->pb_tex_id);
            glTexImage2D(target, 0, GL_RGBA, rect.width(), rect.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

            glTexParameterf(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameterf(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glBindTexture(target, 0);

            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
#ifndef QT_OPENGL_ES
            glOrtho(0, d_ptr->pb->width(), d_ptr->pb->height(), 0, -999999, 999999);
#else
            glOrthof(0, d_ptr->pb->width(), d_ptr->pb->height(), 0, -999999, 999999);
#endif

            d_ptr->pb->d_ptr->qctx->d_func()->internal_context = true;
            return;
        } else {
            qDebug() << "QGLWindowSurface: Failed to create valid pixelbuffer, falling back";
            delete d_ptr->pb;
            d_ptr->pb = 0;
        }
    }

    if ((QGLExtensions::glExtensions & QGLExtensions::FramebufferObject) && (d_ptr->fbo || !d_ptr->tried_fbo)) {
        d_ptr->tried_fbo = true;
        hijackWindow(window());
        QGLContext *ctx = reinterpret_cast<QGLContext *>(window()->d_func()->extraData()->glContext);
        ctx->d_ptr->internal_context = true;
        ctx->makeCurrent();
        delete d_ptr->fbo;
        d_ptr->fbo = new QGLFramebufferObject(rect.size(), QGLFramebufferObject::CombinedDepthStencil,
                                              GLenum(target), GLenum(GL_RGBA));

        d_ptr->fbo->bind();
        if (d_ptr->fbo->isValid()) {
            return;
        } else {
            qDebug() << "QGLWindowSurface: Failed to create valid FBO, falling back";
            delete d_ptr->fbo;
            d_ptr->fbo = 0;
        }
    }

    hijackWindow(window());
    QGLContext *ctx = reinterpret_cast<QGLContext *>(window()->d_func()->extraData()->glContext);
    ctx->makeCurrent();

    glGenTextures(1, &d_ptr->tex_id);
    glBindTexture(target, d_ptr->tex_id);
    glTexImage2D(target, 0, GL_RGBA, rect.width(), rect.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

    glTexParameterf(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glBindTexture(target, 0);

    qDebug() << "QGLWindowSurface: Using plain widget as window surface" << this;;
    d_ptr->ctx = ctx;
    d_ptr->ctx->d_ptr->internal_context = true;
}

bool QGLWindowSurface::scroll(const QRegion &area, int dx, int dy)
{
    // this code randomly fails currently for unknown reasons
    return false;

    if (!d_ptr->pb)
        return false;

    d_ptr->pb->makeCurrent();

    QRect br = area.boundingRect();

#if 0
    // ## workaround driver issue (scrolling by these deltas is unbearably slow for some reason)
    // ## maybe we should use glCopyTexSubImage insteadk
    if (dx == 1 || dx == -1 || dy == 1 || dy == -1 || dy == 2)
        return false;

    glRasterPos2i(br.x() + dx, br.y() + br.height() + dy);
    glCopyPixels(br.x(), d_ptr->pb->height() - (br.y() + br.height()), br.width(), br.height(), GL_COLOR);
    return true;
#endif

    const GLenum target = qt_gl_preferredTextureTarget();

    glBindTexture(target, d_ptr->tex_id);
    glCopyTexImage2D(target, 0, GL_RGBA, br.x(), d_ptr->pb->height() - (br.y() + br.height()), br.width(), br.height(), 0);
    glBindTexture(target, 0);

    drawTexture(br.translated(dx, dy), d_ptr->tex_id, window()->size());

    return true;
}

static void drawTexture(const QRectF &rect, GLuint tex_id, const QSize &texSize, const QRectF &br)
{
    const GLenum target = qt_gl_preferredTextureTarget();
    QRectF src = br.isEmpty()
        ? QRectF(QPointF(), texSize)
        : QRectF(QPointF(br.x(), texSize.height() - br.bottom()), br.size());

    if (target == GL_TEXTURE_2D) {
        qreal width = texSize.width();
        qreal height = texSize.height();

        src.setLeft(src.left() / width);
        src.setRight(src.right() / width);
        src.setTop(src.top() / height);
        src.setBottom(src.bottom() / height);
    }

    const q_vertexType tx1 = f2vt(src.left());
    const q_vertexType tx2 = f2vt(src.right());
    const q_vertexType ty1 = f2vt(src.top());
    const q_vertexType ty2 = f2vt(src.bottom());

    q_vertexType texCoordArray[4*2] = {
        tx1, ty2, tx2, ty2, tx2, ty1, tx1, ty1
    };

    q_vertexType vertexArray[4*2];
    extern void qt_add_rect_to_array(const QRectF &r, q_vertexType *array); // qpaintengine_opengl.cpp
    qt_add_rect_to_array(rect, vertexArray);

    glVertexPointer(2, q_vertexTypeEnum, 0, vertexArray);
    glTexCoordPointer(2, q_vertexTypeEnum, 0, texCoordArray);

    glBindTexture(target, tex_id);
    glEnable(target);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glDisable(target);
    glBindTexture(target, 0);
}

QImage *QGLWindowSurface::buffer(const QWidget *widget)
{
    QImage image;

    if (d_ptr->pb)
        image = d_ptr->pb->toImage();
    else if (d_ptr->fbo)
        image = d_ptr->fbo->toImage();

    if (image.isNull())
        return 0;

    QRect rect = widget->rect();
    rect.translate(widget->mapTo(widget->window(), QPoint()));

    QImage subImage = image.copy(rect);
    d_ptr->buffers << subImage;
    return &d_ptr->buffers.last();
}



QT_END_NAMESPACE

