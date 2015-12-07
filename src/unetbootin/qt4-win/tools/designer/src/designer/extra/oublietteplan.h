/****************************************************************************
**
** Copyright (C) 2005-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt Designer of the Qt Toolkit.
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

#ifndef OUBLIETTEPLAN_H
#define OUBLIETTEPLAN_H

#include <QtCore/QtCore>
#include "tile.h"

QT_BEGIN_NAMESPACE

class OublietteLevel
{
public:
    enum LevelFeature {
        HorizontalCorridor,
        SmallHorizontalCorridor,
        VerticalCorridor,
        SmallVerticalCorridor,
        ThreeByThreeRoom,
        FiveByFiveRoom,
        TenByTenRoom,
        TotalFeatures
    };

    OublietteLevel(int width = 128, int height = 96, int totalFeatures = 200);
    ~OublietteLevel();
    inline QSize size() const { return m_size; }
    inline int width() const { return size().width(); }
    inline int height() const { return size().height(); }
    inline int totalItems() const { return m_totalItems; }
    Tile tile(int x, int y) const;
    inline Tile tile(const QPoint &pos) const { return tile(pos.x(), pos.y()); }
    void updateTileFlags(int x, int y, Tile::TileFlags flags);
    inline void updateTileFlags(const QPoint &point, Tile::TileFlags flags)
    { updateTileFlags(point.x(), point.y(), flags); }

    void addItemToTile(int x, int y, const Item *item);
    inline void addItemToTile(const QPoint &pos, const Item *item)
    { addItemToTile(pos.x(), pos.y(), item); }

    void clearTileFlags(int x, int y);
    inline void clearTileFlags(const QPoint &point) { clearTileFlags(point.x(), point.y()); }
    void printOubliette() const;
    bool blockLOS(int x, int y);
    void setTile(int x, int y, Tile element);
    inline void setTile(const QPoint &pos, Tile element)
    { setTile(pos.x(), pos.y(), element); }

private:
    void generateOubliette();
    void dig(LevelFeature feature, const QRect &rect);
    void digRoom(const QRect &rect, Tile::TileFlags flags = Tile::Default);
    QPoint findWall() const;
    LevelFeature pickFeature() const;
    QRect boundingRect(LevelFeature feature) const;
    bool placeFeature(LevelFeature feature, const QPoint &pt);
    inline void fillRect(const QRect &r, Tile le)
    { fillRect(r.x(), r.y(), r.width(), r.height(), le); }
    void fillRect(int x, int y, int width, int height, Tile le);
    bool checkForFeatures(const QRect &rect) const;

private:
    QSize m_size;
    Tile *m_map;
    int m_totalFeatures;
    int m_totalItems;
    QList<QRect> m_roomList;
};

class OubliettePlan
{
public:
    OubliettePlan(int totalLevels = 1);
    ~OubliettePlan();
    OublietteLevel *level(int level) const;
    inline int totalLevels() const { return m_totalLevels; }

private:
    QVector<OublietteLevel*> m_levels;
    int m_totalLevels;
};

QT_END_NAMESPACE

#endif // OUBLIETTEPLAN_H
