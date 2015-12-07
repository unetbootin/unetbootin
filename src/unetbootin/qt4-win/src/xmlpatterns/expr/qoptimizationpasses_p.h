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

#ifndef Patternist_OptimizationBlocks_H
#define Patternist_OptimizationBlocks_H

#include "qatomiccomparator_p.h"
#include "qexpression_p.h"
#include "qoptimizerframework_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short Contains a set of common OptimizerPass instances.
     *
     * @author Frans englich <fenglich@trolltech.com>
     * @ingroup Patternist_expressions
     */
    namespace OptimizationPasses
    {
        /**
         * A list of OptimizerPass instances that performs the
         * following rewrites:
         *
         * - <tt>count([expr]) ne 0</tt> into <tt>exists([expr])</tt>
         * - <tt>count([expr]) != 0</tt> into <tt>exists([expr])</tt>
         * - <tt>0 ne count([expr])</tt> into <tt>exists([expr])</tt>
         * - <tt>0 != count([expr])</tt> into <tt>exists([expr])</tt>
         * - <tt>count([expr]) eq 0</tt> into <tt>empty([expr])</tt>
         * - <tt>count([expr]) = 0</tt> into <tt>empty([expr])</tt>
         * - <tt>0 eq count([expr])</tt> into <tt>empty([expr])</tt>
         * - <tt>0 = count([expr])</tt> into <tt>empty([expr])</tt>
         * - <tt>count([expr]) ge 1</tt> into <tt>exists([expr])</tt>
         * - <tt>count([expr]) >= 1</tt> into <tt>exists([expr])</tt>
         */
        extern OptimizationPass::List comparisonPasses;

        /**
         * A list of OptimizerPass instances that performs the
         * following rewrites:
         *
         * - <tt>for $var in [expr] return $var</tt> into <tt>[expr]</tt>
         */
        extern OptimizationPass::List forPasses;

        /**
         * A list of OptimizerPass instances that performs the
         * following rewrites:
         *
         * - <tt>if([expr of type xs:boolean]) then true() else false()</tt>
         *   into <tt>[expr of type xs:boolean]</tt>
         */
        extern OptimizationPass::List ifThenPasses;

        /**
         * A list of OptimizerPass instances that performs the
         * following rewrites:
         *
         * - <tt>fn:not(fn:exists([expr]))</tt> into <tt>fn:empty([expr])</tt>
         * - <tt>fn:not(fn:empty([expr]))</tt> into <tt>fn:exists([expr])</tt>
         */
        extern OptimizationPass::List notFN;

        /**
         * Initializes the data members in the OptimizationPasses namespace.
         *
         * This class is not supposed to be instantiated, but to be used via its init()
         * function. In fact, this class cannot be instantiated.
         *
         * @author Frans englich <fenglich@trolltech.com>
         */
        class Coordinator
        {
        public:
            /**
             * Initializes the members in the OptimizationPasses namespace.
             */
            static void init();

        private:
            Q_DISABLE_COPY(Coordinator)
            inline Coordinator();
        };
    }
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
