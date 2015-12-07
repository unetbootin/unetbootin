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

#ifndef QFILEINFO_P_H
#define QFILEINFO_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of QIODevice. This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qfileinfo.h"

QT_BEGIN_NAMESPACE

class QFileInfoPrivate
{
public:
    QFileInfoPrivate(const QFileInfo *copy=0);
    ~QFileInfoPrivate();

    void initFileEngine(const QString &);

    enum Access {
        ReadAccess,
        WriteAccess,
        ExecuteAccess
    };
    bool hasAccess(Access access) const;

    uint getFileFlags(QAbstractFileEngine::FileFlags) const;
    QDateTime &getFileTime(QAbstractFileEngine::FileTime) const;
    QString getFileName(QAbstractFileEngine::FileName) const;

    enum {
        CachedFileFlags = 0x01,
        CachedLinkTypeFlag = 0x02,
        CachedBundleTypeFlag= 0x04,
        CachedMTime = 0x10,
        CachedCTime = 0x20,
        CachedATime = 0x40,
        CachedSize = 0x08
    };

    struct Data
    {
        inline Data()
            : ref(1), fileEngine(0), cache_enabled(1)
        {
            clear();
        }

        inline Data(const Data &copy)
            : ref(1), fileEngine(QAbstractFileEngine::create(copy.fileName)),
              fileName(copy.fileName), cache_enabled(copy.cache_enabled)
        {
            clear();
        }

        inline ~Data()
        {
            delete fileEngine;
        }

        inline void clear()
        {
            fileNames.clear();
            fileFlags = 0;
            cachedFlags = 0;
        }

        mutable QAtomicInt ref;

        QAbstractFileEngine *fileEngine;
        mutable QString fileName;
        mutable QHash<int, QString> fileNames;
        mutable uint cachedFlags : 31;
        mutable uint cache_enabled : 1;
        mutable uint fileFlags;
        mutable qint64 fileSize;
        mutable QDateTime fileTimes[3];

        inline bool getCachedFlag(uint c) const
        { return cache_enabled ? (cachedFlags & c) : 0; }

        inline void setCachedFlag(uint c)
        { if (cache_enabled) cachedFlags |= c; }
    } *data;

    inline void reset() {
        detach();
        data->clear();
    }

    void detach();
};


QT_END_NAMESPACE
#endif

