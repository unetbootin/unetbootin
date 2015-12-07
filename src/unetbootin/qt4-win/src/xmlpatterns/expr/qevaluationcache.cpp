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

/**
 * @file
 * @short This file is included by qevaluationcache_p.h.
 * If you need includes in this file, put them in qevaluationcache_p.h, outside of the namespace.
 */

template<bool IsForGlobal>
EvaluationCache<IsForGlobal>::EvaluationCache(const Expression::Ptr &op,
                                              const VariableDeclaration::Ptr &varDecl,
                                              const VariableSlotID aSlot) : SingleContainer(op)
                                                                          , m_declaration(varDecl)
                                                                          , m_varSlot(aSlot)
{
    Q_ASSERT(m_declaration);
    Q_ASSERT(m_varSlot > -1);
}

template<bool IsForGlobal>
Item EvaluationCache<IsForGlobal>::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    ItemCacheCell &cell = IsForGlobal ? context->globalItemCacheCell(m_varSlot) : context->itemCacheCell(m_varSlot);

    if(cell.cacheState == ItemCacheCell::Full)
        return cell.cachedItem;
    else
    {
        Q_ASSERT(cell.cacheState == ItemCacheCell::Empty);
        cell.cachedItem = m_operand->evaluateSingleton(context);
        cell.cacheState = ItemCacheCell::Full;
        return cell.cachedItem;
    }
}

#if defined(Q_OS_IRIX) && defined(Q_CC_MIPS)
/**
 * @short Compile workaround for MIPSPro on IRIX.
 *
 * This function is never called.
 *
 * It's mere presence means the MIPSPro compiler can accept some other code below.
 *
 * I recommend Buddism.
 */
static inline Item::Iterator::Ptr workaroundIrixMIPSPro(const ItemSequenceCacheCell &cell)
{
  return Item::Iterator::Ptr(new ListIterator<Item, Item::List>(cell.cachedItems));
}
#endif

template<bool IsForGlobal>
Item::Iterator::Ptr EvaluationCache<IsForGlobal>::evaluateSequence(const DynamicContext::Ptr &context) const
{
    ItemSequenceCacheCell::Vector &cells = IsForGlobal ? context->globalItemSequenceCacheCells(m_varSlot) : context->itemSequenceCacheCells(m_varSlot);
    ItemSequenceCacheCell &cell = cells[m_varSlot];


    switch(cell.cacheState)
    {
        case ItemSequenceCacheCell::Full:
        {
            /**
             * We don't use makeListIterator() here because the MIPSPro compiler can't handle it.
             */
            return Item::Iterator::Ptr(new ListIterator<Item, Item::List>(cell.cachedItems));
        }
        case ItemSequenceCacheCell::Empty:
        {
            cell.sourceIterator = m_operand->evaluateSequence(context);
            cell.cacheState = ItemSequenceCacheCell::PartiallyPopulated;
            /* Fallthrough. */
        }
        case ItemSequenceCacheCell::PartiallyPopulated:
            return Item::Iterator::Ptr(new CachingIterator(cells, m_varSlot, context));
        default:
        {
            Q_ASSERT_X(false, Q_FUNC_INFO, "This path is not supposed to be run.");
            return Item::Iterator::Ptr();
        }
    }
}

template<bool IsForGlobal>
Expression::Ptr EvaluationCache<IsForGlobal>::typeCheck(const StaticContext::Ptr &context,
                                                        const SequenceType::Ptr &reqType)
{
    /* It's important that we do the typeCheck() before checking for the use of local variables,
     * because ExpressionVariableReference can reference an expression that is a local variable,
     * so it must rewrite itself to it operand before, and it does that in EvaluationCache::typeCheck(). */
    const Expression::Ptr me(SingleContainer::typeCheck(context, reqType));

    OperandsIterator it(me, OperandsIterator::ExcludeParent);
    Expression::Ptr next(it.next());

    /* If our operand or any sub operand gets its value from a for-loop, we cannot
     * cache it since then our cache would be filled -- but not invalidated -- on the
     * first for-iteration. Consider this query:
     *
     * <tt>for $i in expr
     * let $v := $i/p
     * return ($v, $v)</tt>
     *
     * An evaluation cache is inserted for the two operands in the return clause. However,
     * $i changes for each iteration so the cache can only be active on a per-iteration basis,
     * it it's possible(which it isn't).
     *
     * This means that for some queries we don't cache what we really should, and hence evaluate
     * in a sub-optimal way, since this DependsOnLocalVariable don't communicate whether it references
     * a loop that affects us. The correct fix for this would be to let ForExpression reset the
     * relevant caches only, but we don't know which ones that are. */
    while(next)
    {
        if(next->has(DependsOnLocalVariable))
            return m_operand->typeCheck(context, reqType);

        next = it.next();
    }

    return me;
}

template<bool IsForGlobal>
Expression::Ptr EvaluationCache<IsForGlobal>::compress(const StaticContext::Ptr &context)
{
    const Expression::Ptr me(SingleContainer::compress(context));

    if(me.data() != this)
        return me;

    if(m_operand->is(IDRangeVariableReference))
        return m_operand;

    if(m_declaration->usedByMany())
    {
        /* If it's only an atomic value an EvaluationCache is overkill. However,
         * it's still needed for functions like fn:current-time() that must adhere to
         * query stability. */
        const Properties props(m_operand->properties());

        if(props.testFlag(EvaluationCacheRedundant) ||
           ((props.testFlag(IsEvaluated)) &&
           (!(props & DisableElimination)) &&
           CommonSequenceTypes::ExactlyOneAtomicType->matches(m_operand->staticType())))
        {
            return m_operand;
        }
        else
            return me;
    }
    else
    {
        /* If we're only used once, there's no need for an EvaluationCache. */
        return m_operand;
    }
}

template<bool IsForGlobal>
SequenceType::Ptr EvaluationCache<IsForGlobal>::staticType() const
{
    return m_operand->staticType();
}

template<bool IsForGlobal>
SequenceType::List EvaluationCache<IsForGlobal>::expectedOperandTypes() const
{
    /* Remember that EvaluationCache::typeCheck() will be called from multiple locations,
     * which potentially have different type requirements. For instance, one wants a node,
     * and another requires atomization and casting.
     *
     * Returning ZeroOrMoreItems is safe here because staticType() returns the operand's type
     * and therefore the convertors like Atomizer will be parents to us, and hence only affect
     * the relevant path.
     *
     * ZeroOrMoreItems also make sense logically since we're actually only used where the
     * variable references reference us. */
    SequenceType::List result;
    result.append(CommonSequenceTypes::ZeroOrMoreItems);

    return result;
}

template<bool IsForGlobal>
Expression::Properties EvaluationCache<IsForGlobal>::properties() const
{
    /* We cannot return the operand's properties unconditionally, because some
     * doesn't hold for this Expression.
     *
     * However, some of the properties must propagate through, which are the ones being OR'd here.
     */
    return m_operand->properties() & (DisableElimination | IsEvaluated | DisableTypingDeduction);
}

template<bool IsForGlobal>
ExpressionVisitorResult::Ptr
EvaluationCache<IsForGlobal>::accept(const ExpressionVisitor::Ptr &visitor) const
{
    return visitor->visit(this);
}

template<bool IsForGlobal>
const SourceLocationReflection *EvaluationCache<IsForGlobal>::actualReflection() const
{
    return m_operand->actualReflection();
}

