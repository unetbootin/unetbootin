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

// The below is a hack to make this compile with the
// broken HPUX GL headers. They define GLX_VERSION_1_3
// without defining the GLXFBConfig structure, which
// is wrong. 
#if defined (Q_OS_HPUX) && !defined(GLX_SGIX_pbuffer)
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

QT_BEGIN_INCLUDE_NAMESPACE
#include "QtOpenGL/qglpixelbuffer.h"
#include <private/qgl_p.h>

#ifdef Q_WS_X11
#include <GL/glx.h>
#elif defined(Q_WS_WIN)
DECLARE_HANDLE(HPBUFFERARB);
#elif defined(Q_WS_MACX)
#include <AGL/agl.h>
#elif defined(Q_WS_QWS)
#include <GLES/egl.h>
#endif
QT_END_INCLUDE_NAMESPACE

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

#ifdef Q_WS_X11
    GLXPbuffer pbuf;
    GLXContext ctx;
#elif defined(Q_WS_WIN)
    HDC dc;
    HPBUFFERARB pbuf;
    HGLRC ctx;
    bool has_render_texture :1;
#elif defined(Q_WS_MACX)
#if (MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_3)
    AGLPbuffer pbuf;
#else
    void *pbuf;
#endif
    AGLContext ctx;
    AGLContext share_ctx;
#elif defined(Q_WS_QWS)
    void *pbuf;
    EGLContext ctx;
#endif
};

QT_END_NAMESPACE

#endif // QGLPIXELBUFFER_P_H
