/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
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


#ifdef QT_OPENGL_ES_CL

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of the QGLWidget class.  This header file may change from
// version to version without notice, or even be removed.
//
// We mean it.
//

QT_BEGIN_NAMESPACE

inline void glTexParameterf (GLenum target, GLenum pname, GLfloat param)
{
    glTexParameterx(target, pname, param);
}
inline void glClearColor (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
    glClearColorx(FLOAT2X(red) ,FLOAT2X(green), FLOAT2X(blue), FLOAT2X(alpha));
}
inline void glColor4f (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
    glColor4x(FLOAT2X(red) ,FLOAT2X(green), FLOAT2X(blue), FLOAT2X(alpha));
}

inline void glOrthof (GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar)
{
    glOrthox(FLOAT2X(left), FLOAT2X(right), FLOAT2X(bottom), FLOAT2X(top), FLOAT2X(zNear), FLOAT2X(zFar));
}

inline  void glPointSize (GLfloat size)
{
    glPointSizex(FLOAT2X(size));
}

inline void glPolygonOffset (GLfloat factor, GLfloat units)
{
    glPolygonOffsetx (FLOAT2X(factor), FLOAT2X(units));
}

inline void glRotatef (GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
    glRotatex(FLOAT2X(angle), FLOAT2X(x), FLOAT2X(y), FLOAT2X(z));
}

inline void glTranslatef (GLfloat x, GLfloat y, GLfloat z)
{
    glTranslatex(FLOAT2X(x) ,FLOAT2X(y) ,FLOAT2X(z));
}

inline void glNormal3f (GLfloat nx, GLfloat ny, GLfloat nz)
{
    glNormal3x(FLOAT2X(nx), FLOAT2X(ny), FLOAT2X(nz));
}

inline void glScalef(GLfloat x, GLfloat y, GLfloat z)
{
    glScalex(FLOAT2X(x), FLOAT2X(y), FLOAT2X(z));
}

inline void glClearDepthf (GLclampf depth)
{
    glClearDepthx(FLOAT2X(depth));
}

inline void glAlphaFunc (GLenum func, GLclampf ref)
{
    glAlphaFuncx(func, FLOAT2X(ref));
}

inline void glLoadMatrixf (const GLfloat *_m)
{
    GLfixed m[16];
    for (int i =0; i < 16; i++)
        m[i] = FLOAT2X(_m[i]);
    glLoadMatrixx(m);
}

inline void glMultMatrixf (const GLfloat *_m)
{
    GLfixed m[16];
    for (int i =0; i < 16; i++)
        m[i] = FLOAT2X(_m[i]);
    glMultMatrixx (m);
}


inline void glLineWidth (GLfloat width)
{
    glLineWidthx(FLOAT2X(width));
}

QT_END_NAMESPACE

#endif  //QT_OPENGL_ES_CL

