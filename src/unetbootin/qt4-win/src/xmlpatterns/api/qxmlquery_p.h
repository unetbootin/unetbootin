/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
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

#ifndef QXMLQUERY_P_H
#define QXMLQUERY_P_H

#include <QAbstractMessageHandler>
#include <QAbstractUriResolver>
#include <QCoreApplication>
#include <QPointer>
#include <QSourceLocation>
#include <QUrl>
#include <QVariant>
#include <QXmlName>
#include <QXmlNamePool>

#include "qacceltreebuilder_p.h"
#include "qacceltreeresourceloader_p.h"
#include "qcoloringmessagehandler_p.h"
#include "qcommonsequencetypes_p.h"
#include "qexpressionfactory_p.h"
#include "qfocus_p.h"
#include "qgenericdynamiccontext_p.h"
#include "qgenericstaticcontext_p.h"
#include "qnamepool_p.h"
#include "qqobjectnodemodel_p.h"
#include "qstaticfocuscontext_p.h"
#include "quriloader_p.h"
#include "qvariableloader_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

/*!
   \class QReferenceCountedValue
   \internal
   \since 4.4
   \brief A template class that reference counts a value.

   This class is useful when an instance needs to have ownership semantics
   as if it was value based. A typical examples is a QObject pointer, which
   doesn't have a single owner.

   This is achieved through storing a copy of the object as
   a member inside QReferenceCountedValue, which never is copied. It will
   stay in scope until the last reference to the QReferenceCountedValue instance
   is removed, and subsequently QReferenceCountedValue is deleted and hence also
   the contained value. One should use QReferenceCountedValue by passing around
   copies of Ptr, which is a typedef for the QExplicitlySharedDataPointer
   smart pointer.
*/
template<typename T>
class QReferenceCountedValue : public QSharedData
{
public:
    typedef QExplicitlySharedDataPointer<QReferenceCountedValue<T> > Ptr;

    inline QReferenceCountedValue(const T &v) : value(v)
    {
    }

    T value;
private:
    /*!
      Disabled, no implementation provided.
     */
    inline QReferenceCountedValue();
    Q_DISABLE_COPY(QReferenceCountedValue);
};

class QXmlQueryPrivate
{
public:

    inline QXmlQueryPrivate() : updateVariableValues(true)
                              , messageHandler(0)
                              , uriResolver(0)
                              , m_networkManager(0)
    {
    }

    bool isValid()
    {
        return expression();
    }

    inline void recompileRequired()
    {
        updateVariableValues = true;
        m_expr.reset();
    }

    inline QPatternist::VariableLoader::Ptr variableLoader()
    {
        if(!m_variableLoader || updateVariableValues)
        {
            m_variableLoader = QPatternist::VariableLoader::Ptr(new QPatternist::VariableLoader(variableBindings, namePool.d, deviceBindings));
            updateVariableValues = false;
        }

        return m_variableLoader;
    }

    inline QPatternist::GenericStaticContext::Ptr staticContext()
    {
        if(m_staticContext && !updateVariableValues)
        {
            if(m_expr)
                return m_staticContext;

            /* Else, re-create the staticContext. */
        }

        if(!messageHandler)
            messageHandler = new QPatternist::ColoringMessageHandler(ownerObject());

        const QPatternist::GenericStaticContext::Ptr genericStaticContext(new QPatternist::GenericStaticContext(namePool.d, messageHandler, queryURI));
        const QPatternist::ResourceLoader::Ptr resourceLoader(new QPatternist::AccelTreeResourceLoader(namePool.d,
                                                                                                       networkManager(),
                                                                                                       QPatternist::ReportContext::Ptr(genericStaticContext)));
        genericStaticContext->setResourceLoader(resourceLoader);

        genericStaticContext->setExternalVariableLoader(variableLoader());

        m_staticContext = genericStaticContext;

        if(!contextItem.isNull())
            m_staticContext = QPatternist::StaticContext::Ptr(new QPatternist::StaticFocusContext(QPatternist::AtomicValue::qtToXDMType(contextItem), m_staticContext));

        return m_staticContext;
    }

    inline QPatternist::DynamicContext::Ptr dynamicContext(QAbstractXmlReceiver *const callback = 0)
    {
        const QPatternist::StaticContext::Ptr statContext(staticContext());
        Q_ASSERT(statContext);

        QPatternist::GenericDynamicContext::Ptr dynContext(new QPatternist::GenericDynamicContext(namePool.d, statContext->messageHandler(),
                                                                                                  statContext->sourceLocations()));

        QPatternist::AutoPtr<QPatternist::NodeBuilder> nodeBuilder(new QPatternist::AccelTreeBuilder<false>(QUrl(), QUrl(), namePool.d,
                                                                                                            QPatternist::ReportContext::Ptr(dynContext)));
        dynContext->setNodeBuilder(nodeBuilder);

        dynContext->setResourceLoader(statContext->resourceLoader());
        dynContext->setExternalVariableLoader(statContext->externalVariableLoader());
        dynContext->setUriResolver(uriResolver);

        if(callback)
            dynContext->setOutputReceiver(callback);

        if(contextItem.isNull())
            return dynContext;
        else
        {
            QPatternist::DynamicContext::Ptr focus(new QPatternist::Focus(dynContext));
            QPatternist::Item::Iterator::Ptr it(makeSingletonIterator(QPatternist::Item::fromPublic(contextItem)));
            it->next();
            focus->setFocusIterator(it);
            return focus;
        }
    }

    static inline QUrl normalizeQueryURI(const QUrl &uri)
    {
        Q_ASSERT_X(uri.isEmpty() || uri.isValid(), Q_FUNC_INFO,
                   "The URI passed to QXmlQuery::setQuery() must be valid or empty.");
        if(uri.isEmpty())
            return QUrl::fromLocalFile(QCoreApplication::applicationFilePath());
        else if(uri.isRelative())
            return QUrl::fromLocalFile(QCoreApplication::applicationFilePath()).resolved(uri);
        else
            return uri;
    }

    void setRequiredType(const QPatternist::SequenceType::Ptr &seqType)
    {
        Q_ASSERT(seqType);
        if(!m_requiredType || m_requiredType->is(seqType))
            return;

        m_requiredType = seqType;
        m_staticContext.reset();
    }

    QPatternist::SequenceType::Ptr requiredType()
    {
        if(m_requiredType)
            return m_requiredType;
        else
        {
            m_requiredType = QPatternist::CommonSequenceTypes::ZeroOrMoreItems;
            return m_requiredType;
        }
    }

    QPatternist::Expression::Ptr expression(QIODevice *const queryDevice = 0)
    {
        if(m_expr && !queryDevice)
            return m_expr;

        /* If we need to update, but we don't have any source code, we can
         * never create an Expression. */
        if(!queryDevice)
            return QPatternist::Expression::Ptr();

        try
        {
            /* The static context has source locations, and they need to be
             * updated to the new query. */
            m_staticContext.reset();

            if(!m_expressionFactory)
                m_expressionFactory = QPatternist::ExpressionFactory::Ptr(new QPatternist::ExpressionFactory());

            m_expr = m_expressionFactory->createExpression(queryDevice, staticContext(),
                                                           QPatternist::ExpressionFactory::XQuery10,
                                                           requiredType(),
                                                           queryURI);
        }
        catch(const QPatternist::Exception)
        {
            m_expr.reset();

            /* We don't call m_staticContext.reset() because it shouldn't be
             * necessary, since m_staticContext is changed when the expression
             * is changed. */
        }

        return m_expr;
    }

    static bool isSameType(const QXmlItem &i1,
                           const QXmlItem &i2)
    {
        if(i1.isNode())
        {
            Q_ASSERT(false);
            return false;
        }
        else if(i2.isAtomicValue())
            return i1.toAtomicValue().type() == i2.toAtomicValue().type();
        else
        {
            /* One is an atomic, the other is a node or they are null. */
            return false;
        }
    }

    bool                                        updateVariableValues;
    QPatternist::VariableLoader::BindingHash    variableBindings;
    QHash<QXmlName, QIODevice *>                deviceBindings;
    QXmlNamePool                                namePool;
    QPointer<QAbstractMessageHandler>           messageHandler;
    /**
     * Must be absolute and valid.
     */
    QUrl                                        queryURI;
    const QAbstractUriResolver *                uriResolver;
    QXmlItem                                    contextItem;

    inline QNetworkAccessManager *networkManager()
    {
        delete m_networkManager;
        m_networkManager = new QPatternist::URILoader(ownerObject(), deviceBindings, namePool.d);
        return m_networkManager;
    }

    inline void setExpressionFactory(const QPatternist::ExpressionFactory::Ptr &expr)
    {
        m_expressionFactory = expr;
    }

private:

    inline QObject *ownerObject()
    {
        if(!m_owner)
            m_owner = new QReferenceCountedValue<QObject *>(new QObject());

        return m_owner->value;
    }

    QPatternist::ExpressionFactory::Ptr         m_expressionFactory;
    QPatternist::StaticContext::Ptr             m_staticContext;
    QPatternist::VariableLoader::Ptr            m_variableLoader;
    /**
     * This is the AST for the query.
     */
    QPatternist::Expression::Ptr                m_expr;
    QReferenceCountedValue<QObject *>::Ptr      m_owner;
    QPointer<QNetworkAccessManager>             m_networkManager;
    QPatternist::SequenceType::Ptr              m_requiredType;
};

//Q_DECLARE_OPERATORS_FOR_FLAGS(QXmlQueryPrivate::ComponentsForUpdate)

QT_END_NAMESPACE

QT_END_HEADER

#endif
