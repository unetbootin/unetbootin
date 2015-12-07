/****************************************************************************
**
** Copyright (C) 2005-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the demonstration applications of the Qt Toolkit.
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

#ifndef ITEM_CIRCLE_ANIMATION_H
#define ITEM_CIRCLE_ANIMATION_H

#include <QtCore>
#include <QObject>
#include <QtGui>
#include <QTimeLine>
#include <QList>
#include "demoitem.h"
#include "letteritem.h"
#include "guideline.h"
#include "guidecircle.h"

typedef QList<LetterItem *> LetterList;
class TickerEffect;

class ItemCircleAnimation : public QObject, public DemoItem
{
public:
    ItemCircleAnimation(QGraphicsScene *scene = 0, QGraphicsItem *parent = 0);
    virtual ~ItemCircleAnimation();

    // overidden methods:
    QRectF boundingRect() const;
    void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget * = 0);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void wheelEvent(QGraphicsSceneWheelEvent *event);
    void animationStarted(int id = 0);
    void animationStopped(int id = 0);
    void prepare();
    void tick();
    void switchToNextEffect();
    void useGuideQt();
    void useGuideTt();
    void pause(bool on);

    bool tickOnPaint;
    bool paused;
    bool doIntroTransitions;

private:
    void setupLetters();
    void createLetter(char c);
    void setupGuides();
    void useGuide(Guide *guide, int firstLetter, int lastLetter);
    void swapModel();
    void setTickerScale(float s);

    int showCount;
    float scale;
    QPointF mouseMoveLastPosition;
    int letterCount;
    LetterList *letterList;
    Guide *qtGuide1;
    Guide *qtGuide2;
    Guide *qtGuide3;
    Guide *currGuide;
    TickerEffect *effect;
    QTime tickTimer;
};

#endif // ITEM_CIRCLE_ANIMATION_H



