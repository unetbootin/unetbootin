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

#ifndef Patternist_NamespaceResolver_H
#define Patternist_NamespaceResolver_H

template<typename A, typename B> class QHash;

#include <QSharedData>
#include <QXmlName>


QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{

    /**
     * @short Base class for namespace resolvers.
     *
     * @ingroup Patternist
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class NamespaceResolver : public QSharedData
    {
    public:
        enum Constants
        {
            NoBinding = -1
        };

        typedef QExplicitlySharedDataPointer<NamespaceResolver> Ptr;

        /**
         * A list of namespace bindings. The key is the prefix, and the value is
         * the namespace URI.
         */
        typedef QHash<QXmlName::PrefixCode, QXmlName::NamespaceCode> Bindings;

        NamespaceResolver();
        virtual ~NamespaceResolver();

        /**
         * Adds the mapping from @p prefix to @p namespaceURI to
         * this NamespaceResolver. If this NamespaceResolver already contains
         * a binding involving @p prefix, the old binding is replaced.
         */
        virtual void addBinding(const QXmlName nb) = 0;

        /**
         * Resolves the @p prefix to the corresponding namespace URI. If no binding
         * exists for @p prefix, NoBinding is returned.
         *
         * @returns the namespace corresponding to @p prefix.
         */
        virtual QXmlName::NamespaceCode lookupNamespaceURI(const QXmlName::PrefixCode prefix) const = 0;

        /**
         * @returns all bindings this NamespaceResolver handles.
         */
        virtual Bindings bindings() const = 0;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
