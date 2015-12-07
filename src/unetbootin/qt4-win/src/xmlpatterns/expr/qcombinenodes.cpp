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
#include "qexceptiterator_p.h"
#include "qgenericsequencetype_p.h"
#include "qintersectiterator_p.h"
#include "qitemmappingiterator_p.h"
#include "qnodesortexpression_p.h"
#include "qunioniterator_p.h"

#include "qcombinenodes_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

CombineNodes::CombineNodes(const Expression::Ptr &operand1,
                           const Operator op,
                           const Expression::Ptr &operand2) : PairContainer(operand1, operand2),
                                                              m_operator(op)
{
    Q_ASSERT(op == Union    ||
             op == Except   ||
             op == Intersect);
}

Item::Iterator::Ptr CombineNodes::evaluateSequence(const DynamicContext::Ptr &context) const
{
    const Item::Iterator::Ptr op1(m_operand1->evaluateSequence(context));
    const Item::Iterator::Ptr op2(m_operand2->evaluateSequence(context));

    switch(m_operator)
    {
        case Intersect:
            return Item::Iterator::Ptr(new IntersectIterator(op1, op2));
        case Except:
            return Item::Iterator::Ptr(new ExceptIterator(op1, op2));
        default:
        {
            Q_ASSERT(m_operator == Union);
            return Item::Iterator::Ptr(new UnionIterator(op1, op2));
        }
    }
}

Expression::Ptr CombineNodes::typeCheck(const StaticContext::Ptr &context,
                                        const SequenceType::Ptr &reqType)
{

    const Expression::Ptr me(PairContainer::typeCheck(context, reqType));

    m_operand1 = NodeSortExpression::wrapAround(m_operand1, context);
    m_operand2 = NodeSortExpression::wrapAround(m_operand2, context);

    return me;
}

bool CombineNodes::evaluateEBV(const DynamicContext::Ptr &context) const
{
    /* If it's the union operator, we can possibly avoid
     * evaluating the second operand. */
    if(m_operator == Union)
    {
        return m_operand1->evaluateEBV(context) ||
               m_operand2->evaluateEBV(context);
    }
    else
        return PairContainer::evaluateEBV(context);
}

QString CombineNodes::displayName(const Operator op)
{
    switch(op)
    {
        case Intersect:
            return QLatin1String("intersect");
        case Except:
            return QLatin1String("except");
        default:
        {
            Q_ASSERT(op == Union);
            return QLatin1String("union");
        }
    }
}

SequenceType::Ptr CombineNodes::staticType() const
{
    /* If you change the inference to be narrow, write tests that triggers CombineNodes::evaluateSingleton()
     * to be called. That's right, it doesn't exist, which means we will fallback to evaluateEBV() which
     * isn't what we want in that case. */
    return CommonSequenceTypes::ZeroOrMoreNodes;
}

SequenceType::List CombineNodes::expectedOperandTypes() const
{
    SequenceType::List result;
    result.append(CommonSequenceTypes::ZeroOrMoreNodes);
    result.append(CommonSequenceTypes::ZeroOrMoreNodes);
    return result;
}

CombineNodes::Operator CombineNodes::operatorID() const
{
    return m_operator;
}

ExpressionVisitorResult::Ptr CombineNodes::accept(const ExpressionVisitor::Ptr &visitor) const
{
    return visitor->visit(this);
}

QT_END_NAMESPACE
