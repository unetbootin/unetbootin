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

#include <math.h>

#include "qabstractfloat_p.h"
#include "qatomictype_p.h"
#include "qbuiltintypes_p.h"
#include "qvalidationerror_p.h"

#include "qdecimal_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

Decimal::Decimal(const xsDecimal num) : m_value(num)
{
}

Decimal::Ptr Decimal::fromValue(const xsDecimal num)
{
    return Decimal::Ptr(new Decimal(num));
}

AtomicValue::Ptr Decimal::fromLexical(const QString &strNumeric)
{
    /* QString::toDouble() handles the whitespace facet. */
    const QString strNumericTrimmed(strNumeric.trimmed());

    /* Block these out, as QString::toDouble() supports them. */
    if(strNumericTrimmed.compare(QLatin1String("-INF"), Qt::CaseInsensitive) == 0   ||
       strNumericTrimmed.compare(QLatin1String("INF"), Qt::CaseInsensitive)  == 0   ||
       strNumericTrimmed.compare(QLatin1String("+INF"), Qt::CaseInsensitive)  == 0   ||
       strNumericTrimmed.compare(QLatin1String("nan"), Qt::CaseInsensitive)  == 0)
    {
        return ValidationError::createError();
    }

    bool conversionOk = false;
    const xsDecimal num = strNumericTrimmed.toDouble(&conversionOk);

    if(conversionOk)
        return AtomicValue::Ptr(new Decimal(num));
    else
        return ValidationError::createError();
}

bool Decimal::evaluateEBV(const QExplicitlySharedDataPointer<DynamicContext> &) const
{
    return !Double::isEqual(m_value, 0.0);
}

QString Decimal::stringValue() const
{
    return toString(m_value);
}

QString Decimal::toString(const xsDecimal value)
{
    /*
     * If SV is in the value space of xs:integer, that is, if there are no
     * significant digits after the decimal point, then the value is converted
     * from an xs:decimal to an xs:integer and the resulting xs:integer is
     * converted to an xs:string using the rule above.
     */
    if(Double::isEqual(::floor(value), value))
    {
        /* The static_cast is identical to Integer::toInteger(). */
        return QString::number(static_cast<xsInteger>(value));
    }
    else
    {
        int sign;
        int decimalPoint;
        char *result = 0;
        static_cast<void>(qdtoa(value, 0, 0, &decimalPoint, &sign, 0, &result));
        /* If the copy constructor is used instead of QString::operator=(),
         * it doesn't compile. I have no idea why. */
        const QString qret(QString::fromLatin1(result));
        delete result;

        QString valueAsString;

        if(sign)
            valueAsString += QLatin1Char('-');

        if(0 < decimalPoint)
        {
            valueAsString += qret.left(decimalPoint);
            valueAsString += QLatin1Char('.');
            if (qret.size() <= decimalPoint)
                valueAsString += QLatin1Char('0');
            else
                valueAsString += qret.mid(decimalPoint);
        }
        else
        {
            valueAsString += QLatin1Char('0');
            valueAsString += QLatin1Char('.');

            for(int d = decimalPoint; d < 0; d++)
                valueAsString += QLatin1Char('0');

            valueAsString += qret;
        }

        return valueAsString;
    }
}

ItemType::Ptr Decimal::type() const
{
    return BuiltinTypes::xsDecimal;
}

xsDouble Decimal::toDouble() const
{
    return static_cast<xsDouble>(m_value);
}

xsInteger Decimal::toInteger() const
{
    return static_cast<xsInteger>(m_value);
}

xsFloat Decimal::toFloat() const
{
    return static_cast<xsFloat>(m_value);
}

xsDecimal Decimal::toDecimal() const
{
    return m_value;
}

Numeric::Ptr Decimal::round() const
{
    return Numeric::Ptr(new Decimal(roundFloat(m_value)));
}

Numeric::Ptr Decimal::roundHalfToEven(const xsInteger /*scale*/) const
{
    return Numeric::Ptr();
}

Numeric::Ptr Decimal::floor() const
{
    return Numeric::Ptr(new Decimal(static_cast<xsDecimal>(::floor(m_value))));
}

Numeric::Ptr Decimal::ceiling() const
{
    return Numeric::Ptr(new Decimal(static_cast<xsDecimal>(ceil(m_value))));
}

Numeric::Ptr Decimal::abs() const
{
    return Numeric::Ptr(new Decimal(static_cast<xsDecimal>(fabs(m_value))));
}

bool Decimal::isNaN() const
{
    return false;
}

bool Decimal::isInf() const
{
    return false;
}

Item Decimal::toNegated() const
{
    if(AbstractFloat<true>::isEqual(m_value, 0.0))
        return fromValue(0).data();
    else
        return fromValue(-m_value).data();
}

QT_END_NAMESPACE
