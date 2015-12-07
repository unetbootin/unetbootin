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

/**
 * @file
 * @short This file is included by qdatetimefns_p.h.
 * If you need includes in this file, put them in qdatetimefns_p.h, outside of the namespace.
 */

template<typename TSubClass>
Item ExtractFromDurationFN<TSubClass>::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    const Item item(m_operands.first()->evaluateSingleton(context));
    if(item)
    {
        return static_cast<const TSubClass *>(this)->
               extract(item.as<AbstractDuration>());
    }
    else
        return Item();
}

Item YearsFromDurationFN::extract(const AbstractDuration *const duration) const
{
    return Integer::fromValue(duration->years() * (duration->isPositive() ? 1 : -1));
}

Item MonthsFromDurationFN::extract(const AbstractDuration *const duration) const
{
    return Integer::fromValue(duration->months() * (duration->isPositive() ? 1 : -1));
}

Item DaysFromDurationFN::extract(const AbstractDuration *const duration) const
{
    return Integer::fromValue(duration->days() * (duration->isPositive() ? 1 : -1));
}

Item HoursFromDurationFN::extract(const AbstractDuration *const duration) const
{
    return Integer::fromValue(duration->hours() * (duration->isPositive() ? 1 : -1));
}

Item MinutesFromDurationFN::extract(const AbstractDuration *const duration) const
{
    return Integer::fromValue(duration->minutes() * (duration->isPositive() ? 1 : -1));
}

Item SecondsFromDurationFN::extract(const AbstractDuration *const duration) const
{
    return toItem(Decimal::fromValue((duration->seconds() + duration->mseconds() / 1000.0) *
                                     (duration->isPositive() ? 1 : -1)));
}

template<typename TSubClass>
Item ExtractFromDateTimeFN<TSubClass>::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    const Item item(m_operands.first()->evaluateSingleton(context));
    if(item)
    {
        return static_cast<const TSubClass *>(this)->
               extract(item.as<AbstractDateTime>()->toDateTime());
    }
    else
        return Item();
}

Item YearFromAbstractDateTimeFN::extract(const QDateTime &dt) const
{
    return Integer::fromValue(dt.date().year());
}

Item DayFromAbstractDateTimeFN::extract(const QDateTime &dt) const
{
    return Integer::fromValue(dt.date().day());
}

Item MinutesFromAbstractDateTimeFN::extract(const QDateTime &dt) const
{
    return Integer::fromValue(dt.time().minute());
}

Item SecondsFromAbstractDateTimeFN::extract(const QDateTime &dt) const
{
    const QTime time(dt.time());
    return toItem(Decimal::fromValue(time.second() + time.msec() / 1000.0));
}

Item TimezoneFromAbstractDateTimeFN::extract(const QDateTime &dt) const
{
    if(dt.timeSpec() == Qt::UTC)
        return toItem(CommonValues::DayTimeDurationZero);
    else if(dt.timeSpec() == Qt::OffsetFromUTC)
        return toItem(DayTimeDuration::fromSeconds(dt.utcOffset()));
    else
        return Item();
}

Item MonthFromAbstractDateTimeFN::extract(const QDateTime &dt) const
{
    return Integer::fromValue(dt.date().month());
}

Item HoursFromAbstractDateTimeFN::extract(const QDateTime &dt) const
{
    return Integer::fromValue(dt.time().hour());
}

