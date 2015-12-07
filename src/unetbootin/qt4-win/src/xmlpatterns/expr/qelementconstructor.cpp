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
#include "qdelegatingnamespaceresolver_p.h"
#include "qnamespaceconstructor_p.h"
#include "qnodebuilder_p.h"
#include "qoutputvalidator_p.h"
#include "qqnamevalue_p.h"
#include "qstaticnamespacecontext_p.h"

#include "qelementconstructor_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

ElementConstructor::ElementConstructor(const Expression::Ptr &op1,
                                       const Expression::Ptr &op2) : PairContainer(op1, op2)
{
}

Item ElementConstructor::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    const Item name(m_operand1->evaluateSingleton(context));

    const NodeBuilder::Ptr nodeBuilder(context->nodeBuilder(m_staticBaseURI));
    OutputValidator validator(nodeBuilder.data(), context, this);

    const DynamicContext::Ptr receiverContext(context->createReceiverContext(&validator));

    nodeBuilder->startElement(name.as<QNameValue>()->qName());
    m_operand2->evaluateToSequenceReceiver(receiverContext);
    nodeBuilder->endElement();

    const QAbstractXmlNodeModel::Ptr nm(nodeBuilder->builtDocument());
    context->addNodeModel(nm);

    return nm->root(QXmlNodeModelIndex());
}

void ElementConstructor::evaluateToSequenceReceiver(const DynamicContext::Ptr &context) const
{
    /* We create an OutputValidator here too. If we're serializing(a common case, unfortunately)
     * the receiver is already validating in order to catch cases where a computed attribute
     * constructor is followed by an element constructor, but in the cases where we're not serializing
     * it's necessary that we validate in this step. */
    const Item name(m_operand1->evaluateSingleton(context));
    QAbstractXmlReceiver *const receiver = context->outputReceiver();

    OutputValidator validator(receiver, context, this);
    const DynamicContext::Ptr receiverContext(context->createReceiverContext(&validator));

    receiver->startElement(name.as<QNameValue>()->qName());
    m_operand2->evaluateToSequenceReceiver(receiverContext);
    receiver->endElement();
}

Expression::Ptr ElementConstructor::typeCheck(const StaticContext::Ptr &context,
                                              const SequenceType::Ptr &reqType)
{
    m_staticBaseURI = context->baseURI();

    /* Namespace declarations changes the in-scope bindings, so let's
     * first lookup our child NamespaceConstructors. */
    const ID operandID = m_operand2->id();

    NamespaceResolver::Bindings overrides;
    if(operandID == IDExpressionSequence)
    {
        const Expression::List operands(m_operand2->operands());
        const int len = operands.count();

        for(int i = 0; i < len; ++i)
        {
            if(operands.at(i)->is(IDNamespaceConstructor))
            {
                const QXmlName &nb = operands.at(i)->as<NamespaceConstructor>()->namespaceBinding();
                overrides.insert(nb.prefix(), nb.namespaceURI());
            }
        }
    }

    const NamespaceResolver::Ptr newResolver(new DelegatingNamespaceResolver(context->namespaceBindings(), overrides));
    const StaticContext::Ptr augmented(new StaticNamespaceContext(newResolver, context));

    return PairContainer::typeCheck(augmented, reqType);
}

SequenceType::Ptr ElementConstructor::staticType() const
{
    return CommonSequenceTypes::ExactlyOneElement;
}

SequenceType::List ElementConstructor::expectedOperandTypes() const
{
    SequenceType::List result;
    result.append(CommonSequenceTypes::ExactlyOneQName);
    result.append(CommonSequenceTypes::ZeroOrMoreItems);
    return result;
}

Expression::Properties ElementConstructor::properties() const
{
    return DisableElimination | IsNodeConstructor;
}

ExpressionVisitorResult::Ptr
ElementConstructor::accept(const ExpressionVisitor::Ptr &visitor) const
{
    return visitor->visit(this);
}

QT_END_NAMESPACE
