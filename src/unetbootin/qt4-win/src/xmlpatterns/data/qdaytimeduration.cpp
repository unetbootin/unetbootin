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

#include "qabstractdatetime_p.h"
#include "qbuiltintypes_p.h"
#include "qcommonvalues_p.h"

#include "qdaytimeduration_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

DayTimeDuration::DayTimeDuration(const bool isPositiveP,
                                 const DayCountProperty daysP,
                                 const HourProperty hoursP,
                                 const MinuteProperty minutesP,
                                 const SecondProperty secs,
                                 const MSecondProperty msecs) : AbstractDuration(isPositiveP),
                                                                m_days(daysP),
                                                                m_hours(hoursP),
                                                                m_minutes(minutesP),
                                                                m_seconds(secs),
                                                                m_mseconds(msecs)
{
}

DayTimeDuration::Ptr DayTimeDuration::fromLexical(const QString &lexical)
{
    static const CaptureTable captureTable(
        /* The extra paranthesis is a build fix for GCC 3.3. */
        (QRegExp(QLatin1String(
                "^\\s*"                         /* Any preceding whitespace. */
                "(-)?"                          /* Any minus sign. */
                "P"                             /* Delimiter. */
                "(?:(\\d+)D)?"                  /* Day part. */
                "(?:"                           /* Here starts the optional time part. */
                "(T)"                           /* SchemaTime delimiter. */
                "(?:(\\d+)H)?"                  /* Hour part. */
                "(?:(\\d+)M)?"                  /* Minute part. */
                "(?:(\\d+)(?:\\.(\\d+))?S)?"    /* Seconds & milli seconds. */
                ")?"                            /* End of optional time part. */
                "\\s*$"                         /* Any terminating whitespace. */))),
        /*yearP*/       -1,
        /*monthP*/      -1,
        /*dayP*/        2,
        /*tDelimiterP*/ 3,
        /*hourP*/       4,
        /*minutesP*/    5,
        /*secondsP*/    6,
        /*msecondsP*/   7);

    DayCountProperty days = 0;
    HourProperty hours = 0;
    MinuteProperty minutes = 0;
    SecondProperty sec = 0;
    MSecondProperty msec = 0;
    bool isPos;

    const DayTimeDuration::Ptr err(create(captureTable, lexical, &isPos, 0, 0, &days,
                                          &hours, &minutes, &sec, &msec));
    return err ? err : DayTimeDuration::Ptr(new DayTimeDuration(isPos, days, hours, minutes,
                                                                sec, msec));
}

DayTimeDuration::Ptr DayTimeDuration::fromComponents(const bool isPositive,
                                                     const DayCountProperty days,
                                                     const HourProperty hours,
                                                     const MinuteProperty minutes,
                                                     const SecondProperty seconds,
                                                     const MSecondProperty mseconds)
{
    return DayTimeDuration::Ptr(new DayTimeDuration(isPositive,
                                                    days,
                                                    hours,
                                                    minutes,
                                                    seconds,
                                                    mseconds));
}

DayTimeDuration::Ptr DayTimeDuration::fromSeconds(const SecondCountProperty sourceSecs,
                                                  const MSecondProperty msecs)
{
    Q_ASSERT(msecs >= 0);
    const SecondCountProperty source = qAbs(sourceSecs);
    const bool isPos = sourceSecs >= 0;
    const SecondCountProperty secs = source % 60;
    const MinuteCountProperty mins = (source / 60) % 60;
    const HourCountProperty hours = source / (60 * 60) % 24;
    const DayCountProperty days = source / (60 * 60) / 24;

    return DayTimeDuration::Ptr(new DayTimeDuration(isPos, days, hours, mins, secs, msecs));
}

QString DayTimeDuration::stringValue() const
{
    QString retval;

    if(!m_isPositive)
        retval.append(QLatin1Char('-'));

    retval.append(QLatin1Char('P'));

    if(m_days)
    {
        retval.append(QString::number(m_days));
        retval.append(QLatin1Char('D'));
    }

    if(!m_hours && !m_minutes && !m_seconds && !m_seconds)
    {
        if(!m_days)
            return QLatin1String("PT0S");
        else
            return retval;
    }

    retval.append(QLatin1Char('T'));

    if(m_hours)
    {
        retval.append(QString::number(m_hours));
        retval.append(QLatin1Char('H'));
    }

    if(m_minutes)
    {
        retval.append(QString::number(m_minutes));
        retval.append(QLatin1Char('M'));
    }

    if(m_seconds || m_seconds)
    {
        retval.append(QString::number(m_seconds));

        if(m_mseconds)
            retval.append(serializeMSeconds(m_mseconds));

        retval.append(QLatin1Char('S'));
    }
    else if(!m_days && !m_hours && !m_minutes)
        retval.append(QLatin1String("0S"));

    return retval;
}

AbstractDuration::Value DayTimeDuration::value() const
{
    return ((m_days * 24 * 60 * 60 * 1000) +
            (m_hours * 60 * 60 * 1000) +
            (m_minutes * 60 * 1000) +
            (m_seconds * 1000) +
            m_mseconds) * (m_isPositive ? 1 : -1);
}

Item DayTimeDuration::fromValue(const Value val) const
{
    if(val == 0)
        return toItem(CommonValues::DayTimeDurationZero);
    else
        return toItem(fromSeconds(val / 1000, qAbs(val) % 1000));
}

ItemType::Ptr DayTimeDuration::type() const
{
    return BuiltinTypes::xsDayTimeDuration;
}

YearProperty DayTimeDuration::years() const
{
    return 0;
}

MonthProperty DayTimeDuration::months() const
{
    return 0;
}

DayCountProperty DayTimeDuration::days() const
{
    return m_days;
}

HourProperty DayTimeDuration::hours() const
{
    return m_hours;
}

MinuteProperty DayTimeDuration::minutes() const
{
    return m_minutes;
}

SecondProperty DayTimeDuration::seconds() const
{
    return m_seconds;
}

MSecondProperty DayTimeDuration::mseconds() const
{
    return m_mseconds;
}

QT_END_NAMESPACE
