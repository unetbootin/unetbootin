/****************************************************************************
**
** Copyright (C) 2006-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtXMLPatterns module of the Qt Toolkit.
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

#include "qbuiltintypes_p.h"

#include "qgmonth_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

GMonth::GMonth(const QDateTime &dateTime) : AbstractDateTime(dateTime)
{
}

GMonth::Ptr GMonth::fromLexical(const QString &lexical)
{
    static const CaptureTable captureTable( // STATIC DATA
        /* The extra paranthesis is a build fix for GCC 3.3. */
        (QRegExp(QLatin1String(
                "^\\s*"                             /* Any preceding whitespace. */
                "--"                                /* Delimier. */
                "(\\d{2})"                          /* The month part, "03". */
                "(?:(\\+|-)(\\d{2}):(\\d{2})|(Z))?" /* Timezone, "+08:24". */
                "\\s*$"                             /* Any terminating whitespace. */))),
        /*zoneOffsetSignP*/         2,
        /*zoneOffsetHourP*/         3,
        /*zoneOffsetMinuteP*/       4,
        /*zoneOffsetUTCSymbolP*/    5,
        /*yearP*/                   -1,
        /*monthP*/                  1);

    AtomicValue::Ptr err;
    const QDateTime retval(create(err, lexical, captureTable));

    return err ? err : GMonth::Ptr(new GMonth(retval));
}

GMonth::Ptr GMonth::fromDateTime(const QDateTime &dt)
{
    QDateTime result(QDate(DefaultYear, dt.date().month(), DefaultDay));
    copyTimeSpec(dt, result);

    return GMonth::Ptr(new GMonth(result));
}

QString GMonth::stringValue() const
{
    return m_dateTime.toString(QLatin1String("--MM")) + zoneOffsetToString();
}

ItemType::Ptr GMonth::type() const
{
    return BuiltinTypes::xsGMonth;
}

QT_END_NAMESPACE
