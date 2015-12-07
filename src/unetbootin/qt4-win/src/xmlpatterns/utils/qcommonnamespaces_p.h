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

#ifndef Patternist_CommonNamespaces_H
#define Patternist_CommonNamespaces_H

#include <QLatin1String>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short Contains common, standardized XML namespaces.
     *
     * @author Frans Englich <fenglich@trolltech.com>
     */
    namespace CommonNamespaces
    {
        /**
         * Namespace for the special XML namespace. It is by definition
         * bound to the "xml" prefix, and should have no usage in
         * ordinary code.
         *
         * Specification: http://www.w3.org/TR/REC-xml-names/
         */
        const QLatin1String XML("http://www.w3.org/XML/1998/namespace");

        /**
         * The namespace for the xmlns prefix. The Namespaces in XML recommendation
         * explicitly states that xmlns should not have a namespace, but has since
         * been changed. See:
         *
         * http://www.w3.org/2000/xmlns/
         */
        const QLatin1String XMLNS("http://www.w3.org/2000/xmlns/");

        /**
         * The namespace for W3C XML Schema. This is used for the XML language it
         * is, as well as its built-in data types.
         *
         * Specification: http://www.w3.org/TR/xmlschema-2/
         * @see <a href="http://www.w3.org/TR/xmlschema-2/datatypes.html#namespaces">XML Schema
         * Part 2: Datatypes Second Edition, 3.1 Namespace considerations</a>
         */
        const QLatin1String WXS("http://www.w3.org/2001/XMLSchema");

        /**
         * The namespace for W3C XML Schema attributes used in schema instances.
         *
         * Specification: http://www.w3.org/TR/xmlschema-2/
         *
         * @see <a href="http://www.w3.org/TR/xmlschema-1/structures.html#Instance_Document_Constructions">XML
         * Schema Part 1: Structures Second Edition, 2.6 Schema-Related
         * Markup in Documents Being Validated</a>
         */
        const QLatin1String XSI("http://www.w3.org/2001/XMLSchema-instance");

        /**
         * The namespace for built-in XPath functions, as defined in for example
         * XQuery 1.0 and XPath 2.0 Functions and Operators and XSLT.
         *
         * Specification: http://www.w3.org/TR/xquery-operators/
         */
        const QLatin1String XFN("http://www.w3.org/2005/xpath-functions");

        /**
         * The namespace for XSL-T 1.0 and 2.0.
         *
         * @see <a href="http://www.w3.org/TR/xslt20/#xslt-namespace">XSL
         * Transformations (XSLT) Version 2.0, 3.1 XSLT Namespace</a>
         * @see <a href="http://www.w3.org/TR/xslt">XSL Transformations (XSLT) Version 1.0</a>
         */
        const QLatin1String XSLT("http://www.w3.org/1999/XSL/Transform");

        /**
         * The namespace for identifying errors in XPath.
         *
         * @see <a href="http://www.w3.org/TR/xpath20/#id-identifying-errors">XML Path Language (XPath)
         * 2.0, 2.3.2 Identifying and Reporting Errors</a>
         */
        const QLatin1String XPERR("http://www.w3.org/2005/xqt-errors");

        /**
         * The XPath 2.0 Unicode codepoint collation URI identifier. Collations
         * specifies how strings are compared and ordered.
         */
        const char *const UNICODE_COLLATION = "http://www.w3.org/2005/xpath-functions/collation/codepoint";

        /**
         * A namespace provided in XQuery 1.0, to easily declare local
         * variables and functions.
         */
        const QLatin1String XDT_LOCAL("http://www.w3.org/2005/xquery-local-functions");
    }
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
