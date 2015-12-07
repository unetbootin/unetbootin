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

#include <qdebug.h>
#include "qglpixelbuffer.h"
#include "qglpixelbuffer_p.h"
#include "qgl_egl_p.h"

#include <qimage.h>
#include <private/qgl_p.h>

QT_BEGIN_NAMESPACE

#ifdef EGL_BIND_TO_TEXTURE_RGBA
#define QGL_RENDER_TEXTURE 1
#else
#define QGL_RENDER_TEXTURE 0
#endif

bool QGLPixelBufferPrivate::init(const QSize &size, const QGLFormat &f, QGLWidget *shareWidget)
{
    // Create the EGL context.
    ctx = new QEglContext();
    ctx->setApi(QEglContext::OpenGL);

    // Open the EGL display.
    if (!ctx->openDisplay(0)) {
        delete ctx;
        ctx = 0;
        return false;
    }

    // Choose an appropriate configuration.  We use the best format
    // we can find, even if it is greater than the requested format.
    // We try for a pbuffer that is capable of texture rendering if possible.
    QEglProperties configProps;
    qt_egl_set_format(configProps, QInternal::Pbuffer, f);
    configProps.setRenderableType(ctx->api());
    bool ok = false;
#if QGL_RENDER_TEXTURE
    textureFormat = EGL_TEXTURE_RGBA;
    configProps.setValue(EGL_BIND_TO_TEXTURE_RGBA, EGL_TRUE);
    ok = ctx->chooseConfig(configProps, QEglContext::BestPixelFormat);
    if (!ok) {
        // Try again with RGB texture rendering.
        textureFormat = EGL_TEXTURE_RGB;
        configProps.removeValue(EGL_BIND_TO_TEXTURE_RGBA);
        configProps.setValue(EGL_BIND_TO_TEXTURE_RGB, EGL_TRUE);
        ok = ctx->chooseConfig(configProps, QEglContext::BestPixelFormat);
        if (!ok) {
            // One last try for a pbuffer with no texture rendering.
            configProps.removeValue(EGL_BIND_TO_TEXTURE_RGB);
            textureFormat = EGL_NONE;
        }
    }
#else
    textureFormat = EGL_NONE;
#endif
    if (!ok) {
        if (!ctx->chooseConfig(configProps, QEglContext::BestPixelFormat)) {
            delete ctx;
            ctx = 0;
            return false;
        }
    }

    // Retrieve the actual format properties.
    qt_egl_update_format(*ctx, format);

    // Create the attributes needed for the pbuffer.
    QEglProperties attribs;
    attribs.setValue(EGL_WIDTH, size.width());
    attribs.setValue(EGL_HEIGHT, size.height());
#if QGL_RENDER_TEXTURE
    if (textureFormat != EGL_NONE) {
        attribs.setValue(EGL_TEXTURE_FORMAT, textureFormat);
        attribs.setValue(EGL_TEXTURE_TARGET, EGL_TEXTURE_2D);
    }
#endif

    // Create the pbuffer surface.
    pbuf = eglCreatePbufferSurface(ctx->display(), ctx->config(), attribs.properties());
#if QGL_RENDER_TEXTURE
    if (pbuf == EGL_NO_SURFACE && textureFormat != EGL_NONE) {
        // Try again with texture rendering disabled.
        textureFormat = EGL_NONE;
        attribs.removeValue(EGL_TEXTURE_FORMAT);
        attribs.removeValue(EGL_TEXTURE_TARGET);
        pbuf = eglCreatePbufferSurface(ctx->display(), ctx->config(), attribs.properties());
    }
#endif
    if (pbuf == EGL_NO_SURFACE) {
        qWarning() << "QGLPixelBufferPrivate::init(): Unable to create EGL pbuffer surface:" << QEglContext::errorString(eglGetError());
        return false;
    }
    ctx->setSurface(pbuf);

    // Create a new context for the configuration.
    QEglContext *shareContext = 0;
    if (shareWidget && shareWidget->d_func()->glcx)
        shareContext = shareWidget->d_func()->glcx->d_func()->eglContext;
    if (!ctx->createContext(shareContext)) {
        delete ctx;
        ctx = 0;
        return false;
    }

    return true;
}

bool QGLPixelBufferPrivate::cleanup()
{
    eglDestroySurface(QEglContext::defaultDisplay(0), pbuf);
    return true;
}

bool QGLPixelBuffer::bindToDynamicTexture(GLuint texture_id)
{
#if QGL_RENDER_TEXTURE
    Q_D(QGLPixelBuffer);
    if (d->invalid || d->textureFormat == EGL_NONE || !d->ctx)
        return false;
    glBindTexture(GL_TEXTURE_2D, texture_id);
    return eglBindTexImage(d->ctx->display(), d->ctx->surface(), EGL_BACK_BUFFER);
#else
    Q_UNUSED(texture_id);
    return false;
#endif
}

void QGLPixelBuffer::releaseFromDynamicTexture()
{
#if QGL_RENDER_TEXTURE
    Q_D(QGLPixelBuffer);
    if (d->invalid || d->textureFormat == EGL_NONE || !d->ctx)
        return;
    eglReleaseTexImage(d->ctx->display(), d->ctx->surface(), EGL_BACK_BUFFER);
#endif
}


GLuint QGLPixelBuffer::generateDynamicTexture() const
{
#if QGL_RENDER_TEXTURE
    Q_D(const QGLPixelBuffer);
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    if (d->textureFormat == EGL_TEXTURE_RGB)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, d->req_size.width(), d->req_size.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    else
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, d->req_size.width(), d->req_size.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    return texture;
#else
    return 0;
#endif
}

bool QGLPixelBuffer::hasOpenGLPbuffers()
{
    // See if we have at least 1 configuration that matches the default format.
    QEglContext ctx;
    if (!ctx.openDisplay(0))
        return false;
    QEglProperties configProps;
    qt_egl_set_format(configProps, QInternal::Pbuffer, QGLFormat::defaultFormat());
    configProps.setRenderableType(QEglContext::OpenGL);
    return ctx.chooseConfig(configProps);
}

QT_END_NAMESPACE
