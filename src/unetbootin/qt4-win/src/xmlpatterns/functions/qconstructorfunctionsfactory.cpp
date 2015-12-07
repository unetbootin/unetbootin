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

#include "qatomictype_p.h"
#include "qbuiltintypes_p.h"
#include "qcastas_p.h"
#include "qcommonnamespaces_p.h"
#include "qcommonsequencetypes_p.h"
#include "qfunctionargument_p.h"
#include "qfunctioncall_p.h"
#include "qgenericsequencetype_p.h"
#include "qschematype_p.h"
#include "qschematypefactory_p.h"

#include "qconstructorfunctionsfactory_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

ConstructorFunctionsFactory::ConstructorFunctionsFactory(const NamePool::Ptr &np, const SchemaTypeFactory::Ptr &f) : m_typeFactory(f)
{
    Q_ASSERT(m_typeFactory);
    Q_ASSERT(np);
    SchemaType::Hash::const_iterator it(m_typeFactory->types().constBegin());
    const SchemaType::Hash::const_iterator end(m_typeFactory->types().constEnd());

    FunctionArgument::List args;
    const QXmlName argName(StandardNamespaces::empty, StandardLocalNames::sourceValue);

    args.append(FunctionArgument::Ptr(new FunctionArgument(argName,
                                                           CommonSequenceTypes::ZeroOrOneAtomicType)));

    while(it != end)
    {
        if(!BuiltinTypes::xsAnyAtomicType->wxsTypeMatches(*it) ||
           *BuiltinTypes::xsAnyAtomicType == *static_cast<const AtomicType *>((*it).data()) ||
           *BuiltinTypes::xsNOTATION == *static_cast<const AtomicType *>((*it).data()))
        {
            /* It's not a valid type for a constructor function -- skip it. */
            ++it;
            continue;
        }

        const QXmlName name((*it)->name(np));
        FunctionSignature::Ptr s(new FunctionSignature(name, 1, 1,
                                                       makeGenericSequenceType(AtomicType::Ptr(*it),
                                                                               Cardinality::zeroOrOne())));
        s->setArguments(args);
        m_signatures.insert(name, s);
        ++it;
    }
}

Expression::Ptr ConstructorFunctionsFactory::retrieveExpression(const QXmlName name,
                                                                const Expression::List &args,
                                                                const FunctionSignature::Ptr &sign) const
{
    Q_UNUSED(sign);

    /* This function is only called if the callsite is valid, so createSchemaType() will always
     * return an AtomicType. */
    const AtomicType::Ptr at(static_cast<AtomicType *>(m_typeFactory->createSchemaType(name).data()));

    return Expression::Ptr(new CastAs(args.first(),
                                      makeGenericSequenceType(at,
                                                              Cardinality::zeroOrOne())));
}

FunctionSignature::Ptr ConstructorFunctionsFactory::retrieveFunctionSignature(const NamePool::Ptr &np, const QXmlName name)
{
    Q_UNUSED(np);
    return functionSignatures().value(name);
}

QT_END_NAMESPACE
