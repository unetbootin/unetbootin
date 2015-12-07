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

#ifndef Patternist_CardinalityVerifier_H
#define Patternist_CardinalityVerifier_H

#include "qsinglecontainer_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{

    /**
     * @short Verifies that the sequence an Expression evaluates to conforms to a Cardinality.
     *
     * @see <a href="http://www.w3.org/TR/xpath-functions/#cardinality-funcs">XQuery 1.0 and
     * XPath 2.0 Functions and Operators, 15.2 Functions That Test the Cardinality of Sequences</a>
     * @author Frans Englich <fenglich@trolltech.com>
     * @ingroup Patternist_expressions
     */
    class CardinalityVerifier : public SingleContainer
    {
    public:
        CardinalityVerifier(const Expression::Ptr &operand,
                            const Cardinality &card,
                            const ReportContext::ErrorCode code);

        virtual Item::Iterator::Ptr evaluateSequence(const DynamicContext::Ptr &context) const;
        virtual Item evaluateSingleton(const DynamicContext::Ptr &) const;

        virtual SequenceType::List expectedOperandTypes() const;
        virtual SequenceType::Ptr staticType() const;

        /**
         * If the static cardinality of the operand is within the required cardinality,
         * the operand is returned as is, since results will always be valid and hence
         * is not a CardinalityVerifier necessary.
         *
         * This is an optimization, and hence could be done at the compression() stage, but
         * the earlier the better.
         */
        virtual Expression::Ptr typeCheck(const StaticContext::Ptr &context,
                                          const SequenceType::Ptr &reqType);
        virtual ExpressionVisitorResult::Ptr accept(const ExpressionVisitor::Ptr &visitor) const;

        /**
         * A utility function for determining whether the static type of an Expression matches
         * a cardinality. More specifically, this function performs the cardinality verification
         * part of the Function Conversion Rules.
         *
         * @todo Mention the rewrite and when exactly an error is issued via @p context
         */
        static Expression::Ptr
        verifyCardinality(const Expression::Ptr &operand,
                          const Cardinality &card,
                          const ReportContext::Ptr &context,
                          const ReportContext::ErrorCode code = ReportContext::XPTY0004);

        virtual const SourceLocationReflection *actualReflection() const;

    private:
        /**
         * Centralizes a message string in order to increase consistency and
         * reduce work for translators.
         */
        static inline QString wrongCardinality(const Cardinality &req,
                                               const Cardinality &got = Cardinality::empty());

        const Cardinality m_reqCard;
        const bool m_allowsMany;
        const ReportContext::ErrorCode m_errorCode;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
