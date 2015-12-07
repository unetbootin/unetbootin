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


#include "interfaces.h"
#include "mainwindow.h"
#include "paintarea.h"
#include "plugindialog.h"

#include <QPluginLoader>
#include <QTimer>

#include <QScrollArea>
#include <QMessageBox>
#include <QActionGroup>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QFileDialog>
#include <QColorDialog>
#include <QInputDialog>
#include <QApplication>

MainWindow::MainWindow() :
    paintArea(new PaintArea),
    scrollArea(new QScrollArea)
{
    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setWidget(paintArea);
    setCentralWidget(scrollArea);

    createActions();
    createMenus();
    loadPlugins();

    setWindowTitle(tr("Plug & Paint"));

    if (!brushActionGroup->actions().isEmpty())
        brushActionGroup->actions().first()->trigger();

    QTimer::singleShot(500, this, SLOT(aboutPlugins()));
}

void MainWindow::open()
{
    const QString fileName = QFileDialog::getOpenFileName(this,
                                                          tr("Open File"), 
                                                          QDir::currentPath());
    if (!fileName.isEmpty()) {
        if (!paintArea->openImage(fileName)) {
            QMessageBox::information(this, tr("Plug & Paint"),
                                     tr("Cannot load %1.").arg(fileName));
            return;
        }
        paintArea->adjustSize();
    }
}

bool MainWindow::saveAs()
{
    const QString initialPath = QDir::currentPath() + "/untitled.png";

    const QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),
                                                          initialPath);
    if (fileName.isEmpty()) {
        return false;
    } else {
        return paintArea->saveImage(fileName, "png");
    }
}

void MainWindow::brushColor()
{
    const QColor newColor = QColorDialog::getColor(paintArea->brushColor());
    if (newColor.isValid())
        paintArea->setBrushColor(newColor);
}

void MainWindow::brushWidth()
{
    bool ok;
    const int newWidth = QInputDialog::getInteger(this, tr("Plug & Paint"),
                                                  tr("Select brush width:"),
                                                  paintArea->brushWidth(),
                                                  1, 50, 1, &ok);
    if (ok)
        paintArea->setBrushWidth(newWidth);
}

//! [0]
void MainWindow::changeBrush()
{
    QAction *action = qobject_cast<QAction *>(sender());
    BrushInterface *iBrush = qobject_cast<BrushInterface *>(action->parent());
    const QString brush = action->text();

    paintArea->setBrush(iBrush, brush);
}
//! [0]

//! [1]
void MainWindow::insertShape()
{
    QAction *action = qobject_cast<QAction *>(sender());
    ShapeInterface *iShape = qobject_cast<ShapeInterface *>(action->parent());

    const QPainterPath path = iShape->generateShape(action->text(), this);
    if (!path.isEmpty())
        paintArea->insertShape(path);
}
//! [1]

//! [2]
void MainWindow::applyFilter()
{
    QAction *action = qobject_cast<QAction *>(sender());
    FilterInterface *iFilter =
            qobject_cast<FilterInterface *>(action->parent());

    const QImage image = iFilter->filterImage(action->text(), paintArea->image(),
                                              this);
    paintArea->setImage(image);
}
//! [2]

void MainWindow::about()
{
   QMessageBox::about(this, tr("About Plug & Paint"),
            tr("The <b>Plug & Paint</b> example demonstrates how to write Qt "
               "applications that can be extended through plugins."));
}

//! [3]
void MainWindow::aboutPlugins()
{
    PluginDialog dialog(pluginsDir.path(), pluginFileNames, this);
    dialog.exec();
}
//! [3]

void MainWindow::createActions()
{
    openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcut(tr("Ctrl+O"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    saveAsAct = new QAction(tr("&Save As..."), this);
    saveAsAct->setShortcut(tr("Ctrl+S"));
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcut(tr("Ctrl+Q"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    brushColorAct = new QAction(tr("&Brush Color..."), this);
    connect(brushColorAct, SIGNAL(triggered()), this, SLOT(brushColor()));

    brushWidthAct = new QAction(tr("&Brush Width..."), this);
    connect(brushWidthAct, SIGNAL(triggered()), this, SLOT(brushWidth()));

    brushActionGroup = new QActionGroup(this);

    aboutAct = new QAction(tr("&About"), this);
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    aboutPluginsAct = new QAction(tr("About &Plugins"), this);
    connect(aboutPluginsAct, SIGNAL(triggered()), this, SLOT(aboutPlugins()));
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    brushMenu = menuBar()->addMenu(tr("&Brush"));
    brushMenu->addAction(brushColorAct);
    brushMenu->addAction(brushWidthAct);
    brushMenu->addSeparator();

    shapesMenu = menuBar()->addMenu(tr("&Shapes"));

    filterMenu = menuBar()->addMenu(tr("&Filter"));

    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);
    helpMenu->addAction(aboutPluginsAct);
}

//! [4]
void MainWindow::loadPlugins()
{
    foreach (QObject *plugin, QPluginLoader::staticInstances())
        populateMenus(plugin);
//! [4] //! [5]

    pluginsDir = QDir(qApp->applicationDirPath());

#if defined(Q_OS_WIN)
    if (pluginsDir.dirName().toLower() == "debug" || pluginsDir.dirName().toLower() == "release")
        pluginsDir.cdUp();
#elif defined(Q_OS_MAC)
    if (pluginsDir.dirName() == "MacOS") {
        pluginsDir.cdUp();
        pluginsDir.cdUp();
        pluginsDir.cdUp();
    }
#endif
    pluginsDir.cd("plugins");
//! [5]

//! [6]
    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
        QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = loader.instance();
        if (plugin) {
            populateMenus(plugin);
            pluginFileNames += fileName;
//! [6] //! [7]
        }
//! [7] //! [8]
    }
//! [8]

//! [9]
    brushMenu->setEnabled(!brushActionGroup->actions().isEmpty());
    shapesMenu->setEnabled(!shapesMenu->actions().isEmpty());
    filterMenu->setEnabled(!filterMenu->actions().isEmpty());
}
//! [9]

//! [10]
void MainWindow::populateMenus(QObject *plugin)
{
    BrushInterface *iBrush = qobject_cast<BrushInterface *>(plugin);
    if (iBrush)
        addToMenu(plugin, iBrush->brushes(), brushMenu, SLOT(changeBrush()),
                  brushActionGroup);

    ShapeInterface *iShape = qobject_cast<ShapeInterface *>(plugin);
    if (iShape)
        addToMenu(plugin, iShape->shapes(), shapesMenu, SLOT(insertShape()));

    FilterInterface *iFilter = qobject_cast<FilterInterface *>(plugin);
    if (iFilter)
        addToMenu(plugin, iFilter->filters(), filterMenu, SLOT(applyFilter()));
}
//! [10]

void MainWindow::addToMenu(QObject *plugin, const QStringList &texts,
                           QMenu *menu, const char *member,
                           QActionGroup *actionGroup)
{
    foreach (QString text, texts) {
        QAction *action = new QAction(text, plugin);
        connect(action, SIGNAL(triggered()), this, member);
        menu->addAction(action);

        if (actionGroup) {
            action->setCheckable(true);
            actionGroup->addAction(action);
        }
    }
}
