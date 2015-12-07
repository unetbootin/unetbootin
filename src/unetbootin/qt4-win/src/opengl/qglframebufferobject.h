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

#ifndef QGLFRAMEBUFFEROBJECT_H
#define QGLFRAMEBUFFEROBJECT_H

#include <QtOpenGL/qgl.h>
#include <QtGui/qpaintdevice.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(OpenGL)

class QGLFramebufferObjectPrivate;

class Q_OPENGL_EXPORT QGLFramebufferObject : public QPaintDevice
{
    Q_DECLARE_PRIVATE(QGLFramebufferObject)
public:
    enum Attachment {
        NoAttachment,
        CombinedDepthStencil,
        Depth
    };

    QGLFramebufferObject(const QSize &size, GLenum target = GL_TEXTURE_2D);
    QGLFramebufferObject(int width, int height, GLenum target = GL_TEXTURE_2D);
#if !defined(QT_OPENGL_ES) || defined(Q_QDOC)
    QGLFramebufferObject(const QSize &size, Attachment attachment,
                         GLenum target = GL_TEXTURE_2D, GLenum internal_format = GL_RGBA8);
    QGLFramebufferObject(int width, int height, Attachment attachment,
                         GLenum target = GL_TEXTURE_2D, GLenum internal_format = GL_RGBA8);
#else
    QGLFramebufferObject(const QSize &size, Attachment attachment,
                         GLenum target = GL_TEXTURE_2D, GLenum internal_format = GL_RGBA);
    QGLFramebufferObject(int width, int height, Attachment attachment,
                         GLenum target = GL_TEXTURE_2D, GLenum internal_format = GL_RGBA);
#endif

#ifdef Q_MAC_COMPAT_GL_FUNCTIONS
    QGLFramebufferObject(const QSize &size, QMacCompatGLenum target = GL_TEXTURE_2D);
    QGLFramebufferObject(int width, int height, QMacCompatGLenum target = GL_TEXTURE_2D);

    QGLFramebufferObject(const QSize &size, Attachment attachment,
                         QMacCompatGLenum target = GL_TEXTURE_2D, QMacCompatGLenum internal_format = GL_RGBA8);
    QGLFramebufferObject(int width, int height, Attachment attachment,
                         QMacCompatGLenum target = GL_TEXTURE_2D, QMacCompatGLenum internal_format = GL_RGBA8);
#endif

    virtual ~QGLFramebufferObject();

    bool isValid() const;
    bool bind();
    bool release();
    GLuint texture() const;
    QSize size() const;
    QImage toImage() const;
    Attachment attachment() const;

    QPaintEngine *paintEngine() const;
    GLuint handle() const;

    static bool hasOpenGLFramebufferObjects();

    void drawTexture(const QRectF &target, GLuint textureId, GLenum textureTarget = GL_TEXTURE_2D);
    void drawTexture(const QPointF &point, GLuint textureId, GLenum textureTarget = GL_TEXTURE_2D);
#ifdef Q_MAC_COMPAT_GL_FUNCTIONS
    void drawTexture(const QRectF &target, QMacCompatGLuint textureId, QMacCompatGLenum textureTarget = GL_TEXTURE_2D);
    void drawTexture(const QPointF &point, QMacCompatGLuint textureId, QMacCompatGLenum textureTarget = GL_TEXTURE_2D);
#endif

protected:
    int metric(PaintDeviceMetric metric) const;
    int devType() const { return QInternal::FramebufferObject; }

private:
    Q_DISABLE_COPY(QGLFramebufferObject)
    QGLFramebufferObjectPrivate *d_ptr;
    friend class QGLDrawable;
};

QT_END_NAMESPACE

QT_END_HEADER
#endif // QGLFRAMEBUFFEROBJECT_H
