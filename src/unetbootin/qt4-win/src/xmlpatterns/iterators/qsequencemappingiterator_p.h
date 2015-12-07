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

#ifndef Patternist_SequenceMappingIterator_H
#define Patternist_SequenceMappingIterator_H

#include "qabstractxmlforwarditerator_p.h"
#include "qdynamiccontext_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short Proxies another QAbstractXmlForwardIterator, and for each item, returns the
     * Sequence returned from a mapping function.
     *
     * ItemMappingIterator is practical when the items in an QAbstractXmlForwardIterator needs to
     * be translated to another sequence, while still doing it in a pipe-lined
     * fashion. In contrast to ItemMappingIterator, SequenceMappingIterator maps
     * each item into another QAbstractXmlForwardIterator, and where the SequenceMappingIterator's own
     * result is the concatenation of all those Iterators. Hence, while ItemMappingIterator
     * is better tailored for one-to-one or one-to-zero conversion, SequenceMappingIterator
     * is more suitable for one-to-many conversion.
     *
     * This is achieved by that SequenceMappingIterator's constructor takes
     * an instance of a class, that must have the following member:
     *
     * @code
     * QAbstractXmlForwardIterator<TResult>::Ptr mapToSequence(const TSource::Ptr &item,
     *                                                      const DynamicContext::Ptr &context) const;
     * @endcode
     *
     * @author Frans Englich <fenglich@trolltech.com>
     * @see ItemMappingIterator
     * @ingroup Patternist_iterators
     */
    template<typename TResult, typename TSource, typename TMapper>
    class SequenceMappingIterator : public QAbstractXmlForwardIterator<TResult>
    {
    public:
        /**
         * Constructs a SequenceMappingIterator.
         *
         * @param mapper the object that has the mapToItem() sequence.
         * @param sourceIterator the QAbstractXmlForwardIterator whose items should be mapped.
         * @param context the DynamicContext that will be passed to the map function.
         * May be null.
         */
        SequenceMappingIterator(const TMapper &mapper,
                                const typename QAbstractXmlForwardIterator<TSource>::Ptr &sourceIterator,
                                const DynamicContext::Ptr &context);

        virtual TResult next();
        virtual xsInteger count();
        virtual TResult current() const;
        virtual xsInteger position() const;

        /**
         * The reason the implementation is placed in line here, is due to a bug
         * in MSVC-2005 version 14.00.50727.762. Note that it works with version 14.00.50727.42.
         */
        virtual typename QAbstractXmlForwardIterator<TResult>::Ptr copy() const
        {
            return typename QAbstractXmlForwardIterator<TResult>::Ptr
                    (new SequenceMappingIterator<TResult, TSource, TMapper>(m_mapper,
                                                                            m_mainIterator->copy(),
                                                                            m_context));
        }

    private:
        xsInteger                                           m_position;
        TResult                                             m_current;
        typename QAbstractXmlForwardIterator<TSource>::Ptr  m_mainIterator;
        typename QAbstractXmlForwardIterator<TResult>::Ptr  m_currentIterator;
        const typename DynamicContext::Ptr                  m_context;
        const TMapper                                       m_mapper;
    };

    template<typename TResult, typename TSource, typename TMapper>
    SequenceMappingIterator<TResult, TSource, TMapper>::SequenceMappingIterator(
                                        const TMapper &mapper,
                                        const typename QAbstractXmlForwardIterator<TSource>::Ptr &iterator,
                                        const DynamicContext::Ptr &context)
                                        : m_position(0),
                                          m_mainIterator(iterator),
                                          m_context(context),
                                          m_mapper(mapper)
    {
        Q_ASSERT(mapper);
        Q_ASSERT(iterator);
    }

    template<typename TResult, typename TSource, typename TMapper>
    TResult SequenceMappingIterator<TResult, TSource, TMapper>::next()
    {
        /* This was once implemented with a recursive function, but the stack
         * got blown for some inputs by that approach. */
        while(true)
        {
            while(!m_currentIterator)
            {
                const TSource mainItem(m_mainIterator->next());

                if(qIsForwardIteratorEnd(mainItem)) /* We've reached the very end. */
                {
                    m_position = -1;
                    m_current = TResult();
                    return TResult();
                }
                else
                    m_currentIterator = m_mapper->mapToSequence(mainItem, m_context);
            }

            m_current = m_currentIterator->next();

            if(qIsForwardIteratorEnd(m_current))
            {
                m_currentIterator.reset();
                continue;
            }
            else
            {
                ++m_position;
                return m_current;
            }
        }
    }

    template<typename TResult, typename TSource, typename TMapper>
    xsInteger SequenceMappingIterator<TResult, TSource, TMapper>::count()
    {
        TSource unit(m_mainIterator->next());
        xsInteger c = 0;

        while(!qIsForwardIteratorEnd(unit))
        {
            const typename QAbstractXmlForwardIterator<TResult>::Ptr sit(m_mapper->mapToSequence(unit, m_context));
            c += sit->count();
            unit = m_mainIterator->next();
        }

        return c;
    }

    template<typename TResult, typename TSource, typename TMapper>
    TResult SequenceMappingIterator<TResult, TSource, TMapper>::current() const
    {
        return m_current;
    }

    template<typename TResult, typename TSource, typename TMapper>
    xsInteger SequenceMappingIterator<TResult, TSource, TMapper>::position() const
    {
        return m_position;
    }


    /**
     * @short An object generator for SequenceMappingIterator.
     *
     * makeSequenceMappingIterator() is a convenience function for avoiding specifying
     * the full template instantiation for SequenceMappingIterator. Conceptually, it
     * is identical to Qt's qMakePair().
     *
     * @returns a SequenceMappingIterator wrapped in a smart pointer, that has been
     * passed the constructor arguments @p mapper, @p source, and @p context.
     * @see makeMappingCallbackPtr()
     * @relates QAbstractXmlForwardIterator
     */
    template<typename TResult, typename TSource, typename TMapper>
    static inline
    typename QAbstractXmlForwardIterator<TResult>::Ptr
    makeSequenceMappingIterator(const TMapper &mapper,
                                const QExplicitlySharedDataPointer<QAbstractXmlForwardIterator<TSource> > &source,
                                const DynamicContext::Ptr &context)
    {
        return typename QAbstractXmlForwardIterator<TResult>::Ptr
            (new SequenceMappingIterator<TResult, TSource, TMapper>(mapper, source, context));
    }
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
