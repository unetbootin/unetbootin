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

#include <QDateTime>

#include "qdaytimeduration_p.h"

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

QT_END_NAMESPACE
