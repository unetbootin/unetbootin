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
