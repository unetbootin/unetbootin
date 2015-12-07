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

#include <qnumeric.h>

#include "qabstractfloat_p.h"
#include "qanyuri_p.h"
#include "qbase64binary_p.h"
#include "qboolean_p.h"
#include "qcommonvalues_p.h"
#include "qdate_p.h"
#include "qschemadatetime_p.h"
#include "qdaytimeduration_p.h"
#include "qdecimal_p.h"
#include "qduration_p.h"
#include "qgday_p.h"
#include "qgmonth_p.h"
#include "qgmonthday_p.h"
#include "qgyear_p.h"
#include "qgyearmonth_p.h"
#include "qhexbinary_p.h"
#include "qinteger_p.h"
#include "qatomicstring_p.h"
#include "qschematime_p.h"
#include "quntypedatomic_p.h"
#include "qyearmonthduration_p.h"

#include "qatomiccasters_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

Item ToUntypedAtomicCaster::castFrom(const Item &from,
                                     const QExplicitlySharedDataPointer<DynamicContext> &) const
{
    return UntypedAtomic::fromValue(from.stringValue());
}

Item ToAnyURICaster::castFrom(const Item &from,
                              const QExplicitlySharedDataPointer<DynamicContext> &) const
{
    return toItem(AnyURI::fromLexical(from.stringValue()));
}

Item Base64BinaryToHexBinaryCaster::castFrom(const Item &from,
                                             const QExplicitlySharedDataPointer<DynamicContext> &) const
{
    return HexBinary::fromValue(from.as<Base64Binary>()->asByteArray());
}

Item StringToHexBinaryCaster::castFrom(const Item &from,
                                       const QExplicitlySharedDataPointer<DynamicContext> &context) const
{
    return HexBinary::fromLexical(context->namePool(), from.stringValue());
}

Item HexBinaryToBase64BinaryCaster::castFrom(const Item &from,
                                             const QExplicitlySharedDataPointer<DynamicContext> &) const
{
    return Base64Binary::fromValue(from.as<Base64Binary>()->asByteArray());
}

Item StringToBase64BinaryCaster::castFrom(const Item &from,
                                          const QExplicitlySharedDataPointer<DynamicContext> &) const
{
    return Base64Binary::fromLexical(from.stringValue());
}

Item NumericToBooleanCaster::castFrom(const Item &from,
                                      const QExplicitlySharedDataPointer<DynamicContext> &) const
{
    const xsDouble val = from.as<Numeric>()->toDouble();
    if(Double::isEqual(val, 0.0) || qIsNaN(val))
        return CommonValues::BooleanFalse;
    else
        return CommonValues::BooleanTrue;
}

Item StringToBooleanCaster::castFrom(const Item &from,
                                     const QExplicitlySharedDataPointer<DynamicContext> &) const
{
    return Boolean::fromLexical(from.stringValue());
}

Item StringToDecimalCaster::castFrom(const Item &from,
                                     const QExplicitlySharedDataPointer<DynamicContext> &) const
{
    return Decimal::fromLexical(from.stringValue());
}

Item StringToIntegerCaster::castFrom(const Item &from,
                                     const QExplicitlySharedDataPointer<DynamicContext> &) const
{
    return Integer::fromLexical(from.stringValue());
}

Item BooleanToDecimalCaster::castFrom(const Item &from,
                                      const QExplicitlySharedDataPointer<DynamicContext> &context) const
{
    if(from.as<AtomicValue>()->evaluateEBV(context))
        return CommonValues::DecimalOne;
    else
        return CommonValues::DecimalZero;
}

Item BooleanToIntegerCaster::castFrom(const Item &from,
                                      const QExplicitlySharedDataPointer<DynamicContext> &context) const
{
    if(from.as<AtomicValue>()->evaluateEBV(context))
        return CommonValues::IntegerOne;
    else
        return CommonValues::IntegerZero;
}

Item SelfToSelfCaster::castFrom(const Item &from,
                                const QExplicitlySharedDataPointer<DynamicContext> &) const
{
    return from;
}

Item StringToGYearCaster::castFrom(const Item &from,
                                   const QExplicitlySharedDataPointer<DynamicContext> &) const
{
    return GYear::fromLexical(from.stringValue());
}

Item StringToGDayCaster::castFrom(const Item &from,
                                  const QExplicitlySharedDataPointer<DynamicContext> &) const
{
    return GDay::fromLexical(from.stringValue());
}

Item StringToGMonthCaster::castFrom(const Item &from,
                                    const QExplicitlySharedDataPointer<DynamicContext> &) const
{
    return GMonth::fromLexical(from.stringValue());
}

Item StringToGYearMonthCaster::castFrom(const Item &from,
                                        const QExplicitlySharedDataPointer<DynamicContext> &) const
{
    return GYearMonth::fromLexical(from.stringValue());
}

Item StringToGMonthDayCaster::castFrom(const Item &from,
                                       const QExplicitlySharedDataPointer<DynamicContext> &) const
{
    return GMonthDay::fromLexical(from.stringValue());
}

Item StringToDateTimeCaster::castFrom(const Item &from,
                                      const QExplicitlySharedDataPointer<DynamicContext> &) const
{
    return DateTime::fromLexical(from.stringValue());
}

Item StringToTimeCaster::castFrom(const Item &from,
                                  const QExplicitlySharedDataPointer<DynamicContext> &) const
{
    return SchemaTime::fromLexical(from.stringValue());
}

Item StringToDateCaster::castFrom(const Item &from,
                                  const QExplicitlySharedDataPointer<DynamicContext> &) const
{
    return Date::fromLexical(from.stringValue());
}

Item StringToDurationCaster::castFrom(const Item &from,
                                      const QExplicitlySharedDataPointer<DynamicContext> &) const
{
    return Duration::fromLexical(from.stringValue());
}

Item StringToDayTimeDurationCaster::castFrom(const Item &from,
                                             const QExplicitlySharedDataPointer<DynamicContext> &) const
{
    return toItem(DayTimeDuration::fromLexical(from.stringValue()));
}

Item AbstractDurationToDayTimeDurationCaster::castFrom(const Item &from,
                                                       const QExplicitlySharedDataPointer<DynamicContext> &) const
{
    const AbstractDuration *const val = from.as<AbstractDuration>();

    return toItem(DayTimeDuration::fromComponents(val->isPositive(),
                                                  val->days(),
                                                  val->hours(),
                                                  val->minutes(),
                                                  val->seconds(),
                                                  val->mseconds()));
}

Item AbstractDurationToYearMonthDurationCaster::castFrom(const Item &from,
                                                         const QExplicitlySharedDataPointer<DynamicContext> &) const
{
    const AbstractDuration *const val = from.as<AbstractDuration>();

    return toItem(YearMonthDuration::fromComponents(val->isPositive(),
                                                    val->years(),
                                                    val->months()));
}

Item AbstractDurationToDurationCaster::castFrom(const Item &from,
                                                const QExplicitlySharedDataPointer<DynamicContext> &) const
{
    const AbstractDuration *const val = from.as<AbstractDuration>();

    return Duration::fromComponents(val->isPositive(),
                                    val->years(),
                                    val->months(),
                                    val->days(),
                                    val->hours(),
                                    val->minutes(),
                                    val->seconds(),
                                    val->mseconds());
}

Item StringToYearMonthDurationCaster::castFrom(const Item &from,
                                               const QExplicitlySharedDataPointer<DynamicContext> &) const
{
    return YearMonthDuration::fromLexical(from.stringValue());
}

Item AbstractDateTimeToGYearCaster::castFrom(const Item &from,
                                             const QExplicitlySharedDataPointer<DynamicContext> &) const
{
    QDateTime dt(from.as<AbstractDateTime>()->toDateTime());
    // TODO DT dt.setDateOnly(true);

    return GYear::fromDateTime(dt);
}

Item AbstractDateTimeToGYearMonthCaster::castFrom(const Item &from,
                                                  const QExplicitlySharedDataPointer<DynamicContext> &) const
{
    QDateTime dt(from.as<AbstractDateTime>()->toDateTime());
    // TODO DT dt.setDateOnly(true);

    return GYearMonth::fromDateTime(dt);
}

Item AbstractDateTimeToGMonthCaster::castFrom(const Item &from,
                                              const QExplicitlySharedDataPointer<DynamicContext> &) const
{
    QDateTime dt(from.as<AbstractDateTime>()->toDateTime());
    // TODO DT dt.setDateOnly(true);

    return GMonth::fromDateTime(dt);
}

Item AbstractDateTimeToGMonthDayCaster::castFrom(const Item &from,
                                                 const QExplicitlySharedDataPointer<DynamicContext> &) const
{
    QDateTime dt(from.as<AbstractDateTime>()->toDateTime());
    // TODO DT dt.setDateOnly(true);

    return GMonthDay::fromDateTime(dt);
}

Item AbstractDateTimeToGDayCaster::castFrom(const Item &from,
                                            const QExplicitlySharedDataPointer<DynamicContext> &) const
{
    QDateTime dt(from.as<AbstractDateTime>()->toDateTime());
    // TODO DT dt.setDateOnly(true);

    return GDay::fromDateTime(dt);
}

Item AbstractDateTimeToDateTimeCaster::castFrom(const Item &from,
                                                const QExplicitlySharedDataPointer<DynamicContext> &) const
{
    QDateTime dt(from.as<AbstractDateTime>()->toDateTime());
    // TODO DT dt.setDateOnly(false);

    return DateTime::fromDateTime(dt);
}

Item AbstractDateTimeToDateCaster::castFrom(const Item &from,
                                            const QExplicitlySharedDataPointer<DynamicContext> &) const
{
    QDateTime dt(from.as<AbstractDateTime>()->toDateTime());
    // TODO DT dt.setDateOnly(true);

    return Date::fromDateTime(dt);
}

Item AbstractDateTimeToTimeCaster::castFrom(const Item &from,
                                            const QExplicitlySharedDataPointer<DynamicContext> &) const
{
    QDateTime dt(from.as<AbstractDateTime>()->toDateTime());
    // TODO DT dt.setDateOnly(false);

    return SchemaTime::fromDateTime(dt);
}

QT_END_NAMESPACE
