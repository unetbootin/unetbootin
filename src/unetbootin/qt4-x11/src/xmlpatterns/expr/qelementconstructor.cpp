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
                                       const Expression::Ptr &op2,
                                       const bool isXSLT) : PairContainer(op1, op2)
                                                          , m_isXSLT(isXSLT)
{
}

Item ElementConstructor::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    const Item name(m_operand1->evaluateSingleton(context));

    const NodeBuilder::Ptr nodeBuilder(context->nodeBuilder(m_staticBaseURI));
    OutputValidator validator(nodeBuilder.data(), context, this, m_isXSLT);

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
    /* We create an OutputValidator here too. If we're serializing(a common
     * case, unfortunately) the receiver is already validating in order to
     * catch cases where a computed attribute constructor is followed by an
     * element constructor, but in the cases where we're not serializing it's
     * necessary that we validate in this step. */
    const Item name(m_operand1->evaluateSingleton(context));
    QAbstractXmlReceiver *const receiver = context->outputReceiver();

    OutputValidator validator(receiver, context, this, m_isXSLT);
    const DynamicContext::Ptr receiverContext(context->createReceiverContext(&validator));

    receiver->startElement(name.as<QNameValue>()->qName());
    m_operand2->evaluateToSequenceReceiver(receiverContext);
    receiver->endElement();
}

Expression::Ptr ElementConstructor::typeCheck(const StaticContext::Ptr &context,
                                              const SequenceType::Ptr &reqType)
{
    /* What does this code do? When type checking our children, our namespace
     * bindings, which are also children of the form of NamespaceConstructor
     * instances, must be statically in-scope for them, so find them and
     * shuffle their bindings into the StaticContext. */

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
