/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
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

//! [0]
#define FLOAT2X(f)      ((int) ( (f) * (65536)))
#define X2FLOAT(x)      ((float)(x) / 65536.0f)

#define f2vt(f)     FLOAT2X(f)
#define vt2f(x)     X2FLOAT(x)

#define q_vertexType GLfixed
#define q_vertexTypeEnum GL_FIXED

#define q_glFog             glFogx
#define q_glFogv            glFogxv
//! [0]

#define q_glLight           glLightx
#define q_glLightv          glLightxv
#define q_glLightModel      glLightModelx
#define q_glLightModelv     glLightModelxv

#define q_glAlphaFunc       glAlphaFuncx

#define q_glMaterial        glMaterialx
#define q_glMaterialv       glMaterialxv
#define q_glColor4          glColor4x

#define q_glTexParameter    glTexParameterx
#define q_glTexEnv          glTexEnvx

#define q_glOrtho           glOrthox
#define q_glFrustum         glFrustumx

#define q_glTranslate       glTranslatex
#define q_glScale           glScalex
#define q_glRotate          glRotatex
#define q_glLoadMatrix      glLoadMatrixx

#define q_glClearColor      glClearColorx

#define q_glMultMatrix      glMultMatrixx

#define q_glNormal3         glNormal3x

#define q_glPolygonOffset   glPolygonOffsetx
#define q_glPointSize       glPointSizex

//! [1]
#else

#define f2vt(f)     (f)
#define vt2f(x)     (x)

#define q_vertexType GLfloat
#define q_vertexTypeEnum GL_FLOAT

#define q_glFog             glFogf
#define q_glFogv            glFogfv
//! [1]

#define q_glLight           glLightf
#define q_glLightv          glLightfv
#define q_glLightModel      glLightModelf
#define q_glLightModelv     glLightModelfv

#define q_glAlphaFunc       glAlphaFuncf

#define q_glMaterial        glMaterialf
#define q_glMaterialv       glMaterialfv
#define q_glColor4          glColor4f

#define q_glTexParameter    glTexParameterf
#define q_glTexEnv          glTexEnvf

#define q_glOrtho           glOrthof
#define q_glFrustum         glFrustumf

#define q_glTranslate       glTranslatef
#define q_glScale           glScalef
#define q_glRotate          glRotatef
#define q_glLoadMatrix      glLoadMatrixf

#define q_glClearColor      glClearColor

#define q_glMultMatrix      glMultMatrixf

#define q_glNormal3         glNormal3f

#define q_glPolygonOffset   glPolygonOffsetf
#define q_glPointSize       glPointSizef

#endif
