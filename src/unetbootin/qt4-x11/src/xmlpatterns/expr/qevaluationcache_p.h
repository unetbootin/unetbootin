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

#ifndef Patternist_EvaluationCache_H
#define Patternist_EvaluationCache_H

#include "qcachingiterator_p.h"
#include "qcommonsequencetypes_p.h"
#include "qnodebuilder_p.h"
#include "qoperandsiterator_p.h"
#include "qsinglecontainer_p.h"
#include "qvariabledeclaration_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short Evaluates to the same result as its operand, but ensures the
     * operand is evaluated once even if this Expression is evaluated several
     * times.
     *
     * EvaluationCache does this in a pipelined way, by delivering items from
     * its cache, which is stored in the DynamicContext. If the cache has less
     * items than what the caller requests, EvaluationCache continues to
     * deliver but this time from the source, which it also populates into the
     * cache.
     *
     * EvaluationCache is used as an optimization in order to avoid running
     * expensive code paths multiple times, but also is sometimes a necessity:
     * for instance, when objects must be unique, such as potentially in the
     * case of node identity.
     *
     * EvaluationCache is in particular used for variables, whose sole purpose
     * is to store it once(at least conceptually) and then use it in multiple
     * places.
     *
     * In some cases an EvaluationCache isn't necessary. For instance, when a
     * variable is only referenced once. In those cases EvaluationCache removes
     * itself as an optimization; implemented in compress().
     *
     * @author Frans Englich <fenglich@trolltech.com>
     * @ingroup Patternist_expressions
     */
    template<bool IsForGlobal>
    class EvaluationCache : public SingleContainer
    {
    public:
        EvaluationCache(const Expression::Ptr &operand,
                        const VariableDeclaration::Ptr &varDecl,
                        const VariableSlotID slot);

        virtual Item evaluateSingleton(const DynamicContext::Ptr &context) const;
        virtual Item::Iterator::Ptr evaluateSequence(const DynamicContext::Ptr &context) const;
        virtual Expression::Ptr compress(const StaticContext::Ptr &context);

        virtual SequenceType::Ptr staticType() const;

        /**
         * The first operand must be exactly one @c xs:string.
         */
        virtual SequenceType::List expectedOperandTypes() const;

        virtual ExpressionVisitorResult::Ptr accept(const ExpressionVisitor::Ptr &visitor) const;
        virtual Properties properties() const;
        virtual Expression::Ptr typeCheck(const StaticContext::Ptr &context,
                                          const SequenceType::Ptr &reqType);
        virtual const SourceLocationReflection *actualReflection() const;

        inline VariableSlotID slot() const
        {
            return m_varSlot;
        }

    private:
        static DynamicContext::Ptr topFocusContext(const DynamicContext::Ptr &context);
        const VariableDeclaration::Ptr  m_declaration;
        /**
         * This variable must not be called m_slot. If it so, a compiler bug on
         * HP-UX-aCC-64 is triggered in the constructor initializor. See the
         * preprocessor output.
         *
         * Note that this is the cache slot, and is disjoint to any variable's
         * regular slot.
         */
        const VariableSlotID            m_varSlot;
    };

#include "qevaluationcache.cpp"
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
