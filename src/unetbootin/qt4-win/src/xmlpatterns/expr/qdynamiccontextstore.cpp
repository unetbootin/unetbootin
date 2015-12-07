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

#include "qboolean_p.h"
#include "qcommonsequencetypes_p.h"
#include "qcommonvalues_p.h"

#include "qdynamiccontextstore_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

DynamicContextStore::DynamicContextStore(const Expression::Ptr &operand,
                                         const DynamicContext::Ptr &context) : SingleContainer(operand),
                                                                               m_context(context)
{
    Q_ASSERT(context);
}

bool DynamicContextStore::evaluateEBV(const DynamicContext::Ptr &) const
{
    return m_operand->evaluateEBV(m_context);
}

Item::Iterator::Ptr DynamicContextStore::evaluateSequence(const DynamicContext::Ptr &) const
{
    return m_operand->evaluateSequence(m_context);
}

Item DynamicContextStore::evaluateSingleton(const DynamicContext::Ptr &) const
{
    return m_operand->evaluateSingleton(m_context);
}

SequenceType::Ptr DynamicContextStore::staticType() const
{
    return m_operand->staticType();
}

SequenceType::List DynamicContextStore::expectedOperandTypes() const
{
    SequenceType::List result;
    result.append(CommonSequenceTypes::ZeroOrMoreItems);
    return result;
}

ExpressionVisitorResult::Ptr DynamicContextStore::accept(const ExpressionVisitor::Ptr &visitor) const
{
    return visitor->visit(this);
}

const SourceLocationReflection *DynamicContextStore::actualReflection() const
{
    return m_operand->actualReflection();
}

QT_END_NAMESPACE
