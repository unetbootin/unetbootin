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
#include "qfileinfo.h"
#include "qdatetime.h"
#include "qabstractfileengine.h"
#include "qfsfileengine_p.h"
#include "qglobal.h"
#include "qatomic.h"
#include "qhash.h"
#include "qdir.h"

QT_BEGIN_NAMESPACE

class QFileInfoPrivate
{
public:
    QFileInfoPrivate(const QFileInfo *copy=0);
    ~QFileInfoPrivate();

    void initFileEngine(const QString &);

    enum Access {
        ReadAccess,
        WriteAccess,
        ExecuteAccess
    };
    bool hasAccess(Access access) const;

    uint getFileFlags(QAbstractFileEngine::FileFlags) const;
    QDateTime &getFileTime(QAbstractFileEngine::FileTime) const;
    QString getFileName(QAbstractFileEngine::FileName) const;

    enum { CachedFileFlags=0x01, CachedLinkTypeFlag=0x02, CachedBundleTypeFlag=0x04,
           CachedMTime=0x10, CachedCTime=0x20, CachedATime=0x40,
           CachedSize =0x08 };
    struct Data {
        inline Data()
            : ref(1), fileEngine(0), cache_enabled(1)
        { clear(); }
        inline Data(const Data &copy)
            : ref(1), fileEngine(QAbstractFileEngine::create(copy.fileName)),
              fileName(copy.fileName), cache_enabled(copy.cache_enabled)
        { clear(); }
        inline ~Data() { delete fileEngine; }
        inline void clearFlags() {
            fileFlags = 0;
            cachedFlags = 0;
            if (fileEngine)
                (void)fileEngine->fileFlags(QFSFileEngine::Refresh);
        }
        inline void clear() {
            fileNames.clear();
            clearFlags();
        }
        mutable QAtomicInt ref;

        QAbstractFileEngine *fileEngine;
        mutable QString fileName;
        mutable QHash<int, QString> fileNames;

        mutable uint cachedFlags : 31;
        mutable uint cache_enabled : 1;
        mutable uint fileFlags;
        mutable qint64 fileSize;
        mutable QDateTime fileTimes[3];
        inline bool getCachedFlag(uint c) const
        { return cache_enabled ? (cachedFlags & c) : 0; }
        inline void setCachedFlag(uint c)
        { if (cache_enabled) cachedFlags |= c; }
    } *data;
    inline void reset() {
        detach();
        data->clear();
    }
    void detach();
};

QFileInfoPrivate::QFileInfoPrivate(const QFileInfo *copy)
{
    if(copy) {
        copy->d_func()->data->ref.ref();
        data = copy->d_func()->data;
    } else {
        data = new QFileInfoPrivate::Data;
        data->clear();
    }
}

QFileInfoPrivate::~QFileInfoPrivate()
{
    if (!data->ref.deref())
        delete data;
    data = 0;
}

void QFileInfoPrivate::initFileEngine(const QString &file)
{
    detach();
    delete data->fileEngine;
    data->fileEngine = 0;
    data->clear();
    data->fileEngine = QAbstractFileEngine::create(file);
    data->fileName = file;
}

bool QFileInfoPrivate::hasAccess(Access access) const
{
    if (!(data->fileEngine->fileFlags() & QAbstractFileEngine::LocalDiskFlag)) {
        switch (access) {
        case ReadAccess:
            return getFileFlags(QAbstractFileEngine::ReadUserPerm);
        case WriteAccess:
            return getFileFlags(QAbstractFileEngine::WriteUserPerm);
        case ExecuteAccess:
            return getFileFlags(QAbstractFileEngine::ExeUserPerm);
        default:
            return false;
        }
    }

    int mode = 0;
    switch (access) {
    case ReadAccess:
        mode = R_OK;
        break;
    case WriteAccess:
        mode = W_OK;
        break;
    case ExecuteAccess:
        mode = X_OK;
        break;
    };
#ifdef Q_OS_UNIX
    return QT_ACCESS(QFile::encodeName(data->fileName).data(), mode) == 0;
#endif
#ifdef Q_OS_WIN
    if ((access == ReadAccess && !getFileFlags(QAbstractFileEngine::ReadUserPerm))
        || (access == WriteAccess && !getFileFlags(QAbstractFileEngine::WriteUserPerm))) {
        return false;
    }
    if (access == ExecuteAccess)
        return getFileFlags(QAbstractFileEngine::ExeUserPerm);

    QT_WA( {
        return ::_waccess((TCHAR *)QFSFileEnginePrivate::longFileName(data->fileName).utf16(), mode) == 0;
    } , {
        return QT_ACCESS(QFSFileEnginePrivate::win95Name(data->fileName), mode) == 0;
    } );
#endif
    return false;
}

void QFileInfoPrivate::detach()
{
    qAtomicDetach(data);
}

QString QFileInfoPrivate::getFileName(QAbstractFileEngine::FileName name) const
{
    if(data->cache_enabled && data->fileNames.contains((int)name))
        return data->fileNames.value(name);
    QString ret = data->fileEngine->fileName(name);
    if(data->cache_enabled)
        data->fileNames.insert((int)name, ret);
    return ret;
}

uint QFileInfoPrivate::getFileFlags(QAbstractFileEngine::FileFlags request) const
{
    // We split the testing into tests for for LinkType, BundleType and the rest.
    // In order to determine if a file is a symlink or not, we have to lstat().
    // If we're not interested in that information, we might as well avoid one
    // extra syscall. Bundle detecton on Mac can be slow, expecially on network
    // paths, so we separate out that as well.

    QAbstractFileEngine::FileFlags flags;
    if (!data->getCachedFlag(CachedFileFlags)) {
        QAbstractFileEngine::FileFlags req = QAbstractFileEngine::FileInfoAll;
        req &= (~QAbstractFileEngine::LinkType);
        req &= (~QAbstractFileEngine::BundleType);

        flags = data->fileEngine->fileFlags(req);
        data->setCachedFlag(CachedFileFlags);
        data->fileFlags |= uint(flags);
    } else {
        flags = QAbstractFileEngine::FileFlags(data->fileFlags & request);
    }

    if (request & QAbstractFileEngine::LinkType) {
        if (!data->getCachedFlag(CachedLinkTypeFlag)) {
            QAbstractFileEngine::FileFlags linkflag;
            linkflag = data->fileEngine->fileFlags(QAbstractFileEngine::LinkType);

            data->setCachedFlag(CachedLinkTypeFlag);
            data->fileFlags |= uint(linkflag);
            flags |= linkflag;
        }
    }

    if (request & QAbstractFileEngine::BundleType) {
        if (!data->getCachedFlag(CachedBundleTypeFlag)) {
            QAbstractFileEngine::FileFlags bundleflag;
            bundleflag = data->fileEngine->fileFlags(QAbstractFileEngine::BundleType);

            data->setCachedFlag(CachedBundleTypeFlag);
            data->fileFlags |= uint(bundleflag);
            flags |= bundleflag;
        }
    }

    // no else branch
    // if we had it cached, it was caught in the previous else branch

    return flags & request;
}

QDateTime &QFileInfoPrivate::getFileTime(QAbstractFileEngine::FileTime request) const
{
    if (!data->cache_enabled)
        data->clearFlags();
    if(request == QAbstractFileEngine::CreationTime) {
        if(data->getCachedFlag(CachedCTime))
            return data->fileTimes[request];
        data->setCachedFlag(CachedCTime);
        return (data->fileTimes[request] = data->fileEngine->fileTime(request));
    }
    if(request == QAbstractFileEngine::ModificationTime) {
        if(data->getCachedFlag(CachedMTime))
            return data->fileTimes[request];
        data->setCachedFlag(CachedMTime);
        return (data->fileTimes[request] = data->fileEngine->fileTime(request));
    }
    if(request == QAbstractFileEngine::AccessTime) {
        if(data->getCachedFlag(CachedATime))
            return data->fileTimes[request];
        data->setCachedFlag(CachedATime);
        return (data->fileTimes[request] = data->fileEngine->fileTime(request));
    }
    return data->fileTimes[0]; //cannot really happen
}

//************* QFileInfo

/*!
    \class QFileInfo
    \reentrant
    \brief The QFileInfo class provides system-independent file information.

    \ingroup io
    \ingroup shared

    QFileInfo provides information about a file's name and position
    (path) in the file system, its access rights and whether it is a
    directory or symbolic link, etc. The file's size and last
    modified/read times are also available. QFileInfo can also be
    used to obtain information about a Qt \l{resource
    system}{resource}.

    A QFileInfo can point to a file with either a relative or an
    absolute file path. Absolute file paths begin with the directory
    separator "/" (or with a drive specification on Windows). Relative
    file names begin with a directory name or a file name and specify
    a path relative to the current working directory. An example of an
    absolute path is the string "/tmp/quartz". A relative path might
    look like "src/fatlib". You can use the function isRelative() to
    check whether a QFileInfo is using a relative or an absolute file
    path. You can call the function makeAbsolute() to convert a
    relative QFileInfo's path to an absolute path.

    The file that the QFileInfo works on is set in the constructor or
    later with setFile(). Use exists() to see if the file exists and
    size() to get its size.

    The file's type is obtained with isFile(), isDir() and
    isSymLink(). The symLinkTarget() function provides the name of the file
    the symlink points to.

    On Unix (including Mac OS X), the symlink has the same size() has
    the file it points to, because Unix handles symlinks
    transparently; similarly, opening a symlink using QFile
    effectively opens the link's target. For example:

    \snippet doc/src/snippets/code/src_corelib_io_qfileinfo.cpp 0

    On Windows, symlinks (shortcuts) are \c .lnk files. The reported
    size() is that of the symlink (not the link's target), and
    opening a symlink using QFile opens the \c .lnk file. For
    example:

    \snippet doc/src/snippets/code/src_corelib_io_qfileinfo.cpp 1

    Elements of the file's name can be extracted with path() and
    fileName(). The fileName()'s parts can be extracted with
    baseName(), suffix() or completeSuffix(). QFileInfo objects to
    directories created by Qt classes will not have a trailing file
    separator. If you wish to use trailing separators in your own file
    info objects, just append one to the file name given to the constructors
    or setFile().

    The file's dates are returned by created(), lastModified() and
    lastRead(). Information about the file's access permissions is
    obtained with isReadable(), isWritable() and isExecutable(). The
    file's ownership is available from owner(), ownerId(), group() and
    groupId(). You can examine a file's permissions and ownership in a
    single statement using the permission() function.

    \section1 Performance Issues

    Some of QFileInfo's functions query the file system, but for
    performance reasons, some functions only operate on the
    file name itself. For example: To return the absolute path of
    a relative file name, absolutePath() has to query the file system.
    The path() function, however, can work on the file name directly,
    and so it is faster.

    \note To speed up performance, QFileInfo caches information about
    the file.

    To speed up performance, QFileInfo caches information about the
    file. Because files can be changed by other users or programs, or
    even by other parts of the same program, there is a function that
    refreshes the file information: refresh(). If you want to switch
    off a QFileInfo's caching and force it to access the file system
    every time you request information from it call setCaching(false).

    \sa QDir, QFile
*/

/*!
    Constructs an empty QFileInfo object.

    Note that an empty QFileInfo object contain no file reference.

    \sa setFile()
*/

QFileInfo::QFileInfo() : d_ptr(new QFileInfoPrivate())
{
}

/*!
    Constructs a new QFileInfo that gives information about the given
    file. The \a file can also include an absolute or relative path.

    \sa setFile(), isRelative(), QDir::setCurrent(), QDir::isRelativePath()
*/

QFileInfo::QFileInfo(const QString &file) : d_ptr(new QFileInfoPrivate())
{
    d_ptr->initFileEngine(file);
}

/*!
    Constructs a new QFileInfo that gives information about file \a
    file.

    If the \a file has a relative path, the QFileInfo will also have a
    relative path.

    \sa isRelative()
*/

QFileInfo::QFileInfo(const QFile &file) : d_ptr(new QFileInfoPrivate())
{
    d_ptr->initFileEngine(file.fileName());
}

/*!
    Constructs a new QFileInfo that gives information about the given
    \a file in the directory \a dir.

    If \a dir has a relative path, the QFileInfo will also have a
    relative path.

    \sa isRelative()
*/

QFileInfo::QFileInfo(const QDir &dir, const QString &file) : d_ptr(new QFileInfoPrivate())
{
    d_ptr->initFileEngine(dir.filePath(file));
}

/*!
    Constructs a new QFileInfo that is a copy of the given \a fileinfo.
*/

QFileInfo::QFileInfo(const QFileInfo &fileinfo) : d_ptr(new QFileInfoPrivate(&fileinfo))
{

}

/*!
    Destroys the QFileInfo and frees its resources.
*/


QFileInfo::~QFileInfo()
{
    delete d_ptr;
    d_ptr = 0;
}

/*!
    \fn bool QFileInfo::operator!=(const QFileInfo &fileinfo)

    Returns true if this QFileInfo object refers to a different file
    than the one specified by \a fileinfo; otherwise returns false.

    \sa operator==()
*/

/*!
    \overload
    \fn bool QFileInfo::operator!=(const QFileInfo &fileinfo) const
*/

/*!
    \overload
*/
bool QFileInfo::operator==(const QFileInfo &fileinfo) const
{
    Q_D(const QFileInfo);
    // ### Qt 5: understand long and short file names on Windows
    // ### (GetFullPathName()).
    if(fileinfo.d_func()->data == d->data)
        return true;
    if(!d->data->fileEngine || !fileinfo.d_func()->data->fileEngine)
        return false;
    if(d->data->fileEngine->caseSensitive() != fileinfo.d_func()->data->fileEngine->caseSensitive())
        return false;
    if(fileinfo.size() == size()) { //if the size isn't the same...
        QString file1 = canonicalFilePath(),
                file2 = fileinfo.canonicalFilePath();
        if(file1.length() == file2.length()) {
            if(!fileinfo.d_func()->data->fileEngine->caseSensitive()) {
                for(int i = 0; i < file1.length(); i++) {
                    if(file1.at(i).toLower() != file2.at(i).toLower())
                        return false;
                }
                return true;
            }
            return (file1 == file2);
        }
    }
    return false;
}

/*!
    Returns true if this QFileInfo object refers to a file in the same
    location as \a fileinfo; otherwise returns false.

    Note that the result of comparing two empty QFileInfo objects,
    containing no file references, is undefined.

    \warning This will not compare two different symbolic links
    pointing to the same file.

    \warning Long and short file names that refer to the same file on Windows
    are treated as if they referred to different files.

    \sa operator!=()
*/
bool QFileInfo::operator==(const QFileInfo &fileinfo)
{
    return const_cast<const QFileInfo *>(this)->operator==(fileinfo);
}

/*!
    Makes a copy of the given \a fileinfo and assigns it to this QFileInfo.
*/

QFileInfo &QFileInfo::operator=(const QFileInfo &fileinfo)
{
    Q_D(QFileInfo);
    qAtomicAssign(d->data, fileinfo.d_func()->data);
    return *this;
}

/*!
    Sets the file that the QFileInfo provides information about to \a
    file.

    The \a file can also include an absolute or relative file path.
    Absolute paths begin with the directory separator (e.g. "/" under
    Unix) or a drive specification (under Windows). Relative file
    names begin with a directory name or a file name and specify a
    path relative to the current directory.

    Example:
    \snippet doc/src/snippets/code/src_corelib_io_qfileinfo.cpp 2

    \sa isRelative(), QDir::setCurrent(), QDir::isRelativePath()
*/

void QFileInfo::setFile(const QString &file)
{
    Q_D(QFileInfo);
    d->initFileEngine(file);
}

/*!
    \overload

    Sets the file that the QFileInfo provides information about to \a
    file.

    If \a file includes a relative path, the QFileInfo will also have
    a relative path.

    \sa isRelative()
*/

void QFileInfo::setFile(const QFile &file)
{
    Q_D(QFileInfo);
    d->initFileEngine(file.fileName());
}

/*!
    \overload

    Sets the file that the QFileInfo provides information about to \a
    file in directory \a dir.

    If \a file includes a relative path, the QFileInfo will also
    have a relative path.

    \sa isRelative()
*/

void QFileInfo::setFile(const QDir &dir, const QString &file)
{
    Q_D(QFileInfo);
    d->initFileEngine(dir.filePath(file));
}

/*!
    Returns an absolute path including the file name.

    The absolute path name consists of the full path and the file
    name. On Unix this will always begin with the root, '/',
    directory. On Windows this will always begin 'D:/' where D is a
    drive letter, except for network shares that are not mapped to a
    drive letter, in which case the path will begin '//sharename/'.
    QFileInfo will uppercase drive letters. Note that QDir does not do
    this. The code snippet below shows this.

    \snippet doc/src/snippets/code/src_corelib_io_qfileinfo.cpp newstuff

    This function returns the same as filePath(), unless isRelative()
    is true. In contrast to canonicalFilePath(), symbolic links or
    redundant "." or ".." elements are not necessarily removed.

    If the QFileInfo is empty it returns QDir::currentPath().

    \sa filePath(), canonicalFilePath(), isRelative()
*/

QString QFileInfo::absoluteFilePath() const
{
    Q_D(const QFileInfo);
    if(!d->data->fileEngine)
        return QLatin1String("");
    return d->getFileName(QAbstractFileEngine::AbsoluteName);
}

/*!
    Returns the canonical path including the file name,
    i.e. an absolute path without symbolic links or redundant "." or ".." elements.

    If the file does not exist, canonicalFilePath() returns an empty string.

    \sa filePath(), absoluteFilePath(), dir()
*/

QString QFileInfo::canonicalFilePath() const
{
    Q_D(const QFileInfo);
    if(!d->data->fileEngine)
        return QLatin1String("");
    return d->getFileName(QAbstractFileEngine::CanonicalName);
}


/*!
    Returns a file's path absolute path. This doesn't include the
    file name.

    On Unix the absolute path will always begin with the root, '/',
    directory. On Windows this will always begin 'D:/' where D is a
    drive letter, except for network shares that are not mapped to a
    drive letter, in which case the path will begin '//sharename/'.

    This function returns the same as filePath(), unless isRelative()
    is true. In contrast to canonicalPath() symbolic links or
    redundant "." or ".." elements are not necessarily removed.

    \warning If the QFileInfo object was created with an empty QString,
              the behavior of this function is undefined.

    \sa absoluteFilePath(), path(), canonicalPath(), fileName(), isRelative()
*/

QString QFileInfo::absolutePath() const
{
    Q_D(const QFileInfo);
    if(!d->data->fileEngine)
        return QLatin1String("");
    return d->getFileName(QAbstractFileEngine::AbsolutePathName);
}

/*!
    Returns the file's path canonical path (excluding the file name),
    i.e. an absolute path without symbolic links or redundant "." or ".." elements.

    If the file does not exist, canonicalPath() returns an empty string.

    \sa path(), absolutePath()
*/

QString QFileInfo::canonicalPath() const
{
    Q_D(const QFileInfo);
    if(!d->data->fileEngine)
        return QLatin1String("");
    return d->getFileName(QAbstractFileEngine::CanonicalPathName);
}


/*!
    Returns the file's path. This doesn't include the file name.

    Note that, if this QFileInfo object is given a path ending in a
    slash, the name of the file is considered empty and this function
    will return the entire path.

    \sa filePath(), absolutePath(), canonicalPath(), dir(), fileName(), isRelative()
*/

QString QFileInfo::path() const
{
    Q_D(const QFileInfo);
    if(!d->data->fileEngine)
        return QLatin1String("");
    return d->getFileName(QAbstractFileEngine::PathName);
}

/*!
    \fn bool QFileInfo::isAbsolute() const

    Returns true if the file path name is absolute, otherwise returns
    false if the path is relative.

    \sa isRelative()
*/

/*!
    Returns true if the file path name is relative, otherwise returns
    false if the path is absolute (e.g. under Unix a path is absolute
    if it begins with a "/").

    \sa isAbsolute()
*/

bool QFileInfo::isRelative() const
{
    Q_D(const QFileInfo);
    if(!d->data->fileEngine)
        return true;
    return d->data->fileEngine->isRelativePath();
}


/*!
    Converts the file's path to an absolute path if it is not already in that form.
    Returns true to indicate that the path was converted; otherwise returns false
    to indicate that the path was already absolute.

    \sa filePath(), isRelative()
*/

bool QFileInfo::makeAbsolute()
{
    Q_D(QFileInfo);
    if(!d->data->fileEngine || !d->data->fileEngine->isRelativePath())
        return false;
    QString absFileName = d->getFileName(QAbstractFileEngine::AbsoluteName);
    d->initFileEngine(absFileName);
    return true;
}

/*!
    Returns true if the file exists; otherwise returns false.

    \note If the file is a symlink that points to a non existing
     file, false is returned.
*/

bool QFileInfo::exists() const
{
    Q_D(const QFileInfo);
    if(!d->data->fileEngine)
        return false;
    return d->getFileFlags(QAbstractFileEngine::ExistsFlag);
}

/*!
    Refreshes the information about the file, i.e. reads in information
    from the file system the next time a cached property is fetched.

   \note On Windows CE, there might be a delay for the file system driver
    to detect changes on the file.
*/

void QFileInfo::refresh()
{
    Q_D(QFileInfo);
    d->reset();
}

/*!
    Returns the file name, including the path (which may be absolute
    or relative).

    \sa absoluteFilePath(), canonicalFilePath(), isRelative()
*/

QString QFileInfo::filePath() const
{
    Q_D(const QFileInfo);
    if(!d->data->fileEngine)
        return QLatin1String("");
    return d->getFileName(QAbstractFileEngine::DefaultName);
}

/*!
    Returns the name of the file, excluding the path.

    Example:
    \snippet doc/src/snippets/code/src_corelib_io_qfileinfo.cpp 3

    Note that, if this QFileInfo object is given a path ending in a
    slash, the name of the file is considered empty.

    \sa isRelative(), filePath(), baseName(), extension()
*/

QString QFileInfo::fileName() const
{
    Q_D(const QFileInfo);
    if(!d->data->fileEngine)
        return QLatin1String("");
    return d->getFileName(QAbstractFileEngine::BaseName);
}

/*!
    \since 4.3
    Returns the name of the bundle.

    On Mac OS X this returns the proper localized name for a bundle if the
    path isBundle(). On all other platforms an empty QString is returned.

    Example:
    \snippet doc/src/snippets/code/src_corelib_io_qfileinfo.cpp 4

    \sa isBundle(), filePath(), baseName(), extension()
*/

QString QFileInfo::bundleName() const
{
    Q_D(const QFileInfo);
    if(!d->data->fileEngine)
        return QLatin1String("");
    return d->getFileName(QAbstractFileEngine::BundleName);
}

/*!
    Returns the base name of the file without the path.

    The base name consists of all characters in the file up to (but
    not including) the \e first '.' character.

    Example:
    \snippet doc/src/snippets/code/src_corelib_io_qfileinfo.cpp 5


    The base name of a file is computed equally on all platforms, independent
    of file naming conventions (e.g., ".bashrc" on Unix has an empty base
    name, and the suffix is "bashrc").

    \sa fileName(), suffix(), completeSuffix(), completeBaseName()
*/

QString QFileInfo::baseName() const
{
    Q_D(const QFileInfo);
    if(!d->data->fileEngine)
        return QLatin1String("");
    return d->getFileName(QAbstractFileEngine::BaseName).section(QLatin1Char('.'), 0, 0);
}

/*!
    Returns the complete base name of the file without the path.

    The complete base name consists of all characters in the file up
    to (but not including) the \e last '.' character.

    Example:
    \snippet doc/src/snippets/code/src_corelib_io_qfileinfo.cpp 6

    \sa fileName(), suffix(), completeSuffix(), baseName()
*/

QString QFileInfo::completeBaseName() const
{
    Q_D(const QFileInfo);
    if(!d->data->fileEngine)
        return QLatin1String("");
    QString name = d->getFileName(QAbstractFileEngine::BaseName);
    int index = name.lastIndexOf(QLatin1Char('.'));
    return (index == -1) ? name : name.left(index);
}

/*!
    Returns the complete suffix of the file.

    The complete suffix consists of all characters in the file after
    (but not including) the first '.'.

    Example:
    \snippet doc/src/snippets/code/src_corelib_io_qfileinfo.cpp 7

    \sa fileName(), suffix(), baseName(), completeBaseName()
*/

QString QFileInfo::completeSuffix() const
{
    Q_D(const QFileInfo);
    if(!d->data->fileEngine)
        return QLatin1String("");
    QString fileName = d->getFileName(QAbstractFileEngine::BaseName);
    int firstDot = fileName.indexOf(QLatin1Char('.'));
    if (firstDot == -1)
        return QLatin1String("");
    return fileName.mid(firstDot + 1);
}

/*!
    Returns the suffix of the file.

    The suffix consists of all characters in the file after (but not
    including) the last '.'.

    Example:
    \snippet doc/src/snippets/code/src_corelib_io_qfileinfo.cpp 8

    The suffix of a file is computed equally on all platforms, independent of
    file naming conventions (e.g., ".bashrc" on Unix has an empty base name,
    and the suffix is "bashrc").

    \sa fileName(), completeSuffix(), baseName(), completeBaseName()
*/

QString QFileInfo::suffix() const
{
    Q_D(const QFileInfo);
    if(!d->data->fileEngine)
        return QLatin1String("");
    QString fileName = d->getFileName(QAbstractFileEngine::BaseName);
    int lastDot = fileName.lastIndexOf(QLatin1Char('.'));
    if (lastDot == -1)
        return QLatin1String("");
    return fileName.mid(lastDot + 1);
}


/*!
  Returns the path of the object's parent directory as a QDir object.

  \bold{Note:} The QDir returned always corresponds to the object's
  parent directory, even if the QFileInfo represents a directory.
  
  For each of the follwing, dir() returns a QDir for
  \c{"~/examples/191697"}.

  \snippet doc/src/snippets/fileinfo/main.cpp 0

  For each of the follwing, dir() returns a QDir for
  \c{"."}.

  \snippet doc/src/snippets/fileinfo/main.cpp 1

  \sa absolutePath(), filePath(), fileName(), isRelative(), absoluteDir()
*/

QDir QFileInfo::dir() const
{
    // ### Qt5: Maybe rename this to parentDirectory(), considering what it actually do?
    return QDir(path());
}

/*!
    Returns the file's absolute path as a QDir object.

    \sa dir(), filePath(), fileName(), isRelative()
*/

QDir QFileInfo::absoluteDir() const
{
    return QDir(absolutePath());
}

#ifdef QT3_SUPPORT
/*!
    Use absoluteDir() or the dir() overload that takes no parameters
    instead.
*/
QDir QFileInfo::dir(bool absPath) const
{
    if(absPath)
        return absoluteDir();
    return dir();
}
#endif //QT3_SUPPORT

/*!
    Returns true if the user can read the file; otherwise returns false.

    \sa isWritable(), isExecutable(), permission()
*/

bool QFileInfo::isReadable() const
{
    Q_D(const QFileInfo);
    if(!d->data->fileEngine)
        return false;
    return d->hasAccess(QFileInfoPrivate::ReadAccess);
}

/*!
    Returns true if the user can write to the file; otherwise returns false.

    \sa isReadable(), isExecutable(), permission()
*/

bool QFileInfo::isWritable() const
{
    Q_D(const QFileInfo);
    if(!d->data->fileEngine)
        return false;
    return d->hasAccess(QFileInfoPrivate::WriteAccess);
}

/*!
    Returns true if the file is executable; otherwise returns false.

    \sa isReadable(), isWritable(), permission()
*/

bool QFileInfo::isExecutable() const
{
    Q_D(const QFileInfo);
    if(!d->data->fileEngine)
        return false;
    return d->hasAccess(QFileInfoPrivate::ExecuteAccess);
}

/*!
    Returns true if this is a `hidden' file; otherwise returns false.

    \bold{Note:} This function returns true for the special entries
    "." and ".." on Unix, even though QDir::entryList threats them as shown.
*/
bool QFileInfo::isHidden() const
{
    Q_D(const QFileInfo);
    if(!d->data->fileEngine)
        return false;
    return d->getFileFlags(QAbstractFileEngine::HiddenFlag);
}

/*!
    Returns true if this object points to a file or to a symbolic
    link to a file. Returns false if the
    object points to something which isn't a file, such as a directory.

    \sa isDir(), isSymLink(), isBundle()
*/

bool QFileInfo::isFile() const
{
    Q_D(const QFileInfo);
    if(!d->data->fileEngine)
        return false;
    return d->getFileFlags(QAbstractFileEngine::FileType);
}

/*!
    Returns true if this object points to a directory or to a symbolic
    link to a directory; otherwise returns false.

    \sa isFile(), isSymLink(), isBundle()
*/

bool QFileInfo::isDir() const
{
    Q_D(const QFileInfo);
    if(!d->data->fileEngine)
        return false;
    return d->getFileFlags(QAbstractFileEngine::DirectoryType);
}


/*!
    \since 4.3
    Returns true if this object points to a bundle or to a symbolic
    link to a bundle on Mac OS X; otherwise returns false.

    \sa isDir(), isSymLink(), isFile()
*/

bool QFileInfo::isBundle() const
{
    Q_D(const QFileInfo);
    if(!d->data->fileEngine)
        return false;
    return d->getFileFlags(QAbstractFileEngine::BundleType);
}

/*!
    Returns true if this object points to a symbolic link (or to a
    shortcut on Windows); otherwise returns false.

    On Unix (including Mac OS X), opening a symlink effectively opens
    the \l{symLinkTarget()}{link's target}. On Windows, it opens the \c
    .lnk file itself.

    Example:

    \snippet doc/src/snippets/code/src_corelib_io_qfileinfo.cpp 9

    \note If the symlink points to a non existing file, exists() returns
     false.

    \sa isFile(), isDir(), symLinkTarget()
*/

bool QFileInfo::isSymLink() const
{
    Q_D(const QFileInfo);
    if(!d->data->fileEngine)
        return false;
    return d->getFileFlags(QAbstractFileEngine::LinkType);
}

/*!
  Returns true if the object points to a directory or to a symbolic
  link to a directory, and that directory is the root directory; otherwise
  returns false.
*/

bool QFileInfo::isRoot() const
{
    Q_D(const QFileInfo);
    if (!d->data->fileEngine)
        return true;
    return d->getFileFlags(QAbstractFileEngine::RootFlag);
}

/*!
    \fn QString QFileInfo::symLinkTarget() const
    \since 4.2

    Returns the absolute path to the file or directory a symlink (or shortcut
    on Windows) points to, or a an empty string if the object isn't a symbolic
    link.

    This name may not represent an existing file; it is only a string.
    QFileInfo::exists() returns true if the symlink points to an
    existing file.

    \sa exists(), isSymLink(), isDir(), isFile()
*/

/*!
    \obsolete

    Use symLinkTarget() instead.
*/
QString QFileInfo::readLink() const
{
    Q_D(const QFileInfo);
    if(!d->data->fileEngine)
        return QLatin1String("");
    return d->getFileName(QAbstractFileEngine::LinkName);
}

/*!
    Returns the owner of the file. On systems where files
    do not have owners, or if an error occurs, an empty string is
    returned.

    This function can be time consuming under Unix (in the order of
    milliseconds).

    \sa ownerId(), group(), groupId()
*/

QString QFileInfo::owner() const
{
    Q_D(const QFileInfo);
    if(!d->data->fileEngine)
        return QLatin1String("");
    return d->data->fileEngine->owner(QAbstractFileEngine::OwnerUser);
}

/*!
    Returns the id of the owner of the file.

    On Windows and on systems where files do not have owners this
    function returns ((uint) -2).

    \sa owner(), group(), groupId()
*/

uint QFileInfo::ownerId() const
{
    Q_D(const QFileInfo);
    if(!d->data->fileEngine)
        return 0;
    return d->data->fileEngine->ownerId(QAbstractFileEngine::OwnerUser);
}

/*!
    Returns the group of the file. On Windows, on systems where files
    do not have groups, or if an error occurs, an empty string is
    returned.

    This function can be time consuming under Unix (in the order of
    milliseconds).

    \sa groupId(), owner(), ownerId()
*/

QString QFileInfo::group() const
{
    Q_D(const QFileInfo);
    if(!d->data->fileEngine)
        return QLatin1String("");
    return d->data->fileEngine->owner(QAbstractFileEngine::OwnerGroup);
}

/*!
    Returns the id of the group the file belongs to.

    On Windows and on systems where files do not have groups this
    function always returns (uint) -2.

    \sa group(), owner(), ownerId()
*/

uint QFileInfo::groupId() const
{
    Q_D(const QFileInfo);
    if(!d->data->fileEngine)
        return 0;
    return d->data->fileEngine->ownerId(QAbstractFileEngine::OwnerGroup);
}

/*!
    Tests for file permissions. The \a permissions argument can be
    several flags of type QFile::Permissions OR-ed together to check
    for permission combinations.

    On systems where files do not have permissions this function
    always returns true.

    Example:
    \snippet doc/src/snippets/code/src_corelib_io_qfileinfo.cpp 10

    \sa isReadable(), isWritable(), isExecutable()
*/

bool QFileInfo::permission(QFile::Permissions permissions) const
{
    Q_D(const QFileInfo);
    if(!d->data->fileEngine)
        return false;
    return d->getFileFlags(QAbstractFileEngine::FileFlags((int)permissions)) == (uint)permissions;
}

/*!
    Returns the complete OR-ed together combination of
    QFile::Permissions for the file.
*/

QFile::Permissions QFileInfo::permissions() const
{
    Q_D(const QFileInfo);
    if(!d->data->fileEngine)
        return 0;
    return QFile::Permissions(d->getFileFlags(QAbstractFileEngine::PermsMask) & QAbstractFileEngine::PermsMask);
}


/*!
    Returns the file size in bytes. If the file does not exist or cannot be
    fetched, 0 is returned.

    \sa exists()
*/

qint64 QFileInfo::size() const
{
    Q_D(const QFileInfo);
    if(!d->data->fileEngine)
        return 0;
    if(!d->data->getCachedFlag(QFileInfoPrivate::CachedSize)) {
        d->data->setCachedFlag(QFileInfoPrivate::CachedSize);
        d->data->fileSize = d->data->fileEngine->size();
    }
    return d->data->fileSize;
}

/*!
    Returns the date and time when the file was created.

    On most Unix systems, this function returns the time of the last
    status change. A status change occurs when the file is created,
    but it also occurs whenever the user writes or sets inode
    information (for example, changing the file permissions).

    If neither creation time nor "last status change" time are not
    available, returns the same as lastModified().

    \sa lastModified() lastRead()
*/

QDateTime QFileInfo::created() const
{
    Q_D(const QFileInfo);
    if(!d->data->fileEngine)
        return QDateTime();
    return d->getFileTime(QAbstractFileEngine::CreationTime);
}

/*!
    Returns the date and time when the file was last modified.

    \sa created() lastRead()
*/

QDateTime QFileInfo::lastModified() const
{
    Q_D(const QFileInfo);
    if(!d->data->fileEngine)
        return QDateTime();
    return d->getFileTime(QAbstractFileEngine::ModificationTime);
}

/*!
    Returns the date and time when the file was last read (accessed).

    On platforms where this information is not available, returns the
    same as lastModified().

    \sa created() lastModified()
*/

QDateTime QFileInfo::lastRead() const
{
    Q_D(const QFileInfo);
    if(!d->data->fileEngine)
        return QDateTime();
    return d->getFileTime(QAbstractFileEngine::AccessTime);
}

/*! \internal
    Detaches all internal data.
*/

void QFileInfo::detach()
{
    Q_D(QFileInfo);
    d->detach();
}

/*!
    Returns true if caching is enabled; otherwise returns false.

    \sa setCaching(), refresh()
*/

bool QFileInfo::caching() const
{
    Q_D(const QFileInfo);
    return d->data->cache_enabled;
}

/*!
    If \a enable is true, enables caching of file information. If \a
    enable is false caching is disabled.

    When caching is enabled, QFileInfo reads the file information from
    the file system the first time it's needed, but generally not
    later.

    Caching is enabled by default.

    \sa refresh(), caching()
*/

void QFileInfo::setCaching(bool enable)
{
    Q_D(QFileInfo);
    detach();
    d->data->cache_enabled = enable;
}

/*!
    \fn QString QFileInfo::baseName(bool complete)

    Use completeBaseName() or the baseName() overload that takes no
    parameters instead.
*/

/*!
    \fn QString QFileInfo::extension(bool complete = true) const

    Use completeSuffix() or suffix() instead.
*/

/*!
    \fn QString QFileInfo::absFilePath() const

    Use absoluteFilePath() instead.
*/

/*!
    \fn QString QFileInfo::dirPath(bool absPath) const

    Use absolutePath() if the absolute path is wanted (\a absPath
    is true) or path() if it's not necessary (\a absPath is false).
*/

/*!
    \fn bool QFileInfo::convertToAbs()

    Use makeAbsolute() instead.
*/

/*!
    \enum QFileInfo::Permission

    \compat

    \value ReadOwner
    \value WriteOwner
    \value ExeOwner
    \value ReadUser
    \value WriteUser
    \value ExeUser
    \value ReadGroup
    \value WriteGroup
    \value ExeGroup
    \value ReadOther
    \value WriteOther
    \value ExeOther
*/

/*!
    \fn bool QFileInfo::permission(PermissionSpec permissions) const
    \compat

    Use permission() instead.
*/

/*!
    \typedef QFileInfoList
    \relates QFileInfo

    Synonym for QList<QFileInfo>.
*/

QT_END_NAMESPACE
