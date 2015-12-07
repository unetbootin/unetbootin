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
#include "qitem_p.h"
#include "qvalidationerror_p.h"

#include "qinteger_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

Item Integer::fromValue(const xsInteger num)
{
    return toItem(Integer::Ptr(new Integer(num)));
}

AtomicValue::Ptr Integer::fromLexical(const QString &strNumeric)
{
    bool conversionOk = false;
    const xsInteger num = strNumeric.toLongLong(&conversionOk);

    if(conversionOk)
        return AtomicValue::Ptr(new Integer(num));
    else
        return ValidationError::createError();
}

Integer::Integer(const xsInteger num) : m_value(num)
{
}

bool Integer::evaluateEBV(const QExplicitlySharedDataPointer<DynamicContext> &) const
{
    return m_value != 0;
}

QString Integer::stringValue() const
{
    return QString::number(m_value);
}

ItemType::Ptr Integer::type() const
{
    return BuiltinTypes::xsInteger;
}

xsDouble Integer::toDouble() const
{
    return static_cast<xsDouble>(m_value);
}

xsInteger Integer::toInteger() const
{
    return m_value;
}

xsFloat Integer::toFloat() const
{
    return static_cast<xsFloat>(m_value);
}

xsDecimal Integer::toDecimal() const
{
    return static_cast<xsDecimal>(m_value);
}

Numeric::Ptr Integer::round() const
{
    /* xs:integerS never has a mantissa. */
    return Numeric::Ptr(const_cast<Integer *>(this));
}

Numeric::Ptr Integer::roundHalfToEven(const xsInteger /*scale*/) const
{
    return Numeric::Ptr(const_cast<Integer *>(this));
}

Numeric::Ptr Integer::floor() const
{
    return Numeric::Ptr(const_cast<Integer *>(this));
}

Numeric::Ptr Integer::ceiling() const
{
    return Numeric::Ptr(const_cast<Integer *>(this));
}

Numeric::Ptr Integer::abs() const
{
    /* No reason to allocate an Integer if we're already absolute. */
    if(m_value < 0)
        return Numeric::Ptr(new Integer(qAbs(m_value)));
    else
        return Numeric::Ptr(const_cast<Integer *>(this));
}

bool Integer::isNaN() const
{
    return false;
}

bool Integer::isInf() const
{
    return false;
}

Item Integer::toNegated() const
{
    return fromValue(-m_value);
}

bool Integer::isSigned() const
{
    return true;
}

qulonglong Integer::toUnsignedInteger() const
{
    Q_ASSERT_X(false, Q_FUNC_INFO,
               "It makes no sense to call this function, see Numeric::toUnsignedInteger().");
    return 0;
}

QT_END_NAMESPACE
