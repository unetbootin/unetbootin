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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef Patternist_TreatAs_H
#define Patternist_TreatAs_H

#include "qsinglecontainer_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short Implements XPath 2.0's <tt>treat as</tt> expression.
     *
     * TreatAs is always a compile-time class only, and is always deallocated by re-writing
     * to CardinalityVerifier or ItemVerifier or both, by calling
     * TypeChecker::applyFunctionConversion(). One approach could be to skip instantiating TreatAs and
     * simply let the return value of TypeChecker::applyFunctionConversion() be inserted into the AST, but
     * that wouldn't handle type checking the context item properly, which depends on that the StaticContext
     * have been set by the parent Expression.
     *
     * @see <a href="http://www.w3.org/TR/xpath20/#id-treat">XML Path Language
     * (XPath) 2.0, 3.10.5 Treat</a>
     * @author Frans Englich <fenglich@trolltech.com>
     * @ingroup Patternist_expressions
     */
    class TreatAs : public SingleContainer
    {
    public:
        /**
         * Creats a TreatAs where it is checked that the expression @p operand conforms
         * to the type @p reqType.
         */
        TreatAs(const Expression::Ptr &operand,
                const SequenceType::Ptr &reqType);

        /**
         * This function rewrites always. First the type that this TreatAs expression tests for
         * is verified. Then, the type the the <tt>treat as</tt> expression itself must match, @p reqType,
         * is verified.
         */
        virtual Expression::Ptr typeCheck(const StaticContext::Ptr &context,
                                          const SequenceType::Ptr &reqType);

        /**
         * @returns always the SequenceType passed in the constructor to this class. That is, the
         * SequenceType that the operand must conform to.
         */
        virtual SequenceType::Ptr staticType() const;

        /**
         * @returns a list containing one CommonSequenceTypes::ZeroOrMoreItems
         */
        virtual SequenceType::List expectedOperandTypes() const;

        virtual ExpressionVisitorResult::Ptr accept(const ExpressionVisitor::Ptr &visitor) const;

    private:
        const SequenceType::Ptr m_reqType;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
