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

#include "qargumentconverter_p.h"
#include "qatomictype_p.h"
#include "qatomizer_p.h"
#include "qbuiltintypes_p.h"
#include "qcardinalityverifier_p.h"
#include "qcommonsequencetypes_p.h"
#include "qfunctionfactory_p.h"
#include "qitemverifier_p.h"
#include "qpatternistlocale_p.h"
#include "quntypedatomicconverter_p.h"

#include "qtypechecker_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

QString TypeChecker::wrongType(const NamePool::Ptr &np,
                               const ItemType::Ptr &reqType,
                               const ItemType::Ptr &opType)
{
    return QtXmlPatterns::tr("Required type is %1, but %2 was found.")
              .arg(formatType(np, reqType), formatType(np, opType));
}

Expression::Ptr TypeChecker::applyFunctionConversion(const Expression::Ptr &operand,
                                                     const SequenceType::Ptr &reqType,
                                                     const StaticContext::Ptr &context,
                                                     const ReportContext::ErrorCode code,
                                                     const Options options)
{
    Q_ASSERT_X(!ReportContext::codeToString(code).isEmpty(), Q_FUNC_INFO,
               "This test ensures 'code' exists, otherwise codeToString() would assert.");
    Q_ASSERT(operand);
    Q_ASSERT(reqType);
    Q_ASSERT(context);

    /* Do it in two steps: verify type, and then cardinality. */
    const Expression::Ptr cardVerified(CardinalityVerifier::verifyCardinality(operand, reqType->cardinality(), context, code));
    return verifyType(cardVerified, reqType, context, code, options);
}

bool TypeChecker::promotionPossible(const ItemType::Ptr &fromType,
                                    const ItemType::Ptr &toType,
                                    const StaticContext::Ptr &context)
{
    /* These types can be promoted to xs:string. xs:untypedAtomic should be
     * cast when interpreting it formally, but implementing it as a promotion
     * gives the same result(and is faster). */
    if(*toType == *BuiltinTypes::xsString &&
       (BuiltinTypes::xsUntypedAtomic->xdtTypeMatches(fromType) ||
       BuiltinTypes::xsAnyURI->xdtTypeMatches(fromType)))
        return true;

    if(*toType == *BuiltinTypes::xsDouble &&
       BuiltinTypes::numeric->xdtTypeMatches(fromType))
    {
        /* Any numeric can be promoted to xs:double. */
        return true;
    }

    /* xs:decimal/xs:integer can be promoted to xs:float. */
    if(*toType == *BuiltinTypes::xsFloat && BuiltinTypes::xsDecimal->xdtTypeMatches(fromType))

    {
        context->warning(QtXmlPatterns::tr("Promoting %1 to %2 may cause loss of precision.")
                                   .arg(formatType(context->namePool(), fromType))
                                   .arg(formatType(context->namePool(), BuiltinTypes::xsFloat)));
        return true;
    }

    return false;
}

Expression::Ptr TypeChecker::typeCheck(Expression *const op,
                                       const StaticContext::Ptr &context,
                                       const SequenceType::Ptr &reqType)
{
    return Expression::Ptr(op->typeCheck(context, reqType));
}

Expression::Ptr TypeChecker::verifyType(const Expression::Ptr &operand,
                                        const SequenceType::Ptr &reqSeqType,
                                        const StaticContext::Ptr &context,
                                        const ReportContext::ErrorCode code,
                                        const Options options)
{
    const ItemType::Ptr reqType(reqSeqType->itemType());
    const Expression::Properties props(operand->properties());

    /* If operand requires a focus, do the necessary type checking for that. */
    if(props.testFlag(Expression::RequiresFocus) && options.testFlag(CheckFocus))
    {
        const ItemType::Ptr contextType(context->contextItemType());
        if(contextType)
        {
            if(props.testFlag(Expression::RequiresContextItem))
            {
                Q_ASSERT_X(operand->expectedContextItemType(), Q_FUNC_INFO,
                           "When the Expression sets the RequiresContextItem property, it must "
                           "return a type in expectedContextItemType()");
                const ItemType::Ptr expectedContextType(operand->expectedContextItemType());

                /* Allow the empty sequence. We don't want to trigger XPTY0020 on ()/... . */
                if(!expectedContextType->xdtTypeMatches(contextType) && contextType != CommonSequenceTypes::Empty)
                {
                    context->error(wrongType(context->namePool(), operand->expectedContextItemType(), contextType),
                                            ReportContext::XPTY0020, operand.data());
                    return operand;
                }
            }
        }
        else
        {
            context->error(QtXmlPatterns::tr("The focus is undefined."), ReportContext::XPDY0002, operand.data());
            return operand;
        }
    }

    SequenceType::Ptr operandSeqType(operand->staticType());
    ItemType::Ptr operandType(operandSeqType->itemType());

    /* This returns the operand if the types are identical or if operandType
     * is a subtype of reqType. */
    if(reqType->xdtTypeMatches(operandType) || *operandType == *CommonSequenceTypes::Empty)
        return operand;

    /* Since we haven't exited yet, it means that the operandType is a super type
     * of reqType, and that there hence is a path down to it through the
     * type hierachy -- but that doesn't neccessarily mean that a up-cast(down the
     * hierarchy) would succeed. */

    Expression::Ptr result(operand);

    if(reqType->isAtomicType())
    {
        const Expression::ID opID = operand->id();
        if((opID == Expression::IDArgumentReference ||
            (opID == Expression::IDCardinalityVerifier && operand->operands().first()->is(Expression::IDArgumentReference)))
           && *BuiltinTypes::item == *operandType)
            return Expression::Ptr(new ArgumentConverter(result, reqType));

        if(!operandType->isAtomicType())
        {
            result = Expression::Ptr(new Atomizer(result));
            /* The atomizer might know more about the type. */
            operandType = result->staticType()->itemType();
        }

        if(reqType->xdtTypeMatches(operandType))
        {
            /* Atomization was sufficient. Either the expected type is xs:anyAtomicType
             * or the type the Atomizer knows it returns, matches the required type. */
            return result;
        }

        const bool compatModeEnabled = context->compatModeEnabled();

        if((options.testFlag(AutomaticallyConvert) && BuiltinTypes::xsUntypedAtomic->xdtTypeMatches(operandType)) ||
           (compatModeEnabled && BuiltinTypes::xsString->xdtTypeMatches(reqType)))
        {
            if(*reqType == *BuiltinTypes::numeric)
            {
                result = typeCheck(new UntypedAtomicConverter(result, BuiltinTypes::xsDouble, code),
                                   context, reqSeqType);
            }
            else
                result = typeCheck(new UntypedAtomicConverter(result, reqType, code), context, reqSeqType);

            /* The UntypedAtomicConverter might know more about the type, so reload. */
            operandType = result->staticType()->itemType();
        }
        else if(compatModeEnabled && *reqType == *BuiltinTypes::xsDouble)
        {
            const FunctionFactory::Ptr functions(context->functionSignatures());
            Expression::List numberArgs;
            numberArgs.append(operand);

            result = functions->createFunctionCall(QXmlName(StandardNamespaces::fn, StandardLocalNames::number),
                                                   numberArgs,
                                                   context,
                                                   operand.data())->typeCheck(context, reqSeqType);
            operandType = result->staticType()->itemType();
            context->addLocation(result.data(), context->locationFor(operand.data()));
        }

        if(reqType->xdtTypeMatches(operandType))
            return result;

        /* Test if promotion will solve it; the xdtTypeMatches didn't
         * do that. */
        if(options.testFlag(AutomaticallyConvert) && promotionPossible(operandType, reqType, context))
        {
            if(options.testFlag(GeneratePromotion))
                return Expression::Ptr(new UntypedAtomicConverter(result, reqType));
            else
                return result;
        }

        if(operandType->xdtTypeMatches(reqType))
        {
            /* For example, operandType is numeric, and reqType is xs:integer. */
            return Expression::Ptr(new ItemVerifier(result, reqType, code));
        }
        else
        {
            context->error(wrongType(context->namePool(), reqType, operandType), code, operand.data());
            return result;
        }
    }
    else if(reqType->isNodeType())
    {

        ReportContext::ErrorCode myCode;

        if(*reqType == *CommonSequenceTypes::EBV->itemType())
            myCode = ReportContext::FORG0006;
        else
            myCode = code;

        /* empty-sequence() is considered valid because it's ok to do
         * for example nilled( () ). That is, to pass an empty sequence to a
         * function requiring for example node()?. */
        if(*operandType == *CommonSequenceTypes::Empty)
            return result;
        else if(!operandType->xdtTypeMatches(reqType))
        {
            context->error(wrongType(context->namePool(), reqType, operandType), myCode, operand.data());
            return result;
        }

        /* Operand must be an item. Thus, the sequence can contain both
         * nodes and atomic values: we have to verify. */
        return Expression::Ptr(new ItemVerifier(result, reqType, myCode));
    }
    else
    {
        Q_ASSERT(*reqType == *CommonSequenceTypes::Empty);

        /* element() doesn't match empty-sequence(), but element()* does. */
        if(!reqType->xdtTypeMatches(operandType) &&
           !operandSeqType->cardinality().allowsEmpty())
        {
            context->error(wrongType(context->namePool(), reqType, operandType),
                                     code, operand.data());
            return result;
        }
    }

    /* This line should be reached if required type is
     * EBVType, and the operand is compatible. */
    return result;
}

QT_END_NAMESPACE
