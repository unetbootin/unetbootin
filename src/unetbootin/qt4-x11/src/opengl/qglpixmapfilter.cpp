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

#include "private/qpixmapfilter_p.h"
#include "qglpixmapfilter_p.h"
#include "qgraphicssystem_gl_p.h"
#include "qpaintengine_opengl_p.h"

#include "qglpixelbuffer.h"
#include "qglextensions_p.h"
#include "qgl_p.h"

#include "private/qapplication_p.h"


#ifndef GL_FRAGMENT_SHADER
#define GL_FRAGMENT_SHADER 0x8B30
#endif

#ifndef GL_COMPILE_STATUS
#define GL_COMPILE_STATUS 0x8B81
#endif

#ifndef GL_LINK_STATUS
#define GL_LINK_STATUS 0x8B82
#endif




QT_BEGIN_NAMESPACE


void QGLPixmapFilterBase::bindTexture(const QPixmap &src) const
{
    const_cast<QGLContext *>(QGLContext::currentContext())->d_func()->bindTexture(src, GL_TEXTURE_2D, GL_RGBA, true);
}

void QGLPixmapFilterBase::drawImpl(QPainter *painter, const QPointF &pos, const QPixmap &src, const QRectF& source) const
{
    processGL(painter, pos, src, source);
}

QGLSLProgram::QGLSLProgram(const QString &src)
    : ctx(QGLContext::currentContext())
{
    if (!qt_resolve_glsl_extensions(const_cast<QGLContext *>(ctx))) {
        qWarning("Failed to resolve GLSL functions");
        m_valid = false;
        return;
    }

    m_shader = glCreateShader(GL_FRAGMENT_SHADER);

    const QByteArray src_ba = src.toAscii();
    const char *src_cstr = src_ba.constData();

    glShaderSource(m_shader, 1, &src_cstr, 0);
    glCompileShader(m_shader);
    glGetShaderiv(m_shader, GL_COMPILE_STATUS, &m_valid);
    if (!m_valid) {
        char data[4096];
        GLsizei len;
        glGetShaderInfoLog(m_shader, 4096, &len, data);
        qWarning("Failed to compile GLSL shader:\n%s\nCODE:\n%s\n", data, src_cstr);
        return;
    }

    m_program = glCreateProgram();
    glAttachShader(m_program, m_shader);
    glLinkProgram(m_program);
    glGetProgramiv(m_program, GL_LINK_STATUS, &m_valid);
    if (!m_valid) {
        char data[4096];
        GLsizei len;
        glGetShaderInfoLog(m_shader, 4096, &len, data);
        qWarning("Failed to link GLSL program:\n%s\nCODE:\n%s\n", data, src_cstr);
        return;
    }
}

QGLSLProgram::~QGLSLProgram()
{
    glDeleteProgram(m_program);
    glDeleteShader(m_shader);
}

bool QGLSLProgram::success() const
{
    return m_valid;
}

void QGLSLProgram::enable()
{
    glUseProgram(m_program);
}

void QGLSLProgram::disable()
{
    glUseProgram(0);
}

typedef GLuint (APIENTRY *_glGetUniformLocation) (GLuint, const char*);
typedef void (APIENTRY *_glUniform4fv) (GLint, GLsizei, GLfloat *);
typedef void (APIENTRY *_glUniform3fv) (GLint, GLsizei, GLfloat *);
typedef void (APIENTRY *_glUniform2fv) (GLint, GLsizei, GLfloat *);
typedef void (APIENTRY *_glUniform1fv) (GLint, GLsizei, GLfloat *);
typedef void (APIENTRY *_glUniform1i) (GLint, GLint);

int QGLSLProgram::getUniformLocation(const QString &name)
{
    return glGetUniformLocation(m_program, name.toAscii().constData());
}

void QGLSLProgram::setUniform(int uniform, int value)
{
    enable();
    glUniform1i(uniform, value);
}

void QGLSLProgram::setUniform(int uniform, qreal value)
{
    enable();
    GLfloat v[] = { value };
    glUniform1fv(uniform, 1, v);
}

void QGLSLProgram::setUniform(int uniform, qreal v1, qreal v2)
{
    enable();
    GLfloat v[] = { v1, v2 };
    glUniform2fv(uniform, 1, v);
}

void QGLSLProgram::setUniform(int uniform, qreal v1, qreal v2, qreal v3)
{
    enable();
    GLfloat v[] = { v1, v2, v3 };
    glUniform3fv(uniform, 1, v);
}

void QGLSLProgram::setUniform(int uniform, qreal v1, qreal v2, qreal v3, qreal v4)
{
    enable();
    GLfloat v[] = { v1, v2, v3, v4 };
    glUniform4fv(uniform, 1, v);
}

void QGLSLProgram::setUniform(int uniform, int count, float *v)
{
    enable();
    glUniform1fv(uniform, count, v);
}

class QGLPixmapColorizeFilter: public QGLPixmapFilter<QPixmapColorizeFilter>
{
public:
    QGLPixmapColorizeFilter();

protected:
    bool processGL(QPainter *painter, const QPointF &pos, const QPixmap &pixmap, const QRectF &srcRect) const;

private:
    mutable QGLSLProgram m_program;
    uint m_colorUniform;
};

class QGLPixmapConvolutionFilter: public QGLPixmapFilter<QPixmapConvolutionFilter>
{
public:
    QGLPixmapConvolutionFilter();
    ~QGLPixmapConvolutionFilter();

protected:
    bool processGL(QPainter *painter, const QPointF &pos, const QPixmap &src, const QRectF &srcRect) const;

private:
    QString generateConvolutionShader() const;

    mutable QGLSLProgram *m_program;
    mutable uint m_scaleUniform;
    mutable uint m_matrixUniform;

    mutable int m_kernelWidth;
    mutable int m_kernelHeight;
};

extern QGLWidget *qt_gl_share_widget();

QPixmapFilter *QGLContextPrivate::createPixmapFilter(int type) const
{
    switch (type) {
    case QPixmapFilter::ColorizeFilter:
        return new QGLPixmapColorizeFilter;


    case QPixmapFilter::ConvolutionFilter:
        return new QGLPixmapConvolutionFilter;

    default:
        return 0;
        break;
    }
    return 0;
}

#if !defined(QT_OPENGL_ES_2)
extern void qt_add_rect_to_array(const QRectF &r, q_vertexType *array);
extern void qt_add_texcoords_to_array(qreal x1, qreal y1, qreal x2, qreal y2, q_vertexType *array);
#endif

static void qgl_drawTexture(const QRectF &rect, int tx_width, int tx_height, const QRectF & src)
{
#ifndef QT_OPENGL_ES_2  // XXX: needs to be ported
#ifndef QT_OPENGL_ES
    glPushAttrib(GL_CURRENT_BIT);
#endif
    qreal x1, x2, y1, y2;

    x1 = src.x() / tx_width;
    x2 = x1 + src.width() / tx_width;
    y1 = 1.0 - ((src.y() / tx_height) + (src.height() / tx_height));
    y2 = 1.0 - (src.y() / tx_height);

    q_vertexType vertexArray[4*2];
    q_vertexType texCoordArray[4*2];

    qt_add_rect_to_array(rect, vertexArray);
    qt_add_texcoords_to_array(x1, y2, x2, y1, texCoordArray);

    glVertexPointer(2, q_vertexTypeEnum, 0, vertexArray);
    glTexCoordPointer(2, q_vertexTypeEnum, 0, texCoordArray);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);

#ifndef QT_OPENGL_ES
    glPopAttrib();
#endif
#endif
}

static const char *qt_gl_colorize_filter =
        "uniform sampler2D texture;"
        "uniform vec3 color;"
        "void main(void)"
        "{"
        "        vec2 coords = gl_TexCoord[0].st;"
        "        vec4 src = texture2D(texture, coords);"
        "        float gray = dot(src.rgb, vec3(0.212671, 0.715160, 0.072169));"
        "        vec3 colorizeed = 1.0-((1.0-gray)*(1.0-color));"
        "        gl_FragColor = vec4(colorizeed, src.a);"
        "}";

QGLPixmapColorizeFilter::QGLPixmapColorizeFilter()
    : m_program(QLatin1String(qt_gl_colorize_filter))
{
    m_program.setUniform(m_program.getUniformLocation(QLatin1String("texture")), 0); // GL_TEXTURE_0
    m_colorUniform = m_program.getUniformLocation(QLatin1String("color"));
}

bool QGLPixmapColorizeFilter::processGL(QPainter *, const QPointF &pos, const QPixmap &src, const QRectF &srcRect) const
{
    bindTexture(src);

    QColor col = color();
    m_program.setUniform(m_colorUniform, col.redF(), col.greenF(), col.blueF());

    QRectF target = (srcRect.isNull() ? QRectF(src.rect()) : srcRect).translated(pos);
    m_program.enable();
    qgl_drawTexture(target, src.width(), src.height(), srcRect);
    m_program.disable();

    return true;
}

// generates convolution filter code for arbitrary sized kernel
QString QGLPixmapConvolutionFilter::generateConvolutionShader() const {
    QByteArray code;
    code.append("uniform sampler2D texture;\n");
    code.append("uniform vec2 inv_texture_size;\n");
    code.append("uniform float matrix[");
    code.append(QByteArray::number(m_kernelWidth * m_kernelHeight));
    code.append("];\n");
    code.append("vec2 offset[");
    code.append(QByteArray::number(m_kernelWidth*m_kernelHeight));
    code.append("];\n");
    code.append("void main(void) {\n");

    for(int y = 0; y < m_kernelHeight; y++) {
        for(int x = 0; x < m_kernelWidth; x++) {
            code.append("  offset[");
            code.append(QByteArray::number(y * m_kernelWidth + x));
            code.append("] = vec2(inv_texture_size.x * ");
            code.append(QByteArray::number(x-(int)(m_kernelWidth/2)));
            code.append(".0, inv_texture_size.y * ");
            code.append(QByteArray::number((int)(m_kernelHeight/2)-y));
            code.append(".0)");
            code.append(";\n");
        }
    }

    code.append("  int i = 0;\n");
    code.append("  vec2 coords = gl_TexCoord[0].st;\n");
    code.append("  vec4 sum = vec4(0.0);\n");
    code.append("  for (i = 0; i < ");
    code.append(QByteArray::number(m_kernelWidth * m_kernelHeight));
    code.append("; i++) {\n");
    code.append("    vec4 tmp = texture2D(texture,coords+offset[i]);\n");
    code.append("    sum += matrix[i] * tmp;\n");
    code.append("  }\n");
    code.append("  gl_FragColor = sum;\n");
    code.append("}");
    return QLatin1String(code);
}

QGLPixmapConvolutionFilter::QGLPixmapConvolutionFilter()
    : m_program(0)
    , m_scaleUniform(0)
    , m_matrixUniform(0)
    , m_kernelWidth(0)
    , m_kernelHeight(0)
{
}

QGLPixmapConvolutionFilter::~QGLPixmapConvolutionFilter()
{
    delete m_program;
}

bool QGLPixmapConvolutionFilter::processGL(QPainter *, const QPointF &pos, const QPixmap &src, const QRectF &srcRect) const
{
    QRectF target = (srcRect.isNull() ? QRectF(src.rect()) : srcRect).translated(pos);

    bindTexture(src);
#ifdef GL_CLAMP
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
#endif
    if (!m_program || m_kernelWidth != columns() || m_kernelHeight != rows()) {
        delete m_program;

        m_kernelWidth = columns();
        m_kernelHeight = rows();

        QString code = generateConvolutionShader();
        m_program = new QGLSLProgram(code);
        m_scaleUniform = m_program->getUniformLocation(QLatin1String("inv_texture_size"));
        m_matrixUniform = m_program->getUniformLocation(QLatin1String("matrix"));
    }

    const qreal *kernel = convolutionKernel();
    GLfloat *conv = new GLfloat[m_kernelWidth * m_kernelHeight];
    for(int i = 0; i < m_kernelWidth * m_kernelHeight; ++i)
        conv[i] = kernel[i];

    const qreal iw = 1.0 / src.width();
    const qreal ih = 1.0 / src.height();
    m_program->setUniform(m_scaleUniform, iw, ih);
    m_program->setUniform(m_matrixUniform, m_kernelWidth * m_kernelHeight, conv);

    m_program->enable();
    qgl_drawTexture(target, src.width(), src.height(), boundingRectFor(srcRect));
    m_program->disable();
    return true;
}

QT_END_NAMESPACE
