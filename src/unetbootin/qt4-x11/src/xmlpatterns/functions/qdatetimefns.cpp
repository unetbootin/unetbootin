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

