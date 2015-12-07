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
#include "qqnamevalue_p.h"

#include "qqnameconstructor_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

QNameConstructor::QNameConstructor(const Expression::Ptr &source,
                                   const NamespaceResolver::Ptr &nsResolver) : SingleContainer(source),
                                                                               m_nsResolver(nsResolver)
{
    Q_ASSERT(m_nsResolver);
}

Item QNameConstructor::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    Q_ASSERT(context);
    const QString lexQName(m_operand->evaluateSingleton(context).stringValue());

    const QXmlName expQName(expandQName<DynamicContext::Ptr,
                                        ReportContext::XQDY0074,
                                        ReportContext::XQDY0074>(lexQName,
                                                                 context,
                                                                 m_nsResolver,
                                                                 this));
    return toItem(QNameValue::fromValue(context->namePool(), expQName));
}

QXmlName::NamespaceCode QNameConstructor::namespaceForPrefix(const QXmlName::PrefixCode prefix,
                                                          const StaticContext::Ptr &context,
                                                          const SourceLocationReflection *const r)
{
    Q_ASSERT(context);
    const QXmlName::NamespaceCode ns(context->namespaceBindings()->lookupNamespaceURI(prefix));

    if(ns == NamespaceResolver::NoBinding)
    {
        context->error(QtXmlPatterns::tr("No namespace binding exists for the prefix %1")
                          .arg(formatKeyword(context->namePool()->stringForPrefix(prefix))),
                       ReportContext::XPST0081,
                       r);
        return NamespaceResolver::NoBinding;
    }
    else
        return ns;
}

SequenceType::Ptr QNameConstructor::staticType() const
{
    return CommonSequenceTypes::ExactlyOneQName;
}

SequenceType::List QNameConstructor::expectedOperandTypes() const
{
    SequenceType::List result;
    result.append(CommonSequenceTypes::ExactlyOneString);
    return result;
}

ExpressionVisitorResult::Ptr QNameConstructor::accept(const ExpressionVisitor::Ptr &visitor) const
{
    return visitor->visit(this);
}

const SourceLocationReflection *QNameConstructor::actualReflection() const
{
    return m_operand.data();
}

QT_END_NAMESPACE
