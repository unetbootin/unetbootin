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

#include "qcalltemplate_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

CallTemplate::CallTemplate(const QXmlName &name,
                           const WithParam::Hash &withParams) : TemplateInvoker(withParams, name)
{
}

Item::Iterator::Ptr CallTemplate::evaluateSequence(const DynamicContext::Ptr &context) const
{
    Q_ASSERT(m_template);
    return m_template->body->evaluateSequence(m_template->createContext(this, context, true));
}

bool CallTemplate::evaluateEBV(const DynamicContext::Ptr &context) const
{
    Q_ASSERT(m_template);
    return m_template->body->evaluateEBV(m_template->createContext(this, context, true));
}

void CallTemplate::evaluateToSequenceReceiver(const DynamicContext::Ptr &context) const
{
    Q_ASSERT(m_template);
    m_template->body->evaluateToSequenceReceiver(m_template->createContext(this, context, true));
}

Expression::Ptr CallTemplate::typeCheck(const StaticContext::Ptr &context,
                                        const SequenceType::Ptr &reqType)
{
    /* Check XTSE0680, that every @c xsl:with-param has a corresponding @c
     * xsl:param declaration. */
    {
        const WithParam::Hash::const_iterator end(m_withParams.constEnd());

        for(WithParam::Hash::const_iterator it(m_withParams.constBegin());
            it != end;
            ++it)
        {
            if(!VariableDeclaration::contains(m_template->templateParameters, it.value()->name()))
                Template::raiseXTSE0680(context, it.value()->name(), this);
        }
    }

    const Expression::Ptr me(TemplateInvoker::typeCheck(context, reqType));

    const VariableDeclaration::List args(m_template->templateParameters);
    const VariableDeclaration::List::const_iterator end(args.constEnd());
    VariableDeclaration::List::const_iterator it(args.constBegin());

    for(; it != end; ++it)
    {
        // TODO
        Q_ASSERT((*it)->sequenceType);
    }

    return me;
}

Expression::Properties CallTemplate::properties() const
{
    Q_ASSERT(!m_template || m_template->body);

    /* We may be called before our m_template is resolved, namely when we're
     * the body of a variable. In that case querytransformparser.ypp will
     * manually call TypeChecker::applyFunctionConversion(), which is before
     * ExpressionFactory::createExpression() has resolved us. */
    if(m_template && !isRecursive())
        return m_template->properties();
    else
        return Properties();
}

Expression::Properties CallTemplate::dependencies() const
{
    if(m_template && !isRecursive())
        return m_template->dependencies();
    else
        return Properties();
}

SequenceType::Ptr CallTemplate::staticType() const
{
    return CommonSequenceTypes::ZeroOrMoreItems;
}

ExpressionVisitorResult::Ptr CallTemplate::accept(const ExpressionVisitor::Ptr &visitor) const
{
    return visitor->visit(this);
}

bool CallTemplate::configureRecursion(const CallTargetDescription::Ptr &sign)
{
    Q_UNUSED(sign);
    return false;
}

Expression::Ptr CallTemplate::body() const
{
    return m_template->body;
}

CallTargetDescription::Ptr CallTemplate::callTargetDescription() const
{
    return CallTargetDescription::Ptr();
}

QT_END_NAMESPACE
