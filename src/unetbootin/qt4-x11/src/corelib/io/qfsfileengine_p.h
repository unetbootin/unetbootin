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

    FILE *fh;
#ifdef Q_WS_WIN
    HANDLE fileHandle;
    QHash<uchar *, QPair<int /*offset*/, HANDLE /*handle*/> > maps;
    mutable int cachedFd;
    mutable DWORD fileAttrib;
#else
    QHash<uchar *, QPair<int /*offset*/, int /*handle|len*/> > maps;
    mutable QT_STATBUF st;
#endif
    int fd;

#ifdef Q_USE_DEPRECATED_MAP_API
    void mapHandleClose();
    HANDLE fileMapHandle;
#endif

    enum LastIOCommand
    {
        IOFlushCommand,
        IOReadCommand,
        IOWriteCommand
    };
    LastIOCommand  lastIOCommand;
    bool lastFlushFailed;
    bool closeFileHandle;

    mutable uint is_sequential : 2;
    mutable uint could_stat : 1;
    mutable uint tried_stat : 1;
#ifdef Q_OS_UNIX
    mutable uint need_lstat : 1;
    mutable uint is_link : 1;
#endif
    bool doStat() const;
    bool isSymlink() const;

#if defined(Q_OS_WIN32)
    int sysOpen(const QString &, int flags);
#endif

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
