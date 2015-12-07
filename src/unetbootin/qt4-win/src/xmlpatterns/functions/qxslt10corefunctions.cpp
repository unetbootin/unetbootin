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

#include "qcommonnamespaces_p.h"
#include "qcommonsequencetypes_p.h"
#include "qfunctionavailablefn_p.h"
#include "qsystempropertyfn_p.h"

#include "qxslt10corefunctions_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

Expression::Ptr XSLT10CoreFunctions::retrieveExpression(const QXmlName lname,
                                                        const Expression::List &args,
                                                        const FunctionSignature::Ptr &sign) const
{
    Q_ASSERT(sign);

    Expression::Ptr fn;
#define testXSLTFN(ln, cname) else if(lname.localName() == StandardLocalNames::ln) fn = Expression::Ptr(new cname())

    if(false) /* Dummy for the macro handling. Will be optimized away anyway. */
        return Expression::Ptr();
    /* Alphabetic order. */
    testXSLTFN(function_available,  FunctionAvailableFN);
    testXSLTFN(system_property,     SystemPropertyFN);
#undef testXSLTFN

    Q_ASSERT(fn);
    fn->setOperands(args);
    fn->as<FunctionCall>()->setSignature(sign);

    return fn;
}

FunctionSignature::Ptr XSLT10CoreFunctions::retrieveFunctionSignature(const NamePool::Ptr &np, const QXmlName name)
{
    if(StandardNamespaces::fn != name.namespaceURI())
        return FunctionSignature::Ptr();

    FunctionSignature::Ptr s(functionSignatures().value(name));

    if(!s)
    {
        /* Alphabetic order. */
        if(name.localName() == StandardLocalNames::function_available)
        {
            s = addFunction(StandardLocalNames::function_available, 1, 2, CommonSequenceTypes::ExactlyOneBoolean);
            s->appendArgument(argument(np, "function_name"), CommonSequenceTypes::ExactlyOneString);
            s->appendArgument(argument(np, "arity"), CommonSequenceTypes::ExactlyOneInteger);
        }
        else if(name.localName() == StandardLocalNames::system_property)
        {
            s = addFunction(StandardLocalNames::system_property, 1, 1, CommonSequenceTypes::ExactlyOneString);
            s->appendArgument(argument(np, "property_name"), CommonSequenceTypes::ExactlyOneString);
        }
    }

    return s;
}

QT_END_NAMESPACE
