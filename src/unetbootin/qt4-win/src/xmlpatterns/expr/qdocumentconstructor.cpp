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
#include "qdocumentcontentvalidator_p.h"
#include "qnodebuilder_p.h"

#include "qdocumentconstructor_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

DocumentConstructor::DocumentConstructor(const Expression::Ptr &op) : SingleContainer(op)
{
}

Item DocumentConstructor::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    NodeBuilder::Ptr nodeBuilder(context->nodeBuilder(m_staticBaseURI));

    DocumentContentValidator validator(nodeBuilder.data(), context, ConstPtr(this));
    const DynamicContext::Ptr receiverContext(context->createReceiverContext(&validator));

    validator.startDocument();
    m_operand->evaluateToSequenceReceiver(receiverContext);
    validator.endDocument();

    const QAbstractXmlNodeModel::Ptr nm(nodeBuilder->builtDocument());
    context->addNodeModel(nm);

    return nm->root(QXmlNodeModelIndex());
}

void DocumentConstructor::evaluateToSequenceReceiver(const DynamicContext::Ptr &context) const
{
    QAbstractXmlReceiver *const receiver = context->outputReceiver();

    DocumentContentValidator validator(receiver, context, ConstPtr(this));

    const DynamicContext::Ptr receiverContext(context->createReceiverContext(&validator));

    validator.startDocument();
    m_operand->evaluateToSequenceReceiver(receiverContext);
    validator.endDocument();
}

Expression::Ptr DocumentConstructor::typeCheck(const StaticContext::Ptr &context,
                                               const SequenceType::Ptr &reqType)
{
    m_staticBaseURI = context->baseURI();
    return SingleContainer::typeCheck(context, reqType);
}

SequenceType::Ptr DocumentConstructor::staticType() const
{
    return CommonSequenceTypes::ExactlyOneDocumentNode;
}

SequenceType::List DocumentConstructor::expectedOperandTypes() const
{
    SequenceType::List result;
    result.append(CommonSequenceTypes::ZeroOrMoreItems);
    return result;
}

Expression::Properties DocumentConstructor::properties() const
{
    return DisableElimination | IsNodeConstructor;
}

ExpressionVisitorResult::Ptr
DocumentConstructor::accept(const ExpressionVisitor::Ptr &visitor) const
{
    return visitor->visit(this);
}

QT_END_NAMESPACE
