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

#include <limits>

#include "qabstractfloat_p.h"
#include "qanyuri_p.h"
#include "qboolean_p.h"
#include "qdecimal_p.h"
#include "qinteger_p.h"
#include "qatomicstring_p.h"
#include "quntypedatomic_p.h"

#include "qcommonvalues_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

// STATIC DATA
const AtomicString::Ptr               CommonValues::EmptyString
                                    (new AtomicString(QString(QLatin1String(""))));
const AtomicString::Ptr               CommonValues::TrueString
                                    (new AtomicString(QLatin1String("true")));
const AtomicString::Ptr               CommonValues::FalseString
                                    (new AtomicString(QLatin1String("false")));

const UntypedAtomic::Ptr        CommonValues::UntypedAtomicTrue
                                    (new UntypedAtomic(QLatin1String("true")));
const UntypedAtomic::Ptr        CommonValues::UntypedAtomicFalse
                                    (new UntypedAtomic(QLatin1String("false")));

const AtomicValue::Ptr              CommonValues::BooleanTrue
                                    (new Boolean(true));
const AtomicValue::Ptr              CommonValues::BooleanFalse(new Boolean(false));

const AtomicValue::Ptr               CommonValues::DoubleNaN
                                    (Double::fromValue(std::numeric_limits<xsDouble>::quiet_NaN()));

const AtomicValue::Ptr                CommonValues::FloatNaN
                                    (Float::fromValue(std::numeric_limits<xsFloat>::quiet_NaN()));

const Item                          CommonValues::IntegerZero
                                    (Integer::fromValue(0));

const AtomicValue::Ptr               CommonValues::EmptyAnyURI
                                    (AnyURI::fromValue(QLatin1String("")));

const AtomicValue::Ptr               CommonValues::DoubleOne
                                    (Double::fromValue(1));
const AtomicValue::Ptr                CommonValues::FloatOne
                                    (Float::fromValue(1));
const AtomicValue::Ptr              CommonValues::DecimalOne
                                    (Decimal::fromValue(1));
const Item                          CommonValues::IntegerOne
                                    (Integer::fromValue(1));
const Item                          CommonValues::IntegerOneNegative
                                    (Integer::fromValue(-1));

const AtomicValue::Ptr               CommonValues::DoubleZero
                                    (Double::fromValue(0));
const AtomicValue::Ptr                CommonValues::FloatZero
                                    (Float::fromValue(0));
const AtomicValue::Ptr              CommonValues::DecimalZero
                                    (Decimal::fromValue(0));

const Item::EmptyIterator::Ptr  CommonValues::emptyIterator
                                    (new Item::EmptyIterator());

const AtomicValue::Ptr               CommonValues::NegativeInfDouble
                                    (Double::fromValue(-std::numeric_limits<xsDouble>::infinity()));
const AtomicValue::Ptr               CommonValues::InfDouble
                                    (Double::fromValue(std::numeric_limits<xsDouble>::infinity()));
const AtomicValue::Ptr                CommonValues::NegativeInfFloat
                                    (Float::fromValue(-std::numeric_limits<xsFloat>::infinity()));
const AtomicValue::Ptr                CommonValues::InfFloat
                                    (Float::fromValue(std::numeric_limits<xsFloat>::infinity()));

const DayTimeDuration::Ptr      CommonValues::DayTimeDurationZero
                                    (DayTimeDuration::fromSeconds(0));
const DayTimeDuration::Ptr      CommonValues::YearMonthDurationZero
                                    (YearMonthDuration::fromComponents(true, 0, 0));


QT_END_NAMESPACE
