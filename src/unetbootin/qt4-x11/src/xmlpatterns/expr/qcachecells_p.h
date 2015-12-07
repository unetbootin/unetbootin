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

#ifndef Patternist_CacheCells_H
#define Patternist_CacheCells_H

#include <QList>
#include <QVector>

#include "qitem_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{

    /**
     * @short Represents a cache entry for a single Item,
     * as opposed to for a sequence of items.
     *
     * A characteristic of the ItemCacheCell is that it has two states:
     * either its full or it's not, since it only deals with a single
     * item.
     *
     * Remember that cachedItem doesn't tell the state of the ItemCacheCell.
     * For instance, it can have a null pointer, the empty sequence, and that
     * can be the value of its cache.
     *
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class ItemCacheCell
    {
    public:
        typedef QList<ItemCacheCell> List;
        typedef QVector<ItemCacheCell> Vector;
        enum CacheState
        {
            Full,
            Empty
        };

        inline ItemCacheCell() : cacheState(Empty)
        {
        }

        Item        cachedItem;
        CacheState  cacheState;
    };

    /**
     * @short Represents a cache entry for a sequence of items.
     *
     * As opposed to ItemCacheCell, ItemSequenceCacheCell can be partially
     * populated: e.g, four items is in the cache while three remains in the
     * source. For that reason ItemSequenceCacheCell in addition to the source
     * also carried an QAbstractXmlForwardIterator which is the source, such
     * that it can continue to populate the cache when it runs out.
     *
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class ItemSequenceCacheCell
    {
    public:
        typedef QList<ItemSequenceCacheCell> List;
        typedef QVector<ItemSequenceCacheCell> Vector;

        enum CacheState
        {
            Full,
            Empty,
            PartiallyPopulated
        };

        inline ItemSequenceCacheCell() : cacheState(Empty)
                                       , inUse(false)
        {
        }

        Item::List          cachedItems;
        Item::Iterator::Ptr sourceIterator;
        CacheState          cacheState;
        /**
         * In XSL-T, we can have circularity which we cannot detect statically.
         * For instance, a global variable invokes a template, and the template
         * uses the variable. We can't detect that, because we can't figure out
         * what template will be invoked.
         *
         * For solution we have this toggle, which is set temporarily on the
         * cell such that EvaluationCache can detect whether it's trashing
         * itself.
         *
         * One might think that it would be sufficient to flag usage of the
         * variable in an arbitrary template, but that would also flag valid
         * cases.
         */
        bool                inUse;
    };
}

Q_DECLARE_TYPEINFO(QPatternist::ItemCacheCell, Q_MOVABLE_TYPE);
Q_DECLARE_TYPEINFO(QPatternist::ItemSequenceCacheCell, Q_MOVABLE_TYPE);

QT_END_NAMESPACE

QT_END_HEADER

#endif
