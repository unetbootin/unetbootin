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

#ifndef Patternist_XPathHelper_H
#define Patternist_XPathHelper_H

#include "qcommonnamespaces_p.h"
#include "qitem_p.h"
#include "qpatternistlocale_p.h"
#include "qreportcontext_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short Contains helper and utility functions.
     *
     * The common denominator of its functions is that they do not fit in
     * well elsewhere, such as in a particular class. It is preferred if XPathHelper
     * goes away, and that functions are in more specific classes.
     *
     * @ingroup Patternist
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class XPathHelper
    {
    public:
        /**
         * Determines whether @p qName is a valid QName. For example, "body" and "xhtml:body"
         * is, but "xhtml::body" or "x:body "(note the whitespace) is not.
         */
        static bool isQName(const QString &qName);

        /**
         * @short Splits @p qName into @p ncName and @p prefix.
         *
         * Here's an example:
         * @include Example-XPathHelper-splitQName.cpp
         *
         * @note @p qName must be a valid QName, and that is not checked.
         */
        static void splitQName(const QString &qName, QString &prefix, QString &ncName);

        /**
         * Determines whether @p ns is a reserved namespace.
         *
         * @see <a href="http://www.w3.org/TR/xslt20/#reserved-namespaces">XSL Transformations
         * (XSLT) Version 2.0, 3.2 Reserved Namespaces</a>
         * @see <a href="http://www.w3.org/TR/xquery/#FunctionDeclns">XQuery 1.0: An XML
         * Query Language, 4.15 Function Declaration</a>
         * @returns @c true if @p ns is a reserved namespace, otherwise @c false.
         */
        static bool isReservedNamespace(const QXmlName::NamespaceCode ns);

        /**
         * Determines whether @p collation is a supported string collation. If it is
         * not, error code @p code is raised via @p context.
         */
        template<const ReportContext::ErrorCode code, typename TReportContext>
        static inline void checkCollationSupport(const QString &collation,
                                                 const TReportContext &context,
                                                 const SourceLocationReflection *const r)
        {
            Q_ASSERT(context);
            Q_ASSERT(r);

            if(collation != QLatin1String(CommonNamespaces::UNICODE_COLLATION))
            {
                context->error(QtXmlPatterns::tr("Only the Unicode Codepoint "
                                  "Collation is supported(%1). %2 is unsupported.")
                                  .arg(formatURI(QLatin1String(CommonNamespaces::UNICODE_COLLATION)))
                                  .arg(formatURI(collation)),
                               code, r);
            }
        }

        static QPatternist::ItemTypePtr typeFromKind(const QXmlNodeModelIndex::NodeKind nodeKind);

        /**
         * @short Determines whether @p consists only of whitespace. Characters
         * considered whitespace are the ones for which QChar::isSpace() returns @c true for.
         *
         * For the empty string, @c true is returned.
         *
         * @returns @c true if @p string consists only of whitespace, otherwise @c false.
         */
        static inline bool isWhitespaceOnly(const QString &string)
        {
            const int len = string.length();

            for(int i = 0; i < len; ++i)
            {
                if(!string.at(i).isSpace()) // TODO and this is wrong, see sdk/TODO
                    return false;
            }

            return true;
        }

    private:
        /**
         * @short This default constructor has no definition, in order to avoid
         * instantiation, since it makes no sense to instantiate this class.
         */
        inline XPathHelper();

        Q_DISABLE_COPY(XPathHelper)
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
