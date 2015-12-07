/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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

bool QFileInfoGatherer::fetchedRoot = false;

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
    groupId = getgid();
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
    QExtendedInformation info(fileInfo);
    info.icon = m_iconProvider->icon(fileInfo);
    info.setPermissions(translatePermissions(fileInfo));
    info.displayType = m_iconProvider->type(fileInfo);
#ifndef QT_NO_FILESYSTEMWATCHER
    // ### Not ready to listen all modifications
    #if 0
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
#endif

    if (fileInfo.isSymLink() && m_resolveSymlinks) {
        QFileInfo resolvedInfo(fileInfo.symLinkTarget());
        resolvedInfo = resolvedInfo.canonicalFilePath();
        if (resolvedInfo.exists()) {
            emit nameResolved(fileInfo.filePath(), resolvedInfo.fileName());
        }
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
    //### We test here if the path still exist before adding it in the watcher
    //### because sometime the file is deleted just before enter here so QStringList files is not up to date
    //### It is not a proper fix, perhaps in 4.6 we should have a better way to avoid that
    //### to ensure the gatherer have fresh information
    QFileInfo info(path);
    if (files.isEmpty()
        && !watcher->directories().contains(path)
        && !path.isEmpty()
        && info.exists()
        && !path.startsWith(QLatin1String("//")) /*don't watch UNC path*/) {
        watcher->addPath(path);
    }
#endif

    // List drives
    if (path.isEmpty()) {
#if defined Q_AUTOTEST_EXPORT
        fetchedRoot = true;
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
