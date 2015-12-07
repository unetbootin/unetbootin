/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
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

#include <rcc.h>
#include "../../corelib/kernel/qcorecmdlineargs_p.h"

#include <QDebug>
#include <QDir>
#include <QFile>

QT_BEGIN_NAMESPACE

int showHelp(const QString &argv0, const QString &error)
{
    fprintf(stderr, "Qt resource compiler\n");
    if (!error.isEmpty())
        fprintf(stderr, "%s: %s\n", qPrintable(argv0), qPrintable(error));
    fprintf(stderr, "Usage: %s  [options] <inputs>\n\n"
            "Options:\n"
            "  -o file           write output to file rather than stdout\n"
            "  -name name        create an external initialization function with name\n"
            "  -threshold level  threshold to consider compressing files\n"
            "  -compress level   compress input files by level\n"
            "  -root path        prefix resource access path with root path\n"
            "  -no-compress      disable all compression\n"
            "  -binary           output a binary file for use as a dynamic resource\n"
            "  -namespace        turn off namespace macros\n"
            "  -version          display version\n"
            "  -help             display this information\n",
            qPrintable(argv0));
    return 1;
}


int runRcc(int argc, char *argv[])
{
    QString outFilename;
    bool helpRequested = false;
    bool list = false;
    QStringList files;

    QStringList args = qCmdLineArgs(argc, argv);

    RCCBuilder builder;

    //parse options
    QString errorMsg;
    for (int i = 1; i < args.count() && errorMsg.isEmpty(); i++) {
        if (args[i].isEmpty())
            continue;
        if (args[i][0] == QLatin1Char('-')) {   // option
            QString opt = args[i];
            if (opt == QLatin1String("-o")) {
                if (!(i < argc-1)) {
                    errorMsg = QLatin1String("Missing output name");
                    break;
                }
                outFilename = args[++i];
            } else if (opt == QLatin1String("-name")) {
                if (!(i < argc-1)) {
                    errorMsg = QLatin1String("Missing target name");
                    break;
                }
                builder.initName = args[++i];
            } else if (opt == QLatin1String("-root")) {
                if (!(i < argc-1)) {
                    errorMsg = QLatin1String("Missing root path");
                    break;
                }
                builder.resourceRoot = QDir::cleanPath(args[++i]);
                if (builder.resourceRoot.isEmpty()
                        || builder.resourceRoot.at(0) != QLatin1Char('/'))
                    errorMsg = QLatin1String("Root must start with a /");
            } else if (opt == QLatin1String("-compress")) {
                if (!(i < argc-1)) {
                    errorMsg = QLatin1String("Missing compression level");
                    break;
                }
                builder.compressLevel = args[++i].toInt();
            } else if (opt == QLatin1String("-threshold")) {
                if (!(i < argc-1)) {
                    errorMsg = QLatin1String("Missing compression threshold");
                    break;
                }
                builder.compressThreshold = args[++i].toInt();
            } else if (opt == QLatin1String("-binary")) {
                builder.writeBinary = true;
            } else if (opt == QLatin1String("-namespace")) {
                builder.useNameSpace = !builder.useNameSpace;
            } else if (opt == QLatin1String("-verbose")) {
                builder.verbose = true;
            } else if (opt == QLatin1String("-list")) {
                list = true;
            } else if (opt == QLatin1String("-version") || opt == QLatin1String("-v")) {
                fprintf(stderr, "Qt Resource Compiler version %s\n", QT_VERSION_STR);
                return 1;
            } else if (opt == QLatin1String("-help") || opt == QLatin1String("-h")) {
                helpRequested = true;
            } else if (opt == QLatin1String("-no-compress")) {
                builder.compressLevel = -2;
            } else {
                errorMsg = QString::fromLatin1("Unknown option: '%1'").arg(args[i]);
            }
        } else {
            if (!QFile::exists(args[i])) {
                qWarning("%s: File does not exist '%s'",
                    qPrintable(args[0]), qPrintable(args[i]));
                return 1;
            }
            files.append(args[i]);
        }
    }

    if (!files.size() || !errorMsg.isEmpty() || helpRequested)
        return showHelp(args[0], errorMsg);
    return int(!builder.processResourceFile(files, outFilename, list));
}

QT_END_NAMESPACE

int main(int argc, char *argv[])
{
    return QT_PREPEND_NAMESPACE(runRcc)(argc, argv);
}
