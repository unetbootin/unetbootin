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

#include "qboolean_p.h"
#include "qcommonvalues_p.h"
#include "qliteral_p.h"
#include "qatomicstring_p.h"

#include "qsubstringfns_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

Item ContainsFN::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    const Item op1(m_operands.first()->evaluateSingleton(context));
    QString str1;

    if(op1)
        str1 = op1.stringValue();

    const Item op2(m_operands.at(1)->evaluateSingleton(context));
    QString str2;

    if(op2)
        str2 = op2.stringValue();

    if(str2.isEmpty())
        return CommonValues::BooleanTrue;

    if(str1.isEmpty())
        return CommonValues::BooleanFalse;

    return Boolean::fromValue(str1.contains(str2, caseSensitivity()));
}

Item StartsWithFN::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    const Item op1(m_operands.first()->evaluateSingleton(context));
    QString str1;

    if(op1)
        str1 = op1.stringValue();

    const Item op2(m_operands.at(1)->evaluateSingleton(context));
    QString str2;

    if(op2)
        str2 = op2.stringValue();

    if(str2.isEmpty())
        return CommonValues::BooleanTrue;

    if(str1.isEmpty())
        return CommonValues::BooleanFalse;

    return Boolean::fromValue(str1.startsWith(str2, caseSensitivity()));
}

Item EndsWithFN::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    const Item op1(m_operands.first()->evaluateSingleton(context));
    QString str1;

    if(op1)
        str1 = op1.stringValue();

    const Item op2(m_operands.at(1)->evaluateSingleton(context));
    QString str2;

    if(op2)
        str2 = op2.stringValue();

    if(str2.isEmpty())
        return CommonValues::BooleanTrue;

    if(str1.isEmpty())
        return CommonValues::BooleanFalse;

    return Boolean::fromValue(str1.endsWith(str2, caseSensitivity()));
}

Item SubstringBeforeFN::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    const Item op1(m_operands.first()->evaluateSingleton(context));
    QString str1;

    if(op1)
        str1 = op1.stringValue();

    const Item op2(m_operands.at(1)->evaluateSingleton(context));
    QString str2;

    if(op2)
        str2 = op2.stringValue();

    const int pos = str1.indexOf(str2);
    if(pos == -1)
        return CommonValues::EmptyString;

    return AtomicString::fromValue(QString(str1.left(pos)));
}

Item SubstringAfterFN::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    const Item op1(m_operands.first()->evaluateSingleton(context));
    QString str1;

    if(op1)
        str1 = op1.stringValue();

    const Item op2(m_operands.at(1)->evaluateSingleton(context));
    QString str2;

    if(op2)
        str2 = op2.stringValue();

    if(str2.isEmpty())
    {
        if(op1)
            return op1;
        else
            return CommonValues::EmptyString;
    }

    const int pos = str1.indexOf(str2);

    if(pos == -1)
        return CommonValues::EmptyString;

    return AtomicString::fromValue(QString(str1.right(str1.length() - (pos + str2.length()))));
}

QT_END_NAMESPACE
