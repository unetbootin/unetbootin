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

#ifndef Patternist_CachingIterator_H
#define Patternist_CachingIterator_H

#include <QList>
#include <QVector>

#include "qdynamiccontext_p.h"
#include "qitem_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short An QAbstractXmlForwardIterator that gets its item from a cache unless its empty, in
     * which case it continues to populate the cache as well as deliver on its
     * own from a source QAbstractXmlForwardIterator.
     *
     * @author Frans Englich <frans.fenglich@trolltech.com>
     * @ingroup Patternist_iterators
     */
    class CachingIterator : public Item::Iterator
    {
    public:
        /**
         * We always use the same cache cell so why don't we use it directly,
         * instead of passing the slot and ItemSequenceCacheCell::Vector to
         * this class? Because the GenericDynamicContext might decide to resize
         * the vector and that would invalidate the reference.
         *
         * We intentionally pass in a non-const reference here.
         */
        CachingIterator(ItemSequenceCacheCell::Vector &cacheCells,
                        const VariableSlotID slot,
                        const DynamicContext::Ptr &context);

        virtual Item next();
        virtual Item current() const;
        virtual xsInteger position() const;
        virtual Item::Iterator::Ptr copy() const;

    private:
        Item                        m_current;
        xsInteger                   m_position;

        /**
         * This variable cannot be called m_slot, because
         * /usr/include/sys/sysmacros.h on hpuxi-acc defines it.
         */
        const VariableSlotID        m_varSlot;

        /**
         * We don't use the context. We only keep a reference such that it
         * doesn't get deleted, and m_cacheCells starts to dangle.
         */
        const DynamicContext::Ptr   m_context;

        /**
         * We intentionally store a reference here such that we are able to
         * modify the item.
         */
        ItemSequenceCacheCell::Vector &m_cacheCells;

        /**
         * Whether this CachingIterator is delivering items from
         * m_cacheCell.cacheItems or from m_cacheCell.sourceIterator.
         */
        bool m_usingCache;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
