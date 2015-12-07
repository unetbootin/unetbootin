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

#include "qcommonsequencetypes_p.h"
#include "qpatternistlocale_p.h"
#include "qqnamevalue_p.h"
#include "qatomicstring_p.h"

#include "qerrorfn_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

Item ErrorFN::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    QString msg;

    switch(m_operands.count())
    {
        case 0: /* No args. */
        {
            context->error(QtXmlPatterns::tr("%1 was called.").arg(formatFunction(context->namePool(), signature())),
                            ReportContext::FOER0000, this);
            return Item();
        }
        case 3:
        /* Fallthrough, we don't use the 'error object' param. */
        case 2:
            msg = m_operands.at(1)->evaluateSingleton(context).stringValue();
        /* Fall through. */
        case 1:
        {
            const QNameValue::Ptr qName(m_operands.first()->evaluateSingleton(context).as<QNameValue>());

            if(qName)
                context->error(msg, qName->qName(), this);
            else
                context->error(msg, ReportContext::FOER0000, this);

            return Item();
        }
        default:
        {
            Q_ASSERT_X(false, Q_FUNC_INFO,
                       "Invalid number of arguments passed to fn:error.");
            return Item();
        }
    }
}

FunctionSignature::Ptr ErrorFN::signature() const
{
    const FunctionSignature::Ptr e(FunctionCall::signature());

    if(m_operands.count() != 1)
        return e;

    FunctionSignature::Ptr nev(FunctionSignature::Ptr(new FunctionSignature(e->name(),
                                                      e->minimumArguments(),
                                                      e->maximumArguments(),
                                                      e->returnType(),
                                                      e->properties())));
    const FunctionArgument::List args(e->arguments());
    FunctionArgument::List nargs;
    const QXmlName argName(StandardNamespaces::empty, StandardLocalNames::error);
    nargs.append(FunctionArgument::Ptr(new FunctionArgument(argName, CommonSequenceTypes::ExactlyOneQName)));
    nargs.append(args[1]);
    nargs.append(args[2]);
    nev->setArguments(nargs);

    return nev;
}

QT_END_NAMESPACE
