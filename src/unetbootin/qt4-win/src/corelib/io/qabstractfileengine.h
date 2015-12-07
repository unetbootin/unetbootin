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

#ifndef QABSTRACTFILEENGINE_H
#define QABSTRACTFILEENGINE_H

#include <QtCore/qdir.h>

#ifdef open
#error qabstractfileengine.h must be included before any header file that defines open
#endif

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Core)

class QFileExtension;
class QFileExtensionResult;
class QVariant;
class QAbstractFileEngineIterator;
class QAbstractFileEnginePrivate;

class Q_CORE_EXPORT QAbstractFileEngine
{
public:
    enum FileFlag {
        //perms (overlaps the QFile::Permission)
        ReadOwnerPerm = 0x4000, WriteOwnerPerm = 0x2000, ExeOwnerPerm = 0x1000,
        ReadUserPerm  = 0x0400, WriteUserPerm  = 0x0200, ExeUserPerm  = 0x0100,
        ReadGroupPerm = 0x0040, WriteGroupPerm = 0x0020, ExeGroupPerm = 0x0010,
        ReadOtherPerm = 0x0004, WriteOtherPerm = 0x0002, ExeOtherPerm = 0x0001,

        //types
        LinkType      = 0x10000,
        FileType      = 0x20000,
        DirectoryType = 0x40000,
        BundleType    = 0x80000,

        //flags
        HiddenFlag     = 0x0100000,
        LocalDiskFlag  = 0x0200000,
        ExistsFlag     = 0x0400000,
        RootFlag       = 0x0800000,
        Refresh        = 0x1000000,

        //masks
        PermsMask  = 0x0000FFFF,
        TypesMask  = 0x000F0000,
        FlagsMask  = 0x0FF00000,
        FileInfoAll = FlagsMask | PermsMask | TypesMask
    };
    Q_DECLARE_FLAGS(FileFlags, FileFlag)

    enum FileName {
        DefaultName,
        BaseName,
        PathName,
        AbsoluteName,
        AbsolutePathName,
        LinkName,
        CanonicalName,
        CanonicalPathName,
        BundleName
    };
    enum FileOwner {
        OwnerUser,
        OwnerGroup
    };
    enum FileTime {
        CreationTime,
        ModificationTime,
        AccessTime
    };

    virtual ~QAbstractFileEngine();

    virtual bool open(QIODevice::OpenMode openMode);
    virtual bool close();
    virtual bool flush();
    virtual qint64 size() const;
    virtual qint64 pos() const;
    virtual bool seek(qint64 pos);
    virtual bool isSequential() const;
    virtual bool remove();
    virtual bool copy(const QString &newName);
    virtual bool rename(const QString &newName);
    virtual bool link(const QString &newName);
    virtual bool mkdir(const QString &dirName, bool createParentDirectories) const;
    virtual bool rmdir(const QString &dirName, bool recurseParentDirectories) const;
    virtual bool setSize(qint64 size);
    virtual bool caseSensitive() const;
    virtual bool isRelativePath() const;
    virtual QStringList entryList(QDir::Filters filters, const QStringList &filterNames) const;
    virtual FileFlags fileFlags(FileFlags type=FileInfoAll) const;
    virtual bool setPermissions(uint perms);
    virtual QString fileName(FileName file=DefaultName) const;
    virtual uint ownerId(FileOwner) const;
    virtual QString owner(FileOwner) const;
    virtual QDateTime fileTime(FileTime time) const;
    virtual void setFileName(const QString &file);
    virtual int handle() const;
    bool atEnd() const;
    uchar *map(qint64 offset, qint64 size, QFile::MemoryMapFlags flags);
    bool unmap(uchar *ptr);

    typedef QAbstractFileEngineIterator Iterator;
    virtual Iterator *beginEntryList(QDir::Filters filters, const QStringList &filterNames);
    virtual Iterator *endEntryList();

    virtual qint64 read(char *data, qint64 maxlen);
    virtual qint64 readLine(char *data, qint64 maxlen);
    virtual qint64 write(const char *data, qint64 len);

    QFile::FileError error() const;
    QString errorString() const;

    enum Extension {
        AtEndExtension,
        FastReadLineExtension,
        MapExtension,
        UnMapExtension
    };
    class ExtensionOption
    {};
    class ExtensionReturn
    {};

    class MapExtensionOption : public ExtensionOption {
    public:
        qint64 offset;
        qint64 size;
        QFile::MemoryMapFlags flags;
    };
    class MapExtensionReturn : public ExtensionReturn {
    public:
        uchar *address;
    };

    class UnMapExtensionOption : public ExtensionOption {
    public:
        uchar *address;
    };

    virtual bool extension(Extension extension, const ExtensionOption *option = 0, ExtensionReturn *output = 0);
    virtual bool supportsExtension(Extension extension) const;

    // Factory
    static QAbstractFileEngine *create(const QString &fileName);

protected:
    void setError(QFile::FileError error, const QString &str);

    QAbstractFileEngine();
    QAbstractFileEngine(QAbstractFileEnginePrivate &);

    QAbstractFileEnginePrivate *d_ptr;
private:
    Q_DECLARE_PRIVATE(QAbstractFileEngine)
    Q_DISABLE_COPY(QAbstractFileEngine)
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QAbstractFileEngine::FileFlags)

class Q_CORE_EXPORT QAbstractFileEngineHandler
{
public:
    QAbstractFileEngineHandler();
    virtual ~QAbstractFileEngineHandler();
    virtual QAbstractFileEngine *create(const QString &fileName) const = 0;
};

class QAbstractFileEngineIteratorPrivate;
class Q_CORE_EXPORT QAbstractFileEngineIterator
{
public:
    QAbstractFileEngineIterator(QDir::Filters filters, const QStringList &nameFilters);
    virtual ~QAbstractFileEngineIterator();

    virtual QString next() = 0;
    virtual bool hasNext() const = 0;

    QString path() const;
    QStringList nameFilters() const;
    QDir::Filters filters() const;

    virtual QString currentFileName() const = 0;
    virtual QFileInfo currentFileInfo() const;
    QString currentFilePath() const;

protected:
    enum EntryInfoType {
    };
    virtual QVariant entryInfo(EntryInfoType type) const;

private:
    Q_DISABLE_COPY(QAbstractFileEngineIterator)
    friend class QDirIterator;
    friend class QDirIteratorPrivate;
    void setPath(const QString &path);
    QAbstractFileEngineIteratorPrivate *d;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QABSTRACTFILEENGINE_H
