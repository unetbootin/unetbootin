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

#ifndef Patternist_BuiltinTypesFactory_H
#define Patternist_BuiltinTypesFactory_H

#include <QHash>
#include "qschematypefactory_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{

    /**
     * @short Factory for creating schema types for the types defined in XSL-T 2.0.
     *
     * Theses types are essentially the builtin primitive types, plus @c xs:integer,
     * and the types defined in the XPath Data Model.
     *
     * @ingroup Patternist_types
     * @see <a href="http://www.w3.org/TR/xpath-datamodel/#types-predefined">XQuery 1.0 and
     * XPath 2.0 Data Model, 2.6.2 Predefined Types</a>
     * @see <a href="http://www.w3.org/TR/xslt20/#built-in-types">XSL Transformations (XSLT)
     * Version 2.0, 3.13 Built-in Types</a>
     * @see <a href="http://www.w3.org/TR/xmlschema-2/#built-in-primitive-datatypes">XML Schema
     * Part 2: Datatypes Second Edition, 3.2 Primitive datatypes</a>
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class BasicTypesFactory : public SchemaTypeFactory
    {
    public:

        /**
         * Creates a primitive type for @p name. If @p name is not supported,
         * @c null is returned.
         * The intened supported types are the builtin primitive and derived types.
         * That is, the 19 W3C XML Schema types, and the additional 5 in the XPath Data MOdel.
         *
         * @note This does not handle user defined types, only builtin types.
         * @todo Update documentation, proportionally with progress.
         */
        virtual SchemaType::Ptr createSchemaType(const QXmlName ) const;

        virtual SchemaType::Hash types() const;

        /**
         * @returns the singleton instance of BasicTypesFactory.
         */
        static SchemaTypeFactory::Ptr self(const NamePool::Ptr &np);

    protected:
        /**
         * This constructor is protected. Use the static self() function
         * to retrieve a singleton instance.
         */
        BasicTypesFactory(const NamePool::Ptr &np);

    private:
        /**
         * A dictonary of builtin primitive and derived primitives.
         */
        SchemaType::Hash m_types;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
