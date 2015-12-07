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

#include "qtemplateparameterreference_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

TemplateParameterReference::TemplateParameterReference(const VariableDeclaration::Ptr &varDecl) : m_varDecl(varDecl)
{
}

bool TemplateParameterReference::evaluateEBV(const DynamicContext::Ptr &context) const
{
    return context->templateParameterStore()[m_varDecl->name]->evaluateEBV(context);
}

Item TemplateParameterReference::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    return context->templateParameterStore()[m_varDecl->name]->evaluateSingleton(context);
}

Item::Iterator::Ptr TemplateParameterReference::evaluateSequence(const DynamicContext::Ptr &context) const
{
    Q_ASSERT(!m_varDecl->name.isNull());
    Q_ASSERT(context->templateParameterStore()[m_varDecl->name]);
    return context->templateParameterStore()[m_varDecl->name]->evaluateSequence(context);
}

ExpressionVisitorResult::Ptr TemplateParameterReference::accept(const ExpressionVisitor::Ptr &visitor) const
{
    return visitor->visit(this);
}

Expression::Properties TemplateParameterReference::properties() const
{
    return DisableElimination;
}

SequenceType::Ptr TemplateParameterReference::staticType() const
{
    /* We can't use m_varDecl->expression()'s static type here, because
     * it's the default argument. */
    if(!m_varDecl->sequenceType)
        return CommonSequenceTypes::ZeroOrMoreItems;
    else
        return m_varDecl->sequenceType;
}

QT_END_NAMESPACE
