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
     * times during a query run.
     *
     * EvaluationCache does this in a pipelined way, by delivering items from
     * its cache, which is stored in the DynamicContext. If the cache has less
     * items than what the caller requests, EvaluationCache continues to
     * deliver but this time from the source, which it also populates into the
     * cache.
     *
     * EvaluationCache is used as an optimization in order to avoid running expensive code
     * paths multiple times, but also is sometimes a necessity: for instance,
     * when objects must be unique, such as potentially in the case of node identity.
     *
     * EvaluationCache is in particular used for variables, whose sole purpose
     * is to store it once(at least conceptually) and then use it in multiple
     * places.
     *
     * In some cases an EvaluationCache isn't necessary. For instance, when a
     * variable is only referenced once. In those cases EvaluationCache removes
     * itself as an optimization; implemented in EvaluationCache::compress().
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
        const VariableDeclaration::Ptr  m_declaration;
        /**
         * This variable must not be called m_slot. If it so,
         * a compiler bug on HP-UX-aCC-64 is triggered in the constructor
         * initializor. See the preprocessor output.
         */
        const VariableSlotID            m_varSlot;
    };

#include "qevaluationcache.cpp"
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
