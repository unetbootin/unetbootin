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
#include "qcommonnamespaces_p.h"
#include "qcommonsequencetypes_p.h"
#include "qcurrentfn_p.h"
#include "qdocumentfn_p.h"
#include "qelementavailablefn_p.h"
#include "qfunctionavailablefn_p.h"
#include "qgenerateidfn_p.h"
#include "qsystempropertyfn_p.h"
#include "qtypeavailablefn_p.h"
#include "qunparsedentitypublicidfn_p.h"
#include "qunparsedentityurifn_p.h"
#include "qunparsedtextavailablefn_p.h"
#include "qunparsedtextfn_p.h"

#include "qxslt20corefunctions_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

Expression::Ptr XSLT20CoreFunctions::retrieveExpression(const QXmlName lname,
                                                        const Expression::List &args,
                                                        const FunctionSignature::Ptr &sign) const
{
    Q_ASSERT(sign);

    Expression::Ptr fn;
#define testXSLTFN(ln, cname) else if(lname.localName() == StandardLocalNames::ln) fn = Expression::Ptr(new cname())

    if(false) /* Dummy for the macro handling. Will be optimized away anyway. */
        return Expression::Ptr();
    /* Alphabetic order. */
    testXSLTFN(current,                     CurrentFN);
    testXSLTFN(document,                    DocumentFN);
    testXSLTFN(element_available,           ElementAvailableFN);
    testXSLTFN(function_available,          FunctionAvailableFN);
    testXSLTFN(generate_id,                 GenerateIDFN);
    testXSLTFN(system_property,             SystemPropertyFN);
    testXSLTFN(type_available,              TypeAvailableFN);
    testXSLTFN(unparsed_entity_public_id,   UnparsedEntityPublicIDFN);
    testXSLTFN(unparsed_entity_uri,         UnparsedEntityURIFN);
    testXSLTFN(unparsed_text_available,     UnparsedTextAvailableFN);
    testXSLTFN(unparsed_text,               UnparsedTextFN);
#undef testXSLTFN

    Q_ASSERT(fn);
    fn->setOperands(args);
    fn->as<FunctionCall>()->setSignature(sign);

    return fn;
}

FunctionSignature::Ptr XSLT20CoreFunctions::retrieveFunctionSignature(const NamePool::Ptr &np, const QXmlName name)
{
    if(StandardNamespaces::fn != name.namespaceURI())
        return FunctionSignature::Ptr();

    FunctionSignature::Ptr s(functionSignatures().value(name));

    if(!s)
    {
        /* Alphabetic order. */
        if(name.localName() == StandardLocalNames::element_available)
        {
            s = addFunction(StandardLocalNames::element_available, 1, 1, CommonSequenceTypes::ExactlyOneBoolean);
            s->appendArgument(argument(np, "element-name"), CommonSequenceTypes::ExactlyOneString);
        }
        else if(name.localName() == StandardLocalNames::function_available)
        {
            s = addFunction(StandardLocalNames::function_available, 1, 2, CommonSequenceTypes::ExactlyOneBoolean);
            s->appendArgument(argument(np, "function_name"), CommonSequenceTypes::ExactlyOneString);
            s->appendArgument(argument(np, "arity"), CommonSequenceTypes::ExactlyOneInteger);
        }
        else if(name.localName() == StandardLocalNames::type_available)
        {
            s = addFunction(StandardLocalNames::type_available, 1, 1, CommonSequenceTypes::ExactlyOneBoolean);
            s->appendArgument(argument(np, "type_name"), CommonSequenceTypes::ExactlyOneString);
        }
        else if(name.localName() == StandardLocalNames::system_property)
        {
            s = addFunction(StandardLocalNames::system_property, 1, 1, CommonSequenceTypes::ExactlyOneString);
            s->appendArgument(argument(np, "property_name"), CommonSequenceTypes::ExactlyOneString);
        }
        else if(name.localName() == StandardLocalNames::generate_id)
        {
            s = addFunction(StandardLocalNames::generate_id, 0, 1, CommonSequenceTypes::ExactlyOneString,
                            Expression::UseContextItem);
            s->appendArgument(argument(np, "node"), CommonSequenceTypes::ZeroOrOneNode);
        }
        else if(name.localName() == StandardLocalNames::unparsed_text)
        {
            s = addFunction(StandardLocalNames::unparsed_text, 1, 2, CommonSequenceTypes::ZeroOrOneString,
                            Expression::DisableElimination);
            s->appendArgument(argument(np, "href"), CommonSequenceTypes::ZeroOrOneString);
            s->appendArgument(argument(np, "encoding"), CommonSequenceTypes::ExactlyOneString);
        }
        else if(name.localName() == StandardLocalNames::unparsed_text_available)
        {
            s = addFunction(StandardLocalNames::unparsed_text_available, 1, 2, CommonSequenceTypes::ExactlyOneBoolean,
                            Expression::DisableElimination);
            s->appendArgument(argument(np, "href"), CommonSequenceTypes::ZeroOrOneString);
            s->appendArgument(argument(np, "encoding"), CommonSequenceTypes::ZeroOrOneString);
        }
        else if(name.localName() == StandardLocalNames::current)
        {
            s = addFunction(StandardLocalNames::current, 0, 0, CommonSequenceTypes::ExactlyOneItem,
                            Expression::DisableElimination | Expression::RequiresCurrentItem);
        }
        else if(name.localName() == StandardLocalNames::document)
        {
            s = addFunction(StandardLocalNames::document, 1, 2, CommonSequenceTypes::OneOrMoreDocumentNodes,
                            Expression::DisableElimination);
            s->appendArgument(argument(np, "uri-sequence"), CommonSequenceTypes::ZeroOrMoreStrings);
            s->appendArgument(argument(np, "base-uri-node"), CommonSequenceTypes::ExactlyOneNode);
        }
        else if(name.localName() == StandardLocalNames::unparsed_entity_uri)
        {
            s = addFunction(StandardLocalNames::unparsed_entity_uri, 1, 1, CommonSequenceTypes::ExactlyOneAnyURI,
                            Expression::RequiresFocus | Expression::DisableElimination);
            s->appendArgument(argument(np, "entity-name"), CommonSequenceTypes::ExactlyOneString);
        }
        else if(name.localName() == StandardLocalNames::unparsed_entity_public_id)
        {
            s = addFunction(StandardLocalNames::unparsed_entity_public_id, 1, 1, CommonSequenceTypes::ExactlyOneString,
                            Expression::RequiresFocus | Expression::DisableElimination);
            s->appendArgument(argument(np, "entity-name"), CommonSequenceTypes::ExactlyOneString);
        }
    }

    return s;
}

QT_END_NAMESPACE
