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

#ifndef Patternist_AnyURI_H
#define Patternist_AnyURI_H

#include <QUrl>
#include <QtDebug>

#include "qatomicstring_p.h"
#include "qbuiltintypes_p.h"
#include "qpatternistlocale_p.h"
#include "qreportcontext_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short A value of type <tt>xs:anyURI</tt>.
     *
     * @author Frans Englich <fenglich@trolltech.com>
     * @ingroup Patternist_xdm
     */
    class AnyURI : public AtomicString
    {
    public:
        typedef QExplicitlySharedDataPointer<AnyURI> Ptr;

        /**
         * Creates an instance representing @p value.
         *
         * @note @p value must be a valid @c xs:anyURI. If it is of interest
         * to construct from a lexical representation, use fromLexical().
         */
        static AnyURI::Ptr fromValue(const QString &value);

        static AnyURI::Ptr fromValue(const QUrl &uri);

        /**
         * @short Treates @p value as a lexical representation of @c xs:anyURI
         * but returns the value instance as a QUrl.
         *
         * If @p value is not a valid lexical representation of @c xs:anyURI,
         * an error is issued via @p context.
         */
        template<const ReportContext::ErrorCode code, typename TReportContext>
        static inline QUrl toQUrl(const QString &value,
                                  const TReportContext &context,
                                  const SourceLocationReflection *const r)
        {
            /* QUrl doesn't flag ":/..." so we workaround it. */
            const QString simplified(value.simplified());
            const QUrl uri(simplified);

            if(uri.isValid() && (!simplified.startsWith(QLatin1Char(':')) || !uri.isRelative()))
                return uri;
            else
            {
                context->error(QtXmlPatterns::tr("%1 is not a valid value of type %2.").arg(formatURI(value), formatType(context->namePool(), BuiltinTypes::xsAnyURI)),
                               code, r);
                return QUrl();
            }
        }

        /**
         * @short Constructs a @c xs:anyURI value from the lexical representation @p value.
         *
         * If @p value is not a valid lexical representation of @c xs:anyURI,
         * an error is issued via @p context.
         */
        template<const ReportContext::ErrorCode code, typename TReportContext>
        static inline AnyURI::Ptr fromLexical(const QString &value,
                                              const TReportContext &context,
                                              const SourceLocationReflection *const r)
        {
            return AnyURI::Ptr(new AnyURI(toQUrl<code>(value, context, r).toString()));
        }

        /**
         * If @p value is not a valid lexical representation for @c xs:anyURI,
         * a ValidationError is returned.
         */
        static Item fromLexical(const QString &value);

        /**
         * Creates an AnyURI instance representing an absolute URI which
         * is created from resolving @p relative against @p base.
         *
         * This function must be compatible with the resolution semantics
         * specified for fn:resolve-uri. In fact, the implementation of fn:resolve-uri,
         * ResourceURIFN, relies on this function.
         *
         * @see <a href="http://www.faqs.org/rfcs/rfc3986.html">RFC 3986 - Uniform
         * Resource Identifier (URI): Generic Syntax</a>
         * @see <a href ="http://www.w3.org/TR/xpath-functions/#func-resolve-uri">XQuery 1.0
         * and XPath 2.0 Functions and Operators, 8.1 fn:resolve-uri</a>
         */
        static AnyURI::Ptr resolveURI(const QString &relative,
                                      const QString &base);

        virtual ItemType::Ptr type() const;

        /**
         * @short Returns this @c xs:anyURI value in a QUrl.
         */
        inline QUrl toQUrl() const
        {
            Q_ASSERT_X(QUrl(m_value).isValid(), Q_FUNC_INFO,
                       qPrintable(QString::fromLatin1("%1 is apparently not ok for QUrl.").arg(m_value)));
            return QUrl(m_value);
        }
    protected:
        friend class CommonValues;

        AnyURI(const QString &value);
    };

    /**
     * @short Formats @p uri, that's considered to be a URI, for display.
     */
    static inline QString formatURI(const NamePool::Ptr &np, const QXmlName::NamespaceCode &uri)
    {
        return formatURI(np->stringForNamespace(uri));
    }
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
