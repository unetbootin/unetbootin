/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtXMLPatterns module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qbuiltintypes_p.h"

#include "qgday_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

GDay::GDay(const QDateTime &dateTime) : AbstractDateTime(dateTime)
{
}

GDay::Ptr GDay::fromLexical(const QString &lexical)
{
    static const CaptureTable captureTable( // STATIC DATA
        /* The extra paranthesis is a build fix for GCC 3.3. */
        (QRegExp(QLatin1String(
                "^\\s*"                                 /* Any preceding whitespace. */
                "---"                                   /* Delimiter. */
                "(\\d{2})"                              /* The day part, "03". */
                "(?:(?:(\\+|-))(\\d{2}):(\\d{2})|(Z))?" /* Timezone, "+08:24". */
                "\\s*$"                                 /* Any whitespace at the end. */))),
        /*zoneOffsetSignP*/         2,
        /*zoneOffsetHourP*/         3,
        /*zoneOffsetMinuteP*/       4,
        /*zoneOffsetUTCSymbolP*/    5,
        /*yearP*/                   -1,
        /*monthP*/                  -1,
        /*dayP*/                    1);

    AtomicValue::Ptr err;
    const QDateTime retval(create(err, lexical, captureTable));

    return err ? err : GDay::Ptr(new GDay(retval));
}

GDay::Ptr GDay::fromDateTime(const QDateTime &dt)
{
    QDateTime result(QDate(DefaultYear, DefaultMonth, dt.date().day()));
    copyTimeSpec(dt, result);

    return GDay::Ptr(new GDay(result));
}

QString GDay::stringValue() const
{
    return m_dateTime.toString(QLatin1String("---dd")) + zoneOffsetToString();
}

ItemType::Ptr GDay::type() const
{
    return BuiltinTypes::xsGDay;
}

QT_END_NAMESPACE
