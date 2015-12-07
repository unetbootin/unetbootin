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
  config.cpp
*/

#include <QtCore>

#include "archiveextractor.h"
#include "config.h"
#include "uncompressor.h"
#include <stdlib.h>

QT_BEGIN_NAMESPACE

/*
  An entry on the MetaStack.
 */
class MetaStackEntry
{
public:
    void open();
    void close();

    QStringList accum;
    QStringList next;
};

/*
 */
void MetaStackEntry::open()
{
    next.append(QString());
}

/*
 */
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

    void process(QChar ch, const Location& location);
    QStringList getExpanded(const Location& location);
};

MetaStack::MetaStack()
{
    push(MetaStackEntry());
    top().open();
}

void MetaStack::process(QChar ch, const Location& location)
{
    if (ch == QLatin1Char('{')) {
        push(MetaStackEntry());
        top().open();
    }
    else if (ch == QLatin1Char('}')) {
        if (count() == 1)
            location.fatal(tr("Unexpected '}'"));

        top().close();
        QStringList suffixes = pop().accum;
        QStringList prefixes = top().next;

        top().next.clear();
        QStringList::ConstIterator pre = prefixes.begin();
        while (pre != prefixes.end()) {
                QStringList::ConstIterator suf = suffixes.begin();
            while (suf != suffixes.end()) {
            top().next << (*pre + *suf);
            ++suf;
            }
            ++pre;
        }
    }
    else if (ch == QLatin1Char(',') && count() > 1) {
        top().close();
        top().open();
    }
    else {
        QStringList::Iterator pre = top().next.begin();
        while (pre != top().next.end()) {
            *pre += ch;
            ++pre;
        }
    }
}

QStringList MetaStack::getExpanded(const Location& location)
{
    if (count() > 1)
        location.fatal(tr("Missing '}'"));

    top().close();
    return top().accum;
}

QT_STATIC_CONST_IMPL QString Config::dot = QLatin1String(".");
QMap<QString, QString> Config::uncompressedFiles;
QMap<QString, QString> Config::extractedDirs;
int Config::numInstances;

/*!
  \class Config
  \brief The Config class contains the configuration variables
  for controlling how qdoc produces documentation.

  Its load() function, reads, parses, and processes qdocconf files.
 */

/*!
  The constructor sets the \a programName and initializes all
  internal state to empty values.
 */
Config::Config(const QString& programName)
    : prog(programName)
{
    loc = Location::null;
    lastLoc = Location::null;
    locMap.clear();
    stringValueMap.clear();
    stringListValueMap.clear();
    numInstances++;
}

/*!
  The destructor deletes all the temporary files and
  directories it built.
 */
Config::~Config()
{
    if (--numInstances == 0) {
	QMap<QString, QString>::ConstIterator f = uncompressedFiles.begin();
	while (f != uncompressedFiles.end()) {
	    QDir().remove(*f);
	    ++f;
	}
	uncompressedFiles.clear();

	QMap<QString, QString>::ConstIterator d = extractedDirs.begin();
	while (d != extractedDirs.end()) {
	    removeDirContents(*d);
	    QDir dir(*d);
	    QString name = dir.dirName();
	    dir.cdUp();
	    dir.rmdir(name);
	    ++d;
	}
	extractedDirs.clear();
    }
}

/*!
  Loads and parses the qdoc configuration file \a fileName.
  It just calls the other load() function, which is where all
  the loading, parsing, and processing of the the configuration
  files is done. 
 */
void Config::load(const QString& fileName)
{
    load(Location::null, fileName);
    if (loc.isEmpty()) {
	loc = Location(fileName);
    }
    else {
	loc.setEtc(true);
    }
    lastLoc = Location::null;
}

/*!
  Joins all the strings in \a values into a single string with
  the concatenated string separated by ' '. Then insert the
  single string into the string list map with \a var as the
  key.
 */
void Config::setStringList(const QString& var, const QStringList& values)
{
    stringValueMap[var] = values.join(QLatin1String(" "));
    stringListValueMap[var] = values;
}

/*!
  Looks up the configuarion variable \a var in the string
  map and returns the boolean value.
 */
bool Config::getBool(const QString& var) const
{
    return QVariant(getString(var)).toBool();
}

/*!
  Looks up the configuration variable \a var in the string list
  map. Iterates through the string list found, interpreting each
  string in the list as an integer and adding it to a total sum.
  Returns the sum.
 */
int Config::getInt(const QString& var) const
{
    QStringList strs = getStringList(var);
    QStringList::ConstIterator s = strs.begin();
    int sum = 0;

    while (s != strs.end()) {
	sum += (*s).toInt();
	++s;
    }
    return sum;
}

/*!
  First, this function looks up the configuration variable \a var
  in the location map and, if found, sets the internal variable
  \c{lastLoc} to the Location that \a var maps to.
  
  Then it looks up the configuration variable \a var in the string
  map, and returns the string that \a var maps to.
 */
QString Config::getString(const QString& var) const
{
    if (!locMap[var].isEmpty())
	(Location&) lastLoc = locMap[var];
    return stringValueMap[var];
}

/*!
  Looks up the configuration variable \a var in the string
  list map, converts the string list it maps to into a set
  of strings, and returns the set.
 */
QSet<QString> Config::getStringSet(const QString& var) const
{
    return QSet<QString>::fromList(getStringList(var));
}

/*!
  First, this function looks up the configuration variable \a var
  in the location map and, if found, sets the internal variable
  \c{lastLoc} the Location that \a var maps to.
  
  Then it looks up the configuration variable \a var in the string
  list map, and returns the string list that \a var maps to.
 */
QStringList Config::getStringList(const QString& var) const
{
    if (!locMap[var].isEmpty())
	(Location&) lastLoc = locMap[var];
    return stringListValueMap[var];
}

/*!
  Calls getRegExpList() with the control variable \a var and
  iterates through the resulting list of regular expressions,
  concatening them with some extras characters to form a single
  QRegExp, which is returned/

  \sa getRegExpList()
 */
QRegExp Config::getRegExp(const QString& var) const
{
    QString pattern;
    QList<QRegExp> subRegExps = getRegExpList(var);
    QList<QRegExp>::ConstIterator s = subRegExps.begin();

    while (s != subRegExps.end()) {
        if (!(*s).isValid())
            return *s;
        if (!pattern.isEmpty())
            pattern += QLatin1Char('|');
        pattern += QLatin1String("(?:") + (*s).pattern() + QLatin1Char(')');
        ++s;
    }
    if (pattern.isEmpty())
        pattern = QLatin1String("$x"); // cannot match
    return QRegExp(pattern);
}

/*!
  Looks up the configuration variable \a var in the string list
  map, converts the string list to a list of regular expressions,
  and returns it.
 */
QList<QRegExp> Config::getRegExpList(const QString& var) const
{
    QStringList strs = getStringList(var);
    QStringList::ConstIterator s = strs.begin();
    QList<QRegExp> regExps;

    while (s != strs.end()) {
	regExps += QRegExp(*s);
	++s;
    }
    return regExps;
}

/*!
  This function is slower than it could be.
 */
QSet<QString> Config::subVars(const QString& var) const
{
    QSet<QString> result;
    QString varDot = var + QLatin1Char('.');
    QMap<QString, QString>::ConstIterator v = stringValueMap.begin();
    while (v != stringValueMap.end()) {
        if (v.key().startsWith(varDot)) {
            QString subVar = v.key().mid(varDot.length());
            int dot = subVar.indexOf(QLatin1Char('.'));
            if (dot != -1)
                subVar.truncate(dot);
            result.insert(subVar);
        }
        ++v;
    }
    return result;
}

/*!
 */
QStringList Config::getAllFiles(const QString &filesVar,
                                const QString &dirsVar,
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

/*!
 */
QString Config::findFile(const Location& location,
                         const QStringList& files,
                         const QStringList& dirs,
                         const QString& fileName,
                         QString& userFriendlyFilePath)
{
    if (fileName.isEmpty() || fileName.startsWith(QLatin1Char('/'))) {
        userFriendlyFilePath = fileName;
        return fileName;
    }

    QFileInfo fileInfo;
    QStringList components = fileName.split(QLatin1Char('?'));
    QString firstComponent = components.first();

    QStringList::ConstIterator f = files.begin();
    while (f != files.end()) {
	if (*f == firstComponent ||
            (*f).endsWith(QLatin1Char('/') + firstComponent)) {
	    fileInfo.setFile(*f);
	    if (!fileInfo.exists())
		location.fatal(tr("File '%1' does not exist").arg(*f));
	    break;
	}
	++f;
    }

    if (fileInfo.fileName().isEmpty()) {
	QStringList::ConstIterator d = dirs.begin();
	while (d != dirs.end()) {
	    fileInfo.setFile(QDir(*d), firstComponent);
	    if (fileInfo.exists()) {
		break;
            }
	    ++d;
	}
    }

    userFriendlyFilePath = QString();
    if (!fileInfo.exists())
	    return QString();

    QStringList::ConstIterator c = components.begin();
    for (;;) {
	bool isArchive = (c != components.end() - 1);
	ArchiveExtractor *extractor = 0;
	QString userFriendly = *c;

	if (isArchive) {
	    extractor = ArchiveExtractor::extractorForFileName(userFriendly);
        }

	if (extractor == 0) {
	    Uncompressor *uncompressor =
		    Uncompressor::uncompressorForFileName(userFriendly);
	    if (uncompressor != 0) {
		QString fileNameWithCorrectExtension =
			uncompressor->uncompressedFilePath(
				fileInfo.filePath());
		QString uncompressed = uncompressedFiles[fileInfo.filePath()];
		if (uncompressed.isEmpty()) {
		    uncompressed =
                        QTemporaryFile(fileInfo.filePath()).fileName();
		    uncompressor->uncompressFile(location,
                                                 fileInfo.filePath(),
                                                 uncompressed);
		    uncompressedFiles[fileInfo.filePath()] = uncompressed;
		}
		fileInfo.setFile(uncompressed);

		if (isArchive) {
		    extractor = ArchiveExtractor::extractorForFileName(
					fileNameWithCorrectExtension);
		}
                else {
		    userFriendly = fileNameWithCorrectExtension;
		}
	    }
	}
	userFriendlyFilePath += userFriendly;

	if (isArchive) {
	    if (extractor == 0)
		location.fatal(tr("Unknown archive type '%1'")
				.arg(userFriendlyFilePath));
	    QString extracted = extractedDirs[fileInfo.filePath()];
	    if (extracted.isEmpty()) {
		extracted = QTemporaryFile(fileInfo.filePath()).fileName();
		if (!QDir().mkdir(extracted))
		    location.fatal(tr("Cannot create temporary directory '%1'")
				    .arg(extracted));
		extractor->extractArchive(location, fileInfo.filePath(),
					   extracted);
		extractedDirs[fileInfo.filePath()] = extracted;
	    }
	    ++c;
	    fileInfo.setFile(QDir(extracted), *c);
	}
        else {
	    break;
	}
	userFriendlyFilePath += "?";
    }
    return fileInfo.filePath();
}

/*!
 */
QString Config::findFile(const Location& location,
                         const QStringList& files,
                         const QStringList& dirs,
                         const QString& fileBase,
                         const QStringList& fileExtensions,
                         QString& userFriendlyFilePath)
{
    QStringList::ConstIterator e = fileExtensions.begin();
    while (e != fileExtensions.end()) {
	QString filePath = findFile(location, files, dirs, fileBase + "." + *e,
				     userFriendlyFilePath);
	if (!filePath.isEmpty())
	    return filePath;
	++e;
    }
    return findFile(location, files, dirs, fileBase, userFriendlyFilePath);
}

/*!
 */
QString Config::copyFile(const Location& location,
                         const QString& sourceFilePath,
                         const QString& userFriendlySourceFilePath,
                         const QString& targetDirPath)
{
    QFile inFile(sourceFilePath);
    if (!inFile.open(QFile::ReadOnly)) {
	location.fatal(tr("Cannot open input file '%1': %2")
			.arg(inFile.fileName()).arg(inFile.errorString()));
	return "";
    }

    QString outFileName = userFriendlySourceFilePath;
    int slash = outFileName.lastIndexOf("/");
    if (slash != -1)
	outFileName = outFileName.mid(slash);

    QFile outFile(targetDirPath + "/" + outFileName);
    if (!outFile.open(QFile::WriteOnly)) {
	location.fatal(tr("Cannot open output file '%1': %2")
			.arg(outFile.fileName()).arg(outFile.errorString()));
	return "";
    }

    char buffer[1024];
    int len;
    while ((len = inFile.read(buffer, sizeof(buffer))) > 0) {
	outFile.write(buffer, len);
    }
    return outFileName;
}

/*!
  Finds the largest unicode digit in \a value in the range
  1..7 and returns it.
 */
int Config::numParams(const QString& value)
{
    int max = 0;
    for (int i = 0; i != value.length(); i++) {
        uint c = value[i].unicode();
        if (c > 0 && c < 8)
            max = qMax(max, (int)c);
    }
    return max;
}

/*!
  Removes everything from \a dir. This function is recursive.
  It doesn't remove \a dir itself, but if it was called
  recursively, then the caller will remove \a dir.
 */
bool Config::removeDirContents(const QString& dir)
{
    QDir dirInfo(dir);
    QFileInfoList entries = dirInfo.entryInfoList();

    bool ok = true;

    QFileInfoList::Iterator it = entries.begin();
    while (it != entries.end()) {
	if ((*it).isFile()) {
	    if (!dirInfo.remove((*it).fileName()))
		ok = false;
	}
        else if ((*it).isDir()) {
	    if ((*it).fileName() != "." && (*it).fileName() != "..") {
		if (removeDirContents((*it).absoluteFilePath())) {
		    if (!dirInfo.rmdir((*it).fileName()))
			ok = false;
		}
                else {
		    ok = false;
		}
	    }
	}
	++it;
    }
    return ok;
}

/*!
  Returns true if \a ch is a letter, number, '_', '.',
  '{', '}', or ','.
 */
bool Config::isMetaKeyChar(QChar ch)
{
    return ch.isLetterOrNumber()
        || ch == QLatin1Char('_')
        || ch == QLatin1Char('.')
        || ch == QLatin1Char('{')
        || ch == QLatin1Char('}')
        || ch == QLatin1Char(',');
}

/*!
  Load, parse, and process a qdoc configuration file. This
  function is only called by the other load() function, but
  this one is recursive, i.e., it calls itself when it sees
  an \c{include} statement in the qdog configuration file.
 */
void Config::load(Location location, const QString& fileName)
{
    QRegExp keySyntax("\\w+(?:\\.\\w+)*");

#define SKIP_CHAR() \
    do { \
        location.advance(c); \
        ++i; \
        c = text.at(i); \
        cc = c.unicode(); \
    } while (0)

#define SKIP_SPACES() \
    while (c.isSpace() && cc != '\n') \
        SKIP_CHAR()

#define PUT_CHAR() \
    word += c; \
    SKIP_CHAR();

    if (location.depth() > 16)
        location.fatal(tr("Too many nested includes"));

    QFile fin(fileName);
    if (!fin.open(QFile::ReadOnly | QFile::Text)) {
        fin.setFileName(fileName + ".qdoc");
        if (!fin.open(QFile::ReadOnly | QFile::Text))
            location.fatal(tr("Cannot open file '%1': %2").arg(fileName).arg(fin.errorString()));
    }

    QString text = fin.readAll();
    text += QLatin1String("\n\n");
    text += QChar('\0');
    fin.close();

    location.push(fileName);
    location.start();

    int i = 0;
    QChar c = text.at(0);
    uint cc = c.unicode();
    while (i < (int) text.length()) {
        if (cc == 0)
            ++i;
        else if (c.isSpace()) {
            SKIP_CHAR();
        }
        else if (cc == '#') {
            do {
                SKIP_CHAR();
            } while (cc != '\n');
        }
        else if (isMetaKeyChar(c)) {
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
                stack.process(c, location);
                SKIP_CHAR();
            } while (isMetaKeyChar(c));

            QStringList keys = stack.getExpanded(location);
            SKIP_SPACES();

            if (keys.count() == 1 && keys.first() == "include") {
                QString includeFile;

                if (cc != '(')
                    location.fatal(tr("Bad include syntax"));
                SKIP_CHAR();
                SKIP_SPACES();
                while (!c.isSpace() && cc != '#' && cc != ')') {
                    includeFile += c;
                    SKIP_CHAR();
                }
                SKIP_SPACES();
                if (cc != ')')
                    location.fatal(tr("Bad include syntax"));
                SKIP_CHAR();
                SKIP_SPACES();
                if (cc != '#' && cc != '\n')
                    location.fatal(tr("Trailing garbage"));

                /*
                  Here is the recursive call.
                 */
                load(location,
                      QFileInfo(QFileInfo(fileName).dir(), includeFile)
                      .filePath());
            }
            else {
                /*
                  It wasn't an include statement, so it;s something else.
                 */
                if (cc == '+') {
                    plus = true;
                    SKIP_CHAR();
                }
                if (cc != '=')
                    location.fatal(tr("Expected '=' or '+=' after key"));
                SKIP_CHAR();
                SKIP_SPACES();

                for (;;) {
                    if (cc == '\\') {
                        int metaCharPos;

                        SKIP_CHAR();
                        if (cc == '\n') {
                            SKIP_CHAR();
                        }
                        else if (cc > '0' && cc < '8') {
                            word += QChar(c.digitValue());
                            SKIP_CHAR();
                        }
                        else if ((metaCharPos = QString(QLatin1String("abfnrtv")).indexOf(c)) != -1) {
                            word += "\a\b\f\n\r\t\v"[metaCharPos];
                            SKIP_CHAR();
                        }
                        else {
                            PUT_CHAR();
                        }
                    }
                    else if (c.isSpace() || cc == '#') {
                        if (inQuote) {
                            if (cc == '\n')
                                location.fatal(tr("Unterminated string"));
                            PUT_CHAR();
                        }
                        else {
                            if (!word.isEmpty()) {
                                if (metWord)
                                    stringValue += QLatin1Char(' ');
                                stringValue += word;
                                stringListValue << word;
                                metWord = true;
                                word.clear();
                                prevWordQuoted = false;
                            }
                            if (cc == '\n' || cc == '#')
                                break;
                            SKIP_SPACES();
                        }
                    }
                    else if (cc == '"') {
                        if (inQuote) {
                            if (!prevWordQuoted)
                                stringValue += QLatin1Char(' ');
                            stringValue += word;
                            if (!word.isEmpty())
                                stringListValue << word;
                            metWord = true;
                            word.clear();
                            prevWordQuoted = true;
                        }
                        inQuote = !inQuote;
                        SKIP_CHAR();
                    }
                    else if (cc == '$') {
                        QString var;
                        SKIP_CHAR();
                        while (c.isLetterOrNumber() || cc == '_') {
                            var += c;
                            SKIP_CHAR();
                        }
                        if (!var.isEmpty()) {
                            char *val = getenv(var.toLatin1().data());
                            if (val == 0) {
                                location.fatal(tr("Environment variable '%1' undefined").arg(var));
                            }
                            else {
                                word += QString(val);
                            }
                        }
                    }
                    else {
                        if (!inQuote && cc == '=')
                            location.fatal(tr("Unexpected '='"));
                        PUT_CHAR();
                    }
                }

                QStringList::ConstIterator key = keys.begin();
                while (key != keys.end()) {
                    if (!keySyntax.exactMatch(*key))
                        keyLoc.fatal(tr("Invalid key '%1'").arg(*key));

                    if (plus) {
                        if (locMap[*key].isEmpty()) {
                            locMap[*key] = keyLoc;
                        }
                        else {
                            locMap[*key].setEtc(true);
                        }
                        if (stringValueMap[*key].isEmpty()) {
                            stringValueMap[*key] = stringValue;
                        }
                        else {
                            stringValueMap[*key] +=
                                QLatin1Char(' ') + stringValue;
                        }
                        stringListValueMap[*key] += stringListValue;
                    }
                    else {
                        locMap[*key] = keyLoc;
                        stringValueMap[*key] = stringValue;
                        stringListValueMap[*key] = stringListValue;
                    }
                    ++key;
                }
            }
        }
        else {
            location.fatal(tr("Unexpected character '%1' at beginning of line")
                            .arg(c));
        }
    }
}

QStringList Config::getFilesHere(const QString& dir,
                                 const QString& nameFilter,
                                 const QSet<QString> &excludedDirs)
{
    QStringList result;
    if (excludedDirs.contains(dir))
        return result;

    QDir dirInfo(dir);
    QStringList fileNames;
    QStringList::const_iterator fn;

    dirInfo.setNameFilters(nameFilter.split(' '));
    dirInfo.setSorting(QDir::Name);
    dirInfo.setFilter(QDir::Files);
    fileNames = dirInfo.entryList();
    fn = fileNames.constBegin();
    while (fn != fileNames.constEnd()) {
        if (!fn->startsWith(QLatin1Char('~')))
            result.append(dirInfo.filePath(*fn));
	++fn;
    }    
    
    dirInfo.setNameFilters(QStringList("*"));
    dirInfo.setFilter(QDir::Dirs|QDir::NoDotAndDotDot);
    fileNames = dirInfo.entryList();
    fn = fileNames.constBegin();
    while (fn != fileNames.constEnd()) {
        result += getFilesHere(dirInfo.filePath(*fn), nameFilter, excludedDirs);
	++fn;
    }
    return result;
}

QT_END_NAMESPACE
