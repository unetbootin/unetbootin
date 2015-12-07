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

#include "qanyuri_p.h"
#include "qatomizer_p.h"
#include "qbuiltintypes_p.h"
#include "qcardinalityverifier_p.h"
#include "qcommonsequencetypes_p.h"
#include "qcommonvalues_p.h"
#include "qemptysequence_p.h"
#include "qcommonnamespaces_p.h"
#include "qxmlname.h"
#include "qatomicstring_p.h"

/* Functions */
#include "qaccessorfns_p.h"
#include "qaggregatefns_p.h"
#include "qassemblestringfns_p.h"
#include "qbooleanfns_p.h"
#include "qcomparestringfns_p.h"
#include "qcomparingaggregator_p.h"
#include "qcontextfns_p.h"
#include "qdatetimefn_p.h"
#include "qdatetimefns_p.h"
#include "qdeepequalfn_p.h"
#include "qerrorfn_p.h"
#include "qnodefns_p.h"
#include "qnumericfns_p.h"
#include "qpatternmatchingfns_p.h"
#include "qqnamefns_p.h"
#include "qresolveurifn_p.h"
#include "qsequencefns_p.h"
#include "qsequencegeneratingfns_p.h"
#include "qstringvaluefns_p.h"
#include "qsubstringfns_p.h"
#include "qtimezonefns_p.h"
#include "qtracefn_p.h"

#include "qxpath20corefunctions_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

Expression::Ptr XPath20CoreFunctions::retrieveExpression(const QXmlName name,
                                                         const Expression::List &args,
                                                         const FunctionSignature::Ptr &sign) const
{
    Q_ASSERT(sign);

    Expression::Ptr fn;
#define testFN(ln, cname) else if(name.localName() == StandardLocalNames::ln) fn = Expression::Ptr(new cname())

    if(false) /* Dummy for the macro handling. Will be optimized away anyway. */
        return Expression::Ptr();
    /* Alphabetic order. */
    testFN(QName,                         QNameFN);
    testFN(abs,                           AbsFN);
    testFN(adjust_date_to_timezone,       AdjustDateToTimezoneFN);
    testFN(adjust_dateTime_to_timezone,   AdjustDateTimeToTimezoneFN);
    testFN(adjust_time_to_timezone,       AdjustTimeToTimezoneFN);
    testFN(avg,                           AvgFN);
    testFN(base_uri,                      BaseURIFN);
    testFN(codepoint_equal,               CodepointEqualFN);
    testFN(codepoints_to_string,          CodepointsToStringFN);
    testFN(collection,                    CollectionFN);
    testFN(compare,                       CompareFN);
    testFN(current_date,                  CurrentDateFN);
    testFN(current_dateTime,              CurrentDateTimeFN);
    testFN(current_time,                  CurrentTimeFN);
    testFN(dateTime,                      DateTimeFN);
    testFN(day_from_date,                 DayFromAbstractDateTimeFN);
    testFN(day_from_dateTime,             DayFromAbstractDateTimeFN);
    testFN(days_from_duration,            DaysFromDurationFN);
    testFN(deep_equal,                    DeepEqualFN);
    testFN(default_collation,             DefaultCollationFN);
    testFN(distinct_values,               DistinctValuesFN);
    testFN(doc,                           DocFN);
    testFN(doc_available,                 DocAvailableFN);
    testFN(document_uri,                  DocumentURIFN);
    testFN(empty,                         Existence<Expression::IDEmptyFN>);
    testFN(encode_for_uri,                EncodeForURIFN);
    testFN(ends_with,                     EndsWithFN);
    testFN(error,                         ErrorFN);
    testFN(escape_html_uri,               EscapeHtmlURIFN);
    testFN(exists,                        Existence<Expression::IDExistsFN>);
    testFN(hours_from_dateTime,           HoursFromAbstractDateTimeFN);
    testFN(hours_from_duration,           HoursFromDurationFN);
    testFN(hours_from_time,               HoursFromAbstractDateTimeFN);
    testFN(idref,                         IdrefFN);
    testFN(implicit_timezone,             ImplicitTimezoneFN);
    testFN(in_scope_prefixes,             InScopePrefixesFN);
    testFN(index_of,                      IndexOfFN);
    testFN(insert_before,                 InsertBeforeFN);
    testFN(iri_to_uri,                    IriToURIFN);
    testFN(local_name_from_QName,         LocalNameFromQNameFN);
    testFN(lower_case,                    LowerCaseFN);
    testFN(matches,                       MatchesFN);
    testFN(max,                           MaxFN);
    testFN(min,                           MinFN);
    testFN(minutes_from_dateTime,         MinutesFromAbstractDateTimeFN);
    testFN(minutes_from_duration,         MinutesFromDurationFN);
    testFN(minutes_from_time,             MinutesFromAbstractDateTimeFN);
    testFN(month_from_date,               MonthFromAbstractDateTimeFN);
    testFN(month_from_dateTime,           MonthFromAbstractDateTimeFN);
    testFN(months_from_duration,          MonthsFromDurationFN);
    testFN(namespace_uri_for_prefix,      NamespaceURIForPrefixFN);
    testFN(namespace_uri_from_QName,      NamespaceURIFromQNameFN);
    testFN(nilled,                        NilledFN);
    testFN(node_name,                     NodeNameFN);
    testFN(normalize_unicode,             NormalizeUnicodeFN);
    testFN(prefix_from_QName,             PrefixFromQNameFN);
    testFN(remove,                        RemoveFN);
    testFN(replace,                       ReplaceFN);
    testFN(resolve_QName,                 ResolveQNameFN);
    testFN(resolve_uri,                   ResolveURIFN);
    testFN(generic_string_join,           StringJoinFN);
    testFN(reverse,                       ReverseFN);
    testFN(root,                          RootFN);
    testFN(round_half_to_even,            RoundHalfToEvenFN);
    testFN(seconds_from_dateTime,         SecondsFromAbstractDateTimeFN);
    testFN(seconds_from_duration,         SecondsFromDurationFN);
    testFN(seconds_from_time,             SecondsFromAbstractDateTimeFN);
    testFN(static_base_uri,               StaticBaseURIFN);
    testFN(string_join,                   StringJoinFN);
    testFN(string_to_codepoints,          StringToCodepointsFN);
    testFN(subsequence,                   SubsequenceFN);
    testFN(timezone_from_date,            TimezoneFromAbstractDateTimeFN);
    testFN(timezone_from_dateTime,        TimezoneFromAbstractDateTimeFN);
    testFN(timezone_from_time,            TimezoneFromAbstractDateTimeFN);
    testFN(tokenize,                      TokenizeFN);
    testFN(trace,                         TraceFN);
    testFN(upper_case,                    UpperCaseFN);
    testFN(year_from_date,                YearFromAbstractDateTimeFN);
    testFN(year_from_dateTime,            YearFromAbstractDateTimeFN);
    testFN(years_from_duration,           YearsFromDurationFN);
#undef testFN

    if(fn)
    {
        fn->setOperands(args);
        fn->as<FunctionCall>()->setSignature(sign);
    }
    else
    {
        /* Do the ones which are not FunctionCall sub-classes. The effect is
         * that FunctionCall sub-classes has "automatic" type checking in the base
         * class done from the background of their function signature, while
         * these special classes are on their own, and must do it manually. */
        if(name.localName() == StandardLocalNames::data)
            fn = Expression::Ptr(new Atomizer(args.first()));
        else if(name.localName() == StandardLocalNames::zero_or_one)
            fn = Expression::Ptr(new CardinalityVerifier(args.first(), Cardinality::zeroOrOne(),
                                                         ReportContext::FORG0003));
        else if(name.localName() == StandardLocalNames::one_or_more)
            fn = Expression::Ptr(new CardinalityVerifier(args.first(), Cardinality::oneOrMore(),
                                                         ReportContext::FORG0004));
        else if(name.localName() == StandardLocalNames::exactly_one)
            fn = Expression::Ptr(new CardinalityVerifier(args.first(), Cardinality::exactlyOne(),
                                                         ReportContext::FORG0005));
        else if(name.localName() == StandardLocalNames::unordered)
        /* We don't make use of the unordered() function, so just pop in
         * the arg. */
            fn = args.first();
    }

    return fn;
}

FunctionSignature::Ptr XPath20CoreFunctions::retrieveFunctionSignature(const NamePool::Ptr &np,
                                                                       const QXmlName name)
{
    if(StandardNamespaces::fn != name.namespaceURI() && name.namespaceURI() != StandardNamespaces::InternalXSLT)
        return FunctionSignature::Ptr();

    FunctionSignature::Ptr s(functionSignatures().value(name));

    if(!s)
    {
        const QXmlName::LocalNameCode localName = name.localName();

        /* Alphabetic order. */
        if(StandardLocalNames::QName == localName)
        {
            s = addFunction(StandardLocalNames::QName, 2, 2, CommonSequenceTypes::ExactlyOneQName);
            s->appendArgument(argument(np, "paramURI"), CommonSequenceTypes::ZeroOrOneString);
            s->appendArgument(argument(np, "paramQName"), CommonSequenceTypes::ExactlyOneString);
        }
        else if(StandardLocalNames::abs == localName)
        {
            s = addFunction(StandardLocalNames::abs, 1, 1, CommonSequenceTypes::ZeroOrOneNumeric,
                            Expression::EmptynessFollowsChild | Expression::RewriteToEmptyOnEmpty);
            s->appendArgument(argument(np, "arg"), CommonSequenceTypes::ZeroOrOneNumeric);
        }
        else if(StandardLocalNames::adjust_date_to_timezone == localName)
        {
            s = addFunction(StandardLocalNames::adjust_date_to_timezone, 1, 2, CommonSequenceTypes::ZeroOrOneDate,
                            Expression::EmptynessFollowsChild | Expression::RewriteToEmptyOnEmpty);
            s->appendArgument(argument(np, "arg"), CommonSequenceTypes::ZeroOrOneDate);
            s->appendArgument(argument(np, "timezone"), CommonSequenceTypes::ZeroOrOneDayTimeDuration);
        }
        else if(StandardLocalNames::adjust_dateTime_to_timezone == localName)
        {
            s = addFunction(StandardLocalNames::adjust_dateTime_to_timezone, 1, 2, CommonSequenceTypes::ZeroOrOneDateTime,
                            Expression::EmptynessFollowsChild | Expression::RewriteToEmptyOnEmpty);
            s->appendArgument(argument(np, "arg"), CommonSequenceTypes::ZeroOrOneDateTime);
            s->appendArgument(argument(np, "timezone"), CommonSequenceTypes::ZeroOrOneDayTimeDuration);
        }
        else if(StandardLocalNames::adjust_time_to_timezone == localName)
        {
            s = addFunction(StandardLocalNames::adjust_time_to_timezone, 1, 2, CommonSequenceTypes::ZeroOrOneTime,
                            Expression::EmptynessFollowsChild | Expression::RewriteToEmptyOnEmpty);
            s->appendArgument(argument(np, "arg"), CommonSequenceTypes::ZeroOrOneTime);
            s->appendArgument(argument(np, "timezone"), CommonSequenceTypes::ZeroOrOneDayTimeDuration);
        }
        else if(StandardLocalNames::avg == localName)
        {
            s = addFunction(StandardLocalNames::avg, 1, 1, CommonSequenceTypes::ZeroOrOneAtomicType,
                            Expression::EmptynessFollowsChild | Expression::RewriteToEmptyOnEmpty);
            s->appendArgument(argument(np, "arg"), CommonSequenceTypes::ZeroOrMoreAtomicTypes);
        }
        else if(StandardLocalNames::base_uri == localName)
        {
            s = addFunction(StandardLocalNames::base_uri, 0, 1, CommonSequenceTypes::ZeroOrOneAnyURI,
                            Expression::UseContextItem);
            s->appendArgument(argument(np, "arg"), CommonSequenceTypes::ZeroOrOneNode);
        }
        else if(StandardLocalNames::codepoint_equal == localName)
        {
            s = addFunction(StandardLocalNames::codepoint_equal, 2, 2, CommonSequenceTypes::ZeroOrOneBoolean);
            s->appendArgument(argument(np, "comparand1"), CommonSequenceTypes::ZeroOrOneString);
            s->appendArgument(argument(np, "comparand2"), CommonSequenceTypes::ZeroOrOneString);
        }
        else if(StandardLocalNames::codepoints_to_string == localName)
        {
            s = addFunction(StandardLocalNames::codepoints_to_string, 1, 1, CommonSequenceTypes::ExactlyOneString);
            s->appendArgument(argument(np, "arg"), CommonSequenceTypes::ZeroOrMoreIntegers);
        }
        else if(StandardLocalNames::collection == localName)
        {
            s = addFunction(StandardLocalNames::collection, 0, 1, CommonSequenceTypes::ZeroOrMoreNodes);
            s->appendArgument(argument(np, "uri"), CommonSequenceTypes::ZeroOrOneString);
        }
        else if(StandardLocalNames::compare == localName)
        {
            s = addFunction(StandardLocalNames::compare, 2, 3, CommonSequenceTypes::ZeroOrOneInteger,
                            Expression::LastOperandIsCollation);
            s->appendArgument(argument(np, "comparand1"), CommonSequenceTypes::ZeroOrOneString);
            s->appendArgument(argument(np, "comparand2"), CommonSequenceTypes::ZeroOrOneString);
            s->appendArgument(argument(np, "collation"), CommonSequenceTypes::ExactlyOneString);
        }
        else if(StandardLocalNames::current_date == localName)
        {
            s = addFunction(StandardLocalNames::current_date, 0, 0, CommonSequenceTypes::ExactlyOneDate,
                            Expression::DisableElimination);
        }
        else if(StandardLocalNames::current_dateTime == localName)
        {
            s = addFunction(StandardLocalNames::current_dateTime, 0, 0, CommonSequenceTypes::ExactlyOneDateTime,
                            Expression::DisableElimination);
        }
        else if(StandardLocalNames::current_time == localName)
        {
            s = addFunction(StandardLocalNames::current_time, 0, 0, CommonSequenceTypes::ExactlyOneTime,
                            Expression::DisableElimination);
        }
        else if(StandardLocalNames::data == localName)
        {
            s = addFunction(StandardLocalNames::data, 1, 1, CommonSequenceTypes::ZeroOrMoreAtomicTypes);
            s->appendArgument(argument(np, "arg"), CommonSequenceTypes::ZeroOrMoreItems);
        }
        else if(StandardLocalNames::dateTime == localName)
        {
            s = addFunction(StandardLocalNames::dateTime, 2, 2, CommonSequenceTypes::ZeroOrOneDateTime);
            s->appendArgument(argument(np, "arg1"), CommonSequenceTypes::ZeroOrOneDate);
            s->appendArgument(argument(np, "arg2"), CommonSequenceTypes::ZeroOrOneTime);
        }
        else if(StandardLocalNames::day_from_date == localName)
        {
            s = addFunction(StandardLocalNames::day_from_date, 1, 1, CommonSequenceTypes::ZeroOrOneInteger,
                            Expression::EmptynessFollowsChild | Expression::RewriteToEmptyOnEmpty);
            s->appendArgument(argument(np, "arg"), CommonSequenceTypes::ZeroOrOneDate);
        }
        else if(StandardLocalNames::day_from_dateTime == localName)
        {
            s = addFunction(StandardLocalNames::day_from_dateTime, 1, 1, CommonSequenceTypes::ZeroOrOneInteger,
                            Expression::EmptynessFollowsChild | Expression::RewriteToEmptyOnEmpty);
            s->appendArgument(argument(np, "arg"), CommonSequenceTypes::ZeroOrOneDateTime);
        }
        else if(StandardLocalNames::days_from_duration == localName)
        {
            s = addFunction(StandardLocalNames::days_from_duration, 1, 1, CommonSequenceTypes::ZeroOrOneInteger,
                            Expression::EmptynessFollowsChild | Expression::RewriteToEmptyOnEmpty);
            s->appendArgument(argument(np, "arg"), CommonSequenceTypes::ZeroOrOneDuration);
        }
        else if(StandardLocalNames::deep_equal == localName)
        {
            s = addFunction(StandardLocalNames::deep_equal, 2, 3, CommonSequenceTypes::ExactlyOneBoolean,
                            Expression::LastOperandIsCollation);
            s->appendArgument(argument(np, "arg1"), CommonSequenceTypes::ZeroOrMoreItems);
            s->appendArgument(argument(np, "arg2"), CommonSequenceTypes::ZeroOrMoreItems);
            s->appendArgument(argument(np, "collation"), CommonSequenceTypes::ExactlyOneString);
        }
        else if(StandardLocalNames::default_collation == localName)
        {
            s = addFunction(StandardLocalNames::default_collation, 0, 0, CommonSequenceTypes::ExactlyOneString);
        }
        else if(StandardLocalNames::distinct_values == localName)
        {
            s = addFunction(StandardLocalNames::distinct_values, 1, 2, CommonSequenceTypes::ZeroOrMoreAtomicTypes,
                            Expression::LastOperandIsCollation   |
                            Expression::EmptynessFollowsChild    |
                            Expression::RewriteToEmptyOnEmpty);
            s->appendArgument(argument(np, "arg"), CommonSequenceTypes::ZeroOrMoreAtomicTypes);
            s->appendArgument(argument(np, "collation"), CommonSequenceTypes::ExactlyOneString);
        }
        else if(StandardLocalNames::doc == localName)
        {
            s = addFunction(StandardLocalNames::doc, 1, 1, CommonSequenceTypes::ZeroOrOneDocumentNode, Expression::DisableElimination);
            s->appendArgument(argument(np, "uri"), CommonSequenceTypes::ZeroOrOneString);
        }
        else if(StandardLocalNames::doc_available == localName)
        {
            s = addFunction(StandardLocalNames::doc_available, 1, 1, CommonSequenceTypes::ExactlyOneBoolean, Expression::DisableElimination);
            s->appendArgument(argument(np, "uri"), CommonSequenceTypes::ZeroOrOneString);
        }
        else if(StandardLocalNames::document_uri == localName)
        {
            s = addFunction(StandardLocalNames::document_uri, 1, 1, CommonSequenceTypes::ZeroOrOneAnyURI);
            s->appendArgument(argument(np, "arg"), CommonSequenceTypes::ZeroOrOneNode);
        }
        else if(StandardLocalNames::empty == localName)
        {
            s = addFunction(StandardLocalNames::empty, 1, 1, CommonSequenceTypes::ExactlyOneBoolean, Expression::IDEmptyFN);
            s->appendArgument(argument(np, "arg"), CommonSequenceTypes::ZeroOrMoreItems);
        }
        else if(StandardLocalNames::encode_for_uri == localName)
        {
            s = addFunction(StandardLocalNames::encode_for_uri, 1, 1, CommonSequenceTypes::ExactlyOneString);
            s->appendArgument(argument(np, "uriPart"), CommonSequenceTypes::ZeroOrOneString);
        }
        else if(StandardLocalNames::ends_with == localName)
        {
            s = addFunction(StandardLocalNames::ends_with, 2, 3, CommonSequenceTypes::ExactlyOneBoolean,
                            Expression::LastOperandIsCollation);
            s->appendArgument(argument(np, "arg1"), CommonSequenceTypes::ZeroOrOneString);
            s->appendArgument(argument(np, "arg2"), CommonSequenceTypes::ZeroOrOneString);
            s->appendArgument(argument(np, "collation"), CommonSequenceTypes::ExactlyOneString);
        }
        else if(StandardLocalNames::error == localName)
        {
            s = addFunction(StandardLocalNames::error, 0, 3, CommonSequenceTypes::None,
                            Expression::DisableElimination | Expression::DisableTypingDeduction);
            s->appendArgument(argument(np, "error"), CommonSequenceTypes::ZeroOrOneQName);
            s->appendArgument(argument(np, "description"), CommonSequenceTypes::ExactlyOneString);
            s->appendArgument(argument(np, "errorObject"), CommonSequenceTypes::ZeroOrMoreItems);
        }
        else if(StandardLocalNames::escape_html_uri == localName)
        {
            s = addFunction(StandardLocalNames::escape_html_uri, 1, 1, CommonSequenceTypes::ExactlyOneString);
            s->appendArgument(argument(np, "uri"), CommonSequenceTypes::ZeroOrOneString);
        }
        else if(StandardLocalNames::exactly_one == localName)
        {
            s = addFunction(StandardLocalNames::exactly_one, 1, 1, CommonSequenceTypes::ExactlyOneItem);
            s->appendArgument(argument(np, "arg"), CommonSequenceTypes::ExactlyOneItem);
        }
        else if(StandardLocalNames::exists == localName)
        {
            s = addFunction(StandardLocalNames::exists, 1, 1, CommonSequenceTypes::ExactlyOneBoolean, Expression::IDExistsFN);
            s->appendArgument(argument(np, "arg"), CommonSequenceTypes::ZeroOrMoreItems);
        }
        else if(StandardLocalNames::hours_from_dateTime == localName)
        {
            s = addFunction(StandardLocalNames::hours_from_dateTime, 1, 1, CommonSequenceTypes::ZeroOrOneInteger,
                            Expression::EmptynessFollowsChild | Expression::RewriteToEmptyOnEmpty);
            s->appendArgument(argument(np, "arg"), CommonSequenceTypes::ZeroOrOneDateTime);
        }
        else if(StandardLocalNames::hours_from_duration == localName)
        {
            s = addFunction(StandardLocalNames::hours_from_duration, 1, 1, CommonSequenceTypes::ZeroOrOneInteger,
                            Expression::EmptynessFollowsChild | Expression::RewriteToEmptyOnEmpty);
            s->appendArgument(argument(np, "arg"), CommonSequenceTypes::ZeroOrOneDuration);
        }
        else if(StandardLocalNames::hours_from_time == localName)
        {
            s = addFunction(StandardLocalNames::hours_from_time, 1, 1, CommonSequenceTypes::ZeroOrOneInteger,
                            Expression::EmptynessFollowsChild | Expression::RewriteToEmptyOnEmpty);
            s->appendArgument(argument(np, "arg"), CommonSequenceTypes::ZeroOrOneTime);
        }
        else if(StandardLocalNames::idref == localName)
        {
            s = addFunction(StandardLocalNames::idref, 1, 2, CommonSequenceTypes::ZeroOrMoreElements,
                            Expression::UseContextItem);
            s->appendArgument(argument(np, "idrefs"),      CommonSequenceTypes::ZeroOrMoreStrings);
            s->appendArgument(argument(np, "node"),        CommonSequenceTypes::ExactlyOneNode);
        }
        else if(StandardLocalNames::implicit_timezone == localName)
        {
            s = addFunction(StandardLocalNames::implicit_timezone, 0, 0, CommonSequenceTypes::ExactlyOneDayTimeDuration,
                            Expression::DisableElimination);
        }
        else if(StandardLocalNames::in_scope_prefixes == localName)
        {
            s = addFunction(StandardLocalNames::in_scope_prefixes, 1, 1, CommonSequenceTypes::ZeroOrMoreStrings);
            s->appendArgument(argument(np, "element"), CommonSequenceTypes::ExactlyOneElement);
        }
        else if(StandardLocalNames::index_of == localName)
        {
            s = addFunction(StandardLocalNames::index_of, 2, 3, CommonSequenceTypes::ZeroOrMoreIntegers,
                            Expression::LastOperandIsCollation);
            s->appendArgument(argument(np, "seqParam"), CommonSequenceTypes::ZeroOrMoreAtomicTypes);
            s->appendArgument(argument(np, "searchParam"), CommonSequenceTypes::ExactlyOneAtomicType);
            s->appendArgument(argument(np, "collation"), CommonSequenceTypes::ExactlyOneString);
        }
        else if(StandardLocalNames::insert_before == localName)
        {
            s = addFunction(StandardLocalNames::insert_before, 3, 3, CommonSequenceTypes::ZeroOrMoreItems);
            s->appendArgument(argument(np, "target"), CommonSequenceTypes::ZeroOrMoreItems);
            s->appendArgument(argument(np, "position"), CommonSequenceTypes::ExactlyOneInteger);
            s->appendArgument(argument(np, "insert"), CommonSequenceTypes::ZeroOrMoreItems);
        }
        else if(StandardLocalNames::iri_to_uri == localName)
        {
            s = addFunction(StandardLocalNames::iri_to_uri, 1, 1, CommonSequenceTypes::ExactlyOneString);
            s->appendArgument(argument(np, "uri_part"), CommonSequenceTypes::ZeroOrOneString);
        }
        else if(StandardLocalNames::local_name_from_QName == localName)
        {
            s = addFunction(StandardLocalNames::local_name_from_QName, 1, 1, CommonSequenceTypes::ZeroOrOneNCName,
                            Expression::EmptynessFollowsChild | Expression::RewriteToEmptyOnEmpty);
            s->appendArgument(argument(np, "arg"), CommonSequenceTypes::ZeroOrOneQName);
        }
        else if(StandardLocalNames::lower_case == localName)
        {
            s = addFunction(StandardLocalNames::lower_case, 1, 1, CommonSequenceTypes::ExactlyOneString,
                            Expression::IDLowerCaseFN);
            s->appendArgument(argument(np, "arg"), CommonSequenceTypes::ZeroOrOneString);
        }
        else if(StandardLocalNames::matches == localName)
        {
            s = addFunction(StandardLocalNames::matches, 2, 3, CommonSequenceTypes::ExactlyOneBoolean);
            s->appendArgument(argument(np, "input"), CommonSequenceTypes::ZeroOrOneString);
            s->appendArgument(argument(np, "pattern"), CommonSequenceTypes::ExactlyOneString);
            s->appendArgument(argument(np, "flags"), CommonSequenceTypes::ExactlyOneString);
        }
        else if(StandardLocalNames::max == localName)
        {
            s = addFunction(StandardLocalNames::max, 1, 2, CommonSequenceTypes::ZeroOrOneAtomicType,
                           Expression::LastOperandIsCollation   |
                           Expression::EmptynessFollowsChild    |
                           Expression::RewriteToEmptyOnEmpty);
            s->appendArgument(argument(np, "arg"), CommonSequenceTypes::ZeroOrMoreAtomicTypes);
            s->appendArgument(argument(np, "collation"), CommonSequenceTypes::ExactlyOneString);
        }
        else if(StandardLocalNames::min == localName)
        {
            s = addFunction(StandardLocalNames::min, 1, 2, CommonSequenceTypes::ZeroOrOneAtomicType,
                            Expression::LastOperandIsCollation   |
                            Expression::EmptynessFollowsChild    |
                            Expression::RewriteToEmptyOnEmpty);
            s->appendArgument(argument(np, "arg"), CommonSequenceTypes::ZeroOrMoreAtomicTypes);
            s->appendArgument(argument(np, "collation"), CommonSequenceTypes::ExactlyOneString);
        }
        else if(StandardLocalNames::minutes_from_dateTime == localName)
        {
            s = addFunction(StandardLocalNames::minutes_from_dateTime, 1, 1, CommonSequenceTypes::ZeroOrOneInteger,
                            Expression::EmptynessFollowsChild | Expression::RewriteToEmptyOnEmpty);
            s->appendArgument(argument(np, "arg"), CommonSequenceTypes::ZeroOrOneDateTime);
        }
        else if(StandardLocalNames::minutes_from_duration == localName)
        {
            s = addFunction(StandardLocalNames::minutes_from_duration, 1, 1, CommonSequenceTypes::ZeroOrOneInteger,
                            Expression::EmptynessFollowsChild | Expression::RewriteToEmptyOnEmpty);
            s->appendArgument(argument(np, "arg"), CommonSequenceTypes::ZeroOrOneDuration);
        }
        else if(StandardLocalNames::minutes_from_time == localName)
        {
            s = addFunction(StandardLocalNames::minutes_from_time, 1, 1, CommonSequenceTypes::ZeroOrOneInteger,
                            Expression::EmptynessFollowsChild | Expression::RewriteToEmptyOnEmpty);
            s->appendArgument(argument(np, "arg"), CommonSequenceTypes::ZeroOrOneTime);
        }
        else if(StandardLocalNames::month_from_date == localName)
        {
            s = addFunction(StandardLocalNames::month_from_date, 1, 1, CommonSequenceTypes::ZeroOrOneInteger,
                            Expression::EmptynessFollowsChild | Expression::RewriteToEmptyOnEmpty);
            s->appendArgument(argument(np, "arg"), CommonSequenceTypes::ZeroOrOneDate);
        }
        else if(StandardLocalNames::month_from_dateTime == localName)
        {
            s = addFunction(StandardLocalNames::month_from_dateTime, 1, 1, CommonSequenceTypes::ZeroOrOneInteger,
                            Expression::EmptynessFollowsChild | Expression::RewriteToEmptyOnEmpty);
            s->appendArgument(argument(np, "arg"), CommonSequenceTypes::ZeroOrOneDateTime);
        }
        else if(StandardLocalNames::months_from_duration == localName)
        {
            s = addFunction(StandardLocalNames::months_from_duration, 1, 1, CommonSequenceTypes::ZeroOrOneInteger,
                            Expression::EmptynessFollowsChild | Expression::RewriteToEmptyOnEmpty);
            s->appendArgument(argument(np, "arg"), CommonSequenceTypes::ZeroOrOneDuration);
        }
        else if(StandardLocalNames::namespace_uri_for_prefix == localName)
        {
            s = addFunction(StandardLocalNames::namespace_uri_for_prefix, 2, 2, CommonSequenceTypes::ZeroOrOneAnyURI);
            s->appendArgument(argument(np, "prefix"), CommonSequenceTypes::ZeroOrOneString);
            s->appendArgument(argument(np, "element"), CommonSequenceTypes::ExactlyOneElement);
        }
        else if(StandardLocalNames::namespace_uri_from_QName == localName)
        {
            s = addFunction(StandardLocalNames::namespace_uri_from_QName, 1, 1, CommonSequenceTypes::ZeroOrOneAnyURI,
                            Expression::EmptynessFollowsChild | Expression::RewriteToEmptyOnEmpty);
            s->appendArgument(argument(np, "arg"), CommonSequenceTypes::ZeroOrOneQName);
        }
        else if(StandardLocalNames::nilled == localName)
        {
            s = addFunction(StandardLocalNames::nilled, 1, 1, CommonSequenceTypes::ZeroOrOneBoolean);
            s->appendArgument(argument(np, "arg"), CommonSequenceTypes::ZeroOrOneNode);
        }
        else if(StandardLocalNames::node_name == localName)
        {
            s = addFunction(StandardLocalNames::node_name, 1, 1, CommonSequenceTypes::ZeroOrOneQName);
            s->appendArgument(argument(np, "theNode"), CommonSequenceTypes::ZeroOrOneNode);
        }
        else if(StandardLocalNames::normalize_unicode == localName)
        {
            s = addFunction(StandardLocalNames::normalize_unicode, 1, 2, CommonSequenceTypes::ExactlyOneString);
            s->appendArgument(argument(np, "arg"), CommonSequenceTypes::ZeroOrOneString);
            s->appendArgument(argument(np, "normalizationForm"), CommonSequenceTypes::ExactlyOneString);
        }
        else if(StandardLocalNames::one_or_more == localName)
        {
            s = addFunction(StandardLocalNames::one_or_more, 1, 1, CommonSequenceTypes::OneOrMoreItems);
            s->appendArgument(argument(np, "arg"), CommonSequenceTypes::ZeroOrMoreItems);
        }
        else if(StandardLocalNames::prefix_from_QName == localName)
        {
            s = addFunction(StandardLocalNames::prefix_from_QName, 1, 1, CommonSequenceTypes::ZeroOrOneNCName,
                            Expression::RewriteToEmptyOnEmpty);
            s->appendArgument(argument(np, "arg"), CommonSequenceTypes::ZeroOrOneQName);
        }
        else if(StandardLocalNames::remove == localName)
        {
            s = addFunction(StandardLocalNames::remove, 2, 2, CommonSequenceTypes::ZeroOrMoreItems,
                            Expression::RewriteToEmptyOnEmpty);
            s->appendArgument(argument(np, "target"), CommonSequenceTypes::ZeroOrMoreItems);
            s->appendArgument(argument(np, "position"), CommonSequenceTypes::ExactlyOneInteger);
        }
        else if(StandardLocalNames::replace == localName)
        {
            s = addFunction(StandardLocalNames::replace, 3, 4, CommonSequenceTypes::ExactlyOneString);
            s->appendArgument(argument(np, "input"), CommonSequenceTypes::ZeroOrOneString);
            s->appendArgument(argument(np, "pattern"), CommonSequenceTypes::ExactlyOneString);
            s->appendArgument(argument(np, "replacement"), CommonSequenceTypes::ExactlyOneString);
            s->appendArgument(argument(np, "flags"), CommonSequenceTypes::ExactlyOneString);
        }
        else if(StandardLocalNames::resolve_QName == localName)
        {
            s = addFunction(StandardLocalNames::resolve_QName, 2, 2, CommonSequenceTypes::ZeroOrOneQName,
                            Expression::EmptynessFollowsChild | Expression::RewriteToEmptyOnEmpty);
            s->appendArgument(argument(np, "qname"), CommonSequenceTypes::ZeroOrOneString);
            s->appendArgument(argument(np, "element"), CommonSequenceTypes::ExactlyOneElement);
        }
        else if(StandardLocalNames::resolve_uri == localName)
        {
            s = addFunction(StandardLocalNames::resolve_uri, 1, 2, CommonSequenceTypes::ZeroOrOneAnyURI,
                            Expression::EmptynessFollowsChild | Expression::RewriteToEmptyOnEmpty);
            s->appendArgument(argument(np, "relative"), CommonSequenceTypes::ZeroOrOneString);
            s->appendArgument(argument(np, "base"), CommonSequenceTypes::ExactlyOneString);
        }
        else if(StandardLocalNames::reverse == localName)
        {
            s = addFunction(StandardLocalNames::reverse, 1, 1, CommonSequenceTypes::ZeroOrMoreItems);
            s->appendArgument(argument(np, "arg"), CommonSequenceTypes::ZeroOrMoreItems);
        }
        else if(StandardLocalNames::root == localName)
        {
            s = addFunction(StandardLocalNames::root, 0, 1, CommonSequenceTypes::ZeroOrOneNode,
                            Expression::EmptynessFollowsChild |
                            Expression::RewriteToEmptyOnEmpty |
                            Expression::UseContextItem);
            s->appendArgument(argument(np, "arg"), CommonSequenceTypes::ZeroOrOneNode);
        }
        else if(StandardLocalNames::round_half_to_even == localName)
        {
            s = addFunction(StandardLocalNames::round_half_to_even, 1, 2, CommonSequenceTypes::ZeroOrOneNumeric,
                            Expression::EmptynessFollowsChild | Expression::RewriteToEmptyOnEmpty);
            s->appendArgument(argument(np, "arg"), CommonSequenceTypes::ZeroOrOneNumeric);
            s->appendArgument(argument(np, "precision"), CommonSequenceTypes::ExactlyOneInteger);
        }
        else if(StandardLocalNames::seconds_from_dateTime == localName)
        {
            s = addFunction(StandardLocalNames::seconds_from_dateTime, 1, 1, CommonSequenceTypes::ZeroOrOneDecimal,
                    Expression::EmptynessFollowsChild | Expression::RewriteToEmptyOnEmpty);
            s->appendArgument(argument(np, "arg"), CommonSequenceTypes::ZeroOrOneDateTime);
        }
        else if(StandardLocalNames::seconds_from_duration == localName)
        {
            s = addFunction(StandardLocalNames::seconds_from_duration, 1, 1, CommonSequenceTypes::ZeroOrOneDecimal,
                            Expression::EmptynessFollowsChild | Expression::RewriteToEmptyOnEmpty);
            s->appendArgument(argument(np, "arg"), CommonSequenceTypes::ZeroOrOneDuration);
        }
        else if(StandardLocalNames::seconds_from_time == localName)
        {
            s = addFunction(StandardLocalNames::seconds_from_time, 1, 1, CommonSequenceTypes::ZeroOrOneDecimal,
                            Expression::EmptynessFollowsChild | Expression::RewriteToEmptyOnEmpty);
            s->appendArgument(argument(np, "arg"), CommonSequenceTypes::ZeroOrOneTime);
        }
        else if(StandardLocalNames::static_base_uri == localName)
        {
            s = addFunction(StandardLocalNames::static_base_uri, 0, 0, CommonSequenceTypes::ExactlyOneAnyURI, Expression::EmptynessFollowsChild);
        }
        else if(StandardLocalNames::string_join == localName)
        {
            s = addFunction(StandardLocalNames::string_join, 2, 2, CommonSequenceTypes::ExactlyOneString);
            s->appendArgument(argument(np, "arg1"), CommonSequenceTypes::ZeroOrMoreStrings);
            s->appendArgument(argument(np, "separator"), CommonSequenceTypes::ExactlyOneString);
        }
        else if(StandardLocalNames::generic_string_join == localName)
        {
            s = addFunction(StandardLocalNames::generic_string_join, 2, 2, CommonSequenceTypes::ExactlyOneString,
                            Expression::IDIgnorableExpression,
                            Expression::Properties(),
                            StandardNamespaces::InternalXSLT);
            s->appendArgument(argument(np, "arg1"), CommonSequenceTypes::ZeroOrMoreAtomicTypes);
            s->appendArgument(argument(np, "separator"), CommonSequenceTypes::ExactlyOneString);
        }
        else if(StandardLocalNames::string_to_codepoints == localName)
        {
            s = addFunction(StandardLocalNames::string_to_codepoints, 1, 1, CommonSequenceTypes::ZeroOrMoreIntegers);
            s->appendArgument(argument(np, "arg"), CommonSequenceTypes::ZeroOrOneString);
        }
        else if(StandardLocalNames::subsequence == localName)
        {
            s = addFunction(StandardLocalNames::subsequence, 2, 3, CommonSequenceTypes::ZeroOrMoreItems);
            s->appendArgument(argument(np, "sourceSeq"), CommonSequenceTypes::ZeroOrMoreItems);
            s->appendArgument(argument(np, "startingLoc"), CommonSequenceTypes::ExactlyOneDouble);
            s->appendArgument(argument(np, "length"), CommonSequenceTypes::ExactlyOneDouble);
        }
        else if(StandardLocalNames::timezone_from_date == localName)
        {
            s = addFunction(StandardLocalNames::timezone_from_date, 1, 1, CommonSequenceTypes::ZeroOrOneDayTimeDuration,
                            Expression::RewriteToEmptyOnEmpty);
            s->appendArgument(argument(np, "arg"), CommonSequenceTypes::ZeroOrOneDate);
        }
        else if(StandardLocalNames::timezone_from_dateTime == localName)
        {
            s = addFunction(StandardLocalNames::timezone_from_dateTime, 1, 1, CommonSequenceTypes::ZeroOrOneDayTimeDuration,
                            Expression::RewriteToEmptyOnEmpty);
            s->appendArgument(argument(np, "arg"), CommonSequenceTypes::ZeroOrOneDateTime);
        }
        else if(StandardLocalNames::timezone_from_time == localName)
        {
            s = addFunction(StandardLocalNames::timezone_from_time, 1, 1, CommonSequenceTypes::ZeroOrOneDayTimeDuration,
                            Expression::RewriteToEmptyOnEmpty);
            s->appendArgument(argument(np, "arg"), CommonSequenceTypes::ZeroOrOneTime);
        }
        else if(StandardLocalNames::tokenize == localName)
        {
            s = addFunction(StandardLocalNames::tokenize, 2, 3, CommonSequenceTypes::ZeroOrMoreStrings);
            s->appendArgument(argument(np, "input"), CommonSequenceTypes::ZeroOrOneString);
            s->appendArgument(argument(np, "pattern"), CommonSequenceTypes::ExactlyOneString);
            s->appendArgument(argument(np, "flags"), CommonSequenceTypes::ExactlyOneString);
        }
        else if(StandardLocalNames::trace == localName)
        {
            s = addFunction(StandardLocalNames::trace, 2, 2, CommonSequenceTypes::ZeroOrMoreItems,
                            Expression::DisableElimination);
            s->appendArgument(argument(np, "value"), CommonSequenceTypes::ZeroOrMoreItems);
            s->appendArgument(argument(np, "label"), CommonSequenceTypes::ExactlyOneString);
        }
        else if(StandardLocalNames::unordered == localName)
        {
            s = addFunction(StandardLocalNames::unordered, 1, 1, CommonSequenceTypes::ZeroOrMoreItems);
            s->appendArgument(argument(np, "sourceSeq"), CommonSequenceTypes::ZeroOrMoreItems);
        }
        else if(StandardLocalNames::upper_case == localName)
        {
            s = addFunction(StandardLocalNames::upper_case, 1, 1, CommonSequenceTypes::ExactlyOneString,
                            Expression::IDUpperCaseFN);
            s->appendArgument(argument(np, "arg"), CommonSequenceTypes::ZeroOrOneString);
        }
        else if(StandardLocalNames::year_from_date == localName)
        {
            s = addFunction(StandardLocalNames::year_from_date, 1, 1, CommonSequenceTypes::ZeroOrOneInteger,
                            Expression::EmptynessFollowsChild | Expression::RewriteToEmptyOnEmpty);
            s->appendArgument(argument(np, "arg"), CommonSequenceTypes::ZeroOrOneDate);
        }
        else if(StandardLocalNames::year_from_dateTime == localName)
        {
            s = addFunction(StandardLocalNames::year_from_dateTime, 1, 1, CommonSequenceTypes::ZeroOrOneInteger,
                            Expression::EmptynessFollowsChild | Expression::RewriteToEmptyOnEmpty);
            s->appendArgument(argument(np, "arg"), CommonSequenceTypes::ZeroOrOneDateTime);
        }
        else if(StandardLocalNames::years_from_duration == localName)
        {
            s = addFunction(StandardLocalNames::years_from_duration, 1, 1, CommonSequenceTypes::ZeroOrOneInteger,
                            Expression::EmptynessFollowsChild | Expression::RewriteToEmptyOnEmpty);
            s->appendArgument(argument(np, "arg"), CommonSequenceTypes::ZeroOrOneDuration);
        }
        else if(StandardLocalNames::zero_or_one == localName)
        {
            s = addFunction(StandardLocalNames::zero_or_one, 1, 1, CommonSequenceTypes::ZeroOrOneItem);
            s->appendArgument(argument(np, "arg"), CommonSequenceTypes::ZeroOrMoreItems);
        }
    }

    return s;
}


QT_END_NAMESPACE
