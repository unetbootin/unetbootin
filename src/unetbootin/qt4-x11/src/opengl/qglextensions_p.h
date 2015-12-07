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

#ifndef QGL_EXTENSIONS_P_H
#define QGL_EXTENSIONS_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of the Qt OpenGL classes.  This header file may change from
// version to version without notice, or even be removed.
//
// We mean it.
//

// extension prototypes
#ifndef Q_WS_MAC
# ifndef APIENTRYP
#   ifdef APIENTRY
#     define APIENTRYP APIENTRY *
#   else
#     define APIENTRY
#     define APIENTRYP *
#   endif
# endif
#else
# define APIENTRY
# define APIENTRYP *
#endif

#include <QtCore/qglobal.h>

#ifndef GL_ARB_vertex_buffer_object
typedef ptrdiff_t GLsizeiptrARB;
#endif

// ARB_pixel_buffer_object
typedef void (APIENTRY *_glBindBufferARB) (GLenum, GLuint);
typedef void (APIENTRY *_glDeleteBuffersARB) (GLsizei, const GLuint *);
typedef void (APIENTRY *_glGenBuffersARB) (GLsizei, GLuint *);
typedef void (APIENTRY *_glBufferDataARB) (GLenum, GLsizeiptrARB, const GLvoid *, GLenum);
typedef GLvoid* (APIENTRY *_glMapBufferARB) (GLenum, GLenum);
typedef GLboolean (APIENTRY *_glUnmapBufferARB) (GLenum);

// ARB_fragment_program
typedef void (APIENTRY *_glProgramStringARB) (GLenum, GLenum, GLsizei, const GLvoid *);
typedef void (APIENTRY *_glBindProgramARB) (GLenum, GLuint);
typedef void (APIENTRY *_glDeleteProgramsARB) (GLsizei, const GLuint *);
typedef void (APIENTRY *_glGenProgramsARB) (GLsizei, GLuint *);
typedef void (APIENTRY *_glProgramLocalParameter4fvARB) (GLenum, GLuint, const GLfloat *);

// GLSL
typedef GLuint (APIENTRY *_glCreateShader) (GLenum);
typedef void (APIENTRY *_glShaderSource) (GLuint, GLsizei, const char **, const GLint *);
typedef void (APIENTRY *_glCompileShader) (GLuint);
typedef void (APIENTRY *_glDeleteShader) (GLuint);

typedef GLuint (APIENTRY *_glCreateProgram) ();
typedef void (APIENTRY *_glAttachShader) (GLuint, GLuint);
typedef void (APIENTRY *_glDetachShader) (GLuint, GLuint);
typedef void (APIENTRY *_glLinkProgram) (GLuint);
typedef void (APIENTRY *_glUseProgram) (GLuint);
typedef void (APIENTRY *_glDeleteProgram) (GLuint);

typedef void (APIENTRY *_glGetShaderInfoLog) (GLuint, GLsizei, GLsizei *, char *);
typedef void (APIENTRY *_glGetShaderiv) (GLuint, GLenum, GLint *);
typedef void (APIENTRY *_glGetProgramiv) (GLuint, GLenum, GLint *);

typedef GLuint (APIENTRY *_glGetUniformLocation) (GLuint, const char*);
typedef void (APIENTRY *_glUniform4fv) (GLint, GLsizei, GLfloat *);
typedef void (APIENTRY *_glUniform3fv) (GLint, GLsizei, GLfloat *);
typedef void (APIENTRY *_glUniform2fv) (GLint, GLsizei, GLfloat *);
typedef void (APIENTRY *_glUniform1fv) (GLint, GLsizei, GLfloat *);
typedef void (APIENTRY *_glUniform1i) (GLint, GLint);

typedef void (APIENTRY *_glActiveStencilFaceEXT) (GLenum );

typedef void (APIENTRY *_glMultiTexCoord4f) (GLenum, GLfloat, GLfloat, GLfloat, GLfloat);
typedef void (APIENTRY *_glActiveTexture) (GLenum);

// EXT_GL_framebuffer_object
typedef GLboolean (APIENTRY *_glIsRenderbufferEXT) (GLuint renderbuffer);
typedef void (APIENTRY *_glBindRenderbufferEXT) (GLenum target, GLuint renderbuffer);
typedef void (APIENTRY *_glDeleteRenderbuffersEXT) (GLsizei n, const GLuint *renderbuffers);
typedef void (APIENTRY *_glGenRenderbuffersEXT) (GLsizei n, GLuint *renderbuffers);
typedef void (APIENTRY *_glRenderbufferStorageEXT) (GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
typedef void (APIENTRY *_glGetRenderbufferParameterivEXT) (GLenum target, GLenum pname, GLint *params);
typedef GLboolean (APIENTRY *_glIsFramebufferEXT) (GLuint framebuffer);
typedef void (APIENTRY *_glBindFramebufferEXT) (GLenum target, GLuint framebuffer);
typedef void (APIENTRY *_glDeleteFramebuffersEXT) (GLsizei n, const GLuint *framebuffers);
typedef void (APIENTRY *_glGenFramebuffersEXT) (GLsizei n, GLuint *framebuffers);
typedef GLenum (APIENTRY *_glCheckFramebufferStatusEXT) (GLenum target);
typedef void (APIENTRY *_glFramebufferTexture1DEXT) (GLenum target, GLenum attachment, GLenum textarget,
                                                           GLuint texture, GLint level);
typedef void (APIENTRY *_glFramebufferTexture2DEXT) (GLenum target, GLenum attachment, GLenum textarget,
                                                           GLuint texture, GLint level);
typedef void (APIENTRY *_glFramebufferTexture3DEXT) (GLenum target, GLenum attachment, GLenum textarget,
                                                           GLuint texture, GLint level, GLint zoffset);
typedef void (APIENTRY *_glFramebufferRenderbufferEXT) (GLenum target, GLenum attachment, GLenum renderbuffertarget,
                                                              GLuint renderbuffer);
typedef void (APIENTRY *_glGetFramebufferAttachmentParameterivEXT) (GLenum target, GLenum attachment, GLenum pname,
                                                                          GLint *params);
typedef void (APIENTRY *_glGenerateMipmapEXT) (GLenum target);

QT_BEGIN_NAMESPACE

struct QGLExtensionFuncs
{
    QGLExtensionFuncs() {
        qt_glProgramStringARB = 0;
        qt_glBindProgramARB = 0;
        qt_glDeleteProgramsARB = 0;
        qt_glGenProgramsARB = 0;
        qt_glProgramLocalParameter4fvARB = 0;

        qt_glCreateShader = 0;
        qt_glShaderSource = 0;
        qt_glCompileShader = 0;
        qt_glDeleteShader = 0;

        qt_glCreateProgram = 0;
        qt_glAttachShader = 0;
        qt_glDetachShader = 0;
        qt_glLinkProgram = 0;
        qt_glUseProgram = 0;
        qt_glDeleteProgram = 0;

        qt_glGetShaderInfoLog = 0;
        qt_glGetShaderiv = 0;
        qt_glGetProgramiv = 0;

        qt_glGetUniformLocation = 0;
        qt_glUniform4fv = 0;
        qt_glUniform3fv = 0;
        qt_glUniform2fv = 0;
        qt_glUniform1fv = 0;
        qt_glUniform1i = 0;

        qt_glActiveStencilFaceEXT = 0;

        qt_glMultiTexCoord4f = 0;
        qt_glActiveTexture = 0;

        qt_glIsRenderbufferEXT = 0;
        qt_glBindRenderbufferEXT = 0;
        qt_glDeleteRenderbuffersEXT = 0;
        qt_glGenRenderbuffersEXT = 0;
        qt_glRenderbufferStorageEXT = 0;
        qt_glGetRenderbufferParameterivEXT = 0;
        qt_glIsFramebufferEXT = 0;
        qt_glBindFramebufferEXT = 0;
        qt_glDeleteFramebuffersEXT = 0;
        qt_glGenFramebuffersEXT = 0;
        qt_glCheckFramebufferStatusEXT = 0;
        qt_glFramebufferTexture1DEXT = 0;
        qt_glFramebufferTexture2DEXT = 0;
        qt_glFramebufferTexture3DEXT = 0;
        qt_glFramebufferRenderbufferEXT = 0;
        qt_glGetFramebufferAttachmentParameterivEXT = 0;
        qt_glGenerateMipmapEXT = 0;

        qt_glBindBufferARB = 0;
        qt_glDeleteBuffersARB = 0;
        qt_glGenBuffersARB = 0;
        qt_glBufferDataARB = 0;
        qt_glMapBufferARB = 0;
        qt_glUnmapBufferARB = 0;
    }

    _glProgramStringARB qt_glProgramStringARB;
    _glBindProgramARB qt_glBindProgramARB;
    _glDeleteProgramsARB qt_glDeleteProgramsARB;
    _glGenProgramsARB qt_glGenProgramsARB;
    _glProgramLocalParameter4fvARB qt_glProgramLocalParameter4fvARB;

    // GLSL definitions
    _glCreateShader qt_glCreateShader;
    _glShaderSource qt_glShaderSource;
    _glCompileShader qt_glCompileShader;
    _glDeleteShader qt_glDeleteShader;

    _glCreateProgram qt_glCreateProgram;
    _glAttachShader qt_glAttachShader;
    _glDetachShader qt_glDetachShader;
    _glLinkProgram qt_glLinkProgram;
    _glUseProgram qt_glUseProgram;
    _glDeleteProgram qt_glDeleteProgram;

    _glGetShaderInfoLog qt_glGetShaderInfoLog;
    _glGetShaderiv qt_glGetShaderiv;
    _glGetProgramiv qt_glGetProgramiv;

    _glGetUniformLocation qt_glGetUniformLocation;
    _glUniform4fv qt_glUniform4fv;
    _glUniform3fv qt_glUniform3fv;
    _glUniform2fv qt_glUniform2fv;
    _glUniform1fv qt_glUniform1fv;
    _glUniform1i qt_glUniform1i;

    _glActiveStencilFaceEXT qt_glActiveStencilFaceEXT;

    _glMultiTexCoord4f qt_glMultiTexCoord4f;
    _glActiveTexture qt_glActiveTexture;

    _glIsRenderbufferEXT qt_glIsRenderbufferEXT;
    _glBindRenderbufferEXT qt_glBindRenderbufferEXT;
    _glDeleteRenderbuffersEXT qt_glDeleteRenderbuffersEXT;
    _glGenRenderbuffersEXT qt_glGenRenderbuffersEXT;
    _glRenderbufferStorageEXT qt_glRenderbufferStorageEXT;
    _glGetRenderbufferParameterivEXT qt_glGetRenderbufferParameterivEXT;
    _glIsFramebufferEXT qt_glIsFramebufferEXT;
    _glBindFramebufferEXT qt_glBindFramebufferEXT;
    _glDeleteFramebuffersEXT qt_glDeleteFramebuffersEXT;
    _glGenFramebuffersEXT qt_glGenFramebuffersEXT;
    _glCheckFramebufferStatusEXT qt_glCheckFramebufferStatusEXT;
    _glFramebufferTexture1DEXT qt_glFramebufferTexture1DEXT;
    _glFramebufferTexture2DEXT qt_glFramebufferTexture2DEXT;
    _glFramebufferTexture3DEXT qt_glFramebufferTexture3DEXT;
    _glFramebufferRenderbufferEXT qt_glFramebufferRenderbufferEXT;
    _glGetFramebufferAttachmentParameterivEXT qt_glGetFramebufferAttachmentParameterivEXT;
    _glGenerateMipmapEXT qt_glGenerateMipmapEXT;

    _glBindBufferARB qt_glBindBufferARB;
    _glDeleteBuffersARB qt_glDeleteBuffersARB;
    _glGenBuffersARB qt_glGenBuffersARB;
    _glBufferDataARB qt_glBufferDataARB;
    _glMapBufferARB qt_glMapBufferARB;
    _glUnmapBufferARB qt_glUnmapBufferARB;
};


// OpenGL constants

/* NV_texture_rectangle */
#ifndef GL_NV_texture_rectangle
#define GL_TEXTURE_RECTANGLE_NV           0x84F5
#define GL_TEXTURE_BINDING_RECTANGLE_NV   0x84F6
#define GL_PROXY_TEXTURE_RECTANGLE_NV     0x84F7
#define GL_MAX_RECTANGLE_TEXTURE_SIZE_NV  0x84F8
#endif

#ifndef GL_BGRA
#define GL_BGRA 0x80E1
#endif

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif

#ifndef GL_IBM_texture_mirrored_repeat
#define GL_MIRRORED_REPEAT_IBM            0x8370
#endif

#ifndef GL_SGIS_generate_mipmap
#define GL_GENERATE_MIPMAP_SGIS           0x8191
#define GL_GENERATE_MIPMAP_HINT_SGIS      0x8192
#endif

// ARB_fragment_program extension protos
#ifndef GL_FRAGMENT_PROGRAM_ARB
#define GL_FRAGMENT_PROGRAM_ARB           0x8804
#define GL_PROGRAM_FORMAT_ASCII_ARB       0x8875
#endif

#ifndef GL_PIXEL_UNPACK_BUFFER_ARB
#define GL_PIXEL_UNPACK_BUFFER_ARB 0x88EC
#endif

#ifndef GL_WRITE_ONLY_ARB
#define GL_WRITE_ONLY_ARB 0x88B9
#endif

#ifndef GL_STREAM_DRAW_ARB
#define GL_STREAM_DRAW_ARB 0x88E0
#endif

// Stencil wrap and two-side defines
#ifndef GL_STENCIL_TEST_TWO_SIDE_EXT
#define GL_STENCIL_TEST_TWO_SIDE_EXT 0x8910
#endif
#ifndef GL_INCR_WRAP_EXT
#define GL_INCR_WRAP_EXT 0x8507
#endif
#ifndef GL_DECR_WRAP_EXT
#define GL_DECR_WRAP_EXT 0x8508
#endif

#ifndef GL_TEXTURE0
#define GL_TEXTURE0 0x84C0
#endif

#ifndef GL_TEXTURE1
#define GL_TEXTURE1 0x84C1
#endif

#ifndef GL_EXT_framebuffer_object
#define GL_INVALID_FRAMEBUFFER_OPERATION_EXT                    0x0506
#define GL_MAX_RENDERBUFFER_SIZE_EXT                            0x84E8
#define GL_FRAMEBUFFER_BINDING_EXT                              0x8CA6
#define GL_RENDERBUFFER_BINDING_EXT                             0x8CA7
#define GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE_EXT               0x8CD0
#define GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME_EXT               0x8CD1
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL_EXT             0x8CD2
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE_EXT     0x8CD3
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_3D_ZOFFSET_EXT        0x8CD4
#define GL_FRAMEBUFFER_COMPLETE_EXT                             0x8CD5
#define GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT                0x8CD6
#define GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT        0x8CD7
#define GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT      0x8CD8
#define GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT                0x8CD9
#define GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT                   0x8CDA
#define GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT               0x8CDB
#define GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT               0x8CDC
#define GL_FRAMEBUFFER_UNSUPPORTED_EXT                          0x8CDD
#define GL_MAX_COLOR_ATTACHMENTS_EXT                            0x8CDF
#define GL_COLOR_ATTACHMENT0_EXT                                0x8CE0
#define GL_COLOR_ATTACHMENT1_EXT                                0x8CE1
#define GL_COLOR_ATTACHMENT2_EXT                                0x8CE2
#define GL_COLOR_ATTACHMENT3_EXT                                0x8CE3
#define GL_COLOR_ATTACHMENT4_EXT                                0x8CE4
#define GL_COLOR_ATTACHMENT5_EXT                                0x8CE5
#define GL_COLOR_ATTACHMENT6_EXT                                0x8CE6
#define GL_COLOR_ATTACHMENT7_EXT                                0x8CE7
#define GL_COLOR_ATTACHMENT8_EXT                                0x8CE8
#define GL_COLOR_ATTACHMENT9_EXT                                0x8CE9
#define GL_COLOR_ATTACHMENT10_EXT                               0x8CEA
#define GL_COLOR_ATTACHMENT11_EXT                               0x8CEB
#define GL_COLOR_ATTACHMENT12_EXT                               0x8CEC
#define GL_COLOR_ATTACHMENT13_EXT                               0x8CED
#define GL_COLOR_ATTACHMENT14_EXT                               0x8CEE
#define GL_COLOR_ATTACHMENT15_EXT                               0x8CEF
#define GL_DEPTH_ATTACHMENT_EXT                                 0x8D00
#define GL_STENCIL_ATTACHMENT_EXT                               0x8D20
#define GL_FRAMEBUFFER_EXT                                      0x8D40
#define GL_RENDERBUFFER_EXT                                     0x8D41
#define GL_RENDERBUFFER_WIDTH_EXT                               0x8D42
#define GL_RENDERBUFFER_HEIGHT_EXT                              0x8D43
#define GL_RENDERBUFFER_INTERNAL_FORMAT_EXT                     0x8D44
#define GL_STENCIL_INDEX_EXT                                    0x8D45
#define GL_STENCIL_INDEX1_EXT                                   0x8D46
#define GL_STENCIL_INDEX4_EXT                                   0x8D47
#define GL_STENCIL_INDEX8_EXT                                   0x8D48
#define GL_STENCIL_INDEX16_EXT                                  0x8D49
#define GL_RENDERBUFFER_RED_SIZE_EXT                            0x8D50
#define GL_RENDERBUFFER_GREEN_SIZE_EXT                          0x8D51
#define GL_RENDERBUFFER_BLUE_SIZE_EXT                           0x8D52
#define GL_RENDERBUFFER_ALPHA_SIZE_EXT                          0x8D53
#define GL_RENDERBUFFER_DEPTH_SIZE_EXT                          0x8D54
#define GL_RENDERBUFFER_STENCIL_SIZE_EXT                        0x8D55
#endif

#ifndef GL_EXT_packed_depth_stencil
#define GL_DEPTH_STENCIL_EXT                                    0x84F9
#define GL_UNSIGNED_INT_24_8_EXT                                0x84FA
#define GL_DEPTH24_STENCIL8_EXT                                 0x88F0
#define GL_TEXTURE_STENCIL_SIZE_EXT                             0x88F1
#endif

// ### hm. should be part of the GL 1.2 spec..
#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE                  0x812F
#endif

#ifndef GL_VERSION_1_2
#define GL_PACK_SKIP_IMAGES               0x806B
#define GL_PACK_IMAGE_HEIGHT              0x806C
#define GL_UNPACK_SKIP_IMAGES             0x806D
#define GL_UNPACK_IMAGE_HEIGHT            0x806E
#endif

#define glProgramStringARB QGLContextPrivate::qt_get_extension_funcs(ctx).qt_glProgramStringARB
#define glBindProgramARB QGLContextPrivate::qt_get_extension_funcs(ctx).qt_glBindProgramARB
#define glDeleteProgramsARB QGLContextPrivate::qt_get_extension_funcs(ctx).qt_glDeleteProgramsARB
#define glGenProgramsARB QGLContextPrivate::qt_get_extension_funcs(ctx).qt_glGenProgramsARB
#define glProgramLocalParameter4fvARB QGLContextPrivate::qt_get_extension_funcs(ctx).qt_glProgramLocalParameter4fvARB

#define glActiveStencilFaceEXT QGLContextPrivate::qt_get_extension_funcs(ctx).qt_glActiveStencilFaceEXT

#define glMultiTexCoord4f QGLContextPrivate::qt_get_extension_funcs(ctx).qt_glMultiTexCoord4f

#if !defined(QT_OPENGL_ES_2)
#define glActiveTexture QGLContextPrivate::qt_get_extension_funcs(ctx).qt_glActiveTexture
#endif

#define glIsRenderbufferEXT QGLContextPrivate::qt_get_extension_funcs(ctx).qt_glIsRenderbufferEXT
#define glBindRenderbufferEXT QGLContextPrivate::qt_get_extension_funcs(ctx).qt_glBindRenderbufferEXT
#define glDeleteRenderbuffersEXT QGLContextPrivate::qt_get_extension_funcs(ctx).qt_glDeleteRenderbuffersEXT
#define glGenRenderbuffersEXT QGLContextPrivate::qt_get_extension_funcs(ctx).qt_glGenRenderbuffersEXT
#define glRenderbufferStorageEXT QGLContextPrivate::qt_get_extension_funcs(ctx).qt_glRenderbufferStorageEXT
#define glGetRenderbufferParameterivEXT QGLContextPrivate::qt_get_extension_funcs(ctx).qt_glGetRenderbufferParameterivEXT
#define glIsFramebufferEXT QGLContextPrivate::qt_get_extension_funcs(ctx).qt_glIsFramebufferEXT
#define glBindFramebufferEXT QGLContextPrivate::qt_get_extension_funcs(ctx).qt_glBindFramebufferEXT
#define glDeleteFramebuffersEXT QGLContextPrivate::qt_get_extension_funcs(ctx).qt_glDeleteFramebuffersEXT
#define glGenFramebuffersEXT QGLContextPrivate::qt_get_extension_funcs(ctx).qt_glGenFramebuffersEXT
#define glCheckFramebufferStatusEXT QGLContextPrivate::qt_get_extension_funcs(ctx).qt_glCheckFramebufferStatusEXT
#define glFramebufferTexture1DEXT QGLContextPrivate::qt_get_extension_funcs(ctx).qt_glFramebufferTexture1DEXT
#define glFramebufferTexture2DEXT QGLContextPrivate::qt_get_extension_funcs(ctx).qt_glFramebufferTexture2DEXT
#define glFramebufferTexture3DEXT QGLContextPrivate::qt_get_extension_funcs(ctx).qt_glFramebufferTexture3DEXT
#define glFramebufferRenderbufferEXT QGLContextPrivate::qt_get_extension_funcs(ctx).qt_glFramebufferRenderbufferEXT
#define glGetFramebufferAttachmentParameterivEXT QGLContextPrivate::qt_get_extension_funcs(ctx).qt_glGetFramebufferAttachmentParameterivEXT
#define glGenerateMipmapEXT QGLContextPrivate::qt_get_extension_funcs(ctx).qt_glGenerateMipmapEXT

#define glBindBufferARB QGLContextPrivate::qt_get_extension_funcs(ctx).qt_glBindBufferARB
#define glDeleteBuffersARB QGLContextPrivate::qt_get_extension_funcs(ctx).qt_glDeleteBuffersARB
#define glGenBuffersARB QGLContextPrivate::qt_get_extension_funcs(ctx).qt_glGenBuffersARB
#define glBufferDataARB QGLContextPrivate::qt_get_extension_funcs(ctx).qt_glBufferDataARB
#define glMapBufferARB QGLContextPrivate::qt_get_extension_funcs(ctx).qt_glMapBufferARB
#define glUnmapBufferARB QGLContextPrivate::qt_get_extension_funcs(ctx).qt_glUnmapBufferARB

#define glCreateShader QGLContextPrivate::qt_get_extension_funcs(ctx).qt_glCreateShader
#define glShaderSource QGLContextPrivate::qt_get_extension_funcs(ctx).qt_glShaderSource
#define glCompileShader QGLContextPrivate::qt_get_extension_funcs(ctx).qt_glCompileShader
#define glDeleteShader QGLContextPrivate::qt_get_extension_funcs(ctx).qt_glDeleteShader

#define glCreateProgram QGLContextPrivate::qt_get_extension_funcs(ctx).qt_glCreateProgram
#define glAttachShader QGLContextPrivate::qt_get_extension_funcs(ctx).qt_glAttachShader
#define glDetachShader QGLContextPrivate::qt_get_extension_funcs(ctx).qt_glDetachShader
#define glLinkProgram QGLContextPrivate::qt_get_extension_funcs(ctx).qt_glLinkProgram
#define glUseProgram QGLContextPrivate::qt_get_extension_funcs(ctx).qt_glUseProgram
#define glDeleteProgram QGLContextPrivate::qt_get_extension_funcs(ctx).qt_glDeleteProgram

#define glGetShaderInfoLog QGLContextPrivate::qt_get_extension_funcs(ctx).qt_glGetShaderInfoLog
#define glGetShaderiv QGLContextPrivate::qt_get_extension_funcs(ctx).qt_glGetShaderiv
#define glGetProgramiv QGLContextPrivate::qt_get_extension_funcs(ctx).qt_glGetProgramiv

#define glGetUniformLocation QGLContextPrivate::qt_get_extension_funcs(ctx).qt_glGetUniformLocation
#define glUniform4fv QGLContextPrivate::qt_get_extension_funcs(ctx).qt_glUniform4fv
#define glUniform3fv QGLContextPrivate::qt_get_extension_funcs(ctx).qt_glUniform3fv
#define glUniform2fv QGLContextPrivate::qt_get_extension_funcs(ctx).qt_glUniform2fv
#define glUniform1fv QGLContextPrivate::qt_get_extension_funcs(ctx).qt_glUniform1fv
#define glUniform1i QGLContextPrivate::qt_get_extension_funcs(ctx).qt_glUniform1i

extern bool qt_resolve_framebufferobject_extensions(QGLContext *ctx);
bool qt_resolve_buffer_extensions(QGLContext *ctx);

bool qt_resolve_version_1_3_functions(QGLContext *ctx);
bool qt_resolve_stencil_face_extension(QGLContext *ctx);
bool qt_resolve_frag_program_extensions(QGLContext *ctx);

bool qt_resolve_glsl_extensions(QGLContext *ctx);

QT_END_NAMESPACE

#endif // QGL_EXTENSIONS_P_H
