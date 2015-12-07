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
#include "qcontextfns_p.h"
#include "qdate_p.h"
#include "qdatetime_p.h"
#include "qdaytimeduration_p.h"
#include "qpatternistlocale_p.h"
#include "qschematime_p.h"

#include "qtimezonefns_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

Item AdjustTimezone::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    enum
    {
        /**
         * The maximum zone offset, @c PT14H, in milli seconds.
         */
        MSecLimit = 14 * 60/*M*/ * 60/*S*/ * 1000/*ms*/
    };


    const Item arg(m_operands.first()->evaluateSingleton(context));
    if(!arg)
        return Item();

    QDateTime dt(arg.as<AbstractDateTime>()->toDateTime());
    // TODO DT dt.setDateOnly(false);
    Q_ASSERT(dt.isValid());
    DayTimeDuration::Ptr tz;

    if(m_operands.count() == 2)
        tz = DayTimeDuration::Ptr(m_operands.at(1)->evaluateSingleton(context).as<DayTimeDuration>());
    else
        tz = context->implicitTimezone();

    if(tz)
    {
        const MSecondCountProperty tzMSecs = tz->value();

        if(tzMSecs % (1000 * 60) != 0)
        {
            context->error(QtXmlPatterns::tr("A zone offset must be in the "
                                             "range %1..%2 inclusive. %3 is "
                                             "out of range.")
                           .arg(formatData("-PT14H"))
                           .arg(formatData("PT14H"))
                           .arg(formatData(tz->stringValue())),
                           ReportContext::FODT0003, this);
            return Item();
        }
        else if(tzMSecs > MSecLimit ||
                tzMSecs < -MSecLimit)
        {
            context->error(QtXmlPatterns::tr("%1 is not a whole number of minutes.")
                                            .arg(formatData(tz->stringValue())),
                                       ReportContext::FODT0003, this);
            return Item();
        }

        const SecondCountProperty tzSecs = tzMSecs / 1000;

        if(dt.timeSpec() == Qt::LocalTime) /* $arg has no time zone. */
        {
            /* "If $arg does not have a timezone component and $timezone is not
             * the empty sequence, then the result is $arg with $timezone as
             * the timezone component." */
            //dt.setTimeSpec(QDateTime::Spec(QDateTime::OffsetFromUTC, tzSecs));
            dt.setUtcOffset(tzSecs);
            Q_ASSERT(dt.isValid());
            return createValue(dt);
        }
        else
        {
            /* "If $arg has a timezone component and $timezone is not the empty sequence,
             * then the result is an xs:dateTime value with a timezone component of
             * $timezone that is equal to $arg." */
            dt = dt.toUTC();
            dt = dt.addSecs(tzSecs);
            //dt.setTimeSpec(QDateTime::Spec(QDateTime::OffsetFromUTC, tzSecs));
            dt.setUtcOffset(tzSecs);
            Q_ASSERT(dt.isValid());
            return createValue(dt);
        }
    }
    else
    { /* $timezone is the empty sequence. */
        if(dt.timeSpec() == Qt::LocalTime) /* $arg has no time zone. */
        {
            /* "If $arg does not have a timezone component and $timezone is
             * the empty sequence, then the result is $arg." */
            return arg;
        }
        else
        {
            /* "If $arg has a timezone component and $timezone is the empty sequence,
             * then the result is the localized value of $arg without its timezone component." */
            dt.setTimeSpec(Qt::LocalTime);
            return createValue(dt);
        }
    }
}

Item AdjustDateTimeToTimezoneFN::createValue(const QDateTime &dt) const
{
    Q_ASSERT(dt.isValid());
    return DateTime::fromDateTime(dt);
}

Item AdjustDateToTimezoneFN::createValue(const QDateTime &dt) const
{
    Q_ASSERT(dt.isValid());
    return Date::fromDateTime(dt);
}

Item AdjustTimeToTimezoneFN::createValue(const QDateTime &dt) const
{
    Q_ASSERT(dt.isValid());
    return SchemaTime::fromDateTime(dt);
}

QT_END_NAMESPACE
