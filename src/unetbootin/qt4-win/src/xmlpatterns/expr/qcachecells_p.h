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
     * also carried an QAbstractXmlForwardIterator which is the source, such that it can continue
     * to populate the cache when it runs out.
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
        {
        }

        Item::List          cachedItems;
        Item::Iterator::Ptr sourceIterator;
        CacheState          cacheState;
    };
}

Q_DECLARE_TYPEINFO(QPatternist::ItemCacheCell, Q_MOVABLE_TYPE);
Q_DECLARE_TYPEINFO(QPatternist::ItemSequenceCacheCell, Q_MOVABLE_TYPE);

QT_END_NAMESPACE

QT_END_HEADER

#endif
