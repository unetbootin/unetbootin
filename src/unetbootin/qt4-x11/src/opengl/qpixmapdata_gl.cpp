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

#include "qpixmap.h"

#include <private/qpaintengine_raster_p.h>

#include "qpixmapdata_gl_p.h"

#include <private/qgl_p.h>
#include <private/qdrawhelper_p.h>

QT_BEGIN_NAMESPACE

extern QGLWidget* qt_gl_share_widget();

class QGLShareContextScope
{
public:
    QGLShareContextScope(const QGLContext *ctx)
        : m_oldContext(0)
        , m_ctx(const_cast<QGLContext *>(ctx))
    {
        const QGLContext *currentContext = QGLContext::currentContext();
        if (currentContext != ctx && !qgl_share_reg()->checkSharing(ctx, currentContext)) {
            m_oldContext = const_cast<QGLContext *>(currentContext);
            m_ctx->makeCurrent();
        }
    }

    operator QGLContext *()
    {
        return m_ctx;
    }

    QGLContext *operator->()
    {
        return m_ctx;
    }

    ~QGLShareContextScope()
    {
        if (m_oldContext)
            m_oldContext->makeCurrent();
    }

private:
    QGLContext *m_oldContext;
    QGLContext *m_ctx;
};

void qt_gl_convertFromGLImage(QImage *img)
{
    const int w = img->width();
    const int h = img->height();

    if (QSysInfo::ByteOrder == QSysInfo::BigEndian) {
        uint *p = (uint*)img->bits();
        uint *end = p + w*h;

        while (p < end) {
            uint a = *p << 24;
            *p = (*p >> 8) | a;
            p++;
        }

        *img = img->mirrored();
    } else {
        // mirror image
        uint *data = (uint *)img->bits();

        const int mid = h/2;

        for (int y = 0; y < mid; ++y) {
            uint *p = data + y * w;
            uint *end = p + w;
            uint *q = data + (h - y - 1) * w;

            while (p < end)
                qSwap(*p++, *q++);
        }
    }
}


static int qt_gl_pixmap_serial = 0;

QGLPixmapData::QGLPixmapData(PixelType type)
    : QPixmapData(type, OpenGLClass)
    , m_width(0)
    , m_height(0)
    , m_texture(0)
    , m_dirty(false)
{
    setSerialNumber(++qt_gl_pixmap_serial);
}

QGLPixmapData::~QGLPixmapData()
{
    if (m_texture && qt_gl_share_widget()) {
        QGLShareContextScope ctx(qt_gl_share_widget()->context());
        glDeleteTextures(1, &m_texture);
    }
}

bool QGLPixmapData::isValid() const
{
    return m_width > 0 && m_height > 0;
}

bool QGLPixmapData::isValidContext(const QGLContext *ctx) const
{
    const QGLContext *share_ctx = qt_gl_share_widget()->context();
    return ctx == share_ctx || qgl_share_reg()->checkSharing(ctx, share_ctx);
}

void QGLPixmapData::resize(int width, int height)
{
    if (width == m_width && height == m_height)
        return;

    m_width = width;
    m_height = height;

    m_source = QImage();
    m_dirty = isValid();
    setSerialNumber(++qt_gl_pixmap_serial);
}

void QGLPixmapData::ensureCreated() const
{
    if (!m_dirty)
        return;

    m_dirty = false;

    QGLShareContextScope ctx(qt_gl_share_widget()->context());

    const GLenum format = qt_gl_preferredTextureFormat();
    const GLenum target = qt_gl_preferredTextureTarget();

    if (!m_texture)
        glGenTextures(1, &m_texture);

    glBindTexture(target, m_texture);

    if (m_source.isNull()) {
        glTexImage2D(target, 0, GL_RGBA, m_width, m_height, 0, format, GL_UNSIGNED_BYTE, 0);
    } else {
        const QImage tx = ctx->d_func()->convertToGLFormat(m_source, true, format);

        glBindTexture(target, m_texture);
        glTexImage2D(target, 0, GL_RGBA, m_width, m_height, 0, format,
                GL_UNSIGNED_BYTE, tx.bits());

        m_source = QImage();
    }
}

void QGLPixmapData::fromImage(const QImage &image,
                              Qt::ImageConversionFlags)
{
    if (image.size() == QSize(m_width, m_height))
        setSerialNumber(++qt_gl_pixmap_serial);
    resize(image.width(), image.height());
    m_source = image;
    m_dirty = true;
}

void QGLPixmapData::fill(const QColor &color)
{
    if (!isValid())
        return;

    if (!m_source.isNull()) {
        m_source.fill(PREMUL(color.rgba()));
    } else {
        // ## TODO: improve performance here
        QImage img(m_width, m_height, QImage::Format_ARGB32_Premultiplied);
        img.fill(PREMUL(color.rgba()));

        fromImage(img, 0);
    }
}

bool QGLPixmapData::hasAlphaChannel() const
{
    return true;
}

QImage QGLPixmapData::toImage() const
{
    if (!isValid())
        return QImage();

    if (!m_source.isNull())
        return m_source;
    else if (m_dirty)
        return QImage(m_width, m_height, QImage::Format_ARGB32_Premultiplied);

    ensureCreated();

    QGLShareContextScope ctx(qt_gl_share_widget()->context());
    QImage img(m_width, m_height, QImage::Format_ARGB32_Premultiplied);

    GLenum format = qt_gl_preferredTextureFormat();
    GLenum target = qt_gl_preferredTextureTarget();

    glBindTexture(target, m_texture);
#ifndef QT_OPENGL_ES
    glGetTexImage(target, 0, format, GL_UNSIGNED_BYTE, img.bits());
#else
    // XXX - cannot download textures this way on OpenGL/ES.
#endif

    qt_gl_convertFromGLImage(&img);

    return img;
}

QPaintEngine* QGLPixmapData::paintEngine() const
{
    if (!isValid())
        return 0;

    m_source = toImage();
    m_dirty = true;

    return m_source.paintEngine();
}

GLuint QGLPixmapData::bind() const
{
    ensureCreated();
    glBindTexture(qt_gl_preferredTextureTarget(), m_texture);
    return m_texture;
}

GLuint QGLPixmapData::textureId() const
{
    ensureCreated();
    return m_texture;
}

extern int qt_defaultDpiX();
extern int qt_defaultDpiY();

int QGLPixmapData::metric(QPaintDevice::PaintDeviceMetric metric) const
{
    switch (metric) {
    case QPaintDevice::PdmWidth:
        return m_width;
    case QPaintDevice::PdmHeight:
        return m_height;
    case QPaintDevice::PdmNumColors:
        return 0;
    case QPaintDevice::PdmDepth:
        return pixelType() == QPixmapData::PixmapType ? 32 : 1;
    case QPaintDevice::PdmWidthMM:
        return qRound(m_width * 25.4 / qt_defaultDpiX());
    case QPaintDevice::PdmHeightMM:
        return qRound(m_height * 25.4 / qt_defaultDpiY());
    case QPaintDevice::PdmDpiX:
    case QPaintDevice::PdmPhysicalDpiX:
        return qt_defaultDpiX();
    case QPaintDevice::PdmDpiY:
    case QPaintDevice::PdmPhysicalDpiY:
        return qt_defaultDpiY();
    default:
        qWarning("QGLPixmapData::metric(): Invalid metric");
        return 0;
    }
}

QT_END_NAMESPACE
