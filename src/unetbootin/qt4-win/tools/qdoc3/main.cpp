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

/*
  main.cpp
*/

#include <QtCore>

#include <stdlib.h>

#include "apigenerator.h"
#include "ccodeparser.h"
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

static Tree *treeForLanguage(const QString &lang)
{
    Tree *tree = trees.value(lang);
    if ( tree == 0 ) {
	tree = new Tree;
	trees.insert( lang, tree );
    }
    return tree;
}

static void printHelp()
{
    Location::information( tr("Usage: qdoc [options] file1.qdocconf ...\n"
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

static void printVersion()
{
    Location::information( tr("qdoc version 3.0") );
}

static void processQdocconfFile(const QString &fileName)
{
    QList<QTranslator *> translators;

    Config config( tr("qdoc") );

    int i = 0;
    while (defaults[i].key) {
	config.setStringList(defaults[i].key, QStringList() << defaults[i].value);
	++i;
    }
    config.setStringList(CONFIG_SLOW, QStringList(slow ? "true" : "false"));

    Location::initialize( config );
    config.load( fileName );
    config.setStringList(CONFIG_DEFINES, defines + config.getStringList(CONFIG_DEFINES));

    Location::terminate();

    QString prevCurrentDir = QDir::currentPath();
    QString dir = QFileInfo( fileName ).path();
    if ( !dir.isEmpty() )
	QDir::setCurrent( dir );

    Location::initialize( config );
    Tokenizer::initialize( config );
    Doc::initialize( config );
    CppToQsConverter::initialize( config );
    CodeMarker::initialize( config );
    CodeParser::initialize( config );
    Generator::initialize( config );

    QStringList fileNames = config.getStringList( CONFIG_TRANSLATORS );
    QStringList::Iterator fn = fileNames.begin();
    while ( fn != fileNames.end() ) {
	QTranslator *translator = new QTranslator( 0 );
	if ( !translator->load(*fn) )
	    config.lastLocation().error( tr("Cannot load translator '%1'")
					 .arg(*fn) );
	QCoreApplication::instance()->installTranslator( translator );
	translators.append( translator );
	++fn;
    }

//    QSet<QString> outputLanguages = config.getStringSet(CONFIG_OUTPUTLANGUAGES);

    QString lang = config.getString(CONFIG_LANGUAGE);
    Location langLocation = config.lastLocation();

    Tree *tree = new Tree;
    tree->setVersion(config.getString(CONFIG_VERSION));
    CodeParser *codeParser = CodeParser::parserForLanguage( lang );
    if ( codeParser == 0 )
	config.lastLocation().fatal(tr("Cannot parse programming language '%1'").arg(lang));

    QSet<QString> outputFormats = config.getStringSet(CONFIG_OUTPUTFORMATS);
    Location outputFormatsLocation = config.lastLocation();

    CodeMarker *marker = CodeMarker::markerForLanguage(lang);
    if (!marker && !outputFormats.isEmpty())
	langLocation.fatal(tr("Cannot output documentation for programming language '%1'")
			   .arg(lang));

    QStringList indexFiles = config.getStringList(CONFIG_INDEXES);
    tree->readIndexes(indexFiles);

    QSet<QString> excludedDirs = config.getStringSet(CONFIG_EXCLUDEDIRS);

    QSet<QString> headers = QSet<QString>::fromList(
        config.getAllFiles(CONFIG_HEADERS, CONFIG_HEADERDIRS,
                           codeParser->headerFileNameFilter(),
                           excludedDirs));
    QSet<QString>::ConstIterator h = headers.begin();
    while ( h != headers.end() ) {
	codeParser->parseHeaderFile( config.location(), *h, tree );
	++h;
    }
    codeParser->doneParsingHeaderFiles( tree );

    QSet<QString> sources = QSet<QString>::fromList(
        config.getAllFiles(CONFIG_SOURCES, CONFIG_SOURCEDIRS,
                           codeParser->sourceFileNameFilter(),
                           excludedDirs));
    QSet<QString>::ConstIterator s = sources.begin();
    while ( s != sources.end() ) {
	codeParser->parseSourceFile( config.location(), *s, tree );
	++s;
    }
    codeParser->doneParsingSourceFiles( tree );
    tree->resolveGroups();
    tree->resolveTargets();

    QSet<QString>::ConstIterator of = outputFormats.begin();
    while ( of != outputFormats.end() ) {
	Generator *generator = Generator::generatorForFormat( *of );
	if ( generator == 0 )
	    outputFormatsLocation.fatal(tr("Unknown output format '%1'").arg(*of));
	generator->generateTree( tree, marker );
	++of;
    }

    QString tagFile = config.getString(CONFIG_TAGFILE);
    if (!tagFile.isEmpty())
        tree->generateTagFile(tagFile);

    tree->setVersion("");

    Generator::terminate();
    CodeParser::terminate();
    CodeMarker::terminate();
    CppToQsConverter::terminate();
    Doc::terminate();
    Tokenizer::terminate();
    Location::terminate();
    QDir::setCurrent( prevCurrentDir );

    foreach (QTranslator *translator, translators)
	delete translator;

    delete tree;
}

QT_END_NAMESPACE

int main( int argc, char **argv )
{
    QT_USE_NAMESPACE

    QCoreApplication app(argc, argv);

    PolyArchiveExtractor qsaExtractor( QStringList() << "qsa",
				       "qsauncompress \1 \2" );
    PolyArchiveExtractor tarExtractor( QStringList() << "tar",
				       "tar -C \2 -xf \1" );
    PolyArchiveExtractor tazExtractor( QStringList() << "taz",
				       "tar -C \2 -Zxf \1" );
    PolyArchiveExtractor tbz2Extractor( QStringList() << "tbz" << "tbz2",
					"tar -C \2 -jxf \1" );
    PolyArchiveExtractor tgzExtractor( QStringList() << "tgz",
				       "tar -C \2 -zxf \1" );
    PolyArchiveExtractor zipExtractor( QStringList() << "zip",
				       "unzip \1 -d \2" );

    PolyUncompressor bz2Uncompressor( QStringList() << "bz" << "bz2",
				      "bunzip2 -c \1 > \2" );
    PolyUncompressor gzAndZUncompressor( QStringList() << "gz" << "z" << "Z",
					 "gunzip -c \1 > \2" );
    PolyUncompressor zipUncompressor( QStringList() << "zip",
				      "unzip -c \1 > \2" );

    CCodeParser cParser;
    CppCodeParser cppParser;

    Tree *cppTree = treeForLanguage( cppParser.language() );

    QsCodeParser qsParser( cppTree );
    QsaKernelParser qsaKernelParser( cppTree );
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

    while ( i < argc ) {
	opt = argv[i++];

	if ( opt == "-help" ) {
	    printHelp();
	    return EXIT_SUCCESS;
	} else if ( opt == "-version" ) {
	    printVersion();
	    return EXIT_SUCCESS;
	} else if ( opt == "--" ) {
	    while ( i < argc )
		qdocFiles.append( argv[i++] );
        } else if ( opt.startsWith("-D") ) {
            QString define = opt.mid(2);
            defines += define;
        } else if (opt == "-slow") {
            slow = true;
	} else {
	    qdocFiles.append( opt );
	}
    }

    if ( qdocFiles.isEmpty() ) {
	printHelp();
	return EXIT_FAILURE;
    }

    foreach (QString qf, qdocFiles)
	processQdocconfFile( qf );

    qDeleteAll(trees);
    return EXIT_SUCCESS;
}

