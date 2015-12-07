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

#ifndef QGL_P_H
#define QGL_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of the QGLWidget class.  This header file may change from
// version to version without notice, or even be removed.
//
// We mean it.
//

#include "QtOpenGL/qgl.h"
#include "QtOpenGL/qglcolormap.h"
#include "QtCore/qmap.h"
#include "QtCore/qthread.h"
#include "QtCore/qthreadstorage.h"
#include "QtCore/qhash.h"
#include "private/qwidget_p.h"

#ifndef QT_OPENGL_ES_CL
#define q_vertexType float
#define q_vertexTypeEnum GL_FLOAT
#define f2vt(f)     (f)
#define vt2f(x)     (x)
#else
#define FLOAT2X(f)      ((int) ( (f) * (65536)))
#define X2FLOAT(x)      ((float)(x) / 65536.0f)
#define f2vt(f)     FLOAT2X(f)
#define vt2f(x)     X2FLOAT(x)
#define q_vertexType GLfixed
#define q_vertexTypeEnum GL_FIXED
#endif //QT_OPENGL_ES_CL

#ifdef QT_OPENGL_ES
QT_BEGIN_INCLUDE_NAMESPACE
#include <GLES/egl.h>
QT_END_INCLUDE_NAMESPACE
#endif

QT_BEGIN_NAMESPACE

class QGLContext;
class QGLOverlayWidget;
class QPixmap;
#ifdef Q_WS_MAC
# ifdef qDebug
#   define old_qDebug qDebug
#   undef qDebug
# endif
QT_BEGIN_INCLUDE_NAMESPACE
# include <AGL/agl.h>
QT_END_INCLUDE_NAMESPACE
# ifdef old_qDebug
#   undef qDebug
#   define qDebug QT_QDEBUG_MACRO
#   undef old_qDebug
# endif
class QMacWindowChangeEvent;
#endif

#ifdef Q_WS_QWS
class QGLDirectPainter;
class QWSGLWindowSurface;
#endif

QT_BEGIN_INCLUDE_NAMESPACE
#include <QtOpenGL/private/qglextensions_p.h>
QT_END_INCLUDE_NAMESPACE

class QGLFormatPrivate
{
public:
    QGLFormatPrivate() {
        opts = QGL::DoubleBuffer | QGL::DepthBuffer | QGL::Rgba | QGL::DirectRendering | QGL::StencilBuffer;
        pln = 0;
        depthSize = accumSize = stencilSize = redSize = greenSize = blueSize = alphaSize = -1;
        numSamples = -1;
        swapInterval = -1;
    }
    QGL::FormatOptions opts;
    int pln;
    int depthSize;
    int accumSize;
    int stencilSize;
    int redSize;
    int greenSize;
    int blueSize;
    int alphaSize;
    int numSamples;
    int swapInterval;
};

class QGLWidgetPrivate : public QWidgetPrivate
{
    Q_DECLARE_PUBLIC(QGLWidget)
public:
    QGLWidgetPrivate() : QWidgetPrivate()
#ifdef Q_USE_EGLWINDOWSURFACE
                       , wsurf(0)
#endif
        {}
    ~QGLWidgetPrivate() {}

    void init(QGLContext *context, const QGLWidget* shareWidget);
    void initContext(QGLContext *context, const QGLWidget* shareWidget);
    bool renderCxPm(QPixmap *pixmap);
    void cleanupColormaps();

    QGLContext *glcx;
    bool autoSwap;

    QGLColormap cmap;
    QMap<QString, int> displayListCache;

#if defined(Q_WS_WIN)
    void updateColormap();
    QGLContext *olcx;
#elif defined(Q_WS_X11)
    QGLOverlayWidget *olw;
#elif defined(Q_WS_MAC)
    QGLContext *olcx;
    void updatePaintDevice();
#elif defined(Q_WS_QWS)
    QGLDirectPainter *directPainter;
    void resizeHandler(const QSize &);
    void render(const QRegion&);
#ifdef Q_USE_EGLWINDOWSURFACE
    QWSGLWindowSurface *wsurf;
#endif
#endif
};

class QGLContextPrivate
{
    Q_DECLARE_PUBLIC(QGLContext)
public:
    explicit QGLContextPrivate(QGLContext *context) : q_ptr(context) {}
    ~QGLContextPrivate() {}
    GLuint bindTexture(const QImage &image, GLenum target, GLint format, const QString &key,
                       qint64 qt_id, bool clean = false);
    GLuint bindTexture(const QPixmap &pixmap, GLenum target, GLint format, bool clean);
    GLuint bindTexture(const QImage &image, GLenum target, GLint format, bool clean);
    bool textureCacheLookup(const QString &key, GLuint *id, qint64 *qt_id);
    void init(QPaintDevice *dev, const QGLFormat &format);
    QImage convertToGLFormat(const QImage &image, bool force_premul, GLenum texture_format);

#if defined(Q_WS_WIN)
    HGLRC rc;
    HDC dc;
    WId        win;
    int pixelFormatId;
    QGLCmap* cmap;
    HBITMAP hbitmap;
    HDC hbitmap_hdc;
#elif defined(Q_WS_X11) || defined(Q_WS_MAC)
    void* vi;
    void* cx;
#if defined(Q_WS_X11)
    void* pbuf;
    quint32 gpm;
    int screen;
#endif
#if defined(Q_WS_MAC)
    bool update;
    AGLPixelFormat tryFormat(const QGLFormat &format);
#endif
#endif
#if defined(QT_OPENGL_ES)
    EGLDisplay dpy;
    EGLContext cx;
    EGLConfig  config;
    EGLSurface surface;
#endif
    QGLFormat glFormat;
    QGLFormat reqFormat;

    uint valid : 1;
    uint sharing : 1;
    uint initDone : 1;
    uint crWin : 1;
    uint clear_on_painter_begin : 1;
    QPaintDevice *paintDevice;
    QColor transpColor;
    QGLContext *q_ptr;

    QGLExtensionFuncs extensionFuncs;

#ifdef Q_WS_WIN
    static inline QGLExtensionFuncs& qt_get_extension_funcs(const QGLContext *ctx) { return ctx->d_ptr->extensionFuncs; }
#endif

#if defined(Q_WS_X11) || defined(Q_WS_MAC) || defined(Q_WS_QWS)
    static QGLExtensionFuncs qt_extensionFuncs;
    static inline QGLExtensionFuncs& qt_get_extension_funcs(const QGLContext *) { return qt_extensionFuncs; }
#endif

};

// ### make QGLContext a QObject in 5.0 and remove the proxy stuff
class QGLSignalProxy : public QObject
{
    Q_OBJECT
public:
    QGLSignalProxy() : QObject() {}
    void emitAboutToDestroyContext(const QGLContext *context) {
        emit aboutToDestroyContext(context);
    }
    static QGLSignalProxy *instance();
Q_SIGNALS:
    void aboutToDestroyContext(const QGLContext *context);
};

// GL extension definitions
class QGLExtensions {
public:
    enum Extension {
        TextureRectangle        = 0x00000001,
        SampleBuffers           = 0x00000002,
        GenerateMipmap          = 0x00000004,
        TextureCompression      = 0x00000008,
        FragmentProgram         = 0x00000010,
        MirroredRepeat          = 0x00000020,
        FramebufferObject       = 0x00000040,
        StencilTwoSide          = 0x00000080,
        StencilWrap             = 0x00000100,
        PackedDepthStencil      = 0x00000200,
        NVFloatBuffer           = 0x00000400
    };
    Q_DECLARE_FLAGS(Extensions, Extension)

    static Extensions glExtensions;
    static bool nvidiaFboNeedsFinish;
    static void init(); // sys dependent
    static void init_extensions(); // general: called by init()
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QGLExtensions::Extensions)


struct QGLThreadContext {
    QGLContext *context;
};
extern QThreadStorage<QGLThreadContext *> qgl_context_storage;

typedef QMultiHash<const QGLContext *, const QGLContext *> QGLSharingHash;
class QGLShareRegister
{
public:
    QGLShareRegister() {}
    ~QGLShareRegister() { reg.clear(); }

    bool checkSharing(const QGLContext *context1, const QGLContext *context2, const QGLContext * skip=0) {
        if (context1 == context2)
            return true;
        QList<const QGLContext *> shares = reg.values(context1);
        for (int k=0; k<shares.size(); ++k) {
            const QGLContext *ctx = shares.at(k);
            if (ctx == skip) // avoid an indirect circular loop (infinite recursion)
                continue;
            if (ctx == context2)
                return true;
            if (checkSharing(ctx, context2, context1))
                return true;
        }
        return false;
    }

    void addShare(const QGLContext *context, const QGLContext *share) {
        reg.insert(context, share); // context sharing works both ways
        reg.insert(share, context);
    }

    void removeShare(const QGLContext *context) {
        QGLSharingHash::iterator it = reg.begin();
        while (it != reg.end()) {
            if (it.key() == context || it.value() == context)
                it = reg.erase(it);
            else
                ++it;
        }
    }

    void replaceShare(const QGLContext *oldContext, const QGLContext *newContext) {
        QGLSharingHash::iterator it = reg.begin();
        while (it != reg.end()) {
            if (it.key() == oldContext)
                reg.insert(newContext, it.value());
            else if (it.value() == oldContext)
                reg.insert(it.key(), newContext);
            ++it;
        }
        removeShare(oldContext);
    }

private:
    QGLSharingHash reg;
};

extern Q_OPENGL_EXPORT QGLShareRegister* qgl_share_reg();

#ifdef Q_WS_QWS
class QOpenGLPaintEngine;
extern QOpenGLPaintEngine* qt_qgl_paint_engine();
#endif

QT_END_NAMESPACE

#endif // QGL_P_H
