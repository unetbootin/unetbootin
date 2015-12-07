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

#ifndef CANNON_H
#define CANNON_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QTimer;
QT_END_NAMESPACE

class CannonField : public QWidget
{
    Q_OBJECT

public:
    CannonField(QWidget *parent = 0);

    int angle() const { return currentAngle; }
    int force() const { return currentForce; }

public slots:
    void setAngle(int angle);
    void setForce(int force);
    void shoot();
//! [0]
    void newTarget();
//! [0]

private slots:
    void moveShot();

//! [1]
signals:
    void hit();
    void missed();
//! [1]
    void angleChanged(int newAngle);
    void forceChanged(int newForce);

protected:
    void paintEvent(QPaintEvent *event);

private:
    void paintShot(QPainter &painter);
//! [2]
    void paintTarget(QPainter &painter);
//! [2]
    void paintCannon(QPainter &painter);
    QRect cannonRect() const;
    QRect shotRect() const;
//! [3]
    QRect targetRect() const;
//! [3]

    int currentAngle;
    int currentForce;

    int timerCount;
    QTimer *autoShootTimer;
    float shootAngle;
    float shootForce;

//! [4]
    QPoint target;
//! [4]
};

#endif
