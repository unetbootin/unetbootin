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

#include <QUrl>

#include "qcommonsequencetypes_p.h"
#include "qnodebuilder_p.h"
#include "qqnamevalue_p.h"

#include "qattributeconstructor_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

AttributeConstructor::AttributeConstructor(const Expression::Ptr &op1,
                                           const Expression::Ptr &op2) : PairContainer(op1, op2)
{
}

QString AttributeConstructor::processValue(const QXmlName name,
                                           const Item &value)
{
    if(!value)
        return QString();
    else if(name == QXmlName(StandardNamespaces::xml, StandardLocalNames::id))
        return value.stringValue().simplified();
    else
        return value.stringValue();
}

Item AttributeConstructor::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    const Item nameItem(m_operand1->evaluateSingleton(context));
    const Item content(m_operand2->evaluateSingleton(context));

    const QXmlName name(nameItem.as<QNameValue>()->qName());
    const QString value(processValue(name, content));
    const NodeBuilder::Ptr nodeBuilder(context->nodeBuilder(QUrl()));

    nodeBuilder->attribute(name, QStringRef(&value));

    const QAbstractXmlNodeModel::Ptr nm(nodeBuilder->builtDocument());
    context->addNodeModel(nm);
    return nm->root(QXmlNodeModelIndex());
}

void AttributeConstructor::evaluateToSequenceReceiver(const DynamicContext::Ptr &context) const
{
    QAbstractXmlReceiver *const receiver = context->outputReceiver();
    const Item nameItem(m_operand1->evaluateSingleton(context));

    const Item content(m_operand2->evaluateSingleton(context));
    const QXmlName name(nameItem.as<QNameValue>()->qName());
    const QString value(processValue(name, content));

    receiver->attribute(name, QStringRef(&value));
}

SequenceType::Ptr AttributeConstructor::staticType() const
{
    return CommonSequenceTypes::ExactlyOneAttribute;
}

SequenceType::List AttributeConstructor::expectedOperandTypes() const
{
    SequenceType::List result;
    result.append(CommonSequenceTypes::ExactlyOneQName);
    result.append(CommonSequenceTypes::ZeroOrMoreItems);
    return result;
}

Expression::Properties AttributeConstructor::properties() const
{
    return DisableElimination | IsNodeConstructor;
}

ExpressionVisitorResult::Ptr
AttributeConstructor::accept(const ExpressionVisitor::Ptr &visitor) const
{
    return visitor->visit(this);
}

Expression::ID AttributeConstructor::id() const
{
    return IDAttributeConstructor;
}

QT_END_NAMESPACE
