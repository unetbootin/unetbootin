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

#ifndef Patternist_ItemMappingIterator_H
#define Patternist_ItemMappingIterator_H

#include "qabstractxmlforwarditerator_p.h"
#include "qdynamiccontext_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short Proxies another QAbstractXmlForwardIterator, and for each item, returns the
     * Item returned from a mapping function.
     *
     * ItemMappingIterator is practical when the items in an QAbstractXmlForwardIterator needs to
     * be translated to another sequence, while still doing it in a pipe-lined
     * fashion.
     *
     * This is achieved by that ItemMappingIterator's constructor takes
     * an instance of a class, that must have the following member:
     *
     * @code
     * TResult::Ptr mapToItem(const TSource &item,
     *                        const Context &context) const
     * @endcode
     *
     * For each item in the QAbstractXmlForwardIterator ItemMappingIterator proxies, this function is
     * called and its return value becomes the return value of the ItemMappingIterator. If the
     * mapping function returns null, ItemMappingIterator maps the next item in the source sequence
     * such that a contiguous sequence of items is returned.
     *
     * Declaring the mapToItem() function as inline, can be a good way to improve performance.
     *
     * @see SequenceMappingIterator
     * @author Frans Englich <fenglich@trolltech.com>
     * @ingroup Patternist_iterators
     */
    template<typename TResult, typename TSource, typename TMapper, typename Context = DynamicContext::Ptr>
    class ItemMappingIterator : public QAbstractXmlForwardIterator<TResult>
    {
    public:
        /**
         * Constructs an ItemMappingIterator.
         *
         * @param mapper the object that has the mapToItem() sequence.
         * @param iterator the QAbstractXmlForwardIterator whose items should be mapped.
         * @param context the context that will be passed to the map function.
         * May be null.
         */
        ItemMappingIterator(const TMapper &mapper,
                            const typename QAbstractXmlForwardIterator<TSource>::Ptr &iterator,
                            const Context &context) : m_mapper(mapper)
                                                    , m_it(iterator)
                                                    , m_context(context)
                                                    , m_position(0)
        {
            Q_ASSERT(mapper);
            Q_ASSERT(iterator);
        }

        /**
         * @returns the next item in the sequence, or
         * @c null if the end have been reached.
         */
        virtual TResult next()
        {
            const TSource sourceItem(m_it->next());

            if(qIsForwardIteratorEnd(sourceItem))
            {
                m_current = TResult();
                m_position = -1;
                return TResult();
            }
            else
            {
                m_current = m_mapper->mapToItem(sourceItem, m_context);
                if(qIsForwardIteratorEnd(m_current))
                    return next(); /* The mapper returned null, so continue with the next in the source. */
                else
                {
                    ++m_position;
                    return m_current;
                }
            }
        }

        virtual TResult current() const
        {
            return m_current;
        }

        virtual xsInteger position() const
        {
            return m_position;
        }

        virtual typename QAbstractXmlForwardIterator<TResult>::Ptr copy() const
        {
            return typename QAbstractXmlForwardIterator<TResult>::Ptr
                (new ItemMappingIterator<TResult, TSource, TMapper, Context>(m_mapper, m_it->copy(), m_context));
        }

    private:
        const TMapper                                               m_mapper;
        const typename QAbstractXmlForwardIterator<TSource>::Ptr    m_it;
        const Context                                               m_context;
        TResult                                                     m_current;
        xsInteger                                                   m_position;
    };

    /**
     * @short An object generator for ItemMappingIterator.
     *
     * makeItemMappingIterator() is a convenience function for avoiding specifying
     * the full template instantiation for ItemMappingIterator. Conceptually, it
     * is identical to Qt's qMakePair().
     *
     * @relates ItemMappingIterator
     */
    template<typename TResult, typename TSource, typename TMapper, typename Context>
    static inline
    typename QAbstractXmlForwardIterator<TResult>::Ptr
    makeItemMappingIterator(const TMapper &mapper,
                            const QExplicitlySharedDataPointer<QAbstractXmlForwardIterator<TSource> > &source,
                            const Context &context)
    {
        return typename QAbstractXmlForwardIterator<TResult>::Ptr
            (new ItemMappingIterator<TResult, TSource, TMapper, Context>(mapper, source, context));
    }
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
