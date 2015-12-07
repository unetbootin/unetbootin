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
#include "qpatternistlocale_p.h"
#include "qnodebuilder_p.h"
#include "qqnamevalue_p.h"

#include "qprocessinginstructionconstructor_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

ProcessingInstructionConstructor::
ProcessingInstructionConstructor(const Expression::Ptr &op1,
                                 const Expression::Ptr &op2) : PairContainer(op1, op2)
{
}

QString ProcessingInstructionConstructor::leftTrimmed(const QString &input)
{
    const int len = input.length();

    for(int i = 0; i < len; ++i)
    {
        if(!input.at(i).isSpace())
            return input.mid(i);
    }

    return QString(); /* input consists only of whitespace. All was trimmed. */
}

QString ProcessingInstructionConstructor::data(const DynamicContext::Ptr &context) const
{
    const Item name(m_operand1->evaluateSingleton(context));
    const Item dataArg(m_operand2->evaluateSingleton(context));

    if(dataArg)
    {
        /* Perform trimming before validation, to increase speed. */
        const QString value(leftTrimmed(dataArg.stringValue()));

        if(value.contains(QLatin1String("?>")))
        {
            context->error(QtXmlPatterns::tr("The data of a processing instruction cannot contain the string %1").arg(formatData("?>")),
                              ReportContext::XQDY0026, this);
            return QString();
        }
        else
            return value;
    }
    else
        return QString();
}

QXmlName ProcessingInstructionConstructor::evaluateTardata(const DynamicContext::Ptr &context) const
{
    const Item name(m_operand1->evaluateSingleton(context));
    return context->namePool()->allocateQName(QString(), name.stringValue());
}

Item ProcessingInstructionConstructor::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    const NodeBuilder::Ptr nodeBuilder(context->nodeBuilder(QUrl()));

    nodeBuilder->processingInstruction(evaluateTardata(context), data(context));

    const QAbstractXmlNodeModel::Ptr nm(nodeBuilder->builtDocument());
    context->addNodeModel(nm);

    return nm->root(QXmlNodeModelIndex());
}

void ProcessingInstructionConstructor::evaluateToSequenceReceiver(const DynamicContext::Ptr &context) const
{
    QAbstractXmlReceiver *const receiver = context->outputReceiver();

    receiver->processingInstruction(evaluateTardata(context), data(context));
}

SequenceType::Ptr ProcessingInstructionConstructor::staticType() const
{
    return CommonSequenceTypes::ExactlyOneProcessingInstruction;
}

SequenceType::List ProcessingInstructionConstructor::expectedOperandTypes() const
{
    SequenceType::List result;
    result.append(CommonSequenceTypes::ExactlyOneString);
    result.append(CommonSequenceTypes::ZeroOrOneString);
    return result;
}

Expression::Properties ProcessingInstructionConstructor::properties() const
{
    return DisableElimination | IsNodeConstructor;
}

ExpressionVisitorResult::Ptr
ProcessingInstructionConstructor::accept(const ExpressionVisitor::Ptr &visitor) const
{
    return visitor->visit(this);
}

QT_END_NAMESPACE
