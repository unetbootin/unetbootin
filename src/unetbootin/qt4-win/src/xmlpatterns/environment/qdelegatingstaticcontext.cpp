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

/* Patternist */
#include "qbasictypesfactory_p.h"
#include "qfunctionfactorycollection_p.h"
#include "qgenericnamespaceresolver_p.h"
#include "qcommonnamespaces_p.h"
#include "qgenericdynamiccontext_p.h"

#include "qstaticfocuscontext_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

DelegatingStaticContext::DelegatingStaticContext(const StaticContext::Ptr &context) : m_context(context)
{
    Q_ASSERT(context);
}

NamespaceResolver::Ptr DelegatingStaticContext::namespaceBindings() const
{
    return m_context->namespaceBindings();
}

FunctionFactory::Ptr DelegatingStaticContext::functionSignatures() const
{
    return m_context->functionSignatures();
}

DynamicContext::Ptr DelegatingStaticContext::dynamicContext() const
{
    return m_context->dynamicContext();
}

SchemaTypeFactory::Ptr DelegatingStaticContext::schemaDefinitions() const
{
    return m_context->schemaDefinitions();
}

QUrl DelegatingStaticContext::baseURI() const
{
    return m_context->baseURI();
}

void DelegatingStaticContext::setBaseURI(const QUrl &uri)
{
    m_context->setBaseURI(uri);
}

bool DelegatingStaticContext::compatModeEnabled() const
{
    return m_context->compatModeEnabled();
}

QUrl DelegatingStaticContext::defaultCollation() const
{
    return m_context->defaultCollation();
}

QAbstractMessageHandler * DelegatingStaticContext::messageHandler() const
{
    return m_context->messageHandler();
}

void DelegatingStaticContext::setDefaultCollation(const QUrl &uri)
{
    m_context->setDefaultCollation(uri);
}

void DelegatingStaticContext::setNamespaceBindings(const NamespaceResolver::Ptr &resolver)
{
    m_context->setNamespaceBindings(resolver);
}

StaticContext::BoundarySpacePolicy DelegatingStaticContext::boundarySpacePolicy() const
{
    return m_context->boundarySpacePolicy();
}

void DelegatingStaticContext::setBoundarySpacePolicy(const BoundarySpacePolicy policy)
{
    m_context->setBoundarySpacePolicy(policy);
}

StaticContext::ConstructionMode DelegatingStaticContext::constructionMode() const
{
    return m_context->constructionMode();
}

void DelegatingStaticContext::setConstructionMode(const ConstructionMode mode)
{
    m_context->setConstructionMode(mode);
}

StaticContext::OrderingMode DelegatingStaticContext::orderingMode() const
{
    return m_context->orderingMode();
}

void DelegatingStaticContext::setOrderingMode(const OrderingMode mode)
{
    m_context->setOrderingMode(mode);
}

StaticContext::OrderingEmptySequence DelegatingStaticContext::orderingEmptySequence() const
{
    return m_context->orderingEmptySequence();
}

void DelegatingStaticContext::setOrderingEmptySequence(const OrderingEmptySequence ordering)
{
    m_context->setOrderingEmptySequence(ordering);
}

QString DelegatingStaticContext::defaultFunctionNamespace() const
{
    return m_context->defaultFunctionNamespace();
}

void DelegatingStaticContext::setDefaultFunctionNamespace(const QString &ns)
{
    m_context->setDefaultFunctionNamespace(ns);
}

QString DelegatingStaticContext::defaultElementNamespace() const
{
    return m_context->defaultElementNamespace();
}

void DelegatingStaticContext::setDefaultElementNamespace(const QString &ns)
{
    m_context->setDefaultElementNamespace(ns);
}

StaticContext::InheritMode DelegatingStaticContext::inheritMode() const
{
    return m_context->inheritMode();
}

void DelegatingStaticContext::setInheritMode(const InheritMode mode)
{
    m_context->setInheritMode(mode);
}

StaticContext::PreserveMode DelegatingStaticContext::preserveMode() const
{
    return m_context->preserveMode();
}

void DelegatingStaticContext::setPreserveMode(const PreserveMode mode)
{
    m_context->setPreserveMode(mode);
}

ItemType::Ptr DelegatingStaticContext::contextItemType() const
{
    return m_context->contextItemType();
}

ExternalVariableLoader::Ptr DelegatingStaticContext::externalVariableLoader() const
{
    return m_context->externalVariableLoader();
}

StaticContext::Ptr DelegatingStaticContext::copy() const
{
    return StaticContext::Ptr(new DelegatingStaticContext(m_context->copy()));
}

ResourceLoader::Ptr DelegatingStaticContext::resourceLoader() const
{
    return m_context->resourceLoader();
}

NamePool::Ptr DelegatingStaticContext::namePool() const
{
    return m_context->namePool();
}

void DelegatingStaticContext::addLocation(const SourceLocationReflection *const reflection,
                                     const QSourceLocation &location)
{
    m_context->addLocation(reflection, location);
}

StaticContext::LocationHash DelegatingStaticContext::sourceLocations() const
{
    return m_context->sourceLocations();
}

QSourceLocation DelegatingStaticContext::locationFor(const SourceLocationReflection *const reflection) const
{
    return m_context->locationFor(reflection);
}

const QAbstractUriResolver *DelegatingStaticContext::uriResolver() const
{
    return m_context->uriResolver();
}

QT_END_NAMESPACE
