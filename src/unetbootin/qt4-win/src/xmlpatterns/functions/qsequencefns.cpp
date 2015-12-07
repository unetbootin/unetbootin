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

#include "qbuiltintypes_p.h"
#include "qcommonsequencetypes_p.h"
#include "qcommonvalues_p.h"
#include "qdistinctiterator_p.h"
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
    if(*CommonSequenceTypes::EBV->itemType() == *reqType->itemType())
        return m_operands.first()->typeCheck(context, reqType);
    else
        return FunctionCall::typeCheck(context, reqType);
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
    if(me.data() != this)
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

    /* The subsequence(expr, 1, 1), add empty-sequence() to the static type. */
    if(m_operands.at(1)->isEvaluated()                                                      &&
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

    Cardinality card;

    if(opCard.isEmpty())
        card = Cardinality::empty();
    else
        card = opCard.toWithoutMany();

    return makeGenericSequenceType(opType->itemType(), card);
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
