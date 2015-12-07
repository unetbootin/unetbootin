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

#ifndef Patternist_ExpressionDispatch_H
#define Patternist_ExpressionDispatch_H

#include <QSharedData>


QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    class AndExpression;
    class ArgumentConverter;
    class ArgumentReference;
    class ArithmeticExpression;
    class Atomizer;
    class AttributeConstructor;
    class AttributeNameValidator;
    class CardinalityVerifier;
    class CastAs;
    class CastableAs;
    class CollationChecker;
    class CombineNodes;
    class CommentConstructor;
    class CopyOf;
    class ContextItem;
    class DocumentConstructor;
    class DynamicContextStore;
    class ElementConstructor;
    class EmptySequence;
    class ExpressionSequence;
    template<bool IsForGlobal> class EvaluationCache;
    class ExpressionVariableReference;
    class ExternalVariableReference;
    class FirstItemPredicate;
    class ForClause;
    class FunctionCall;
    class GeneralComparison;
    class GenericPredicate;
    class IfThenClause;
    class InstanceOf;
    class ItemVerifier;
    class LetClause;
    class Literal;
    class LiteralSequence;
    class NamespaceConstructor;
    class NCNameConstructor;
    class NodeComparison;
    class NodeSortExpression;
    class OrderBy;
    class OrExpression;
    class ParentNodeAxis;
    class Path;
    class PositionalVariableReference;
    class ProcessingInstructionConstructor;
    class QNameConstructor;
    class QuantifiedExpression;
    class RangeExpression;
    class ReturnOrderBy;
    class RangeVariableReference;
    class SimpleContentConstructor;
    class AxisStep;
    class TextNodeConstructor;
    class TreatAs;
    class TruthPredicate;
    class UntypedAtomicConverter;
    class UserFunctionCallsite;
    class ValidationError;
    class ValueComparison;

    /**
     * @todo Documentation's missing
     *
     * @defgroup Patternist_expr_dispatch Expression Dispatching
     */

    /**
     * @ingroup Patternist_expr_dispatch
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class ExpressionVisitorResult : public QSharedData
    {
    public:
        typedef QExplicitlySharedDataPointer<ExpressionVisitorResult> Ptr;
        ExpressionVisitorResult() {}
        virtual ~ExpressionVisitorResult() {}
    };

    /**
     * @ingroup Patternist_expr_dispatch
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class ExpressionVisitor : public QSharedData
    {
    public:
        typedef QExplicitlySharedDataPointer<ExpressionVisitor> Ptr;
        virtual ~ExpressionVisitor() {}

        virtual ExpressionVisitorResult::Ptr visit(const AndExpression *) const = 0;
        virtual ExpressionVisitorResult::Ptr visit(const ArithmeticExpression *) const = 0;
        virtual ExpressionVisitorResult::Ptr visit(const ArgumentConverter *) const = 0;
        virtual ExpressionVisitorResult::Ptr visit(const ArgumentReference *) const = 0;
        virtual ExpressionVisitorResult::Ptr visit(const Atomizer *) const = 0;
        virtual ExpressionVisitorResult::Ptr visit(const AttributeConstructor *) const = 0;
        virtual ExpressionVisitorResult::Ptr visit(const AttributeNameValidator *) const = 0;
        virtual ExpressionVisitorResult::Ptr visit(const CardinalityVerifier *) const = 0;
        virtual ExpressionVisitorResult::Ptr visit(const CastableAs *) const = 0;
        virtual ExpressionVisitorResult::Ptr visit(const CastAs *) const = 0;
        virtual ExpressionVisitorResult::Ptr visit(const CollationChecker *) const = 0;
        virtual ExpressionVisitorResult::Ptr visit(const CombineNodes *) const = 0;
        virtual ExpressionVisitorResult::Ptr visit(const CommentConstructor *) const = 0;
        virtual ExpressionVisitorResult::Ptr visit(const ContextItem *) const = 0;
        virtual ExpressionVisitorResult::Ptr visit(const CopyOf *) const = 0;
        virtual ExpressionVisitorResult::Ptr visit(const DocumentConstructor *) const = 0;
        virtual ExpressionVisitorResult::Ptr visit(const DynamicContextStore *) const = 0;
        virtual ExpressionVisitorResult::Ptr visit(const ElementConstructor *) const = 0;
        virtual ExpressionVisitorResult::Ptr visit(const EmptySequence *) const = 0;
        virtual ExpressionVisitorResult::Ptr visit(const EvaluationCache<false> *) const = 0;
        virtual ExpressionVisitorResult::Ptr visit(const EvaluationCache<true> *) const = 0;
        virtual ExpressionVisitorResult::Ptr visit(const ExpressionSequence *) const = 0;
        virtual ExpressionVisitorResult::Ptr visit(const ExpressionVariableReference *) const = 0;
        virtual ExpressionVisitorResult::Ptr visit(const ExternalVariableReference *) const = 0;
        virtual ExpressionVisitorResult::Ptr visit(const FirstItemPredicate *) const = 0;
        virtual ExpressionVisitorResult::Ptr visit(const ForClause *) const = 0;
        virtual ExpressionVisitorResult::Ptr visit(const FunctionCall *) const = 0;
        virtual ExpressionVisitorResult::Ptr visit(const GeneralComparison *) const = 0;
        virtual ExpressionVisitorResult::Ptr visit(const GenericPredicate *) const = 0;
        virtual ExpressionVisitorResult::Ptr visit(const IfThenClause *) const = 0;
        virtual ExpressionVisitorResult::Ptr visit(const AxisStep *) const = 0;
        virtual ExpressionVisitorResult::Ptr visit(const InstanceOf *) const = 0;
        virtual ExpressionVisitorResult::Ptr visit(const ItemVerifier *) const = 0;
        virtual ExpressionVisitorResult::Ptr visit(const LetClause *) const = 0;
        virtual ExpressionVisitorResult::Ptr visit(const Literal *) const = 0;
        virtual ExpressionVisitorResult::Ptr visit(const LiteralSequence *) const = 0;
        virtual ExpressionVisitorResult::Ptr visit(const NamespaceConstructor *) const = 0;
        virtual ExpressionVisitorResult::Ptr visit(const NCNameConstructor *) const = 0;
        virtual ExpressionVisitorResult::Ptr visit(const NodeComparison *) const = 0;
        virtual ExpressionVisitorResult::Ptr visit(const NodeSortExpression *) const = 0;
        virtual ExpressionVisitorResult::Ptr visit(const OrderBy *) const = 0;
        virtual ExpressionVisitorResult::Ptr visit(const OrExpression *) const = 0;
        virtual ExpressionVisitorResult::Ptr visit(const ParentNodeAxis *) const = 0;
        virtual ExpressionVisitorResult::Ptr visit(const Path *) const = 0;
        virtual ExpressionVisitorResult::Ptr visit(const PositionalVariableReference *) const = 0;
        virtual ExpressionVisitorResult::Ptr visit(const ProcessingInstructionConstructor *) const = 0;
        virtual ExpressionVisitorResult::Ptr visit(const QNameConstructor *) const = 0;
        virtual ExpressionVisitorResult::Ptr visit(const QuantifiedExpression *) const = 0;
        virtual ExpressionVisitorResult::Ptr visit(const RangeExpression *) const = 0;
        virtual ExpressionVisitorResult::Ptr visit(const RangeVariableReference *) const = 0;
        virtual ExpressionVisitorResult::Ptr visit(const ReturnOrderBy *) const = 0;
        virtual ExpressionVisitorResult::Ptr visit(const SimpleContentConstructor *) const = 0;
        virtual ExpressionVisitorResult::Ptr visit(const TextNodeConstructor *) const = 0;
        virtual ExpressionVisitorResult::Ptr visit(const TreatAs *) const = 0;
        virtual ExpressionVisitorResult::Ptr visit(const TruthPredicate *) const = 0;
        virtual ExpressionVisitorResult::Ptr visit(const UntypedAtomicConverter *) const = 0;
        virtual ExpressionVisitorResult::Ptr visit(const UserFunctionCallsite *) const = 0;
        virtual ExpressionVisitorResult::Ptr visit(const ValidationError *) const = 0;
        virtual ExpressionVisitorResult::Ptr visit(const ValueComparison *) const = 0;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
