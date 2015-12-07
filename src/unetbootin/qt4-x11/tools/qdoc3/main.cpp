/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the tools applications of the Qt Toolkit.
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

/*
  main.cpp
*/

#include <QtCore>

#include <stdlib.h>

#include "apigenerator.h"
#include "codemarker.h"
#include "codeparser.h"
#include "config.h"
#include "cppcodemarker.h"
#include "cppcodeparser.h"
#include "cpptoqsconverter.h"
#include "doc.h"
#include "htmlgenerator.h"
#include "jambiapiparser.h"
#include "javacodemarker.h"
#include "javadocgenerator.h"
#include "linguistgenerator.h"
#include "loutgenerator.h"
#include "mangenerator.h"
#include "plaincodemarker.h"
#include "polyarchiveextractor.h"
#include "polyuncompressor.h"
#include "qsakernelparser.h"
#include "qscodemarker.h"
#include "qscodeparser.h"
#include "sgmlgenerator.h"
#include "webxmlgenerator.h"
#include "tokenizer.h"
#include "tree.h"

QT_BEGIN_NAMESPACE

static const struct {
    const char *key;
    const char *value;
} defaults[] = {
    { CONFIG_CODEINDENT, "4" },
    { CONFIG_FALSEHOODS, "0" },
    { CONFIG_LANGUAGE, "Cpp" },
    { CONFIG_OUTPUTFORMATS, "HTML" },
    { CONFIG_TABSIZE, "8" },
    { 0, 0 }
};

static bool slow = false;
static QStringList defines;
static QHash<QString, Tree *> trees;
static int doxygen = 0;

/*!
  Find the Tree for language \a lang and return a pointer to it.
  If there is no Tree for language \a lang in the Tree table, add
  a new one. The Tree table is indexed by \a lang strings.
 */
static Tree* treeForLanguage(const QString &lang)
{
    Tree* tree = trees.value(lang);
    if (tree == 0) {
        tree = new Tree;
        trees.insert( lang, tree );
    }
    return tree;
}

/*!
  Print the help message to \c stdout.
 */
static void printHelp()
{
    Location::information(tr("Usage: qdoc [options] file1.qdocconf ...\n"
                              "Options:\n"
                              "    -help     "
                              "Display this information and exit\n"
                              "    -version  "
                              "Display version of qdoc and exit\n"
                              "    -D<name>  "
                              "Define <name> as a macro while parsing sources\n"
                              "    -slow     "
                              "Turn on features that slow down qdoc") );
}

/*!
  Prints the qdoc version number to stdout.
 */
static void printVersion()
{
    Location::information(tr("qdoc version 4.4.1"));
}

/*!
  Processes the qdoc config file \a fileName. This is the
  controller for all of qdoc.
 */
static void processQdocconfFile(const QString &fileName)
{
    QList<QTranslator *> translators;

    Config config(tr("qdoc"));

    /*
      Insert the default configuration variables into the
      configuration variable string list map.
     */
    int i = 0;
    while (defaults[i].key) {
	config.setStringList(defaults[i].key,
                             QStringList() << defaults[i].value);
	++i;
    }
    config.setStringList(CONFIG_SLOW, QStringList(slow ? "true" : "false"));

    Location::initialize(config);
    config.load(fileName);
    config.setStringList(CONFIG_DEFINES,
                         defines + config.getStringList(CONFIG_DEFINES));

    Location::terminate();

    QString prevCurrentDir = QDir::currentPath();
    QString dir = QFileInfo(fileName).path();
    if (!dir.isEmpty())
	QDir::setCurrent(dir);

    Location::initialize(config);
    Tokenizer::initialize(config);
    Doc::initialize(config);
    CppToQsConverter::initialize(config);
    CodeMarker::initialize(config);
    CodeParser::initialize(config);
    Generator::initialize(config);

    QStringList fileNames = config.getStringList(CONFIG_TRANSLATORS);
    QStringList::Iterator fn = fileNames.begin();
    while (fn != fileNames.end()) {
	QTranslator *translator = new QTranslator(0);
	if (!translator->load(*fn))
	    config.lastLocation().error(tr("Cannot load translator '%1'")
					 .arg(*fn));
	QCoreApplication::instance()->installTranslator(translator);
	translators.append(translator);
	++fn;
    }

//    QSet<QString> outputLanguages = config.getStringSet(CONFIG_OUTPUTLANGUAGES);

    QString lang = config.getString(CONFIG_LANGUAGE);
    Location langLocation = config.lastLocation();

    // qdoc -> doxygen
    if (doxygen == 2) {
        qDebug() << "READING anchors.txt";
        DoxWriter::readAnchors();
        qDebug() << "READING title maps";
        DoxWriter::readTitles();
        qDebug() << "READING member multimaps";
        DoxWriter::readMembers();
    }

    Tree *tree = new Tree;
    tree->setVersion(config.getString(CONFIG_VERSION));
    CodeParser *codeParser = CodeParser::parserForLanguage(lang);
    if (codeParser == 0)
        config.lastLocation().fatal(tr("Cannot parse programming language '%1'").arg(lang));

    QSet<QString> outputFormats = config.getStringSet(CONFIG_OUTPUTFORMATS);
    Location outputFormatsLocation = config.lastLocation();

    CodeMarker *marker = CodeMarker::markerForLanguage(lang);
    if (!marker && !outputFormats.isEmpty())
	langLocation.fatal(tr("Cannot output documentation for programming language '%1'").arg(lang));

    QStringList indexFiles = config.getStringList(CONFIG_INDEXES);
    tree->readIndexes(indexFiles);

    QSet<QString> excludedDirs;
    QStringList excludedDirsList = config.getStringList(CONFIG_EXCLUDEDIRS);
    foreach (const QString &excludeDir, excludedDirsList)
        excludedDirs.insert(QDir::fromNativeSeparators(excludeDir));

    QSet<QString> headers = QSet<QString>::fromList(
        config.getAllFiles(CONFIG_HEADERS, CONFIG_HEADERDIRS,
                           codeParser->headerFileNameFilter(),
                           excludedDirs));
    QSet<QString>::ConstIterator h = headers.begin();
    while (h != headers.end()) {
	codeParser->parseHeaderFile(config.location(), *h, tree);
	++h;
    }
    codeParser->doneParsingHeaderFiles(tree);

    QSet<QString> sources = QSet<QString>::fromList(
        config.getAllFiles(CONFIG_SOURCES, CONFIG_SOURCEDIRS,
                           codeParser->sourceFileNameFilter(),
                           excludedDirs));
    QSet<QString>::ConstIterator s = sources.begin();
    while (s != sources.end()) {
	codeParser->parseSourceFile(config.location(), *s, tree);
	++s;
    }
    codeParser->doneParsingSourceFiles(tree);
    tree->resolveGroups();
    tree->resolveTargets();
    
    // qdoc -> doxygen
    if (doxygen == 1) {
        DoxWriter::writeAnchors();
        DoxWriter::writeTitles();
        DoxWriter::writeMembers();
    }

    if (doxygen == 0) {
        QSet<QString>::ConstIterator of = outputFormats.begin();
        while (of != outputFormats.end()) {
            Generator *generator = Generator::generatorForFormat(*of);
            if (generator == 0)
                outputFormatsLocation.fatal(tr("Unknown output format '%1'").arg(*of));
            generator->generateTree(tree, marker);
            ++of;
        }

        QString tagFile = config.getString(CONFIG_TAGFILE);
        if (!tagFile.isEmpty())
            tree->generateTagFile(tagFile);

        tree->setVersion("");

        Generator::terminate();
    }

    CodeParser::terminate();
    CodeMarker::terminate();
    CppToQsConverter::terminate();
    Doc::terminate();
    Tokenizer::terminate();
    Location::terminate();
    QDir::setCurrent(prevCurrentDir);

    foreach (QTranslator *translator, translators)
        delete translator;

    delete tree;
}

QT_END_NAMESPACE

int main(int argc, char **argv)
{
    QT_USE_NAMESPACE

    QCoreApplication app(argc, argv);

    PolyArchiveExtractor qsaExtractor(QStringList() << "qsa",
                                      "qsauncompress \1 \2");
    PolyArchiveExtractor tarExtractor(QStringList() << "tar",
                                      "tar -C \2 -xf \1");
    PolyArchiveExtractor tazExtractor(QStringList() << "taz",
                                      "tar -C \2 -Zxf \1");
    PolyArchiveExtractor tbz2Extractor(QStringList() << "tbz" << "tbz2",
                                       "tar -C \2 -jxf \1");
    PolyArchiveExtractor tgzExtractor(QStringList() << "tgz",
                                      "tar -C \2 -zxf \1");
    PolyArchiveExtractor zipExtractor(QStringList() << "zip",
                                      "unzip \1 -d \2");

    PolyUncompressor bz2Uncompressor(QStringList() << "bz" << "bz2",
                                     "bunzip2 -c \1 > \2");
    PolyUncompressor gzAndZUncompressor(QStringList() << "gz" << "z" << "Z",
                                        "gunzip -c \1 > \2");
    PolyUncompressor zipUncompressor(QStringList() << "zip",
                                     "unzip -c \1 > \2");

    CppCodeParser cppParser;
    Tree *cppTree = treeForLanguage(cppParser.language());

    QsCodeParser qsParser(cppTree);
    QsaKernelParser qsaKernelParser(cppTree);
    JambiApiParser jambiParser(cppTree);

    PlainCodeMarker plainMarker;
    CppCodeMarker cppMarker;
    JavaCodeMarker javaMarker;
    QsCodeMarker qsMarker;

    ApiGenerator apiGenerator;
    HtmlGenerator htmlGenerator;
    JavadocGenerator javadocGenerator;
    LinguistGenerator linguistGenerator;
    LoutGenerator loutGenerator;
    ManGenerator manGenerator;
    SgmlGenerator smglGenerator;
    WebXMLGenerator webxmlGenerator;

    QStringList qdocFiles;
    QString opt;
    int i = 1;

    while (i < argc) {
        opt = argv[i++];

	if (opt == "-help") {
	    printHelp();
	    return EXIT_SUCCESS;
	}
        else if (opt == "-version") {
	    printVersion();
	    return EXIT_SUCCESS;
	}
        else if (opt == "--") {
	    while (i < argc)
		qdocFiles.append(argv[i++]);
        }
        else if (opt.startsWith("-D")) {
            QString define = opt.mid(2);
            defines += define;
        }
        else if (opt == "-slow") {
            slow = true;
	}
        else if (opt == "-doxygen1") {
            // qdoc -> doxygen
            // Don't use this; it isn't ready yet.
            qDebug() << "doxygen pass 1";
            doxygen = 1;
            DoxWriter::setDoxPass(1);
        }
        else if (opt == "-doxygen2") {
            // qdoc -> doxygen
            // Don't use this; it isn't ready yet.
            qDebug() << "doxygen pass 2";
            doxygen = 2;
            DoxWriter::setDoxPass(2);
        }
        else {
	    qdocFiles.append(opt);
	}
    }

    if (qdocFiles.isEmpty()) {
        printHelp();
        return EXIT_FAILURE;
    }

    /*
      Main loop.
     */
    foreach (QString qf, qdocFiles)
	processQdocconfFile(qf);

    qDeleteAll(trees);
    return EXIT_SUCCESS;
}

