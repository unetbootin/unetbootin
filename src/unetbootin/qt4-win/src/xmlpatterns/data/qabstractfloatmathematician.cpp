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

