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
  config.cpp
*/

#include <QtCore>

#include "archiveextractor.h"
#include "config.h"
#include "uncompressor.h"
#include <stdlib.h>

QT_BEGIN_NAMESPACE

/*
  ###
*/
class MetaStackEntry
{
public:
    void open();
    void close();

    QStringList accum;
    QStringList next;
};

void MetaStackEntry::open()
{
    next.append(QString());
}

void MetaStackEntry::close()
{
    accum += next;
    next.clear();
}

/*
  ###
*/
class MetaStack : private QStack<MetaStackEntry>
{
public:
    MetaStack();

    void process( QChar ch, const Location& location );
    QStringList getExpanded( const Location& location );
};

MetaStack::MetaStack()
{
    push( MetaStackEntry() );
    top().open();
}

void MetaStack::process( QChar ch, const Location& location )
{
    if ( ch == QLatin1Char('{') ) {
        push( MetaStackEntry() );
        top().open();
    } else if ( ch == QLatin1Char('}') ) {
        if ( count() == 1 )
            location.fatal( tr("Unexpected '}'") );

        top().close();
        QStringList suffixes = pop().accum;
        QStringList prefixes = top().next;

        top().next.clear();
        QStringList::ConstIterator pre = prefixes.begin();
        while ( pre != prefixes.end() ) {
                QStringList::ConstIterator suf = suffixes.begin();
            while ( suf != suffixes.end() ) {
            top().next << ( *pre + *suf );
            ++suf;
            }
            ++pre;
        }
    } else if ( ch == QLatin1Char(',') && count() > 1 ) {
        top().close();
        top().open();
    } else {
        QStringList::Iterator pre = top().next.begin();
        while ( pre != top().next.end() ) {
            *pre += ch;
            ++pre;
        }
    }
}

QStringList MetaStack::getExpanded( const Location& location )
{
    if ( count() > 1 )
        location.fatal( tr("Missing '}'") );

    top().close();
    return top().accum;
}

QT_STATIC_CONST_IMPL QString Config::dot = QLatin1String(".");
QMap<QString, QString> Config::uncompressedFiles;
QMap<QString, QString> Config::extractedDirs;
int Config::numInstances;

/*!

*/
Config::Config( const QString& programName )
    : prog( programName )
{
    loc = Location::null;
    lastLoc = Location::null;
    locMap.clear();
    stringValueMap.clear();
    stringListValueMap.clear();
    numInstances++;
}

Config::~Config()
{
    if ( --numInstances == 0 ) {
	QMap<QString, QString>::ConstIterator f = uncompressedFiles.begin();
	while ( f != uncompressedFiles.end() ) {
	    QDir().remove( *f );
	    ++f;
	}
	uncompressedFiles.clear();

	QMap<QString, QString>::ConstIterator d = extractedDirs.begin();
	while ( d != extractedDirs.end() ) {
	    removeDirContents( *d );
	    QDir dir( *d );
	    QString name = dir.dirName();
	    dir.cdUp();
	    dir.rmdir( name );
	    ++d;
	}
	extractedDirs.clear();
    }
}

/*!

*/
void Config::load( const QString& fileName )
{
    load( Location::null, fileName );
    if ( loc.isEmpty() ) {
	loc = Location( fileName );
    } else {
	loc.setEtc( true );
    }
    lastLoc = Location::null;
}

/*!

*/
void Config::setStringList( const QString& var, const QStringList& values )
{
    stringValueMap[var] = values.join( QLatin1String(" ") );
    stringListValueMap[var] = values;
}

/*!
*/
bool Config::getBool( const QString& var ) const
{
    return QVariant(getString(var)).toBool();
}

/*!
  Returns the value of the \a var configuration variable as an
  integer.
*/
int Config::getInt( const QString& var ) const
{
    QStringList strs = getStringList( var );
    QStringList::ConstIterator s = strs.begin();
    int sum = 0;

    while ( s != strs.end() ) {
	sum += (*s).toInt();
	++s;
    }
    return sum;
}

/*!
  Returns the value of the \a var configuration variable as a string.
*/
QString Config::getString( const QString& var ) const
{
    if ( !locMap[var].isEmpty() )
	(Location&) lastLoc = locMap[var];
    return stringValueMap[var];
}

QSet<QString> Config::getStringSet( const QString& var ) const
{
    return QSet<QString>::fromList(getStringList(var));
}

QStringList Config::getStringList( const QString& var ) const
{
    if ( !locMap[var].isEmpty() )
	(Location&) lastLoc = locMap[var];
    return stringListValueMap[var];
}

QRegExp Config::getRegExp( const QString& var ) const
{
    QString pattern;
    QList<QRegExp> subRegExps = getRegExpList( var );
    QList<QRegExp>::ConstIterator s = subRegExps.begin();

    while ( s != subRegExps.end() ) {
        if ( !(*s).isValid() )
            return *s;
        if ( !pattern.isEmpty() )
            pattern += QLatin1Char('|');
        pattern += QLatin1String("(?:") + (*s).pattern() + QLatin1Char(')');
        ++s;
    }
    if ( pattern.isEmpty() )
        pattern = QLatin1String("$x"); // cannot match
    return QRegExp( pattern );
}

QList<QRegExp> Config::getRegExpList( const QString& var ) const
{
    QStringList strs = getStringList( var );
    QStringList::ConstIterator s = strs.begin();
    QList<QRegExp> regExps;

    while ( s != strs.end() ) {
	regExps += QRegExp( *s );
	++s;
    }
    return regExps;
}

/*!
  This function is slower than it could be.
*/
QSet<QString> Config::subVars( const QString& var ) const
{
    QSet<QString> result;
    QString varDot = var + QLatin1Char('.');
    QMap<QString, QString>::ConstIterator v = stringValueMap.begin();
    while ( v != stringValueMap.end() ) {
        if ( v.key().startsWith(varDot) ) {
            QString subVar = v.key().mid( varDot.length() );
            int dot = subVar.indexOf( QLatin1Char('.') );
            if ( dot != -1 )
                subVar.truncate( dot );
            result.insert( subVar );
        }
        ++v;
    }
    return result;
}

QStringList Config::getAllFiles(const QString &filesVar, const QString &dirsVar,
				const QString &defaultNameFilter,
                                const QSet<QString> &excludedDirs)
{
    QStringList result = getStringList(filesVar);
    QStringList dirs = getStringList(dirsVar);

    QString nameFilter = getString(filesVar + dot +
        QLatin1String(CONFIG_FILEEXTENSIONS));
    if (nameFilter.isEmpty())
        nameFilter = defaultNameFilter;

    QStringList::ConstIterator d = dirs.begin();
    while (d != dirs.end()) {
	result += getFilesHere(*d, nameFilter, excludedDirs);
	++d;
    }
    return result;
}

QString Config::findFile( const Location& location, const QStringList& files,
			  const QStringList& dirs, const QString& fileName,
			  QString& userFriendlyFilePath )
{
    if ( fileName.isEmpty() || fileName.startsWith(QLatin1Char('/')) ) {
        userFriendlyFilePath = fileName;
        return fileName;
    }

    QFileInfo fileInfo;
    QStringList components = fileName.split(QLatin1Char('?'));
    QString firstComponent = components.first();

    QStringList::ConstIterator f = files.begin();
    while ( f != files.end() ) {
	if ( *f == firstComponent || (*f).endsWith(QLatin1Char('/') + firstComponent) ) {
	    fileInfo.setFile( *f );
	    if ( !fileInfo.exists() )
		location.fatal( tr("File '%1' does not exist").arg(*f) );
	    break;
	}
	++f;
    }

    if ( fileInfo.fileName().isEmpty() ) {
	QStringList::ConstIterator d = dirs.begin();
	while ( d != dirs.end() ) {
	    fileInfo.setFile( QDir(*d), firstComponent );
	    if ( fileInfo.exists() )
		break;
	    ++d;
	}
    }

    userFriendlyFilePath = QString();
    if ( !fileInfo.exists() )
	    return QString();

    QStringList::ConstIterator c = components.begin();
    for ( ;; ) {
	bool isArchive = ( c != components.end() - 1 );
	ArchiveExtractor *extractor = 0;
	QString userFriendly = *c;

	if ( isArchive )
	    extractor = ArchiveExtractor::extractorForFileName( userFriendly );

	if ( extractor == 0 ) {
	    Uncompressor *uncompressor =
		    Uncompressor::uncompressorForFileName( userFriendly );
	    if ( uncompressor != 0 ) {
		QString fileNameWithCorrectExtension =
			uncompressor->uncompressedFilePath(
				fileInfo.filePath() );
		QString uncompressed = uncompressedFiles[fileInfo.filePath()];
		if ( uncompressed.isEmpty() ) {
		    uncompressed = QTemporaryFile(fileInfo.filePath()).fileName();
		    uncompressor->uncompressFile( location,
						  fileInfo.filePath(),
						  uncompressed );
		    uncompressedFiles[fileInfo.filePath()] = uncompressed;
		}
		fileInfo.setFile( uncompressed );

		if ( isArchive ) {
		    extractor = ArchiveExtractor::extractorForFileName(
					fileNameWithCorrectExtension );
		} else {
		    userFriendly = fileNameWithCorrectExtension;
		}
	    }
	}
	userFriendlyFilePath += userFriendly;

	if ( isArchive ) {
	    if ( extractor == 0 )
		location.fatal( tr("Unknown archive type '%1'")
				.arg(userFriendlyFilePath) );
	    QString extracted = extractedDirs[fileInfo.filePath()];
	    if ( extracted.isEmpty() ) {
		extracted = QTemporaryFile(fileInfo.filePath()).fileName();
		if ( !QDir().mkdir(extracted) )
		    location.fatal( tr("Cannot create temporary directory '%1'")
				    .arg(extracted) );
		extractor->extractArchive( location, fileInfo.filePath(),
					   extracted );
		extractedDirs[fileInfo.filePath()] = extracted;
	    }
	    ++c;
	    fileInfo.setFile( QDir(extracted), *c );
	} else {
	    break;
	}
	userFriendlyFilePath += "?";
    }
    return fileInfo.filePath();
}

QString Config::findFile( const Location& location, const QStringList& files,
			  const QStringList& dirs, const QString& fileBase,
			  const QStringList& fileExtensions,
			  QString& userFriendlyFilePath )
{
    QStringList::ConstIterator e = fileExtensions.begin();
    while ( e != fileExtensions.end() ) {
	QString filePath = findFile( location, files, dirs, fileBase + "." + *e,
				     userFriendlyFilePath );
	if ( !filePath.isEmpty() )
	    return filePath;
	++e;
    }
    return findFile( location, files, dirs, fileBase, userFriendlyFilePath );
}

QString Config::copyFile( const Location& location,
			  const QString& sourceFilePath,
			  const QString& userFriendlySourceFilePath,
			  const QString& targetDirPath )
{
    QFile inFile( sourceFilePath );
    if ( !inFile.open(QFile::ReadOnly) ) {
	location.fatal( tr("Cannot open input file '%1': %2")
			.arg(inFile.fileName()).arg(inFile.errorString()) );
	return "";
    }

    QString outFileName = userFriendlySourceFilePath;
    int slash = outFileName.lastIndexOf( "/" );
    if ( slash != -1 )
	outFileName = outFileName.mid( slash );

    QFile outFile( targetDirPath + "/" + outFileName );
    if ( !outFile.open(QFile::WriteOnly) ) {
	location.fatal( tr("Cannot open output file '%1': %2")
			.arg(outFile.fileName()).arg(outFile.errorString()) );
	return "";
    }

    char buffer[1024];
    int len;
    while ( (len = inFile.read(buffer, sizeof(buffer))) > 0 ) {
	outFile.write( buffer, len );
    }
    return outFileName;
}

int Config::numParams( const QString& value )
{
    int max = 0;
    for ( int i = 0; i != value.length(); i++ ) {
        uint c = value[i].unicode();
        if ( c > 0 && c < 8 )
            max = qMax( max, (int)c );
    }
    return max;
}

bool Config::removeDirContents( const QString& dir )
{
    QDir dirInfo( dir );
    QFileInfoList entries = dirInfo.entryInfoList();

    bool ok = true;

    QFileInfoList::Iterator it = entries.begin();
    while ( it != entries.end() ) {
	if ( (*it).isFile() ) {
	    if ( !dirInfo.remove((*it).fileName()) )
		ok = false;
	} else if ( (*it).isDir() ) {
	    if ( (*it).fileName() != "." && (*it).fileName() != ".." ) {
		if ( removeDirContents((*it).absoluteFilePath()) ) {
		    if ( !dirInfo.rmdir((*it).fileName()) )
			ok = false;
		} else {
		    ok = false;
		}
	    }
	}
	++it;
    }
    return ok;
}

bool Config::isMetaKeyChar( QChar ch )
{
    return ch.isLetterOrNumber()
        || ch == QLatin1Char('_')
        || ch == QLatin1Char('.')
        || ch == QLatin1Char('{')
        || ch == QLatin1Char('}')
        || ch == QLatin1Char(',');
}

void Config::load( Location location, const QString& fileName )
{
    QRegExp keySyntax( "\\w+(?:\\.\\w+)*" );

#define SKIP_CHAR() \
    do { \
        location.advance( c ); \
        ++i; \
        c = text.at(i); \
        cc = c.unicode(); \
    } while (0)

#define SKIP_SPACES() \
    while ( c.isSpace() && cc != '\n' ) \
        SKIP_CHAR()

#define PUT_CHAR() \
    word += c; \
    SKIP_CHAR();

    if ( location.depth() > 16 )
        location.fatal( tr("Too many nested includes") );

    QFile fin( fileName );
    if ( !fin.open(QFile::ReadOnly | QFile::Text) ) {
        fin.setFileName(fileName + ".qdoc");
        if (!fin.open(QFile::ReadOnly | QFile::Text))
            location.fatal( tr("Cannot open file '%1': %2").arg(fileName).arg(fin.errorString()) );
    }

    QString text = fin.readAll();
    text += QLatin1String("\n\n");
    text += QChar('\0');
    fin.close();

    location.push( fileName );
    location.start();

    int i = 0;
    QChar c = text.at(0);
    uint cc = c.unicode();
    while ( i < (int) text.length() ) {
        if ( cc == 0 )
            ++i;
        else if ( c.isSpace() ) {
            SKIP_CHAR();
        } else if ( cc == '#' ) {
            do {
                SKIP_CHAR();
            } while ( cc != '\n' );
        } else if ( isMetaKeyChar(c) ) {
            Location keyLoc = location;
            bool plus = false;
            QString stringValue;
            QStringList stringListValue;
            QString word;
            bool inQuote = false;
            bool prevWordQuoted = true;
            bool metWord = false;

            MetaStack stack;
            do {
                stack.process( c, location );
                SKIP_CHAR();
            } while ( isMetaKeyChar(c) );

            QStringList keys = stack.getExpanded( location );
            SKIP_SPACES();

            if ( keys.count() == 1 && keys.first() == "include" ) {
                QString includeFile;

                if ( cc != '(' )
                    location.fatal( tr("Bad include syntax") );
                SKIP_CHAR();
                SKIP_SPACES();
                while ( !c.isSpace() && cc != '#' && cc != ')' ) {
                    includeFile += c;
                    SKIP_CHAR();
                }
                SKIP_SPACES();
                if ( cc != ')' )
                    location.fatal( tr("Bad include syntax") );
                SKIP_CHAR();
                SKIP_SPACES();
                if ( cc != '#' && cc != '\n' )
                    location.fatal( tr("Trailing garbage") );

                load( location,
                      QFileInfo(QFileInfo(fileName).dir(), includeFile)
                      .filePath() );
            } else {
                if ( cc == '+' ) {
                    plus = true;
                    SKIP_CHAR();
                }
                if ( cc != '=' )
                    location.fatal(tr("Expected '=' or '+=' after key"));
                SKIP_CHAR();
                SKIP_SPACES();

                for ( ;; ) {
                    if ( cc == '\\' ) {
                        int metaCharPos;

                        SKIP_CHAR();
                        if ( cc == '\n' ) {
                            SKIP_CHAR();
                        } else if ( cc > '0' && cc < '8' ) {
                            word += QChar( c.digitValue() );
                            SKIP_CHAR();
                        } else if ( (metaCharPos = QString(QLatin1String("abfnrtv")).indexOf(c)) != -1 ) {
                            word += "\a\b\f\n\r\t\v"[metaCharPos];
                            SKIP_CHAR();
                        } else {
                            PUT_CHAR();
                        }
                    } else if ( c.isSpace() || cc == '#' ) {
                        if ( inQuote ) {
                            if ( cc == '\n' )
                                location.fatal( tr("Unterminated string") );
                            PUT_CHAR();
                        } else {
                            if ( !word.isEmpty() ) {
                                if ( metWord )
                                    stringValue += QLatin1Char(' ');
                                stringValue += word;
                                stringListValue << word;
                                metWord = true;
                                word.clear();
                                prevWordQuoted = false;
                            }
                            if ( cc == '\n' || cc == '#' )
                                break;
                            SKIP_SPACES();
                        }
                    } else if ( cc == '"' ) {
                        if ( inQuote ) {
                            if ( !prevWordQuoted )
                                stringValue += QLatin1Char(' ');
                            stringValue += word;
                            if ( !word.isEmpty() )
                                stringListValue << word;
                            metWord = true;
                            word.clear();
                            prevWordQuoted = true;
                        }
                        inQuote = !inQuote;
                        SKIP_CHAR();
                    } else if ( cc == '$' ) {
                        QString var;
                        SKIP_CHAR();
                        while ( c.isLetterOrNumber() || cc == '_' ) {
                            var += c;
                            SKIP_CHAR();
                        }
                        if ( !var.isEmpty() ) {
                            char *val = getenv( var.toLatin1().data() );
                            if ( val == 0 ) {
                                location.fatal(tr("Environment variable '%1' undefined").arg(var));
                            } else {
                                word += QString( val );
                            }
                        }
                    } else {
                        if ( !inQuote && cc == '=' )
                            location.fatal( tr("Unexpected '='") );
                        PUT_CHAR();
                    }
                }

                QStringList::ConstIterator key = keys.begin();
                while ( key != keys.end() ) {
                    if ( !keySyntax.exactMatch(*key) )
                        keyLoc.fatal( tr("Invalid key '%1'").arg(*key) );

                    if ( plus ) {
                        if ( locMap[*key].isEmpty() ) {
                            locMap[*key] = keyLoc;
                        } else {
                            locMap[*key].setEtc( true );
                        }
                        if ( stringValueMap[*key].isEmpty() ) {
                            stringValueMap[*key] = stringValue;
                        } else {
                            stringValueMap[*key] += QLatin1Char(' ') + stringValue;
                        }
                        stringListValueMap[*key] += stringListValue;
                    } else {
                        locMap[*key] = keyLoc;
                        stringValueMap[*key] = stringValue;
                        stringListValueMap[*key] = stringListValue;
                    }
                    ++key;
                }
            }
        } else {
            location.fatal( tr("Unexpected character '%1' at beginning of line")
                            .arg(c) );
        }
    }
}

QStringList Config::getFilesHere(const QString& dir, const QString& nameFilter,
                                 const QSet<QString> &excludedDirs)
{
    QStringList result;
    if (excludedDirs.contains(dir))
        return result;

    QDir dirInfo( dir );
    QStringList fileNames;
    QStringList::const_iterator fn;

    dirInfo.setNameFilters(nameFilter.split(' '));
    dirInfo.setSorting( QDir::Name );
    dirInfo.setFilter( QDir::Files );
    fileNames = dirInfo.entryList();
    fn = fileNames.constBegin();
    while ( fn != fileNames.constEnd() ) {
        if (!fn->startsWith(QLatin1Char('~')))
            result.append(dirInfo.filePath(*fn));
	++fn;
    }    
    
    dirInfo.setNameFilters(QStringList("*"));
    dirInfo.setFilter(QDir::Dirs|QDir::NoDotAndDotDot);
    fileNames = dirInfo.entryList();
    fn = fileNames.constBegin();
    while ( fn != fileNames.constEnd() ) {
        result += getFilesHere( dirInfo.filePath(*fn), nameFilter, excludedDirs );
	++fn;
    }
    return result;
}

QT_END_NAMESPACE
