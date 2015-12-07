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

#ifndef QGLPIXELBUFFER_P_H
#define QGLPIXELBUFFER_P_H

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

QT_BEGIN_NAMESPACE

QT_BEGIN_INCLUDE_NAMESPACE
#include "QtOpenGL/qglpixelbuffer.h"
#include <private/qgl_p.h>

#if defined(Q_WS_X11) && !defined(QT_OPENGL_ES)
#include <GL/glx.h>

// The below is needed to for compilation on HPUX, due to broken GLX
// headers. Some of the systems define GLX_VERSION_1_3 without
// defining the GLXFBConfig structure, which is wrong.
#if defined (Q_OS_HPUX) && defined(QT_DEFINE_GLXFBCONFIG_STRUCT)
typedef unsigned long GLXPbuffer;

struct GLXFBConfig {
    int visualType;
    int transparentType;
                                /*    colors are floats scaled to ints */
    int transparentRed, transparentGreen, transparentBlue, transparentAlpha;
    int transparentIndex;

    int visualCaveat;

    int associatedVisualId;
    int screen;

    int drawableType;
    int renderType;

    int maxPbufferWidth, maxPbufferHeight, maxPbufferPixels;
    int optimalPbufferWidth, optimalPbufferHeight;  /* for SGIX_pbuffer */

    int visualSelectGroup;	/* visuals grouped by select priority */

    unsigned int id;

    GLboolean rgbMode;
    GLboolean colorIndexMode;
    GLboolean doubleBufferMode;
    GLboolean stereoMode;
    GLboolean haveAccumBuffer;
    GLboolean haveDepthBuffer;
    GLboolean haveStencilBuffer;

    /* The number of bits present in various buffers */
    GLint accumRedBits, accumGreenBits, accumBlueBits, accumAlphaBits;
    GLint depthBits;
    GLint stencilBits;
    GLint indexBits;
    GLint redBits, greenBits, blueBits, alphaBits;
    GLuint redMask, greenMask, blueMask, alphaMask;

    GLuint multiSampleSize;     /* Number of samples per pixel (0 if no ms) */

    GLuint nMultiSampleBuffers; /* Number of available ms buffers */
    GLint maxAuxBuffers;

    /* frame buffer level */
    GLint level;

    /* color ranges (for SGI_color_range) */
    GLboolean extendedRange;
    GLdouble minRed, maxRed;
    GLdouble minGreen, maxGreen;
    GLdouble minBlue, maxBlue;
    GLdouble minAlpha, maxAlpha;
};

#endif // Q_OS_HPUX

#elif defined(Q_WS_WIN)
DECLARE_HANDLE(HPBUFFERARB);
#elif defined(QT_OPENGL_ES_2)
#include <EGL/egl.h>
#elif defined(QT_OPENGL_ES)
#include <GLES/egl.h>
#endif
QT_END_INCLUDE_NAMESPACE

class QEglContext;

class QGLPixelBufferPrivate {
    Q_DECLARE_PUBLIC(QGLPixelBuffer)
public:
    QGLPixelBufferPrivate(QGLPixelBuffer *q) : q_ptr(q), invalid(true), qctx(0), pbuf(0), ctx(0)
    {
        QGLExtensions::init();
#ifdef Q_WS_WIN
        dc = 0;
#elif defined(Q_WS_MACX)
        share_ctx = 0;
#endif
    }
    bool init(const QSize &size, const QGLFormat &f, QGLWidget *shareWidget);
    void common_init(const QSize &size, const QGLFormat &f, QGLWidget *shareWidget);
    bool cleanup();

    QGLPixelBuffer *q_ptr;
    bool invalid;
    QGLContext *qctx;
    QGLFormat format;

    QGLFormat req_format;
    QPointer<QGLWidget> req_shareWidget;
    QSize req_size;

#if defined(Q_WS_X11) && !defined(QT_OPENGL_ES)
    GLXPbuffer pbuf;
    GLXContext ctx;
#elif defined(Q_WS_WIN)
    HDC dc;
    bool has_render_texture :1;
#if !defined(QT_OPENGL_ES)
    HPBUFFERARB pbuf;
    HGLRC ctx;
#endif
#elif defined(Q_WS_MACX)
#  ifdef QT_MAC_USE_COCOA
    void *pbuf;
    void *ctx;
    void *share_ctx;
#  else
    AGLPbuffer pbuf;
    AGLContext ctx;
    AGLContext share_ctx;
#  endif
#endif
#if defined(QT_OPENGL_ES)
    EGLSurface pbuf;
    QEglContext *ctx;
    int textureFormat;
#endif
};

QT_END_NAMESPACE

#endif // QGLPIXELBUFFER_P_H
