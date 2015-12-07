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

#include "qbuiltintypes_p.h"
#include "qcommonsequencetypes_p.h"
#include "qcommonvalues_p.h"
#include "qliteral_p.h"
#include "qnumeric_p.h"

#include "qdeepequalfn_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

bool DeepEqualFN::evaluateEBV(const DynamicContext::Ptr &context) const
{
    const Item::Iterator::Ptr it1(m_operands.first()->evaluateSequence(context));
    const Item::Iterator::Ptr it2(m_operands.at(1)->evaluateSequence(context));

    while(true)
    {
        const Item item1(it1->next());
        const Item item2(it2->next());

        if(!item1)
        {
            if(item2)
                return false;
            else
                return true;
        }
        else if(!item2)
        {
            if(item1)
                return false;
            else
                return true;
        }
        else if(item1.isNode())
        {
            if(item2.isNode())
            {
                if(item1.asNode().isDeepEqual(item2.asNode()))
                    continue;
                else
                    return false;
            }
            else
                return false;
        }
        else if(item2.isNode())
        {
            /* We know that item1 is not a node due to the check above. */
            return false;
        }
        else if(flexibleCompare(item1, item2, context))
            continue;
        else if(BuiltinTypes::numeric->itemMatches(item1) &&
                item1.as<Numeric>()->isNaN() &&
                item2.as<Numeric>()->isNaN())
        {
            // TODO
            /* Handle the specific NaN circumstances. item2 isn't checked whether it's of
             * type numeric, since the AtomicComparator lookup would have failed if both weren't
             * numeric. */
            continue;
        }
        else
            return false;
    };
}

Expression::Ptr DeepEqualFN::typeCheck(const StaticContext::Ptr &context,
                                       const SequenceType::Ptr &reqType)
{
    const Expression::Ptr me(FunctionCall::typeCheck(context, reqType));
    const ItemType::Ptr t1(m_operands.first()->staticType()->itemType());
    const ItemType::Ptr t2(m_operands.at(1)->staticType()->itemType());
    /* TODO This can be much more improved, and the optimizations should be moved
     * to compress(). */

    if(*CommonSequenceTypes::Empty == *t1)
    {
        if(*CommonSequenceTypes::Empty == *t2)
            return wrapLiteral(CommonValues::BooleanTrue, context, this);
        else
            return me;
    }
    else if(*CommonSequenceTypes::Empty == *t2)
    {
        if(*CommonSequenceTypes::Empty == *t1)
            return wrapLiteral(CommonValues::BooleanTrue, context, this);
        else
            return me;
    }
    else if(BuiltinTypes::node->xdtTypeMatches(t1) &&
            BuiltinTypes::node->xdtTypeMatches(t2))
        return me; /* We're comparing nodes. */
    else if(BuiltinTypes::xsAnyAtomicType->xdtTypeMatches(t1) &&
            BuiltinTypes::xsAnyAtomicType->xdtTypeMatches(t2))
    {
        prepareComparison(fetchComparator(t1, t2, context));
        return me;
    }
    else
    {
        if(BuiltinTypes::node->xdtTypeMatches(t1) && BuiltinTypes::xsAnyAtomicType->xdtTypeMatches(t2) ||
           BuiltinTypes::node->xdtTypeMatches(t2) && BuiltinTypes::xsAnyAtomicType->xdtTypeMatches(t1))
        {
            /* One operand contains nodes and the other atomic values, or vice versa. They can never
             * be identical. */
        // TODO warn?
            return wrapLiteral(CommonValues::BooleanFalse, context, this);
        }
        else
        {
            // TODO Warn?
            return me;
        }
    }
}

QT_END_NAMESPACE
