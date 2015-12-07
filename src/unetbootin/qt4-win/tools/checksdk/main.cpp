/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the tools applications of the Qt Toolkit.
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
#include "cesdkhandler.h"
#include <QtCore/QStringList>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QDebug>

void usage()
{
    qDebug() << "SDK Scanner - Convenience Tool to setup your environment";
    qDebug() << "              for crosscompilation to Windows CE";
    qDebug() << "Options:";
    qDebug() << "-help                This output";
    qDebug() << "-list                List all available SDKs";
    qDebug() << "-sdk <name>          Select specified SDK.";
    qDebug() << "                     Note: SDK names with spaces need to be";
    qDebug() << "                     specified in parenthesis";
    qDebug() << "                     default: Windows Mobile 5.0 Pocket PC SDK (ARMV4I)";
    qDebug() << "-script <file>       Create a script file which can be launched";
    qDebug() << "                     to setup your environment for specified SDK";
}

int main(int argc, char **argv)
{
    if (argc == 1) {
        usage();
        return 0;
    }
    QString sdkName;
    bool operationList = false;
    QString scriptFile;

    QStringList arguments;
    for (int i=0; i < argc; ++i)
        arguments.append(QLatin1String(argv[i]));
    for (int i=1; i < arguments.size(); ++i) {
        if (arguments[i].toLower() == QLatin1String("-help")) {
            usage();
            return 0;
        } else if (arguments[i].toLower() == QLatin1String("-list")) {
            operationList = true;
        } else if (arguments[i].toLower() == QLatin1String("-sdk")) {
            if (i+1 >= arguments.size()) {
                qWarning("No SDK specified.");
                return -1;
            }
            sdkName = arguments[++i];
        } else if (arguments[i].toLower() == QLatin1String("-script")) {
            if (i+1 >= arguments.size()) {
                qWarning("No scriptfile specified.");
                return -1;
            }
            scriptFile = arguments[++i];
        } else {
            qWarning("Unknown option:%s", qPrintable(arguments[i]));
            usage();
            return -1;
        }
    }

    // Check for SDK Name, otherwise use Windows Mobile as default
    if (sdkName.isEmpty()) {
        qWarning("No SDK specified: Defaulting to Windows Mobile 5.0 Pocket PC SDK");
        sdkName = QString::fromLatin1("Windows Mobile 5.0 Pocket PC SDK (ARMV4I)");
    }

    CeSdkHandler handler;
    if (!handler.parse()) {
        qWarning("Could not find any installed SDK, aborting!");
        return -1;
    }

    QList<CeSdkInfo> list = handler.listAll();

    if (operationList) {
        qDebug() << "Available SDKs:";
        for (QList<CeSdkInfo>::iterator it = list.begin(); it != list.end(); ++it)
            qDebug() << "SDK Name:" << it->name();
        return 0;
    }

    // finally find the given SDK and prompt out the environment to be set
    for (QList<CeSdkInfo>::iterator it = list.begin(); it != list.end(); ++it ) {
        if (sdkName == it->name()) {
            if (!it->isValid()) {
                qWarning("Selected SDK is not valid!");
                return -1;
            } else if (!it->isSupported()) {
                qWarning("Selected SDK is not officially supported and might not work");
            }
            QString binPath, includePath, libPath;
            binPath = QString::fromLatin1("PATH=") + it->binPath();
            includePath = QString::fromLatin1("INCLUDE=") + it->includePath();
            libPath = QString::fromLatin1("LIB=") + it->libPath();
            if (scriptFile.isEmpty()) {
                qDebug() << "Please set up your environment with the following paths:";
                qDebug() << qPrintable(binPath);
                qDebug() << qPrintable(includePath);
                qDebug() << qPrintable(libPath);
                return 0;
            } else {
                QFile file(scriptFile);
                if (!file.open(QIODevice::WriteOnly)) {
                    qWarning("Could not open target script file");
                    return -1;
                }
                QString content;
                content += QLatin1String("@echo off\n");
                content += QLatin1String("echo Environment Selection:") + sdkName + QLatin1String("\n");
                content += QLatin1String("set ") + binPath + QLatin1String("\n");
                content += QLatin1String("set ") + includePath + QLatin1String("\n");
                content += QLatin1String("set ") + libPath + QLatin1String("\n");
                file.write(content.toLatin1());
                return 0;
            }
        }
    }
    qWarning("Could not find specified SDK: %s" , qPrintable(sdkName));
    return 0;
}