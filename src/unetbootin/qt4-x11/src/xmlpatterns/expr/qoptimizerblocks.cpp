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

#include "qcommonnamespaces_p.h"

#include "qcommonsequencetypes_p.h"
#include "qfunctionfactory_p.h"
#include "qgeneralcomparison_p.h"
#include "qliteral_p.h"
#include "qschemanumeric_p.h"
#include "qvaluecomparison_p.h"

#include "qoptimizerblocks_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

ByIDIdentifier::ByIDIdentifier(const Expression::ID id) : m_id(id)
{
}

bool ByIDIdentifier::matches(const Expression::Ptr &expr) const
{
    return expr->is(m_id);
}

ComparisonIdentifier::ComparisonIdentifier(const QVector<Expression::ID> hosts,
                                           const AtomicComparator::Operator op) : m_hosts(hosts),
                                                                                  m_op(op)
{
}

bool ComparisonIdentifier::matches(const Expression::Ptr &e) const
{
    const Expression::ID eID = e->id();

    if(eID == Expression::IDGeneralComparison)
    {
        if(m_hosts.contains(Expression::IDGeneralComparison))
            return e->as<GeneralComparison>()->operatorID() == m_op;
        else
            return false;
    }
    else if(eID == Expression::IDValueComparison)
    {
        if(m_hosts.contains(Expression::IDValueComparison))
            return e->as<ValueComparison>()->operatorID() == m_op;
        else
            return false;
    }
    else
        return false;
}

BySequenceTypeIdentifier::BySequenceTypeIdentifier(const SequenceType::Ptr &seqType) : m_seqType(seqType)
{
    Q_ASSERT(seqType);
}

bool BySequenceTypeIdentifier::matches(const Expression::Ptr &expr) const
{
    const SequenceType::Ptr t(expr->staticType());

    return m_seqType->itemType()->xdtTypeMatches(t->itemType())
           &&
           m_seqType->cardinality().isMatch(t->cardinality());
}

IntegerIdentifier::IntegerIdentifier(const xsInteger num) : m_num(num)
{
}

bool IntegerIdentifier::matches(const Expression::Ptr &expr) const
{
    return expr->is(Expression::IDIntegerValue) &&
           expr->as<Literal>()->item().as<Numeric>()->toInteger() == m_num;
}

BooleanIdentifier::BooleanIdentifier(const bool value) : m_value(value)
{
}

bool BooleanIdentifier::matches(const Expression::Ptr &expr) const
{
    return expr->is(Expression::IDBooleanValue) &&
           expr->evaluateEBV(DynamicContext::Ptr()) == m_value;
}

ByIDCreator::ByIDCreator(const Expression::ID id) : m_id(id)
{
    Q_ASSERT(id != Expression::IDIgnorableExpression);
}

Expression::Ptr ByIDCreator::create(const Expression::List &operands,
                                    const StaticContext::Ptr &context,
                                    const SourceLocationReflection *const r) const
{
    return create(m_id, operands, context, r);
}

Expression::Ptr ByIDCreator::create(const Expression::ID id,
                                    const Expression::List &operands,
                                    const StaticContext::Ptr &context,
                                    const SourceLocationReflection *const r)
{
    Q_ASSERT(context);

    QXmlName::LocalNameCode fnName;

    switch(id)
    {
        case Expression::IDExistsFN:
        {
            fnName = StandardLocalNames::exists;
            break;
        }
        case Expression::IDEmptyFN:
        {
            fnName = StandardLocalNames::empty;
            break;
        }
        default:
        {
            Q_ASSERT_X(false, Q_FUNC_INFO,
                       "Cannot create an expression of requested type; m_id is wrong.");
            return Expression::Ptr();
        }
    }

    /* The reason we don't simply do 'new ExistsFN()' ourselves, is that all FunctionCall
     * instances needs their FunctionSignature in order to function, and the FunctionFactories
     * sets that. */
    const QXmlName qName(StandardNamespaces::fn, fnName);

    const Expression::Ptr result(context->functionSignatures()->createFunctionCall(qName, operands, context, r));
    context->addLocation(result.data(), context->locationFor(r));
    return result;
}

QT_END_NAMESPACE
