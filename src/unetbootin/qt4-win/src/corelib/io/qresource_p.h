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

#ifndef QRESOURCE_P_H
#define QRESOURCE_P_H

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

#include "QtCore/qabstractfileengine.h"

QT_BEGIN_NAMESPACE

class QResourceFileEnginePrivate;
class QResourceFileEngine : public QAbstractFileEngine
{
private:
    Q_DECLARE_PRIVATE(QResourceFileEngine)
public:
    explicit QResourceFileEngine(const QString &path);
    ~QResourceFileEngine();

    virtual void setFileName(const QString &file);

    virtual bool open(QIODevice::OpenMode flags) ;
    virtual bool close();
    virtual bool flush();
    virtual qint64 size() const;
    virtual qint64 pos() const;
    virtual bool atEnd() const;
    virtual bool seek(qint64);
    virtual qint64 read(char *data, qint64 maxlen);
    virtual qint64 write(const char *data, qint64 len);

    virtual bool remove();
    virtual bool copy(const QString &newName);
    virtual bool rename(const QString &newName);
    virtual bool link(const QString &newName);

    virtual bool isSequential() const;

    virtual bool isRelativePath() const;

    virtual bool mkdir(const QString &dirName, bool createParentDirectories) const;
    virtual bool rmdir(const QString &dirName, bool recurseParentDirectories) const;

    virtual bool setSize(qint64 size);

    virtual QStringList entryList(QDir::Filters filters, const QStringList &filterNames) const;

    virtual bool caseSensitive() const;

    virtual FileFlags fileFlags(FileFlags type) const;

    virtual bool setPermissions(uint perms);

    virtual QString fileName(QAbstractFileEngine::FileName file) const;

    virtual uint ownerId(FileOwner) const;
    virtual QString owner(FileOwner) const;

    virtual QDateTime fileTime(FileTime time) const;

    virtual Iterator *beginEntryList(QDir::Filters filters, const QStringList &filterNames);
    virtual Iterator *endEntryList();
    
    bool extension(Extension extension, const ExtensionOption *option = 0, ExtensionReturn *output = 0);
    bool supportsExtension(Extension extension) const;
};

QT_END_NAMESPACE

#endif // QRESOURCE_P_H
