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

#include "qgl_p.h"

QT_BEGIN_NAMESPACE

bool qt_resolve_framebufferobject_extensions(QGLContext *ctx)
{
    if (glIsRenderbufferEXT != 0)
        return true;

    if (ctx == 0) {
        qWarning("QGLFramebufferObject: Unable to resolve framebuffer object extensions -"
                 " make sure there is a current context when creating the framebuffer object.");
        return false;
    }

    glIsRenderbufferEXT = (_glIsRenderbufferEXT) ctx->getProcAddress(QLatin1String("glIsRenderbufferEXT"));
    glBindRenderbufferEXT = (_glBindRenderbufferEXT) ctx->getProcAddress(QLatin1String("glBindRenderbufferEXT"));
    glDeleteRenderbuffersEXT = (_glDeleteRenderbuffersEXT) ctx->getProcAddress(QLatin1String("glDeleteRenderbuffersEXT"));
    glGenRenderbuffersEXT = (_glGenRenderbuffersEXT) ctx->getProcAddress(QLatin1String("glGenRenderbuffersEXT"));
    glRenderbufferStorageEXT = (_glRenderbufferStorageEXT) ctx->getProcAddress(QLatin1String("glRenderbufferStorageEXT"));
    glGetRenderbufferParameterivEXT =
        (_glGetRenderbufferParameterivEXT) ctx->getProcAddress(QLatin1String("glGetRenderbufferParameterivEXT"));
    glIsFramebufferEXT = (_glIsFramebufferEXT) ctx->getProcAddress(QLatin1String("glIsFramebufferEXT"));
    glBindFramebufferEXT = (_glBindFramebufferEXT) ctx->getProcAddress(QLatin1String("glBindFramebufferEXT"));
    glDeleteFramebuffersEXT = (_glDeleteFramebuffersEXT) ctx->getProcAddress(QLatin1String("glDeleteFramebuffersEXT"));
    glGenFramebuffersEXT = (_glGenFramebuffersEXT) ctx->getProcAddress(QLatin1String("glGenFramebuffersEXT"));
    glCheckFramebufferStatusEXT = (_glCheckFramebufferStatusEXT) ctx->getProcAddress(QLatin1String("glCheckFramebufferStatusEXT"));
    glFramebufferTexture1DEXT = (_glFramebufferTexture1DEXT) ctx->getProcAddress(QLatin1String("glFramebufferTexture1DEXT"));
    glFramebufferTexture2DEXT = (_glFramebufferTexture2DEXT) ctx->getProcAddress(QLatin1String("glFramebufferTexture2DEXT"));
    glFramebufferTexture3DEXT = (_glFramebufferTexture3DEXT) ctx->getProcAddress(QLatin1String("glFramebufferTexture3DEXT"));
    glFramebufferRenderbufferEXT = (_glFramebufferRenderbufferEXT) ctx->getProcAddress(QLatin1String("glFramebufferRenderbufferEXT"));
    glGetFramebufferAttachmentParameterivEXT =
        (_glGetFramebufferAttachmentParameterivEXT) ctx->getProcAddress(QLatin1String("glGetFramebufferAttachmentParameterivEXT"));
    glGenerateMipmapEXT = (_glGenerateMipmapEXT) ctx->getProcAddress(QLatin1String("glGenerateMipmapEXT"));
    return glIsRenderbufferEXT;
}

bool qt_resolve_version_1_3_functions(QGLContext *ctx)
{
    if (glMultiTexCoord4f != 0)
        return true;

    QGLContext cx(QGLFormat::defaultFormat());
    glMultiTexCoord4f = (_glMultiTexCoord4f) ctx->getProcAddress(QLatin1String("glMultiTexCoord4f"));

#if defined(QT_OPENGL_ES_2)
    return glMultiTexCoord4f;
#else
    glActiveTexture = (_glActiveTexture) ctx->getProcAddress(QLatin1String("glActiveTexture"));
    return glMultiTexCoord4f && glActiveTexture;
#endif
}

bool qt_resolve_stencil_face_extension(QGLContext *ctx)
{
    if (glActiveStencilFaceEXT != 0)
        return true;

    QGLContext cx(QGLFormat::defaultFormat());
    glActiveStencilFaceEXT = (_glActiveStencilFaceEXT) ctx->getProcAddress(QLatin1String("glActiveStencilFaceEXT"));

    return glActiveStencilFaceEXT;
}

bool qt_resolve_frag_program_extensions(QGLContext *ctx)
{
    if (glProgramStringARB != 0)
        return true;

    // ARB_fragment_program
    glProgramStringARB = (_glProgramStringARB) ctx->getProcAddress(QLatin1String("glProgramStringARB"));
    glBindProgramARB = (_glBindProgramARB) ctx->getProcAddress(QLatin1String("glBindProgramARB"));
    glDeleteProgramsARB = (_glDeleteProgramsARB) ctx->getProcAddress(QLatin1String("glDeleteProgramsARB"));
    glGenProgramsARB = (_glGenProgramsARB) ctx->getProcAddress(QLatin1String("glGenProgramsARB"));
    glProgramLocalParameter4fvARB = (_glProgramLocalParameter4fvARB) ctx->getProcAddress(QLatin1String("glProgramLocalParameter4fvARB"));

    return glProgramStringARB
        && glBindProgramARB
        && glDeleteProgramsARB
        && glGenProgramsARB
        && glProgramLocalParameter4fvARB;
}

bool qt_resolve_buffer_extensions(QGLContext *ctx)
{
    if (glBindBufferARB && glDeleteBuffersARB && glGenBuffersARB && glBufferDataARB
        && glMapBufferARB && glUnmapBufferARB)
        return true;

    glBindBufferARB = (_glBindBufferARB) ctx->getProcAddress(QLatin1String("glBindBufferARB"));
    glDeleteBuffersARB = (_glDeleteBuffersARB) ctx->getProcAddress(QLatin1String("glDeleteBuffersARB"));
    glGenBuffersARB = (_glGenBuffersARB) ctx->getProcAddress(QLatin1String("glGenBuffersARB"));
    glBufferDataARB = (_glBufferDataARB) ctx->getProcAddress(QLatin1String("glBufferDataARB"));
    glMapBufferARB = (_glMapBufferARB) ctx->getProcAddress(QLatin1String("glMapBufferARB"));
    glUnmapBufferARB = (_glUnmapBufferARB) ctx->getProcAddress(QLatin1String("glUnmapBufferARB"));

    return glBindBufferARB
        && glDeleteBuffersARB
        && glGenBuffersARB
        && glBufferDataARB
        && glMapBufferARB
        && glUnmapBufferARB;
}

bool qt_resolve_glsl_extensions(QGLContext *ctx)
{
    if (glCreateShader)
        return true;

    glCreateShader = (_glCreateShader) ctx->getProcAddress(QLatin1String("glCreateShader"));
    glShaderSource = (_glShaderSource) ctx->getProcAddress(QLatin1String("glShaderSource"));
    glCompileShader = (_glCompileShader) ctx->getProcAddress(QLatin1String("glCompileShader"));
    glDeleteShader = (_glDeleteShader) ctx->getProcAddress(QLatin1String("glDeleteShader"));

    glCreateProgram = (_glCreateProgram) ctx->getProcAddress(QLatin1String("glCreateProgram"));
    glAttachShader = (_glAttachShader) ctx->getProcAddress(QLatin1String("glAttachShader"));
    glDetachShader = (_glDetachShader) ctx->getProcAddress(QLatin1String("glDetachShader"));
    glLinkProgram = (_glLinkProgram) ctx->getProcAddress(QLatin1String("glLinkProgram"));
    glUseProgram = (_glUseProgram) ctx->getProcAddress(QLatin1String("glUseProgram"));
    glDeleteProgram = (_glDeleteProgram) ctx->getProcAddress(QLatin1String("glDeleteProgram"));

    glGetShaderInfoLog = (_glGetShaderInfoLog) ctx->getProcAddress(QLatin1String("glGetShaderInfoLog"));
    glGetShaderiv = (_glGetShaderiv) ctx->getProcAddress(QLatin1String("glGetShaderiv"));
    glGetProgramiv = (_glGetProgramiv) ctx->getProcAddress(QLatin1String("glGetProgramiv"));

    glGetUniformLocation = (_glGetUniformLocation) ctx->getProcAddress(QLatin1String("glGetUniformLocation"));
    glUniform4fv = (_glUniform4fv) ctx->getProcAddress(QLatin1String("glUniform4fv"));
    glUniform3fv = (_glUniform3fv) ctx->getProcAddress(QLatin1String("glUniform3fv"));
    glUniform2fv = (_glUniform2fv) ctx->getProcAddress(QLatin1String("glUniform2fv"));
    glUniform1fv = (_glUniform1fv) ctx->getProcAddress(QLatin1String("glUniform1fv"));
    glUniform1i = (_glUniform1i) ctx->getProcAddress(QLatin1String("glUniform1i"));

    return glCreateShader && glShaderSource && glCompileShader && glDeleteProgram &&
        glCreateProgram && glAttachShader && glDetachShader && glLinkProgram && glUseProgram &&
        glDeleteProgram && glGetShaderInfoLog && glGetShaderiv && glGetProgramiv && glGetUniformLocation &&
        glUniform1i && glUniform1fv && glUniform2fv && glUniform3fv && glUniform4fv;
}

QT_END_NAMESPACE
