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

#ifndef QFILEINFOGATHERER_H
#define QFILEINFOGATHERER_H

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

#include <qthread.h>
#include <qmutex.h>
#include <qwaitcondition.h>
#include <qfilesystemwatcher.h>
#include <qfileiconprovider.h>
#include <qpair.h>
#include <qdatetime.h>
#include <qstack.h>
#include <qdir.h>

QT_BEGIN_NAMESPACE

class QExtendedInformation {
public:
    enum Type { Dir, File, System };

    QExtendedInformation() : size(0), fileType(System), isHidden(false),
                             isSymLink(false), caseSensitive(true) {}

    qint64 size;
    QString displayType;
    QIcon icon;
    QDateTime lastModified;
    QFile::Permissions permissions;
    Type fileType;
    bool isHidden : 1;
    bool isSymLink : 1;
    bool caseSensitive : 1;

    inline bool isDir() { return fileType == Dir; }
    inline bool isFile() { return fileType == File; }
    inline bool isSystem() { return fileType == System; }

    bool operator ==(const QExtendedInformation &fileInfo) const {
       return fileInfo.size == size
       && fileInfo.displayType == displayType
       && fileInfo.lastModified == lastModified
       && fileInfo.permissions == permissions
       && fileInfo.fileType == fileType
       && fileInfo.isHidden == isHidden
       && fileInfo.isSymLink == isSymLink
       && fileInfo.caseSensitive == caseSensitive;
    }
    void operator =(const QExtendedInformation &fileInfo) {
        size = fileInfo.size;
        displayType = fileInfo.displayType;
        icon = fileInfo.icon;
        lastModified = fileInfo.lastModified;
        permissions = fileInfo.permissions;
        fileType = fileInfo.fileType;
        isHidden = fileInfo.isHidden;
        isSymLink = fileInfo.isSymLink;
        caseSensitive = fileInfo.caseSensitive;
    }
};

class QFileIconProvider;

#ifndef QT_NO_FILESYSTEMMODEL

class Q_AUTOTEST_EXPORT QFileInfoGatherer : public QThread
{
Q_OBJECT

Q_SIGNALS:
    void updates(const QString &directory, const QList<QPair<QString, QFileInfo> > &updates);
    void newListOfFiles(const QString &directory, const QStringList &listOfFiles) const;
    void nameResolved(const QString &fileName, const QString &resolvedName) const;

public:
    QFileInfoGatherer(QObject *parent = 0);
    ~QFileInfoGatherer();

    void clear();
    QExtendedInformation getInfo(const QFileInfo &info) const;

public Q_SLOTS:
    void list(const QString &directoryPath);
    void fetchExtendedInformation(const QString &path, const QStringList &files);
    void updateFile(const QString &path);
    void setResolveSymlinks(bool enable);
    bool resolveSymlinks() const;
    void setIconProvider(QFileIconProvider *provider);
    QFileIconProvider *iconProvider() const;

protected:
    void run();
    void getFileInfos(const QString &path, const QStringList &files);

private:
    void fetch(const QFileInfo &info, QTime &base, bool &firstTime, QList<QPair<QString, QFileInfo> > &updatedFiles, const QString &path);
    QString translateDriveName(const QFileInfo &drive) const;
    QFile::Permissions translatePermissions(const QFileInfo &fileInfo) const;

    QMutex mutex;
    QWaitCondition condition;
    bool abort;

    QStack<QString> path;
    QStack<QStringList> files;

#ifndef QT_NO_FILESYSTEMWATCHER
    QFileSystemWatcher *watcher;
#endif
    bool m_resolveSymlinks;
    QFileIconProvider *m_iconProvider;
    QFileIconProvider defaultProvider;
#ifndef Q_OS_WIN
    uint userId;
    uint groupId;
#endif
};
#endif // QT_NO_FILESYSTEMMODEL


QT_END_NAMESPACE
#endif // QFILEINFOGATHERER_H

