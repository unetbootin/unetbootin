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

#include "mainwindow.h"

#include <QApplication>
#include <QMenuBar>
#include <QGroupBox>
#include <QGridLayout>
#include <QSlider>
#include <QLabel>
#include <QTimer>

#include "glwidget.h"

MainWindow::MainWindow()
{
    GLWidget *glwidget = new GLWidget();
    QLabel *label = new QLabel(this);
    QTimer *timer = new QTimer(this);
    QSlider *slider = new QSlider(this);
    slider->setOrientation(Qt::Horizontal);
    
    slider->setRange(0, 100);
    slider->setSliderPosition(50);
    timer->setInterval(10);
    label->setText("A QGlWidget with OpenGl ES");
    label->setAlignment(Qt::AlignHCenter);

    QGroupBox * groupBox = new QGroupBox(this);
    setCentralWidget(groupBox);
    groupBox->setTitle("OpenGL ES Example");

    QGridLayout *layout = new QGridLayout(groupBox);

    layout->addWidget(glwidget,1,0,8,1);
    layout->addWidget(label,9,0,1,1);
    layout->addWidget(slider, 11,0,1,1);

    groupBox->setLayout(layout);

    QMenu *fileMenu = new QMenu("File");
    QMenu *helpMenu = new QMenu("Help");
    QMenu *showMenu = new QMenu("Show");
    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(showMenu);
    menuBar()->addMenu(helpMenu);
    QAction *exit = new QAction("Exit", fileMenu);
    QAction *aboutQt = new QAction("AboutQt", helpMenu);
    QAction *showLogo = new QAction("Show 3D Logo", showMenu);
    QAction *showTexture = new QAction("Show 2D Texture", showMenu);
    QAction *showBubbles = new QAction("Show bubbles", showMenu);
    showBubbles->setCheckable(true);
    showBubbles->setChecked(true);
    fileMenu->addAction(exit);
    helpMenu->addAction(aboutQt);
    showMenu->addAction(showLogo);
    showMenu->addAction(showTexture);
    showMenu->addAction(showBubbles);

    QObject::connect(timer, SIGNAL(timeout()), glwidget, SLOT(updateGL()));
    QObject::connect(exit, SIGNAL(triggered(bool)), this, SLOT(close()));
    QObject::connect(aboutQt, SIGNAL(triggered(bool)), qApp, SLOT(aboutQt()));

    QObject::connect(showLogo, SIGNAL(triggered(bool)), glwidget, SLOT(setLogo()));
    QObject::connect(showTexture, SIGNAL(triggered(bool)), glwidget, SLOT(setTexture()));
    QObject::connect(showBubbles, SIGNAL(triggered(bool)), glwidget, SLOT(showBubbles(bool)));
    QObject::connect(slider, SIGNAL(valueChanged(int)), glwidget, SLOT(setScaling(int)));
    timer->start();
}
