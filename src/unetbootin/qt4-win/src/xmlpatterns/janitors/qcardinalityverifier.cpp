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
#include "qcommonvalues_p.h"
#include "qgenericsequencetype_p.h"
#include "qinsertioniterator_p.h"
#include "qpatternistlocale_p.h"

#include "qcardinalityverifier_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

QString CardinalityVerifier::wrongCardinality(const Cardinality &req,
                                              const Cardinality &got)
{
    return QtXmlPatterns::tr("Required cardinality is %1; got cardinality %2.")
                .arg(formatType(req), formatType(got));
}

Expression::Ptr CardinalityVerifier::verifyCardinality(const Expression::Ptr &operand,
                                                       const Cardinality &requiredCard,
                                                       const ReportContext::Ptr &context,
                                                       const ReportContext::ErrorCode code)
{
    const Cardinality opCard(operand->staticType()->cardinality());

    if(requiredCard.isMatch(opCard))
        return operand;
    else if(requiredCard.canMatch(opCard))
        return Expression::Ptr(new CardinalityVerifier(operand, requiredCard, code));
    else
    {
        /* Sequences within this cardinality can never match. */
        context->error(wrongCardinality(requiredCard, opCard), code, operand.data());
        return operand;
    }
}

CardinalityVerifier::CardinalityVerifier(const Expression::Ptr &operand,
                                         const Cardinality &card,
                                         const ReportContext::ErrorCode code)
                                         : SingleContainer(operand),
                                           m_reqCard(card),
                                           m_allowsMany(operand->staticType()->cardinality().allowsMany()),
                                           m_errorCode(code)
{
    Q_ASSERT_X(m_reqCard != Cardinality::zeroOrMore(), Q_FUNC_INFO,
               "It makes no sense to use CardinalityVerifier for cardinality zero-or-more.");
}

Item::Iterator::Ptr CardinalityVerifier::evaluateSequence(const DynamicContext::Ptr &context) const
{
    const Item::Iterator::Ptr it(m_operand->evaluateSequence(context));
    const Item next(it->next());

    if(next)
    {
        const Item next2(it->next());

        if(next2)
        {
            if(m_reqCard.allowsMany())
            {
                Item::List start;
                start.append(next);
                start.append(next2);

                return Item::Iterator::Ptr(new InsertionIterator(it, 1, makeListIterator(start)));
            }
            else
            {
                context->error(wrongCardinality(m_reqCard, Cardinality::twoOrMore()), m_errorCode, this);
                return CommonValues::emptyIterator;
            }
        }
        else
            return makeSingletonIterator(next);
    }
    else
    {
        if(m_reqCard.allowsEmpty())
            return CommonValues::emptyIterator;
        else
        {
            context->error(wrongCardinality(m_reqCard, Cardinality::twoOrMore()), m_errorCode, this);
            return CommonValues::emptyIterator;
        }
    }
}

Item CardinalityVerifier::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    if(m_allowsMany)
    {
        const Item::Iterator::Ptr it(m_operand->evaluateSequence(context));
        const Item item(it->next());

        if(item)
        {
            if(it->next())
            {
                context->error(wrongCardinality(m_reqCard, Cardinality::twoOrMore()),
                               m_errorCode, this);
                return Item();
            }
            else
                return item;
        }
        else if(m_reqCard.allowsEmpty())
            return Item();
        else
        {
            context->error(wrongCardinality(m_reqCard), m_errorCode, this);
            return Item();
        }
    }
    else
    {
        const Item item(m_operand->evaluateSingleton(context));

        if(item)
            return item;
        else if(m_reqCard.allowsEmpty())
            return Item();
        else
        {
            context->error(wrongCardinality(m_reqCard), m_errorCode, this);
            return Item();
        }
    }
}

const SourceLocationReflection *CardinalityVerifier::actualReflection() const
{
    return m_operand->actualReflection();
}

Expression::Ptr CardinalityVerifier::typeCheck(const StaticContext::Ptr &context,
                                               const SequenceType::Ptr &reqType)
{
    if(m_reqCard.isMatch(m_operand->staticType()->cardinality()))
        return m_operand->typeCheck(context, reqType);
    else
        return SingleContainer::typeCheck(context, reqType);
}

SequenceType::List CardinalityVerifier::expectedOperandTypes() const
{
    SequenceType::List result;
    result.append(CommonSequenceTypes::ZeroOrMoreItems);
    return result;
}

SequenceType::Ptr CardinalityVerifier::staticType() const
{
    return makeGenericSequenceType(m_operand->staticType()->itemType(), m_reqCard);
}

ExpressionVisitorResult::Ptr CardinalityVerifier::accept(const ExpressionVisitor::Ptr &visitor) const
{
    return visitor->visit(this);
}

QT_END_NAMESPACE
