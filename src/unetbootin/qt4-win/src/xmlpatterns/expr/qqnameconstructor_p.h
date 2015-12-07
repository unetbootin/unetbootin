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

#ifndef Patternist_QNameConstructor_H
#define Patternist_QNameConstructor_H

#include "qsinglecontainer_p.h"
#include "qbuiltintypes_p.h"
#include "qpatternistlocale_p.h"
#include "qxpathhelper_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short Creates an @c xs:QName value from a lexical QName using
     * statically known namespace bindings.
     *
     * @author Frans Englich <fenglich@trolltech.com>
     * @ingroup Patternist_expressions
     */
    class QNameConstructor : public SingleContainer
    {
    public:

        QNameConstructor(const Expression::Ptr &source,
                         const NamespaceResolver::Ptr &nsResolver);

        virtual Item evaluateSingleton(const DynamicContext::Ptr &) const;

        virtual SequenceType::List expectedOperandTypes() const;

        virtual SequenceType::Ptr staticType() const;

        virtual ExpressionVisitorResult::Ptr accept(const ExpressionVisitor::Ptr &visitor) const;

        /**
         * Expands @p lexicalQName, which is a lexical representation of a QName such as "x:body", into
         * a QName using @p nsResolver to supply the namespace bindings.
         *
         * If @p lexicalQName is lexically invalid @p InvalidQName is raised via @p context, or if
         * no namespace binding does not exists for a prefix(if any) in @p lexicalQName, @p NoBinding
         * is raised via @p context.
         *
         * If @p asForAttribute is @c true, the name is considered to be for an
         * attribute in some way, and @p lexicalQName will not pick up the
         * default namespace if it doesn't have a prefix.
         *
         * @p nsResolver is parameterized meaning the function can be instantiated with either
         * DynamicContext or StaticContext.
         */
        template<typename TReportContext,
                 const ReportContext::ErrorCode InvalidQName,
                 const ReportContext::ErrorCode NoBinding>
        static
        QXmlName expandQName(const QString &lexicalQName,
                             const TReportContext &context,
                             const NamespaceResolver::Ptr &nsResolver,
                             const SourceLocationReflection *const r,
                             const bool asForAttribute = false);

        /**
         * Resolves the namespace prefix @p prefix to its namespace if it exists, or
         * raised ReportContext::XPST0081 otherwise.
         *
         * @returns the namespace URI corresponding to @p prefix
         */
        static QXmlName::NamespaceCode namespaceForPrefix(const QXmlName::PrefixCode prefix,
                                                       const StaticContext::Ptr &context,
                                                       const SourceLocationReflection *const r);

        virtual const SourceLocationReflection *actualReflection() const;

    private:
        const NamespaceResolver::Ptr m_nsResolver;
    };

    template<typename TReportContext,
             const ReportContext::ErrorCode InvalidQName,
             const ReportContext::ErrorCode NoBinding>
    QXmlName QNameConstructor::expandQName(const QString &lexicalQName,
                                        const TReportContext &context,
                                        const NamespaceResolver::Ptr &nsResolver,
                                        const SourceLocationReflection *const r,
                                        const bool asForAttribute)
    {
        Q_ASSERT(nsResolver);
        Q_ASSERT(context);

        if(XPathHelper::isQName(lexicalQName))
        {
            QString prefix;
            QString local;
            XPathHelper::splitQName(lexicalQName, prefix, local);
            const QXmlName::NamespaceCode nsCode = asForAttribute && prefix.isEmpty() ? QXmlName::NamespaceCode(StandardNamespaces::empty)
                                                                                   : (nsResolver->lookupNamespaceURI(context->namePool()->allocatePrefix(prefix)));

            if(nsCode == NamespaceResolver::NoBinding)
            {
                context->error(QtXmlPatterns::tr("No namespace binding exists for "
                                  "the prefix %1 in %2").arg(formatKeyword(prefix),
                                                             formatKeyword(lexicalQName)),
                               NoBinding,
                               r);
                return QXmlName(); /* Silence compiler warning. */
            }
            else
                return context->namePool()->allocateQName(context->namePool()->stringForNamespace(nsCode), local, prefix);
        }
        else
        {
            context->error(QtXmlPatterns::tr("%1 is an invalid %2")
                              .arg(formatData(lexicalQName))
                              .arg(formatType(context->namePool(), BuiltinTypes::xsQName)),
                           InvalidQName,
                           r);
            return QXmlName(); /* Silence compiler warning. */
        }
    }
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
