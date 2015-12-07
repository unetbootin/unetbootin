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

/*
 * @file
 * @short This file is included by qatomicmathematicians_p.h
 * if you need some includes, put them in qabstractfloatmathematician_p.h, outside of the namespace.
 */

template <const bool isDouble>
Item AbstractFloatMathematician<isDouble>::calculate(const Item &o1,
                                                          const Operator op,
                                                          const Item &o2,
                                                          const QExplicitlySharedDataPointer<DynamicContext> &context) const
{
    const Numeric *const num1 = o1.template as<Numeric>();
    const Numeric *const num2 = o2.template as<Numeric>();
    switch(op)
    {
        case Div:
            return toItem(AbstractFloat<isDouble>::fromValue(num1->toDouble() / num2->toDouble()));
        case IDiv:
        {
            if(num1->isNaN() || num2->isNaN())
            {
                context->error(QtXmlPatterns::tr("No operand in an integer division, %1, can be %2.")
                                  .arg(formatKeyword("idiv"))
                                  .arg(formatData("NaN")),
                               ReportContext::FOAR0002, this);
            }
            else if(num1->isInf())
            {
                context->error(QtXmlPatterns::tr("The first operand in an integer division, %1, cannot be infinity (%2).")
                                                 .arg(formatKeyword("idiv"))
                                                 .arg(formatData("INF")),
                               ReportContext::FOAR0002, this);
            }
            else if(num2->toInteger() == 0)
                context->error(QtXmlPatterns::tr("The second operand in a division, %1, cannot be zero (%2).")
                                                 .arg(formatKeyword("idiv"))
                                                 .arg(formatData("0")),
                               ReportContext::FOAR0001, this);

            return Integer::fromValue(static_cast<xsInteger>(num1->toDouble() / num2->toDouble()));
        }
        case Substract:
            return toItem(AbstractFloat<isDouble>::fromValue(num1->toDouble() - num2->toDouble()));
        case Mod:
            return toItem(AbstractFloat<isDouble>::fromValue(::fmod(num1->toDouble(), num2->toDouble())));
        case Multiply:
            return toItem(AbstractFloat<isDouble>::fromValue(num1->toDouble() * num2->toDouble()));
        case Add:
            return toItem(AbstractFloat<isDouble>::fromValue(num1->toDouble() + num2->toDouble()));
    }

    Q_ASSERT(false);
    return Item(); /* GCC unbarfer. */
}

