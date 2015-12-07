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

#ifndef QGL_H
#define QGL_H

#include <QtGui/qwidget.h>
#include <QtOpenGL/qglcolormap.h>
#include <QtCore/qmap.h>

QT_BEGIN_HEADER

#if defined(Q_WS_WIN)
# include <QtCore/qt_windows.h>
#endif

#if defined(Q_WS_MAC)
# include <OpenGL/gl.h>
# include <OpenGL/glu.h>
#elif defined(QT_OPENGL_ES_1) || defined(QT_OPENGL_ES_1_CL)
# include <GLES/gl.h>
#ifndef GL_DOUBLE
# define GL_DOUBLE GL_FLOAT
#endif
#ifndef GLdouble
typedef GLfloat GLdouble;
#endif
#elif defined(QT_OPENGL_ES_2)
# include <GLES2/gl2.h>
#ifndef GL_DOUBLE
# define GL_DOUBLE GL_FLOAT
#endif
#ifndef GLdouble
typedef GLfloat GLdouble;
#endif
#else
# include <GL/gl.h>
# ifndef QT_LINUXBASE
#   include <GL/glu.h>
# endif
#endif

QT_BEGIN_NAMESPACE

QT_MODULE(OpenGL)

#if defined(Q_WS_MAC) && defined (QT_BUILD_OPENGL_LIB) && !defined(QT_MAC_USE_COCOA) && !defined(QDOC)
#define Q_MAC_COMPAT_GL_FUNCTIONS

template <typename T>
struct QMacGLCompatTypes
{
    typedef long CompatGLint;
    typedef unsigned long CompatGLuint;
    typedef unsigned long CompatGLenum;
};

template <>
struct QMacGLCompatTypes<long>
{
    typedef int CompatGLint;
    typedef unsigned int CompatGLuint;
    typedef unsigned int CompatGLenum;
};

typedef QMacGLCompatTypes<GLint>::CompatGLint QMacCompatGLint;
typedef QMacGLCompatTypes<GLint>::CompatGLuint QMacCompatGLuint;
typedef QMacGLCompatTypes<GLint>::CompatGLenum QMacCompatGLenum;

#endif

#ifdef QT3_SUPPORT
#define QGL_VERSION        460
#define QGL_VERSION_STR        "4.6"
inline QT3_SUPPORT const char *qGLVersion() {
    return QGL_VERSION_STR;
}
#endif

#if defined(Q_WS_WIN) || defined(Q_WS_MAC)
class QGLCmap;
#endif

class QPixmap;
#if defined(Q_WS_X11) && !defined(QT_OPENGL_ES)
class QGLOverlayWidget;
#endif
class QGLWidgetPrivate;
class QGLContextPrivate;

// Namespace class:
namespace QGL
{
    enum FormatOption {
        DoubleBuffer            = 0x0001,
        DepthBuffer             = 0x0002,
        Rgba                    = 0x0004,
        AlphaChannel            = 0x0008,
        AccumBuffer             = 0x0010,
        StencilBuffer           = 0x0020,
        StereoBuffers           = 0x0040,
        DirectRendering         = 0x0080,
        HasOverlay              = 0x0100,
        SampleBuffers           = 0x0200,
        SingleBuffer            = DoubleBuffer    << 16,
        NoDepthBuffer           = DepthBuffer     << 16,
        ColorIndex              = Rgba            << 16,
        NoAlphaChannel          = AlphaChannel    << 16,
        NoAccumBuffer           = AccumBuffer     << 16,
        NoStencilBuffer         = StencilBuffer   << 16,
        NoStereoBuffers         = StereoBuffers   << 16,
        IndirectRendering       = DirectRendering << 16,
        NoOverlay               = HasOverlay      << 16,
        NoSampleBuffers         = SampleBuffers   << 16
    };
    Q_DECLARE_FLAGS(FormatOptions, FormatOption)
}

Q_DECLARE_OPERATORS_FOR_FLAGS(QGL::FormatOptions)

class QGLFormatPrivate;

class Q_OPENGL_EXPORT QGLFormat
{
public:
    QGLFormat();
    QGLFormat(QGL::FormatOptions options, int plane = 0);
    QGLFormat(const QGLFormat &other);
    QGLFormat &operator=(const QGLFormat &other);
    ~QGLFormat();

    void setDepthBufferSize(int size);
    int  depthBufferSize() const;

    void setAccumBufferSize(int size);
    int  accumBufferSize() const;

    void setRedBufferSize(int size);
    int  redBufferSize() const;

    void setGreenBufferSize(int size);
    int  greenBufferSize() const;

    void setBlueBufferSize(int size);
    int  blueBufferSize() const;

    void setAlphaBufferSize(int size);
    int  alphaBufferSize() const;

    void setStencilBufferSize(int size);
    int  stencilBufferSize() const;

    void setSampleBuffers(bool enable);
    bool sampleBuffers() const;

    void setSamples(int numSamples);
    int  samples() const;

    void setSwapInterval(int interval);
    int  swapInterval() const;

    bool doubleBuffer() const;
    void setDoubleBuffer(bool enable);
    bool depth() const;
    void setDepth(bool enable);
    bool rgba() const;
    void setRgba(bool enable);
    bool alpha() const;
    void setAlpha(bool enable);
    bool accum() const;
    void setAccum(bool enable);
    bool stencil() const;
    void setStencil(bool enable);
    bool stereo() const;
    void setStereo(bool enable);
    bool directRendering() const;
    void setDirectRendering(bool enable);
    bool hasOverlay() const;
    void setOverlay(bool enable);

    int plane() const;
    void setPlane(int plane);

    void setOption(QGL::FormatOptions opt);
    bool testOption(QGL::FormatOptions opt) const;

    static QGLFormat defaultFormat();
    static void setDefaultFormat(const QGLFormat& f);

    static QGLFormat defaultOverlayFormat();
    static void setDefaultOverlayFormat(const QGLFormat& f);

    static bool hasOpenGL();
    static bool hasOpenGLOverlays();

    enum  OpenGLVersionFlag {
        OpenGL_Version_None               = 0x00000000,
        OpenGL_Version_1_1                = 0x00000001,
        OpenGL_Version_1_2                = 0x00000002,
        OpenGL_Version_1_3                = 0x00000004,
        OpenGL_Version_1_4                = 0x00000008,
        OpenGL_Version_1_5                = 0x00000010,
        OpenGL_Version_2_0                = 0x00000020,
        OpenGL_Version_2_1                = 0x00000040,
        OpenGL_ES_Common_Version_1_0      = 0x00000080,
        OpenGL_ES_CommonLite_Version_1_0  = 0x00000100,
        OpenGL_ES_Common_Version_1_1      = 0x00000200,
        OpenGL_ES_CommonLite_Version_1_1  = 0x00000400,
        OpenGL_ES_Version_2_0             = 0x00000800,
        OpenGL_Version_3_0                = 0x00001000
    };
    Q_DECLARE_FLAGS(OpenGLVersionFlags, OpenGLVersionFlag)

    static OpenGLVersionFlags openGLVersionFlags();

private:
    QGLFormatPrivate *d;

    friend Q_OPENGL_EXPORT bool operator==(const QGLFormat&, const QGLFormat&);
    friend Q_OPENGL_EXPORT bool operator!=(const QGLFormat&, const QGLFormat&);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QGLFormat::OpenGLVersionFlags)

Q_OPENGL_EXPORT bool operator==(const QGLFormat&, const QGLFormat&);
Q_OPENGL_EXPORT bool operator!=(const QGLFormat&, const QGLFormat&);

class Q_OPENGL_EXPORT QGLContext
{
    Q_DECLARE_PRIVATE(QGLContext)
public:
    QGLContext(const QGLFormat& format, QPaintDevice* device);
    QGLContext(const QGLFormat& format);
    virtual ~QGLContext();

    virtual bool create(const QGLContext* shareContext = 0);
    bool isValid() const;
    bool isSharing() const;
    void reset();

    // ### Qt 5: make format() return a const ref instead
    QGLFormat format() const;
    QGLFormat requestedFormat() const;
    void setFormat(const QGLFormat& format);

    // ### Qt 5: return bools + maybe remove virtuals
    virtual void makeCurrent();
    virtual void doneCurrent();

    virtual void swapBuffers() const;

    GLuint bindTexture(const QImage &image, GLenum target = GL_TEXTURE_2D,
                       GLint format = GL_RGBA);
    GLuint bindTexture(const QPixmap &pixmap, GLenum target = GL_TEXTURE_2D,
                       GLint format = GL_RGBA);
    GLuint bindTexture(const QString &fileName);

    void deleteTexture(GLuint tx_id);

    void drawTexture(const QRectF &target, GLuint textureId, GLenum textureTarget = GL_TEXTURE_2D);
    void drawTexture(const QPointF &point, GLuint textureId, GLenum textureTarget = GL_TEXTURE_2D);

#ifdef Q_MAC_COMPAT_GL_FUNCTIONS
    GLuint bindTexture(const QImage &image, QMacCompatGLenum = GL_TEXTURE_2D,
                       QMacCompatGLint format = GL_RGBA);
    GLuint bindTexture(const QPixmap &pixmap, QMacCompatGLenum = GL_TEXTURE_2D,
                       QMacCompatGLint format = GL_RGBA);

    void deleteTexture(QMacCompatGLuint tx_id);

    void drawTexture(const QRectF &target, QMacCompatGLuint textureId, QMacCompatGLenum textureTarget = GL_TEXTURE_2D);
    void drawTexture(const QPointF &point, QMacCompatGLuint textureId, QMacCompatGLenum textureTarget = GL_TEXTURE_2D);
#endif

    static void setTextureCacheLimit(int size);
    static int textureCacheLimit();

    void *getProcAddress(const QString &proc) const;
    QPaintDevice* device() const;
    QColor overlayTransparentColor() const;

    static const QGLContext* currentContext();

protected:
    virtual bool chooseContext(const QGLContext* shareContext = 0);

#if defined(Q_WS_WIN)
    virtual int choosePixelFormat(void* pfd, HDC pdc);
#endif
#if defined(Q_WS_X11) && !defined(QT_OPENGL_ES)
    virtual void* tryVisual(const QGLFormat& f, int bufDepth = 1);
    virtual void* chooseVisual();
#endif
#if defined(Q_WS_MAC)
    virtual void* chooseMacVisual(GDHandle);
#endif

    bool deviceIsPixmap() const;
    bool windowCreated() const;
    void setWindowCreated(bool on);
    bool initialized() const;
    void setInitialized(bool on);
    void generateFontDisplayLists(const QFont & fnt, int listBase); // ### Qt 5: remove

    uint colorIndex(const QColor& c) const;
    void setValid(bool valid);
    void setDevice(QPaintDevice *pDev);

protected:
    static QGLContext* currentCtx;

private:
    QGLContextPrivate* d_ptr;

    friend class QGLPixelBuffer;
    friend class QGLPixelBufferPrivate;
    friend class QGLWidget;
    friend class QGLDrawable;
    friend class QGLWidgetPrivate;
    friend class QGLGlyphCache;
    friend class QOpenGLPaintEngine;
    friend class QOpenGLPaintEnginePrivate;
    friend class QGL2PaintEngineEx;
    friend class QGL2PaintEngineExPrivate;
    friend class QGLWindowSurface;
    friend class QGLPixmapData;
    friend class QGLPixmapFilterBase;
    friend QGLFormat::OpenGLVersionFlags QGLFormat::openGLVersionFlags();
#ifdef Q_WS_MAC
public:
    void updatePaintDevice();
private:
    friend class QMacGLWindowChangeEvent;
    friend QGLContextPrivate *qt_phonon_get_dptr(const QGLContext *);
#endif
#ifdef Q_WS_WIN
    friend class QGLFramebufferObject;
    friend class QGLFramebufferObjectPrivate;
    friend bool qt_resolve_GLSL_functions(QGLContext *ctx);
    friend bool qt_createGLSLProgram(QGLContext *ctx, GLuint &program, const char *shader_src, GLuint &shader);
#endif
private:
    Q_DISABLE_COPY(QGLContext)
};


class Q_OPENGL_EXPORT QGLWidget : public QWidget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QGLWidget)
public:
    explicit QGLWidget(QWidget* parent=0,
                       const QGLWidget* shareWidget = 0, Qt::WindowFlags f=0);
    explicit QGLWidget(QGLContext *context, QWidget* parent=0,
                       const QGLWidget* shareWidget = 0, Qt::WindowFlags f=0);
    explicit QGLWidget(const QGLFormat& format, QWidget* parent=0,
                       const QGLWidget* shareWidget = 0, Qt::WindowFlags f=0);
#ifdef QT3_SUPPORT
    QT3_SUPPORT_CONSTRUCTOR QGLWidget(QWidget* parent, const char* name,
                                    const QGLWidget* shareWidget = 0, Qt::WindowFlags f=0);
    QT3_SUPPORT_CONSTRUCTOR QGLWidget(QGLContext *context, QWidget* parent, const char* name,
                                    const QGLWidget* shareWidget = 0, Qt::WindowFlags f=0);
    QT3_SUPPORT_CONSTRUCTOR QGLWidget(const QGLFormat& format, QWidget* parent, const char* name,
                                    const QGLWidget* shareWidget = 0, Qt::WindowFlags f=0);
#endif
    ~QGLWidget();

    void qglColor(const QColor& c) const;
    void qglClearColor(const QColor& c) const;

    bool isValid() const;
    bool isSharing() const;

    // ### Qt 5: return bools
    void makeCurrent();
    void doneCurrent();

    bool doubleBuffer() const;
    void swapBuffers();

    // ### Qt 5: make format() return a const ref instead
    QGLFormat format() const;
    void setFormat(const QGLFormat& format);

    const QGLContext* context() const;
    void setContext(QGLContext* context, const QGLContext* shareContext = 0,
                    bool deleteOldContext = true);

    QPixmap renderPixmap(int w = 0, int h = 0, bool useContext = false);
    QImage grabFrameBuffer(bool withAlpha = false);

    void makeOverlayCurrent();
    const QGLContext* overlayContext() const;

    static QImage convertToGLFormat(const QImage& img);

    void setMouseTracking(bool enable);

    const QGLColormap & colormap() const;
    void  setColormap(const QGLColormap & map);

    void renderText(int x, int y, const QString & str,
                     const QFont & fnt = QFont(), int listBase = 2000);
    void renderText(double x, double y, double z, const QString & str,
                     const QFont & fnt = QFont(), int listBase = 2000);
    QPaintEngine *paintEngine() const;

    GLuint bindTexture(const QImage &image, GLenum target = GL_TEXTURE_2D,
                       GLint format = GL_RGBA);
    GLuint bindTexture(const QPixmap &pixmap, GLenum target = GL_TEXTURE_2D,
                       GLint format = GL_RGBA);
    GLuint bindTexture(const QString &fileName);

    void deleteTexture(GLuint tx_id);

    void drawTexture(const QRectF &target, GLuint textureId, GLenum textureTarget = GL_TEXTURE_2D);
    void drawTexture(const QPointF &point, GLuint textureId, GLenum textureTarget = GL_TEXTURE_2D);

#ifdef Q_MAC_COMPAT_GL_FUNCTIONS
    GLuint bindTexture(const QImage &image, QMacCompatGLenum = GL_TEXTURE_2D,
                       QMacCompatGLint format = GL_RGBA);
    GLuint bindTexture(const QPixmap &pixmap, QMacCompatGLenum = GL_TEXTURE_2D,
                       QMacCompatGLint format = GL_RGBA);

    void deleteTexture(QMacCompatGLuint tx_id);

    void drawTexture(const QRectF &target, QMacCompatGLuint textureId, QMacCompatGLenum textureTarget = GL_TEXTURE_2D);
    void drawTexture(const QPointF &point, QMacCompatGLuint textureId, QMacCompatGLenum textureTarget = GL_TEXTURE_2D);
#endif

public Q_SLOTS:
    virtual void updateGL();
    virtual void updateOverlayGL();

protected:
    bool event(QEvent *);
    virtual void initializeGL();
    virtual void resizeGL(int w, int h);
    virtual void paintGL();

    virtual void initializeOverlayGL();
    virtual void resizeOverlayGL(int w, int h);
    virtual void paintOverlayGL();

    void setAutoBufferSwap(bool on);
    bool autoBufferSwap() const;

    void paintEvent(QPaintEvent*);
    void resizeEvent(QResizeEvent*);

    virtual void glInit();
    virtual void glDraw();
    int fontDisplayListBase(const QFont & fnt, int listBase = 2000); // ### Qt 5: remove

private:
    Q_DISABLE_COPY(QGLWidget)

#ifdef Q_WS_MAC
    friend class QMacGLWindowChangeEvent;
#endif
    friend class QGLDrawable;
    friend class QGLPixelBuffer;
    friend class QGLPixelBufferPrivate;
    friend class QGLContext;
    friend class QGLOverlayWidget;
    friend class QOpenGLPaintEngine;
};


//
// QGLFormat inline functions
//

inline bool QGLFormat::doubleBuffer() const
{
    return testOption(QGL::DoubleBuffer);
}

inline bool QGLFormat::depth() const
{
    return testOption(QGL::DepthBuffer);
}

inline bool QGLFormat::rgba() const
{
    return testOption(QGL::Rgba);
}

inline bool QGLFormat::alpha() const
{
    return testOption(QGL::AlphaChannel);
}

inline bool QGLFormat::accum() const
{
    return testOption(QGL::AccumBuffer);
}

inline bool QGLFormat::stencil() const
{
    return testOption(QGL::StencilBuffer);
}

inline bool QGLFormat::stereo() const
{
    return testOption(QGL::StereoBuffers);
}

inline bool QGLFormat::directRendering() const
{
    return testOption(QGL::DirectRendering);
}

inline bool QGLFormat::hasOverlay() const
{
    return testOption(QGL::HasOverlay);
}

inline bool QGLFormat::sampleBuffers() const
{
    return testOption(QGL::SampleBuffers);
}

QT_END_NAMESPACE

QT_END_HEADER

#endif // QGL_H
