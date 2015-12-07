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

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QString>
#include <QtCore/QStringList>

static int usage(const QStringList & /*args*/)
{
    qDebug() <<
        "\nUsage:\n"
        "    lconvert [options] <infile> [<infile>...]\n\n"
        "If multiple input files are specified, translations from later files\n"
        "take precedence.\n\n"
        "Options:\n"
        "    -h\n"
        "    --help  Display this information and exit.\n\n"
        "    -i <infile>\n"
        "    --input-file <infile>\n"
        "           Specify input file. Use if <infile> might start with a dash.\n"
        "           This option can be used several times to merge inputs.\n"
        "           May be '-' (standard input) for use in a pipe.\n\n"
        "    -o <outfile>\n"
        "    --output-file <outfile>\n"
        "           Specify output file. Default is '-' (standard output).\n\n"
        "    -if <informat>\n"
        "    --input-format <format>\n"
        "           Specify input format for subsequent <infile>s.\n"
        "           Available formats are 'ts', 'po' and 'xlf'.\n"
        "           The format is auto-detected from the file name and defaults to 'ts'.\n\n"
        "    -of <outformat>\n"
        "    --output-format <outformat>\n"
        "           Specify output format. See -if.\n\n"
        "    --drop-tags <regexp>\n"
        "           Drop named extra tags when writing 'ts' or 'xlf' files.\n"
        "           May be specified repeatedly.\n\n"
        "    --drop-translations\n"
        "           Drop existing translations and reset the status to 'unfinished'.\n"
        "           Note: this implies --no-obsolete.\n\n"
        "    --source-language <language>[_<region>]\n"
        "           Specify/override the language of the source strings. Defaults to\n"
        "           POSIX if not specified and the file does not name it yet.\n"
        "    --target-language <language>[_<region>]\n"
        "           Specify/override the language of the translation.\n"
        "           The target language is guessed from the file name if this option\n"
        "           is not specified and the file contents name no language yet.\n\n"
        "    --no-obsolete\n"
        "           Drop obsolete messages.\n\n"
        "    --no-finished\n"
        "           Drop finished messages.\n\n"
        "    --verbose\n"
        "           be a bit more verbose\n\n"
        "Long options can be specified with only one leading dash, too.\n\n"
        "Return value:\n"
        "    0 on success\n"
        "    1 on command line parse failures\n"
        "    2 on read failures\n"
        "    3 on write failures\n";
    return 1;
}

struct File
{
    QString name;
    QString format;
};

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QStringList args = app.arguments();

    QList<File> inFiles;
    QString inFormat(QLatin1String("auto"));
    QString outFileName;
    QString outFormat(QLatin1String("auto"));
    QString targetLanguage;
    QString sourceLanguage;
    bool dropTranslations = false;
    bool noObsolete = false;
    bool noFinished = false;
    bool verbose = false;

    ConversionData cd;
    Translator tr;

    for (int i = 1; i < args.size(); ++i) {
        if (args[i].startsWith(QLatin1String("--")))
            args[i].remove(0, 1);
        if (args[i] == QLatin1String("-o")
         || args[i] == QLatin1String("-output-file")) {
            if (++i >= args.size())
                return usage(args);
            outFileName = args[i];
        } else if (args[i] == QLatin1String("-of")
                || args[i] == QLatin1String("-output-format")) {
            if (++i >= args.size())
                return usage(args);
            outFormat = args[i];
        } else if (args[i] == QLatin1String("-i")
                || args[i] == QLatin1String("-input-file")) {
            if (++i >= args.size())
                return usage(args);
            File file;
            file.name = args[i];
            file.format = inFormat;
            inFiles.append(file);
        } else if (args[i] == QLatin1String("-if")
                || args[i] == QLatin1String("-input-format")) {
            if (++i >= args.size())
                return usage(args);
            inFormat = args[i];
        } else if (args[i] == QLatin1String("-drop-tag")) {
            if (++i >= args.size())
                return usage(args);
            cd.m_dropTags.append(args[i]);
        } else if (args[i] == QLatin1String("-drop-translations")) {
            dropTranslations = true;
        } else if (args[i] == QLatin1String("-target-language")) {
            if (++i >= args.size())
                return usage(args);
            targetLanguage = args[i];
        } else if (args[i] == QLatin1String("-source-language")) {
            if (++i >= args.size())
                return usage(args);
            sourceLanguage = args[i];
        } else if (args[i].startsWith(QLatin1String("-h"))) {
            usage(args);
            return 0;
        } else if (args[i] == QLatin1String("-no-obsolete")) {
            noObsolete = true;
        } else if (args[i] == QLatin1String("-no-finished")) {
            noFinished = true;
        } else if (args[i] == QLatin1String("-verbose")) {
            verbose = true;
        } else if (args[i].startsWith(QLatin1Char('-'))) {
            return usage(args);
        } else {
            File file;
            file.name = args[i];
            file.format = inFormat;
            inFiles.append(file);
        }
    }

    if (inFiles.isEmpty())
        return usage(args);

    tr.setLanguageCode(Translator::guessLanguageCodeFromFileName(inFiles[0].name));
    if (!targetLanguage.isEmpty())
        tr.setLanguageCode(targetLanguage);
    if (!sourceLanguage.isEmpty())
        tr.setSourceLanguageCode(sourceLanguage);

    if (!tr.load(inFiles[0].name, cd, inFiles[0].format)) {
        qWarning() << qPrintable(cd.error());
        return 2;
    }

    for (int i = 1; i < inFiles.size(); ++i) {
        Translator tr2;
        if (!tr2.load(inFiles[i].name, cd, inFiles[i].format)) {
            qWarning() << qPrintable(cd.error());
            return 2;
        }
        for (int j = 0; j < tr2.messageCount(); ++j)
            tr.replaceSorted(tr2.message(j));
    }

    if (noObsolete)
        tr.stripObsoleteMessages();
    if (noFinished)
        tr.stripFinishedMessages();
    if (dropTranslations)
        tr.dropTranslations();

    if (!tr.save(outFileName, cd, outFormat)) {
        qWarning("%s", qPrintable(cd.error()));
        return 3;
    }
    return 0;
}
