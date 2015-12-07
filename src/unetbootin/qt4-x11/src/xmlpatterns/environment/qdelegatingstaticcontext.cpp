/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtXMLPatterns module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
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

ItemType::Ptr DelegatingStaticContext::currentItemType() const
{
    return m_context->currentItemType();
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

VariableSlotID DelegatingStaticContext::currentRangeSlot() const
{
    return m_context->currentRangeSlot();
}

VariableSlotID DelegatingStaticContext::allocateRangeSlot()
{
    return m_context->allocateRangeSlot();
}

void DelegatingStaticContext::setCompatModeEnabled(const bool newVal)
{
    m_context->setCompatModeEnabled(newVal);
}

QT_END_NAMESPACE
