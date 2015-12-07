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

#include <QtCore/QSize>
#include <QtCore/QTimer>
#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QKeyEvent>
#include <QtGui/QLabel>
#include <QtGui/QListWidget>
#include <QtGui/QMessageBox>
#include <QtGui/QPainter>
#include <QtGui/QPixmapCache>
#include <QtGui/QPushButton>
#include <QtGui/QScrollArea>
#include <qdebug.h>

#include "oubliette.h"
#include "fov.h"
#include "itemdialog.h"
#include <stdio.h>

QT_BEGIN_NAMESPACE

static const int TileWidth = 32;
static const int TileHeight = 32;

struct ItemEffect {
    static const int TotalSteps;
    ItemEffect(const Item *newItem, const QPoint &newPos)
        : item(newItem), rect(0, 0, 140, 60), step(0)
    {
        rect.moveCenter(QPoint(newPos.x() * TileWidth, newPos.y() * TileHeight));
    }
    const Item *item;
    QRect rect;
    int directionX;
    int directionY;
    int step;
};

const int ItemEffect::TotalSteps = 15;

Oubliette::Oubliette()
    : m_timerID(-1)
{
    // set a fixed size, pased on the oubliette size
    m_currentLevel = 1;
    QSize sz = m_oubliettePlan.level(m_currentLevel)->size();
    setFixedSize(sz.width() * TileWidth, sz.height() * TileHeight);
    m_character.setPosition(QRect(QPoint(0, 0), sz).center(), false);
    m_oldCursorPosition = m_character.position();
    QPalette pal = palette();
    pal.setColor(backgroundRole(), Qt::black);
    setPalette(pal);
}

Oubliette::~Oubliette()
{
}

void Oubliette::showEvent(QShowEvent *ev)
{
    QWidget::showEvent(ev);
    QTimer::singleShot(0, this, SLOT(showInstructions()));
}

void Oubliette::paintEvent(QPaintEvent *pe)
{
    QPainter p(this);
    paintOubliette(&p, pe->rect());

    QPoint pt = m_character.position();
    QPixmap pm;
    QString pcString = QLatin1String(":/trolltech/qthack/images/human.png");
    QPixmapCache::find(pcString, pm);
    if (pm.isNull()) {
        pm = QPixmap(pcString);
        QPixmapCache::insert(pcString, pm);
    }
    p.drawPixmap(pt.x() * TileWidth, pt.y() * TileHeight, TileWidth, TileHeight, pm);

    // paint the moving cards
    foreach (ItemEffect *effect, m_effects) {
        if (effect->step >= ItemEffect::TotalSteps) {
            m_effects.removeAt(m_effects.indexOf(effect));
            ItemDialog *dlg = new ItemDialog(this, effect->item);
            QRect dlgRect(mapToGlobal(effect->rect.topLeft()), effect->rect.size());
            QRect desktopRect = QApplication::desktop()->availableGeometry(this);
            if (dlgRect.intersect(desktopRect) != dlgRect)
                dlgRect.moveCenter(desktopRect.center());
            dlg->resize(dlgRect.size());
            dlg->move(dlgRect.topLeft());
            dlg->show();
            delete effect;
            continue;
        }
        p.fillRect(effect->rect, QColor(150, 150, 150, 240));
    }
}

void Oubliette::paintOubliette(QPainter *p, const QRect &paintRect)
{
   // snap down
    int tile_x1 = paintRect.x() / TileWidth;
    int tile_y1 = paintRect.y() / TileHeight;

    // snap up
    int tile_x2 = (paintRect.x() + paintRect.width()) / TileWidth + TileWidth;
    int tile_y2 = (paintRect.y() + paintRect.height()) / TileHeight + TileHeight;

    OublietteLevel *level = m_oubliettePlan.level(m_currentLevel);
    for (int y = tile_y1; y <= tile_y2; ++y) {
        for (int x = tile_x1; x <= tile_x2; ++x) {
            fillTile(p, x, y, level->tile(x, y));
        }
    }
}

void Oubliette::fillTile(QPainter *p, int x, int y, Tile le)
{
    QString str;
    switch (le.type) {
    case Tile::ClosedDoor:
        str = QLatin1String(":/trolltech/qthack/images/dngn_closed_door.png");
        break;
    case Tile::OpenDoor:
        str = QLatin1String(":/trolltech/qthack/images/dngn_open_door.png");
        break;
    case Tile::Wall:
        str = QLatin1String(":/trolltech/qthack/images/dngn_rock_wall_07.png");
        break;
    case Tile::Floor:
        break;
    default:
        return; // Don't draw it...
    }

    if (!le.items.isEmpty())
        str = le.items.at(0)->pixmapName();
    if (le.flags & Tile::Explored) {
        if (str.isEmpty()) {
            p->fillRect(x * TileWidth, y * TileHeight, TileWidth, TileHeight, QColor("teal"));
        } else {
            p->fillRect(x * TileWidth, y * TileHeight, TileWidth, TileHeight, QColor("teal"));
            QPixmap pm;
            QPixmapCache::find(str, pm);
            if (pm.isNull()) {
                pm = QPixmap(str);
                QPixmapCache::insert(str, pm);
            }
            p->drawPixmap(x * TileWidth, y * TileHeight, TileWidth, TileHeight, pm);
        }
    }
}

void Oubliette::keyPressEvent(QKeyEvent *ke)
{
    QPoint newPos = m_character.position();
    switch (ke->key()) {
    case Qt::Key_Up:
    case Qt::Key_K:
        newPos.ry() -= 1;
        break;
    case Qt::Key_Down:
    case Qt::Key_J:
        newPos.ry() += 1;
        break;
    case Qt::Key_Left:
    case Qt::Key_H:
        newPos.rx() -= 1;
        break;
    case Qt::Key_Right:
    case Qt::Key_L:
        newPos.rx() += 1;
        break;
    case Qt::Key_I:
        showInventory();
        break;
    default:
        QWidget::keyPressEvent(ke);
    }
    if (tryMove(newPos)) {
        QRect r(QPoint((newPos.x() - 8) * TileWidth, (newPos.y() - 8) * TileHeight),
                QSize(24 * TileWidth, 24 * TileHeight));
        update(r);
        emit characterMoved(visualCursorPos());
    }
}

void Oubliette::showInventory()
{
    QDialog *d = new QDialog();
    d->setWindowTitle(tr("Inventory"));
    d->setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    QGridLayout *gl = new QGridLayout(d);
    int row = 0;
    QLabel *label;
    const QList<const Item *> items = m_character.items();
    if (items.isEmpty()) {
        label = new QLabel(tr("You have <B>No</B> Items"), d);
        label->setAlignment(Qt::AlignCenter);
        gl->addWidget(label, row, 0, 1, 4);
        ++row;
    } else {
        QListWidget *lw = new QListWidget(d);
        gl->addWidget(lw, 0, 0, 1, 4);
        int i = 0;
        foreach (const Item *item, items) {
            QListWidgetItem *lwi = new QListWidgetItem(item->name(), lw);
            lwi->setIcon(QIcon(item->pixmapName()));
            lwi->setData(99, i);
            ++i;
        }
        connect(lw, SIGNAL(itemActivated(QListWidgetItem*)),
                this, SLOT(showInventoryItem(QListWidgetItem*)));
    }
    label = new QLabel(tr("You have %1 of %2 items", "", m_oubliettePlan.level(m_currentLevel)->totalItems())
                      .arg(items.size()), d);
    gl->addWidget(label, 1, 1, 1, 1);
    QPushButton *btn = new QPushButton(tr("OK"), d);
    btn->setDefault(true);
    btn->setFocus();
    gl->addWidget(btn, 2, 3, 1, 1);
    connect(btn, SIGNAL(clicked()), d, SLOT(accept()));
    d->setAttribute(Qt::WA_ShowModal);
    d->setAttribute(Qt::WA_DeleteOnClose);
    d->show();
}

/*
    Try and move the character to the absolute position \a newPos. If it is possible to move there,
    set the characters \a newPos and return true; otherwise return false.
*/
bool Oubliette::tryMove(const QPoint &newPos)
{
    OublietteLevel *level = m_oubliettePlan.level(m_currentLevel);
    Tile le = level->tile(newPos);
    if (le.type == Tile::Floor || le.type == Tile::ClosedDoor || le.type == Tile::OpenDoor) {
        m_oldCursorPosition = m_character.position();
        m_character.setPosition(newPos);
        if (le.type == Tile::ClosedDoor) {
            le.type = Tile::OpenDoor;
            level->setTile(newPos, le);
        }
        if (!le.items.isEmpty()) {
            foreach (const Item *item, le.items) {
                m_character.addItem(item);
                animateItem(item, newPos);
                if (m_character.items().size() == m_oubliettePlan.level(m_currentLevel)->totalItems())
                    QTimer::singleShot(0, this, SLOT(showVictory()));

            }
            le.items.clear();
            level->setTile(newPos, le);
        }
        updateExplored();
        return true;
    }
    return false;
}

void Oubliette::updateExplored()
{
    static SIMPLEFOV fov;
    QPoint exploredPoint = m_character.position();
    OublietteLevel *level = m_oubliettePlan.level(m_currentLevel);
    fov.start(level, exploredPoint.x(), exploredPoint.y(), 8);
}

QPoint Oubliette::visualCursorPos() const
{
    QPoint pt = m_character.position();
    return QPoint(pt.x() * TileWidth, pt.y() * TileHeight);
}

void Oubliette::animateItem(const Item *item, const QPoint &pos)
{
    ItemEffect *newEffect = new ItemEffect(item, pos);
    m_effects.append(newEffect);
    if (m_timerID == -1) {
        m_timerID = startTimer(20);
    }
}

void Oubliette::timerEvent(QTimerEvent *)
{
    if (m_effects.isEmpty()) {
        killTimer(m_timerID);
        m_timerID = -1;
    }
    QRegion region;
    foreach (ItemEffect *effect, m_effects) {
        if (effect->step >= ItemEffect::TotalSteps) {
            region += effect->rect;
            continue;
        }
        effect->rect.adjust(-10, -5, 10, 5);
        ++effect->step;
        region += effect->rect;
    }
    update(region);
}

void Oubliette::showInventoryItem(QListWidgetItem *lwi)
{
    ItemDialog *dlg = new ItemDialog(this, m_character.items().at(lwi->data(99).toInt()));
    dlg->exec();
}

void Oubliette::showInstructions()
{
    QMessageBox::information(window(), tr("Easter Egg Found"),
                             tr("Welcome to the Trolltech Business Card Hunt\n"
                                "Use the direction keys to move around and find"
                                " the business cards for all the trolls."));
}

void Oubliette::showVictory()
{
    int value = QMessageBox::information(window(), tr("You Did It!"),
            tr("You've collected all the Trolltech cards. It took you %n steps.\n"
               "There's nothing more here. You should get back to work.", "", m_character.totalSteps()),
            tr("That's rather anti-climatic"), tr("Quit"));
    if (value == 1)
        QApplication::instance()->quit();
}

QT_END_NAMESPACE
