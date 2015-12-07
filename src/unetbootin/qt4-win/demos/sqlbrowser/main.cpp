/****************************************************************************
**
** Copyright (C) 2004-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the demonstration applications of the Qt Toolkit.
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

#include "browser.h"

#include <QtCore>
#include <QtGui>
#include <QtSql>

void addConnectionsFromCommandline(const QStringList &args, Browser *browser)
{
    for (int i = 1; i < args.count(); ++i) {
        QUrl url(args.at(i), QUrl::TolerantMode);
        if (!url.isValid()) {
            qWarning("Invalid URL: %s", qPrintable(args.at(i)));
            continue;
        }
        QSqlError err = browser->addConnection(url.scheme(), url.path().mid(1), url.host(),
                                               url.userName(), url.password(), url.port(-1));
        if (err.type() != QSqlError::NoError)
            qDebug() << "Unable to open connection:" << err;
    }
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QMainWindow mainWin;
    mainWin.setWindowTitle(QObject::tr("Qt SQL Browser"));

    Browser browser(&mainWin);
    mainWin.setCentralWidget(&browser);

    QMenu *fileMenu = mainWin.menuBar()->addMenu(QObject::tr("&File"));
    fileMenu->addAction(QObject::tr("Add &Connection..."), &browser, SLOT(addConnection()));
    fileMenu->addSeparator();
    fileMenu->addAction(QObject::tr("&Quit"), &app, SLOT(quit()));

    QMenu *helpMenu = mainWin.menuBar()->addMenu(QObject::tr("&Help"));
    helpMenu->addAction(QObject::tr("About"), &browser, SLOT(about()));
    helpMenu->addAction(QObject::tr("About Qt"), qApp, SLOT(aboutQt()));

    QObject::connect(&browser, SIGNAL(statusMessage(QString)),
                     mainWin.statusBar(), SLOT(showMessage(QString)));

    addConnectionsFromCommandline(app.arguments(), &browser);
    mainWin.show();
    if (QSqlDatabase::connectionNames().isEmpty())
        QMetaObject::invokeMethod(&browser, "addConnection", Qt::QueuedConnection);

    return app.exec();
}
