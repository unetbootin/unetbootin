/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#include "qplatformdefs.h"
#include "qdir.h"
#include "qabstractfileengine.h"
#ifndef QT_NO_DEBUG_STREAM
#include "qdebug.h"
#endif
#include "qfsfileengine.h"
#include "qdatetime.h"
#include "qstring.h"
#include "qregexp.h"
#include "qvector.h"
#ifdef QT_BUILD_CORE_LIB
# include "qresource.h"
#endif

#include "../kernel/qcoreglobaldata_p.h"
#include <stdlib.h>

QT_BEGIN_NAMESPACE

static QString driveSpec(const QString &path)
{
#ifdef Q_OS_WIN
    if (path.size() < 2)
        return QString();
    char c = path.at(0).toAscii();
    if (c < 'a' && c > 'z' && c < 'A' && c > 'Z')
        return QString();
    if (path.at(1).toAscii() != ':')
        return QString();
    return path.mid(0, 2);
#else
    Q_UNUSED(path);
    return QString();
#endif
}

//************* QDirPrivate
class QDirPrivate
{
    QDir *q_ptr;
    Q_DECLARE_PUBLIC(QDir)

protected:
    QDirPrivate(QDir*, const QDir *copy=0);
    ~QDirPrivate();

    QString initFileEngine(const QString &file);

    void updateFileLists() const;
    void sortFileList(QDir::SortFlags, QStringList &, QStringList *, QFileInfoList *) const;

private:
#ifdef QT3_SUPPORT
    QChar filterSepChar;
    bool matchAllDirs;
#endif
    static inline QChar getFilterSepChar(const QString &nameFilter)
    {
        QChar sep(QLatin1Char(';'));
        int i = nameFilter.indexOf(sep, 0);
        if (i == -1 && nameFilter.indexOf(QLatin1Char(' '), 0) != -1)
            sep = QChar(QLatin1Char(' '));
        return sep;
    }
    static inline QStringList splitFilters(const QString &nameFilter, QChar sep=0) {
        if(sep == 0)
            sep = getFilterSepChar(nameFilter);
        QStringList ret = nameFilter.split(sep);
        for(int i = 0; i < ret.count(); i++)
            ret[i] = ret[i].trimmed();
        return ret;
    }

    struct Data {
        inline Data()
            : ref(1), fileEngine(0)
        { clear(); }
        inline Data(const Data &copy)
            : ref(1), path(copy.path), nameFilters(copy.nameFilters), sort(copy.sort),
              filters(copy.filters), fileEngine(0)
        { clear(); }
        inline ~Data()
        { delete fileEngine; }

        inline void clear() {
            listsDirty = 1;
        }
        mutable QAtomicInt ref;

        QString path;
        QStringList nameFilters;
        QDir::SortFlags sort;
        QDir::Filters filters;

        mutable QAbstractFileEngine *fileEngine;

        mutable uint listsDirty : 1;
        mutable QStringList files;
        mutable QFileInfoList fileInfos;
    } *data;
    inline void setPath(const QString &p)
    {
        detach(false);
        QString path = p;
        if ((path.endsWith(QLatin1Char('/')) || path.endsWith(QLatin1Char('\\')))
                && path.length() > 1) {
#ifdef Q_OS_WIN
            if (!(path.length() == 3 && path.at(1) == QLatin1Char(':')))
#endif
                path.truncate(path.length() - 1);
        }
        if(!data->fileEngine || !QDir::isRelativePath(path))
            path = initFileEngine(path);
        data->fileEngine->setFileName(path);
        // set the path to be the qt friendly version so then we can operate on it using just /
        data->path = data->fileEngine->fileName(QAbstractFileEngine::DefaultName);
        data->clear();
    }
    inline void reset() {
        detach();
        data->clear();
    }
    void detach(bool createFileEngine = true);
};

QDirPrivate::QDirPrivate(QDir *qq, const QDir *copy) : q_ptr(qq)
#ifdef QT3_SUPPORT
                                                     , filterSepChar(0)
                                                     , matchAllDirs(false)
#endif
{
    if(copy) {
        copy->d_func()->data->ref.ref();
        data = copy->d_func()->data;
    } else {
        data = new QDirPrivate::Data;
        data->clear();
    }
}

QDirPrivate::~QDirPrivate()
{
    if (!data->ref.deref())
        delete data;
    data = 0;
    q_ptr = 0;
}

/* For sorting */
struct QDirSortItem {
    QString filename_cache;
    QString suffix_cache;
    QFileInfo item;
};
static int qt_cmp_si_sort_flags;

#if defined(Q_C_CALLBACKS)
extern "C" {
#endif

#ifdef Q_OS_WINCE
static int __cdecl qt_cmp_si(const void *n1, const void *n2)
#else
static int qt_cmp_si(const void *n1, const void *n2)
#endif
{
    if (!n1 || !n2)
        return 0;

    QDirSortItem* f1 = (QDirSortItem*)n1;
    QDirSortItem* f2 = (QDirSortItem*)n2;

    if ((qt_cmp_si_sort_flags & QDir::DirsFirst) && (f1->item.isDir() != f2->item.isDir()))
        return f1->item.isDir() ? -1 : 1;
    if ((qt_cmp_si_sort_flags & QDir::DirsLast) && (f1->item.isDir() != f2->item.isDir()))
        return f1->item.isDir() ? 1 : -1;

    int r = 0;
    int sortBy = (qt_cmp_si_sort_flags & QDir::SortByMask)
                 | (qt_cmp_si_sort_flags & QDir::Type);

    switch (sortBy) {
      case QDir::Time:
        r = f1->item.lastModified().secsTo(f2->item.lastModified());
        break;
      case QDir::Size:
          r = int(qBound<qint64>(-1, f2->item.size() - f1->item.size(), 1));
        break;
      case QDir::Type:
      {
        bool ic = qt_cmp_si_sort_flags & QDir::IgnoreCase;

        if (f1->suffix_cache.isNull())
            f1->suffix_cache = ic ? f1->item.suffix().toLower()
                               : f1->item.suffix();
        if (f2->suffix_cache.isNull())
            f2->suffix_cache = ic ? f2->item.suffix().toLower()
                               : f2->item.suffix();

	r = qt_cmp_si_sort_flags & QDir::LocaleAware
            ? f1->suffix_cache.localeAwareCompare(f2->suffix_cache)
            : f1->suffix_cache.compare(f2->suffix_cache);
      }
        break;
      default:
        ;
    }

    if (r == 0 && sortBy != QDir::Unsorted) {
        // Still not sorted - sort by name
        bool ic = qt_cmp_si_sort_flags & QDir::IgnoreCase;

        if (f1->filename_cache.isNull())
            f1->filename_cache = ic ? f1->item.fileName().toLower()
                                    : f1->item.fileName();
        if (f2->filename_cache.isNull())
            f2->filename_cache = ic ? f2->item.fileName().toLower()
                                    : f2->item.fileName();

	r = qt_cmp_si_sort_flags & QDir::LocaleAware
            ? f1->filename_cache.localeAwareCompare(f2->filename_cache)
            : f1->filename_cache.compare(f2->filename_cache);
    }

    if (r == 0) // Enforce an order - the order the items appear in the array
        r = (char*)n1 - (char*)n2;

    if (qt_cmp_si_sort_flags & QDir::Reversed)
        return -r;
    return r;
}

#if defined(Q_C_CALLBACKS)
}
#endif

inline void QDirPrivate::sortFileList(QDir::SortFlags sort, QStringList &l,
                                      QStringList *names, QFileInfoList *infos) const
{
    if(names)
        names->clear();
    if(infos)
        infos->clear();
    if(!l.isEmpty()) {
        QDirSortItem *si= new QDirSortItem[l.count()];
        int i;
        for (i = 0; i < l.size(); ++i) {
            QString path = data->path;
            if (!path.isEmpty() && !path.endsWith(QLatin1Char('/')))
                path += QLatin1Char('/');
            si[i].item = QFileInfo(path + l.at(i));
        }
        qt_cmp_si_sort_flags = sort;
        if ((sort & QDir::SortByMask) != QDir::Unsorted)
            qsort(si, i, sizeof(si[0]), qt_cmp_si);
        // put them back in the list(s)
        for (int j = 0; j<i; j++) {
            if(infos)
                infos->append(si[j].item);
            if(names)
                names->append(si[j].item.fileName());
        }
        delete [] si;
    }
}

inline void QDirPrivate::updateFileLists() const
{
    if(data->listsDirty) {
        QStringList l = data->fileEngine->entryList(data->filters, data->nameFilters);
        sortFileList(data->sort, l, &data->files, &data->fileInfos);
        data->listsDirty = 0;
    }
}

QString QDirPrivate::initFileEngine(const QString &path)
{
    detach(false);
    delete data->fileEngine;
    data->fileEngine = 0;
    data->clear();
    data->fileEngine = QAbstractFileEngine::create(path);
    return data->fileEngine->fileName(QAbstractFileEngine::DefaultName);
}

void QDirPrivate::detach(bool createFileEngine)
{
    qAtomicDetach(data);
    if (createFileEngine) {
        delete data->fileEngine;
        data->fileEngine = QAbstractFileEngine::create(data->path);
    }
}

/*!
    \class QDir
    \brief The QDir class provides access to directory structures and their contents.

    \ingroup io
    \ingroup shared
    \reentrant
    \mainclass

    A QDir is used to manipulate path names, access information
    regarding paths and files, and manipulate the underlying file
    system. It can also be used to access Qt's \l{resource system}.

    Qt uses "/" as a universal directory separator in the same way
    that "/" is used as a path separator in URLs. If you always use
    "/" as a directory separator, Qt will translate your paths to
    conform to the underlying operating system.

    A QDir can point to a file using either a relative or an absolute
    path. Absolute paths begin with the directory separator
    (optionally preceded by a drive specification under Windows).
    Relative file names begin with a directory name or a file name and
    specify a path relative to the current directory.

    Examples of absolute paths:

    \snippet doc/src/snippets/code/src_corelib_io_qdir.cpp 0

    On Windows, the second example above will be translated to
    \c{C:\Documents and Settings} when used to access files.

    Examples of relative paths:

    \snippet doc/src/snippets/code/src_corelib_io_qdir.cpp 1

    You can use the isRelative() or isAbsolute() functions to check if
    a QDir is using a relative or an absolute file path. Call
    makeAbsolute() to convert a relative QDir to an absolute one.

    \section1 Navigation and Directory Operations

    A directory's path can be obtained with the path() function, and
    a new path set with the setPath() function. The absolute path to
    a directory is found by calling absolutePath().

    The name of a directory is found using the dirName() function. This
    typically returns the last element in the absolute path that specifies
    the location of the directory. However, it can also return "." if
    the QDir represents the current directory.

    \snippet doc/src/snippets/code/src_corelib_io_qdir.cpp 2

    The path for a directory can also be changed with the cd() and cdUp()
    functions, both of which operate like familiar shell commands.
    When cd() is called with the name of an existing directory, the QDir
    object changes directory so that it represents that directory instead.
    The cdUp() function changes the directory of the QDir object so that
    it refers to its parent directory; i.e. cd("..") is equivalent to
    cdUp().

    Directories can be created with mkdir(), renamed with rename(), and
    removed with rmdir().

    You can test for the presence of a directory with a given name by
    using exists(), and the properties of a directory can be tested with
    isReadable(), isAbsolute(), isRelative(), and isRoot().

    The refresh() function re-reads the directory's data from disk.

    \section1 Files and Directory Contents

    Directories contain a number of entries, representing files,
    directories, and symbolic links. The number of entries in a
    directory is returned by count().
    A string list of the names of all the entries in a directory can be
    obtained with entryList(). If you need information about each
    entry, use entryInfoList() to obtain a list of QFileInfo objects.

    Paths to files and directories within a directory can be
    constructed using filePath() and absoluteFilePath().
    The filePath() function returns a path to the specified file
    or directory relative to the path of the QDir object;
    absoluteFilePath() returns an absolute path to the specified
    file or directory. Neither of these functions checks for the
    existence of files or directory; they only construct paths.

    \snippet doc/src/snippets/code/src_corelib_io_qdir.cpp 3

    Files can be removed by using the remove() function. Directories
    cannot be removed in the same way as files; use rmdir() to remove
    them instead.

    It is possible to reduce the number of entries returned by
    entryList() and entryInfoList() by applying filters to a QDir object.
    You can apply a name filter to specify a pattern with wildcards that
    file names need to match, an attribute filter that selects properties
    of entries and can distinguish between files and directories, and a
    sort order.

    Name filters are lists of strings that are passed to setNameFilters().
    Attribute filters consist of a bitwise OR combination of Filters, and
    these are specified when calling setFilter().
    The sort order is specified using setSorting() with a bitwise OR
    combination of SortFlags.

    You can test to see if a filename matches a filter using the match()
    function.

    Filter and sort order flags may also be specified when calling
    entryList() and entryInfoList() in order to override previously defined
    behavior.

    \section1 The Current Directory and Other Special Paths

    Access to some common directories is provided with a number of static
    functions that return QDir objects. There are also corresponding functions
    for these that return strings:

    \table
    \header \o QDir      \o QString         \o Return Value
    \row    \o current() \o currentPath()   \o The application's working directory
    \row    \o home()    \o homePath()      \o The user's home directory
    \row    \o root()    \o rootPath()      \o The root directory
    \row    \o temp()    \o tempPath()      \o The system's temporary directory
    \endtable

    The setCurrent() static function can also be used to set the application's
    working directory.

    If you want to find the directory containing the application's executable,
    see \l{QCoreApplication::applicationDirPath()}.

    The drives() static function provides a list of root directories for each
    device that contains a filing system. On Unix systems this returns a list
    containing a single root directory "/"; on Windows the list will usually
    contain \c{C:/}, and possibly other drive letters such as \c{D:/}, depending
    on the configuration of the user's system.

    \section1 Path Manipulation and Strings

    Paths containing "." elements that reference the current directory at that
    point in the path, ".." elements that reference the parent directory, and
    symbolic links can be reduced to a canonical form using the canonicalPath()
    function.

    Paths can also be simplified by using cleanPath() to remove redundant "/"
    and ".." elements.

    It is sometimes necessary to be able to show a path in the native
    representation for the user's platform. The static toNativeSeparators()
    function returns a copy of the specified path in which each directory
    separator is replaced by the appropriate separator for the underlying
    operating system.

    \section1 Examples

    Check if a directory exists:

    \snippet doc/src/snippets/code/src_corelib_io_qdir.cpp 4

    (We could also use the static convenience function
    QFile::exists().)

    Traversing directories and reading a file:

    \snippet doc/src/snippets/code/src_corelib_io_qdir.cpp 5

    A program that lists all the files in the current directory
    (excluding symbolic links), sorted by size, smallest first:

    \snippet doc/src/snippets/qdir-listfiles/main.cpp 0

    \sa QFileInfo, QFile, QFileDialog, QApplication::applicationDirPath(), {Find Files Example}
*/

/*!
    Constructs a QDir pointing to the given directory \a path. If path
    is empty the program's working directory, ("."), is used.

    \sa currentPath()
*/

QDir::QDir(const QString &path) : d_ptr(new QDirPrivate(this))
{
    Q_D(QDir);
    d->setPath(path.isEmpty() ? QString::fromLatin1(".") : path);
    d->data->nameFilters = QStringList(QString::fromLatin1("*"));
    d->data->filters = AllEntries;
    d->data->sort = SortFlags(Name | IgnoreCase);
}

/*!
    Constructs a QDir with path \a path, that filters its entries by
    name using \a nameFilter and by attributes using \a filters. It
    also sorts the names using \a sort.

    The default \a nameFilter is an empty string, which excludes
    nothing; the default \a filters is \l AllEntries, which also means
    exclude nothing. The default \a sort is \l Name | \l IgnoreCase,
    i.e. sort by name case-insensitively.

    If \a path is an empty string, QDir uses "." (the current
    directory). If \a nameFilter is an empty string, QDir uses the
    name filter "*" (all files).

    Note that \a path need not exist.

    \sa exists(), setPath(), setNameFilter(), setFilter(), setSorting()
*/

QDir::QDir(const QString &path, const QString &nameFilter,
           SortFlags sort, Filters filters)  : d_ptr(new QDirPrivate(this))
{
    Q_D(QDir);
    d->setPath(path.isEmpty() ? QString::fromLatin1(".") : path);
    d->data->nameFilters = QDir::nameFiltersFromString(nameFilter);
    bool empty = d->data->nameFilters.isEmpty();
    if(!empty) {
        empty = true;
        for(int i = 0; i < d->data->nameFilters.size(); ++i) {
            if(!d->data->nameFilters.at(i).isEmpty()) {
                empty = false;
                break;
            }
        }
    }
    if (empty)
        d->data->nameFilters = QStringList(QString::fromLatin1("*"));
    d->data->sort = sort;
    d->data->filters = filters;
}

/*!
    Constructs a QDir object that is a copy of the QDir object for
    directory \a dir.

    \sa operator=()
*/

QDir::QDir(const QDir &dir)  : d_ptr(new QDirPrivate(this, &dir))
{
}

/*!
    Destroys the QDir object frees up its resources. This has no
    effect on the underlying directory in the file system.
*/

QDir::~QDir()
{
    delete d_ptr;
    d_ptr = 0;
}

/*!
    Sets the path of the directory to \a path. The path is cleaned of
    redundant ".", ".." and of multiple separators. No check is made
    to see whether a directory with this path actually exists; but you
    can check for yourself using exists().

    The path can be either absolute or relative. Absolute paths begin
    with the directory separator "/" (optionally preceded by a drive
    specification under Windows). Relative file names begin with a
    directory name or a file name and specify a path relative to the
    current directory. An example of an absolute path is the string
    "/tmp/quartz", a relative path might look like "src/fatlib".

    \sa path(), absolutePath(), exists(), cleanPath(), dirName(),
      absoluteFilePath(), isRelative(), makeAbsolute()
*/

void QDir::setPath(const QString &path)
{
    Q_D(QDir);
    d->setPath(path);
}

/*!
    Returns the path. This may contain symbolic links, but never
    contains redundant ".", ".." or multiple separators.

    The returned path can be either absolute or relative (see
    setPath()).

    \sa setPath(), absolutePath(), exists(), cleanPath(), dirName(),
    absoluteFilePath(), toNativeSeparators(), makeAbsolute()
*/

QString QDir::path() const
{
    Q_D(const QDir);
    return d->data->path;
}

/*!
    Returns the absolute path (a path that starts with "/" or with a
    drive specification), which may contain symbolic links, but never
    contains redundant ".", ".." or multiple separators.

    \sa setPath(), canonicalPath(), exists(), cleanPath(),
    dirName(), absoluteFilePath()
*/

QString QDir::absolutePath() const
{
    Q_D(const QDir);
    QString ret = d->data->path;
    if (QDir::isRelativePath(ret))
        ret = absoluteFilePath(QString::fromLatin1(""));
    return cleanPath(ret);
}


/*!
    Returns the canonical path, i.e. a path without symbolic links or
    redundant "." or ".." elements.

    On systems that do not have symbolic links this function will
    always return the same string that absolutePath() returns. If the
    canonical path does not exist (normally due to dangling symbolic
    links) canonicalPath() returns an empty string.

    Example:

    \snippet doc/src/snippets/code/src_corelib_io_qdir.cpp 6

    \sa path(), absolutePath(), exists(), cleanPath(), dirName(),
        absoluteFilePath()
*/

QString QDir::canonicalPath() const
{
    Q_D(const QDir);

    if(!d->data->fileEngine)
        return QLatin1String("");
    return cleanPath(d->data->fileEngine->fileName(QAbstractFileEngine::CanonicalName));
}

/*!
    Returns the name of the directory; this is \e not the same as the
    path, e.g. a directory with the name "mail", might have the path
    "/var/spool/mail". If the directory has no name (e.g. it is the
    root directory) an empty string is returned.

    No check is made to ensure that a directory with this name
    actually exists; but see exists().

    \sa path(), filePath(), absolutePath(), absoluteFilePath()
*/

QString QDir::dirName() const
{
    Q_D(const QDir);
    int pos = d->data->path.lastIndexOf(QLatin1Char('/'));
    if (pos == -1)
        return d->data->path;
    return d->data->path.mid(pos + 1);
}

/*!
    Returns the path name of a file in the directory. Does \e not
    check if the file actually exists in the directory; but see
    exists(). If the QDir is relative the returned path name will also
    be relative. Redundant multiple separators or "." and ".."
    directories in \a fileName are not removed (see cleanPath()).

    \sa dirName() absoluteFilePath(), isRelative(), canonicalPath()
*/

QString QDir::filePath(const QString &fileName) const
{
    Q_D(const QDir);
    if (isAbsolutePath(fileName))
        return QString(fileName);

    QString ret = d->data->path;
    if(!fileName.isEmpty()) {
        if (!ret.isEmpty() && ret[(int)ret.length()-1] != QLatin1Char('/') && fileName[0] != QLatin1Char('/'))
            ret += QLatin1Char('/');
        ret += fileName;
    }
    return ret;
}

/*!
    Returns the absolute path name of a file in the directory. Does \e
    not check if the file actually exists in the directory; but see
    exists(). Redundant multiple separators or "." and ".."
    directories in \a fileName are not removed (see cleanPath()).

    \sa relativeFilePath() filePath() canonicalPath()
*/

QString QDir::absoluteFilePath(const QString &fileName) const
{
    Q_D(const QDir);
    if (isAbsolutePath(fileName))
        return fileName;
    if(!d->data->fileEngine)
        return fileName;

    QString ret;
#ifndef QT_NO_FSFILEENGINE
    if (isRelativePath(d->data->path)) //get pwd
        ret = QFSFileEngine::currentPath(fileName);
#endif
    if(!d->data->path.isEmpty() && d->data->path != QLatin1String(".")) {
        if (!ret.isEmpty() && !ret.endsWith(QLatin1Char('/')))
            ret += QLatin1Char('/');
        ret += d->data->path;
    }
    if (!fileName.isEmpty()) {
        if (!ret.isEmpty() && !ret.endsWith(QLatin1Char('/')))
            ret += QLatin1Char('/');
        ret += fileName;
    }
    return ret;
}

/*!
    Returns the path to \a fileName relative to the directory.

    \snippet doc/src/snippets/code/src_corelib_io_qdir.cpp 7

    \sa absoluteFilePath() filePath() canonicalPath()
*/

QString QDir::relativeFilePath(const QString &fileName) const
{
    QString dir = absolutePath();
    QString file = cleanPath(fileName);

    if (isRelativePath(file) || isRelativePath(dir))
        return file;

    QString dirDrive = driveSpec(dir);
    QString fileDrive = driveSpec(file);

    bool fileDriveMissing = false;
    if (fileDrive.isEmpty()) {
        fileDrive = dirDrive;
        fileDriveMissing = true;
    }

#ifdef Q_OS_WIN
    if (fileDrive.toLower() != dirDrive.toLower()
        || (file.startsWith(QLatin1String("//"))
        && !dir.startsWith(QLatin1String("//"))))
#else
    if (fileDrive != dirDrive)
#endif
        return file;

    dir.remove(0, dirDrive.size());
    if (!fileDriveMissing)
        file.remove(0, fileDrive.size());

    QString result;
    QStringList dirElts = dir.split(QLatin1Char('/'), QString::SkipEmptyParts);
    QStringList fileElts = file.split(QLatin1Char('/'), QString::SkipEmptyParts);

    int i = 0;
    while (i < dirElts.size() && i < fileElts.size() &&
#ifdef Q_OS_WIN
           dirElts.at(i).toLower() == fileElts.at(i).toLower())
#else
           dirElts.at(i) == fileElts.at(i))
#endif
        ++i;

    for (int j = 0; j < dirElts.size() - i; ++j)
        result += QLatin1String("../");

    for (int j = i; j < fileElts.size(); ++j) {
        result += fileElts.at(j);
        if (j < fileElts.size() - 1)
            result += QLatin1Char('/');
    }

    return result;
}

/*!
    \obsolete

    Use QDir::toNativeSeparators() instead.
*/
QString QDir::convertSeparators(const QString &pathName)
{
    return toNativeSeparators(pathName);
}

/*!
    \since 4.2

    Returns \a pathName with the '/' separators converted to
    separators that are appropriate for the underlying operating
    system.

    On Windows, toNativeSeparators("c:/winnt/system32") returns
    "c:\\winnt\\system32".

    The returned string may be the same as the argument on some
    operating systems, for example on Unix.

    \sa fromNativeSeparators(), separator()
*/
QString QDir::toNativeSeparators(const QString &pathName)
{
    QString n(pathName);
#if defined(Q_FS_FAT) || defined(Q_OS_OS2EMX)
    for (int i=0; i<(int)n.length(); i++) {
        if (n[i] == QLatin1Char('/'))
            n[i] = QLatin1Char('\\');
    }
#endif
    return n;
}

/*!
    \since 4.2

    Returns \a pathName using '/' as file separator. On Windows,
    for instance, fromNativeSeparators("\c{c:\\winnt\\system32}") returns
    "c:/winnt/system32".

    The returned string may be the same as the argument on some
    operating systems, for example on Unix.

    \sa toNativeSeparators(), separator()
*/
QString QDir::fromNativeSeparators(const QString &pathName)
{
    QString n(pathName);
#if defined(Q_FS_FAT) || defined(Q_OS_OS2EMX)
    for (int i=0; i<(int)n.length(); i++) {
        if (n[i] == QLatin1Char('\\'))
            n[i] = QLatin1Char('/');
    }
#endif
    return n;
}

/*!
    Changes the QDir's directory to \a dirName.

    Returns true if the new directory exists and is readable;
    otherwise returns false. Note that the logical cd() operation is
    not performed if the new directory does not exist.

    Calling cd("..") is equivalent to calling cdUp().

    \sa cdUp(), isReadable(), exists(), path()
*/

bool QDir::cd(const QString &dirName)
{
    Q_D(QDir);

    if (dirName.isEmpty() || dirName == QLatin1String("."))
        return true;
    QString newPath = d->data->path;
    if (isAbsolutePath(dirName)) {
        newPath = cleanPath(dirName);
    } else {
        if (isRoot()) {
            if (dirName == QLatin1String(".."))
                return false;
        } else {
            newPath += QLatin1Char('/');
        }

        newPath += dirName;
        if (dirName.indexOf(QLatin1Char('/')) >= 0
            || d->data->path == QLatin1String(".")
            || dirName == QLatin1String("..")) {
            newPath = cleanPath(newPath);
            /*
              If newPath starts with .., we convert it to absolute to
              avoid infinite looping on

                  QDir dir(".");
                  while (dir.cdUp())
                      ;
            */
            if (newPath.startsWith(QLatin1String(".."))) {
                newPath = QFileInfo(newPath).absoluteFilePath();
            }
        }
    }
    {
        QFileInfo fi(newPath);
        if (!(fi.exists() && fi.isDir()))
            return false;
    }

    d->setPath(newPath);
    refresh();
    return true;
}

/*!
    Changes directory by moving one directory up from the QDir's
    current directory.

    Returns true if the new directory exists and is readable;
    otherwise returns false. Note that the logical cdUp() operation is
    not performed if the new directory does not exist.

    \sa cd(), isReadable(), exists(), path()
*/

bool QDir::cdUp()
{
    return cd(QString::fromLatin1(".."));
}

/*!
    Returns the string list set by setNameFilters()
*/

QStringList QDir::nameFilters() const
{
    Q_D(const QDir);

    return d->data->nameFilters;
}

/*!
    Sets the name filters used by entryList() and entryInfoList() to the
    list of filters specified by \a nameFilters.

    Each name filter is a wildcard (globbing) filter that understands
    \c{*} and \c{?} wildcards. (See \l{QRegExp wildcard matching}.)

    For example, the following code sets three name filters on a QDir
    to ensure that only files with extensions typically used for C++
    source files are listed:

    \snippet doc/src/snippets/qdir-namefilters/main.cpp 0

    \sa nameFilters(), setFilter()
*/

void QDir::setNameFilters(const QStringList &nameFilters)
{
    Q_D(QDir);
    d->detach();
    d->data->nameFilters = nameFilters;
}

/*!
    \obsolete
    Adds \a path to the search paths searched in to find resources
    that are not specified with an absolute path. The default search
    path is to search only in the root (\c{:/}).

    Use QDir::addSearchPath() with a prefix instead.

    \sa {The Qt Resource System}, QResource::addSearchPath()
*/

void QDir::addResourceSearchPath(const QString &path)
{
#ifdef QT_BUILD_CORE_LIB
    QResource::addSearchPath(path);
#else
    Q_UNUSED(path)
#endif
}

#ifdef QT_BUILD_CORE_LIB
/*!
    \since 4.3

    Sets or replaces Qt's search paths for file names with the prefix \a prefix
    to \a searchPaths.

    To specify a prefix for a file name, prepend the prefix followed by a single
    colon (e.g., "images:undo.png", "xmldocs:books.xml"). \a prefix can only
    contain letters or numbers (e.g., it cannot contain a colon, nor a slash).

    Qt uses this search path to locate files with a known prefix. The search
    path entries are tested in order, starting with the first entry.

    \snippet doc/src/snippets/code/src_corelib_io_qdir.cpp 8

    File name prefix must be at least 2 characters long to avoid conflicts with
    Windows drive letters.

    Search paths may contain paths to \l{The Qt Resource System}.
*/
void QDir::setSearchPaths(const QString &prefix, const QStringList &searchPaths)
{
    if (prefix.length() < 2) {
        qWarning("QDir::setSearchPaths: Prefix must be longer than 1 character");
        return;
    }

    for (int i = 0; i < prefix.count(); i++) {
        if (!prefix.at(i).isLetterOrNumber()) {
            qWarning("QDir::setSearchPaths: Prefix can only contain letters or numbers");
            return;
        }
    }

    QWriteLocker lock(&QCoreGlobalData::instance()->dirSearchPathsLock);
    QMap<QString, QStringList> &paths = QCoreGlobalData::instance()->dirSearchPaths;
    if (searchPaths.isEmpty()) {
        paths.remove(prefix);
    } else {
        paths.insert(prefix, searchPaths);
    }
}

/*!
    \since 4.3

    Adds \a path to the search path for \a prefix.

    \sa setSearchPaths()
*/
void QDir::addSearchPath(const QString &prefix, const QString &path)
{
    if (path.isEmpty())
        return;

    QWriteLocker lock(&QCoreGlobalData::instance()->dirSearchPathsLock);
    QCoreGlobalData::instance()->dirSearchPaths[prefix] += path;
}

/*!
    \since 4.3

    Returns the search paths for \a prefix.

    \sa setSearchPaths(), addSearchPath()
*/
QStringList QDir::searchPaths(const QString &prefix)
{
    QReadLocker lock(&QCoreGlobalData::instance()->dirSearchPathsLock);
    return QCoreGlobalData::instance()->dirSearchPaths.value(prefix);
}

#endif // QT_BUILD_CORE_LIB

/*!
    Returns the value set by setFilter()
*/

QDir::Filters QDir::filter() const
{
    Q_D(const QDir);

    return d->data->filters;
}

/*!
    \enum QDir::Filter

    This enum describes the filtering options available to QDir; e.g.
    for entryList() and entryInfoList(). The filter value is specified
    by combining values from the following list using the bitwise OR
    operator:

    \value Dirs    List directories that match the filters.
    \value AllDirs  List all directories; i.e. don't apply the filters
                    to directory names.
    \value Files   List files.
    \value Drives  List disk drives (ignored under Unix).
    \value NoSymLinks  Do not list symbolic links (ignored by operating
                       systems that don't support symbolic links).
    \value NoDotAndDotDot Do not list the special entries "." and "..".
    \value AllEntries  List directories, files, drives and symlinks (this does not list
                broken symlinks unless you specify System).
    \value Readable    List files for which the application has read
                       access. The Readable value needs to be combined
                       with Dirs or Files.
    \value Writable    List files for which the application has write
                       access. The Writable value needs to be combined
                       with Dirs or Files.
    \value Executable  List files for which the application has
                       execute access. The Executable value needs to be
                       combined with Dirs or Files.
    \value Modified  Only list files that have been modified (ignored
                     under Unix).
    \value Hidden  List hidden files (on Unix, files starting with a .).
    \value System  List system files (on Unix, FIFOs, sockets and
                   device files)
    \value CaseSensitive  The filter should be case sensitive.

    \omitvalue DefaultFilter
    \omitvalue TypeMask
    \omitvalue All
    \omitvalue RWEMask
    \omitvalue AccessMask
    \omitvalue PermissionMask
    \omitvalue NoFilter

    Functions that use Filter enum values to filter lists of files
    and directories will include symbolic links to files and directories
    unless you set the NoSymLinks value.

    A default constructed QDir will not filter out files based on
    their permissions, so entryList() and entryInfoList() will return
    all files that are readable, writable, executable, or any
    combination of the three.  This makes the default easy to write,
    and at the same time useful.

    For example, setting the \c Readable, \c Writable, and \c Files
    flags allows all files to be listed for which the application has read
    access, write access or both. If the \c Dirs and \c Drives flags are
    also included in this combination then all drives, directories, all
    files that the application can read, write, or execute, and symlinks
    to such files/directories can be listed.

    To retrieve the permissons for a directory, use the
    entryInfoList() function to get the associated QFileInfo objects
    and then use the QFileInfo::permissons() to obtain the permissions
    and ownership for each file.
*/

/*!
    Sets the filter used by entryList() and entryInfoList() to \a
    filters. The filter is used to specify the kind of files that
    should be returned by entryList() and entryInfoList(). See
    \l{QDir::Filter}.

    \sa filter(), setNameFilters()
*/

void QDir::setFilter(Filters filters)
{
    Q_D(QDir);

    d->detach();
    d->data->filters = filters;
}

/*!
    Returns the value set by setSorting()

    \sa setSorting() SortFlag
*/

QDir::SortFlags QDir::sorting() const
{
    Q_D(const QDir);

    return d->data->sort;
}

/*!
    \enum QDir::SortFlag

    This enum describes the sort options available to QDir, e.g. for
    entryList() and entryInfoList(). The sort value is specified by
    OR-ing together values from the following list:

    \value Name  Sort by name.
    \value Time  Sort by time (modification time).
    \value Size  Sort by file size.
    \value Type  Sort by file type (extension).
    \value Unsorted  Do not sort.
    \value NoSort Not sorted by default.

    \value DirsFirst  Put the directories first, then the files.
    \value DirsLast Put the files first, then the directories.
    \value Reversed  Reverse the sort order.
    \value IgnoreCase  Sort case-insensitively.
    \value LocaleAware Sort items appropriately using the current locale settings.

    \omitvalue SortByMask
    \omitvalue DefaultSort

    You can only specify one of the first four.

    If you specify both DirsFirst and Reversed, directories are
    still put first, but in reverse order; the files will be listed
    after the directories, again in reverse order.
*/

/*!
    Sets the sort order used by entryList() and entryInfoList().

    The \a sort is specified by OR-ing values from the enum
    \l{QDir::SortFlag}.

    \sa sorting() SortFlag
*/

void QDir::setSorting(SortFlags sort)
{
    Q_D(QDir);

    d->detach();
    d->data->sort = sort;
}


/*!
    Returns the total number of directories and files in the directory.

    Equivalent to entryList().count().

    \sa operator[](), entryList()
*/

uint QDir::count() const
{
    Q_D(const QDir);

    d->updateFileLists();
    return d->data->files.count();
}

/*!
    Returns the file name at position \a pos in the list of file
    names. Equivalent to entryList().at(index).

    Returns an empty string if \a pos is out of range or if the
    entryList() function failed.

    \sa count(), entryList()
*/

QString QDir::operator[](int pos) const
{
    Q_D(const QDir);

    d->updateFileLists();
    return d->data->files[pos];
}

/*!
    \overload

    Returns a list of the names of all the files and directories in
    the directory, ordered according to the name and attribute filters
    previously set with setNameFilters() and setFilter(), and sorted according
    to the flags set with setSorting().

    The attribute filter and sorting specifications can be overridden using the
    \a filters and \a sort arguments.

    Returns an empty list if the directory is unreadable, does not
    exist, or if nothing matches the specification.

    \note To list symlinks that point to non existing files, \l System must be
     passed to the filter.

    \sa entryInfoList(), setNameFilters(), setSorting(), setFilter()
*/

QStringList QDir::entryList(Filters filters, SortFlags sort) const
{
    Q_D(const QDir);

    return entryList(d->data->nameFilters, filters, sort);
}


/*!
    \overload

    Returns a list of QFileInfo objects for all the files and directories in
    the directory, ordered according to the name and attribute filters
    previously set with setNameFilters() and setFilter(), and sorted according
    to the flags set with setSorting().

    The attribute filter and sorting specifications can be overridden using the
    \a filters and \a sort arguments.

    Returns an empty list if the directory is unreadable, does not
    exist, or if nothing matches the specification.

    \sa entryList(), setNameFilters(), setSorting(), setFilter(), isReadable(), exists()
*/

QFileInfoList QDir::entryInfoList(Filters filters, SortFlags sort) const
{
    Q_D(const QDir);

    return entryInfoList(d->data->nameFilters, filters, sort);
}

/*!
    Returns a list of the names of all the files and
    directories in the directory, ordered according to the name
    and attribute filters previously set with setNameFilters()
    and setFilter(), and sorted according to the flags set with
    setSorting().

    The name filter, file attribute filter, and sorting specification
    can be overridden using the \a nameFilters, \a filters, and \a sort
    arguments.

    Returns an empty list if the directory is unreadable, does not
    exist, or if nothing matches the specification.

    \sa entryInfoList(), setNameFilters(), setSorting(), setFilter()
*/

QStringList QDir::entryList(const QStringList &nameFilters, Filters filters,
                            SortFlags sort) const
{
    Q_D(const QDir);

    if (filters == NoFilter)
        filters = d->data->filters;
#ifdef QT3_SUPPORT
    if (d->matchAllDirs)
        filters |= AllDirs;
#endif
    if (sort == NoSort)
        sort = d->data->sort;
    if (filters == NoFilter && sort == NoSort && nameFilters == d->data->nameFilters) {
        d->updateFileLists();
        return d->data->files;
    }
    QStringList l = d->data->fileEngine->entryList(filters, nameFilters);
    if ((sort & QDir::SortByMask) == QDir::Unsorted)
        return l;

    QStringList ret;
    d->sortFileList(sort, l, &ret, 0);
    return ret;
}

/*!
    Returns a list of QFileInfo objects for all the files and
    directories in the directory, ordered according to the name
    and attribute filters previously set with setNameFilters()
    and setFilter(), and sorted according to the flags set with
    setSorting().

    The name filter, file attribute filter, and sorting specification
    can be overridden using the \a nameFilters, \a filters, and \a sort
    arguments.

    Returns an empty list if the directory is unreadable, does not
    exist, or if nothing matches the specification.

    \sa entryList(), setNameFilters(), setSorting(), setFilter(), isReadable(), exists()
*/

QFileInfoList QDir::entryInfoList(const QStringList &nameFilters, Filters filters,
                                  SortFlags sort) const
{
    Q_D(const QDir);

    if (filters == NoFilter)
        filters = d->data->filters;
#ifdef QT3_SUPPORT
    if (d->matchAllDirs)
        filters |= AllDirs;
#endif
    if (sort == NoSort)
        sort = d->data->sort;
    if (filters == NoFilter && sort == NoSort && nameFilters == d->data->nameFilters) {
        d->updateFileLists();
        return d->data->fileInfos;
    }
    QFileInfoList ret;
    QStringList l = d->data->fileEngine->entryList(filters, nameFilters);
    d->sortFileList(sort, l, 0, &ret);
    return ret;
}

/*!
    Creates a sub-directory called \a dirName.

    Returns true on success; otherwise returns false.

    \sa rmdir()
*/

bool QDir::mkdir(const QString &dirName) const
{
    Q_D(const QDir);

    if (dirName.isEmpty()) {
        qWarning("QDir::mkdir: Empty or null file name(s)");
        return false;
    }
    if(!d->data->fileEngine)
        return false;

    QString fn = filePath(dirName);
    return d->data->fileEngine->mkdir(fn, false);
}

/*!
    Removes the directory specified by \a dirName.

    The directory must be empty for rmdir() to succeed.

    Returns true if successful; otherwise returns false.

    \sa mkdir()
*/

bool QDir::rmdir(const QString &dirName) const
{
    Q_D(const QDir);

    if (dirName.isEmpty()) {
        qWarning("QDir::rmdir: Empty or null file name(s)");
        return false;
    }
    if(!d->data->fileEngine)
        return false;

    QString fn = filePath(dirName);
    return d->data->fileEngine->rmdir(fn, false);
}

/*!
    Creates the directory path \a dirPath.

    The function will create all parent directories necessary to
    create the directory.

    Returns true if successful; otherwise returns false.

    \sa rmpath()
*/

bool QDir::mkpath(const QString &dirPath) const
{
    Q_D(const QDir);

    if (dirPath.isEmpty()) {
        qWarning("QDir::mkpath: Empty or null file name(s)");
        return false;
    }
    if(!d->data->fileEngine)
        return false;

    QString fn = filePath(dirPath);
    return d->data->fileEngine->mkdir(fn, true);
}

/*!
    Removes the directory path \a dirPath.

    The function will remove all parent directories in \a dirPath,
    provided that they are empty. This is the opposite of
    mkpath(dirPath).

    Returns true if successful; otherwise returns false.

    \sa mkpath()
*/
bool QDir::rmpath(const QString &dirPath) const
{
    Q_D(const QDir);

    if (dirPath.isEmpty()) {
        qWarning("QDir::rmpath: Empty or null file name(s)");
        return false;
    }
    if(!d->data->fileEngine)
        return false;

    QString fn = filePath(dirPath);
    return d->data->fileEngine->rmdir(fn, true);
}

/*!
    Returns true if the directory is readable \e and we can open files
    by name; otherwise returns false.

    \warning A false value from this function is not a guarantee that
    files in the directory are not accessible.

    \sa QFileInfo::isReadable()
*/


bool QDir::isReadable() const
{
    Q_D(const QDir);

    if(!d->data->fileEngine)
        return false;
    const QAbstractFileEngine::FileFlags info = d->data->fileEngine->fileFlags(QAbstractFileEngine::DirectoryType
                                                                       |QAbstractFileEngine::PermsMask);
    if(!(info & QAbstractFileEngine::DirectoryType))
        return false;
    return info & QAbstractFileEngine::ReadUserPerm;
}

/*!
    \overload

    Returns true if the \e directory exists; otherwise returns false.
    (If a file with the same name is found this function will return
    false).

    \sa QFileInfo::exists(), QFile::exists()
*/

bool QDir::exists() const
{
    Q_D(const QDir);

    if(!d->data->fileEngine)
        return false;
    const QAbstractFileEngine::FileFlags info =
        d->data->fileEngine->fileFlags(
            QAbstractFileEngine::DirectoryType
            | QAbstractFileEngine::ExistsFlag
            | QAbstractFileEngine::Refresh);
    if(!(info & QAbstractFileEngine::DirectoryType))
        return false;
    return info & QAbstractFileEngine::ExistsFlag;
}

/*!
    Returns true if the directory is the root directory; otherwise
    returns false.

    Note: If the directory is a symbolic link to the root directory
    this function returns false. If you want to test for this use
    canonicalPath(), e.g.

    \snippet doc/src/snippets/code/src_corelib_io_qdir.cpp 9

    \sa root(), rootPath()
*/

bool QDir::isRoot() const
{
    Q_D(const QDir);

    if(!d->data->fileEngine)
        return true;
    return d->data->fileEngine->fileFlags(QAbstractFileEngine::FlagsMask) & QAbstractFileEngine::RootFlag;
}

/*!
    \fn bool QDir::isAbsolute() const

    Returns true if the directory's path is absolute; otherwise
    returns false. See isAbsolutePath().

    \sa isRelative() makeAbsolute() cleanPath()
*/

/*!
   \fn bool QDir::isAbsolutePath(const QString &)

    Returns true if \a path is absolute; returns false if it is
    relative.

    \sa isAbsolute() isRelativePath() makeAbsolute() cleanPath()
*/

/*!
    Returns true if the directory path is relative; otherwise returns
    false. (Under Unix a path is relative if it does not start with a
    "/").

    \sa makeAbsolute() isAbsolute() isAbsolutePath() cleanPath()
*/

bool QDir::isRelative() const
{
    Q_D(const QDir);

    if(!d->data->fileEngine)
        return false;
    return d->data->fileEngine->isRelativePath();
}


/*!
    Converts the directory path to an absolute path. If it is already
    absolute nothing happens. Returns true if the conversion
    succeeded; otherwise returns false.

    \sa isAbsolute() isAbsolutePath() isRelative() cleanPath()
*/

bool QDir::makeAbsolute() // ### What do the return values signify?
{
    Q_D(QDir);

    if(!d->data->fileEngine)
        return false;
    QString absolutePath = d->data->fileEngine->fileName(QAbstractFileEngine::AbsoluteName);
    if(QDir::isRelativePath(absolutePath))
        return false;
    d->detach();
    d->data->path = absolutePath;
    d->data->fileEngine->setFileName(absolutePath);
    if(!(d->data->fileEngine->fileFlags(QAbstractFileEngine::TypesMask) & QAbstractFileEngine::DirectoryType))
        return false;
    return true;
}

/*!
    Returns true if directory \a dir and this directory have the same
    path and their sort and filter settings are the same; otherwise
    returns false.

    Example:

    \snippet doc/src/snippets/code/src_corelib_io_qdir.cpp 10
*/

bool QDir::operator==(const QDir &dir) const
{
    const QDirPrivate *d = d_func();
    const QDirPrivate *other = dir.d_func();

    if(d->data == other->data)
        return true;
    Q_ASSERT(d->data->fileEngine && other->data->fileEngine);
    if(d->data->fileEngine->caseSensitive() != other->data->fileEngine->caseSensitive())
        return false;
    if(d->data->filters == other->data->filters
       && d->data->sort == other->data->sort
       && d->data->nameFilters == other->data->nameFilters) {
        QString dir1 = absolutePath(), dir2 = dir.absolutePath();
        if(!other->data->fileEngine->caseSensitive())
            return (dir1.toLower() == dir2.toLower());

        return (dir1 == dir2);

    }
    return false;
}

/*!
    Makes a copy of the \a dir object and assigns it to this QDir
    object.
*/

QDir &QDir::operator=(const QDir &dir)
{
    if (this == &dir)
        return *this;

    Q_D(QDir);
    qAtomicAssign(d->data, dir.d_func()->data);
    return *this;
}

/*!
    \overload
    \obsolete

    Sets the directory path to the given \a path.

    Use setPath() instead.
*/

QDir &QDir::operator=(const QString &path)
{
    Q_D(QDir);

    d->setPath(path);
    return *this;
}

/*!
    \fn bool QDir::operator!=(const QDir &dir) const

    Returns true if directory \a dir and this directory have different
    paths or different sort or filter settings; otherwise returns
    false.

    Example:

    \snippet doc/src/snippets/code/src_corelib_io_qdir.cpp 11
*/


/*!
    Removes the file, \a fileName.

    Returns true if the file is removed successfully; otherwise
    returns false.
*/

bool QDir::remove(const QString &fileName)
{
    if (fileName.isEmpty()) {
        qWarning("QDir::remove: Empty or null file name");
        return false;
    }
    QString p = filePath(fileName);
    return QFile::remove(p);
}

/*!
    Renames a file or directory from \a oldName to \a newName, and returns
    true if successful; otherwise returns false.

    On most file systems, rename() fails only if \a oldName does not
    exist, if \a newName and \a oldName are not on the same
    partition or if a file with the new name already exists.
    However, there are also other reasons why rename() can
    fail. For example, on at least one file system rename() fails if
    \a newName points to an open file.
*/

bool QDir::rename(const QString &oldName, const QString &newName)
{
    Q_D(QDir);

    if (oldName.isEmpty() || newName.isEmpty()) {
        qWarning("QDir::rename: Empty or null file name(s)");
        return false;
    }
    if(!d->data->fileEngine)
        return false;

    QFile file(filePath(oldName));
    if(!file.exists())
        return false;
    return file.rename(filePath(newName));
}

/*!
    Returns true if the file called \a name exists; otherwise returns
    false. Unless \a name contains an absolute file path, the file
    name is assumed to be relative to the current directory.

    \sa QFileInfo::exists(), QFile::exists()
*/

bool QDir::exists(const QString &name) const
{
    if (name.isEmpty()) {
        qWarning("QDir::exists: Empty or null file name");
        return false;
    }
    QString tmp = filePath(name);
    return QFile::exists(tmp);
}

/*!
    Returns a list of the root directories on this system.

    On Windows this returns a list of QFileInfo objects containing "C:/",
    "D:/", etc. On other operating systems, it returns a list containing
    just one root directory (i.e. "/").

    \sa root(), rootPath()
*/

QFileInfoList QDir::drives()
{
#ifdef QT_NO_FSFILEENGINE
    return QFileInfoList();
#else
    return QFSFileEngine::drives();
#endif
}

/*!
    Returns the native directory separator: "/" under Unix (including
    Mac OS X) and "\\" under Windows.

    You do not need to use this function to build file paths. If you
    always use "/", Qt will translate your paths to conform to the
    underlying operating system. If you want to display paths to the
    user using their operating system's separator use
    toNativeSeparators().
*/

QChar QDir::separator()
{
#if defined(Q_OS_UNIX)
    return QLatin1Char('/');
#elif defined (Q_FS_FAT) || defined(Q_WS_WIN)
    return QLatin1Char('\\');
#elif defined (Q_OS_MAC)
    return QLatin1Char(':');
#else
    return QLatin1Char('/');
#endif
}

/*!
    Sets the application's current working directory to \a path.
    Returns true if the directory was successfully changed; otherwise
    returns false.

    \sa current() currentPath() home() root() temp()
*/

bool QDir::setCurrent(const QString &path)
{
#ifdef QT_NO_FSFILEENGINE
    Q_UNUSED(path);
    return false;
#else
    return QFSFileEngine::setCurrentPath(path);
#endif
}

/*!
    \fn QDir QDir::current()

    Returns the application's current directory.

    The directory is constructed using the absolute path of the current directory,
    ensuring that its path() will be the same as its absolutePath().

    \sa currentPath(), home(), root(), temp()
*/

/*!
    Returns the absolute path of the application's current directory.

    \sa current(), homePath(), rootPath(), tempPath()
*/
QString QDir::currentPath()
{
#ifdef QT_NO_FSFILEENGINE
    return QString();
#else
    return QFSFileEngine::currentPath();
#endif
}

/*!
  \fn QString QDir::currentDirPath()
    Returns the absolute path of the application's current directory.

    Use currentPath() instead.

    \sa currentPath()
*/

/*!
    \fn QDir QDir::home()

    Returns the user's home directory.

    The directory is constructed using the absolute path of the home directory,
    ensuring that its path() will be the same as its absolutePath().

    See homePath() for details.

    \sa drives(), current(), root(), temp()
*/

/*!
    Returns the absolute path of the user's home directory.

    Under Windows this function will return the directory of the
    current user's profile. Typically, this is:

    \snippet doc/src/snippets/code/src_corelib_io_qdir.cpp 12

    Use the toNativeSeparators() function to convert the separators to
    the ones that are appropriate for the underlying operating system.

    If the directory of the current user's profile does not exist or
    cannot be retrieved, the following alternatives will be checked (in
    the given order) until an existing and available path is found:

    \list 1
    \o The path specified by the \c USERPROFILE environment variable.
    \o The path formed by concatenating the \c HOMEDRIVE and \c HOMEPATH
    environment variables.
    \o The path specified by the \c HOME environment variable.
    \o The path returned by the rootPath() function (which uses the \c SystemDrive
    environment variable)
    \o  The \c{C:/} directory.
    \endlist

    Under non-Windows operating systems the \c HOME environment
    variable is used if it exists, otherwise the path returned by the
    rootPath() function is used.

    \sa home(), currentPath(), rootPath(), tempPath()
*/
QString QDir::homePath()
{
#ifdef QT_NO_FSFILEENGINE
    return QString();
#else
    return cleanPath(QFSFileEngine::homePath());
#endif
}

/*!
  \fn QString QDir::homeDirPath()

  Returns the absolute path of the user's home directory.

  Use homePath() instead.

  \sa homePath()
 */

/*!
    \fn QDir QDir::temp()

    Returns the system's temporary directory.

    The directory is constructed using the absolute path of the temporary directory,
    ensuring that its path() will be the same as its absolutePath().

    See tempPath() for details.

    \sa drives(), current(), home(), root()
*/

/*!
    Returns the absolute path of the system's temporary directory.

    On Unix/Linux systems this is usually \c{/tmp}; on Windows this is
    usually the path in the \c TEMP or \c TMP environment
    variable. Whether a directory separator is added to the end or
    not, depends on the operating system.

    \sa temp(), currentPath(), homePath(), rootPath()
*/
QString QDir::tempPath()
{
#ifdef QT_NO_FSFILEENGINE
    return QString();
#else
    return cleanPath(QFSFileEngine::tempPath());
#endif
}

/*!
    \fn QDir QDir::root()

    Returns the root directory.

    The directory is constructed using the absolute path of the root directory,
    ensuring that its path() will be the same as its absolutePath().

    See rootPath() for details.

    \sa drives(), current(), home(), temp()
*/

/*!
    Returns the absolute path of the root directory.

    For Unix operating systems this returns "/". For Windows file
    systems this normally returns "c:/".

    \sa root(), drives(), currentPath(), homePath(), tempPath()
*/
QString QDir::rootPath()
{
#ifdef QT_NO_FSFILEENGINE
    return QString();
#else
    return QFSFileEngine::rootPath();
#endif
}

/*!
  \fn QString QDir::rootDirPath()

  Returns the absolute path of the root directory.

  Use rootPath() instead.

  \sa rootPath()
*/

#ifndef QT_NO_REGEXP
/*!
    \overload

    Returns true if the \a fileName matches any of the wildcard (glob)
    patterns in the list of \a filters; otherwise returns false. The
    matching is case insensitive.

    \sa {QRegExp wildcard matching}, QRegExp::exactMatch() entryList() entryInfoList()
*/


bool QDir::match(const QStringList &filters, const QString &fileName)
{
    for(QStringList::ConstIterator sit = filters.begin(); sit != filters.end(); ++sit) {
        QRegExp rx(*sit, Qt::CaseInsensitive, QRegExp::Wildcard);
        if (rx.exactMatch(fileName))
            return true;
    }
    return false;
}

/*!
    Returns true if the \a fileName matches the wildcard (glob)
    pattern \a filter; otherwise returns false. The \a filter may
    contain multiple patterns separated by spaces or semicolons.
    The matching is case insensitive.

    \sa {QRegExp wildcard matching}, QRegExp::exactMatch() entryList() entryInfoList()
*/

bool QDir::match(const QString &filter, const QString &fileName)
{
    return match(nameFiltersFromString(filter), fileName);
}
#endif

/*!
    Removes all multiple directory separators "/" and resolves any
    "."s or ".."s found in the path, \a path.

    Symbolic links are kept. This function does not return the
    canonical path, but rather the simplest version of the input.
    For example, "./local" becomes "local", "local/../bin" becomes
    "bin" and "/local/usr/../bin" becomes "/local/bin".

    \sa absolutePath() canonicalPath()
*/

QString QDir::cleanPath(const QString &path)
{
    if (path.isEmpty())
        return path;
    QString name = path;
    QChar dir_separator = separator();
    if(dir_separator != QLatin1Char('/'))
	name.replace(dir_separator, QLatin1Char('/'));

    int used = 0, levels = 0;
    const int len = name.length();
    QVector<QChar> out(len);
    const QChar *p = name.unicode();
    for(int i = 0, last = -1, iwrite = 0; i < len; i++) {
        if(p[i] == QLatin1Char('/')) {
            while(i < len-1 && p[i+1] == QLatin1Char('/')) {
#if defined(Q_OS_WIN) && !defined(Q_OS_WINCE) //allow unc paths
                if(!i)
                    break;
#endif
                i++;
            }
            bool eaten = false;
            if(i < len - 1 && p[i+1] == QLatin1Char('.')) {
                int dotcount = 1;
                if(i < len - 2 && p[i+2] == QLatin1Char('.'))
                    dotcount++;
                if(i == len - dotcount - 1) {
                    if(dotcount == 1) {
                        break;
                    } else if(levels) {
                        if(last == -1) {
                            for(int i2 = iwrite-1; i2 >= 0; i2--) {
                                if(out[i2] == QLatin1Char('/')) {
                                    last = i2;
                                    break;
                                }
                            }
                        }
                        used -= iwrite - last - 1;
                        break;
                    }
                } else if(p[i+dotcount+1] == QLatin1Char('/')) {
                    if(dotcount == 2 && levels) {
                        if(last == -1 || iwrite - last == 1) {
                            for(int i2 = (last == -1) ? (iwrite-1) : (last-1); i2 >= 0; i2--) {
                                if(out[i2] == QLatin1Char('/')) {
                                    eaten = true;
                                    last = i2;
                                    break;
                                }
                            }
                        } else {
                            eaten = true;
                        }
                        if(eaten) {
                            levels--;
                            used -= iwrite - last;
                            iwrite = last;
                            last = -1;
                        }
                    } else if (dotcount == 2 && i > 0 && p[i - 1] != QLatin1Char('.')) {
                        eaten = true;
                        used -= iwrite - qMax(0, last);
                        iwrite = qMax(0, last);
                        last = -1;
                        ++i;
                    } else if(dotcount == 1) {
                        eaten = true;
                    }
                    if(eaten)
                        i += dotcount;
                } else {
                    levels++;
                }
            } else if(last != -1 && iwrite - last == 1) {
#ifdef Q_OS_WIN
                eaten = (iwrite > 2);
#else
                eaten = true;
#endif
                last = -1;
            } else if(last != -1 && i == len-1) {
                eaten = true;
            } else {
                levels++;
            }
            if(!eaten)
                last = i - (i - iwrite);
            else
                continue;
        } else if(!i && p[i] == QLatin1Char('.')) {
            int dotcount = 1;
            if(len >= 1 && p[1] == QLatin1Char('.'))
                dotcount++;
            if(len >= dotcount && p[dotcount] == QLatin1Char('/')) {
                if(dotcount == 1) {
                    i++;
                    while(i+1 < len-1 && p[i+1] == QLatin1Char('/'))
                        i++;
                    continue;
                }
            }
        }
        out[iwrite++] = p[i];
        used++;
    }
    QString ret;
    if(used == len)
        ret = name;
    else
	ret = QString(out.data(), used);

    // Strip away last slash except for root directories
    if (ret.endsWith(QLatin1Char('/'))
        && !(ret.size() == 1 || (ret.size() == 3 && ret.at(1) == QLatin1Char(':'))))
        ret = ret.left(ret.length() - 1);

    return ret;
}

/*!
    Returns true if \a path is relative; returns false if it is
    absolute.

    \sa isRelative() isAbsolutePath() makeAbsolute()
*/

bool QDir::isRelativePath(const QString &path)
{
    return QFileInfo(path).isRelative();
}

/*!
    Refreshes the directory information.
*/

void QDir::refresh() const
{
    Q_D(const QDir);

    d->data->clear();
}

/*!
    \internal

    Returns a list of name filters from the given \a nameFilter. (If
    there is more than one filter, each pair of filters is separated
    by a space or by a semicolon.)
*/

QStringList QDir::nameFiltersFromString(const QString &nameFilter)
{
    return QDirPrivate::splitFilters(nameFilter);
}

/*!
    \macro void Q_INIT_RESOURCE(name)
    \relates QDir

    Initializes the resources specified by the \c .qrc file with the
    specified base \a name. Normally, Qt resources are loaded
    automatically at startup. The Q_INIT_RESOURCE() macro is
    necessary on some platforms for resources stored in a static
    library.

    For example, if your application's resources are listed in a file
    called \c myapp.qrc, you can ensure that the resources are
    initialized at startup by adding this line to your \c main()
    function:

    \snippet doc/src/snippets/code/src_corelib_io_qdir.cpp 13

    Note: This macro cannot be used in a namespace. It should be called from
    main(). If that is not possible, the following workaround can be used
    to init the resource \c myapp from the function \c{MyNamespace::myFunction}:

    \snippet doc/src/snippets/code/src_corelib_io_qdir.cpp 14

    \sa Q_CLEANUP_RESOURCE(), {The Qt Resource System}
*/

/*!
    \since 4.1
    \macro void Q_CLEANUP_RESOURCE(name)
    \relates QDir

    Unloads the resources specified by the \c .qrc file with the base
    name \a name.

    Normally, Qt resources are unloaded automatically when the
    application terminates, but if the resources are located in a
    plugin that is being unloaded, call Q_CLEANUP_RESOURCE() to force
    removal of your resources.

    Note: This macro cannot be used in a namespace. Please see the
    Q_INIT_RESOURCE documentation for a workaround.

    Example:

    \snippet doc/src/snippets/code/src_corelib_io_qdir.cpp 15

    \sa Q_INIT_RESOURCE(), {The Qt Resource System}
*/

#ifdef QT3_SUPPORT

/*!
    \fn bool QDir::matchAllDirs() const

    Use filter() & AllDirs instead.
*/
bool QDir::matchAllDirs() const
{
    Q_D(const QDir);
    return d->matchAllDirs;
}


/*!
    \fn void QDir::setMatchAllDirs(bool on)

    Use setFilter() instead.
*/
void QDir::setMatchAllDirs(bool on)
{
    Q_D(QDir);
    d->matchAllDirs = on;
}

/*!
    Use nameFilters() instead.
*/
QString QDir::nameFilter() const
{
    Q_D(const QDir);

    return nameFilters().join(QString(d->filterSepChar));
}

/*!
    Use setNameFilters() instead.

    The \a nameFilter is a wildcard (globbing) filter that understands
    "*" and "?" wildcards. (See \l{QRegExp wildcard matching}.) You may
    specify several filter entries, each separated by spaces or by
    semicolons.

    For example, if you want entryList() and entryInfoList() to list
    all files ending with either ".cpp" or ".h", you would use either
    dir.setNameFilters("*.cpp *.h") or dir.setNameFilters("*.cpp;*.h").

    \oldcode
        QString filter = "*.cpp *.cxx *.cc";
        dir.setNameFilter(filter);
    \newcode
        QString filter = "*.cpp *.cxx *.cc";
        dir.setNameFilters(filter.split(' '));
    \endcode
*/
void QDir::setNameFilter(const QString &nameFilter)
{
    Q_D(QDir);

    d->filterSepChar = QDirPrivate::getFilterSepChar(nameFilter);
    setNameFilters(QDirPrivate::splitFilters(nameFilter, d->filterSepChar));
}

/*!
    \fn QString QDir::absPath() const

    Use absolutePath() instead.
*/

/*!
    \fn QString QDir::absFilePath(const QString &fileName, bool acceptAbsPath) const

    Use absoluteFilePath(\a fileName) instead.

    The \a acceptAbsPath parameter is ignored.
*/

/*!
    \fn bool QDir::mkdir(const QString &dirName, bool acceptAbsPath) const

    Use mkdir(\a dirName) instead.

    The \a acceptAbsPath parameter is ignored.
*/

/*!
    \fn bool QDir::rmdir(const QString &dirName, bool acceptAbsPath) const

    Use rmdir(\a dirName) instead.

    The \a acceptAbsPath parameter is ignored.
*/

/*!
    \fn QStringList QDir::entryList(const QString &nameFilter, Filters filters,
                                    SortFlags sort) const
    \overload

    Use the overload that takes a name filter string list as first
    argument instead of a combination of attribute filter flags.
*/

/*!
    \fn QFileInfoList QDir::entryInfoList(const QString &nameFilter, Filters filters,
                                          SortFlags sort) const
    \overload

    Use the overload that takes a name filter string list as first
    argument instead of a combination of attribute filter flags.
*/

/*!
    \fn void QDir::convertToAbs()

    Use makeAbsolute() instead.
*/

/*!
    \fn QString QDir::cleanDirPath(const QString &name)

    Use cleanPath() instead.
*/

/*!
    \typedef QDir::FilterSpec

    Use QDir::Filters instead.
*/

/*!
    \typedef QDir::SortSpec

    Use QDir::SortFlags instead.
*/
#endif
#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug debug, QDir::Filters filters)
{
    QStringList flags;
    if (filters == QDir::NoFilter) {
        flags << QLatin1String("NoFilter");
    } else {
        if (filters & QDir::Dirs) flags << QLatin1String("Dirs");
        if (filters & QDir::AllDirs) flags << QLatin1String("AllDirs");
        if (filters & QDir::Files) flags << QLatin1String("Files");
        if (filters & QDir::Drives) flags << QLatin1String("Drives");
        if (filters & QDir::NoSymLinks) flags << QLatin1String("NoSymLinks");
        if (filters & QDir::NoDotAndDotDot) flags << QLatin1String("NoDotAndDotDot");
        if ((filters & QDir::AllEntries) == QDir::AllEntries) flags << QLatin1String("AllEntries");
        if (filters & QDir::Readable) flags << QLatin1String("Readable");
        if (filters & QDir::Writable) flags << QLatin1String("Writable");
        if (filters & QDir::Executable) flags << QLatin1String("Executable");
        if (filters & QDir::Modified) flags << QLatin1String("Modified");
        if (filters & QDir::Hidden) flags << QLatin1String("Hidden");
        if (filters & QDir::System) flags << QLatin1String("System");
        if (filters & QDir::CaseSensitive) flags << QLatin1String("CaseSensitive");
    }
    debug << "QDir::Filters(" << qPrintable(flags.join(QLatin1String("|"))) << ")";
    return debug;
}

QDebug operator<<(QDebug debug, QDir::SortFlags sorting)
{
    if (sorting == QDir::NoSort) {
        debug << "QDir::SortFlags(NoSort)";
    } else {
        QString type;
        if ((sorting & 3) == QDir::Name) type = QLatin1String("Name");
        if ((sorting & 3) == QDir::Time) type = QLatin1String("Time");
        if ((sorting & 3) == QDir::Size) type = QLatin1String("Size");
        if ((sorting & 3) == QDir::Unsorted) type = QLatin1String("Unsorted");

        QStringList flags;
        if (sorting & QDir::DirsFirst) flags << QLatin1String("DirsFirst");
        if (sorting & QDir::DirsLast) flags << QLatin1String("DirsLast");
        if (sorting & QDir::IgnoreCase) flags << QLatin1String("IgnoreCase");
        if (sorting & QDir::LocaleAware) flags << QLatin1String("LocaleAware");
        if (sorting & QDir::Type) flags << QLatin1String("Type");
        debug << "QDir::SortFlags(" << qPrintable(type)
              << "|"
              << qPrintable(flags.join(QLatin1String("|"))) << ")";
    }
    return debug;
}

QDebug operator<<(QDebug debug, const QDir &dir)
{
    debug.maybeSpace() << "QDir(" << dir.path()
                       << ", nameFilters = {"
                       << qPrintable(dir.nameFilters().join(QLatin1String(",")))
                       << "}, "
                       << dir.sorting()
                       << ","
                       << dir.filter()
                       << ")";
    return debug.space();
}



#endif

QT_END_NAMESPACE
