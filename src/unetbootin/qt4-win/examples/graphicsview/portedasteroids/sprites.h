/****************************************************************************
**
** Copyright (C) 2006-2008 Trolltech ASA. All rights reserved.
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

/*
 * KAsteroids - Copyright (c) Martin R. Jones 1997
 *
 * Part of the KDE project
 */

#ifndef __SPRITES_H__
#define __SPRITES_H__

#include "animateditem.h"

#define ID_ROCK_LARGE           1024
#define ID_ROCK_MEDIUM          1025
#define ID_ROCK_SMALL           1026

#define ID_MISSILE              1030

#define ID_BIT                  1040
#define ID_EXHAUST              1041

#define ID_ENERGY_POWERUP       1310
#define ID_TELEPORT_POWERUP     1311
#define ID_BRAKE_POWERUP        1312
#define ID_SHIELD_POWERUP       1313
#define ID_SHOOT_POWERUP        1314

#define ID_SHIP                 1350
#define ID_SHIELD               1351

#define MAX_SHIELD_AGE          350
#define MAX_POWERUP_AGE         500
#define MAX_MISSILE_AGE         40

class KMissile : public AnimatedPixmapItem
{
public:
    KMissile( const QList<QPixmap> &s, QGraphicsScene *c ) : AnimatedPixmapItem( s, c )
        { myAge = 0; }

    virtual int type() const { return ID_MISSILE; }

    void growOlder() { myAge++; }
    bool expired() { return myAge > MAX_MISSILE_AGE; }

private:
    int myAge;
};

class KBit : public AnimatedPixmapItem
{
public:
    KBit( const QList<QPixmap> &s, QGraphicsScene *c ) : AnimatedPixmapItem( s, c )
	{  death = 7; }

    virtual int type() const {  return ID_BIT; }

    void setDeath( int d ) { death = d; }
    void growOlder() { death--; }
    bool expired() { return death <= 0; }

private:
    int death;
};

class KExhaust : public AnimatedPixmapItem
{
public:
    KExhaust( const QList<QPixmap> &s, QGraphicsScene *c ) : AnimatedPixmapItem( s, c )
	{  death = 1; }

    virtual int type() const {  return ID_EXHAUST; }

    void setDeath( int d ) { death = d; }
    void growOlder() { death--; }
    bool expired() { return death <= 0; }

private:
    int death;
};

class KPowerup : public AnimatedPixmapItem
{
public:
  KPowerup( const QList<QPixmap> &s, QGraphicsScene *c, int t ) : AnimatedPixmapItem( s, c ),
        myAge( 0 ), _type(t) { }

  virtual int type() const { return _type; }

  void growOlder() { myAge++; }
  bool expired() const { return myAge > MAX_POWERUP_AGE; }

protected:
  int myAge;
  int _type;
};

class KRock : public AnimatedPixmapItem
{
public:
    KRock (const QList<QPixmap> &s, QGraphicsScene *c, int t, int sk, int st) : AnimatedPixmapItem( s, c )
        { _type = t; skip = cskip = sk; step = st; }

    void nextFrame()
	{
	    if (cskip-- <= 0) {
		setFrame( (frame()+step+frameCount())%frameCount() );
		cskip = QABS(skip);
	    }
	}

    virtual int type() const { return _type; }

private:
    int _type;
    int skip;
    int cskip;
    int step;
};

class KShield : public AnimatedPixmapItem
{
public:
  KShield( QList<QPixmap> &s, QGraphicsScene *c )
      : AnimatedPixmapItem( s, c ) {}

  virtual int type() const { return ID_SHIELD; }
};

#endif
