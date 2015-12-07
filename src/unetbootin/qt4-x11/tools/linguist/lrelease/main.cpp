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

#include "translator.h"
#include "translatortools.h"
#include "profileevaluator.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QRegExp>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QTextStream>
#include <QtCore/QTranslator>

static void printOut(const QString & out)
{
    QTextStream stream(stdout);
    stream << out;
}

static void printUsage()
{
    printOut(QCoreApplication::tr(
        "Usage:\n"
        "    lrelease [options] project-file\n"
        "    lrelease [options] ts-files [-qm qm-file]\n"
        "Options:\n"
        "    -help  Display this information and exit\n"
        "    -compress\n"
        "           Compress the .qm files\n"
        "    -nounfinished\n"
        "           Do not include unfinished translations\n"
        "    -removeidentical\n"
        "           If the translated text is the same as\n"
        "           the source text, do not include the message\n"
        "    -silent\n"
        "           Don't explain what is being done\n"
        "    -version\n"
        "           Display the version of lrelease and exit\n"
    ));
}

static bool loadTsFile(Translator &tor, const QString &tsFileName, bool /* verbose */)
{
    ConversionData cd;
    bool ok = tor.load(tsFileName, cd, QLatin1String("auto"));
    if (!ok) {
        qWarning("lrelease error: %s\n", qPrintable(cd.error()));
    } else {
        if (!cd.errors().isEmpty())
            printOut(cd.error());
        const QList<TranslatorMessage> dupes = tor.findDuplicates();
        if (!dupes.isEmpty()) {
            qWarning("lrelease error: duplicate messages found in '%s':",
                     qPrintable(tsFileName));
            foreach (const TranslatorMessage &msg, dupes) {
                qWarning("\n* Context: %s\n* Source: %s",
                        qPrintable(msg.context()),
                        qPrintable(msg.sourceText()));
                if (!msg.comment().isEmpty())
                    qWarning("\n* Comment: %s", qPrintable(msg.comment()));
            }
            ok = false;
        }
    }
    return ok;
}

static bool releaseTranslator(Translator &tor, const QString &qmFileName,
    bool verbose, bool ignoreUnfinished,
    bool removeIdentical, TranslatorSaveMode mode)
{
    if (verbose)
        printOut(QCoreApplication::tr( "Updating '%1'...\n").arg(qmFileName));
    if (removeIdentical) {
        if ( verbose )
            printOut(QCoreApplication::tr( "Removing translations equal to source text in '%1'...\n").arg(qmFileName));
        tor.stripIdenticalSourceTranslations();
    }

    QFile file(qmFileName);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning("lrelease error: cannot create '%s': %s\n",
                 qPrintable(qmFileName), qPrintable(file.errorString()));
        return false;
    }

    ConversionData cd;
    cd.m_verbose = verbose;
    cd.m_ignoreUnfinished = ignoreUnfinished;
    cd.m_saveMode = mode;
    bool ok = tor.release(&file, cd);
    file.close();

    if (!ok) {
        qWarning("lrelease error: cannot save '%s': %s\n",
                 qPrintable(qmFileName), qPrintable(cd.error()));
        return false;
    } else if (!cd.errors().isEmpty()) {
        printOut(cd.error());
    }
    return true;
}

static bool releaseTsFile(const QString& tsFileName, bool verbose,
    bool ignoreUnfinished, bool removeIdentical, TranslatorSaveMode mode)
{
    Translator tor;
    if (!loadTsFile(tor, tsFileName, verbose))
        return false;

    QString qmFileName = tsFileName;
    foreach (const Translator::FileFormat &fmt, Translator::registeredFileFormats()) {
        if (qmFileName.endsWith(QLatin1Char('.') + fmt.extension)) {
            qmFileName.chop(fmt.extension.length() + 1);
            break;
        }
    }
    qmFileName += QLatin1String(".qm");

    return releaseTranslator(tor, qmFileName, verbose, ignoreUnfinished, removeIdentical, mode);
}

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    QStringList args = app.arguments();
    QTranslator translator;
    if (translator.load(QLatin1String("lrelease_") + QLocale::system().name()))
        app.installTranslator(&translator);

    bool verbose = true; // the default is true starting with Qt 4.2
    bool ignoreUnfinished = false;
    // the default mode is SaveEverything starting with Qt 4.2
    TranslatorSaveMode mode = SaveEverything;
    bool removeIdentical = false;
    Translator tor;
    QString outputFile;
    int numFiles = 0;

    for (int i = 1; i < argc; ++i) {
        if (args[i] == QLatin1String("-compress")) {
            mode = SaveStripped;
            continue;
        } else if (args[i] == QLatin1String("-nocompress")) {
            mode = SaveEverything;
            continue;
        } else if (args[i] == QLatin1String("-removeidentical")) {
            removeIdentical = true;
            continue;
        } else if (args[i] == QLatin1String("-nounfinished")) {
            ignoreUnfinished = true;
            continue;
        } else if (args[i] == QLatin1String("-silent")) {
            verbose = false;
            continue;
        } else if (args[i] == QLatin1String("-verbose")) {
            verbose = true;
            continue;
        } else if (args[i] == QLatin1String("-version")) {
            printOut(QCoreApplication::tr( "lrelease version %1\n").arg(QLatin1String(QT_VERSION_STR)) );
            return 0;
        } else if (args[i] == QLatin1String("-qm")) {
            if (i == argc - 1) {
                printUsage();
                return 1;
            }
            i++;
            outputFile = args[i];
        } else if (args[i] == QLatin1String("-help")) {
            printUsage();
            return 0;
        } else if (args[i][0] == QLatin1Char('-')) {
            printUsage();
            return 1;
        } else {
            numFiles++;
        }
    }

    if (numFiles == 0) {
        printUsage();
        return 1;
    }

    for (int i = 1; i < argc; ++i) {
        if (args[i][0] == QLatin1Char('-') || args[i] == outputFile)
            continue;

        if (args[i].endsWith(QLatin1String(".pro"), Qt::CaseInsensitive)
            || args[i].endsWith(QLatin1String(".pri"), Qt::CaseInsensitive)) {
            QHash<QByteArray, QStringList> varMap;
            bool ok = evaluateProFile(args[i], verbose, &varMap );
            if (ok) {
                QStringList translations = varMap.value("TRANSLATIONS");
                if (translations.isEmpty()) {
                    qWarning("lrelease warning: Met no 'TRANSLATIONS' entry in"
                             " project file '%s'\n",
                             qPrintable(args[i]));
                } else {
                    foreach (const QString &trans, translations)
                        if (!releaseTsFile(trans, verbose, ignoreUnfinished, removeIdentical, mode))
                            return 1;
                }
            } else {
                qWarning("error: lrelease encountered project file functionality that is currently not supported.\n"
                    "You might want to consider using .ts files as input instead of a project file.\n"
                    "Try the following syntax:\n"
                    "    lrelease [options] ts-files [-qm qm-file]\n");
            }
        } else {
            if (outputFile.isEmpty()) {
                if (!releaseTsFile(args[i], verbose, ignoreUnfinished, removeIdentical, mode))
                    return 1;
            } else {
                if (!loadTsFile(tor, args[i], verbose))
                    return 1;
            }
        }
    }

    if (!outputFile.isEmpty())
        return releaseTranslator(tor, outputFile, verbose, ignoreUnfinished,
                                 removeIdentical, mode) ? 0 : 1;

    return 0;
}
