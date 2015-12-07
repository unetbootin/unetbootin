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
