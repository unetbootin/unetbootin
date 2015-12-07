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
