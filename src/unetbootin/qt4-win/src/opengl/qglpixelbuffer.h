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

#ifndef QGLPIXELBUFFER_H
#define QGLPIXELBUFFER_H

#include <QtOpenGL/qgl.h>
#include <QtGui/qpaintdevice.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(OpenGL)

class QGLPixelBufferPrivate;

class Q_OPENGL_EXPORT QGLPixelBuffer : public QPaintDevice
{
    Q_DECLARE_PRIVATE(QGLPixelBuffer)
public:
    QGLPixelBuffer(const QSize &size, const QGLFormat &format = QGLFormat::defaultFormat(),
                   QGLWidget *shareWidget = 0);
    QGLPixelBuffer(int width, int height, const QGLFormat &format = QGLFormat::defaultFormat(),
                   QGLWidget *shareWidget = 0);
    virtual ~QGLPixelBuffer();

    bool isValid() const;
    bool makeCurrent();
    bool doneCurrent();

    GLuint generateDynamicTexture() const;
    bool bindToDynamicTexture(GLuint texture);
    void releaseFromDynamicTexture();
    void updateDynamicTexture(GLuint texture_id) const;

    GLuint bindTexture(const QImage &image, GLenum target = GL_TEXTURE_2D);
    GLuint bindTexture(const QPixmap &pixmap, GLenum target = GL_TEXTURE_2D);
    GLuint bindTexture(const QString &fileName);
    void deleteTexture(GLuint texture_id);

    void drawTexture(const QRectF &target, GLuint textureId, GLenum textureTarget = GL_TEXTURE_2D);
    void drawTexture(const QPointF &point, GLuint textureId, GLenum textureTarget = GL_TEXTURE_2D);

#ifdef Q_MAC_COMPAT_GL_FUNCTIONS
    bool bindToDynamicTexture(QMacCompatGLuint texture);
    void updateDynamicTexture(QMacCompatGLuint texture_id) const;
    GLuint bindTexture(const QImage &image, QMacCompatGLenum target = GL_TEXTURE_2D);
    GLuint bindTexture(const QPixmap &pixmap, QMacCompatGLenum target = GL_TEXTURE_2D);

    void drawTexture(const QRectF &target, QMacCompatGLuint textureId, QMacCompatGLenum textureTarget = GL_TEXTURE_2D);
    void drawTexture(const QPointF &point, QMacCompatGLuint textureId, QMacCompatGLenum textureTarget = GL_TEXTURE_2D);

    void deleteTexture(QMacCompatGLuint texture_id);
#endif

    QSize size() const;
    Qt::HANDLE handle() const;
    QImage toImage() const;

    QPaintEngine *paintEngine() const;
    QGLFormat format() const;

    static bool hasOpenGLPbuffers();

protected:
    int metric(PaintDeviceMetric metric) const;
    int devType() const { return QInternal::Pbuffer; }

private:
    Q_DISABLE_COPY(QGLPixelBuffer)
    QGLPixelBufferPrivate *d_ptr;
    friend class QGLDrawable;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QGLPIXELBUFFER_H
