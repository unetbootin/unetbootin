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

#ifndef Patternist_DynamicContext_H
#define Patternist_DynamicContext_H

#include "qautoptr_p.h"
#include "qcachecells_p.h"
#include "qexternalvariableloader_p.h"
#include "qitem_p.h"
#include "qnamepool_p.h"
#include "qnodebuilder_p.h"
#include "qprimitives_p.h"
#include "qreportcontext_p.h"
#include "qresourceloader_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QDateTime;
template<typename T> class QVector;

namespace QPatternist
{
    class DayTimeDuration;
    class Expression;

    /**
     * @short Carries information and facilities used at runtime, and hence
     * provides a state for that stage in a thread-safe manner.
     *
     * @see <a href="http://www.w3.org/TR/xquery/#eval_context">XQuery
     * 1.0: An XML Query Language, 2.1.2 Dynamic Context</a>
     * @see <a href="http://www.w3.org/TR/xquery/#id-dynamic-evaluation">XQuery
     * 1.0: An XML Query Language, 2.2.3.2 Dynamic Evaluation Phase</a>
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class DynamicContext : public ReportContext
    {
    public:
        typedef QExplicitlySharedDataPointer<DynamicContext> Ptr;

        virtual ~DynamicContext()
        {
        }

        /**
         * This function intentionally returns by reference.
         *
         * @see globalItemCacheCell()
         */
        virtual ItemCacheCell &itemCacheCell(const VariableSlotID slot) = 0;

        /**
         * This function intentionally returns by reference.
         *
         * @see globalItemSequenceCacheCells
         */
        virtual ItemSequenceCacheCell::Vector &itemSequenceCacheCells(const VariableSlotID slot) = 0;

        virtual xsInteger contextPosition() const = 0;
        virtual Item contextItem() const = 0;
        virtual xsInteger contextSize() = 0;

        virtual void setRangeVariable(const VariableSlotID slot,
                                      const Item &newValue) = 0;
        virtual Item rangeVariable(const VariableSlotID slot) const = 0;
        virtual void setExpressionVariable(const VariableSlotID slot,
                                           const QExplicitlySharedDataPointer<Expression> &newValue) = 0;
        virtual QExplicitlySharedDataPointer<Expression>
        expressionVariable(const VariableSlotID slot) const = 0;

        virtual Item::Iterator::Ptr positionIterator(const VariableSlotID slot) const = 0;
        virtual void setPositionIterator(const VariableSlotID slot,
                                         const Item::Iterator::Ptr &newValue) = 0;

        virtual void setFocusIterator(const Item::Iterator::Ptr &it) = 0;
        virtual Item::Iterator::Ptr focusIterator() const = 0;

        virtual QExplicitlySharedDataPointer<DayTimeDuration> implicitTimezone() const = 0;
        virtual QDateTime currentDateTime() const = 0;

        virtual QAbstractXmlReceiver *outputReceiver() const = 0;
        virtual NodeBuilder::Ptr nodeBuilder(const QUrl &baseURI) const = 0;
        virtual ResourceLoader::Ptr resourceLoader() const = 0;
        virtual ExternalVariableLoader::Ptr externalVariableLoader() const = 0;
        virtual NamePool::Ptr namePool() const = 0;

        DynamicContext::Ptr createFocus();
        DynamicContext::Ptr createStack();
        DynamicContext::Ptr createReceiverContext(QAbstractXmlReceiver *const receiver);

        /**
         * Whenever a tree gets built, this function is called. DynamicContext
         * has the responsibility of keeping a copy of @p nm, such that it
         * doesn't go out of scope, since no one else will reference @p nm.
         *
         * The caller guarantees that @p nm is not @c null.
         */
        virtual void addNodeModel(const QAbstractXmlNodeModel::Ptr &nm) = 0;

        /**
         * Same as itemCacheCell(), but is only used for global varibles. This
         * is needed because sometimes stack frames needs to be created for
         * other kinds of variables(such as in the case of user function
         * calls), while the global variable(s) needs to continue to use the
         * same cache, instead of one for each new stack frame, typically an
         * instance of StackContextBase.
         *
         * This has two effects:
         *
         * - It's an optimization. Instead of that a global variable gets evaluated each
         * time a user function is called, think recursive functions, it's done
         * only once.
         * - Query stability, hence affects things like node identity and
         * therfore conformance. Hence affects for instance what nodes a query
         * returns, since node identity affect node deduplication.
         */
        virtual ItemCacheCell &globalItemCacheCell(const VariableSlotID slot) = 0;

        /**
         * Same as itemSequenceCacheCells() but applies only for global
         * variables.
         *
         * @see globalItemCacheCell()
         */
        virtual ItemSequenceCacheCell::Vector &globalItemSequenceCacheCells(const VariableSlotID slot) = 0;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
