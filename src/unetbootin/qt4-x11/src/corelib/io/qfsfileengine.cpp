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

#include "qfsfileengine_p.h"
#include "qfsfileengine_iterator_p.h"
#include "qdatetime.h"
#include "qdiriterator.h"
#include "qset.h"

#ifndef QT_NO_FSFILEENGINE

#if !defined(Q_OS_WINCE)
#include <errno.h>
#endif
#include <stdio.h>

QT_BEGIN_NAMESPACE

#ifdef Q_OS_WIN
#  ifndef S_ISREG
#    define S_ISREG(x)   (((x) & S_IFMT) == S_IFREG)
#  endif
#  ifndef S_ISCHR
#    define S_ISCHR(x)   (((x) & S_IFMT) == S_IFCHR)
#  endif
#  ifndef S_ISFIFO
#    define S_ISFIFO(x) false
#  endif
#  ifndef S_ISSOCK
#    define S_ISSOCK(x) false
#  endif
#  ifndef INVALID_FILE_ATTRIBUTES
#    define INVALID_FILE_ATTRIBUTES (DWORD (-1))
#  endif
#endif


/*! \class QFSFileEngine
    \brief The QFSFileEngine class implements Qt's default file engine.
    \since 4.1

    This class is part of the file engine framework in Qt. If you only want to
    access files or directories, use QFile, QFileInfo or QDir instead.

    QFSFileEngine is the default file engine for accessing regular files. It
    is provided for convenience; by subclassing this class, you can alter its
    behavior slightly, without having to write a complete QAbstractFileEngine
    subclass. To install your custom file engine, you must also subclass
    QAbstractFileEngineHandler and create an instance of your handler.

    It can also be useful to create a QFSFileEngine object directly if you
    need to use the local file system inside QAbstractFileEngine::create(), in
    order to avoid recursion (as higher-level classes tend to call
    QAbstractFileEngine::create()).
*/

//**************** QFSFileEnginePrivate
QFSFileEnginePrivate::QFSFileEnginePrivate() : QAbstractFileEnginePrivate()
{
    init();
}

/*!
    \internal
*/
void QFSFileEnginePrivate::init()
{
    is_sequential = 0;
    tried_stat = 0;
#ifdef Q_OS_UNIX
    need_lstat = 1;
    is_link = 0;
#endif
    openMode = QIODevice::NotOpen;
    fd = -1;
    fh = 0;
    lastIOCommand = IOFlushCommand;
    lastFlushFailed = false;
    closeFileHandle = false;
#ifdef Q_OS_WIN
    fileAttrib = INVALID_FILE_ATTRIBUTES;
    fileHandle = INVALID_HANDLE_VALUE;
    cachedFd = -1;
#endif
#ifdef Q_USE_DEPRECATED_MAP_API
    fileMapHandle = INVALID_HANDLE_VALUE;
#endif
}

/*!
    \internal

    Returns the canonicalized form of \a path (i.e., with all symlinks
    resolved, and all redundant path elements removed.
*/
QString QFSFileEnginePrivate::canonicalized(const QString &path)
{
    if (path.isEmpty())
        return path;

    QFileInfo fi;
    const QChar slash(QLatin1Char('/'));
    QString tmpPath = path;
    int separatorPos = 0;
    QSet<QString> nonSymlinks;
    QSet<QString> known;

    known.insert(path);
    do {
#ifdef Q_OS_WIN
        // UNC, skip past the first two elements
        if (separatorPos == 0 && tmpPath.startsWith(QLatin1String("//")))
            separatorPos = tmpPath.indexOf(slash, 2);
        if (separatorPos != -1)
#endif
        separatorPos = tmpPath.indexOf(slash, separatorPos + 1);
        QString prefix = separatorPos == -1 ? tmpPath : tmpPath.left(separatorPos);
        if (!nonSymlinks.contains(prefix)) {
            fi.setFile(prefix);
            if (fi.isSymLink()) {
                QString target = fi.symLinkTarget();
                if (separatorPos != -1) {
                    if (fi.isDir() && !target.endsWith(slash))
                        target.append(slash);
                    target.append(tmpPath.mid(separatorPos));
                }
                tmpPath = QDir::cleanPath(target);
                separatorPos = 0;

                if (known.contains(tmpPath))
                    return QString();
                known.insert(tmpPath);
            } else {
                nonSymlinks.insert(prefix);
            }
        }
    } while (separatorPos != -1);

    return QDir::cleanPath(tmpPath);
}

/*!
    Constructs a QFSFileEngine for the file name \a file.
*/
QFSFileEngine::QFSFileEngine(const QString &file) : QAbstractFileEngine(*new QFSFileEnginePrivate)
{
    Q_D(QFSFileEngine);
    d->filePath = QDir::fromNativeSeparators(file);
    d->nativeInitFileName();
}

/*!
    Constructs a QFSFileEngine.
*/
QFSFileEngine::QFSFileEngine() : QAbstractFileEngine(*new QFSFileEnginePrivate)
{
}

/*!
    \internal
*/
QFSFileEngine::QFSFileEngine(QFSFileEnginePrivate &dd)
    : QAbstractFileEngine(dd)
{
}

/*!
    Destructs the QFSFileEngine.
*/
QFSFileEngine::~QFSFileEngine()
{
    Q_D(QFSFileEngine);
    if (d->closeFileHandle) {
        if (d->fh) {
            int ret;
            do {
                ret = fclose(d->fh);
            } while (ret == EOF && errno == EINTR);
        } else if (d->fd != -1) {
            int ret;
            do {
                ret = QT_CLOSE(d->fd);
            } while (ret == -1 && errno == EINTR);
        }
    }
    QList<uchar*> keys = d->maps.keys();
    for (int i = 0; i < keys.count(); ++i)
        unmap(keys.at(i));
}

/*!
    \reimp
*/
void QFSFileEngine::setFileName(const QString &file)
{
    Q_D(QFSFileEngine);
    d->init();
    d->filePath = QDir::fromNativeSeparators(file);
    d->nativeInitFileName();
}

/*!
    \reimp
*/
bool QFSFileEngine::open(QIODevice::OpenMode openMode)
{
    Q_D(QFSFileEngine);
    if (d->filePath.isEmpty()) {
        qWarning("QFSFileEngine::open: No file name specified");
        setError(QFile::OpenError, QLatin1String("No file name specified"));
        return false;
    }

    // Append implies WriteOnly.
    if (openMode & QFile::Append)
        openMode |= QFile::WriteOnly;

    // WriteOnly implies Truncate if neither ReadOnly nor Append are sent.
    if ((openMode & QFile::WriteOnly) && !(openMode & (QFile::ReadOnly | QFile::Append)))
        openMode |= QFile::Truncate;

    d->openMode = openMode;
    d->lastFlushFailed = false;
    d->tried_stat = 0;
    d->fh = 0;
    d->fd = -1;

    return d->nativeOpen(openMode);
}

/*!
    Opens the file handle \a fh in \a openMode mode. Returns true on
    success; otherwise returns false.
*/
bool QFSFileEngine::open(QIODevice::OpenMode openMode, FILE *fh)
{
    Q_D(QFSFileEngine);

    // Append implies WriteOnly.
    if (openMode & QFile::Append)
        openMode |= QFile::WriteOnly;

    // WriteOnly implies Truncate if neither ReadOnly nor Append are sent.
    if ((openMode & QFile::WriteOnly) && !(openMode & (QFile::ReadOnly | QFile::Append)))
        openMode |= QFile::Truncate;

    d->openMode = openMode;
    d->lastFlushFailed = false;
    d->closeFileHandle = false;
    d->nativeFilePath.clear();
    d->filePath.clear();
    d->tried_stat = 0;
    d->fd = -1;

    return d->openFh(openMode, fh);
}

/*!
    Opens the file handle \a fh using the open mode \a flags.
*/
bool QFSFileEnginePrivate::openFh(QIODevice::OpenMode openMode, FILE *fh)
{
    Q_Q(QFSFileEngine);
    this->fh = fh;
    fd = -1;

    // Seek to the end when in Append mode.
    if (openMode & QIODevice::Append) {
        int ret;
        do {
            ret = QT_FSEEK(fh, 0, SEEK_END);
        } while (ret == -1 && errno == EINTR);

        if (ret == -1) {
            q->setError(errno == EMFILE ? QFile::ResourceError : QFile::OpenError,
                        qt_error_string(int(errno)));
            return false;
        }
    }

    return true;
}

/*!
    Opens the file descriptor \a fd in \a openMode mode. Returns true
    on success; otherwise returns false.
*/
bool QFSFileEngine::open(QIODevice::OpenMode openMode, int fd)
{
    Q_D(QFSFileEngine);

    // Append implies WriteOnly.
    if (openMode & QFile::Append)
        openMode |= QFile::WriteOnly;

    // WriteOnly implies Truncate if neither ReadOnly nor Append are sent.
    if ((openMode & QFile::WriteOnly) && !(openMode & (QFile::ReadOnly | QFile::Append)))
        openMode |= QFile::Truncate;

    d->lastFlushFailed = false;
    d->closeFileHandle = false;
    d->nativeFilePath.clear();
    d->filePath.clear();
    d->fh = 0;
    d->fd = -1;
    d->tried_stat = 0;

    return d->openFd(openMode, fd);
}


/*!
    Opens the file descriptor \a fd to the file engine, using the open mode \a
    flags.
*/
bool QFSFileEnginePrivate::openFd(QIODevice::OpenMode openMode, int fd)
{
    Q_Q(QFSFileEngine);
    this->fd = fd;
    fh = 0;

    // Seek to the end when in Append mode.
    if (openMode & QFile::Append) {
        int ret;
        do {
            ret = QT_LSEEK(fd, 0, SEEK_END);
        } while (ret == -1 && errno == EINTR);

        if (ret == -1) {
            q->setError(errno == EMFILE ? QFile::ResourceError : QFile::OpenError,
                        qt_error_string(int(errno)));
            return false;
        }
    }

    return true;
}

/*!
    \reimp
*/
bool QFSFileEngine::close()
{
    Q_D(QFSFileEngine);
    d->openMode = QIODevice::NotOpen;
    return d->nativeClose();
}

/*!
    \internal
*/
bool QFSFileEnginePrivate::closeFdFh()
{
    Q_Q(QFSFileEngine);
    if (fd == -1 && !fh)
        return false;

    // Flush the file if it's buffered, and if the last flush didn't fail.
    bool flushed = !fh || (!lastFlushFailed && q->flush());
    bool closed = true;
    tried_stat = 0;

    // Close the file if we created the handle.
    if (closeFileHandle) {
        int ret;
        do {
            if (fh) {
                // Close buffered file.
                ret = fclose(fh) != 0 ? -1 : 0;
            } else {
                // Close unbuffered file.
                ret = QT_CLOSE(fd);
            }
        } while (ret == -1 && errno == EINTR);

        // We must reset these guys regardless; calling close again after a
        // failed close causes crashes on some systems.
        fh = 0;
        fd = -1;
        closed = (ret == 0);
    }

    // Report errors.
    if (!flushed || !closed) {
        if (flushed) {
            // If not flushed, we want the flush error to fall through.
            q->setError(QFile::UnspecifiedError, qt_error_string(errno));
        }
        return false;
    }

    return true;
}

/*!
    \reimp
*/
bool QFSFileEngine::flush()
{
    Q_D(QFSFileEngine);
    if ((d->openMode & QIODevice::WriteOnly) == 0) {
        // Nothing in the write buffers, so flush succeeds in doing
        // nothing.
        return true;
    }
    return d->nativeFlush();
}

/*!
    \internal
*/
bool QFSFileEnginePrivate::flushFh()
{
    Q_Q(QFSFileEngine);

    // Never try to flush again if the last flush failed. Otherwise you can
    // get crashes on some systems (AIX).
    if (lastFlushFailed)
        return false;

    int ret = fflush(fh);

    lastFlushFailed = (ret != 0);
    lastIOCommand = QFSFileEnginePrivate::IOFlushCommand;

    if (ret != 0) {
        q->setError(errno == ENOSPC ? QFile::ResourceError : QFile::WriteError,
                    qt_error_string(errno));
        return false;
    }
    return true;
}

/*!
    \reimp
*/
qint64 QFSFileEngine::size() const
{
    Q_D(const QFSFileEngine);
    return d->nativeSize();
}

/*!
    \internal
*/
qint64 QFSFileEnginePrivate::sizeFdFh() const
{
    Q_Q(const QFSFileEngine);
    // ### Fix this function, it should not stat unless the file is closed.
    QT_STATBUF st;
    int ret = 0;
    const_cast<QFSFileEngine *>(q)->flush();
    if (fh && nativeFilePath.isEmpty()) {
        // Buffered stdlib mode.
        // ### This should really be an ftell
        ret = QT_FSTAT(QT_FILENO(fh), &st);
    } else if (fd == -1) {
        // Stateless stat.
        ret = QT_STAT(nativeFilePath.constData(), &st);
    } else {
        // Unbuffered stdio mode.
        ret = QT_FSTAT(fd, &st);
    }
    if (ret == -1)
        return 0;
    return st.st_size;
}

/*!
    \reimp
*/
qint64 QFSFileEngine::pos() const
{
    Q_D(const QFSFileEngine);
    return d->nativePos();
}

/*!
    \internal
*/
qint64 QFSFileEnginePrivate::posFdFh() const
{
    if (fh)
        return qint64(QT_FTELL(fh));
    return QT_LSEEK(fd, 0, SEEK_CUR);
}

/*!
    \reimp
*/
bool QFSFileEngine::seek(qint64 pos)
{
    Q_D(QFSFileEngine);
    return d->nativeSeek(pos);
}

/*!
    \internal
*/
bool QFSFileEnginePrivate::seekFdFh(qint64 pos)
{
    Q_Q(QFSFileEngine);

    // On Windows' stdlib implementation, the results of calling fread and
    // fwrite are undefined if not called either in sequence, or if preceded
    // with a call to fflush().
    if (lastIOCommand != QFSFileEnginePrivate::IOFlushCommand && !q->flush())
        return false;

    if (fh) {
        // Buffered stdlib mode.
        int ret;
        do {
            ret = QT_FSEEK(fh, QT_OFF_T(pos), SEEK_SET);
        } while (ret == -1 && errno == EINTR);

        if (ret == -1) {
            q->setError(QFile::ReadError, qt_error_string(int(errno)));
            return false;
        }
    } else {
        // Unbuffered stdio mode.
        if (QT_LSEEK(fd, pos, SEEK_SET) == -1) {
            qWarning("QFile::at: Cannot set file position %lld", pos);
            q->setError(QFile::PositionError, qt_error_string(errno));
            return false;
        }
    }
    return true;
}

/*!
    \reimp
*/
int QFSFileEngine::handle() const
{
    Q_D(const QFSFileEngine);
    return d->nativeHandle();
}

/*!
    \reimp
*/
qint64 QFSFileEngine::read(char *data, qint64 maxlen)
{
    Q_D(QFSFileEngine);

    // On Windows' stdlib implementation, the results of calling fread and
    // fwrite are undefined if not called either in sequence, or if preceded
    // with a call to fflush().
    if (d->lastIOCommand != QFSFileEnginePrivate::IOReadCommand) {
        flush();
        d->lastIOCommand = QFSFileEnginePrivate::IOReadCommand;
    }

    return d->nativeRead(data, maxlen);
}

/*!
    \internal
*/
qint64 QFSFileEnginePrivate::readFdFh(char *data, qint64 len)
{
    Q_Q(QFSFileEngine);

    // Buffered stdlib mode.
    if (fh) {
        qint64 readBytes = 0;
        qint64 read = 0;
        int retry = 0;

        // Read in blocks of 4k to avoid platform limitations (Windows
        // commonly bails out if you read or write too large blocks at once).
        qint64 bytesToRead;
        do {
            if (retry == 1)
                retry = 2;

            bytesToRead = qMin<qint64>(4096, len - read);
            do {
                readBytes = fread(data + read, 1, size_t(bytesToRead), fh);
            } while (readBytes == 0 && !feof(fh) && errno == EINTR);

            if (readBytes > 0) {
                read += readBytes;
            } else if (!retry && feof(fh)) {
                // Synchronize and try again (just once though).
                if (++retry == 1)
                    QT_FSEEK(fh, QT_FTELL(fh), SEEK_SET);
            }
        } while (retry == 1 || (readBytes == bytesToRead && read < len));

        // Return the number of bytes read, or if nothing was read, return -1
        // if an error occurred, or 0 if we detected EOF.
        if (read == 0) {
            q->setError(QFile::ReadError, qt_error_string(int(errno)));
            if (!feof(fh))
                read = -1;
        }
        return read;
    }

    // Unbuffered stdio mode.
    qint64 ret = 0;
    if (len) {
        int result;
        qint64 read = 0;
        errno = 0;

        // Read in blocks of 4k to avoid platform limitations (Windows
        // commonly bails out if you read or write too large blocks at once).
        do {
            qint64 bytesToRead = qMin<qint64>(4096, len - read);
            do {
                result = QT_READ(fd, data + read, int(bytesToRead));
            } while (result == -1 && errno == EINTR);
            if (result > 0)
                read += result;
        } while (result > 0 && read < len);

        // Return the number of bytes read, or if nothing was read, return -1
        // if an error occurred.
        if (read > 0) {
            ret += read;
        } else if (read == 0 && result < 0) {
            ret = -1;
            q->setError(QFile::ReadError, qt_error_string(errno));
        }
    }
    return ret;
}

/*!
    \reimp
*/
qint64 QFSFileEngine::readLine(char *data, qint64 maxlen)
{
    Q_D(QFSFileEngine);

    // On Windows' stdlib implementation, the results of calling fread and
    // fwrite are undefined if not called either in sequence, or if preceded
    // with a call to fflush().
    if (d->lastIOCommand != QFSFileEnginePrivate::IOReadCommand) {
        flush();
        d->lastIOCommand = QFSFileEnginePrivate::IOReadCommand;
    }

    return d->nativeReadLine(data, maxlen);
}

/*!
    \internal
*/
qint64 QFSFileEnginePrivate::readLineFdFh(char *data, qint64 maxlen)
{
    Q_Q(QFSFileEngine);
    if (!fh)
        return q->QAbstractFileEngine::readLine(data, maxlen);

    QT_OFF_T oldPos = 0;
#ifdef Q_OS_WIN
    bool seq = q->isSequential();
    if (!seq)
#endif
        oldPos = QT_FTELL(fh);

    // QIODevice::readLine() passes maxlen - 1 to QFile::readLineData()
    // because it has made space for the '\0' at the end of data.  But fgets
    // does the same, so we'd get two '\0' at the end - passing maxlen + 1
    // solves this.
    if (!fgets(data, int(maxlen + 1), fh)) {
        if (!feof(fh))
            q->setError(QFile::ReadError, qt_error_string(int(errno)));
        return -1;              // error
    }

#ifdef Q_OS_WIN
    if (seq)
        return qstrlen(data);
#endif

    qint64 lineLength = QT_FTELL(fh) - oldPos;
    return lineLength > 0 ? lineLength : qstrlen(data);
}

/*!
    \reimp
*/
qint64 QFSFileEngine::write(const char *data, qint64 len)
{
    Q_D(QFSFileEngine);

    // On Windows' stdlib implementation, the results of calling fread and
    // fwrite are undefined if not called either in sequence, or if preceded
    // with a call to fflush().
    if (d->lastIOCommand != QFSFileEnginePrivate::IOWriteCommand) {
        flush();
        d->lastIOCommand = QFSFileEnginePrivate::IOWriteCommand;
    }

    return d->nativeWrite(data, len);
}

/*!
    \internal
*/
qint64 QFSFileEnginePrivate::writeFdFh(const char *data, qint64 len)
{
    Q_Q(QFSFileEngine);
    qint64 result;
    qint64 written = 0;

    do {
        // Write blocks of 4k to avoid platform limitations (Windows commonly
        // bails out if you read or write too large blocks at once).
        qint64 bytesToWrite = qMin<qint64>(4096, len - written);
        if (fh) {
            do {
                // Buffered stdlib mode.
                result = qint64(fwrite(data + written, 1, size_t(bytesToWrite), fh));
            } while (result == 0 && errno == EINTR);
            if (bytesToWrite > 0 && result == 0)
                result = -1;
        } else {
            do {
                // Unbuffered stdio mode.
                result = QT_WRITE(fd, data + written, bytesToWrite);
            } while (result == -1 && errno == EINTR);
        }
        if (result > 0)
            written += qint64(result);
    } while (written < len && result > 0);

    // If we read anything, return that with success. Otherwise, set an error,
    // and return the last return value.
    if (result > 0)
        return written;
    q->setError(errno == ENOSPC ? QFile::ResourceError : QFile::WriteError, qt_error_string(errno));
    return result;
}

/*!
    \internal
*/
QAbstractFileEngine::Iterator *QFSFileEngine::beginEntryList(QDir::Filters filters, const QStringList &filterNames)
{
    return new QFSFileEngineIterator(filters, filterNames);
}

/*!
    \internal
*/
QAbstractFileEngine::Iterator *QFSFileEngine::endEntryList()
{
    return 0;
}

/*!
    \internal
*/
QStringList QFSFileEngine::entryList(QDir::Filters filters, const QStringList &filterNames) const
{
    return QAbstractFileEngine::entryList(filters, filterNames);
}

/*!
    \reimp
*/
bool QFSFileEngine::isSequential() const
{
    Q_D(const QFSFileEngine);
    if (d->is_sequential == 0)
        d->is_sequential = d->nativeIsSequential() ? 1 : 2;
    return d->is_sequential == 1;
}

/*!
    \internal
*/
bool QFSFileEnginePrivate::isSequentialFdFh() const
{
    if (!tried_stat)
        doStat();
    if (could_stat) {
#ifdef Q_OS_UNIX
        return (st.st_mode & S_IFMT) != S_IFREG;
        // ### WINDOWS!
#endif
    }
    return true;
}

/*!
    \reimp
*/
bool QFSFileEngine::extension(Extension extension, const ExtensionOption *option, ExtensionReturn *output)
{
    Q_D(QFSFileEngine);
    if (extension == AtEndExtension && d->fh && isSequential())
        return feof(d->fh);

    if (extension == MapExtension) {
        const MapExtensionOption *options = (MapExtensionOption*)(option);
        MapExtensionReturn *returnValue = static_cast<MapExtensionReturn*>(output);
        returnValue->address = d->map(options->offset, options->size, options->flags);
        return (returnValue->address != 0);
    }
    if (extension == UnMapExtension) {
        UnMapExtensionOption *options = (UnMapExtensionOption*)option;
        return d->unmap(options->address);
    }

    return false;
}

/*!
    \reimp
*/
bool QFSFileEngine::supportsExtension(Extension extension) const
{
    Q_D(const QFSFileEngine);
    if (extension == AtEndExtension && d->fh && isSequential())
        return true;
    if (extension == FastReadLineExtension && d->fh)
        return true;
    if (extension == FastReadLineExtension && d->fd != -1 && isSequential())
        return true;
    if (extension == UnMapExtension || extension == MapExtension)
        return true;
    return false;
}

QT_END_NAMESPACE

#endif // QT_NO_FSFILEENGINE
