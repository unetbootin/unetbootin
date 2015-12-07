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

#include "locationdialog.h"
#include "mainwindow.h"
#include "settingstree.h"

MainWindow::MainWindow()
{
    settingsTree = new SettingsTree;
    setCentralWidget(settingsTree);

    locationDialog = 0;

    createActions();
    createMenus();

    autoRefreshAct->setChecked(true);
    fallbacksAct->setChecked(true);

    setWindowTitle(tr("Settings Editor"));
    resize(500, 600);
}

void MainWindow::openSettings()
{
    if (!locationDialog)
        locationDialog = new LocationDialog(this);

    if (locationDialog->exec()) {
        QSettings *settings = new QSettings(locationDialog->format(),
                                            locationDialog->scope(),
                                            locationDialog->organization(),
                                            locationDialog->application());
        setSettingsObject(settings);
        fallbacksAct->setEnabled(true);
    }
}

void MainWindow::openIniFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open INI File"),
                               "", tr("INI Files (*.ini *.conf)"));
    if (!fileName.isEmpty()) {
        QSettings *settings = new QSettings(fileName, QSettings::IniFormat);
        setSettingsObject(settings);
        fallbacksAct->setEnabled(false);
    }
}

void MainWindow::openPropertyList()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                               tr("Open Property List"),
                               "", tr("Property List Files (*.plist)"));
    if (!fileName.isEmpty()) {
        QSettings *settings = new QSettings(fileName, QSettings::NativeFormat);
        setSettingsObject(settings);
        fallbacksAct->setEnabled(false);
    }
}

void MainWindow::openRegistryPath()
{
    QString path = QInputDialog::getText(this, tr("Open Registry Path"),
                           tr("Enter the path in the Windows registry:"),
                           QLineEdit::Normal, "HKEY_CURRENT_USER\\");
    if (!path.isEmpty()) {
        QSettings *settings = new QSettings(path, QSettings::NativeFormat);
        setSettingsObject(settings);
        fallbacksAct->setEnabled(false);
    }
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About Settings Editor"),
            tr("The <b>Settings Editor</b> example shows how to access "
               "application settings using Qt."));
}

void MainWindow::createActions()
{
    openSettingsAct = new QAction(tr("&Open Application Settings..."), this);
    openSettingsAct->setShortcut(tr("Ctrl+O"));
    connect(openSettingsAct, SIGNAL(triggered()), this, SLOT(openSettings()));

    openIniFileAct = new QAction(tr("Open I&NI File..."), this);
    openIniFileAct->setShortcut(tr("Ctrl+N"));
    connect(openIniFileAct, SIGNAL(triggered()), this, SLOT(openIniFile()));

    openPropertyListAct = new QAction(tr("Open Mac &Property List..."), this);
    openPropertyListAct->setShortcut(tr("Ctrl+P"));
    connect(openPropertyListAct, SIGNAL(triggered()),
            this, SLOT(openPropertyList()));

    openRegistryPathAct = new QAction(tr("Open Windows &Registry Path..."),
                                      this);
    openRegistryPathAct->setShortcut(tr("Ctrl+G"));
    connect(openRegistryPathAct, SIGNAL(triggered()),
            this, SLOT(openRegistryPath()));

    refreshAct = new QAction(tr("&Refresh"), this);
    refreshAct->setShortcut(tr("Ctrl+R"));
    refreshAct->setEnabled(false);
    connect(refreshAct, SIGNAL(triggered()), settingsTree, SLOT(refresh()));

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcut(tr("Ctrl+Q"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    autoRefreshAct = new QAction(tr("&Auto-Refresh"), this);
    autoRefreshAct->setShortcut(tr("Ctrl+A"));
    autoRefreshAct->setCheckable(true);
    autoRefreshAct->setEnabled(false);
    connect(autoRefreshAct, SIGNAL(triggered(bool)),
            settingsTree, SLOT(setAutoRefresh(bool)));
    connect(autoRefreshAct, SIGNAL(triggered(bool)),
            refreshAct, SLOT(setDisabled(bool)));

    fallbacksAct = new QAction(tr("&Fallbacks"), this);
    fallbacksAct->setShortcut(tr("Ctrl+F"));
    fallbacksAct->setCheckable(true);
    fallbacksAct->setEnabled(false);
    connect(fallbacksAct, SIGNAL(triggered(bool)),
            settingsTree, SLOT(setFallbacksEnabled(bool)));

    aboutAct = new QAction(tr("&About"), this);
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

#ifndef Q_WS_MAC
    openPropertyListAct->setEnabled(false);
#endif
#ifndef Q_WS_WIN
    openRegistryPathAct->setEnabled(false);
#endif
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(openSettingsAct);
    fileMenu->addAction(openIniFileAct);
    fileMenu->addAction(openPropertyListAct);
    fileMenu->addAction(openRegistryPathAct);
    fileMenu->addSeparator();
    fileMenu->addAction(refreshAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    optionsMenu = menuBar()->addMenu(tr("&Options"));
    optionsMenu->addAction(autoRefreshAct);
    optionsMenu->addAction(fallbacksAct);

    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);
}

void MainWindow::setSettingsObject(QSettings *settings)
{
    settings->setFallbacksEnabled(fallbacksAct->isChecked());
    settingsTree->setSettingsObject(settings);

    refreshAct->setEnabled(true);
    autoRefreshAct->setEnabled(true);

    QString niceName = settings->fileName();
    niceName.replace("\\", "/");
    int pos = niceName.lastIndexOf("/");
    if (pos != -1)
        niceName.remove(0, pos + 1);

    if (!settings->isWritable())
        niceName = tr("%1 (read only)").arg(niceName);

    setWindowTitle(tr("%1 - %2").arg(niceName).arg(tr("Settings Editor")));
}
