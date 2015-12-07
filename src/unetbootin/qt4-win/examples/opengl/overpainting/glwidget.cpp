/****************************************************************************
**
** Copyright (C) 2006-2008 Trolltech ASA. All rights reserved.
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
#include <stdlib.h>

#include <math.h>

#include "bubble.h"
#include "glwidget.h"

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

//! [0]
GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
    QTime midnight(0, 0, 0);
    qsrand(midnight.secsTo(QTime::currentTime()));

    object = 0;
    xRot = 0;
    yRot = 0;
    zRot = 0;

    trolltechGreen = QColor::fromCmykF(0.40, 0.0, 1.0, 0.0);
    trolltechPurple = QColor::fromCmykF(0.39, 0.39, 0.0, 0.0);

    animationTimer.setSingleShot(false);
    connect(&animationTimer, SIGNAL(timeout()), this, SLOT(animate()));
    animationTimer.start(25);

    setAutoFillBackground(false);
    setMinimumSize(200, 200);
    setWindowTitle(tr("Overpainting a Scene"));
}
//! [0]

//! [1]
GLWidget::~GLWidget()
{
    makeCurrent();
    glDeleteLists(object, 1);
}
//! [1]

void GLWidget::setXRotation(int angle)
{
    normalizeAngle(&angle);
    if (angle != xRot)
        xRot = angle;
}

void GLWidget::setYRotation(int angle)
{
    normalizeAngle(&angle);
    if (angle != yRot)
        yRot = angle;
}

void GLWidget::setZRotation(int angle)
{
    normalizeAngle(&angle);
    if (angle != zRot)
        zRot = angle;
}

//! [2]
void GLWidget::initializeGL()
{
    object = makeObject();
}
//! [2]

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
}

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

void GLWidget::paintEvent(QPaintEvent *event)
{
    makeCurrent();
//! [4]
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
//! [4]

//! [6]
    qglClearColor(trolltechPurple.dark());
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_MULTISAMPLE);
    static GLfloat lightPosition[4] = { 0.5, 5.0, 7.0, 1.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    setupViewport(width(), height());
//! [6]

//! [7]
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslated(0.0, 0.0, -10.0);
    glRotated(xRot / 16.0, 1.0, 0.0, 0.0);
    glRotated(yRot / 16.0, 0.0, 1.0, 0.0);
    glRotated(zRot / 16.0, 0.0, 0.0, 1.0);
    glCallList(object);
//! [7]

//! [8]
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
//! [8]

//! [10]
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    foreach (Bubble *bubble, bubbles) {
        if (bubble->rect().intersects(event->rect()))
            bubble->drawBubble(&painter);
    }
    drawInstructions(&painter);
    painter.end();
}
//! [10]

//! [11]
void GLWidget::resizeGL(int width, int height)
{
    setupViewport(width, height);
}
//! [11]

//! [12]
void GLWidget::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);
    createBubbles(20 - bubbles.count());
}
//! [12]

QSize GLWidget::sizeHint() const
{
    return QSize(400, 400);
}

GLuint GLWidget::makeObject()
{
    GLuint list = glGenLists(1);
    glNewList(list, GL_COMPILE);

    glEnable(GL_NORMALIZE);
    glBegin(GL_QUADS);

    static GLfloat logoDiffuseColor[4] = {trolltechGreen.red()/255.0,
                                          trolltechGreen.green()/255.0,
                                          trolltechGreen.blue()/255.0, 1.0};
    glMaterialfv(GL_FRONT, GL_DIFFUSE, logoDiffuseColor);

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
    glNormal3d(0.0, 0.0, -1.0);
    glVertex3d(x1, y1, -0.05);
    glVertex3d(x2, y2, -0.05);
    glVertex3d(x3, y3, -0.05);
    glVertex3d(x4, y4, -0.05);

    glNormal3d(0.0, 0.0, 1.0);
    glVertex3d(x4, y4, +0.05);
    glVertex3d(x3, y3, +0.05);
    glVertex3d(x2, y2, +0.05);
    glVertex3d(x1, y1, +0.05);
}

void GLWidget::extrude(GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2)
{
    qglColor(trolltechGreen.dark(250 + int(100 * x1)));

    glNormal3d((x1 + x2)/2.0, (y1 + y2)/2.0, 0.0);
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

void GLWidget::createBubbles(int number)
{
    for (int i = 0; i < number; ++i) {
        QPointF position(width()*(0.1 + (0.8*qrand()/(RAND_MAX+1.0))),
                        height()*(0.1 + (0.8*qrand()/(RAND_MAX+1.0))));
        qreal radius = qMin(width(), height())*(0.0125 + 0.0875*qrand()/(RAND_MAX+1.0));
        QPointF velocity(width()*0.0125*(-0.5 + qrand()/(RAND_MAX+1.0)),
                        height()*0.0125*(-0.5 + qrand()/(RAND_MAX+1.0)));

        bubbles.append(new Bubble(position, radius, velocity));
    }
}

//! [13]
void GLWidget::animate()
{
    QMutableListIterator<Bubble*> iter(bubbles);

    while (iter.hasNext()) {
        Bubble *bubble = iter.next();
        bubble->move(rect());
    }
    update();
}
//! [13]

//! [14]
void GLWidget::setupViewport(int width, int height)
{
    int side = qMin(width, height);
    glViewport((width - side) / 2, (height - side) / 2, side, side);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-0.5, +0.5, +0.5, -0.5, 4.0, 15.0);
    glMatrixMode(GL_MODELVIEW);
}
//! [14]

//! [15]
void GLWidget::drawInstructions(QPainter *painter)
{
    QString text = tr("Click and drag with the left mouse button "
                      "to rotate the Qt logo.");
    QFontMetrics metrics = QFontMetrics(font());
    int border = qMax(4, metrics.leading());

    QRect rect = metrics.boundingRect(0, 0, width() - 2*border, int(height()*0.125),
                                      Qt::AlignCenter | Qt::TextWordWrap, text);
    painter->setRenderHint(QPainter::TextAntialiasing);
    painter->fillRect(QRect(0, 0, width(), rect.height() + 2*border),
                     QColor(0, 0, 0, 127));
    painter->setPen(Qt::white);
    painter->fillRect(QRect(0, 0, width(), rect.height() + 2*border),
                      QColor(0, 0, 0, 127));
    painter->drawText((width() - rect.width())/2, border,
                      rect.width(), rect.height(),
                      Qt::AlignCenter | Qt::TextWordWrap, text);
}
//! [15]
