/****************************************************************************
**
** Copyright (C) 2004-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the documentation of the Qt Toolkit.
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

QWidget *win;
QWidget *panel;

void snippet_ctor1()
{
//! [0]
    QSettings settings("MySoft", "Star Runner");
//! [0]
}

void snippet_ctor2()
{
//! [1]
    QCoreApplication::setOrganizationName("MySoft");
//! [1] //! [2]
    QCoreApplication::setOrganizationDomain("mysoft.com");
//! [2] //! [3]
    QCoreApplication::setApplicationName("Star Runner");
//! [3]

//! [4]
    QSettings settings;
//! [4]

//! [5]
    settings.setValue("editor/wrapMargin", 68);
//! [5] //! [6]
    int margin = settings.value("editor/wrapMargin").toInt();
//! [6]
    {
//! [7]
    int margin = settings.value("editor/wrapMargin", 80).toInt();
//! [7]
    }

//! [8]
    settings.setValue("mainwindow/size", win->size());
//! [8] //! [9]
    settings.setValue("mainwindow/fullScreen", win->isFullScreen());
//! [9] //! [10]
    settings.setValue("outputpanel/visible", panel->isVisible());
//! [10]

//! [11]
    settings.beginGroup("mainwindow");
    settings.setValue("size", win->size());
    settings.setValue("fullScreen", win->isFullScreen());
    settings.endGroup();
//! [11]

//! [12]
    settings.beginGroup("outputpanel");
    settings.setValue("visible", panel->isVisible());
    settings.endGroup();
//! [12]
}

void snippet_locations()
{
//! [13]
    QSettings obj1("MySoft", "Star Runner");
//! [13] //! [14]
    QSettings obj2("MySoft");
    QSettings obj3(QSettings::SystemScope, "MySoft", "Star Runner");
    QSettings obj4(QSettings::SystemScope, "MySoft");
//! [14]

    {
//! [15]
    QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                       "MySoft", "Star Runner");
//! [15]
    }

    {
    QSettings settings("starrunner.ini", QSettings::IniFormat);
    }

    {
    QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft",
                       QSettings::NativeFormat);
    }
}

class MainWindow : public QMainWindow
{
public:
    MainWindow();

    void writeSettings();
    void readSettings();

protected:
    void closeEvent(QCloseEvent *event);
};

//! [16]
void MainWindow::writeSettings()
{
    QSettings settings("Moose Soft", "Clipper");

    settings.beginGroup("MainWindow");
    settings.setValue("size", size());
    settings.setValue("pos", pos());
    settings.endGroup();
}
//! [16]

//! [17]
void MainWindow::readSettings()
{
    QSettings settings("Moose Soft", "Clipper");

    settings.beginGroup("MainWindow");
    resize(settings.value("size", QSize(400, 400)).toSize());
    move(settings.value("pos", QPoint(200, 200)).toPoint());
    settings.endGroup();
}
//! [17]

//! [18]
MainWindow::MainWindow()
{
//! [18] //! [19]
    readSettings();
//! [19] //! [20]
}
//! [20]

bool userReallyWantsToQuit() { return true; }

//! [21]
void MainWindow::closeEvent(QCloseEvent *event)
{
    if (userReallyWantsToQuit()) {
        writeSettings();
        event->accept();
    } else {
        event->ignore();
    }
}
//! [21]
