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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef Patternist_AbstractFunctionFactory_H
#define Patternist_AbstractFunctionFactory_H

#include "qcommonnamespaces_p.h"
#include "qfunctionfactory_p.h"
#include "qfunctionsignature_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{

    /**
     * @short Supplies convenience code for the function factories.
     *
     * @ingroup Patternist_functions
     * @see XPath10CoreFunctions
     * @see XPath20CoreFunctions
     * @see XSLT10CoreFunctions
     * @author Vincent Ricard <magic@magicninja.org>
     */
    class AbstractFunctionFactory : public FunctionFactory
    {
    public:
        virtual Expression::Ptr createFunctionCall(const QXmlName name,
                                                   const Expression::List &arguments,
                                                   const StaticContext::Ptr &context,
                                                   const SourceLocationReflection *const r);

        virtual FunctionSignature::Hash functionSignatures() const;

    protected:
        /**
         * This function is responsible for creating the actual Expression, corresponding
         * to @p localName and the function signature @p sign. It is called by
         * createFunctionCall(), once it have been determined the function actually
         * exists and have the correct arity.
         *
         * This function will only be called for names in the @c fn namespace.
         */
        virtual Expression::Ptr retrieveExpression(const QXmlName name,
                                                   const Expression::List &args,
                                                   const FunctionSignature::Ptr &sign) const = 0;

        inline
        FunctionSignature::Ptr addFunction(const QXmlName::LocalNameCode localName,
                                           const FunctionSignature::Arity minArgs,
                                           const FunctionSignature::Arity maxArgs,
                                           const SequenceType::Ptr &returnType,
                                           const Expression::Properties props)
        {
            return addFunction(localName,
                               minArgs,
                               maxArgs,
                               returnType,
                               Expression::IDIgnorableExpression,
                               props);
        }

        FunctionSignature::Ptr addFunction(const QXmlName::LocalNameCode &localName,
                                           const FunctionSignature::Arity minArgs,
                                           const FunctionSignature::Arity maxArgs,
                                           const SequenceType::Ptr &returnType,
                                           const Expression::ID id = Expression::IDIgnorableExpression,
                                           const Expression::Properties props = Expression::Properties())
        {
            const QXmlName name(StandardNamespaces::fn, localName);

            const FunctionSignature::Ptr s(new FunctionSignature(name, minArgs, maxArgs,
                                                                 returnType, props, id));

            m_signatures.insert(name, s);
            return s;
        }

        static inline QXmlName::LocalNameCode argument(const NamePool::Ptr &np, const char *const name)
        {
            return np->allocateLocalName(QLatin1String(name));
        }

        FunctionSignature::Hash m_signatures;

    private:
        /**
         * @short Determines whether @p arity is a valid number of
         * arguments for the function with signature @p sign.
         *
         * If it is not, a static error with error code ReportContext::XPST0017
         * is issued via @p context.
         */
        void verifyArity(const FunctionSignature::Ptr &sign,
                         const StaticContext::Ptr &context,
                         const xsInteger arity,
                         const SourceLocationReflection *const r) const;

    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
