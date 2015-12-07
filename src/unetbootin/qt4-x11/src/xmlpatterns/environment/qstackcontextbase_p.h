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

#ifndef Patternist_StackContextBase_H
#define Patternist_StackContextBase_H

#include <QVector>

#include "qdaytimeduration_p.h"
#include "qdelegatingdynamiccontext_p.h"
#include "qexpression_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short Base class for all DynamicContext classes that needs to supply
     * variables. It has a new frame for local caches, position iterators,
     * expressions, range variables, template parameters but notably continues
     * to delegate global caches.
     *
     * @author Frans Englich <fenglich@trolltech.com>
     */
    template<typename TSuperClass>
    class StackContextBase : public TSuperClass
    {
    public:
        StackContextBase();
        /**
         * Construct a StackContextBase and passes @p prevContext to its super class. This
         * constructor is typically used when the super class is DelegatingDynamicContext.
         */
        StackContextBase(const DynamicContext::Ptr &prevContext);

        virtual void setRangeVariable(const VariableSlotID slotNumber,
                                      const Item &newValue);
        virtual Item rangeVariable(const VariableSlotID slotNumber) const;

        virtual void setExpressionVariable(const VariableSlotID slotNumber,
                                           const Expression::Ptr &newValue);
        virtual Expression::Ptr expressionVariable(const VariableSlotID slotNumber) const;

        virtual Item::Iterator::Ptr positionIterator(const VariableSlotID slot) const;
        virtual void setPositionIterator(const VariableSlotID slot,
                                         const Item::Iterator::Ptr &newValue);
        virtual ItemCacheCell &itemCacheCell(const VariableSlotID slot);
        virtual ItemSequenceCacheCell::Vector &itemSequenceCacheCells(const VariableSlotID slot);

        virtual DynamicContext::TemplateParameterHash &templateParameterStore();

    protected:
        /**
         * This function is protected, although it only is used in this class. I don't
         * know why it has to be, but it won't compile when private.
         */
        template<typename VectorType, typename UnitType>
        inline
        void setSlotVariable(const VariableSlotID slot,
                             const UnitType &newValue,
                             VectorType &container) const;

    private:
        Item::Vector                            m_rangeVariables;
        Expression::Vector                      m_expressionVariables;
        Item::Iterator::Vector                  m_positionIterators;
        ItemCacheCell::Vector                   m_itemCacheCells;
        ItemSequenceCacheCell::Vector           m_itemSequenceCacheCells;
        DynamicContext::TemplateParameterHash   m_templateParameterStore;
    };

    #include "qstackcontextbase.cpp"

    /**
     * @short A DynamicContext that creates a new scope for variables.
     *
     * This DynamicContext is used for recursive user function calls, for example.
     */
    typedef StackContextBase<DelegatingDynamicContext> StackContext;
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
