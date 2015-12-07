/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt3Support module of the Qt Toolkit.
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

#ifndef Q3MEMBUF_P_H
#define Q3MEMBUF_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of a number of Qt sources files.  This header file may change from
// version to version without notice, or even be removed.
//
// We mean it.
//

#include "QtCore/qbytearray.h"
#include "QtCore/qlist.h"

QT_BEGIN_NAMESPACE

class Q_COMPAT_EXPORT Q3Membuf
{
public:
    Q3Membuf();
    ~Q3Membuf();

    void append(QByteArray *ba);
    void clear();

    bool consumeBytes(Q_ULONG nbytes, char *sink);
    QByteArray readAll();
    bool scanNewline(QByteArray *store);
    bool canReadLine() const;

    int ungetch(int ch);

    qint64 size() const;

private:

    QList<QByteArray *> buf;
    qint64 _size;
    qint64 _index;
};

inline void Q3Membuf::append(QByteArray *ba)
{ buf.append(ba); _size += ba->size(); }

inline void Q3Membuf::clear()
{ qDeleteAll(buf); buf.clear(); _size=0; _index=0; }

inline QByteArray Q3Membuf::readAll()
{ QByteArray ba; ba.resize(_size); consumeBytes(_size,ba.data()); return ba; }

inline bool Q3Membuf::canReadLine() const
{ return const_cast<Q3Membuf*>(this)->scanNewline(0); }

inline qint64 Q3Membuf::size() const
{ return _size; }

QT_END_NAMESPACE

#endif // Q3MEMBUF_P_H
