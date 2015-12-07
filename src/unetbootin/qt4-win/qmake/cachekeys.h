/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the qmake application of the Qt Toolkit.
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
