/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt Assistant of the Qt Toolkit.
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

#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QLocale>
#include <QtCore/QTranslator>
#include <QtCore/QLibraryInfo>

#include <QtGui/QApplication>
#include <QtGui/QDesktopServices>

#include <QtHelp/QHelpEngineCore>

#include "mainwindow.h"
#include "cmdlineparser.h"

QT_USE_NAMESPACE

#if defined(USE_STATIC_SQLITE_PLUGIN)
  #include <QtPlugin>
  Q_IMPORT_PLUGIN(qsqlite)
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CmdLineParser cmd;
    CmdLineParser::Result res = cmd.parse(a.arguments());
    if (res == CmdLineParser::Help)
        return 0;
    else if (res == CmdLineParser::Error)
        return -1;

    if (cmd.registerRequest() != CmdLineParser::None) {
        QHelpEngineCore help(cmd.collectionFile());
        help.setupData();
        if (cmd.registerRequest() == CmdLineParser::Register) {
            if (!help.registerDocumentation(cmd.helpFile())) {
                cmd.showMessage(
                    QObject::tr("Could not register documentation file\n%1\n\nReason:\n%2")
                    .arg(cmd.helpFile()).arg(help.error()), true);
                return -1;
            } else {
                cmd.showMessage(QObject::tr("Documentation successfully registered."),
                    false);
            }
        } else {
            if (!help.unregisterDocumentation(QHelpEngineCore::namespaceName(cmd.helpFile()))) {
                cmd.showMessage(
                    QObject::tr("Could not unregister documentation file\n%1\n\nReason:\n%2")
                    .arg(cmd.helpFile()).arg(help.error()), true);
                return -1;
            } else {
                cmd.showMessage(QObject::tr("Documentation successfully unregistered."),
                    false);
            }
        }
        return 0;
    }

    if (!cmd.collectionFile().isEmpty()) {
        QHelpEngineCore he(cmd.collectionFile());
        if (!he.setupData()) {
            cmd.showMessage(QObject::tr("The specified collection file could not be read!"),
                true);
            return -1;
        }
        QString fileName = QFileInfo(cmd.collectionFile()).fileName();
        QString dir = he.customValue(QLatin1String("CacheDirectory"),
            QString()).toString();

        QString dataDir = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
        if (dataDir.isEmpty()) {
            if (dir.isEmpty())
                dir = QDir::homePath() + QDir::separator() + QLatin1String(".assistant");
            else
                dir = QDir::homePath() + QLatin1String("/.") + QDir::cleanPath(dir);
        } else {
            if (dir.isEmpty())
                dir = dataDir + QLatin1String("/Trolltech/Assistant");
            else
                dir = dataDir + QDir::separator() + dir;
        }
                
        QFileInfo fi(dir + QDir::separator() + fileName);
        if (!fi.exists()
            && !he.copyCollectionFile(fi.absoluteFilePath())) {
            cmd.showMessage(he.error(), true);                
            return -1;
        }
        cmd.setCollectionFile(fi.absoluteFilePath());
    }
    
    QString resourceDir = QLibraryInfo::location(QLibraryInfo::TranslationsPath);
    QTranslator translator(0);
    translator.load(QLatin1String("assistant_") + QLocale::system().name(), resourceDir);
    a.installTranslator(&translator);

    QTranslator qtTranslator(0);
    qtTranslator.load(QLatin1String("qt_") + QLocale::system().name(), resourceDir);
    a.installTranslator(&qtTranslator);

    QTranslator qtHelpTranslator(0);
    qtHelpTranslator.load(QLatin1String("qt_help_") + QLocale::system().name(), resourceDir);
    a.installTranslator(&qtHelpTranslator);

    MainWindow w(&cmd);
    w.show();
    a.connect(&a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()));
    return a.exec();
}
