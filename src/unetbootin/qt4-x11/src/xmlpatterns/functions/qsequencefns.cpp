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
#include "qcommonsequencetypes_p.h"
#include "qcommonvalues_p.h"
#include "qdistinctiterator_p.h"
#include "qebvextractor_p.h"
#include "qemptysequence_p.h"
#include "qgenericsequencetype_p.h"
#include "qindexofiterator_p.h"
#include "qinsertioniterator_p.h"
#include "qinteger_p.h"
#include "qremovaliterator_p.h"
#include "qsequencegeneratingfns_p.h"
#include "qsubsequenceiterator_p.h"

#include "qsequencefns_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

bool BooleanFN::evaluateEBV(const DynamicContext::Ptr &context) const
{
    return m_operands.first()->evaluateEBV(context);
}

Expression::Ptr BooleanFN::typeCheck(const StaticContext::Ptr &context,
                                     const SequenceType::Ptr &reqType)
{
    return EBVExtractor::typeCheck<FunctionCall>(context, reqType, this);
}

Item::Iterator::Ptr IndexOfFN::evaluateSequence(const DynamicContext::Ptr &context) const
{
    return Item::Iterator::Ptr(new IndexOfIterator(m_operands.first()->evaluateSequence(context),
                                                   m_operands.at(1)->evaluateSingleton(context),
                                                   comparator(), context,
                                                   ConstPtr(this)));
}

Expression::Ptr IndexOfFN::typeCheck(const StaticContext::Ptr &context,
                                     const SequenceType::Ptr &reqType)
{
    const Expression::Ptr me(FunctionCall::typeCheck(context, reqType));
    const ItemType::Ptr t1(m_operands.first()->staticType()->itemType());
    const ItemType::Ptr t2(m_operands.at(1)->staticType()->itemType());

    if(*CommonSequenceTypes::Empty == *t1 ||
       *CommonSequenceTypes::Empty == *t2)
    {
        return EmptySequence::create(this, context);
    }
    else
    {
        prepareComparison(fetchComparator(t1, t2, context));
        return me;
    }
}

Item::Iterator::Ptr DistinctValuesFN::evaluateSequence(const DynamicContext::Ptr &context) const
{
    return Item::Iterator::Ptr(new DistinctIterator(m_operands.first()->evaluateSequence(context),
                                                    comparator(),
                                                    ConstPtr(this),
                                                    context));
}

Expression::Ptr DistinctValuesFN::typeCheck(const StaticContext::Ptr &context,
                                            const SequenceType::Ptr &reqType)
{
    const Expression::Ptr me(FunctionCall::typeCheck(context, reqType));
    const ItemType::Ptr t1(m_operands.first()->staticType()->itemType());

    if(*CommonSequenceTypes::Empty == *t1)
        return EmptySequence::create(this, context);
    else if(!m_operands.first()->staticType()->cardinality().allowsMany())
        return m_operands.first();
    else if(BuiltinTypes::xsAnyAtomicType->xdtTypeMatches(t1))
        return me;
    else
    {
        prepareComparison(fetchComparator(t1, t1, context));
        return me;
    }
}

SequenceType::Ptr DistinctValuesFN::staticType() const
{
    const SequenceType::Ptr t(m_operands.first()->staticType());
    return makeGenericSequenceType(t->itemType(),
                                   t->cardinality().allowsMany() ? Cardinality::oneOrMore()
                                                                 : Cardinality::exactlyOne());
}

Item::Iterator::Ptr InsertBeforeFN::evaluateSequence(const DynamicContext::Ptr &context) const
{
    const Item::Iterator::Ptr target(m_operands.first()->evaluateSequence(context));
    const Item::Iterator::Ptr inserts(m_operands.at(2)->evaluateSequence(context));

    xsInteger position = m_operands.at(1)->evaluateSingleton(context).as<Numeric>()->toInteger();

    if(position < 1)
        position = 1;

    return Item::Iterator::Ptr(new InsertionIterator(target, position, inserts));
}

Item InsertBeforeFN::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    return evaluateSequence(context)->next();
}

SequenceType::Ptr InsertBeforeFN::staticType() const
{
    const SequenceType::Ptr t1(m_operands.first()->staticType());
    const SequenceType::Ptr t2(m_operands.last()->staticType());

    return makeGenericSequenceType(t1->itemType() | t2->itemType(),
                                   t1->cardinality() + t2->cardinality());
}

Item::Iterator::Ptr RemoveFN::evaluateSequence(const DynamicContext::Ptr &context) const
{
    const xsInteger pos = m_operands.last()->evaluateSingleton(context).as<Numeric>()->toInteger();
    Item::Iterator::Ptr it(m_operands.first()->evaluateSequence(context));

    if(pos < 1)
        return it;

    return Item::Iterator::Ptr(new RemovalIterator(it, pos));
}

Item RemoveFN::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    const xsInteger pos = m_operands.last()->evaluateSingleton(context).as<Numeric>()->toInteger();
    if(pos <= 1)
        return Item();

    return m_operands.first()->evaluateSingleton(context);
}

SequenceType::Ptr RemoveFN::staticType() const
{
    const SequenceType::Ptr opType(m_operands.first()->staticType());
    const Cardinality c(opType->cardinality());

    if(c.minimum() == 0)
        return makeGenericSequenceType(opType->itemType(), c);
    else
    {
        return makeGenericSequenceType(opType->itemType(),
                                       Cardinality::fromRange(c.minimum() - 1,
                                                              c.maximum()));
    }
}

Item::Iterator::Ptr ReverseFN::evaluateSequence(const DynamicContext::Ptr &context) const
{
    return m_operands.first()->evaluateSequence(context)->toReversed();
}

Expression::Ptr ReverseFN::typeCheck(const StaticContext::Ptr &context,
                                     const SequenceType::Ptr &reqType)
{
    if(m_operands.first()->staticType()->cardinality().allowsMany())
        return FunctionCall::typeCheck(context, reqType);
    else
        return m_operands.first()->typeCheck(context, reqType);
}

SequenceType::Ptr ReverseFN::staticType() const
{
    return m_operands.first()->staticType();
}

SubsequenceFN::SubsequenceFN() : m_hasTypeChecked(false)
{
}

Expression::Ptr SubsequenceFN::typeCheck(const StaticContext::Ptr &context,
                                         const SequenceType::Ptr &reqType)
{
    m_hasTypeChecked = true;
    return FunctionCall::typeCheck(context, reqType);
}

Item::Iterator::Ptr SubsequenceFN::evaluateSequence(const DynamicContext::Ptr &context) const
{
    Item::Iterator::Ptr it(m_operands.first()->evaluateSequence(context));

    xsInteger startingLoc = m_operands.at(1)->evaluateSingleton(context).as<Numeric>()->round()->toInteger();
    xsInteger length = -1;

    if(m_operands.count() == 3)
    {
        length = m_operands.last()->evaluateSingleton(context).as<Numeric>()->toInteger();

        if(startingLoc + length < 1 || (startingLoc > (startingLoc + length)))
            return CommonValues::emptyIterator;
    }

    /* F&O, 15.1.10, "If $startingLoc is zero or negative, the
     * subsequence includes items from the beginning of the $sourceSeq." */
    if(startingLoc < 1)
        startingLoc = 1;

    if(length < 1 && length != -1)
        return CommonValues::emptyIterator;
    else
        return Item::Iterator::Ptr(new SubsequenceIterator(it, startingLoc, length));
}

Item SubsequenceFN::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    return evaluateSequence(context)->next();
}

Expression::Ptr SubsequenceFN::compress(const StaticContext::Ptr &context)
{
    const Expression::Ptr me(FunctionCall::compress(context));
    if(me != this)
        return me;

    const Expression::Ptr lenArg(m_operands.value(2));
    if(lenArg && lenArg->isEvaluated())
    {
        const xsInteger length = lenArg->as<Literal>()->item().as<Numeric>()->round()->toInteger();

        if(length <= 0)
            return EmptySequence::create(this, context);
    }

    return me;
}

SequenceType::Ptr SubsequenceFN::staticType() const
{
    const SequenceType::Ptr opType(m_operands.first()->staticType());
    const Cardinality opCard(opType->cardinality());

    /* Optimization: we can do much stronger inference here. If the length is a
     * constant, we can constrain the range at least upwards of the
     * cardinality, for instance. */

    /* The subsequence(expr, 1, 1), add empty-sequence() to the static type.
     *
     * Note that we cannot do all these inferences before we've typechecked our
     * operands. The only known case of where our staticType() is called before
     * typeCheck() is through xmlpatternsview, although it wouldn't be
     * surprising if the more exotic paths can achieve that too.
     */
    if(m_hasTypeChecked &&
       m_operands.at(1)->isEvaluated()                                                      &&
       m_operands.count() == 3                                                              &&
       m_operands.at(2)->isEvaluated()                                                      &&
       m_operands.at(1)->as<Literal>()->item().as<Numeric>()->round()->toInteger() == 1     &&
       m_operands.at(2)->as<Literal>()->item().as<Numeric>()->round()->toInteger() == 1)
    {
        return makeGenericSequenceType(opType->itemType(),
                                       opCard.toWithoutMany());
    }
    else
    {
        return makeGenericSequenceType(opType->itemType(),
                                       opCard | Cardinality::zeroOrOne());
    }
}

Expression::Ptr DocFN::typeCheck(const StaticContext::Ptr &context,
                                 const SequenceType::Ptr &reqType)
{
    /* See the doxygen documentation for this function for the explanation
     * to why this implementation is here, as opposed to in
     * qsequencegeneratingfns.cpp. */

    Q_ASSERT(context);

    prepareStaticBaseURI(context);

    const Expression::Ptr uriOp(m_operands.first());

    if(!uriOp->isEvaluated())
        return Expression::Ptr(FunctionCall::typeCheck(context, reqType));

    const Item uriItem(uriOp->evaluateSingleton(context->dynamicContext()));

    if(!uriItem)
        return EmptySequence::create(this, context)->typeCheck(context, reqType); // TODO test this

    /* These two lines were previously in a separate function but are now duplicated
     * in DocFN::evaluateSingleton(), as part of a workaround for solaris-cc-64. */
    const QUrl mayRela(AnyURI::toQUrl<ReportContext::FODC0005>(uriItem.stringValue(), context, this));
    const QUrl uri(context->resolveURI(mayRela, staticBaseURI()));

    /* The URI is supplied statically, so, let's try to be clever. */
    Q_ASSERT_X(context->resourceLoader(), Q_FUNC_INFO,
               "No resource loader is set in the StaticContext.");
    m_type = context->resourceLoader()->announceDocument(uri, ResourceLoader::MayUse);

    if(m_type)
    {
        Q_ASSERT(CommonSequenceTypes::ZeroOrOneDocumentNode->matches(m_type));
        return Expression::Ptr(FunctionCall::typeCheck(context, reqType));
    }
    else
    {
        context->error(QtXmlPatterns::tr("It will not be possible to retrieve %1.").arg(formatURI(uri)),
                       ReportContext::FODC0002, this);
        return Expression::Ptr();
    }
}

QT_END_NAMESPACE
