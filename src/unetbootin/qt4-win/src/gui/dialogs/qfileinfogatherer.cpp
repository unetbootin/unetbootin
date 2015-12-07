/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#include "qfileinfogatherer_p.h"
#include <qdebug.h>
#include <qfsfileengine.h>
#include <qdiriterator.h>
#ifndef Q_OS_WIN
#include <unistd.h>
#include <sys/types.h>
#endif

QT_BEGIN_NAMESPACE

#ifndef QT_NO_FILESYSTEMMODEL

#if defined Q_AUTOTEST_EXPORT
    Q_AUTOTEST_EXPORT bool QFileInfoGatherer_fetchedRoot = false;
#endif

/*!
    Creates thread
*/
QFileInfoGatherer::QFileInfoGatherer(QObject *parent)
    : QThread(parent), abort(false),
#ifndef QT_NO_FILESYSTEMWATCHER
      watcher(0),
#endif
      m_resolveSymlinks(false), m_iconProvider(&defaultProvider)
{
#ifndef Q_OS_WIN
    userId = getuid();
    groupId = getuid();
#else
    m_resolveSymlinks = true;
#endif
#ifndef QT_NO_FILESYSTEMWATCHER
    watcher = new QFileSystemWatcher(this);
    connect(watcher, SIGNAL(directoryChanged(const QString &)), this, SLOT(list(const QString &)));
    connect(watcher, SIGNAL(fileChanged(const QString &)), this, SLOT(updateFile(const QString &)));
#endif
    start(LowPriority);
}

/*!
    Destroys thread
*/
QFileInfoGatherer::~QFileInfoGatherer()
{
    mutex.lock();
    abort = true;
    condition.wakeOne();
    mutex.unlock();
    wait();
}

void QFileInfoGatherer::setResolveSymlinks(bool enable)
{
    Q_UNUSED(enable);
#ifdef Q_OS_WIN
    mutex.lock();
    m_resolveSymlinks = enable;
    mutex.unlock();
#endif
}

bool QFileInfoGatherer::resolveSymlinks() const
{
    return m_resolveSymlinks;
}

void QFileInfoGatherer::setIconProvider(QFileIconProvider *provider)
{
    mutex.lock();
    m_iconProvider = provider;
    mutex.unlock();
}

QFileIconProvider *QFileInfoGatherer::iconProvider() const
{
    return m_iconProvider;
}

/*!
    Fetch extended information for all \a files in \a path

    \sa updateFile(), update(), resolvedName()
*/
void QFileInfoGatherer::fetchExtendedInformation(const QString &path, const QStringList &files)
{
    mutex.lock();
    // See if we already have this dir/file in our que
    int loc = this->path.lastIndexOf(path);
    while (loc > 0)  {
        if (this->files.at(loc) == files) {
            mutex.unlock();
            return;
        }
        loc = this->path.lastIndexOf(path, loc - 1);
    }
    this->path.push(path);
    this->files.push(files);
    condition.wakeAll();
    mutex.unlock();
}

/*!
    Fetch extended information for all \a filePath

    \sa fetchExtendedInformation()
*/
void QFileInfoGatherer::updateFile(const QString &filePath)
{
    QString dir = filePath.mid(0, filePath.lastIndexOf(QDir::separator()));
    QString fileName = filePath.mid(dir.length() + 1);
    fetchExtendedInformation(dir, QStringList(fileName));
}

/*
    List all files in \a directoryPath

    \sa listed()
*/
void QFileInfoGatherer::clear()
{
#ifndef QT_NO_FILESYSTEMWATCHER
    mutex.lock();
    watcher->removePaths(watcher->files());
    watcher->removePaths(watcher->directories());
    mutex.unlock();
#endif
}

/*
    List all files in \a directoryPath

    \sa listed()
*/
void QFileInfoGatherer::list(const QString &directoryPath)
{
    fetchExtendedInformation(directoryPath, QStringList());
}

/*
    Until aborted wait to fetch a directory or files
*/
void QFileInfoGatherer::run()
{
    forever {
        bool updateFiles = false;
        mutex.lock();
        if (abort) {
            mutex.unlock();
            return;
        }
        if (this->path.isEmpty())
            condition.wait(&mutex);
        QString path;
        QStringList list;
        if (!this->path.isEmpty()) {
            path = this->path.first();
            list = this->files.first();
            this->path.pop_front();
            this->files.pop_front();
            updateFiles = true;
        }
        mutex.unlock();
        if (updateFiles) getFileInfos(path, list);
    }
}

/*
    QFileInfo::permissions is different depending upon your platform.

    "normalize this" so they can mean the same to us.
*/
QFile::Permissions QFileInfoGatherer::translatePermissions(const QFileInfo &fileInfo) const {
    QFile::Permissions permissions = fileInfo.permissions();
#ifdef Q_OS_WIN
    return permissions;
#else
    QFile::Permissions p = permissions;
    p &= ~(QFile::ReadUser|QFile::WriteUser|QFile::ExeUser);
    if (                                     permissions & QFile::ReadOther
        || (fileInfo.ownerId() == userId  && permissions & QFile::ReadOwner)
        || (fileInfo.groupId() == groupId && permissions & QFile::ReadGroup))
        p |= QFile::ReadUser;

    if (                                     permissions & QFile::WriteOther
        || (fileInfo.ownerId() == userId  && permissions & QFile::WriteOwner)
        || (fileInfo.groupId() == groupId && permissions & QFile::WriteGroup))
        p |= QFile::WriteUser;

    if (                                     permissions & QFile::ExeOther
        || (fileInfo.ownerId() == userId  && permissions & QFile::ExeOwner)
        || (fileInfo.groupId() == groupId && permissions & QFile::ExeGroup))
        p |= QFile::ExeUser;
    return p;
#endif
}

QExtendedInformation QFileInfoGatherer::getInfo(const QFileInfo &fileInfo) const
{
    QExtendedInformation info;
    info.size = fileInfo.size();
    QFSFileEngine fe(fileInfo.absoluteFilePath());
    info.caseSensitive = fe.caseSensitive();
    info.lastModified = fileInfo.lastModified();
    info.permissions = translatePermissions(fileInfo);
    info.isHidden = fileInfo.isHidden();
    info.isSymLink = fileInfo.isSymLink();
    info.icon = m_iconProvider->icon(fileInfo);
    info.displayType = m_iconProvider->type(fileInfo);
    if (fileInfo.isDir()) info.fileType = QExtendedInformation::Dir;
    if (fileInfo.isFile()) info.fileType = QExtendedInformation::File;

#ifndef QT_NO_FILESYSTEMWATCHER
    // Enable the next two commented out lines to get updates when the file sizes change...
    if (!fileInfo.exists() && !fileInfo.isSymLink()) {
        info.size = -1;
        //watcher->removePath(fileInfo.absoluteFilePath());
    } else {
        if (!fileInfo.absoluteFilePath().isEmpty() && fileInfo.exists() && fileInfo.isReadable()
            && !watcher->files().contains(fileInfo.absoluteFilePath())) {
            //watcher->addPath(fileInfo.absoluteFilePath());
        }
    }
#endif

    if (fileInfo.isSymLink() && m_resolveSymlinks) {
        QFileInfo resolvedInfo(fileInfo.symLinkTarget());
        resolvedInfo = resolvedInfo.canonicalFilePath();
        if (resolvedInfo.exists()) {
            emit nameResolved(fileInfo.filePath(), resolvedInfo.fileName());
        } else {
            info.fileType = QExtendedInformation::System;
        }
    }

    if (!fileInfo.exists() && fileInfo.isSymLink()) {
        info.fileType = QExtendedInformation::System;
    }

    return info;
}

QString QFileInfoGatherer::translateDriveName(const QFileInfo &drive) const
{
    QString driveName = drive.absoluteFilePath();
#if defined(Q_OS_WIN) && !defined(Q_OS_WINCE)
    if (driveName.startsWith(QLatin1Char('/'))) // UNC host
        return drive.fileName();
    if (driveName.endsWith(QLatin1Char('/')))
        driveName.chop(1);
#endif
    return driveName;
}

/*
    Get specific file info's, batch the files so update when we have 100
    items and every 200ms after that
 */
void QFileInfoGatherer::getFileInfos(const QString &path, const QStringList &files)
{
#ifndef QT_NO_FILESYSTEMWATCHER
    if (files.isEmpty()
        && !watcher->directories().contains(path)
        && !path.isEmpty()) {
        watcher->addPath(path);
    }
#endif

    // List drives
    if (path.isEmpty()) {
#if defined Q_AUTOTEST_EXPORT
        QFileInfoGatherer_fetchedRoot = true;
#endif
        QFileInfoList infoList;
        if (files.isEmpty()) {
            infoList = QDir::drives();
        } else {
            for (int i = 0; i < files.count(); ++i)
                infoList << QFileInfo(files.at(i));
        }
        for (int i = infoList.count() - 1; i >= 0; --i) {
            QString driveName = translateDriveName(infoList.at(i));
            QList<QPair<QString,QFileInfo> > updatedFiles;
            updatedFiles.append(QPair<QString,QFileInfo>(driveName, infoList.at(i)));
            emit updates(path, updatedFiles);
        }
        return;
    }

    QTime base = QTime::currentTime();
    QFileInfo fileInfo;
    bool firstTime = true;
    QList<QPair<QString, QFileInfo> > updatedFiles;
    QStringList filesToCheck = files;

    QString itPath = QDir::fromNativeSeparators(files.isEmpty() ? path : QLatin1String(""));
    QDirIterator dirIt(itPath, QDir::AllEntries | QDir::System | QDir::Hidden);
    QStringList allFiles;
    while(!abort && dirIt.hasNext()) {
        dirIt.next();
        fileInfo = dirIt.fileInfo();
        allFiles.append(fileInfo.fileName());
	fetch(fileInfo, base, firstTime, updatedFiles, path);
    }
    if (!allFiles.isEmpty())
        emit newListOfFiles(path, allFiles);

    QStringList::const_iterator filesIt = filesToCheck.constBegin();
    while(!abort && filesIt != filesToCheck.constEnd()) {
        fileInfo.setFile(path + QDir::separator() + *filesIt);
        ++filesIt;
        fetch(fileInfo, base, firstTime, updatedFiles, path);
    }
    if (!updatedFiles.isEmpty())
        emit updates(path, updatedFiles);
}

void QFileInfoGatherer::fetch(const QFileInfo &fileInfo, QTime &base, bool &firstTime, QList<QPair<QString, QFileInfo> > &updatedFiles, const QString &path) {
    updatedFiles.append(QPair<QString, QFileInfo>(fileInfo.fileName(), fileInfo));
    QTime current = QTime::currentTime();
    if ((firstTime && updatedFiles.count() > 100) || base.msecsTo(current) > 1000) {
        emit updates(path, updatedFiles);
        updatedFiles.clear();
        base = current;
        firstTime = false;
    }
}

#endif // QT_NO_FILESYSTEMMODEL

QT_END_NAMESPACE
