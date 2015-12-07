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

#ifndef Patternist_SingletonIterator_H
#define Patternist_SingletonIterator_H

#include <QtXmlPatterns/private/qabstractxmlforwarditerator_p.h>

#include <QtXmlPatterns/private/qprimitives_p.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short An QAbstractXmlForwardIterator over exactly one item.
     *
     * SingletonIterator's constructor takes one value which is
     * the item it forms an QAbstractXmlForwardIterator over. Other QAbstractXmlForwardIterator instances can
     * also form an QAbstractXmlForwardIterator with one in length, but by that SingletonIterator
     * has this as it only task, it means it is efficient at it.
     *
     * Having to represent single items in Iterators is relatively common.
     *
     * @author Frans Englich <fenglich@trolltech.com>
     * @ingroup Patternist_iterators
     */
    template<typename T>
    class SingletonIterator : public QAbstractXmlForwardIterator<T>
    {
    public:
        /**
         * Creates an iterator over @p item.
         *
         * @note item may not be @c null. Use the EmptyIterator for
         * the empty sequence
         */
        SingletonIterator(const T &item) : m_item(item),
                                           m_position(0)
        {
            Q_ASSERT(!qIsForwardIteratorEnd(item));
        }

        virtual T next()
        {
            switch(m_position)
            {
                case 0:
                {
                    ++m_position;
                    return m_item;
                }
                case 1:
                {
                    m_position = -1;
                    return T();
                }
                default:
                {
                    Q_ASSERT(m_position == -1);
                    return T();
                }
            }
        }

        virtual T current() const
        {
            if(m_position == 1)
                return m_item;
            else
                return T();
        }

        virtual xsInteger position() const
        {
            return m_position;
        }

        /**
         * @returns a copy of this instance, rewinded to the beginning.
         */
        virtual typename QAbstractXmlForwardIterator<T>::Ptr toReversed()
        {
            return typename QAbstractXmlForwardIterator<T>::Ptr(new SingletonIterator<T>(m_item));
        }

        /**
         * @returns always 1
         */
        virtual xsInteger count()
        {
            return 1;
        }

        virtual typename QAbstractXmlForwardIterator<T>::Ptr copy() const
        {
            return typename QAbstractXmlForwardIterator<T>::Ptr(new SingletonIterator(m_item));
        }

    private:
        const T m_item;
        qint8 m_position;
    };

    /**
     * @short An object generator for SingletonIterator.
     *
     * makeSingletonIterator() is a convenience function for avoiding specifying
     * the full template instantiation for SingletonIterator. Conceptually, it
     * is identical to Qt's qMakePair().
     *
     * @relates SingletonIterator
     */
    template<typename T>
    inline
    typename SingletonIterator<T>::Ptr
    makeSingletonIterator(const T &item)
    {
        return typename SingletonIterator<T>::Ptr(new SingletonIterator<T>(item));
    }
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
