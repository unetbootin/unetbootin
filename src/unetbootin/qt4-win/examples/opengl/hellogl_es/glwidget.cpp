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
#include <QPainter>
#include <math.h>

#include "bubble.h"
#include "cl_helper.h"


const int bubbleNum = 8;

inline void CrossProduct(qreal &xOut, qreal &yOut, qreal &zOut, qreal x1, qreal y1, qreal z1, qreal x2, qreal y2, qreal z2)
{
   xOut = y1 * z2 - z1 * y2;
   yOut = z1 * x2 - x1 * z2;
   zOut = x1 * y2 - y1 * x2;
}

inline void Normalize(qreal &x, qreal &y, qreal &z)
{
    qreal l = sqrt(x*x + y*y + z*z);
    x = x / l;
    y = y / l;
    z = z / l;
}

GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(parent)
{
    qtLogo = true;
    createdVertices = 0;
    createdNormals = 0;
    m_vertexNumber = 0;
    frames = 0;
    setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_NoSystemBackground);
    setAutoBufferSwap(false);
    m_showBubbles = true;
}

GLWidget::~GLWidget()
{
  if (createdVertices)
      delete[] createdVertices;
  if (createdNormals)
      delete[] createdNormals;
}

void GLWidget::setScaling(int scale) {

    if (scale > 50)
        m_fScale = 1 + qreal(scale -50) / 50 * 0.5;
    else if (scale < 50)
        m_fScale =  1- (qreal(50 - scale) / 50 * 1/2);
    else 
      m_fScale = 1;
}

void GLWidget::setLogo() {
    qtLogo = true;
}

void GLWidget::setTexture() {
    qtLogo = false;
}

void GLWidget::showBubbles(bool bubbles)
{
   m_showBubbles = bubbles;
}

//! [2]
void GLWidget::paintQtLogo()
{
    glDisable(GL_TEXTURE_2D);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3,q_vertexTypeEnum,0, createdVertices);
    glEnableClientState(GL_NORMAL_ARRAY);
    glNormalPointer(q_vertexTypeEnum,0,createdNormals);
    glDrawArrays(GL_TRIANGLES, 0, m_vertexNumber / 3);
}
//! [2]

void GLWidget::paintTexturedCube()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m_uiTexture);
    q_vertexType afVertices[] = {
        f2vt(-0.5), f2vt(0.5), f2vt(0.5), f2vt(0.5),f2vt(-0.5),f2vt(0.5),f2vt(-0.5),f2vt(-0.5),f2vt(0.5),
        f2vt(0.5), f2vt(-0.5), f2vt(0.5), f2vt(-0.5),f2vt(0.5),f2vt(0.5),f2vt(0.5),f2vt(0.5),f2vt(0.5),
        f2vt(-0.5), f2vt(-0.5), f2vt(-0.5), f2vt(0.5),f2vt(-0.5),f2vt(-0.5),f2vt(-0.5),f2vt(0.5),f2vt(-0.5),
        f2vt(0.5), f2vt(0.5), f2vt(-0.5), f2vt(-0.5),f2vt(0.5),f2vt(-0.5),f2vt(0.5),f2vt(-0.5),f2vt(-0.5),

        f2vt(0.5), f2vt(-0.5), f2vt(-0.5), f2vt(0.5),f2vt(-0.5),f2vt(0.5),f2vt(0.5),f2vt(0.5),f2vt(-0.5),
        f2vt(0.5), f2vt(0.5), f2vt(0.5), f2vt(0.5),f2vt(0.5),f2vt(-0.5),f2vt(0.5),f2vt(-0.5),f2vt(0.5),
        f2vt(-0.5), f2vt(0.5), f2vt(-0.5), f2vt(-0.5),f2vt(-0.5),f2vt(0.5),f2vt(-0.5),f2vt(-0.5),f2vt(-0.5),
        f2vt(-0.5), f2vt(-0.5), f2vt(0.5), f2vt(-0.5),f2vt(0.5),f2vt(-0.5),f2vt(-0.5),f2vt(0.5),f2vt(0.5),

        f2vt(0.5), f2vt(0.5),  f2vt(-0.5), f2vt(-0.5), f2vt(0.5),  f2vt(0.5),  f2vt(-0.5),  f2vt(0.5),  f2vt(-0.5),
        f2vt(-0.5),  f2vt(0.5),  f2vt(0.5),  f2vt(0.5),  f2vt(0.5),  f2vt(-0.5), f2vt(0.5), f2vt(0.5),  f2vt(0.5),
        f2vt(-0.5),  f2vt(-0.5), f2vt(-0.5), f2vt(-0.5), f2vt(-0.5), f2vt(0.5),  f2vt(0.5), f2vt(-0.5), f2vt(-0.5),
        f2vt(0.5), f2vt(-0.5), f2vt(0.5),  f2vt(0.5),  f2vt(-0.5), f2vt(-0.5), f2vt(-0.5),  f2vt(-0.5), f2vt(0.5)
    };
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3,q_vertexTypeEnum,0,afVertices);

    q_vertexType afTexCoord[] = {
        f2vt(0.0f),f2vt(0.0f), f2vt(1.0f),f2vt(1.0f), f2vt(1.0f),f2vt(0.0f),
        f2vt(1.0f),f2vt(1.0f), f2vt(0.0f),f2vt(0.0f), f2vt(0.0f),f2vt(1.0f),
        f2vt(1.0f),f2vt(1.0f), f2vt(1.0f),f2vt(0.0f), f2vt(0.0f),f2vt(1.0f),
        f2vt(0.0f),f2vt(0.0f), f2vt(0.0f),f2vt(1.0f), f2vt(1.0f),f2vt(0.0f),

        f2vt(1.0f),f2vt(1.0f), f2vt(1.0f),f2vt(0.0f), f2vt(0.0f),f2vt(1.0f),
        f2vt(0.0f),f2vt(0.0f), f2vt(0.0f),f2vt(1.0f), f2vt(1.0f),f2vt(0.0f),
        f2vt(0.0f),f2vt(0.0f), f2vt(1.0f),f2vt(1.0f), f2vt(1.0f),f2vt(0.0f),
        f2vt(1.0f),f2vt(1.0f), f2vt(0.0f),f2vt(0.0f), f2vt(0.0f),f2vt(1.0f),

        f2vt(0.0f),f2vt(1.0f), f2vt(1.0f),f2vt(0.0f), f2vt(1.0f),f2vt(1.0f),
        f2vt(1.0f),f2vt(0.0f), f2vt(0.0f),f2vt(1.0f), f2vt(0.0f),f2vt(0.0f),
        f2vt(1.0f),f2vt(0.0f), f2vt(1.0f),f2vt(1.0f), f2vt(0.0f),f2vt(0.0f),
        f2vt(0.0f),f2vt(1.0f), f2vt(0.0f),f2vt(0.0f), f2vt(1.0f),f2vt(1.0f)
    };
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(2,q_vertexTypeEnum,0,afTexCoord);

    q_vertexType afNormals[] = {

        f2vt(0),f2vt(0),f2vt(-1), f2vt(0),f2vt(0),f2vt(-1), f2vt(0),f2vt(0),f2vt(-1),
        f2vt(0),f2vt(0),f2vt(-1), f2vt(0),f2vt(0),f2vt(-1), f2vt(0),f2vt(0),f2vt(-1),
        f2vt(0),f2vt(0),f2vt(1), f2vt(0),f2vt(0),f2vt(1), f2vt(0),f2vt(0),f2vt(1),
        f2vt(0),f2vt(0),f2vt(1), f2vt(0),f2vt(0),f2vt(1), f2vt(0),f2vt(0),f2vt(1),

        f2vt(-1),f2vt(0),f2vt(0), f2vt(-1),f2vt(0),f2vt(0), f2vt(-1),f2vt(0),f2vt(0),
        f2vt(-1),f2vt(0),f2vt(0), f2vt(-1),f2vt(0),f2vt(0), f2vt(-1),f2vt(0),f2vt(0),
        f2vt(1),f2vt(0),f2vt(0), f2vt(1),f2vt(0),f2vt(0), f2vt(1),f2vt(0),f2vt(0),
        f2vt(1),f2vt(0),f2vt(0), f2vt(1),f2vt(0),f2vt(0), f2vt(1),f2vt(0),f2vt(0),

        f2vt(0),f2vt(-1),f2vt(0), f2vt(0),f2vt(-1),f2vt(0), f2vt(0),f2vt(-1),f2vt(0),
        f2vt(0),f2vt(-1),f2vt(0), f2vt(0),f2vt(-1),f2vt(0), f2vt(0),f2vt(-1),f2vt(0),
        f2vt(0),f2vt(1),f2vt(0), f2vt(0),f2vt(1),f2vt(0), f2vt(0),f2vt(1),f2vt(0),
        f2vt(0),f2vt(1),f2vt(0), f2vt(0),f2vt(1),f2vt(0), f2vt(0),f2vt(1),f2vt(0)
    };
    glEnableClientState(GL_NORMAL_ARRAY);
    glNormalPointer(q_vertexTypeEnum,0,afNormals);

    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void GLWidget::initializeGL ()
{
    q_glClearColor(f2vt(0.1f), f2vt(0.1f), f2vt(0.2f), f2vt(1.0f));

    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &m_uiTexture);
    m_uiTexture = bindTexture(QImage(":/qt.png"));

    q_glTexParameter(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    q_glTexParameter(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    q_vertexType aLightPosition[] = {f2vt(0.0f),f2vt(0.3f),f2vt(1.0f),f2vt(0.0f)};

    q_glLightv(GL_LIGHT0, GL_SPOT_DIRECTION, aLightPosition);
    m_fAngle = 0;
    m_fScale = 1;
    createGeometry();
    createBubbles(bubbleNum - bubbles.count());
}

void GLWidget::paintGL()
{
    createBubbles(bubbleNum - bubbles.count());

//! [3]    
    QPainter painter;
    painter.begin(this);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
//! [3]    

//! [4]    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glMatrixMode(GL_TEXTURE);
    glPushMatrix();

    //Since OpenGL ES does not support glPush/PopAttrib(GL_ALL_ATTRIB_BITS) 
    //we have to take care of the states ourselves

    q_glClearColor(f2vt(0.1f), f2vt(0.1f), f2vt(0.2f), f2vt(1.0f));
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_TEXTURE_2D);

    q_glTexParameter(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    q_glTexParameter(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glShadeModel(GL_FLAT);
    glFrontFace(GL_CW);
    glCullFace(GL_FRONT);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    q_glRotate(f2vt(m_fAngle), f2vt(0.0), f2vt(1.0), f2vt(0.0));
    q_glRotate(f2vt(m_fAngle), f2vt(1.0), f2vt(0.0), f2vt(0.0));
    q_glRotate(f2vt(m_fAngle), f2vt(0.0), f2vt(0.0), f2vt(1.0));
    q_glScale(f2vt(m_fScale), f2vt(m_fScale),f2vt(m_fScale));
    q_glTranslate(f2vt(0),f2vt(-0.2),f2vt(0));

    q_vertexType matDiff[] = {f2vt(0.40), f2vt(1.0), f2vt(0.0), f2vt(1.0)};
    q_glMaterialv(GL_FRONT_AND_BACK, GL_DIFFUSE, matDiff);

    if (qtLogo)
        paintQtLogo();
    else
        paintTexturedCube();
//! [4]    

//![5]
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_TEXTURE);
    glPopMatrix();

    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
//![5]

//! [6]
    if (m_showBubbles)
        foreach (Bubble *bubble, bubbles) {
            bubble->drawBubble(&painter);
    }
//! [6]

//! [7]
    QString framesPerSecond;
    framesPerSecond.setNum(frames /(time.elapsed() / 1000.0), 'f', 2);

    painter.setPen(Qt::white);

    painter.drawText(20, 40, framesPerSecond + " fps");

    painter.end();
//! [7]
    
//! [8]
    swapBuffers();
//! [8]

    QMutableListIterator<Bubble*> iter(bubbles);

    while (iter.hasNext()) {
        Bubble *bubble = iter.next();
        bubble->move(rect());
    }
    if (!(frames % 100)) {
        time.start();
        frames = 0;
    }
    m_fAngle += 1.0f;
    frames ++;
}

void GLWidget::createBubbles(int number)
{
    for (int i = 0; i < number; ++i) {
        QPointF position(width()*(0.1 + (0.8*qrand()/(RAND_MAX+1.0))),
                        height()*(0.1 + (0.8*qrand()/(RAND_MAX+1.0))));
        qreal radius = qMin(width(), height())*(0.0175 + 0.0875*qrand()/(RAND_MAX+1.0));
        QPointF velocity(width()*0.0175*(-0.5 + qrand()/(RAND_MAX+1.0)),
                        height()*0.0175*(-0.5 + qrand()/(RAND_MAX+1.0)));

        bubbles.append(new Bubble(position, radius, velocity));
    }
}

void GLWidget::createGeometry()
{
    vertices.clear();
    normals.clear();

    qreal x1 = +0.06f;
    qreal y1 = -0.14f;
    qreal x2 = +0.14f;
    qreal y2 = -0.06f;
    qreal x3 = +0.08f;
    qreal y3 = +0.00f;
    qreal x4 = +0.30f;
    qreal y4 = +0.22f;

    quad(x1, y1, x2, y2, y2, x2, y1, x1);
    quad(x3, y3, x4, y4, y4, x4, y3, x3);

    extrude(x1, y1, x2, y2);
    extrude(x2, y2, y2, x2);
    extrude(y2, x2, y1, x1);
    extrude(y1, x1, x1, y1);
    extrude(x3, y3, x4, y4);
    extrude(x4, y4, y4, x4);
    extrude(y4, x4, y3, x3);

    const qreal Pi = 3.14159f;
    const int NumSectors = 100;

    for (int i = 0; i < NumSectors; ++i) {
        qreal angle1 = (i * 2 * Pi) / NumSectors;
        qreal x5 = 0.30 * sin(angle1);
        qreal y5 = 0.30 * cos(angle1);
        qreal x6 = 0.20 * sin(angle1);
        qreal y6 = 0.20 * cos(angle1);

        qreal angle2 = ((i + 1) * 2 * Pi) / NumSectors;
        qreal x7 = 0.20 * sin(angle2);
        qreal y7 = 0.20 * cos(angle2);
        qreal x8 = 0.30 * sin(angle2);
        qreal y8 = 0.30 * cos(angle2);

        quad(x5, y5, x6, y6, x7, y7, x8, y8);

        extrude(x6, y6, x7, y7);
        extrude(x8, y8, x5, y5);
    }

//! [1]
    m_vertexNumber = vertices.size();
    createdVertices = new q_vertexType[m_vertexNumber];
    createdNormals = new q_vertexType[m_vertexNumber];
    for (int i = 0;i < m_vertexNumber;i++) {
      createdVertices[i] = f2vt(vertices.at(i) * 2);
      createdNormals[i] = f2vt(normals.at(i));
    }
    vertices.clear();
    normals.clear();
}
//! [1]

//! [0]
void GLWidget::quad(qreal x1, qreal y1, qreal x2, qreal y2, qreal x3, qreal y3, qreal x4, qreal y4)
{
    qreal nx, ny, nz;

    vertices << x1 << y1 << -0.05f;
    vertices << x2 << y2 << -0.05f;
    vertices << x4 << y4 << -0.05f;

    vertices << x3 << y3 << -0.05f;
    vertices << x4 << y4 << -0.05f;
    vertices << x2 << y2 << -0.05f;

    CrossProduct(nx, ny, nz, x2 - x1, y2 - y1, 0, x4 - x1, y4 - y1, 0);
    Normalize(nx, ny, nz);

    normals << nx << ny << nz;
    normals << nx << ny << nz;
    normals << nx << ny << nz;

    normals << nx << ny << nz;
    normals << nx << ny << nz;
    normals << nx << ny << nz;

    vertices << x4 << y4 << 0.05f;
    vertices << x2 << y2 << 0.05f;
    vertices << x1 << y1 << 0.05f;

    vertices << x2 << y2 << 0.05f;
    vertices << x4 << y4 << 0.05f;
    vertices << x3 << y3 << 0.05f;

    CrossProduct(nx, ny, nz, x2 - x4, y2 - y4, 0, x1 - x4, y1 - y4, 0);
    Normalize(nx, ny, nz);

    normals << nx << ny << nz;
    normals << nx << ny << nz;
    normals << nx << ny << nz;

    normals << nx << ny << nz;
    normals << nx << ny << nz;
    normals << nx << ny << nz;
}
//! [0]

void GLWidget::extrude(qreal x1, qreal y1, qreal x2, qreal y2)
{
    qreal nx, ny, nz;

    vertices << x1 << y1 << +0.05f;
    vertices << x2 << y2 << +0.05f;
    vertices << x1 << y1 << -0.05f;

    vertices << x2 << y2 << -0.05f;
    vertices << x1 << y1 << -0.05f;
    vertices << x2 << y2 << +0.05f;

    CrossProduct(nx, ny, nz, x2 - x1, y2 - y1, 0.0f, 0.0f, 0.0f, -0.1f);
    Normalize(nx, ny, nz);

    normals << nx << ny << nz;
    normals << nx << ny << nz;
    normals << nx << ny << nz;

    normals << nx << ny << nz;
    normals << nx << ny << nz;
    normals << nx << ny << nz;
}
