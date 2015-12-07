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

#ifndef QFILEINFO_H
#define QFILEINFO_H

#include <QtCore/qfile.h>
#include <QtCore/qlist.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Core)

class QDir;
class QDateTime;
class QFileInfoPrivate;

class Q_CORE_EXPORT QFileInfo
{
public:
    QFileInfo();
    QFileInfo(const QString &file);
    QFileInfo(const QFile &file);
    QFileInfo(const QDir &dir, const QString &file);
    QFileInfo(const QFileInfo &fileinfo);
    ~QFileInfo();

    QFileInfo &operator=(const QFileInfo &fileinfo);
    bool operator==(const QFileInfo &fileinfo); // 5.0 - remove me
    bool operator==(const QFileInfo &fileinfo) const;
    inline bool operator!=(const QFileInfo &fileinfo) { return !(operator==(fileinfo)); } // 5.0 - remove me
    inline bool operator!=(const QFileInfo &fileinfo) const { return !(operator==(fileinfo)); }

    void setFile(const QString &file);
    void setFile(const QFile &file);
    void setFile(const QDir &dir, const QString &file);
    bool exists() const;
    void refresh();

    QString filePath() const;
    QString absoluteFilePath() const;
    QString canonicalFilePath() const;
    QString fileName() const;
    QString baseName() const;
    QString completeBaseName() const;
    QString suffix() const;
    QString bundleName() const;
    QString completeSuffix() const;

    QString path() const;
    QString absolutePath() const;
    QString canonicalPath() const;
    QDir dir() const;
    QDir absoluteDir() const;

    bool isReadable() const;
    bool isWritable() const;
    bool isExecutable() const;
    bool isHidden() const;

    bool isRelative() const;
    inline bool isAbsolute() const { return !isRelative(); }
    bool makeAbsolute();

    bool isFile() const;
    bool isDir() const;
    bool isSymLink() const;
    bool isRoot() const;
    bool isBundle() const;

    QString readLink() const;
    inline QString symLinkTarget() const { return readLink(); }

    QString owner() const;
    uint ownerId() const;
    QString group() const;
    uint groupId() const;

    bool permission(QFile::Permissions permissions) const;
    QFile::Permissions permissions() const;

    qint64 size() const;

    QDateTime created() const;
    QDateTime lastModified() const;
    QDateTime lastRead() const;

    void detach();

    bool caching() const;
    void setCaching(bool on);

#ifdef QT3_SUPPORT
    enum Permission {
        ReadOwner = QFile::ReadOwner, WriteOwner = QFile::WriteOwner, ExeOwner = QFile::ExeOwner,
        ReadUser  = QFile::ReadUser,  WriteUser  = QFile::WriteUser,  ExeUser  = QFile::ExeUser,
        ReadGroup = QFile::ReadGroup, WriteGroup = QFile::WriteGroup, ExeGroup = QFile::ExeGroup,
        ReadOther = QFile::ReadOther, WriteOther = QFile::WriteOther, ExeOther = QFile::ExeOther
    };
    Q_DECLARE_FLAGS(PermissionSpec, Permission)

    inline QT3_SUPPORT QString baseName(bool complete) {
        if(complete)
            return completeBaseName();
        return baseName();
    }
    inline QT3_SUPPORT QString extension(bool complete = true) const {
        if(complete)
            return completeSuffix();
        return suffix();
    }
    inline QT3_SUPPORT QString absFilePath() const { return absoluteFilePath(); }

    inline QT3_SUPPORT QString dirPath(bool absPath = false) const {
        if(absPath)
            return absolutePath();
        return path();
    }
    QT3_SUPPORT QDir dir(bool absPath) const;
    inline QT3_SUPPORT bool convertToAbs() { return makeAbsolute(); }
#if !defined(Q_NO_TYPESAFE_FLAGS)
    inline QT3_SUPPORT bool permission(PermissionSpec permissions) const
    { return permission(QFile::Permissions(static_cast<int>(permissions))); }
#endif
#endif

protected:
    QFileInfoPrivate *d_ptr;
private:
    Q_DECLARE_PRIVATE(QFileInfo)
};
Q_DECLARE_TYPEINFO(QFileInfo, Q_MOVABLE_TYPE);

#ifdef QT3_SUPPORT
Q_DECLARE_OPERATORS_FOR_FLAGS(QFileInfo::PermissionSpec)
#endif

typedef QList<QFileInfo> QFileInfoList;
#ifdef QT3_SUPPORT
typedef QList<QFileInfo>::Iterator QFileInfoListIterator;
#endif

QT_END_NAMESPACE

QT_END_HEADER

#endif // QFILEINFO_H
