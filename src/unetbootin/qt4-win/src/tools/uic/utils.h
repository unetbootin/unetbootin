/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the tools applications of the Qt Toolkit.
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

#ifndef UTILS_H
#define UTILS_H

#include "ui4.h"
#include <QtCore/QString>
#include <QtCore/QList>
#include <QtCore/QHash>

QT_BEGIN_NAMESPACE

inline bool toBool(const QString &str)
{ return str.toLower() == QLatin1String("true"); }

inline QString toString(const DomString *str)
{ return str ? str->text() : QString(); }

inline QString fixString(const QString &str, const QString &indent)
{
    QString cursegment;
    QStringList result;
    const QByteArray utf8 = str.toUtf8();
    const int utf8Length = utf8.length();

    for (int i = 0; i < utf8Length; ++i) {
        const uchar cbyte = utf8.at(i);
        if (cbyte >= 0x80) {
            cursegment += QLatin1Char('\\');
            cursegment += QString::number(cbyte, 8);
        } else {
            switch(cbyte) {
            case '\\':
                cursegment += QLatin1String("\\\\"); break;
            case '\"':
                cursegment += QLatin1String("\\\""); break;
            case '\r':
                break;
            case '\n':
                cursegment += QLatin1String("\\n\"\n\""); break;
            default:
                cursegment += QLatin1Char(cbyte);
            }
        }

        if (cursegment.length() > 1024) {
            result << cursegment;
            cursegment.clear();
        }
    }

    if (!cursegment.isEmpty())
        result << cursegment;


    QString joinstr = QLatin1String("\"\n");
    joinstr += indent;
    joinstr += indent;
    joinstr += QLatin1Char('"');

    QString rc(QLatin1Char('"'));
    rc += result.join(joinstr);
    rc += QLatin1Char('"');
    return rc;
}

inline QHash<QString, DomProperty *> propertyMap(const QList<DomProperty *> &properties)
{
    QHash<QString, DomProperty *> map;

    for (int i=0; i<properties.size(); ++i) {
        DomProperty *p = properties.at(i);
        map.insert(p->attributeName(), p);
    }

    return map;
}

inline QStringList unique(const QStringList &lst)
{
    QHash<QString, bool> h;
    for (int i=0; i<lst.size(); ++i)
        h.insert(lst.at(i), true);
    return h.keys();
}

QT_END_NAMESPACE

#endif // UTILS_H
