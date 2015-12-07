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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

/*
Uniform Types in OpenGL ES 2.0
==============================
GL_FLOAT        GLfloat         GLfloat
GL_FLOAT_VEC2   QGLVec2         GLfloat[2]
GL_FLOAT_VEC3   QGLVec3         GLfloat[3]
GL_FLOAT_VEC4   QGLVec4         GLfloat[4]
GL_INT          GLint           GLint
GL_INT_VEC2     QGLIVec2        GLint[2]
GL_INT_VEC3     QGLIVec3        GLint[3]
GL_INT_VEC4     QGLIVec4        GLint[4]
GL_BOOL         GLbool          GLbool
GL_BOOL_VEC2    QGLBVec2        GLbool[2]
GL_BOOL_VEC3    QGLBVec3        GLbool[3]
GL_BOOL_VEC4    QGLBVec4        GLbool[4]
GL_FLOAT_MAT2   QGLMat2         GLfloat[2][2]
GL_FLOAT_MAT3   QGLMat3         GLfloat[3][3]
GL_FLOAT_MAT4   QGLMat4         GLfloat[4][4]
GL_SAMPLER_2D   QGLSampler2D    GLuint
GL_SAMPLER_CUBE QGLSamplerCube  GLuint

Additional Types in Desktop OpenGL 2.0
======================================
SAMPLER_1D,
SAMPLER_3D,
SAMPLER_1D_SHADOW,
SAMPLER_2D_SHADOW.
*/

#include <QtOpenGL>


typedef struct {
    GLfloat a;
    GLfloat b;
} QGLVec2;

typedef struct {
    GLfloat a;
    GLfloat b;
    GLfloat c;
} QGLVec3;

typedef struct {
    GLfloat a;
    GLfloat b;
    GLfloat c;
    GLfloat d;
} QGLVec4;


class QGLShaderProgram;

class QGLShaderPrivate;

class QGLShader : QObject
{
    Q_OBJECT
public:
    enum ShaderType {VertexShader, FragmentShader};

    QGLShader(ShaderType type, const QGLContext* ctx = 0);

    GLuint id();
    void clearSource();
    void addSource(const QLatin1String& newSource);
    bool compile();
    bool isValid();
    QString log();
    const QGLContext* context(); //maybe make private with prog a friend?

private:
    QGLShaderPrivate* d_ptr;
    Q_DECLARE_PRIVATE(QGLShader);

/*
public slots:
    void cleanupGLContextRefs(const QGLContext *context);
*/
};


enum QGLType {
    QGLInvalidType      = 0,
    QGLFloatType        = GL_FLOAT,
    QGLVec2Type         = GL_FLOAT_VEC2,
    QGLVec3Type         = GL_FLOAT_VEC3,
    QGLVec4Type         = GL_FLOAT_VEC4,
    QGLIntType          = GL_INT,
    QGLIVec2Type        = GL_INT_VEC2,
    QGLIVec3Type        = GL_INT_VEC3,
    QGLIVec4Type        = GL_INT_VEC4,
    QGLBoolType         = GL_BOOL,
    QGLBVec2Type        = GL_BOOL_VEC2,
    QGLBVec3Type        = GL_BOOL_VEC3,
    QGLBVec4Type        = GL_BOOL_VEC4,
    QGLMat2Type         = GL_FLOAT_MAT2,
    QGLMat3Type         = GL_FLOAT_MAT3,
    QGLMat4Type         = GL_FLOAT_MAT4,
    QGLSampler2DType    = GL_SAMPLER_2D,
    QGLSamplerCubeType  = GL_SAMPLER_CUBE
};

class QGLUniform
{
public:

    QGLUniform(GLenum glType, GLint location, QGLContext* context)
        : m_id(location), m_type(QGLType(glType)), ctx(context) {}
    
    QGLUniform(); // Called by QMap when there's no match on the name

    QGLType type() const {return m_type;}
    GLuint  id()   const {return m_id;}

    // Seems odd to be const, but it doesn't actually modify any of the
    // class members, only the GL state!
    const QGLUniform& operator=(const GLfloat&) const;

    const QGLUniform& operator=(const QGLVec2&) const;
    const QGLUniform& operator=(const QSizeF&) const;
    const QGLUniform& operator=(const QPointF&) const;

    const QGLUniform& operator=(const QGLVec3&) const;

    const QGLUniform& operator=(const QGLVec4&) const;
    const QGLUniform& operator=(const QColor&) const;

    const QGLUniform& operator=(const GLfloat[2][2]) const;

    const QGLUniform& operator=(const GLfloat[3][3]) const;
    const QGLUniform& operator=(const QTransform&) const;

    const QGLUniform& operator=(const GLfloat[4][4]) const;

    const QGLUniform& operator=(const GLuint&) const;   // sampler2d, specifying a texture unit


protected:
    GLuint      m_id;
    QGLType     m_type;
    QGLContext* ctx;
};

typedef QMap<QString, QGLUniform> QGLUniformList;
typedef QMapIterator<QString, QGLUniform> QGLUniformListIterator;


class QGLVertexAttribute
{
public:
    QGLVertexAttribute(GLenum glType, GLuint location, QGLContext* context)
        : m_id(location), m_type(QGLType(glType)), ctx(context) {}

    QGLVertexAttribute(); // Called by QMap when there's no match on the name

    QGLType type() const {return m_type;}
    GLuint  id()   const {return m_id;}
    void    enable() const;
    void    disable() const;

    const QGLVertexAttribute& operator=(const GLfloat* rhs) const;
    const QGLVertexAttribute& operator=(const QGLVec3* rhs) const;

protected:
    GLuint        m_id;
    QGLType       m_type;
    QGLContext*   ctx;
};

//TODO: Convert into setter overloads on QGLShaderProgram
typedef QMap<QString, QGLVertexAttribute> QGLVertexAttributeList;
typedef QMapIterator<QString, QGLVertexAttribute> QGLVertexAttributeListIterator;



class QGLShaderProgramPrivate;

class QGLShaderProgram : QObject
{
    Q_OBJECT
public:
    QGLShaderProgram(const QGLContext* ctx = 0);

    const QGLUniformList &          uniforms();
    const QGLVertexAttributeList&   vertexAttributes();

    bool addShader(QGLShader* newShader);
    bool removeShader(QGLShader* oldShader);
    bool removeAllShaders();

    bool link();
    QString log();
    bool isValid();
    void use();

    GLuint id();

private:
    QGLShaderProgramPrivate* d_ptr;
    Q_DECLARE_PRIVATE(QGLShaderProgram);

/*
public slots:
    void cleanupGLContextRefs(const QGLContext *context);
*/
};

