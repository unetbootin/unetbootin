/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#ifndef QFSFILEENGINE_P_H
#define QFSFILEENGINE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qplatformdefs.h"
#include "QtCore/qfsfileengine.h"
#include "private/qabstractfileengine_p.h"
#include <qhash.h>

#ifndef QT_NO_FSFILEENGINE

QT_BEGIN_NAMESPACE

#if defined(Q_OS_WINCE_STD) && _WIN32_WCE < 0x600
#define Q_USE_DEPRECATED_MAP_API 1
#endif

class QFSFileEnginePrivate : public QAbstractFileEnginePrivate
{
    Q_DECLARE_PUBLIC(QFSFileEngine)

public:
#ifdef Q_WS_WIN
    static QByteArray win95Name(const QString &path);
    static QString longFileName(const QString &path);
#endif
    static QString canonicalized(const QString &path);

    QString filePath;
    QByteArray nativeFilePath;
    QIODevice::OpenMode openMode;

    void nativeInitFileName();
    bool nativeOpen(QIODevice::OpenMode openMode);
    bool openFh(QIODevice::OpenMode flags, FILE *fh);
    bool openFd(QIODevice::OpenMode flags, int fd);
    bool nativeClose();
    bool closeFdFh();
    bool nativeFlush();
    bool flushFh();
    qint64 nativeSize() const;
    qint64 sizeFdFh() const;
    qint64 nativePos() const;
    qint64 posFdFh() const;
    bool nativeSeek(qint64);
    bool seekFdFh(qint64);
    qint64 nativeRead(char *data, qint64 maxlen);
    qint64 readFdFh(char *data, qint64 maxlen);
    qint64 nativeReadLine(char *data, qint64 maxlen);
    qint64 readLineFdFh(char *data, qint64 maxlen);
    qint64 nativeWrite(const char *data, qint64 len);
    qint64 writeFdFh(const char *data, qint64 len);
    int nativeHandle() const;
    bool nativeIsSequential() const;
    bool isSequentialFdFh() const;

    uchar *map(qint64 offset, qint64 size, QFile::MemoryMapFlags flags);
    bool unmap(uchar *ptr);
#ifdef Q_WS_WIN
    QHash<uchar *, QPair<int /*offset*/, HANDLE /*handle*/> > maps;
#else
    QHash<uchar *, QPair<int /*offset*/, int /*handle|len*/> > maps;
#endif
    int fd;
    FILE *fh;
#ifdef Q_WS_WIN
    HANDLE fileHandle;
#endif
#ifdef Q_USE_DEPRECATED_MAP_API
    void mapHandleClose();
    HANDLE fileMapHandle;
#endif

    mutable uint is_sequential : 2;
    mutable uint could_stat : 1;
    mutable uint tried_stat : 1;
#ifdef Q_OS_UNIX
    mutable uint need_lstat : 1;
    mutable uint is_link : 1;
#endif
#ifdef Q_WS_WIN
    mutable DWORD fileAttrib;
#else
    mutable QT_STATBUF st;
#endif
    bool doStat() const;
    bool isSymlink() const;

#if defined(Q_OS_WIN32)
    int sysOpen(const QString &, int flags);
#endif

    bool closeFileHandle;
    enum LastIOCommand
    {
        IOFlushCommand,
        IOReadCommand,
        IOWriteCommand
    };
    LastIOCommand  lastIOCommand;
    bool lastFlushFailed;
#if defined(Q_OS_WIN32) || defined(Q_OS_WINCE)
    static void resolveLibs();
    static bool resolveUNCLibs_NT();
    static bool resolveUNCLibs_9x();
    static bool uncListSharesOnServer(const QString &server, QStringList *list);
#endif

protected:
    QFSFileEnginePrivate();

    void init();

#if defined(Q_OS_WIN32) || defined(Q_OS_WINCE)
    QAbstractFileEngine::FileFlags getPermissions() const;
    QString getLink() const;
#endif
};

QT_END_NAMESPACE

#endif // QT_NO_FSFILEENGINE

#endif // QFSFILEENGINE_P_H
