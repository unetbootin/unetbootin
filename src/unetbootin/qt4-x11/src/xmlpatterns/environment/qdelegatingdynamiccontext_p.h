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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef Patternist_DelegatingDynamicContext_H
#define Patternist_DelegatingDynamicContext_H

#include "qdynamiccontext_p.h"
#include "qexpression_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short Base class for dynamic contexts that are
     * created from an existing one.
     *
     * In some cases multiple DynamicContext instances must be used in
     * order to maintain somekind of scope. This class delegates
     * the DynamicContext interface onto another DynamicContext instance,
     * allowing the sub-class to only implement what it needs to.
     *
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class DelegatingDynamicContext : public DynamicContext
    {
    public:
        virtual xsInteger contextPosition() const;
        virtual Item contextItem() const;
        virtual xsInteger contextSize();

        virtual ItemCacheCell &itemCacheCell(const VariableSlotID slot);
        virtual ItemSequenceCacheCell::Vector &itemSequenceCacheCells(const VariableSlotID slot);

        virtual void setRangeVariable(const VariableSlotID slotNumber,
                                      const Item &newValue);
        virtual Item rangeVariable(const VariableSlotID slotNumber) const;

        virtual void setExpressionVariable(const VariableSlotID slotNumber,
                                           const Expression::Ptr &newValue);
        virtual Expression::Ptr expressionVariable(const VariableSlotID slotNumber) const;

        virtual void setFocusIterator(const Item::Iterator::Ptr &it);
        virtual Item::Iterator::Ptr focusIterator() const;

        virtual Item::Iterator::Ptr positionIterator(const VariableSlotID slot) const;
        virtual void setPositionIterator(const VariableSlotID slot,
                                         const Item::Iterator::Ptr &newValue);

        virtual QAbstractMessageHandler * messageHandler() const;
        virtual QExplicitlySharedDataPointer<DayTimeDuration> implicitTimezone() const;
        virtual QDateTime currentDateTime() const;
        virtual QAbstractXmlReceiver *outputReceiver() const;
        virtual NodeBuilder::Ptr nodeBuilder(const QUrl &baseURI) const;
        virtual ResourceLoader::Ptr resourceLoader() const;
        virtual ExternalVariableLoader::Ptr externalVariableLoader() const;
        virtual NamePool::Ptr namePool() const;
        virtual QSourceLocation locationFor(const SourceLocationReflection *const reflection) const;
        virtual void addNodeModel(const QAbstractXmlNodeModel::Ptr &nm);
        virtual const QAbstractUriResolver *uriResolver() const;
        virtual ItemCacheCell &globalItemCacheCell(const VariableSlotID slot);
        virtual ItemSequenceCacheCell::Vector &globalItemSequenceCacheCells(const VariableSlotID slot);
        virtual Item currentItem() const;
        virtual TemplateParameterHash &templateParameterStore();

        virtual DynamicContext::Ptr previousContext() const;
        virtual QExplicitlySharedDataPointer<TemplateMode> currentTemplateMode() const;

    protected:
        DelegatingDynamicContext(const DynamicContext::Ptr &prevContext);

        const DynamicContext::Ptr m_prevContext;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
