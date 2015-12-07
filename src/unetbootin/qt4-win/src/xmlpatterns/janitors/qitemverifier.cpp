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

#include "qcommonsequencetypes_p.h"
#include "qgenericsequencetype_p.h"
#include "qitemmappingiterator_p.h"
#include "qpatternistlocale_p.h"

#include "qitemverifier_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

ItemVerifier::ItemVerifier(const Expression::Ptr &operand,
                           const ItemType::Ptr &reqType,
                           const ReportContext::ErrorCode errorCode) : SingleContainer(operand),
                                                                       m_reqType(reqType),
                                                                       m_errorCode(errorCode)
{
    Q_ASSERT(reqType);
}

void ItemVerifier::verifyItem(const Item &item, const DynamicContext::Ptr &context) const
{
    if(m_reqType->itemMatches(item))
        return;

    context->error(QtXmlPatterns::tr("The item %1 did not match the required type %2.")
                                    .arg(formatData(item.stringValue()),
                                         formatType(context->namePool(), m_reqType)),
                   m_errorCode,
                   this);
}

const SourceLocationReflection *ItemVerifier::actualReflection() const
{
    return m_operand->actualReflection();
}

Item ItemVerifier::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    const Item item(m_operand->evaluateSingleton(context));

    if(item)
    {
        verifyItem(item, context);
        return item;
    }
    else
        return Item();
}

Item ItemVerifier::mapToItem(const Item &item, const DynamicContext::Ptr &context) const
{
    verifyItem(item, context);
    return item;
}

Item::Iterator::Ptr ItemVerifier::evaluateSequence(const DynamicContext::Ptr &context) const
{
    return makeItemMappingIterator<Item>(ConstPtr(this),
                                         m_operand->evaluateSequence(context),
                                         context);
}

SequenceType::Ptr ItemVerifier::staticType() const
{
    return makeGenericSequenceType(m_reqType, m_operand->staticType()->cardinality());
}

SequenceType::List ItemVerifier::expectedOperandTypes() const
{
    SequenceType::List result;
    result.append(CommonSequenceTypes::ZeroOrMoreItems);
    return result;
}

ExpressionVisitorResult::Ptr ItemVerifier::accept(const ExpressionVisitor::Ptr &visitor) const
{
    return visitor->visit(this);
}

QT_END_NAMESPACE
