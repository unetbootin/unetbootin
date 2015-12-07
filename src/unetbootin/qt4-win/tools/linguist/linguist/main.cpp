/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt Linguist of the Qt Toolkit.
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

#include "trwindow.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QPixmap>
#include <QTextCodec>
#include <QTranslator>
#include <QSettings>
#include <QSplashScreen>
#include <QLibraryInfo>
#include <QLocale>
#include <QFile>

QT_USE_NAMESPACE

int main(int argc, char **argv)
{
    Q_INIT_RESOURCE(linguist);

    QApplication app(argc, argv);
    QApplication::setOverrideCursor(Qt::WaitCursor);

    QStringList files;
    QString resourceDir = QLibraryInfo::location(QLibraryInfo::TranslationsPath);
    QStringList args = app.arguments();

    for (int i = 1; i < args.count(); ++i)
    {
        QString argument = args.at(i);
        if (argument == QLatin1String("-resourcedir")) {
            if (i + 1 < args.count()) {
                resourceDir = QFile::decodeName(args.at(++i).toLocal8Bit());
            } else {
                // issue a warning
            }
        } else if (!files.contains(argument)) {
            files.append(argument);
        }
    }

    QTranslator translator(0);
    translator.load(QLatin1String("linguist_") + QLocale::system().name(), resourceDir);
    app.installTranslator(&translator);

    QTranslator qtTranslator(0);
    qtTranslator.load(QLatin1String("qt_") + QLocale::system().name(), resourceDir);
    app.installTranslator(&qtTranslator);

    app.setOrganizationName(QLatin1String("Trolltech"));
    app.setApplicationName(QLatin1String("Linguist"));
    QString keybase(QString::number( (QT_VERSION >> 16) & 0xff ) +
                     QLatin1Char('.') + QString::number( (QT_VERSION >> 8) & 0xff ) + QLatin1Char('/') );
    QSettings config;

    QWidget tmp;
    tmp.restoreGeometry(config.value(keybase + QLatin1String("Geometry/WindowGeometry")).toByteArray());

    QSplashScreen *splash = 0;
    int screenId = QApplication::desktop()->screenNumber(tmp.geometry().center());
    splash = new QSplashScreen(QApplication::desktop()->screen(screenId),
        QPixmap(QLatin1String(":/images/splash.png")));
    if (QApplication::desktop()->isVirtualDesktop()) {
        QRect srect(0, 0, splash->width(), splash->height());
        splash->move(QApplication::desktop()->availableGeometry(screenId).center() - srect.center() );
    }
    splash->setAttribute(Qt::WA_DeleteOnClose);
    splash->show();

    TrWindow tw;
    tw.show();

    splash->finish(&tw);

    if (files.count())
        tw.openFile(files.last());

    QApplication::restoreOverrideCursor();

    return app.exec();
}
