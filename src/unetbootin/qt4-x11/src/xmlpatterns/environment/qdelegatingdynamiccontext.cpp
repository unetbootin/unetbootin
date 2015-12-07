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

#include <QDateTime>

#include "qdaytimeduration_p.h"
#include "qtemplatemode_p.h"

#include "qdelegatingdynamiccontext_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

DelegatingDynamicContext::DelegatingDynamicContext(const DynamicContext::Ptr &prevContext)
                                                  : m_prevContext(prevContext)
{
    Q_ASSERT(m_prevContext);
}

ItemCacheCell &DelegatingDynamicContext::itemCacheCell(const VariableSlotID slot)
{
    return m_prevContext->itemCacheCell(slot);
}

ItemSequenceCacheCell::Vector &DelegatingDynamicContext::itemSequenceCacheCells(const VariableSlotID slot)
{
    return m_prevContext->itemSequenceCacheCells(slot);
}

xsInteger DelegatingDynamicContext::contextPosition() const
{
    return m_prevContext->contextPosition();
}

Item DelegatingDynamicContext::contextItem() const
{
    return m_prevContext->contextItem();
}

xsInteger DelegatingDynamicContext::contextSize()
{
    return m_prevContext->contextSize();
}

void DelegatingDynamicContext::setFocusIterator(const Item::Iterator::Ptr &it)
{
    m_prevContext->setFocusIterator(it);
}

Item::Iterator::Ptr DelegatingDynamicContext::positionIterator(const VariableSlotID slot) const
{
    return m_prevContext->positionIterator(slot);
}

void DelegatingDynamicContext::setPositionIterator(const VariableSlotID slot,
                                                   const Item::Iterator::Ptr &newValue)
{
    m_prevContext->setPositionIterator(slot, newValue);
}

void DelegatingDynamicContext::setRangeVariable(const VariableSlotID slotNumber,
                                               const Item &newValue)
{
    m_prevContext->setRangeVariable(slotNumber, newValue);
}

Item::Iterator::Ptr DelegatingDynamicContext::focusIterator() const
{
    return m_prevContext->focusIterator();
}

Item DelegatingDynamicContext::rangeVariable(const VariableSlotID slotNumber) const
{
    return m_prevContext->rangeVariable(slotNumber);
}

void DelegatingDynamicContext::setExpressionVariable(const VariableSlotID slotNumber,
                                                     const Expression::Ptr &newValue)
{
    m_prevContext->setExpressionVariable(slotNumber, newValue);
}

Expression::Ptr DelegatingDynamicContext::expressionVariable(const VariableSlotID slotNumber) const
{
    return m_prevContext->expressionVariable(slotNumber);
}

QAbstractMessageHandler * DelegatingDynamicContext::messageHandler() const
{
    return m_prevContext->messageHandler();
}

QExplicitlySharedDataPointer<DayTimeDuration> DelegatingDynamicContext::implicitTimezone() const
{
    return m_prevContext->implicitTimezone();
}

QDateTime DelegatingDynamicContext::currentDateTime() const
{
    return m_prevContext->currentDateTime();
}

QAbstractXmlReceiver *DelegatingDynamicContext::outputReceiver() const
{
    return m_prevContext->outputReceiver();
}

NodeBuilder::Ptr DelegatingDynamicContext::nodeBuilder(const QUrl &baseURI) const
{
    return m_prevContext->nodeBuilder(baseURI);
}

ResourceLoader::Ptr DelegatingDynamicContext::resourceLoader() const
{
    return m_prevContext->resourceLoader();
}

ExternalVariableLoader::Ptr DelegatingDynamicContext::externalVariableLoader() const
{
    return m_prevContext->externalVariableLoader();
}

NamePool::Ptr DelegatingDynamicContext::namePool() const
{
    return m_prevContext->namePool();
}

QSourceLocation DelegatingDynamicContext::locationFor(const SourceLocationReflection *const reflection) const
{
    return m_prevContext->locationFor(reflection);
}

void DelegatingDynamicContext::addNodeModel(const QAbstractXmlNodeModel::Ptr &nm)
{
    m_prevContext->addNodeModel(nm);
}

const QAbstractUriResolver *DelegatingDynamicContext::uriResolver() const
{
    return m_prevContext->uriResolver();
}

ItemCacheCell &DelegatingDynamicContext::globalItemCacheCell(const VariableSlotID slot)
{
    return m_prevContext->globalItemCacheCell(slot);
}

ItemSequenceCacheCell::Vector &DelegatingDynamicContext::globalItemSequenceCacheCells(const VariableSlotID slot)
{
    return m_prevContext->globalItemSequenceCacheCells(slot);
}

Item DelegatingDynamicContext::currentItem() const
{
    return m_prevContext->currentItem();
}

DynamicContext::TemplateParameterHash &DelegatingDynamicContext::templateParameterStore()
{
    return m_prevContext->templateParameterStore();
}

DynamicContext::Ptr DelegatingDynamicContext::previousContext() const
{
    return m_prevContext;
}

QExplicitlySharedDataPointer<TemplateMode> DelegatingDynamicContext::currentTemplateMode() const
{
    return m_prevContext->currentTemplateMode();
}

QT_END_NAMESPACE
