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

#ifndef Patternist_GenericStaticContext_H
#define Patternist_GenericStaticContext_H

#include <QUrl>

#include "qstaticcontext_p.h"
#include "qfunctionfactory_p.h"
#include "qschematypefactory_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short Provides setters and getters for the properties defined in StaticContext.
     *
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class GenericStaticContext : public StaticContext
    {
    public:
        typedef QExplicitlySharedDataPointer<GenericStaticContext> Ptr;
        /**
         * Constructs a GenericStaticContext. The components are initialized as per
         * the recommended default values in XQuery 1.0. <tt>Default order for empty sequences</tt>,
         * orderingEmptySequence(), is initialized to Greatest.
         *
         * @see <a href="http://www.w3.org/TR/xquery/#id-xq-static-context-components">XQuery
         * 1.0: An XML Query Language, C.1 Static Context Components</a>
         * @param errorHandler the error handler. May be null.
         * @param np the NamePool. May not be null.
         * @param aBaseURI the base URI in the static context. Must be absolute
         * and valid.
         */
        GenericStaticContext(const NamePool::Ptr &np,
                             QAbstractMessageHandler *const errorHandler,
                             const QUrl &aBaseURI);

        virtual NamespaceResolver::Ptr namespaceBindings() const;
        virtual void setNamespaceBindings(const NamespaceResolver::Ptr &);

        virtual FunctionFactory::Ptr functionSignatures() const;
        virtual SchemaTypeFactory::Ptr schemaDefinitions() const;

        /**
         * Returns a DynamicContext used for evaluation at compile time.
         *
         * @bug The DynamicContext isn't stable. It should be cached privately.
         */
        virtual DynamicContext::Ptr dynamicContext() const;

        virtual QUrl baseURI() const;
        virtual void setBaseURI(const QUrl &uri);

        virtual bool compatModeEnabled() const;

        /**
         * @returns always the Unicode codepoint collation URI
         */
        virtual QUrl defaultCollation() const;

        virtual QAbstractMessageHandler * messageHandler() const;

        virtual void setDefaultCollation(const QUrl &uri);

        virtual BoundarySpacePolicy boundarySpacePolicy() const;
        virtual void setBoundarySpacePolicy(const BoundarySpacePolicy policy);

        virtual ConstructionMode constructionMode() const;
        virtual void setConstructionMode(const ConstructionMode mode);

        virtual OrderingMode orderingMode() const;
        virtual void setOrderingMode(const OrderingMode mode);
        virtual OrderingEmptySequence orderingEmptySequence() const;
        virtual void setOrderingEmptySequence(const OrderingEmptySequence ordering);

        virtual QString defaultFunctionNamespace() const;
        virtual void setDefaultFunctionNamespace(const QString &ns);

        virtual QString defaultElementNamespace() const;
        virtual void setDefaultElementNamespace(const QString &ns);

        virtual InheritMode inheritMode() const;
        virtual void setInheritMode(const InheritMode mode);

        virtual PreserveMode preserveMode() const;
        virtual void setPreserveMode(const PreserveMode mode);

        virtual ItemType::Ptr contextItemType() const;
        void setContextItemType(const ItemType::Ptr &type);

        virtual StaticContext::Ptr copy() const;

        virtual ResourceLoader::Ptr resourceLoader() const;
        void setResourceLoader(const ResourceLoader::Ptr &loader);

        virtual ExternalVariableLoader::Ptr externalVariableLoader() const;
        void setExternalVariableLoader(const ExternalVariableLoader::Ptr &loader);
        virtual NamePool::Ptr namePool() const;

        virtual void addLocation(const SourceLocationReflection *const reflection,
                                 const QSourceLocation &location);
        virtual QSourceLocation locationFor(const SourceLocationReflection *const reflection) const;

        virtual LocationHash sourceLocations() const;
        virtual QAbstractUriResolver *uriResolver() const;

    private:
        BoundarySpacePolicy         m_boundarySpacePolicy;
        ConstructionMode            m_constructionMode;
        QString                     m_defaultElementNamespace;
        QString                     m_defaultFunctionNamespace;
        OrderingEmptySequence       m_orderingEmptySequence;
        OrderingMode                m_orderingMode;
        QUrl                        m_defaultCollation;
        QUrl                        m_baseURI;
        QAbstractMessageHandler *   m_messageHandler;
        PreserveMode                m_preserveMode;
        InheritMode                 m_inheritMode;
        NamespaceResolver::Ptr      m_namespaceResolver;
        ExternalVariableLoader::Ptr m_externalVariableLoader;
        ResourceLoader::Ptr         m_resourceLoader;
        const NamePool::Ptr         m_namePool;
        ItemType::Ptr               m_contextItemType;
        LocationHash                m_locations;
        QAbstractUriResolver *      m_uriResolver;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
