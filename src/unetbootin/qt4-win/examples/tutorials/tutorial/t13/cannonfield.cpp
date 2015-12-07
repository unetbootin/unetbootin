/****************************************************************************
**
** Copyright (C) 2005-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
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

#include <QDateTime>
#include <QPaintEvent>
#include <QPainter>
#include <QTimer>

#include <math.h>
#include <stdlib.h>

#include "cannonfield.h"

CannonField::CannonField(QWidget *parent)
    : QWidget(parent)
{
    currentAngle = 45;
    currentForce = 0;
    timerCount = 0;
    autoShootTimer = new QTimer(this);
    connect(autoShootTimer, SIGNAL(timeout()), this, SLOT(moveShot()));
    shootAngle = 0;
    shootForce = 0;
    target = QPoint(0, 0);
//! [0]
    gameEnded = false;
//! [0]
    setPalette(QPalette(QColor(250, 250, 200)));
    setAutoFillBackground(true);
    newTarget();
}

void CannonField::setAngle(int angle)
{
    if (angle < 5)
        angle = 5;
    if (angle > 70)
        angle = 70;
    if (currentAngle == angle)
        return;
    currentAngle = angle;
    update(cannonRect());
    emit angleChanged(currentAngle);
}

void CannonField::setForce(int force)
{
    if (force < 0)
        force = 0;
    if (currentForce == force)
        return;
    currentForce = force;
    emit forceChanged(currentForce);
}

//! [1]
void CannonField::shoot()
{
    if (isShooting())
        return;
    timerCount = 0;
    shootAngle = currentAngle;
    shootForce = currentForce;
    autoShootTimer->start(5);
    emit canShoot(false);
}
//! [1]

void CannonField::newTarget()
{
    static bool firstTime = true;

    if (firstTime) {
        firstTime = false;
        QTime midnight(0, 0, 0);
        qsrand(midnight.secsTo(QTime::currentTime()));
    }
    target = QPoint(200 + qrand() % 190, 10 + qrand() % 255);
    update();
}

//! [2]
void CannonField::setGameOver()
{
    if (gameEnded)
        return;
    if (isShooting())
        autoShootTimer->stop();
    gameEnded = true;
    update();
}
//! [2]

//! [3]
void CannonField::restartGame()
{
    if (isShooting())
        autoShootTimer->stop();
    gameEnded = false;
    update();
    emit canShoot(true);
}
//! [3]

void CannonField::moveShot()
{
    QRegion region = shotRect();
    ++timerCount;

    QRect shotR = shotRect();

    if (shotR.intersects(targetRect())) {
        autoShootTimer->stop();
        emit hit();
        emit canShoot(true);
    } else if (shotR.x() > width() || shotR.y() > height()) {
        autoShootTimer->stop();
        emit missed();
        emit canShoot(true);
    } else {
        region = region.unite(shotR);
    }
    update(region);
}

//! [4]
void CannonField::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter(this);

    if (gameEnded) {
        painter.setPen(Qt::black);
        painter.setFont(QFont("Courier", 48, QFont::Bold));
        painter.drawText(rect(), Qt::AlignCenter, tr("Game Over"));
    }
//! [4] //! [5]
    paintCannon(painter);
    if (isShooting())
        paintShot(painter);
    if (!gameEnded)
        paintTarget(painter);
}
//! [5]

void CannonField::paintShot(QPainter &painter)
{
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::black);
    painter.drawRect(shotRect());
}

void CannonField::paintTarget(QPainter &painter)
{
    painter.setPen(Qt::black);
    painter.setBrush(Qt::red);
    painter.drawRect(targetRect());
}

const QRect barrelRect(30, -5, 20, 10);

void CannonField::paintCannon(QPainter &painter)
{
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::blue);

    painter.save();
    painter.translate(0, height());
    painter.drawPie(QRect(-35, -35, 70, 70), 0, 90 * 16);
    painter.rotate(-currentAngle);
    painter.drawRect(barrelRect);
    painter.restore();
}

QRect CannonField::cannonRect() const
{
    QRect result(0, 0, 50, 50);
    result.moveBottomLeft(rect().bottomLeft());
    return result;
}

QRect CannonField::shotRect() const
{
    const double gravity = 4;

    double time = timerCount / 20.0;
    double velocity = shootForce;
    double radians = shootAngle * 3.14159265 / 180;

    double velx = velocity * cos(radians);
    double vely = velocity * sin(radians);
    double x0 = (barrelRect.right() + 5) * cos(radians);
    double y0 = (barrelRect.right() + 5) * sin(radians);
    double x = x0 + velx * time;
    double y = y0 + vely * time - 0.5 * gravity * time * time;

    QRect result(0, 0, 6, 6);
    result.moveCenter(QPoint(qRound(x), height() - 1 - qRound(y)));
    return result;
}

QRect CannonField::targetRect() const
{
    QRect result(0, 0, 20, 10);
    result.moveCenter(QPoint(target.x(), height() - 1 - target.y()));
    return result;
}

bool CannonField::isShooting() const
{
    return autoShootTimer->isActive();
}
