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
#include "qtypechecker_p.h"

#include "qtreatas_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

TreatAs::TreatAs(const Expression::Ptr &operand,
                 const SequenceType::Ptr &reqType) : SingleContainer(operand),
                                                     m_reqType(reqType)
{
    Q_ASSERT(reqType);
}

Expression::Ptr TreatAs::typeCheck(const StaticContext::Ptr &context,
                                   const SequenceType::Ptr &reqType)
{
    Q_ASSERT(context);
    Q_ASSERT(reqType);

    /* Apply function conversion with the special error code XPDY0050. After that, we
     * let the regular typeCheck() function be invoked on the operand before we rewrite
     * to it. Hence is applyFunctionConversion() called twice, which doesn't break anything,
     * but indeed is redundant. */
    const Expression::Ptr treated(TypeChecker::applyFunctionConversion(m_operand,
                                                                       m_reqType,
                                                                       context,
                                                                       ReportContext::XPDY0050));
    return treated->typeCheck(context, reqType);
}

ExpressionVisitorResult::Ptr TreatAs::accept(const ExpressionVisitor::Ptr &visitor) const
{
    return visitor->visit(this);
}

SequenceType::Ptr TreatAs::staticType() const
{
    return m_reqType;
}

SequenceType::List TreatAs::expectedOperandTypes() const
{
    SequenceType::List result;
    result.append(CommonSequenceTypes::ZeroOrMoreItems);
    return result;
}

QT_END_NAMESPACE
