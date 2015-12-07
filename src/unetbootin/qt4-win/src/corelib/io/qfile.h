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

#ifndef QFILE_H
#define QFILE_H

#include <QtCore/qiodevice.h>
#include <QtCore/qstring.h>
#include <stdio.h>

#ifdef open
#error qfile.h must be included before any header file that defines open
#endif

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Core)

class QAbstractFileEngine;
class QFilePrivate;

class Q_CORE_EXPORT QFile : public QIODevice
{
#ifndef QT_NO_QOBJECT
    Q_OBJECT
#endif
    Q_DECLARE_PRIVATE(QFile)

public:

    enum FileError {
        NoError = 0,
        ReadError = 1,
        WriteError = 2,
        FatalError = 3,
        ResourceError = 4,
        OpenError = 5,
        AbortError = 6,
        TimeOutError = 7,
        UnspecifiedError = 8,
        RemoveError = 9,
        RenameError = 10,
        PositionError = 11,
        ResizeError = 12,
        PermissionsError = 13,
        CopyError = 14
#ifdef QT3_SUPPORT
        , ConnectError = 30
#endif
    };

    enum Permission {
        ReadOwner = 0x4000, WriteOwner = 0x2000, ExeOwner = 0x1000,
        ReadUser  = 0x0400, WriteUser  = 0x0200, ExeUser  = 0x0100,
        ReadGroup = 0x0040, WriteGroup = 0x0020, ExeGroup = 0x0010,
        ReadOther = 0x0004, WriteOther = 0x0002, ExeOther = 0x0001
    };
    Q_DECLARE_FLAGS(Permissions, Permission)

    QFile();
    QFile(const QString &name);
#ifndef QT_NO_QOBJECT
    explicit QFile(QObject *parent);
    QFile(const QString &name, QObject *parent);
#endif
    ~QFile();

    FileError error() const;
    void unsetError();

    QString fileName() const;
    void setFileName(const QString &name);

    typedef QByteArray (*EncoderFn)(const QString &fileName);
    typedef QString (*DecoderFn)(const QByteArray &localfileName);
    static QByteArray encodeName(const QString &fileName);
    static QString decodeName(const QByteArray &localFileName);
    inline static QString decodeName(const char *localFileName)
        { return decodeName(QByteArray(localFileName)); };
    static void setEncodingFunction(EncoderFn);
    static void setDecodingFunction(DecoderFn);

    bool exists() const;
    static bool exists(const QString &fileName);

    QString readLink() const;
    static QString readLink(const QString &fileName);
    inline QString symLinkTarget() const { return readLink(); }
    inline static QString symLinkTarget(const QString &fileName) { return readLink(fileName); }

    bool remove();
    static bool remove(const QString &fileName);

    bool rename(const QString &newName);
    static bool rename(const QString &oldName, const QString &newName);

    bool link(const QString &newName);
    static bool link(const QString &oldname, const QString &newName);

    bool copy(const QString &newName);
    static bool copy(const QString &fileName, const QString &newName);

    bool isSequential() const;

    bool open(OpenMode flags);
    bool open(FILE *f, OpenMode flags);
    bool open(int fd, OpenMode flags);
    virtual void close();

    qint64 size() const;
    qint64 pos() const;
    bool seek(qint64 offset);
    bool atEnd() const;
    bool flush();

    bool resize(qint64 sz);
    static bool resize(const QString &filename, qint64 sz);

    Permissions permissions() const;
    static Permissions permissions(const QString &filename);
    bool setPermissions(Permissions permissionSpec);
    static bool setPermissions(const QString &filename, Permissions permissionSpec);

    int handle() const;

    enum MemoryMapFlags {
        NoOptions = 0
    };

    uchar *map(qint64 offset, qint64 size, MemoryMapFlags flags = NoOptions);
    bool unmap(uchar *address);

    virtual QAbstractFileEngine *fileEngine() const;

#ifdef QT3_SUPPORT
    typedef Permission PermissionSpec;
    inline QT3_SUPPORT QString name() const { return fileName(); }
    inline QT3_SUPPORT void setName(const QString &aName) { setFileName(aName); }
    inline QT3_SUPPORT bool open(OpenMode aFlags, FILE *f) { return open(f, aFlags); }
    inline QT3_SUPPORT bool open(OpenMode aFlags, int fd) { return open(fd, aFlags); }
#endif

protected:
#ifdef QT_NO_QOBJECT
    QFile(QFilePrivate &dd);
#else
    QFile(QFilePrivate &dd, QObject *parent = 0);
#endif

    qint64 readData(char *data, qint64 maxlen);
    qint64 writeData(const char *data, qint64 len);
    qint64 readLineData(char *data, qint64 maxlen);

private:
    Q_DISABLE_COPY(QFile)
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QFile::Permissions)

QT_END_NAMESPACE

QT_END_HEADER

#endif // QFILE_H
