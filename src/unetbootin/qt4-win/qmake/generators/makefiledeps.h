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

#ifndef MAKEFILEDEPS_H
#define MAKEFILEDEPS_H

#include <qstringlist.h>
#include <qfileinfo.h>

QT_BEGIN_NAMESPACE

struct SourceFile;
struct SourceDependChildren;
class SourceFiles;

class QMakeLocalFileName {
    uint is_null : 1;
    mutable QString real_name, local_name;
public:
    QMakeLocalFileName() : is_null(1) { }
    QMakeLocalFileName(const QString &);
    bool isNull() const { return is_null; }
    inline const QString &real() const { return real_name; }
    const QString &local() const;

    bool operator==(const QMakeLocalFileName &other) {
        return (this->real_name == other.real_name);
    }
    bool operator!=(const QMakeLocalFileName &other) {
        return !(*this == other);
    }
};

class QMakeSourceFileInfo
{
private:
    //quick project lookups
    SourceFiles *files, *includes;
    bool files_changed;
    QList<QMakeLocalFileName> depdirs;

    //sleezy buffer code
    char *spare_buffer;
    int   spare_buffer_size;
    char *getBuffer(int s);

    //actual guts
    bool findMocs(SourceFile *);
    bool findDeps(SourceFile *);
    void dependTreeWalker(SourceFile *, SourceDependChildren *);

    //cache
    QString cachefile;

protected:
    virtual QMakeLocalFileName fixPathForFile(const QMakeLocalFileName &, bool forOpen=false);
    virtual QMakeLocalFileName findFileForDep(const QMakeLocalFileName &, const QMakeLocalFileName &);
    virtual QFileInfo findFileInfo(const QMakeLocalFileName &);

public:
    QMakeSourceFileInfo(const QString &cachefile="");
    virtual ~QMakeSourceFileInfo();

    QList<QMakeLocalFileName> dependencyPaths() const { return depdirs; }
    void setDependencyPaths(const QList<QMakeLocalFileName> &);

    enum DependencyMode { Recursive, NonRecursive };
    inline void setDependencyMode(DependencyMode mode) { dep_mode = mode; }
    inline DependencyMode dependencyMode() const { return dep_mode; }

    enum SourceFileType { TYPE_UNKNOWN, TYPE_C, TYPE_UI, TYPE_QRC };
    enum SourceFileSeek { SEEK_DEPS=0x01, SEEK_MOCS=0x02 };
    void addSourceFiles(const QStringList &, uchar seek, SourceFileType type=TYPE_C);
    void addSourceFile(const QString &, uchar seek, SourceFileType type=TYPE_C);
    bool containsSourceFile(const QString &, SourceFileType type=TYPE_C);

    int included(const QString &file);
    QStringList dependencies(const QString &file);

    bool mocable(const QString &file);

    virtual QMap<QString, QStringList> getCacheVerification();
    virtual bool verifyCache(const QMap<QString, QStringList> &);
    void setCacheFile(const QString &cachefile); //auto caching
    void loadCache(const QString &cf);
    void saveCache(const QString &cf);

private:
    DependencyMode dep_mode;
};

QT_END_NAMESPACE

#endif // MAKEFILEDEPS_H
