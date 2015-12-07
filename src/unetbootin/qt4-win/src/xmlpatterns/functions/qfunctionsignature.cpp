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

#include "qxmlname.h"

#include "qfunctionsignature_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

FunctionSignature::FunctionSignature(const QXmlName nameP,
                                     const Arity minArgs,
                                     const Arity maxArgs,
                                     const SequenceType::Ptr &returnTypeP,
                                     const Expression::Properties props,
                                     const Expression::ID idP) : m_name(nameP),
                                                                 m_minArgs(minArgs),
                                                                 m_maxArgs(maxArgs),
                                                                 m_returnType(returnTypeP),
                                                                 m_arguments(),
                                                                 m_props(props),
                                                                 m_id(idP)
{
    Q_ASSERT(!nameP.isNull());
    Q_ASSERT(minArgs <= maxArgs || maxArgs == FunctionSignature::UnlimitedArity);
    Q_ASSERT(m_maxArgs >= -1);
    Q_ASSERT(returnTypeP);
}

void FunctionSignature::appendArgument(const QXmlName::LocalNameCode nameP,
                                       const SequenceType::Ptr &type)
{
    Q_ASSERT(type);

    m_arguments.append(FunctionArgument::Ptr(new FunctionArgument(QXmlName(StandardNamespaces::empty, nameP), type)));
}

QString FunctionSignature::displayName(const NamePool::Ptr &np) const
{
    QString result;
    result += np->displayName(m_name);
    result += QLatin1Char('(');

    FunctionArgument::List::const_iterator it(m_arguments.constBegin());
    const FunctionArgument::List::const_iterator end(m_arguments.constEnd());

    if(it != end)
    {
        while(true)
        {
            result += QLatin1Char('$');
            result += np->displayName((*it)->name());
            result += QLatin1String(" as ");
            result += (*it)->type()->displayName(np);

            ++it;
            if(it == end)
                break;

            result += QLatin1String(", ");
        }
    }

    if(m_maxArgs == FunctionSignature::UnlimitedArity)
        result += QLatin1String(", ...");

    result += QLatin1String(") as ");
    result += m_returnType->displayName(np);

    return result;
}

bool FunctionSignature::operator==(const FunctionSignature &other) const
{
    return name() == other.name() &&
           isArityValid(other.maximumArguments()) &&
           isArityValid(other.minimumArguments());
}

void FunctionSignature::setArguments(const FunctionArgument::List &args)
{
    m_arguments = args;
}

FunctionArgument::List FunctionSignature::arguments() const
{
    return m_arguments;
}

bool FunctionSignature::isArityValid(const xsInteger arity) const
{
    return arity >= m_minArgs && arity <= m_maxArgs;
}

QXmlName FunctionSignature::name() const
{
    return m_name;
}

FunctionSignature::Arity FunctionSignature::minimumArguments() const
{
    return m_minArgs;
}

FunctionSignature::Arity FunctionSignature::maximumArguments() const
{
    return m_maxArgs;
}

SequenceType::Ptr FunctionSignature::returnType() const
{
    return m_returnType;
}

Expression::Properties FunctionSignature::properties() const
{
    return m_props;
}

Expression::ID FunctionSignature::id() const
{
    return m_id;
}

QT_END_NAMESPACE
