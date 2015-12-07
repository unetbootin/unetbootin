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

#include <QtGui>
#include <QtOpenGL>

#include <math.h>

#include "glwidget.h"

//! [0]
GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(parent)
{
    object = 0;
    xRot = 0;
    yRot = 0;
    zRot = 0;

    trolltechGreen = QColor::fromCmykF(0.40, 0.0, 1.0, 0.0);
    trolltechPurple = QColor::fromCmykF(0.39, 0.39, 0.0, 0.0);
}
//! [0]

//! [1]
GLWidget::~GLWidget()
{
    makeCurrent();
    glDeleteLists(object, 1);
}
//! [1]

//! [2]
QSize GLWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}
//! [2]

//! [3]
QSize GLWidget::sizeHint() const
//! [3] //! [4]
{
    return QSize(400, 400);
}
//! [4]

//! [5]
void GLWidget::setXRotation(int angle)
{
    normalizeAngle(&angle);
    if (angle != xRot) {
        xRot = angle;
        emit xRotationChanged(angle);
        updateGL();
    }
}
//! [5]

void GLWidget::setYRotation(int angle)
{
    normalizeAngle(&angle);
    if (angle != yRot) {
        yRot = angle;
        emit yRotationChanged(angle);
        updateGL();
    }
}

void GLWidget::setZRotation(int angle)
{
    normalizeAngle(&angle);
    if (angle != zRot) {
        zRot = angle;
        emit zRotationChanged(angle);
        updateGL();
    }
}

//! [6]
void GLWidget::initializeGL()
{
    qglClearColor(trolltechPurple.dark());
    object = makeObject();
    glShadeModel(GL_FLAT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
}
//! [6]

//! [7]
void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslated(0.0, 0.0, -10.0);
    glRotated(xRot / 16.0, 1.0, 0.0, 0.0);
    glRotated(yRot / 16.0, 0.0, 1.0, 0.0);
    glRotated(zRot / 16.0, 0.0, 0.0, 1.0);
    glCallList(object);
}
//! [7]

//! [8]
void GLWidget::resizeGL(int width, int height)
{
    int side = qMin(width, height);
    glViewport((width - side) / 2, (height - side) / 2, side, side);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-0.5, +0.5, +0.5, -0.5, 4.0, 15.0);
    glMatrixMode(GL_MODELVIEW);
}
//! [8]

//! [9]
void GLWidget::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
}
//! [9]

//! [10]
void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - lastPos.x();
    int dy = event->y() - lastPos.y();

    if (event->buttons() & Qt::LeftButton) {
        setXRotation(xRot + 8 * dy);
        setYRotation(yRot + 8 * dx);
    } else if (event->buttons() & Qt::RightButton) {
        setXRotation(xRot + 8 * dy);
        setZRotation(zRot + 8 * dx);
    }
    lastPos = event->pos();
}
//! [10]

GLuint GLWidget::makeObject()
{
    GLuint list = glGenLists(1);
    glNewList(list, GL_COMPILE);

    glBegin(GL_QUADS);

    GLdouble x1 = +0.06;
    GLdouble y1 = -0.14;
    GLdouble x2 = +0.14;
    GLdouble y2 = -0.06;
    GLdouble x3 = +0.08;
    GLdouble y3 = +0.00;
    GLdouble x4 = +0.30;
    GLdouble y4 = +0.22;

    quad(x1, y1, x2, y2, y2, x2, y1, x1);
    quad(x3, y3, x4, y4, y4, x4, y3, x3);

    extrude(x1, y1, x2, y2);
    extrude(x2, y2, y2, x2);
    extrude(y2, x2, y1, x1);
    extrude(y1, x1, x1, y1);
    extrude(x3, y3, x4, y4);
    extrude(x4, y4, y4, x4);
    extrude(y4, x4, y3, x3);

    const double Pi = 3.14159265358979323846;
    const int NumSectors = 200;

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

        quad(x5, y5, x6, y6, x7, y7, x8, y8);

        extrude(x6, y6, x7, y7);
        extrude(x8, y8, x5, y5);
    }

    glEnd();

    glEndList();
    return list;
}

void GLWidget::quad(GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2,
                    GLdouble x3, GLdouble y3, GLdouble x4, GLdouble y4)
{
    qglColor(trolltechGreen);

    glVertex3d(x1, y1, -0.05);
    glVertex3d(x2, y2, -0.05);
    glVertex3d(x3, y3, -0.05);
    glVertex3d(x4, y4, -0.05);

    glVertex3d(x4, y4, +0.05);
    glVertex3d(x3, y3, +0.05);
    glVertex3d(x2, y2, +0.05);
    glVertex3d(x1, y1, +0.05);
}

void GLWidget::extrude(GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2)
{
    qglColor(trolltechGreen.dark(250 + int(100 * x1)));

    glVertex3d(x1, y1, +0.05);
    glVertex3d(x2, y2, +0.05);
    glVertex3d(x2, y2, -0.05);
    glVertex3d(x1, y1, -0.05);
}

void GLWidget::normalizeAngle(int *angle)
{
    while (*angle < 0)
        *angle += 360 * 16;
    while (*angle > 360 * 16)
        *angle -= 360 * 16;
}
