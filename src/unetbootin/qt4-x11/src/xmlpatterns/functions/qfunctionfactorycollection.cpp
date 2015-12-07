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

#include "qbasictypesfactory_p.h"
#include "qconstructorfunctionsfactory_p.h"
#include "qfunctioncall_p.h"
#include "qxpath10corefunctions_p.h"
#include "qxpath20corefunctions_p.h"
#include "qxslt20corefunctions_p.h"

#include "qfunctionfactorycollection_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

Expression::Ptr FunctionFactoryCollection::createFunctionCall(const QXmlName name,
                                                              const Expression::List &arguments,
                                                              const StaticContext::Ptr &context,
                                                              const SourceLocationReflection *const r)
{
    const_iterator it;
    const_iterator e(constEnd());
    Expression::Ptr function;

    for(it = constBegin(); it != e; ++it)
    {
        function = (*it)->createFunctionCall(name, arguments, context, r);

        if(function)
            break;
    }

    return function;
}

bool FunctionFactoryCollection::isAvailable(const NamePool::Ptr &np, const QXmlName name, const xsInteger arity)
{
    const_iterator it;
    const_iterator e(constEnd());

    for(it = constBegin(); it != e; ++it)
        if((*it)->isAvailable(np, name, arity))
            return true;

    return false;
}

FunctionSignature::Hash FunctionFactoryCollection::functionSignatures() const
{
    /* We simply grab the function signatures for each library, and
     * put them all in one list. */

    const const_iterator e(constEnd());
    FunctionSignature::Hash result;

    for(const_iterator it(constBegin()); it != e; ++it)
    {
        const FunctionSignature::Hash::const_iterator e2((*it)->functionSignatures().constEnd());
        FunctionSignature::Hash::const_iterator sit((*it)->functionSignatures().constBegin());

        for(; sit != e2; ++sit)
            result.insert(sit.key(), sit.value());
    }

    return result;
}

FunctionSignature::Ptr FunctionFactoryCollection::retrieveFunctionSignature(const NamePool::Ptr &, const QXmlName name)
{
    return functionSignatures().value(name);
}

FunctionFactory::Ptr FunctionFactoryCollection::xpath10Factory()
{
    /* We don't use a global static for caching this, because AbstractFunctionFactory
     * stores state specific to the NamePool, when being used. */
    return  FunctionFactory::Ptr(new XPath10CoreFunctions());
}

FunctionFactory::Ptr FunctionFactoryCollection::xpath20Factory(const NamePool::Ptr &np)
{
    /* We don't use a global static for caching this, because AbstractFunctionFactory
     * stores state specific to the NamePool, when being used. */
    const FunctionFactoryCollection::Ptr fact(new FunctionFactoryCollection());
    fact->append(xpath10Factory());
    fact->append(FunctionFactory::Ptr(new XPath20CoreFunctions()));
    fact->append(FunctionFactory::Ptr(
                            new ConstructorFunctionsFactory(np, BasicTypesFactory::self(np))));
    return fact;
}

FunctionFactory::Ptr FunctionFactoryCollection::xslt20Factory(const NamePool::Ptr &np)
{
    const FunctionFactory::Ptr retval(xpath20Factory(np));
    static_cast<FunctionFactoryCollection *>(retval.data())->append(FunctionFactory::Ptr(new XSLT20CoreFunctions()));
    return retval;
}

QT_END_NAMESPACE
