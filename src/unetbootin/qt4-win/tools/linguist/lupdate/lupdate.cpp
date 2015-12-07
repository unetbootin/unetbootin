/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
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

#include "lupdate.h"

#include <proparser.h>

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QCoreApplication>

#include <errno.h>
#include <string.h>
#include <QtCore/QDebug>

QT_BEGIN_NAMESPACE

typedef QList<TranslatorMessage> TML;

LupdateApplication::LupdateApplication(int &argc, char **argv)
  : QCoreApplication(argc, argv),
    m_defaultExtensions(QLatin1String("ui,c,c++,cc,cpp,cxx,ch,h,h++,hh,hpp,hxx"))
{}

void LupdateApplication::recursiveFileInfoList(const QDir &dir,
    const QStringList &nameFilters, QDir::Filters filter, bool recursive,
    QFileInfoList *fileinfolist)
{
    if (recursive)
        filter |= QDir::AllDirs;
    QFileInfoList entries = dir.entryInfoList(nameFilters, filter);

    QFileInfoList::iterator it;
    for (it = entries.begin(); it != entries.end(); ++it) {
        QString fname = it->fileName();
        if (fname != QLatin1String(".") && fname != QLatin1String("..")) {
            if (it->isDir()) {
                recursiveFileInfoList(QDir(it->absoluteFilePath()), nameFilters, filter, recursive, fileinfolist);
            } else {
                fileinfolist->append(*it);
            }
        }
    }
}

void LupdateApplication::printUsage()
{
    Console::out( tr("Usage:\n"
             "    lupdate [options] [project-file]\n"
             "    lupdate [options] [source-file|path]... -ts ts-files\n"
             "Options:\n"
             "    -help  Display this information and exit.\n"
             "    -noobsolete\n"
             "           Drop all obsolete strings.\n"
             "    -extensions <ext>[,<ext>]...\n"
             "           Process files with the given extensions only.\n"
             "           The extension list must be separated with commas, not with whitespace.\n"
             "           Default: '%1'.\n"
             "    -pluralonly\n"
             "           Only include plural form messages.\n"
             "    -silent\n"
             "           Don't explain what is being done.\n"
             "    -version\n"
             "           Display the version of lupdate and exit.\n").arg(m_defaultExtensions));
}

void LupdateApplication::updateTsFiles( const MetaTranslator& fetchedTor,
                           const QStringList& tsFileNames, const QString& codecForTr,
                           bool noObsolete, bool onlyPlural, bool verbose )
{
    QStringList::ConstIterator t = tsFileNames.begin();
    QDir dir;
    while ( t != tsFileNames.end() ) {
        QString fn = dir.relativeFilePath(*t);
        MetaTranslator tor;
        MetaTranslator out;
        tor.load( *t );
        tor.makeFileNamesAbsolute();
        if ( !codecForTr.isEmpty() ) {
            out.setCodec( codecForTr.toLatin1() );
            tor.setCodec( codecForTr.toLatin1() );
        }
        if ( verbose )
            Console::out(tr("Updating '%1'...\n").arg(fn));

        merge( &tor, &fetchedTor, &out, verbose, noObsolete );
        if ( onlyPlural) {
            Console::out(tr("Stripping non plural forms in '%1'...\n").arg(fn));
            out.stripNonPluralForms();
        }
        if ( noObsolete )
            out.stripObsoleteMessages();
        out.stripEmptyContexts();

            if ( !out.save(*t) ) {
#if defined(_MSC_VER) && _MSC_VER >= 1400
                char buf[100];
                strerror_s(buf, sizeof(buf), errno);
                qWarning("lupdate error: Cannot save '%s': %s\n",
                     qPrintable(fn), buf );
#else
                qWarning("lupdate error: Cannot save '%s': %s\n",
                     qPrintable(fn), strerror(errno) );
#endif
            }
        ++t;
    }
}


int LupdateApplication::start()
{
    QStringList argv = arguments();
    int argc = argv.count();
    QString defaultContext = QLatin1String("@default");
    MetaTranslator fetchedTor;
    QByteArray codecForTr;
    QByteArray codecForSource;
    QStringList tsFileNames;
    QStringList proFiles;
    QStringList sourceFiles;

    bool verbose = true; // verbose is on by default starting with Qt 4.2
    bool noObsolete = false;
    bool onlyPlural = false;
    int numFiles = 0;
    bool standardSyntax = true;
    bool metTsFlag = false;

    QString extensions = m_defaultExtensions;
    QStringList extensionsNameFilters;
    int i;

    for ( i = 1; i < argc; i++ ) {
        if ( argv.at(i) == QLatin1String("-ts") )
            standardSyntax = false;
    }

    for ( i = 1; i < argc; i++ ) {
        QString arg = argv.at(i);
        if ( arg == QLatin1String("-help")
                || arg == QLatin1String("--help")
                || arg == QLatin1String("-h")) {
            printUsage();
            return 0;
        } else if ( arg == QLatin1String("-pluralonly") ) {
            onlyPlural = true;
            continue;
        } else if ( arg == QLatin1String("-noobsolete") ) {
            noObsolete = true;
            continue;
        } else if ( arg == QLatin1String("-silent") ) {
            verbose = false;
            continue;
        } else if ( arg == QLatin1String("-verbose") ) {
            verbose = true;
            continue;
        } else if ( arg == QLatin1String("-version") ) {
            Console::out(tr("lupdate version %1\n").arg(QLatin1String(QT_VERSION_STR)) );
            return 0;
        } else if ( arg == QLatin1String("-ts") ) {
            metTsFlag = true;
            continue;
        } else if ( arg == QLatin1String("-extensions") ) {
            ++i;
            if (i == argc) {
                qWarning("The -extensions option should be followed by an extension list.");
                return 1;
            }
            extensions = arg;
            continue;
        }

        numFiles++;

        QString fullText;

        if ( standardSyntax && !metTsFlag ) {
            QFile f( arg );
            if ( !f.open(QIODevice::ReadOnly) ) {
#if defined(_MSC_VER) && _MSC_VER >= 1400
                char buf[100];
                strerror_s(buf, sizeof(buf), errno);
                qWarning("lupdate error: Cannot open file '%s': %s\n",
                         qPrintable(arg), buf );
#else
                qWarning("lupdate error: Cannot open file '%s': %s\n",
                         qPrintable(arg), strerror(errno) );
#endif
                return 1;
            }
            f.close();
        }

        codecForTr.clear();
		codecForSource.clear();

        if (metTsFlag) {
            if ( arg.endsWith(QLatin1String(".ts"), Qt::CaseInsensitive)
                || arg.endsWith(QLatin1String(".xlf"), Qt::CaseInsensitive)) {
                QFileInfo fi( arg );
                if ( !fi.exists() || fi.isWritable() ) {
                    tsFileNames.append( QFileInfo(arg).absoluteFilePath() );
                } else {
                    qWarning("lupdate warning: For some reason, I cannot save '%s'\n",
                             qPrintable(arg) );
                }
            } else {
                qWarning("lupdate error: File '%s' lacks .ts or .xlf extension\n",
                         qPrintable(arg) );
            }
        } else if (arg.endsWith(QLatin1String(".pro"), Qt::CaseInsensitive)) {
            proFiles << arg;
        } else {
            QFileInfo fi(arg);
            if (fi.isDir()) {
                if ( verbose )
                    Console::out(tr("Scanning directory '%1'...\n").arg(arg));
                QDir dir = QDir(fi.filePath());
                if (extensionsNameFilters.isEmpty()) {
                    extensions = extensions.trimmed();
                    // Remove the potential dot in front of each extension
                    if (extensions.startsWith(QLatin1Char('.')))
                        extensions.remove(0,1);
                    extensions.replace(QLatin1String(",."), QLatin1String(","));

                    extensions.insert(0, QLatin1String("*."));
                    extensions.replace(QLatin1Char(','), QLatin1String(",*."));
                    extensionsNameFilters = extensions.split(QLatin1Char(','));
                }
                QDir::Filters filters = QDir::Files | QDir::NoSymLinks;
                QFileInfoList fileinfolist;
                recursiveFileInfoList(dir, extensionsNameFilters, filters, true, &fileinfolist);
                QFileInfoList::iterator ii;
                QString fn;
                for (ii = fileinfolist.begin(); ii != fileinfolist.end(); ++ii) {
                    // Make sure the path separator is stored with '/' in the ts file
                    fn = ii->canonicalFilePath().replace(QLatin1Char('\\'),QLatin1Char('/'));
#ifdef LINGUIST_DEBUG
                    qDebug() << fn;
#endif
                    sourceFiles << fn;
                }
            }else{
                sourceFiles << fi.canonicalFilePath().replace(QLatin1Char('\\'),QLatin1Char('/'));
            }
        }
    }   //for


    if ( proFiles.count() > 0 ) {
        proFiles = getListOfProfiles(proFiles, verbose);
    }
    bool firstPass = true;
    for (int pi = 0; firstPass || pi < proFiles.count(); ++pi) {
        QStringList tsFiles = tsFileNames;
        if (proFiles.count() > 0) {
            QString pf = proFiles.at(pi);
            QMap<QByteArray, QStringList> variables;

            if(!evaluateProFile(pf, verbose, &variables))
                return 2;

            sourceFiles = variables.value("SOURCES");

            QStringList tmp = variables.value("CODECFORTR");
            if (!tmp.isEmpty()) {
                codecForTr = tmp.first().toAscii();
                fetchedTor.setCodecForTr(codecForTr.constData());
            }
            tmp = variables.value("CODECFORSRC");
            if (!tmp.isEmpty()) {
                codecForSource = tmp.first().toAscii();
            }

            tsFiles += variables.value("TRANSLATIONS");
        }

        for (QStringList::iterator it = sourceFiles.begin(); it != sourceFiles.end(); ++it) {
#ifdef LINGUIST_DEBUG
            qDebug() << "  " << (*it);
#endif
            if ( (*it).endsWith(QLatin1String(".java"), Qt::CaseInsensitive) ) {
                fetchtr_java( *it, &fetchedTor, defaultContext, true, codecForSource );
            }
            else if ( (*it).endsWith(QLatin1String(".ui"), Qt::CaseInsensitive) ) {
#ifdef LINGUIST_DEBUG
                qDebug() << "  " << (*it) + ".h";
#endif
                fetchtr_ui( *it, &fetchedTor, defaultContext, true );
                fetchtr_cpp( *it + QLatin1String(".h"), &fetchedTor,
                             defaultContext, false, codecForSource );
            } else {
                fetchtr_cpp( *it, &fetchedTor, defaultContext, true, codecForSource );
            }
        }

        removeDuplicates(&tsFiles, false);

        if ( tsFiles.count() > 0) {
            updateTsFiles( fetchedTor, tsFiles, QString::fromLatin1(codecForTr.constData()), noObsolete, onlyPlural, verbose );
        }
        firstPass = false;
    }

    if ( numFiles == 0 ) {
        printUsage();
        return 1;
    }
    return 0;
}

QT_END_NAMESPACE
