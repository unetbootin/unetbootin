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

#include "mainwindow.h"

MainWindow::MainWindow()
    : QMainWindow()
{
    area = new SvgWindow;

    QMenu *fileMenu = new QMenu(tr("&File"), this);
    QAction *openAction = fileMenu->addAction(tr("&Open..."));
    openAction->setShortcut(QKeySequence(tr("Ctrl+O")));
    QAction *quitAction = fileMenu->addAction(tr("E&xit"));
    quitAction->setShortcut(QKeySequence(tr("Ctrl+Q")));

    menuBar()->addMenu(fileMenu);

    QMenu *rendererMenu = new QMenu(tr("&Renderer"), this);
    nativeAction = rendererMenu->addAction(tr("&Native"));
    nativeAction->setCheckable(true);
    nativeAction->setChecked(true);
    #ifndef QT_NO_OPENGL
    glAction = rendererMenu->addAction(tr("&OpenGL"));
    glAction->setCheckable(true);
    #endif
    imageAction = rendererMenu->addAction(tr("&Image"));
    imageAction->setCheckable(true);

    #ifndef QT_NO_OPENGL
    rendererMenu->addSeparator();
    highQualityAntialiasingAction = rendererMenu->addAction(tr("&High Quality Antialiasing"));
    highQualityAntialiasingAction->setEnabled(false);
    highQualityAntialiasingAction->setCheckable(true);
    highQualityAntialiasingAction->setChecked(false);
    connect(highQualityAntialiasingAction, SIGNAL(toggled(bool)), this, SLOT(setHighQualityAntialiasing(bool)));
    #endif

    QActionGroup *rendererGroup = new QActionGroup(this);
    rendererGroup->addAction(nativeAction);
    #ifndef QT_NO_OPENGL
    rendererGroup->addAction(glAction);
    #endif
    rendererGroup->addAction(imageAction);

    menuBar()->addMenu(rendererMenu);

    connect(openAction, SIGNAL(triggered()), this, SLOT(openFile()));
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
    connect(rendererGroup, SIGNAL(triggered(QAction *)),
            this, SLOT(setRenderer(QAction *)));

    setCentralWidget(area);
    setWindowTitle(tr("SVG Viewer"));
}

void MainWindow::setHighQualityAntialiasing(bool highQualityAntialiasing)
{
    area->setHighQualityAntialiasing(highQualityAntialiasing);
}

void MainWindow::openFile(const QString &path)
{
    QString fileName;
    if (path.isNull())
        fileName = QFileDialog::getOpenFileName(this, tr("Open SVG File"),
                                                currentPath, "*.svg");
    else
        fileName = path;

    if (!fileName.isEmpty()) {
        area->openFile(fileName);
        if (!fileName.startsWith(":/")) {
            currentPath = fileName;
            setWindowTitle(tr("%1 - SVGViewer").arg(currentPath));
        }
    }
}

void MainWindow::setRenderer(QAction *action)
{
    #ifndef QT_NO_OPENGL
    highQualityAntialiasingAction->setEnabled(false);
    #endif

    if (action == nativeAction)
        area->setRenderer(SvgWindow::Native);
    #ifndef QT_NO_OPENGL
    else if (action == glAction) {
        area->setRenderer(SvgWindow::OpenGL);
        highQualityAntialiasingAction->setEnabled(true);
    }
    #endif
    else if (action == imageAction)
        area->setRenderer(SvgWindow::Image);
}
