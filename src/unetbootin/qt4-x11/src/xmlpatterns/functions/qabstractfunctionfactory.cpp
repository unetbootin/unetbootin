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

#include "qpatternistlocale_p.h"

#include "qabstractfunctionfactory_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

Expression::Ptr AbstractFunctionFactory::createFunctionCall(const QXmlName name,
                                                            const Expression::List &args,
                                                            const StaticContext::Ptr &context,
                                                            const SourceLocationReflection *const r)
{
    const FunctionSignature::Ptr sign(retrieveFunctionSignature(context->namePool(), name));

    if(!sign) /* The function doesn't exist(at least not in this factory). */
        return Expression::Ptr();

    /* May throw. */
    verifyArity(sign, context, args.count(), r);

    /* Ok, the function does exist and the arity is correct. */
    return retrieveExpression(name, args, sign);
}

void AbstractFunctionFactory::verifyArity(const FunctionSignature::Ptr &s,
                                          const StaticContext::Ptr &context,
                                          const xsInteger arity,
                                          const SourceLocationReflection *const r) const
{
    /* Same code in both branches, but more specific error messages in order
     * to improve usability. */
    if(s->maximumArguments() != FunctionSignature::UnlimitedArity &&
       arity > s->maximumArguments())
    {
        context->error(QtXmlPatterns::tr("%1 takes at most %n argument(s). "
                                         "%2 is therefore invalid.", 0, s->maximumArguments())
                          .arg(formatFunction(context->namePool(), s))
                          .arg(arity),
                       ReportContext::XPST0017,
                       r);
        return;
    }

    if(arity < s->minimumArguments())
    {
        context->error(QtXmlPatterns::tr("%1 requires at least %n argument(s). "
                                         "%2 is therefore invalid.", 0, s->minimumArguments())
                          .arg(formatFunction(context->namePool(), s))
                          .arg(arity),
                       ReportContext::XPST0017,
                       r);
        return;
    }
}

FunctionSignature::Hash AbstractFunctionFactory::functionSignatures() const
{
    return m_signatures;
}

QT_END_NAMESPACE
