/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt Linguist of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "mainwindow.h"

#include <QtCore/QFile>
#include <QtCore/QLibraryInfo>
#include <QtCore/QLocale>
#include <QtCore/QSettings>
#include <QtCore/QTextCodec>
#include <QtCore/QTranslator>

#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>
#include <QtGui/QPixmap>
#include <QtGui/QSplashScreen>

QT_USE_NAMESPACE

int main(int argc, char **argv)
{
    Q_INIT_RESOURCE(linguist);

    QApplication app(argc, argv);
    QApplication::setOverrideCursor(Qt::WaitCursor);

    QStringList files;
    QString resourceDir = QLibraryInfo::location(QLibraryInfo::TranslationsPath);
    QStringList args = app.arguments();

    for (int i = 1; i < args.count(); ++i) {
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

    QTranslator translator;
    translator.load(QLatin1String("linguist_") + QLocale::system().name(), resourceDir);
    app.installTranslator(&translator);

    QTranslator qtTranslator;
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
        splash->move(QApplication::desktop()->availableGeometry(screenId).center() - srect.center());
    }
    splash->setAttribute(Qt::WA_DeleteOnClose);
    splash->show();

    MainWindow mw;
    mw.show();
    splash->finish(&mw);
    QApplication::restoreOverrideCursor();

    mw.openFiles(files, true);

    return app.exec();
}
