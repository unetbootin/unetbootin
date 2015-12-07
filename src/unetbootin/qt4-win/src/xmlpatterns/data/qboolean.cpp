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

#include "qatomictype_p.h"
#include "qbuiltintypes_p.h"
#include "qcommonvalues_p.h"
#include "qdynamiccontext_p.h"
#include "qpatternistlocale_p.h"
#include "qvalidationerror_p.h"

#include "qboolean_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

bool Boolean::evaluateEBV(const Item::Iterator::Ptr &it,
                          const QExplicitlySharedDataPointer<DynamicContext> &context)
{
    return evaluateEBV(it->next(), it, context);
}

bool Boolean::evaluateEBV(const Item &first,
                          const Item::Iterator::Ptr &it,
                          const QExplicitlySharedDataPointer<DynamicContext> &context)
{
    Q_ASSERT(it);
    Q_ASSERT(context);

    if(!first)
        return false;
    else if(first.isNode())
        return true;

    const Item second(it->next());

    if(second)
    {
        Q_ASSERT(context);
        context->error(QtXmlPatterns::tr("Effective Boolean Value cannot be calculated for a sequence "
                          "containing two or more atomic values."),
                          ReportContext::FORG0006,
                          QSourceLocation());
        return false;
    }
    else
        return first.as<AtomicValue>()->evaluateEBV(context);
}

bool Boolean::evaluateEBV(const Item &item,
                          const QExplicitlySharedDataPointer<DynamicContext> &context)
{
    if(!item)
        return false;
    else if(item.isNode())
        return true;
    else
        return item.as<AtomicValue>()->evaluateEBV(context);
}

Boolean::Boolean(const bool value) : m_value(value)
{
}

QString Boolean::stringValue() const
{
    return m_value
            ? CommonValues::TrueString->stringValue()
            : CommonValues::FalseString->stringValue();
}

bool Boolean::evaluateEBV(const QExplicitlySharedDataPointer<DynamicContext> &) const
{
    return m_value;
}

Boolean::Ptr Boolean::fromValue(const bool value)
{
    return value ? CommonValues::BooleanTrue : CommonValues::BooleanFalse;
}

AtomicValue::Ptr Boolean::fromLexical(const QString &lexical)
{
    const QString val(lexical.trimmed()); /* Apply the whitespace facet. */

    if(val == QLatin1String("true") || val == QChar(QLatin1Char('1')))
        return CommonValues::BooleanTrue;
    else if(val == QLatin1String("false") || val == QChar(QLatin1Char('0')))
        return CommonValues::BooleanFalse;
    else
        return ValidationError::createError();
}

ItemType::Ptr Boolean::type() const
{
    return BuiltinTypes::xsBoolean;
}

QT_END_NAMESPACE
