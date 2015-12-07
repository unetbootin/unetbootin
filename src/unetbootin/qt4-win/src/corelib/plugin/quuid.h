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

#ifndef QUUID_H
#define QUUID_H

#include <QtCore/qstring.h>

QT_BEGIN_HEADER

#if defined(Q_OS_WIN)
#ifndef GUID_DEFINED
#define GUID_DEFINED
typedef struct _GUID
{
    ulong   Data1;
    ushort  Data2;
    ushort  Data3;
    uchar   Data4[8];
} GUID, *REFGUID, *LPGUID;
#endif
#endif


QT_BEGIN_NAMESPACE

QT_MODULE(Core)

struct Q_CORE_EXPORT QUuid
{
    enum Variant {
        VarUnknown        =-1,
        NCS                = 0, // 0 - -
        DCE                = 2, // 1 0 -
        Microsoft        = 6, // 1 1 0
        Reserved        = 7  // 1 1 1
    };

    enum Version {
        VerUnknown        =-1,
        Time                = 1, // 0 0 0 1
        EmbeddedPOSIX        = 2, // 0 0 1 0
        Name                = 3, // 0 0 1 1
        Random                = 4  // 0 1 0 0
    };

    QUuid()
    {
        data1 = 0;
        data2 = 0;
        data3 = 0;
        for(int i = 0; i < 8; i++)
            data4[i] = 0;
    }
    QUuid(uint l, ushort w1, ushort w2, uchar b1, uchar b2, uchar b3, uchar b4, uchar b5, uchar b6, uchar b7, uchar b8)
    {
        data1 = l;
        data2 = w1;
        data3 = w2;
        data4[0] = b1;
        data4[1] = b2;
        data4[2] = b3;
        data4[3] = b4;
        data4[4] = b5;
        data4[5] = b6;
        data4[6] = b7;
        data4[7] = b8;
    }
#ifndef QT_NO_QUUID_STRING
    QUuid(const QString &);
    QUuid(const char *);
    QString toString() const;
    operator QString() const { return toString(); }
#endif
    bool isNull() const;

    bool operator==(const QUuid &orig) const
    {
        uint i;
        if (data1 != orig.data1 || data2 != orig.data2 ||
             data3 != orig.data3)
            return false;

        for(i = 0; i < 8; i++)
            if (data4[i] != orig.data4[i])
                return false;

        return true;
    }

    bool operator!=(const QUuid &orig) const
    {
        return !(*this == orig);
    }

    bool operator<(const QUuid &other) const;
    bool operator>(const QUuid &other) const;

#if defined(Q_OS_WIN)
    // On Windows we have a type GUID that is used by the platform API, so we
    // provide convenience operators to cast from and to this type.
    QUuid(const GUID &guid)
    {
        data1 = guid.Data1;
        data2 = guid.Data2;
        data3 = guid.Data3;
        for(int i = 0; i < 8; i++)
            data4[i] = guid.Data4[i];
    }

    QUuid &operator=(const GUID &guid)
    {
        *this = QUuid(guid);
        return *this;
    }

    operator GUID() const
    {
        GUID guid = { data1, data2, data3, { data4[0], data4[1], data4[2], data4[3], data4[4], data4[5], data4[6], data4[7] } };
        return guid;
    }

    bool operator==(const GUID &guid) const
    {
        return *this == QUuid(guid);
    }

    bool operator!=(const GUID &guid) const
    {
        return !(*this == guid);
    }
#endif
    static QUuid createUuid();
    QUuid::Variant variant() const;
    QUuid::Version version() const;

    uint    data1;
    ushort  data2;
    ushort  data3;
    uchar   data4[8];
};

#ifndef QT_NO_DATASTREAM
Q_CORE_EXPORT QDataStream &operator<<(QDataStream &, const QUuid &);
Q_CORE_EXPORT QDataStream &operator>>(QDataStream &, QUuid &);
#endif

QT_END_NAMESPACE

QT_END_HEADER

#endif // QUUID_H
