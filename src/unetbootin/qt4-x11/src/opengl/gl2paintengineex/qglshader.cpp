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

#include "qglshader_p.h"


// Windows needs to resolve OpenGL 2.0 function pointers for each context. The
// QGL OpenGL 2.0 functions are actually macros, which take a "ctx" parameter.
#define Q_CTX   QGLContext* ctx = d->ctx; \
                if (!ctx) \
                    return false; \
                ctx->makeCurrent(); \




class QGLShaderPrivate
{
public:
    QGLShaderPrivate() : shaderId(0), valid(false), ctx(0) {}

    GLuint                  shaderId;
    QString                 source;
    bool                    valid;
    QGLShader::ShaderType   type;
    QGLContext*             ctx;
};


QGLShader::QGLShader(QGLShader::ShaderType type, const QGLContext* ctx)
    : d_ptr(new QGLShaderPrivate)
{
    Q_D(QGLShader);

    if (!ctx)
        ctx = QGLContext::currentContext();

    if (!ctx) {
        qWarning("QGLShader being created without a context");
        return;
    }

    d->ctx = const_cast<QGLContext*>(ctx);
    d->ctx->makeCurrent();

    if (type == QGLShader::FragmentShader)
        d->shaderId = glCreateShader(GL_FRAGMENT_SHADER);
    else
        d->shaderId = glCreateShader(GL_VERTEX_SHADER);

    if (d->shaderId == 0) {
        qWarning("Error creating shader object");
        return;
    }

    d->type = type;
}

GLuint QGLShader::id()
{
    Q_D(QGLShader);
    return d->shaderId;
}

const QGLContext* QGLShader::context()
{
    Q_D(QGLShader);
    return d->ctx;
}

void QGLShader::clearSource()
{
    Q_D(QGLShader);
    d->source.clear();
    d->valid = false;
}

void QGLShader::addSource(const QLatin1String& newSource)
{
    Q_D(QGLShader);
    d->source += newSource;
    d->valid = false;
}


bool QGLShader::compile()
{
    Q_D(QGLShader);

    d->valid = false;

    if (d->source.size() == 0)
        return false;

    const QByteArray src_ba = d->source.toAscii();
    const char* src = src_ba.constData();

    glShaderSource(d->shaderId, 1, &src, 0);

    glCompileShader(d->shaderId);

    GLint shaderCompiled;
    glGetShaderiv(d->shaderId, GL_COMPILE_STATUS, &shaderCompiled);
    if (!shaderCompiled)
        return false;

    d->valid = true;
    return true;
}

bool QGLShader::isValid()
{
    Q_D(QGLShader);
    return d->valid;
}

QString QGLShader::log()
{
    Q_D(QGLShader);

    char*  logData;
    GLint  logSize;
    GLint  logLength;

    glGetShaderiv(d->shaderId, GL_INFO_LOG_LENGTH, &logSize);

    if (!logSize)
        return QString();

    logData = new char[logSize];
    glGetShaderInfoLog(d->shaderId, logSize, &logLength, logData);
    QString result = QString::fromAscii(logData);
    delete [] logData;

    return result;
}












class QGLShaderProgramPrivate
{
public:
    QGLShaderProgramPrivate() : valid(false), programId(0), ctx(0) {}
    void populateVariableLists();

    QVector<QGLShader*>     shaders;
    QGLUniformList          uniforms;
    QGLVertexAttributeList  attributeArrays;
    bool                    valid;
    GLuint                  programId;
    QGLContext*             ctx;
};



void QGLShaderProgramPrivate::populateVariableLists()
{
    attributeArrays.clear();
    uniforms.clear();

    int count;
    int sizeOfNameBuff;
    char* name;
    GLint nameLength;
    GLenum type;
    GLint  size;
    GLint  location;

    glGetProgramiv(programId, GL_ACTIVE_ATTRIBUTES, &count);
    glGetProgramiv(programId, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &sizeOfNameBuff);
    name = new char[sizeOfNameBuff];

    for (int i = 0; i < count; ++i) {
        nameLength = -1;
        glGetActiveAttrib(programId, i, sizeOfNameBuff, &nameLength, &size, &type, name);
        if (nameLength == -1)
            continue;

        location = glGetAttribLocation(programId, name);
        attributeArrays.insert(QString::fromAscii(name), QGLVertexAttribute(type, location, ctx));
    }

    delete [] name;


    glGetProgramiv(programId, GL_ACTIVE_UNIFORMS, &count);
    glGetProgramiv(programId, GL_ACTIVE_UNIFORM_MAX_LENGTH, &sizeOfNameBuff);

    name = new char[sizeOfNameBuff];

    for (int i = 0; i < count; ++i) {
        nameLength = -1;
        glGetActiveUniform(programId, i, sizeOfNameBuff, &nameLength, &size, &type, name);
        if (nameLength == -1)
            continue;

        location = glGetUniformLocation(programId, name);
        uniforms.insert(QString::fromAscii(name), QGLUniform(type, location, ctx));
    }
}


QGLShaderProgram::QGLShaderProgram(const QGLContext* ctx)
    : d_ptr(new QGLShaderProgramPrivate)
{
    Q_D(QGLShaderProgram);
    if (!ctx)
        ctx = QGLContext::currentContext();

    if (!ctx) {
        qWarning("QGLShaderProgram being created without a context");
        return;
    }

    d->ctx = const_cast<QGLContext*>(ctx);
    d->ctx->makeCurrent();

    d->programId = glCreateProgram();

    d->valid = false;
}


const QGLUniformList & QGLShaderProgram::uniforms()
{
    Q_D(QGLShaderProgram);
    return const_cast<const QGLUniformList&>(d->uniforms);
}


const QGLVertexAttributeList& QGLShaderProgram::vertexAttributes()
{
    Q_D(QGLShaderProgram);
    return const_cast<const QGLVertexAttributeList&>(d->attributeArrays);
}


bool QGLShaderProgram::addShader(QGLShader* newShader)
{
    Q_D(QGLShaderProgram);
    if (!newShader || !d->ctx)
        return false;

    if (newShader->context() != d->ctx) {
        qWarning("Shader object's context does not match program's context");
        return false;
    }

    if (!newShader->isValid())
        return false;

    QGLContext* ctx = d->ctx;
    if (!ctx)
        return false;
    ctx->makeCurrent();

    glAttachShader(d->programId, newShader->id());

    d->shaders.append(newShader);
    return true;
}


bool QGLShaderProgram::removeShader(QGLShader* oldShader)
{
    Q_D(QGLShaderProgram);

    int idx = d->shaders.indexOf(oldShader);

    if (idx == -1)
        return false;

    d->shaders.remove(idx);

    QGLContext* ctx = d->ctx;
    if (!ctx)
        return false;
    ctx->makeCurrent();

    glDetachShader(d->programId, oldShader->id());
    return true;
}


bool QGLShaderProgram::removeAllShaders()
{
    Q_D(QGLShaderProgram);

    QGLContext* ctx = d->ctx;
    if (!ctx)
        return false;
    ctx->makeCurrent();

    foreach (QGLShader* shader, d->shaders)
        glDetachShader(d->programId, shader->id());

    d->shaders.clear();
    return true;
}

#include <stdio.h>

bool QGLShaderProgram::link()
{
    Q_D(QGLShaderProgram);

    QGLContext* ctx = d->ctx;
    if (!ctx)
        return false;
    ctx->makeCurrent();

    glLinkProgram(d->programId);


    GLint linked;
    glGetProgramiv(d->programId, GL_LINK_STATUS, &linked);

    if (!linked)
        return false;

    d->populateVariableLists();

    d->valid = true;
    return true;
}

void QGLShaderProgram::use()
{
    Q_D(QGLShaderProgram);
    if (!d->valid)
        return;

    glUseProgram(d->programId);
}


QString QGLShaderProgram::log()
{
    Q_D(QGLShaderProgram);

    QGLContext* ctx = d->ctx;
    if (!ctx)
        return QString();
    ctx->makeCurrent();

    GLint  logSize = -666;
    glGetProgramiv(d->programId, GL_INFO_LOG_LENGTH, &logSize);

    char* logData = new char[logSize];
    GLint  logLength;

    glGetProgramInfoLog(d->programId, logSize, &logLength, logData);

    QString result = QString::fromAscii(logData);
    delete [] logData;

    return result;
}

GLuint QGLShaderProgram::id()
{
    Q_D(QGLShaderProgram);
    return d->programId;
}

/////////////////////////////////////////////////////////////////////////




QGLUniform::QGLUniform()
    : m_id(0), m_type(QGLInvalidType), ctx(0)
{
    qWarning("Unknown uniform! Either the uniform doesn't exist or it was removed at shader link");
}

const QGLUniform& QGLUniform::operator=(const GLfloat& rhs) const
{
    if (m_type != QGLFloatType)
        return *this;

    glUniform1f(m_id, rhs);

    return *this;
}

const QGLUniform& QGLUniform::operator=(const QGLVec2& rhs) const
{
    if (m_type != QGLVec2Type)
        return *this;

    glUniform2fv(m_id, 1, (const GLfloat*)&rhs);

    return *this;
}

const QGLUniform& QGLUniform::operator=(const QSizeF& rhs) const
{
    if (m_type != QGLVec2Type)
        return *this;

    glUniform2f(m_id, rhs.width(), rhs.height());

    return *this;
}

const QGLUniform& QGLUniform::operator=(const QPointF& rhs) const
{
    if (m_type != QGLVec2Type)
        return *this;

    glUniform2f(m_id, rhs.x(), rhs.y());

    return *this;
}

const QGLUniform& QGLUniform::operator=(const QGLVec3& rhs) const
{
    if (m_type != QGLVec3Type)
        return *this;

    glUniform3fv(m_id, 1, (const GLfloat*)&rhs);

    return *this;
}

const QGLUniform& QGLUniform::operator=(const QGLVec4& rhs) const
{
    if (m_type != QGLVec4Type)
        return *this;

    glUniform4fv(m_id, 1, (const GLfloat*)&rhs);

    return *this;
}

const QGLUniform& QGLUniform::operator=(const QColor& rhs) const
{
    if (m_type != QGLVec4Type)
        return *this;

    glUniform4f(m_id, rhs.redF(), rhs.greenF(), rhs.blueF(), rhs.alphaF());

    return *this;
}

const QGLUniform& QGLUniform::operator=(const GLfloat rhs[2][2]) const
{
    if (m_type != QGLMat2Type)
        return *this;

    glUniformMatrix2fv(m_id, 1, GL_FALSE, (GLfloat*)rhs);

    return *this;
}

const QGLUniform& QGLUniform::operator=(const GLfloat rhs[3][3]) const
{
    if (m_type != QGLMat3Type)
        return *this;

    glUniformMatrix3fv(m_id, 1, GL_FALSE, (GLfloat*)rhs);

    return *this;
}

// Transposes ready for GL
const QGLUniform& QGLUniform::operator=(const QTransform& rhs) const
{
    if (m_type != QGLMat3Type)
        return *this;

    GLfloat mat3[3][3] = {
        {rhs.m11(), rhs.m12(), rhs.m13()},
        {rhs.m21(), rhs.m22(), rhs.m23()},
        {rhs.m31(), rhs.m32(), rhs.m33()}
    };

    glUniformMatrix3fv(m_id, 1, GL_FALSE, (GLfloat*)mat3);

    return *this;
}

const QGLUniform& QGLUniform::operator=(const GLfloat rhs[4][4]) const
{
    if (m_type != QGLMat4Type)
        return *this;

    glUniformMatrix4fv(m_id, 1, GL_FALSE, (GLfloat*)rhs);

    return *this;
}

const QGLUniform& QGLUniform::operator=(const GLuint& rhs) const
{
    if ((m_type != QGLSampler2DType) || (m_type != QGLSamplerCubeType))
        return *this;

    glUniform1i(m_id, rhs);

    return *this;
}




/////////////////////////////////////////////////////////////////////////

QGLVertexAttribute::QGLVertexAttribute()
    : m_id(0), m_type(QGLInvalidType), ctx(0)
{
    qWarning("Unknown vertex attribute!");
}

void QGLVertexAttribute::enable() const
{
    glEnableVertexAttribArray(m_id);
}

void QGLVertexAttribute::disable() const
{
    glDisableVertexAttribArray(m_id);
}

// NOTE: Under PC emulation, QGLVec4Type is _always_ returned as the type, so this
// method isn't very useful. I.e. The datatypes are needed to distinguish the different
// sizes for the function signatures.
const QGLVertexAttribute& QGLVertexAttribute::operator=(const GLfloat* rhs) const
{
    int size = -1;
    if (m_type == QGLFloatType)
        size = 1;
    else if (m_type == QGLVec2Type)
        size = 2;
    else if (m_type == QGLVec3Type)
        size = 3;
    else if (m_type == QGLVec4Type)
        size = 4;
    else if (m_type == QGLMat2Type) //### Not sure if this is right for matrix attributes...
        size = 4;
    else if (m_type == QGLMat3Type) //### Not sure if this is right for matrix attributes...
        size = 9;
    else if (m_type == QGLMat4Type) //### Not sure if this is right for matrix attributes...
        size = 16;
    else
        return *this;

    glVertexAttribPointer(m_id, size, GL_FLOAT, GL_FALSE, 0, rhs);

    return *this;
}

const QGLVertexAttribute& QGLVertexAttribute::operator=(const QGLVec3* rhs) const
{
    glVertexAttribPointer(m_id, 3, GL_FLOAT, GL_FALSE, 0, (GLfloat*)rhs);

    return *this;
}
