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

#include "qbuiltintypes_p.h"
#include "qcommonnamespaces_p.h"
#include "qcommonsequencetypes_p.h"
#include "qcommonvalues_p.h"
#include "qpatternistlocale_p.h"
#include "qxmlname.h"

/* Functions */
#include "qaccessorfns_p.h"
#include "qaggregatefns_p.h"
#include "qbooleanfns_p.h"
#include "qcomparestringfns_p.h"
#include "qcontextfns_p.h"
#include "qnodefns_p.h"
#include "qnumericfns_p.h"
#include "qsequencefns_p.h"
#include "qsequencegeneratingfns_p.h"
#include "qstringvaluefns_p.h"
#include "qsubstringfns_p.h"

#include "qxpath10corefunctions_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

Expression::Ptr XPath10CoreFunctions::retrieveExpression(const QXmlName name,
                                                         const Expression::List &args,
                                                         const FunctionSignature::Ptr &sign) const
{
    Q_ASSERT(sign);

    Expression::Ptr fn;
#define testFN(ln, cname) else if(name.localName() == StandardLocalNames::ln) fn = Expression::Ptr(new cname())

    if(false) /* Dummy for the macro handling. Will be optimized away anyway. */
        return Expression::Ptr();
    /* Alphabetic order. */
    testFN(boolean,           BooleanFN);
    testFN(ceiling,           CeilingFN);
    testFN(concat,            ConcatFN);
    testFN(contains,          ContainsFN);
    testFN(count,             CountFN);
    testFN(False,             FalseFN);
    testFN(floor,             FloorFN);
    testFN(id,                IdFN);
    testFN(lang,              LangFN);
    testFN(last,              LastFN);
    testFN(local_name,        LocalNameFN);
    testFN(name,              NameFN);
    testFN(namespace_uri,     NamespaceURIFN);
    testFN(normalize_space,   NormalizeSpaceFN);
    testFN(Not,               NotFN);
    testFN(number,            NumberFN);
    testFN(position,          PositionFN);
    testFN(round,             RoundFN);
    testFN(starts_with,       StartsWithFN);
    testFN(string,            StringFN);
    testFN(string_length,     StringLengthFN);
    testFN(substring,         SubstringFN);
    testFN(substring_after,   SubstringAfterFN);
    testFN(substring_before,  SubstringBeforeFN);
    testFN(sum,               SumFN);
    testFN(translate,         TranslateFN);
    testFN(True,              TrueFN);
#undef testFN

    Q_ASSERT(fn);
    fn->setOperands(args);
    fn->as<FunctionCall>()->setSignature(sign);

    return fn;
}

FunctionSignature::Ptr XPath10CoreFunctions::retrieveFunctionSignature(const NamePool::Ptr &np, const QXmlName name)
{
    if(StandardNamespaces::fn != name.namespaceURI())
        return FunctionSignature::Ptr();

    FunctionSignature::Ptr s(functionSignatures().value(name));

    if(!s)
    {
        const QXmlName::LocalNameCode localName(name.localName());

        /* Alphabetic order. */
        if(StandardLocalNames::boolean == localName)
        {
            s = addFunction(StandardLocalNames::boolean, 1, 1, CommonSequenceTypes::ExactlyOneBoolean);
            s->appendArgument(argument(np, "arg"), CommonSequenceTypes::EBV);
        }
        else if(StandardLocalNames::ceiling == localName)
        {
            s = addFunction(StandardLocalNames::ceiling, 1, 1, CommonSequenceTypes::ZeroOrOneNumeric,
                            Expression::EmptynessFollowsChild | Expression::RewriteToEmptyOnEmpty);
            s->appendArgument(argument(np, "arg"), CommonSequenceTypes::ZeroOrOneNumeric);
        }
        else if(StandardLocalNames::concat == localName)
        {
            s = addFunction(StandardLocalNames::concat, 2, FunctionSignature::UnlimitedArity,
                            CommonSequenceTypes::ExactlyOneString);
            s->appendArgument(argument(np, "arg1"), CommonSequenceTypes::ZeroOrOneAtomicType);
            s->appendArgument(argument(np, "arg2"), CommonSequenceTypes::ZeroOrOneAtomicType);
        }
        else if(StandardLocalNames::contains == localName)
        {
            s = addFunction(StandardLocalNames::contains, 2, 3, CommonSequenceTypes::ExactlyOneBoolean,
                            Expression::LastOperandIsCollation);
            s->appendArgument(argument(np, "arg1"), CommonSequenceTypes::ZeroOrOneString);
            s->appendArgument(argument(np, "arg2"), CommonSequenceTypes::ZeroOrOneString);
            s->appendArgument(argument(np, "collation"), CommonSequenceTypes::ExactlyOneString);
        }
        else if(StandardLocalNames::count == localName)
        {
            s = addFunction(StandardLocalNames::count, 1, 1, CommonSequenceTypes::ExactlyOneInteger, Expression::IDCountFN);
            s->appendArgument(argument(np, "arg"), CommonSequenceTypes::ZeroOrMoreItems);
        }
        else if(StandardLocalNames::False == localName)
        {
            s = addFunction(StandardLocalNames::False, 0, 0, CommonSequenceTypes::ExactlyOneBoolean);
        }
        else if(StandardLocalNames::floor == localName)
        {
            s = addFunction(StandardLocalNames::floor, 1, 1, CommonSequenceTypes::ZeroOrOneNumeric,
                            Expression::EmptynessFollowsChild | Expression::RewriteToEmptyOnEmpty);
            s->appendArgument(argument(np, "arg"), CommonSequenceTypes::ZeroOrOneNumeric);
        }
        else if(StandardLocalNames::id == localName)
        {
            s = addFunction(StandardLocalNames::id, 1, 2, CommonSequenceTypes::ZeroOrMoreElements,
                            Expression::UseContextItem);
            s->appendArgument(argument(np, "idrefs"), CommonSequenceTypes::ZeroOrMoreStrings);
            s->appendArgument(argument(np, "node"), CommonSequenceTypes::ExactlyOneNode);
        }
        else if(StandardLocalNames::lang == localName)
        {
            s = addFunction(StandardLocalNames::lang, 1, 2, CommonSequenceTypes::ExactlyOneBoolean,
                            Expression::UseContextItem);
            s->appendArgument(argument(np, "testLang"), CommonSequenceTypes::ZeroOrOneString);
            s->appendArgument(argument(np, "node"), CommonSequenceTypes::ExactlyOneNode);
        }
        else if(StandardLocalNames::last == localName)
        {
            s = addFunction(StandardLocalNames::last, 0, 0, CommonSequenceTypes::ExactlyOneInteger,
                            Expression::DisableElimination | Expression::RequiresFocus);
        }
        else if(StandardLocalNames::local_name == localName)
        {
            s = addFunction(StandardLocalNames::local_name, 0, 1, CommonSequenceTypes::ExactlyOneString,
                            Expression::UseContextItem);
            s->appendArgument(argument(np, "arg"), CommonSequenceTypes::ZeroOrOneNode);
        }
        else if(StandardLocalNames::name == localName)
        {
            s = addFunction(StandardLocalNames::name, 0, 1, CommonSequenceTypes::ExactlyOneString,
                            Expression::UseContextItem);
            s->appendArgument(argument(np, "arg"), CommonSequenceTypes::ZeroOrOneNode);
        }
        else if(StandardLocalNames::namespace_uri == localName)
        {
            s = addFunction(StandardLocalNames::namespace_uri, 0, 1, CommonSequenceTypes::ExactlyOneAnyURI,
                            Expression::UseContextItem);
            s->appendArgument(argument(np, "arg"), CommonSequenceTypes::ZeroOrOneNode);
        }
        else if(StandardLocalNames::normalize_space == localName)
        {
            s = addFunction(StandardLocalNames::normalize_space, 0, 1, CommonSequenceTypes::ExactlyOneString,
                            Expression::UseContextItem);
            s->appendArgument(argument(np, "arg"), CommonSequenceTypes::ZeroOrOneString);
        }
        else if(StandardLocalNames::Not == localName)
        {
            s = addFunction(StandardLocalNames::Not, 1, 1, CommonSequenceTypes::ExactlyOneBoolean);
            s->appendArgument(argument(np, "arg"), CommonSequenceTypes::EBV);
        }
        else if(StandardLocalNames::number == localName)
        {
            s = addFunction(StandardLocalNames::number, 0, 1, CommonSequenceTypes::ExactlyOneDouble,
                            Expression::UseContextItem);
            s->appendArgument(argument(np, "arg"), CommonSequenceTypes::ZeroOrOneAtomicType);
        }
        else if(StandardLocalNames::position == localName)
        {
            s = addFunction(StandardLocalNames::position, 0, 0, CommonSequenceTypes::ExactlyOneInteger,
                            Expression::DisableElimination | Expression::RequiresFocus);
        }
        else if(StandardLocalNames::round == localName)
        {
            s = addFunction(StandardLocalNames::round, 1, 1, CommonSequenceTypes::ZeroOrOneNumeric,
                            Expression::EmptynessFollowsChild | Expression::RewriteToEmptyOnEmpty);
            s->appendArgument(argument(np, "arg"), CommonSequenceTypes::ZeroOrOneNumeric);
        }
        else if(StandardLocalNames::starts_with == localName)
        {
            s = addFunction(StandardLocalNames::starts_with, 2, 3, CommonSequenceTypes::ExactlyOneBoolean,
                            Expression::LastOperandIsCollation);
            s->appendArgument(argument(np, "arg1"), CommonSequenceTypes::ZeroOrOneString);
            s->appendArgument(argument(np, "arg2"), CommonSequenceTypes::ZeroOrOneString);
            s->appendArgument(argument(np, "collation"), CommonSequenceTypes::ExactlyOneString);
        }
        else if(StandardLocalNames::string == localName)
        {
            s = addFunction(StandardLocalNames::string, 0, 1, CommonSequenceTypes::ExactlyOneString,
                            Expression::UseContextItem);
            s->appendArgument(argument(np, "arg"), CommonSequenceTypes::ZeroOrOneItem);
        }
        else if(StandardLocalNames::string_length == localName)
        {
            s = addFunction(StandardLocalNames::string_length, 0, 1,
                            CommonSequenceTypes::ExactlyOneInteger,
                            Expression::UseContextItem);
            s->appendArgument(argument(np, "arg"), CommonSequenceTypes::ZeroOrOneString);
        }
        else if(StandardLocalNames::substring == localName)
        {
            s = addFunction(StandardLocalNames::substring, 2, 3, CommonSequenceTypes::ExactlyOneString);
            s->appendArgument(argument(np, "sourceString"), CommonSequenceTypes::ZeroOrOneString);
            s->appendArgument(argument(np, "startingLoc"), CommonSequenceTypes::ExactlyOneDouble);
            s->appendArgument(argument(np, "length"), CommonSequenceTypes::ExactlyOneDouble);
        }
        else if(StandardLocalNames::substring_after == localName)
        {
            s = addFunction(StandardLocalNames::substring_after, 2, 3, CommonSequenceTypes::ExactlyOneString,
                            Expression::LastOperandIsCollation);
            s->appendArgument(argument(np, "arg1"), CommonSequenceTypes::ZeroOrOneString);
            s->appendArgument(argument(np, "arg2"), CommonSequenceTypes::ZeroOrOneString);
            s->appendArgument(argument(np, "collation"), CommonSequenceTypes::ExactlyOneString);
        }
        else if(StandardLocalNames::substring_before == localName)
        {
            s = addFunction(StandardLocalNames::substring_before, 2, 3, CommonSequenceTypes::ExactlyOneString,
                            Expression::LastOperandIsCollation);
            s->appendArgument(argument(np, "arg1"), CommonSequenceTypes::ZeroOrOneString);
            s->appendArgument(argument(np, "arg2"), CommonSequenceTypes::ZeroOrOneString);
            s->appendArgument(argument(np, "collation"), CommonSequenceTypes::ExactlyOneString);
        }
        else if(StandardLocalNames::sum == localName)
        {
            s = addFunction(StandardLocalNames::sum, 1, 2, CommonSequenceTypes::ZeroOrOneAtomicType);
            s->appendArgument(argument(np, "arg"), CommonSequenceTypes::ZeroOrMoreAtomicTypes);
            s->appendArgument(argument(np, "zero"), CommonSequenceTypes::ZeroOrOneAtomicType);
        }
        else if(StandardLocalNames::translate == localName)
        {
            s = addFunction(StandardLocalNames::translate, 3, 3, CommonSequenceTypes::ExactlyOneString);
            s->appendArgument(argument(np, "arg"), CommonSequenceTypes::ZeroOrOneString);
            s->appendArgument(argument(np, "mapString"), CommonSequenceTypes::ExactlyOneString);
            s->appendArgument(argument(np, "transString"), CommonSequenceTypes::ExactlyOneString);
        }
        else if(StandardLocalNames::True == localName)
        {
            s = addFunction(StandardLocalNames::True, 0, 0, CommonSequenceTypes::ExactlyOneBoolean);
        }
    }

    return s;
}

QT_END_NAMESPACE
