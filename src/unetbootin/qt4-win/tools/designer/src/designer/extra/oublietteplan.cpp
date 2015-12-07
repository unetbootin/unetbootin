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

#include <QtCore/QTextStream>
#include <QtCore/QDateTime>
#include "oublietteplan.h"
#include "tile.h"
#include <stdlib.h>
#include <assert.h>
#include <time.h>

QT_BEGIN_NAMESPACE

OublietteLevel::OublietteLevel(int width, int height, int totalFeatures)
    : m_size(width, height), m_totalFeatures(totalFeatures), m_totalItems(0)
{
#ifndef Q_OS_WINCE
    srand(::time(0));
#else
    srand(QDateTime::currentDateTime().toTime_t());
#endif
    m_map = new Tile[m_size.width() * m_size.height()];
    generateOubliette();
}

OublietteLevel::~OublietteLevel()
{
    for (int y = 0; y < m_size.height(); ++y) {
        for (int x = 0; x < m_size.width(); ++x) {
            qDeleteAll(tile(x, y).items);
        }
    }
    delete [] m_map;
}

Tile OublietteLevel::tile(int x, int y) const
{
    if (y < 0 || x < 0 || y >= m_size.height() || x >= m_size.width())
        return Tile(); // bit of a lie, but it makes gereneration easier...

    return m_map[y * m_size.width() + x];
}

void OublietteLevel::setTile(int x, int y, Tile element)
{
    assert((y >= 0 && y < m_size.height())
           && (x >= 0 && x < m_size.width()));
    m_map[y * m_size.width() + x] = element;
}

struct TmpStruct {
    const char *name;
    const char *desc;
    const char *path;
} bcards[] = {
#include "names.txt"
    , {0 , 0, 0}
};

void OublietteLevel::generateOubliette()
{
    m_roomList.clear();
    QRect levelRect(QPoint(0, 0), m_size);
    fillRect(levelRect, Tile());

    QRect initalRoom(0, 0, 10, 10);
    initalRoom.moveCenter(levelRect.center());
    digRoom(initalRoom, Tile::Lit);
    m_roomList << initalRoom;

    int featureCount = 0;
    while (featureCount < m_totalFeatures) {
        QPoint point = findWall();
        // pick a feature, make sure there is room for it, if there isn't, then start over
        // Otherwise place it and find another feature. Do this until we've placed
        // the total number of feature we want. Try it a couple of times...
        for (int attempts = 0; attempts < 3; ++attempts) {
            if (placeFeature(pickFeature(), point)) {
                setTile(point, Tile(Tile::ClosedDoor));
                ++featureCount;
                break;
            }
        }
    }

    QList<struct TmpStruct *> names;
    for (int i = 0; bcards[i].name; ++i)
        names << &bcards[i];

    m_totalItems = names.size();

    int x, y;
    QPoint centerPoint = QRect(0, 0, m_size.width(), m_size.height()).center();
    QPoint itemPoint;
    for (int i = 0; i < m_totalItems; ++i) {
        do {
            QRect r = m_roomList.at(rand() % m_roomList.size());
            x = rand() % (r.width() - 2);
            y = rand() % (r.height() - 2);
            itemPoint.rx() = r.x() + x + 1;
            itemPoint.ry() = r.y() + y + 1;
        } while (itemPoint == centerPoint);

        int card = rand() % names.size();
        TmpStruct *tmp = names.at(card);
        names.removeAt(card);
        addItemToTile(itemPoint, new BusinessCard(QString::fromUtf8(tmp->name), QString::fromUtf8(tmp->desc), QString::fromUtf8(tmp->path)));
    }



    // Now that everything is built, go through and mark the initial room explored...
    for (int y = initalRoom.top(); y < initalRoom.bottom() + 1; ++y) {
        for (int x = initalRoom.left(); x < initalRoom.right() + 1; ++x) {
            Tile t = tile(x, y);
            t.flags |= Tile::Explored;
            setTile(x, y, t);
        }
    }
}

OublietteLevel::LevelFeature OublietteLevel::pickFeature() const
{
    int roll = rand() % 100;
    LevelFeature ret;
    if (roll >= 0 && roll < 20)
        ret = HorizontalCorridor;
    else if (roll >= 20 && roll < 40)
        ret = VerticalCorridor;
    else if (roll >= 40 && roll < 50)
        ret = SmallHorizontalCorridor;
    else if (roll >= 50 && roll < 60)
        ret = SmallVerticalCorridor;
    else if (roll >= 60 && roll < 75)
        ret = FiveByFiveRoom;
    else if (roll >= 75 && roll < 85)
        ret = ThreeByThreeRoom;
    else
        ret = TenByTenRoom;
    return ret;
}

QRect OublietteLevel::boundingRect(LevelFeature feature) const
{
    QRect ret;
    switch (feature) {
    case HorizontalCorridor:
        ret.setRect(0, 0, 17, 3);
        break;
    case SmallHorizontalCorridor:
        ret.setRect(0, 0, 10, 3);
        break;
    case VerticalCorridor:
        ret.setRect(0, 0, 3, 17);
        break;
    case SmallVerticalCorridor:
        ret.setRect(0, 0, 10, 3);
        break;
    case ThreeByThreeRoom:
        ret.setRect(0, 0, 5, 5);
        break;
    case FiveByFiveRoom:
        ret.setRect(0, 0, 7, 7);
        break;
    case TenByTenRoom:
        ret.setRect(0, 0, 12, 12);
        break;
    default:
        break;
    }
    return ret;
}

bool OublietteLevel::checkForFeatures(const QRect &rect) const
{
    // Simple checks first...
    if (rect.right() + 1 > m_size.width() || rect.left() < 0
        || rect.top() < 0 || rect.bottom() + 1> m_size.height())
        return false;

    for (int y = rect.top(); y < rect.bottom(); ++y) {
        for (int x = rect.left(); x < rect.right(); ++x) {
            Tile le = tile(x, y);
            if (le.type != Tile::Earth)
                return false;
        }
    }
    return true;
}

bool OublietteLevel::placeFeature(LevelFeature feature, const QPoint &pt)
{
    QRect br = boundingRect(feature);
    int deltaX = 0;
    int deltaY = 0;
    if (tile(pt - QPoint(1, 0)).type == Tile::Floor) { // Place it East
        br.moveCenter(pt + QPoint(br.center().x() + 1, 0));
        deltaX = -1;
    } else if (tile(pt + QPoint(1, 0)).type == Tile::Floor) { // West
        br.moveCenter(pt - QPoint(br.center().x(), 0));
    } else if (tile(pt + QPoint(0, 1)).type == Tile::Floor) { // North
        br.moveCenter(pt - QPoint(0, br.center().y()));
    } else { // by elimination
        br.moveCenter(pt + QPoint(0, br.center().y() + 1));
        deltaY = -1;
    }

    if (checkForFeatures(br)) {
        br.adjust(deltaX, deltaY, 0, 0);
        m_roomList << br;
        dig(feature, br);
        return true;
    }
    return false;
}

void OublietteLevel::digRoom(const QRect &rect, Tile::TileFlags flags)
{
    // cheap trick for now, fill with the wall a little bigger and then the room
    QRect interiorRect = rect.adjusted(1, 1, -1, -1);

    fillRect(rect, Tile(Tile::Wall, flags));
    fillRect(interiorRect, Tile(Tile::Floor, flags));
}

void OublietteLevel::dig(LevelFeature /*feature*/, const QRect &rect)
{
    digRoom(rect);
}

void OublietteLevel::printOubliette() const
{
    // print out the darn thing...
    QString str;
    for (int y = 0; y < m_size.height(); ++y) {
        str.clear();
        for (int x = 0; x < m_size.width(); ++x) {
            str += QString::number(tile(x, y).type);
        }
        qDebug("y: %2d: %s", y, str.toLatin1().data());
    }
    qDebug();
}

OubliettePlan::OubliettePlan(int numLevels)
    : m_totalLevels(numLevels)
{
    m_levels.resize(m_totalLevels);
    for (int i = 0; i < m_totalLevels; ++i) {
        m_levels[i] = new OublietteLevel(128, 128, 150);
    }
}

OubliettePlan::~OubliettePlan()
{
    qDeleteAll(m_levels);
    m_levels.clear();
}


OublietteLevel *OubliettePlan::level(int level) const
{
    return m_levels[level - 1];
}

void OublietteLevel::fillRect(int x, int y, int width, int height, Tile le)
{
    for (int r = y; r < y + height; ++r) {
        for (int c = x; c < x + width; ++c) {
            setTile(c, r, le);
        }
    }
}

QPoint OublietteLevel::findWall() const
{
    QPoint ret;
    // randly walk through the oubliette and find a point,
    // if it is a wall that isn't a corner we are in good shape.
    // Technically we could get in here forever, but there is a very big chance that we will
    // break out.
    for (;;) {
        ret = QPoint(rand() % m_size.width(), rand() % m_size.height());

        // Try to see if it works.
        Tile le = tile(ret);
        if (le.type == Tile::Wall) {
            // Check for the floor nearby
            if (tile(ret + QPoint(0, 1)).type == Tile::Floor
                || tile(ret + QPoint(0, -1)).type == Tile::Floor
                || tile(ret + QPoint(1, 0)).type == Tile::Floor
                || tile(ret + QPoint(-1, 0)).type == Tile::Floor)
                break;
        }
    }
    return ret;
}

void OublietteLevel::updateTileFlags(int x, int y, Tile::TileFlags flags)
{
    assert(y >= 0 && y < m_size.width() && x >=0 && y < m_size.height());
    m_map[y * m_size.width() + x].flags |= flags;
}

void OublietteLevel::clearTileFlags(int x, int y)
{
    assert(y >= 0 && y < m_size.width() && x >=0 && y < m_size.height());
    m_map[y * m_size.width() + x].flags = Tile::Default;
}

bool OublietteLevel::blockLOS(int x, int y)
{
    Tile::Type t = tile(x, y).type;

    return t == Tile::Wall || t == Tile::ClosedDoor;
}

void OublietteLevel::addItemToTile(int x, int y, const Item *item)
{
    m_map[y * m_size.width() + x].items.append(item);
}

QT_END_NAMESPACE
