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

#include <QApplication>
#include <QFont>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLCDNumber>
#include <QLabel>
#include <QPushButton>
#include <QShortcut>
#include <QVBoxLayout>
#include <QWidget>

#include "cannonfield.h"
#include "gameboard.h"
#include "lcdrange.h"

GameBoard::GameBoard(QWidget *parent)
    : QWidget(parent)
{
    QPushButton *quit = new QPushButton(tr("&Quit"));
    quit->setFont(QFont("Times", 18, QFont::Bold));

    connect(quit, SIGNAL(clicked()), qApp, SLOT(quit()));

    LCDRange *angle = new LCDRange(tr("ANGLE"));
    angle->setRange(5, 70);

    LCDRange *force = new LCDRange(tr("FORCE"));
    force->setRange(10, 50);

//! [0]
    QFrame *cannonBox = new QFrame;
//! [0] //! [1]
    cannonBox->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
//! [1]

    cannonField = new CannonField;

    connect(angle, SIGNAL(valueChanged(int)),
            cannonField, SLOT(setAngle(int)));
    connect(cannonField, SIGNAL(angleChanged(int)),
            angle, SLOT(setValue(int)));

    connect(force, SIGNAL(valueChanged(int)),
            cannonField, SLOT(setForce(int)));
    connect(cannonField, SIGNAL(forceChanged(int)),
            force, SLOT(setValue(int)));

    connect(cannonField, SIGNAL(hit()),
            this, SLOT(hit()));
    connect(cannonField, SIGNAL(missed()),
            this, SLOT(missed()));

    QPushButton *shoot = new QPushButton(tr("&Shoot"));
    shoot->setFont(QFont("Times", 18, QFont::Bold));

    connect(shoot, SIGNAL(clicked()),
            this, SLOT(fire()));
    connect(cannonField, SIGNAL(canShoot(bool)),
            shoot, SLOT(setEnabled(bool)));

    QPushButton *restart = new QPushButton(tr("&New Game"));
    restart->setFont(QFont("Times", 18, QFont::Bold));

    connect(restart, SIGNAL(clicked()), this, SLOT(newGame()));

    hits = new QLCDNumber(2);
    hits->setSegmentStyle(QLCDNumber::Filled);

    shotsLeft = new QLCDNumber(2);
    shotsLeft->setSegmentStyle(QLCDNumber::Filled);

    QLabel *hitsLabel = new QLabel(tr("HITS"));
    QLabel *shotsLeftLabel = new QLabel(tr("SHOTS LEFT"));

//! [2]
    (void) new QShortcut(Qt::Key_Enter, this, SLOT(fire()));
//! [2] //! [3]
    (void) new QShortcut(Qt::Key_Return, this, SLOT(fire()));
//! [3] //! [4]
    (void) new QShortcut(Qt::CTRL + Qt::Key_Q, this, SLOT(close()));
//! [4]

    QHBoxLayout *topLayout = new QHBoxLayout;
    topLayout->addWidget(shoot);
    topLayout->addWidget(hits);
    topLayout->addWidget(hitsLabel);
    topLayout->addWidget(shotsLeft);
    topLayout->addWidget(shotsLeftLabel);
    topLayout->addStretch(1);
    topLayout->addWidget(restart);

    QVBoxLayout *leftLayout = new QVBoxLayout;
    leftLayout->addWidget(angle);
    leftLayout->addWidget(force);

//! [5]
    QVBoxLayout *cannonLayout = new QVBoxLayout;
//! [5] //! [6]
    cannonLayout->addWidget(cannonField);
    cannonBox->setLayout(cannonLayout);

    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->addWidget(quit, 0, 0);
    gridLayout->addLayout(topLayout, 0, 1);
    gridLayout->addLayout(leftLayout, 1, 0);
    gridLayout->addWidget(cannonBox, 1, 1, 2, 1);
    gridLayout->setColumnStretch(1, 10);
    setLayout(gridLayout);
//! [6]

    angle->setValue(60);
    force->setValue(25);
    angle->setFocus();

    newGame();
}

void GameBoard::fire()
{
    if (cannonField->gameOver() || cannonField->isShooting())
        return;
    shotsLeft->display(shotsLeft->intValue() - 1);
    cannonField->shoot();
}

void GameBoard::hit()
{
    hits->display(hits->intValue() + 1);
    if (shotsLeft->intValue() == 0)
        cannonField->setGameOver();
    else
        cannonField->newTarget();
}

void GameBoard::missed()
{
    if (shotsLeft->intValue() == 0)
        cannonField->setGameOver();
}

void GameBoard::newGame()
{
    shotsLeft->display(15);
    hits->display(0);
    cannonField->restartGame();
    cannonField->newTarget();
}
