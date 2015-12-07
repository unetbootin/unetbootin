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

#include "qfilesystemwatcher.h"
#include "qfilesystemwatcher_win_p.h"

#ifndef QT_NO_FILESYSTEMWATCHER

#include <qdebug.h>
#include <qfileinfo.h>
#include <qstringlist.h>
#include <qset.h>
#include <qdatetime.h>
#include <qdir.h>

QT_BEGIN_NAMESPACE

QWindowsFileSystemWatcherEngine::QWindowsFileSystemWatcherEngine()
    : msg(0)
{
    HANDLE h = QT_WA_INLINE(CreateEventW(0, false, false, 0),
                                CreateEventA(0, false, false, 0));
    if (h != INVALID_HANDLE_VALUE) {
        handles.reserve(MAXIMUM_WAIT_OBJECTS);
        handles.append(h);
    }
}

QWindowsFileSystemWatcherEngine::~QWindowsFileSystemWatcherEngine()
{
    if (handles.isEmpty())
        return;

    stop();
    wait();

    CloseHandle(handles.at(0));
    handles[0] = INVALID_HANDLE_VALUE;

    foreach (HANDLE h, handles) {
        if (h == INVALID_HANDLE_VALUE)
            continue;
        FindCloseChangeNotification(h);
    }
}

void QWindowsFileSystemWatcherEngine::run()
{
    QMutexLocker locker(&mutex);
    forever {
        QVector<HANDLE> handlesCopy = handles;
        locker.unlock();
        // qDebug() << "QWindowsFileSystemWatcherEngine: waiting on" << handlesCopy.count() << "handles";
        DWORD r = WaitForMultipleObjects(handlesCopy.count(), handlesCopy.constData(), false, INFINITE);
        locker.relock();
        do {
            if (r == WAIT_OBJECT_0) {
                int m = msg;
                msg = 0;
                if (m == 'q') {
                    // qDebug() << "thread told to quit";
                    return;
                }
                if (m != '@')  {
                    qDebug("QWindowsFileSystemWatcherEngine: unknown message '%c' send to thread", char(m));
                }
                break;
            } else if (r > WAIT_OBJECT_0 && r < WAIT_OBJECT_0 + uint(handlesCopy.count())) {
                int at = r - WAIT_OBJECT_0;
                Q_ASSERT(at < handlesCopy.count());
                HANDLE handle = handlesCopy.at(at);

                // When removing a path, FindCloseChangeNotification might actually fire a notification
                // for some reason, so we must check if the handle exist in the handles vector
                if (handles.contains(handle)) {
                    // qDebug("Acknowledged handle: %d, %p", at, handle);
                    if (!FindNextChangeNotification(handle)) {
                        qErrnoWarning("QFileSystemWatcher: FindNextChangeNotification failed");
                    }

                    QHash<QString, PathInfo> &h = pathInfoForHandle[handle];
                    QMutableHashIterator<QString, PathInfo> it(h);
                    while (it.hasNext()) {
                        QHash<QString, PathInfo>::iterator x = it.next();
                        QString absolutePath = x.value().absolutePath;
                        QFileInfo fileInfo(x.value().path);
                        // qDebug() << "checking" << x.key();
                        if (!fileInfo.exists()) {
                            // qDebug() << x.key() << "removed!";
                            if (x.value().isDir)
                                emit directoryChanged(x.value().path, true);
                            else
                                emit fileChanged(x.value().path, true);
                            h.erase(x);

                            // close the notification handle if the directory has been removed
                            if (h.isEmpty()) {
                                // qDebug() << "Thread closing handle" << handle;
                                FindCloseChangeNotification(handle);    // This one might generate a notification

                                int indexOfHandle = handles.indexOf(handle);
                                Q_ASSERT(indexOfHandle != -1);
                                handles.remove(indexOfHandle);

                                handleForDir.remove(absolutePath);
                                // h is now invalid
                            }
                        } else if (x.value().isDir) {
                            // qDebug() << x.key() << "directory changed!";
                            emit directoryChanged(x.value().path, false);
                            x.value() = fileInfo;
                        } else if (x.value() != fileInfo) {
                            // qDebug() << x.key() << "file changed!";
                            emit fileChanged(x.value().path, false);
                            x.value() = fileInfo;
                        }
                    }
                }
            } else {
                // qErrnoWarning("QFileSystemWatcher: error while waiting for change notification");
                break;  // avoid endless loop
            }
            handlesCopy = handles;
            r = WaitForMultipleObjects(handlesCopy.count(), handlesCopy.constData(), false, 0);
        } while (r != WAIT_TIMEOUT);
    }
}

QStringList QWindowsFileSystemWatcherEngine::addPaths(const QStringList &paths,
                                                      QStringList *files,
                                                      QStringList *directories)
{
    if (handles.isEmpty() || handles.count() == MAXIMUM_WAIT_OBJECTS)
        return paths;

    QMutexLocker locker(&mutex);

    QStringList p = paths;
    QMutableListIterator<QString> it(p);
    while (it.hasNext()) {
        QString path = it.next();
        QString normalPath = path;
        if ((normalPath.endsWith(QLatin1Char('/')) || normalPath.endsWith(QLatin1Char('\\')))
#ifdef Q_OS_WINCE
            && normalPath.size() > 1)
#else
            )
#endif
            normalPath.chop(1);
        QFileInfo fileInfo(normalPath.toLower());
        if (!fileInfo.exists())
            continue;

        bool isDir = fileInfo.isDir();
        if (isDir) {
            if (directories->contains(path))
                continue;
        } else {
            if (files->contains(path))
                continue;
        }

        const QString absolutePath = isDir ? fileInfo.absoluteFilePath() : fileInfo.absolutePath();
        const uint flags = isDir
            ? (FILE_NOTIFY_CHANGE_DIR_NAME
               | FILE_NOTIFY_CHANGE_FILE_NAME)
            : (FILE_NOTIFY_CHANGE_DIR_NAME
               | FILE_NOTIFY_CHANGE_FILE_NAME
               | FILE_NOTIFY_CHANGE_ATTRIBUTES
               | FILE_NOTIFY_CHANGE_SIZE
               | FILE_NOTIFY_CHANGE_LAST_WRITE
               | FILE_NOTIFY_CHANGE_SECURITY);

        Handle handle = handleForDir.value(absolutePath);
        if (handle.handle == INVALID_HANDLE_VALUE || handle.flags != flags) {
            QT_WA({
                    handle.handle = FindFirstChangeNotificationW((TCHAR *) QDir::toNativeSeparators(absolutePath).utf16(),
                                                      false, flags);
            },{
                    handle.handle = FindFirstChangeNotificationA(QDir::toNativeSeparators(absolutePath).toLocal8Bit(),
                                                      false, flags);
            })
            handle.flags = flags;
            if (handle.handle == INVALID_HANDLE_VALUE)
                continue;
            // qDebug() << "Added handle" << handle << "for" << absolutePath << "to watch" << fileInfo.absoluteFilePath();
            handles.append(handle.handle);
            handleForDir.insert(absolutePath, handle);
        }

        PathInfo pathInfo;
        pathInfo.absolutePath = absolutePath;
        pathInfo.isDir = isDir;
        pathInfo.path = path;
        pathInfo = fileInfo;
        QHash<QString, PathInfo> &h = pathInfoForHandle[handle.handle];
        if (!h.contains(fileInfo.absoluteFilePath())) {
            pathInfoForHandle[handle.handle].insert(fileInfo.absoluteFilePath(), pathInfo);
            if (isDir)
                directories->append(path);
            else
                files->append(path);
        }

        it.remove();
    }

    if (!isRunning()) {
        msg = '@';
        start();
    } else {
        wakeup();
    }

    return p;
}

QStringList QWindowsFileSystemWatcherEngine::removePaths(const QStringList &paths,
                                                         QStringList *files,
                                                         QStringList *directories)
{
    QMutexLocker locker(&mutex);

    QStringList p = paths;
    QMutableListIterator<QString> it(p);
    while (it.hasNext()) {
        QString path = it.next();
        QString normalPath = path;
        if (normalPath.endsWith(QLatin1Char('/')) || normalPath.endsWith(QLatin1Char('\\')))
            normalPath.chop(1);
        QFileInfo fileInfo(normalPath.toLower());

        QString absolutePath = fileInfo.absoluteFilePath();
        Handle handle = handleForDir.value(absolutePath);
        if (handle.handle == INVALID_HANDLE_VALUE) {
            // perhaps path is a file?
            absolutePath = fileInfo.absolutePath();
            handle = handleForDir.value(absolutePath);
            if (handle.handle == INVALID_HANDLE_VALUE)
                continue;
        }

        QHash<QString, PathInfo> &h = pathInfoForHandle[handle.handle];
        if (h.remove(fileInfo.absoluteFilePath())) {
            // ###
            files->removeAll(path);
            directories->removeAll(path);

            if (h.isEmpty()) {
                // qDebug() << "Closing handle" << handle;
                FindCloseChangeNotification(handle.handle);    // This one might generate a notification

                int indexOfHandle = handles.indexOf(handle.handle);
                Q_ASSERT(indexOfHandle != -1);
                handles.remove(indexOfHandle);

                handleForDir.remove(absolutePath);
                // h is now invalid

                it.remove();
            }
        }
    }

    if (handleForDir.isEmpty()) {
        stop();
        locker.unlock();
        wait();
        locker.relock();
    } else {
        wakeup();
    }

    return p;
}

void QWindowsFileSystemWatcherEngine::stop()
{
    msg = 'q';
    SetEvent(handles.at(0));
}

void QWindowsFileSystemWatcherEngine::wakeup()
{
    msg = '@';
    SetEvent(handles.at(0));
}

QT_END_NAMESPACE
#endif // QT_NO_FILESYSTEMWATCHER
