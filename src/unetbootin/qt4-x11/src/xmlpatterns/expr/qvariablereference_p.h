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

#ifndef Patternist_VariableReference_H
#define Patternist_VariableReference_H

#include "qemptycontainer_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

template<typename T> class QList;

namespace QPatternist
{
    /**
     * @short Baseclass for classes being references to variables.
     *
     * @author Frans Englich <fenglich@trolltech.com>
     * @ingroup Patternist_expressions
     */
    class VariableReference : public EmptyContainer
    {
    public:
        typedef QExplicitlySharedDataPointer<VariableReference> Ptr;
        typedef QList<VariableReference::Ptr> List;

        /**
         * Creates a VariableReference.
         *
         * @param slot must be a valid slot. That is, zero or larger.
         */
        VariableReference(const VariableSlotID slot);

        /**
         * @returns the slot that this reference communicates through.
         *
         * This is a slot in the DynamicContext. Which one, depends on the
         * type, which this VariableReference does not have information about.
         * For instance, it could DynamicContext::expressionVariable() or
         * DynamicContext::rangeVariable().
         */
        inline VariableSlotID slot() const;

        /**
         * @returns DisableElimination
         */
        virtual Properties properties() const;

    private:
        /**
         * The slot. Same as returned by slot().
         *
         *  This variable is not called m_slot, because that creates a weird
         *  compiler error on hpuxi-acc. See the preprocessor output. EvaluationCache::m_varSlot
         *  is a similar workaround.
         */
        const VariableSlotID m_varSlot;
    };

    inline VariableSlotID VariableReference::slot() const
    {
        return m_varSlot;
    }

}

QT_END_NAMESPACE

QT_END_HEADER

#endif
