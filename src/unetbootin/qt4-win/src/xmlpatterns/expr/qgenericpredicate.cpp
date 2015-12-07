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

#include "qabstractfloat_p.h"
#include "qboolean_p.h"
#include "qbuiltintypes_p.h"
#include "qcommonsequencetypes_p.h"
#include "qemptysequence_p.h"
#include "qfirstitempredicate_p.h"
#include "qgenericsequencetype_p.h"
#include "qitemmappingiterator_p.h"
#include "qliteral_p.h"
#include "qpatternistlocale_p.h"
#include "qtruthpredicate_p.h"

#include "qgenericpredicate_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

GenericPredicate::GenericPredicate(const Expression::Ptr &sourceExpression,
                                   const Expression::Ptr &predicate) : PairContainer(sourceExpression,
                                                                                     predicate)
{
}

Expression::Ptr GenericPredicate::create(const Expression::Ptr &sourceExpression,
                                         const Expression::Ptr &predicateExpression,
                                         const StaticContext::Ptr &context,
                                         const QSourceLocation &location)
{
    Q_ASSERT(sourceExpression);
    Q_ASSERT(predicateExpression);
    Q_ASSERT(context);
    const ItemType::Ptr type(predicateExpression->staticType()->itemType());

    if(predicateExpression->is(IDIntegerValue) &&
       predicateExpression->as<Literal>()->item().as<Numeric>()->toInteger() == 1)
    { /* Handle [1] */
        return Expression::Ptr(new FirstItemPredicate(sourceExpression));
    }
    else if(BuiltinTypes::numeric->xdtTypeMatches(type))
    { /* A numeric predicate, other than [1]. */
        /* TODO at somepoint we'll return a specialized expr here, NumericPredicate or so.
         * Dependency analysis is a bit tricky, since the contained expression can depend on
         * some loop component. */
        return Expression::Ptr(new GenericPredicate(sourceExpression, predicateExpression));
    }
    else if(*CommonSequenceTypes::Empty == *type)
    {
        return EmptySequence::create(predicateExpression.data(), context);
    }
    else if(*BuiltinTypes::item == *type ||
            *BuiltinTypes::xsAnyAtomicType == *type)
    {
        /* The type couldn't be narrowed at compile time, so we use
         * a generic predicate. This check is before the CommonSequenceTypes::EBV check,
         * because the latter matches these types as well. */
        return Expression::Ptr(new GenericPredicate(sourceExpression, predicateExpression));
    }
    else if(CommonSequenceTypes::EBV->itemType()->xdtTypeMatches(type))
    {
        return Expression::Ptr(new TruthPredicate(sourceExpression, predicateExpression));
    }
    else
    {
        context->error(QtXmlPatterns::tr("A value of type %1 cannot be a "
                                         "predicate. A predicate must have "
                                         "either a numeric type or an "
                                         "Effective Boolean Value type.")
                       .arg(formatType(context->namePool(),
                                       sourceExpression->staticType())),
                       ReportContext::FORG0006, location);
        return Expression::Ptr(); /* Silence compiler warning. */
    }
}

Item GenericPredicate::mapToItem(const Item &item,
                                      const DynamicContext::Ptr &context) const
{
    const Item::Iterator::Ptr it(m_operand2->evaluateSequence(context));
    const Item pcateItem(it->next());

    if(!pcateItem)
        return Item(); /* The predicate evaluated to the empty sequence */
    else if(pcateItem.isNode())
        return item;
    /* Ok, now it must be an AtomicValue */
    else if(BuiltinTypes::numeric->xdtTypeMatches(pcateItem.type()))
    { /* It's a positional predicate. */
        if(it->next())
        {
            context->error(QtXmlPatterns::tr("A positional predicate must "
                                             "evaluate to a single numeric "
                                             "value."),
                              ReportContext::FORG0006, this);
            return Item();
        }

        if(Double::isEqual(static_cast<xsDouble>(context->contextPosition()),
                           pcateItem.as<Numeric>()->toDouble()))
        {
            return item;
        }
        else
            return Item();
    }
    else if(Boolean::evaluateEBV(pcateItem, it, context)) /* It's a truth predicate. */
        return item;
    else
        return Item();
}

Item::Iterator::Ptr GenericPredicate::evaluateSequence(const DynamicContext::Ptr &context) const
{
    const Item::Iterator::Ptr focus(m_operand1->evaluateSequence(context));
    const DynamicContext::Ptr newContext(context->createFocus());
    newContext->setFocusIterator(focus);

    return makeItemMappingIterator<Item>(ConstPtr(this),
                                         focus,
                                         newContext);
}

Item GenericPredicate::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    const Item::Iterator::Ptr focus(m_operand1->evaluateSequence(context));
    const DynamicContext::Ptr newContext(context->createFocus());
    newContext->setFocusIterator(focus);
    return mapToItem(focus->next(), newContext);
}

SequenceType::List GenericPredicate::expectedOperandTypes() const
{
    SequenceType::List result;
    result.append(CommonSequenceTypes::ZeroOrMoreItems);
    result.append(CommonSequenceTypes::ZeroOrMoreItems);
    return result;
}

SequenceType::Ptr GenericPredicate::staticType() const
{
    const SequenceType::Ptr type(m_operand1->staticType());
    return makeGenericSequenceType(type->itemType(),
                                   type->cardinality() | Cardinality::zeroOrOne());
}

ExpressionVisitorResult::Ptr GenericPredicate::accept(const ExpressionVisitor::Ptr &visitor) const
{
    return visitor->visit(this);
}

ItemType::Ptr GenericPredicate::newFocusType() const
{
    return m_operand1->staticType()->itemType();
}

Expression::Properties GenericPredicate::properties() const
{
    return CreatesFocusForLast;
}

QString GenericPredicate::description() const
{
    return QLatin1String("predicate");
}

QT_END_NAMESPACE
