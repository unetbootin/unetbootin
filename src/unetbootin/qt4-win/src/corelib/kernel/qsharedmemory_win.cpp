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

#include "qsharedmemory.h"
#include "qsharedmemory_p.h"
#include "qsystemsemaphore.h"
#include <qdebug.h>

QT_BEGIN_NAMESPACE

#ifndef QT_NO_SHAREDMEMORY

QSharedMemoryPrivate::QSharedMemoryPrivate() : QObjectPrivate(),
        memory(0), size(0), error(QSharedMemory::NoError),
           systemSemaphore(QString()), lockedByMe(false), hand(0)
{
}

void QSharedMemoryPrivate::setErrorString(const QString &function)
{
    BOOL windowsError = GetLastError();
    if (windowsError == 0)
        return;
    switch (windowsError) {
    case ERROR_ALREADY_EXISTS:
        error = QSharedMemory::AlreadyExists;
        errorString = QSharedMemory::tr("%1: already exists").arg(function);
    break;
    case ERROR_FILE_NOT_FOUND:
#ifdef Q_OS_WINCE
        // This happens on CE only if no file is present as CreateFileMappingW
        // bails out with this error code
    case ERROR_INVALID_PARAMETER:
#endif
        error = QSharedMemory::NotFound;
        errorString = QSharedMemory::tr("%1: doesn't exists").arg(function);
        break;
    case ERROR_COMMITMENT_LIMIT:
        error = QSharedMemory::InvalidSize;
        errorString = QSharedMemory::tr("%1: invalid size").arg(function);
        break;
    case ERROR_NO_SYSTEM_RESOURCES:
    case ERROR_NOT_ENOUGH_MEMORY:
        error = QSharedMemory::OutOfResources;
        errorString = QSharedMemory::tr("%1: out of resources").arg(function);
        break;
    case ERROR_ACCESS_DENIED:
        error = QSharedMemory::PermissionDenied;
        errorString = QSharedMemory::tr("%1: permission denied").arg(function);
        break;
    default:
        errorString = QSharedMemory::tr("%1: unknown error %2").arg(function).arg(windowsError);
        error = QSharedMemory::UnknownError;
#if defined QSHAREDMEMORY_DEBUG
        qDebug() << errorString << "key" << key;
#endif
    }
}

HANDLE QSharedMemoryPrivate::handle()
{
    if (!hand) {
        QString function = QLatin1String("QSharedMemory::handle");
        QString safeKey = makePlatformSafeKey(key);
        if (safeKey.isEmpty()) {
            error = QSharedMemory::KeyError;
            errorString = QSharedMemory::tr("%1: unable to make key").arg(function);
            return false;
        }
#ifndef Q_OS_WINCE
    QT_WA({
            hand = OpenFileMappingW(FILE_MAP_ALL_ACCESS, false, (TCHAR*)safeKey.utf16());
        }, {
            hand = OpenFileMappingA(FILE_MAP_ALL_ACCESS, false, safeKey.toLocal8Bit().constData());
        });
#else
        // This works for opening a mapping too, but always opens it with read/write access in
        // attach as it seems.
        hand = CreateFileMappingW(INVALID_HANDLE_VALUE,
               0, PAGE_READWRITE, 0, 0, (TCHAR*)safeKey.utf16());
#endif
        if (!hand) {
            setErrorString(function);
            return false;
        }
    }
    return hand;
}

bool QSharedMemoryPrivate::cleanHandle()
{
    if (hand != 0 && !CloseHandle(hand)) {
        hand = 0;
        return false;
        setErrorString(QLatin1String("QSharedMemory::cleanHandle"));
    }
    hand = 0;
    return true;
}

bool QSharedMemoryPrivate::create(int size)
{
    // Get a windows acceptable key
    QString safeKey = makePlatformSafeKey(key);
    QString function = QLatin1String("QSharedMemory::create");
    if (safeKey.isEmpty()) {
        error = QSharedMemory::KeyError;
        errorString = QSharedMemory::tr("%1: key error").arg(function);
        return false;
    }

    // Create the file mapping.
    QT_WA( {
        hand = CreateFileMappingW(INVALID_HANDLE_VALUE,
               0, PAGE_READWRITE, 0, size, (TCHAR*)safeKey.utf16());
    }, {
        hand = CreateFileMappingA(INVALID_HANDLE_VALUE,
               0, PAGE_READWRITE, 0, size, safeKey.toLocal8Bit().constData());
    } );
    setErrorString(function);

    // hand is valid when it already exists unlike unix so explicitly check
    if (error == QSharedMemory::AlreadyExists || !hand)
        return false;

    return true;
}

bool QSharedMemoryPrivate::attach(QSharedMemory::AccessMode mode)
{
    // Grab a pointer to the memory block
    int permissions = (mode == QSharedMemory::ReadOnly ? FILE_MAP_READ : FILE_MAP_ALL_ACCESS);
    memory = (void *)MapViewOfFile(handle(), permissions, 0, 0, 0);
    if (0 == memory) {
        setErrorString(QLatin1String("QSharedMemory::attach"));
        cleanHandle();
        return false;
    }

    // Grab the size of the memory we have been given (a multiple of 4K on windows)
    MEMORY_BASIC_INFORMATION info;
    if (!VirtualQuery(memory, &info, sizeof(info))) {
        // Windows doesn't set an error code on this one,
        // it should only be a kernel memory error.
        error = QSharedMemory::UnknownError;
        errorString = QSharedMemory::tr("%1: size query failed").arg(QLatin1String("QSharedMemory::attach: "));
        return false;
    }
    size = info.RegionSize;

    return true;
}

bool QSharedMemoryPrivate::detach()
{
    // umap memory
    if (!UnmapViewOfFile(memory)) {
        setErrorString(QLatin1String("QSharedMemory::detach"));
        return false;
    }
    memory = 0;

    // close handle
    return cleanHandle();
}

#endif //QT_NO_SHAREDMEMORY


QT_END_NAMESPACE
