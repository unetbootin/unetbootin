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

#include "qglframebufferobject.h"

#include <qdebug.h>
#include <private/qgl_p.h>
#include <private/qpaintengine_opengl_p.h>
#include <qglframebufferobject.h>
#include <qlibrary.h>
#include <qimage.h>

QT_BEGIN_NAMESPACE

extern QImage qt_gl_read_framebuffer(const QSize&, bool, bool);

#define QGL_FUNC_CONTEXT QGLContext *ctx = d_ptr->ctx;

#define QT_CHECK_GLERROR()                                \
{                                                         \
    GLenum err = glGetError();                            \
    if (err != GL_NO_ERROR) {                             \
        qDebug("[%s line %d] GL Error: %d",               \
               __FILE__, __LINE__, (int)err);             \
    }                                                     \
}

class QGLFramebufferObjectPrivate
{
public:
    QGLFramebufferObjectPrivate() : depth_stencil_buffer(0), valid(false), bound(false), ctx(0) {}
    ~QGLFramebufferObjectPrivate() {}

    void init(const QSize& sz, QGLFramebufferObject::Attachment attachment,
              GLenum internal_format, GLenum texture_target);
    bool checkFramebufferStatus() const;
    GLuint texture;
    GLuint fbo;
    GLuint depth_stencil_buffer;
    GLenum target;
    QSize size;
    uint valid : 1;
    uint bound : 1;
    QGLFramebufferObject::Attachment fbo_attachment;
    QGLContext *ctx; // for Windows extension ptrs
};

bool QGLFramebufferObjectPrivate::checkFramebufferStatus() const
{
    GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
    switch(status) {
    case GL_NO_ERROR:
    case GL_FRAMEBUFFER_COMPLETE_EXT:
        return true;
        break;
    case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
        qDebug("QGLFramebufferObject: Unsupported framebuffer format.");
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
        qDebug("QGLFramebufferObject: Framebuffer incomplete attachment.");
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
        qDebug("QGLFramebufferObject: Framebuffer incomplete, missing attachment.");
        break;
#ifdef GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT
    case GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT:
        qDebug("QGLFramebufferObject: Framebuffer incomplete, duplicate attachment.");
        break;
#endif
    case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
        qDebug("QGLFramebufferObject: Framebuffer incomplete, attached images must have same dimensions.");
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
        qDebug("QGLFramebufferObject: Framebuffer incomplete, attached images must have same format.");
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
        qDebug("QGLFramebufferObject: Framebuffer incomplete, missing draw buffer.");
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
        qDebug("QGLFramebufferObject: Framebuffer incomplete, missing read buffer.");
        break;
    default:
        qDebug() <<"QGLFramebufferObject: An undefined error has occurred: "<< status;
        break;
    }
    return false;
}

void QGLFramebufferObjectPrivate::init(const QSize &sz, QGLFramebufferObject::Attachment attachment,
                                       GLenum texture_target, GLenum internal_format)
{
    ctx = const_cast<QGLContext *>(QGLContext::currentContext());
    bool ext_detected = (QGLExtensions::glExtensions & QGLExtensions::FramebufferObject);
    if (!ext_detected || (ext_detected && !qt_resolve_framebufferobject_extensions(ctx)))
        return;

    size = sz;
    target = texture_target;
    // texture dimensions

    while (glGetError() != GL_NO_ERROR) {} // reset error state
    glGenFramebuffersEXT(1, &fbo);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);

    QT_CHECK_GLERROR();
    // init texture
    glGenTextures(1, &texture);
    glBindTexture(target, texture);
    glTexImage2D(target, 0, internal_format, size.width(), size.height(), 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, NULL);
#ifndef QT_OPENGL_ES
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
#else
    glTexParameterf(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
#endif
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
                              target, texture, 0);

    QT_CHECK_GLERROR();
    valid = checkFramebufferStatus();

    if (attachment == QGLFramebufferObject::CombinedDepthStencil
        && (QGLExtensions::glExtensions & QGLExtensions::PackedDepthStencil)) {
        // depth and stencil buffer needs another extension
        glGenRenderbuffersEXT(1, &depth_stencil_buffer);
        Q_ASSERT(!glIsRenderbufferEXT(depth_stencil_buffer));
        glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depth_stencil_buffer);
        Q_ASSERT(glIsRenderbufferEXT(depth_stencil_buffer));
        glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH24_STENCIL8_EXT, size.width(), size.height());
        GLint i = 0;
        glGetRenderbufferParameterivEXT(GL_RENDERBUFFER_EXT, GL_RENDERBUFFER_DEPTH_SIZE_EXT, &i);
        glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,
                                     GL_RENDERBUFFER_EXT, depth_stencil_buffer);
        glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT,
                                     GL_RENDERBUFFER_EXT, depth_stencil_buffer);
        fbo_attachment = QGLFramebufferObject::CombinedDepthStencil;
        valid = checkFramebufferStatus();
        if (!valid)
            glDeleteRenderbuffersEXT(1, &depth_stencil_buffer);
    } else if (attachment == QGLFramebufferObject::Depth
               || attachment == QGLFramebufferObject::CombinedDepthStencil)
    {
        glGenRenderbuffersEXT(1, &depth_stencil_buffer);
        Q_ASSERT(!glIsRenderbufferEXT(depth_stencil_buffer));
        glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depth_stencil_buffer);
        Q_ASSERT(glIsRenderbufferEXT(depth_stencil_buffer));
#ifdef QT_OPENGL_ES
#define GL_DEPTH_COMPONENT16 0x81A5
        glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT16, size.width(), size.height());
#else
        glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, size.width(), size.height());
#endif
        GLint i = 0;
        glGetRenderbufferParameterivEXT(GL_RENDERBUFFER_EXT, GL_RENDERBUFFER_DEPTH_SIZE_EXT, &i);
        glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,
                                     GL_RENDERBUFFER_EXT, depth_stencil_buffer);
        fbo_attachment = QGLFramebufferObject::Depth;
        valid = checkFramebufferStatus();
        if (!valid)
            glDeleteRenderbuffersEXT(1, &depth_stencil_buffer);
    } else {
        fbo_attachment = QGLFramebufferObject::NoAttachment;
    }

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    if (!valid) {
        glDeleteTextures(1, &texture);
        glDeleteFramebuffersEXT(1, &fbo);
    }
    QT_CHECK_GLERROR();
}

/*!
    \class QGLFramebufferObject
    \brief The QGLFramebufferObject class encapsulates an OpenGL framebuffer object.
    \since 4.2

    \ingroup multimedia

    The QGLFramebufferObject class encapsulates an OpenGL framebuffer
    object, defined by the \c{GL_EXT_framebuffer_object} extension. In
    addition it provides a rendering surface that can be painted on
    with a QPainter, rendered to using native GL calls, or both. This
    surface can be bound and used as a regular texture in your own GL
    drawing code.  By default, the QGLFramebufferObject class
    generates a 2D GL texture (using the \c{GL_TEXTURE_2D} target),
    which is used as the internal rendering target.

    \bold{It is important to have a current GL context when creating a
    QGLFramebufferObject, otherwise initialization will fail.}

    OpenGL framebuffer objects and pbuffers (see
    \l{QGLPixelBuffer}{QGLPixelBuffer}) can both be used to render to
    offscreen surfaces, but there are a number of advantages with
    using framebuffer objects instead of pbuffers:

    \list 1
    \o A framebuffer object does not require a separate rendering
    context, so no context switching will occur when switching
    rendering targets. There is an overhead involved in switching
    targets, but in general it is cheaper than a context switch to a
    pbuffer.

    \o Rendering to dynamic textures (i.e. render-to-texture
    functionality) works on all platforms. No need to do explicit copy
    calls from a render buffer into a texture, as was necessary on
    systems that did not support the \c{render_texture} extension.

    \o It is possible to attach several rendering buffers (or texture
    objects) to the same framebuffer object, and render to all of them
    without doing a context switch.

    \o The OpenGL framebuffer extension is a pure GL extension with no
    system dependant WGL, CGL, or GLX parts. This makes using
    framebuffer objects more portable.
    \endlist

    Note that primitives drawn to a QGLFramebufferObject with QPainter
    will only be antialiased if the QPainter::HighQualityAntialiasing
    render hint is set. This is because there is currently no support
    for the \c{GL_EXT_framebuffer_multisample} extension, which is
    required to do multisample based antialiasing. Also note that the
    QPainter::HighQualityAntialiasing render hint requires the
    \c{GL_ARB_fragment_program} extension to work in OpenGL.

    \sa {Framebuffer Object Example}
*/


/*!
    \enum QGLFramebufferObject::Attachment
    \since 4.3

    This enum type is used to configure the depth and stencil buffers
    attached to the framebuffer object when it is created.

    \value NoAttachment         No attachment is added to the framebuffer object. Note that the
                                OpenGL depth and stencil tests won't work when rendering to a
                                framebuffer object without any depth or stencil buffers.
                                This is the default value.

    \value CombinedDepthStencil If the \c GL_EXT_packed_depth_stencil extension is present,
                                a combined depth and stencil buffer is attached.
                                If the extension is not present, only a depth buffer is attached.

    \value Depth                A depth buffer is attached to the framebuffer object.

    \sa attachment()
*/


/*! \fn QGLFramebufferObject::QGLFramebufferObject(const QSize &size, GLenum target)

    Constructs an OpenGL framebuffer object and binds a 2D GL texture
    to the buffer of the size \a size. The texture is bound to the
    \c GL_COLOR_ATTACHMENT0 target in the framebuffer object.

    The \a target parameter is used to specify the GL texture
    target. The default target is \c GL_TEXTURE_2D. Keep in mind that
    \c GL_TEXTURE_2D textures must have a power of 2 width and height
    (e.g. 256x512), unless you are using OpenGL 2.0 or higher.

    By default, no depth and stencil buffers are attached. This behavior
    can be toggled using one of the overloaded constructors.

    The default internal texture format is \c GL_RGBA8.

    It is important that you have a current GL context set when
    creating the QGLFramebufferObject, otherwise the initialization
    will fail.

    \sa size(), texture(), attachment()
*/

#ifndef QT_OPENGL_ES
#define DEFAULT_FORMAT GL_RGBA8
#else
#define DEFAULT_FORMAT GL_RGBA
#endif

QGLFramebufferObject::QGLFramebufferObject(const QSize &size, GLenum target)
    : d_ptr(new QGLFramebufferObjectPrivate)
{
    Q_D(QGLFramebufferObject);
    d->init(size, NoAttachment, target, DEFAULT_FORMAT);
}

#ifdef Q_MAC_COMPAT_GL_FUNCTIONS
/*! \internal */
QGLFramebufferObject::QGLFramebufferObject(const QSize &size, QMacCompatGLenum target)
    : d_ptr(new QGLFramebufferObjectPrivate)
{
    Q_D(QGLFramebufferObject);
    d->init(size, NoAttachment, target, DEFAULT_FORMAT);
}
#endif

/*! \overload

    Constructs an OpenGL framebuffer object and binds a 2D GL texture
    to the buffer of the given \a width and \a height.

    \sa size(), texture()
*/
QGLFramebufferObject::QGLFramebufferObject(int width, int height, GLenum target)
    : d_ptr(new QGLFramebufferObjectPrivate)
{
    Q_D(QGLFramebufferObject);
    d->init(QSize(width, height), NoAttachment, target, DEFAULT_FORMAT);
}

#ifdef Q_MAC_COMPAT_GL_FUNCTIONS
/*! \internal */
QGLFramebufferObject::QGLFramebufferObject(int width, int height, QMacCompatGLenum target)
    : d_ptr(new QGLFramebufferObjectPrivate)
{
    Q_D(QGLFramebufferObject);
    d->init(QSize(width, height), NoAttachment, target, DEFAULT_FORMAT);
}
#endif

/*! \overload

    Constructs an OpenGL framebuffer object and binds a texture to the
    buffer of the given \a width and \a height.

    The \a attachment parameter describes the depth/stencil buffer
    configuration, \a target the texture target and \a internal_format
    the internal texture format. The default texture target is \c
    GL_TEXTURE_2D, while the default internal format is \c GL_RGBA8.

    \sa size(), texture(), attachment()
*/
QGLFramebufferObject::QGLFramebufferObject(int width, int height, Attachment attachment,
                                           GLenum target, GLenum internal_format)
    : d_ptr(new QGLFramebufferObjectPrivate)
{
    Q_D(QGLFramebufferObject);
    d->init(QSize(width, height), attachment, target, internal_format);
}

#ifdef Q_MAC_COMPAT_GL_FUNCTIONS
/*! \internal */
QGLFramebufferObject::QGLFramebufferObject(int width, int height, Attachment attachment,
                                           QMacCompatGLenum target, QMacCompatGLenum internal_format)
    : d_ptr(new QGLFramebufferObjectPrivate)
{
    Q_D(QGLFramebufferObject);
    d->init(QSize(width, height), attachment, target, internal_format);
}
#endif

/*! \overload

    Constructs an OpenGL framebuffer object and binds a texture to the
    buffer of the given \a size.

    The \a attachment parameter describes the depth/stencil buffer
    configuration, \a target the texture target and \a internal_format
    the internal texture format. The default texture target is \c
    GL_TEXTURE_2D, while the default internal format is \c GL_RGBA8.

    \sa size(), texture(), attachment()
*/
QGLFramebufferObject::QGLFramebufferObject(const QSize &size, Attachment attachment,
                                           GLenum target, GLenum internal_format)
    : d_ptr(new QGLFramebufferObjectPrivate)
{
    Q_D(QGLFramebufferObject);
    d->init(size, attachment, target, internal_format);
}

#ifdef Q_MAC_COMPAT_GL_FUNCTIONS
/*! \internal */
QGLFramebufferObject::QGLFramebufferObject(const QSize &size, Attachment attachment,
                                           QMacCompatGLenum target, QMacCompatGLenum internal_format)
    : d_ptr(new QGLFramebufferObjectPrivate)
{
    Q_D(QGLFramebufferObject);
    d->init(size, attachment, target, internal_format);
}
#endif

/*!
    \fn QGLFramebufferObject::~QGLFramebufferObject()

    Destroys the framebuffer object and frees any allocated resources.
*/
QGLFramebufferObject::~QGLFramebufferObject()
{
    Q_D(QGLFramebufferObject);
    QGL_FUNC_CONTEXT;

    if (isValid()
        && (d->ctx == QGLContext::currentContext()
            || qgl_share_reg()->checkSharing(d->ctx, QGLContext::currentContext())))
    {
        glDeleteTextures(1, &d->texture);
        if (d->depth_stencil_buffer)
            glDeleteRenderbuffersEXT(1, &d->depth_stencil_buffer);
        glDeleteFramebuffersEXT(1, &d->fbo);
    }
    delete d_ptr;
}

/*!
    \fn bool QGLFramebufferObject::isValid() const

    Returns true if the framebuffer object is valid.

    The framebuffer can become invalid if the initialization process
    fails, the user attaches an invalid buffer to the framebuffer
    object, or a non-power of 2 width/height is specified as the
    texture size if the texture target is \c{GL_TEXTURE_2D}.
*/
bool QGLFramebufferObject::isValid() const
{
    Q_D(const QGLFramebufferObject);
    return d->valid;
}

/*!
    \fn bool QGLFramebufferObject::bind()

    Switches rendering from the default, windowing system provided
    framebuffer to this framebuffer object.
    Returns true upon success, false otherwise.
*/
bool QGLFramebufferObject::bind()
{
    if (!isValid())
	return false;
    Q_D(QGLFramebufferObject);
    QGL_FUNC_CONTEXT;
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, d->fbo);
    d->bound = d->valid = d->checkFramebufferStatus();
    return d->valid;
}

/*!
    \fn bool QGLFramebufferObject::release()

    Switches rendering back to the default, windowing system provided
    framebuffer.
    Returns true upon success, false otherwise.
*/
bool QGLFramebufferObject::release()
{
    if (!isValid())
	return false;
    Q_D(QGLFramebufferObject);
    QGL_FUNC_CONTEXT;
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    d->valid = d->checkFramebufferStatus();
    d->bound = false;
    return d->valid;
}

/*!
    \fn GLuint QGLFramebufferObject::texture() const

    Returns the texture id for the texture attached as the default
    rendering target in this framebuffer object. This texture id can
    be bound as a normal texture in your own GL code.
*/
GLuint QGLFramebufferObject::texture() const
{
    Q_D(const QGLFramebufferObject);
    return d->texture;
}

/*!
    \fn QSize QGLFramebufferObject::size() const

    Returns the size of the texture attached to this framebuffer
    object.
*/
QSize QGLFramebufferObject::size() const
{
    Q_D(const QGLFramebufferObject);
    return d->size;
}

/*!
    \fn QImage QGLFramebufferObject::toImage() const

    Returns the contents of this framebuffer object as a QImage.
*/
QImage QGLFramebufferObject::toImage() const
{
    Q_D(const QGLFramebufferObject);
    if (!d->valid)
        return QImage();

    const_cast<QGLFramebufferObject *>(this)->bind();
    QImage image = qt_gl_read_framebuffer(d->size, d->ctx->format().alpha(), true);
    const_cast<QGLFramebufferObject *>(this)->release();

    return image;
}

#if !defined(QT_OPENGL_ES_2)
Q_GLOBAL_STATIC(QOpenGLPaintEngine, qt_buffer_paintengine)
#endif

/*! \reimp */
QPaintEngine *QGLFramebufferObject::paintEngine() const
{
#if !defined(QT_OPENGL_ES_2)
    return qt_buffer_paintengine();
#else
    return 0;
#endif
}

/*!
    \fn bool QGLFramebufferObject::hasOpenGLFramebufferObjects()

    Returns true if the OpenGL \c{GL_EXT_framebuffer_object} extension
    is present on this system; otherwise returns false.
*/
bool QGLFramebufferObject::hasOpenGLFramebufferObjects()
{
    QGLWidget dmy; // needed to detect and init the QGLExtensions object
    return (QGLExtensions::glExtensions & QGLExtensions::FramebufferObject);
}

/*!
    \since 4.4

    Draws the given texture, \a textureId, to the given target rectangle,
    \a target, in OpenGL model space. The \a textureTarget should be a 2D
    texture target.

    The framebuffer object should be bound when calling this function.

    Equivalent to the corresponding QGLContext::drawTexture().
*/
void QGLFramebufferObject::drawTexture(const QRectF &target, GLuint textureId, GLenum textureTarget)
{
    Q_D(QGLFramebufferObject);
    d->ctx->drawTexture(target, textureId, textureTarget);
}

#ifdef Q_MAC_COMPAT_GL_FUNCTIONS
/*! \internal */
void QGLFramebufferObject::drawTexture(const QRectF &target, QMacCompatGLuint textureId, QMacCompatGLenum textureTarget)
{
    Q_D(QGLFramebufferObject);
    d->ctx->drawTexture(target, textureId, textureTarget);
}
#endif

/*!
    \since 4.4

    Draws the given texture, \a textureId, at the given \a point in OpenGL
    model space. The \a textureTarget should be a 2D texture target.

    The framebuffer object should be bound when calling this function.

    Equivalent to the corresponding QGLContext::drawTexture().
*/
void QGLFramebufferObject::drawTexture(const QPointF &point, GLuint textureId, GLenum textureTarget)
{
    Q_D(QGLFramebufferObject);
    d->ctx->drawTexture(point, textureId, textureTarget);
}

#ifdef Q_MAC_COMPAT_GL_FUNCTIONS
/*! \internal */
void QGLFramebufferObject::drawTexture(const QPointF &point, QMacCompatGLuint textureId, QMacCompatGLenum textureTarget)
{
    Q_D(QGLFramebufferObject);
    d->ctx->drawTexture(point, textureId, textureTarget);
}
#endif

extern int qt_defaultDpi();

/*! \reimp */
int QGLFramebufferObject::metric(PaintDeviceMetric metric) const
{
    Q_D(const QGLFramebufferObject);

    float dpmx = qt_defaultDpi()*100./2.54;
    float dpmy = qt_defaultDpi()*100./2.54;
    int w = d->size.width();
    int h = d->size.height();
    switch (metric) {
    case PdmWidth:
        return w;

    case PdmHeight:
        return h;

    case PdmWidthMM:
        return qRound(w * 1000 / dpmx);

    case PdmHeightMM:
        return qRound(h * 1000 / dpmy);

    case PdmNumColors:
        return 0;

    case PdmDepth:
        return 32;//d->depth;

    case PdmDpiX:
        return (int)(dpmx * 0.0254);

    case PdmDpiY:
        return (int)(dpmy * 0.0254);

    case PdmPhysicalDpiX:
        return (int)(dpmx * 0.0254);

    case PdmPhysicalDpiY:
        return (int)(dpmy * 0.0254);

    default:
        qWarning("QGLFramebufferObject::metric(), Unhandled metric type: %d.\n", metric);
        break;
    }
    return 0;
}

/*!
    \fn GLuint QGLFramebufferObject::handle() const

    Returns the GL framebuffer object handle for this framebuffer
    object (returned by the \c{glGenFrameBuffersEXT()} function). This
    handle can be used to attach new images or buffers to the
    framebuffer. The user is responsible for cleaning up and
    destroying these objects.
*/
GLuint QGLFramebufferObject::handle() const
{
    Q_D(const QGLFramebufferObject);
    return d->fbo;
}

/*! \fn int QGLFramebufferObject::devType() const

    \reimp
*/


/*!
    Returns the status of the depth and stencil buffers attached to
    this framebuffer object.
*/

QGLFramebufferObject::Attachment QGLFramebufferObject::attachment() const
{
    Q_D(const QGLFramebufferObject);
    if (d->valid)
        return d->fbo_attachment;
    return NoAttachment;
}

/*!
    \since 4.5

    Returns true if the framebuffer object is currently bound to a context,
    otherwise false is returned.
*/

bool QGLFramebufferObject::isBound() const
{
    Q_D(const QGLFramebufferObject);
    return d->bound;
}

QT_END_NAMESPACE
