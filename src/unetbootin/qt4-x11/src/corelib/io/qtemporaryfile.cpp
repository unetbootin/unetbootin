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

#include "qtemporaryfile.h"

#ifndef QT_NO_TEMPORARYFILE

#include "qplatformdefs.h"
#include "qabstractfileengine.h"
#include "private/qfile_p.h"
#include "private/qabstractfileengine_p.h"
#include "private/qfsfileengine_p.h"

#include <stdlib.h>
#if !defined(Q_OS_WINCE)
#  include <errno.h>
#  include <sys/stat.h>
#  include <sys/types.h>
#endif

#include <stdlib.h>
#include <time.h>
#include <ctype.h>

#if defined(Q_OS_WIN) && !defined(Q_OS_WINCE)
# include <process.h>
# if defined(_MSC_VER) && _MSC_VER >= 1400
#  include <share.h>
# endif
#endif

#if defined(Q_OS_WINCE)
#  include <types.h>
#  include "qfunctions_wince.h"
#endif


QT_BEGIN_NAMESPACE

/*
 * Copyright (c) 1987, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
static int _gettemp(char *path, int *doopen, int domkdir, int slen)
{
	char *start, *trv, *suffp;
	QT_STATBUF sbuf;
	int rval;
#if defined(Q_OS_WIN)
    int pid;
#else
	pid_t pid;
#endif

	if (doopen && domkdir) {
		errno = EINVAL;
		return(0);
	}

	for (trv = path; *trv; ++trv)
		;
	trv -= slen;
	suffp = trv;
	--trv;
	if (trv < path) {
		errno = EINVAL;
		return (0);
	}
#if defined(Q_OS_WIN) && defined(_MSC_VER) && _MSC_VER >= 1400
        pid = _getpid();
#else
        pid = getpid();
#endif
	while (trv >= path && *trv == 'X' && pid != 0) {
		*trv-- = (pid % 10) + '0';
		pid /= 10;
        }

#ifndef S_ISDIR
#define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#endif

        while (trv >= path && *trv == 'X') {
            char c;

            // CHANGE arc4random() -> random()
            pid = (qrand() & 0xffff) % (26+26);
		if (pid < 26)
			c = pid + 'A';
		else
			c = (pid - 26) + 'a';
		*trv-- = c;
	}
	start = trv + 1;

	/*
	 * check the target directory; if you have six X's and it
	 * doesn't exist this runs for a *very* long time.
	 */
	if (doopen || domkdir) {
		for (;; --trv) {
			if (trv <= path)
				break;
			if (*trv == '/') {
				*trv = '\0';
#if defined (Q_OS_WIN) && !defined(Q_OS_WINCE)
                                if (trv - path == 2 && path[1] == ':') {
                                    // Special case for Windows drives
                                    // (e.g., "C:" => "C:\").
                                    // ### Better to use a Windows
                                    // call for this.
									char drive[] = "c:\\";
                                    drive[0] = path[0];
                                    rval = QT_STAT(drive, &sbuf);
                                } else
#endif
				rval = QT_STAT(path, &sbuf);
				*trv = '/';
				if (rval != 0)
					return(0);
				if (!S_ISDIR(sbuf.st_mode)) {
					errno = ENOTDIR;
					return(0);
				}
				break;
			}
		}
	}

	for (;;) {
		if (doopen) {
#if defined(Q_OS_WIN) && !defined(Q_OS_WINCE) && defined(_MSC_VER) && _MSC_VER >= 1400
                        if (_sopen_s(doopen, path, QT_OPEN_CREAT|O_EXCL|QT_OPEN_RDWR|QT_OPEN_BINARY
#ifdef QT_LARGEFILE_SUPPORT
                                     |QT_OPEN_LARGEFILE
#endif
                                     , _SH_DENYNO, _S_IREAD | _S_IWRITE)== 0)
#else
#if defined(Q_OS_WINCE)
            QString targetPath;
            if (QDir::isAbsolutePath(QString::fromLatin1(path)))
                targetPath = QLatin1String(path);
            else
                targetPath = QDir::currentPath().append(QLatin1String("/")) + QLatin1String(path);

            if ((*doopen =
                QT_OPEN(targetPath.toLocal8Bit(), O_CREAT|O_EXCL|O_RDWR
#else
                        if ((*doopen =
                            open(path, QT_OPEN_CREAT|O_EXCL|QT_OPEN_RDWR
#endif
#ifdef QT_LARGEFILE_SUPPORT
                                 |QT_OPEN_LARGEFILE
#endif
#  if defined(Q_OS_WINCE)
                                                             |_O_BINARY
#  elif defined(Q_OS_WIN)
                                 |O_BINARY
#  endif
                                 , 0600)) >= 0)
#endif

                                return(1);
                        if (errno != EEXIST)
                                return(0);
                } else if (domkdir) {
#ifdef Q_OS_WIN
                    if (QT_MKDIR(path) == 0)
#else
                    if (mkdir(path, 0700) == 0)
#endif
                                return(1);
                        if (errno != EEXIST)
                                return(0);
            }
#ifndef Q_OS_WIN
            else if (QT_LSTAT(path, &sbuf))
			return(errno == ENOENT ? 1 : 0);
#else
            if (!QFileInfo(QLatin1String(path)).exists())
                return 1;
#endif

                /* tricky little algorwwithm for backward compatibility */
                for (trv = start;;) {
                        if (!*trv)
                                return (0);
                        if (*trv == 'Z') {
                                if (trv == suffp)
                                        return (0);
                                *trv++ = 'a';
                        } else {
                                if (isdigit(*trv))
                                        *trv = 'a';
                                else if (*trv == 'z')	/* inc from z to A */
                                        *trv = 'A';
                                else {
                                        if (trv == suffp)
                                                return (0);
                                        ++*trv;
                                }
                                break;
                        }
                }
        }
        /*NOTREACHED*/
}

static int qt_mkstemps(char *path, int slen)
{
        int fd = 0;
        return (_gettemp(path, &fd, 0, slen) ? fd : -1);
}

//************* QTemporaryFileEngine
class QTemporaryFileEngine : public QFSFileEngine
{
    Q_DECLARE_PRIVATE(QFSFileEngine)
public:
    QTemporaryFileEngine(const QString &file) : QFSFileEngine(file) { }
    ~QTemporaryFileEngine();

    bool open(QIODevice::OpenMode flags);
    bool remove();
    bool close();
};

QTemporaryFileEngine::~QTemporaryFileEngine()
{
    QFSFileEngine::close();
}

bool QTemporaryFileEngine::open(QIODevice::OpenMode openMode)
{
    Q_D(QFSFileEngine);

    QString qfilename = d->filePath;
    if(!qfilename.contains(QLatin1String("XXXXXX")))
        qfilename += QLatin1String(".XXXXXX");

    int suffixLength = qfilename.length() - (qfilename.lastIndexOf(QLatin1String("XXXXXX"), -1, Qt::CaseSensitive) + 6);
    d->closeFileHandle = true;
    char *filename = qstrdup(qfilename.toLocal8Bit());

#ifndef Q_WS_WIN
    int fd = qt_mkstemps(filename, suffixLength);
    if (fd != -1) {
        // First open the fd as an external file descriptor to
        // initialize the engine properly.
        QFSFileEngine::open(openMode, fd);

        // Allow the engine to close the handle even if it's "external".
        d->closeFileHandle = true;

        // Restore the file names (open() resets them).
        d->filePath = QString::fromLocal8Bit(filename); //changed now!
        d->nativeInitFileName();
        delete [] filename;
        return true;
    }
    delete [] filename;
    setError(errno == EMFILE ? QFile::ResourceError : QFile::OpenError, qt_error_string(errno));
    return false;
#else
    if (!_gettemp(filename, 0, 0, suffixLength)) {
        delete [] filename;
        return false;
    }

    d->filePath = QString::fromLocal8Bit(filename);
    d->nativeInitFileName();
    d->closeFileHandle = true;
    delete [] filename;
    return QFSFileEngine::open(openMode);
#endif
}

bool QTemporaryFileEngine::remove()
{
    Q_D(QFSFileEngine);
    // Since the QTemporaryFileEngine::close() does not really close the file,
    // we must explicitly call QFSFileEngine::close() before we remove it.
    QFSFileEngine::close();
    bool removed = QFSFileEngine::remove();
    d->filePath.clear();
    return removed;
}

bool QTemporaryFileEngine::close()
{
    // Don't close the file, just seek to the front.
    seek(0);
    setError(QFile::UnspecifiedError, QString());
    return true;
}

//************* QTemporaryFilePrivate
class QTemporaryFilePrivate : public QFilePrivate
{
    Q_DECLARE_PUBLIC(QTemporaryFile)

protected:
    QTemporaryFilePrivate();
    ~QTemporaryFilePrivate();

    bool autoRemove;
    QString templateName;
    mutable QTemporaryFileEngine *fileEngine;
};

QTemporaryFilePrivate::QTemporaryFilePrivate() : autoRemove(true), fileEngine(0)
{
}

QTemporaryFilePrivate::~QTemporaryFilePrivate()
{
    delete fileEngine;
    fileEngine = 0;
}

//************* QTemporaryFile

/*!
    \class QTemporaryFile
    \reentrant
    \brief The QTemporaryFile class is an I/O device that operates on temporary files.

    \ingroup io
    \mainclass

    QTemporaryFile is used to create unique temporary files safely.
    The file itself is created by calling open(). The name of the
    temporary file is guaranteed to be unique (i.e., you are
    guaranteed to not overwrite an existing file), and the file will
    subsequently be removed upon destruction of the QTemporaryFile
    object. This is an important technique that avoids data
    corruption for applications that store data in temporary files.
    The file name is either auto-generated, or created based on a
    template, which is passed to QTemporaryFile's constructor.

    Example:

    \snippet doc/src/snippets/code/src_corelib_io_qtemporaryfile.cpp 0

    Reopening a QTemporaryFile after calling close() is safe. For as long as
    the QTemporaryFile object itself is not destroyed, the unique temporary
    file will exist and be kept open internally by QTemporaryFile.

    The file name of the temporary file can be found by calling fileName().
    Note that this is only defined while the file is open; the function returns
    an empty string before the file is opened and after it is closed.

    A temporary file will have some static part of the name and some
    part that is calculated to be unique. The default filename \c
    qt_temp will be placed into the temporary path as returned by
    QDir::tempPath(). If you specify your own filename, a relative
    file path will not be placed in the temporary directory by
    default, but be relative to the current working directory.

    Specified filenames can contain the following template \c XXXXXX
    (six upper case "X" characters), which will be replaced by the
    auto-generated portion of the filename. Note that the template is
    case sensitive. If the template is not present in the filename,
    QTemporaryFile appends the generated part to the filename given.

    \sa QDir::tempPath(), QFile
*/

#ifdef QT_NO_QOBJECT
QTemporaryFile::QTemporaryFile()
    : QFile(*new QTemporaryFilePrivate)
{
    Q_D(QTemporaryFile);
    d->templateName = QDir::tempPath() + QLatin1String("/qt_temp.XXXXXX");
}

QTemporaryFile::QTemporaryFile(const QString &templateName)
    : QFile(*new QTemporaryFilePrivate)
{
    Q_D(QTemporaryFile);
    d->templateName = templateName;
}

#else
/*!
    Constructs a QTemporaryFile in QDir::tempPath(), using the file template
    "qt_temp.XXXXXX". The file is stored in the system's temporary directory.

    \sa setFileTemplate(), QDir::tempPath()
*/
QTemporaryFile::QTemporaryFile()
    : QFile(*new QTemporaryFilePrivate, 0)
{
    Q_D(QTemporaryFile);
    d->templateName = QDir::tempPath() + QLatin1String("/qt_temp.XXXXXX");
}

/*!
    Constructs a QTemporaryFile with a template filename of \a
    templateName. Upon opening the temporary file this will be used to create
    a unique filename.

    If the \a templateName does not contain XXXXXX it will automatically be
    appended and used as the dynamic portion of the filename.

    If \a templateName is a relative path, the path will be relative to the
    current working directory. You can use QDir::tempPath() to construct \a
    templateName if you want use the system's temporary directory.

    \sa open(), fileTemplate()
*/
QTemporaryFile::QTemporaryFile(const QString &templateName)
    : QFile(*new QTemporaryFilePrivate, 0)
{
    setFileTemplate(templateName);
}

/*!
    Constructs a QTemporaryFile (with the given \a parent) in
    QDir::tempPath(), using the file template "qt_temp.XXXXXX".

    \sa setFileTemplate()
*/
QTemporaryFile::QTemporaryFile(QObject *parent)
    : QFile(*new QTemporaryFilePrivate, parent)
{
    Q_D(QTemporaryFile);
    d->templateName = QDir::tempPath() + QLatin1String("/qt_temp.XXXXXX");
}

/*!
    Constructs a QTemporaryFile with a template filename of \a
    templateName and the specified \a parent.
    Upon opening the temporary file this will be used to
    create a unique filename.

    If the \a templateName does not contain XXXXXX it will automatically be
    appended and used as the dynamic portion of the filename.

    If \a templateName is a relative path, the path will be relative to the
    current working directory. You can use QDir::tempPath() to construct \a
    templateName if you want use the system's temporary directory.

    \sa open(), fileTemplate()
*/
QTemporaryFile::QTemporaryFile(const QString &templateName, QObject *parent)
    : QFile(*new QTemporaryFilePrivate, parent)
{
    setFileTemplate(templateName);
}
#endif

/*!
    Destroys the temporary file object, the file is automatically
    closed if necessary and if in auto remove mode it will
    automatically delete the file.

    \sa autoRemove()
*/
QTemporaryFile::~QTemporaryFile()
{
    Q_D(QTemporaryFile);
    close();
    if (!d->fileName.isEmpty() && d->autoRemove)
        remove();
}

/*!
  \fn bool QTemporaryFile::open()

  A QTemporaryFile will always be opened in QIODevice::ReadWrite mode,
  this allows easy access to the data in the file. This function will
  return true upon success and will set the fileName() to the unique
  filename used.

  \sa fileName()
*/

/*!
   Returns true if the QTemporaryFile is in auto remove
   mode. Auto-remove mode will automatically delete the filename from
   disk upon destruction. This makes it very easy to create your
   QTemporaryFile object on the stack, fill it with data, read from
   it, and finally on function return it will automatically clean up
   after itself.

   Auto-remove is on by default.

   \sa setAutoRemove(), remove()
*/
bool QTemporaryFile::autoRemove() const
{
    Q_D(const QTemporaryFile);
    return d->autoRemove;
}

/*!
    Sets the QTemporaryFile into auto-remove mode if \a b is true.

    Auto-remove is on by default.

    \sa autoRemove(), remove()
*/
void QTemporaryFile::setAutoRemove(bool b)
{
    Q_D(QTemporaryFile);
    d->autoRemove = b;
}

/*!
   Returns the complete unique filename backing the QTemporaryFile
   object. This string is null before the QTemporaryFile is opened,
   afterwards it will contain the fileTemplate() plus
   additional characters to make it unique.

   \sa fileTemplate()
*/

QString QTemporaryFile::fileName() const
{
    if(!isOpen())
        return QString();
    return fileEngine()->fileName(QAbstractFileEngine::DefaultName);
}

/*!
  Returns the set file template. The default file template will be
  called qt_temp and be placed in QDir::tempPath().

  \sa setFileTemplate()
*/
QString QTemporaryFile::fileTemplate() const
{
    Q_D(const QTemporaryFile);
    return d->templateName;
}

/*!
   Sets the static portion of the file name to \a name. If the file
   template ends in XXXXXX that will automatically be replaced with
   the unique part of the filename, otherwise a filename will be
   determined automatically based on the static portion specified.

    If \a name contains a relative file path, the path will be relative to the
    current working directory. You can use QDir::tempPath() to construct \a
    name if you want use the system's temporary directory.

   \sa fileTemplate()
*/
void QTemporaryFile::setFileTemplate(const QString &name)
{
    Q_ASSERT(!isOpen());
    Q_D(QTemporaryFile);
    fileEngine()->setFileName(name);
    d->templateName = name;
}

/*!
  \fn QTemporaryFile *QTemporaryFile::createLocalFile(const QString &fileName)
  \overload

  Works on the given \a fileName rather than an existing QFile
  object.
*/


/*!
  If \a file is not on a local disk, a temporary file is created
  on a local disk, \a file is copied into the temporary local file,
  and a pointer to the temporary local file is returned. If \a file
  is already on a local disk, a copy is not created and 0 is returned.
*/
QTemporaryFile *QTemporaryFile::createLocalFile(QFile &file)
{
    if (QAbstractFileEngine *engine = file.fileEngine()) {
        if(engine->fileFlags(QAbstractFileEngine::FlagsMask) & QAbstractFileEngine::LocalDiskFlag)
            return 0; //local already
        //cache
        bool wasOpen = file.isOpen();
        qint64 old_off = 0;
        if(wasOpen)
            old_off = file.pos();
        else
            file.open(QIODevice::ReadOnly);
        //dump data
        QTemporaryFile *ret = new QTemporaryFile;
        ret->open();
        file.seek(0);
        char buffer[1024];
        while(true) {
            qint64 len = file.read(buffer, 1024);
            if(len < 1)
                break;
            ret->write(buffer, len);
        }
        ret->seek(0);
        //restore
        if(wasOpen)
            file.seek(old_off);
        else
            file.close();
        //done
        return ret;
    }
    return 0;
}

/*!
   \internal
*/

QAbstractFileEngine *QTemporaryFile::fileEngine() const
{
    Q_D(const QTemporaryFile);
    if(!d->fileEngine)
        d->fileEngine = new QTemporaryFileEngine(d->templateName);
    return d->fileEngine;
}

/*!
   \reimp

    Creates a unique file name for the temporary file, and opens it.  You can
    get the unique name later by calling fileName(). The file is guaranteed to
    have been created by this function (i.e., it has never existed before).
*/
bool QTemporaryFile::open(OpenMode flags)
{
    Q_D(QTemporaryFile);
    if (!d->fileName.isEmpty()) {
        setOpenMode(flags);
        return true;
    }

    if (QFile::open(flags)) {
        d->fileName = d->fileEngine->fileName(QAbstractFileEngine::DefaultName);
        return true;
    }
    return false;
}

#endif // QT_NO_TEMPORARYFILE

QT_END_NAMESPACE
