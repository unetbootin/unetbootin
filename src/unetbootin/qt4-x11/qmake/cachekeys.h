/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the qmake application of the Qt Toolkit.
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

#ifndef CACHEKEYS_H
#define CACHEKEYS_H

#include "project.h"
#include <qstring.h>
#include <qstringlist.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qhash.h>

QT_BEGIN_NAMESPACE

// -------------------------------------------------------------------------------------------------
struct FixStringCacheKey
{
    mutable uint hash;
    QString string, pwd;
    uchar flags;
    FixStringCacheKey(const QString &s, uchar f)
    {
        hash = 0;
        pwd = qmake_getpwd();
        string = s;
        flags = f;
    }
    bool operator==(const FixStringCacheKey &f) const
    {
        return (hashCode() == f.hashCode() &&
                f.flags == flags &&
                f.string == string &&
                f.pwd == pwd);
    }
    inline uint hashCode() const {
        if(!hash)
            hash = qHash(string) | qHash(flags) /*| qHash(pwd)*/;
        return hash;
    }
};
inline uint qHash(const FixStringCacheKey &f) { return f.hashCode(); }

// -------------------------------------------------------------------------------------------------
struct FileInfoCacheKey
{
    mutable uint hash;
    QString file, pwd;
    FileInfoCacheKey(const QString &f)
    {
        hash = 0;
        if(isRelativePath(f))
            pwd = qmake_getpwd();
        file = f;
    }
    bool operator==(const FileInfoCacheKey &f) const
    {
        return (hashCode() == f.hashCode() && f.file == file &&
                f.pwd == pwd);
    }
    inline uint hashCode() const {
        if(!hash)
            hash = qHash(file) /*| qHash(pwd)*/;
        return hash;
    }
    inline bool isRelativePath(const QString &file) {
        int length = file.length();
        if (!length)
            return true;

        const QChar c0 = file.at(0);
        const QChar c1 = length >= 2 ? file.at(1) : QChar(0);
        return !(c0 == QLatin1Char('/')
                || c0 == QLatin1Char('\\')
                || (c0.isLetter() && c1 == QLatin1Char(':'))
                || (c0 == QLatin1Char('/') && c1 == QLatin1Char('/'))
                || (c0 == QLatin1Char('\\') && c1 == QLatin1Char('\\')));
    }
};
inline uint qHash(const FileInfoCacheKey &f) { return f.hashCode(); }

// -------------------------------------------------------------------------------------------------
struct FileFixifyCacheKey
{
    mutable uint hash;
    QString in_d, out_d;
    QString file, pwd;
    uint fixType;
    bool canonicalize;
    FileFixifyCacheKey(const QString &f, const QString &od, const QString &id,
                       uint ft, bool c)
    {
        hash = 0;
        pwd = qmake_getpwd();
        file = f;
        if(od.isNull())
            out_d = Option::output_dir;
        else
            out_d = od;
        if(id.isNull())
            in_d = qmake_getpwd();
        else
            in_d = id;
        fixType = ft;
        canonicalize = c;
    }
    QString toString() const {
        return file + "--" + in_d + "--" + out_d + "--" + pwd + "--" +
            QString::number(fixType) + "--" + QString::number((int)canonicalize);
    }
    bool operator==(const FileFixifyCacheKey &f) const
    {
        return (f.canonicalize == canonicalize &&
                f.fixType == fixType &&
                f.file == file &&
                f.in_d == in_d &&
                f.out_d == out_d &&
                f.pwd == pwd);
    }
    inline uint hashCode() const {
        if(!hash)
            hash = uint(canonicalize) | uint(fixType) |
                   qHash(file) | qHash(in_d) | qHash(out_d) /*|qHash(pwd)*/;
        return hash;
    }
};

inline uint qHash(const FileFixifyCacheKey &f) { return f.hashCode(); }
// -------------------------------------------------------------------------------------------------

// As MSVC 6.0 can't handle template functions that well, we need a separate function for each type
inline void qmakeDeleteCacheClear_QMapStringInt(void *i) { delete reinterpret_cast<QMap<QString,int> *>(i); }
inline void qmakeDeleteCacheClear_QStringList(void *i) { delete reinterpret_cast<QStringList *>(i); }
inline void qmakeDeleteCacheClear_QHashFixStringCacheKeyQString(void *i) { delete reinterpret_cast<QHash<FixStringCacheKey, QString> *>(i); }
inline void qmakeDeleteCacheClear_QHashFileInfoCacheKeyQFileInfo(void *i) { delete reinterpret_cast<QHash<FileInfoCacheKey, QFileInfo> *>(i); }
inline void qmakeDeleteCacheClear_QHashFileFixifyCacheKeyQString(void *i) { delete reinterpret_cast<QHash<FileFixifyCacheKey, QString> *>(i); }
inline void qmakeFreeCacheClear(void *i) { free(i); }

typedef void (*qmakeCacheClearFunc)(void *);
void qmakeAddCacheClear(qmakeCacheClearFunc func, void **);
void qmakeClearCaches();

QT_END_NAMESPACE

#endif // CACHEKEYS_H
