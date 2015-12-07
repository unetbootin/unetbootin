/****************************************************************************
**
** Copyright (C) 2005-2008 Trolltech ASA. All rights reserved.
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

#include "glwidget.h"
#include <math.h>

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
    startTimer(40);
    setWindowTitle(tr("Sample Buffers"));
}

void GLWidget::initializeGL()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-.5, .5, .5, -.5, -1000, 1000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    makeObject();
}

void GLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void GLWidget::paintGL()
{
    static float rot = 0.0;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glEnable(GL_MULTISAMPLE);
    glTranslatef(-0.25f, -0.10f, 0.0f);
    glScalef(0.75f, 1.15f, 0.0f);
    glRotatef(rot, 0.0f, 0.f, 1.0f);
    glCallList(list);
    glPopMatrix();

    glPushMatrix();
    glDisable(GL_MULTISAMPLE);
    glTranslatef(0.25f, -0.10f, 0.0f);
    glScalef(0.75f, 1.15f, 0.0f);
    glRotatef(rot, 0.0f, 0.0f, 1.0f);
    glCallList(list);
    glPopMatrix();

    rot += 0.2f;

    qglColor(Qt::black);
    renderText(-0.35, 0.4, 0.0, "Multisampling enabled");
    renderText(0.15, 0.4, 0.0, "Multisampling disabled");
}

void GLWidget::timerEvent(QTimerEvent *)
{
    update();
}

void GLWidget::makeObject()
{
    QColor trolltechGreen(QColor::fromCmykF(0.40, 0.0, 1.0, 0.0));
    const double Pi = 3.14159265358979323846;
    const int NumSectors = 15;
    GLdouble x1 = +0.06;
    GLdouble y1 = -0.14;
    GLdouble x2 = +0.14;
    GLdouble y2 = -0.06;
    GLdouble x3 = +0.08;
    GLdouble y3 = +0.00;
    GLdouble x4 = +0.30;
    GLdouble y4 = +0.22;

    list = glGenLists(1);
    glNewList(list, GL_COMPILE);
    {
        for (int i = 0; i < NumSectors; ++i) {
            double angle1 = (i * 2 * Pi) / NumSectors;
            GLdouble x5 = 0.30 * sin(angle1);
            GLdouble y5 = 0.30 * cos(angle1);
            GLdouble x6 = 0.20 * sin(angle1);
            GLdouble y6 = 0.20 * cos(angle1);

            double angle2 = ((i + 1) * 2 * Pi) / NumSectors;
            GLdouble x7 = 0.20 * sin(angle2);
            GLdouble y7 = 0.20 * cos(angle2);
            GLdouble x8 = 0.30 * sin(angle2);
            GLdouble y8 = 0.30 * cos(angle2);

            qglColor(trolltechGreen);
            quad(GL_QUADS, x5, y5, x6, y6, x7, y7, x8, y8);
            qglColor(Qt::black);
            quad(GL_LINE_LOOP, x5, y5, x6, y6, x7, y7, x8, y8);
        }

        qglColor(trolltechGreen);
        quad(GL_QUADS, x1, y1, x2, y2, y2, x2, y1, x1);
        quad(GL_QUADS, x3, y3, x4, y4, y4, x4, y3, x3);

        qglColor(Qt::black);
        quad(GL_LINE_LOOP, x1, y1, x2, y2, y2, x2, y1, x1);
        quad(GL_LINE_LOOP, x3, y3, x4, y4, y4, x4, y3, x3);
    }
    glEndList();
}

void GLWidget::quad(GLenum primitive, GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2,
                    GLdouble x3, GLdouble y3, GLdouble x4, GLdouble y4)
{
    glBegin(primitive);
    {
        glVertex2d(x1, y1);
        glVertex2d(x2, y2);
        glVertex2d(x3, y3);
        glVertex2d(x4, y4);
    }
    glEnd();
}
