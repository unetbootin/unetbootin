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

#include "itemcircleanimation.h"
#include "demoitemanimation.h"
#include "colors.h"
#include "menumanager.h"
#include "mainwindow.h"
#include "menumanager.h"

static QGraphicsScene *sscene;

//////////////////// POST EFFECT STUFF ////////////////////////////////////////

class TickerPostEffect
{
public:
    virtual ~TickerPostEffect(){};
    virtual void tick(float){};
    virtual void transform(DemoItem *, QPointF &){};
};

class PostRotateXY : public TickerPostEffect
{
public:
    float currRotX, currRotY;
    float speedx, speedy, curvx, curvy;

    PostRotateXY(float speedx, float speedy, float curvx, float curvy)
        : currRotX(0), currRotY(0),
        speedx(speedx), speedy(speedy),
        curvx(curvx), curvy(curvy){};

    void tick(float adjust)
    {
        currRotX += speedx * adjust;
        currRotY += speedy * adjust;
    }

    void transform(DemoItem *item, QPointF &pos)
    {
        DemoItem *parent = (DemoItem *) item->parentItem();
        QPointF center = parent->boundingRect().center();
        pos.setX(center.x() + (pos.x() - center.x()) * cos(currRotX + pos.x() * curvx));
        pos.setY(center.y() + (pos.y() - center.y()) * cos(currRotY + pos.y() * curvy));
    }
};

class PostRotateXYTwist : public TickerPostEffect
{
public:
    float currRotX, currRotY;
    float speedx, speedy, curvx, curvy;

    PostRotateXYTwist(float speedx, float speedy, float curvx, float curvy)
        : currRotX(0), currRotY(0),
        speedx(speedx), speedy(speedy),
        curvx(curvx), curvy(curvy){};

    void tick(float adjust)
    {
        currRotX += speedx * adjust;
        currRotY += speedy * adjust;
    }

    void transform(DemoItem *item, QPointF &pos)
    {
        DemoItem *parent = (DemoItem *) item->parentItem();
        QPointF center = parent->boundingRect().center();
        pos.setX(center.x() + (pos.x() - center.x()) * cos(currRotX + pos.y() * curvx));
        pos.setY(center.y() + (pos.y() - center.y()) * cos(currRotY + pos.x() * curvy));
    }
};

//////////////////// TICKER EFFECT STUFF //////////////////////////////////////

class TickerEffect
{
    TickerPostEffect *postEffect;
public:
    enum EffectStatus{Normal, Intro, Outro} status;
    LetterList *letters;
    float morphSpeed, moveSpeed;
    float normalMorphSpeed, normalMoveSpeed;
    bool useSheepDog, morphBetweenModels;

    TickerEffect(LetterList *letters)
        : postEffect(new TickerPostEffect()), status(Intro), letters(letters),
        morphSpeed(Colors::tickerMorphSpeed), moveSpeed(Colors::tickerMoveSpeed),
        normalMorphSpeed(Colors::tickerMorphSpeed), normalMoveSpeed(Colors::tickerMoveSpeed),
        useSheepDog(true), morphBetweenModels(!Colors::noTickerMorph){}

    void setPostEffect(TickerPostEffect *effect)
    {
        delete postEffect;
        postEffect = effect;
    }

    virtual ~TickerEffect()
    {
        delete postEffect;
    }

    void slowDownAfterIntro(float adjust)
    {
        if (morphBetweenModels){
            if (status == Intro){
                float dec = 0.1 * adjust;
                moveSpeed -= dec;
                if (moveSpeed < Colors::tickerMoveSpeed){
                    moveSpeed = normalMoveSpeed;
                    morphSpeed = normalMorphSpeed;
                    status = Normal;
                }
            }
        }
    }

    void moveLetters(float adjust)
    {
        float adaptedMoveSpeed = this->moveSpeed * adjust;
        float adaptedMorphSpeed = this->morphSpeed * adjust;
        postEffect->tick(adjust);

        for (int i=0; i<letters->size(); i++){
            LetterItem *letter = letters->at(i);
            letter->guideAdvance(this->morphBetweenModels ? adaptedMoveSpeed : Colors::tickerMoveSpeed);
            letter->guideMove(this->morphBetweenModels ? adaptedMorphSpeed : -1);

            QPointF pos = letter->getGuidedPos();
            postEffect->transform(letter, pos);

            if (useSheepDog)
                letter->setPosUsingSheepDog(pos, QRectF(0, 0, 800, 600));
            else
                letter->setPos(pos);
        }
    }

    virtual void tick(float adjust)
    {
        slowDownAfterIntro(adjust);
        moveLetters(adjust);
    }

};

class EffectWhirlWind : public TickerEffect
{
public:
    EffectWhirlWind(LetterList *letters) : TickerEffect(letters)
    {
        moveSpeed = 50;
        for (int i=0; i<this->letters->size(); i++){
            LetterItem *letter = this->letters->at(i);
            letter->setGuidedPos(QPointF(0, 100));
        }
    }
};

class EffectSnake : public TickerEffect
{
public:
    EffectSnake(LetterList *letters) : TickerEffect(letters)
    {
        moveSpeed = 40;
        for (int i=0; i<this->letters->size(); i++){
            LetterItem *letter = this->letters->at(i);
            letter->setGuidedPos(QPointF(0, -250 - (i * 5)));
        }
    }
};

class EffectScan : public TickerEffect
{
public:
    EffectScan(LetterList *letters) : TickerEffect(letters)
    {
        for (int i=0; i<this->letters->size(); i++){
            LetterItem *letter = this->letters->at(i);
            letter->setGuidedPos(QPointF(100, -300));
        }
    }
};

class EffectRaindrops : public TickerEffect
{
public:
    EffectRaindrops(LetterList *letters) : TickerEffect(letters)
    {
        for (int i=0; i<this->letters->size(); i++){
            LetterItem *letter = this->letters->at(i);
            letter->setGuidedPos(QPointF(-100 + rand() % 200, - 200.0f - rand() % 1300));
        }
    }
};

class EffectLine : public TickerEffect
{
public:
    EffectLine(LetterList *letters) : TickerEffect(letters)
    {
        for (int i=0; i<this->letters->size(); i++){
            LetterItem *letter = this->letters->at(i);
            letter->setGuidedPos(QPointF(100, 500.0f + i * 20));
        }
    }
};

//////////////////// TICKER STUFF /////////////////////////////////////////////

ItemCircleAnimation::ItemCircleAnimation(QGraphicsScene *scene, QGraphicsItem *parent)
    : DemoItem(scene, parent)
{
    sscene = scene;
    this->letterCount = Colors::tickerLetterCount;
    this->scale = 1;
    this->showCount = -1;
    this->tickOnPaint = false;
    this->paused = false;
    this->doIntroTransitions = true;
    this->setAcceptsHoverEvents(true);
    this->setCursor(Qt::OpenHandCursor);
    this->setupGuides();
    this->setupLetters();
    this->useGuideQt();
    this->effect = 0;//new TickerEffect(this->letterList);
}

ItemCircleAnimation::~ItemCircleAnimation()
{
    delete this->letterList;
    delete this->qtGuide1;
    delete this->qtGuide2;
    delete this->qtGuide3;
    delete this->effect;
}

void ItemCircleAnimation::createLetter(char c)
{
    LetterItem *letter = new LetterItem(c, sscene, this);
    this->letterList->append(letter);
}

void ItemCircleAnimation::setupLetters()
{
    this->letterList = new LetterList();

    QString s = Colors::tickerText;
    int len = s.length();
    int i = 0;
    for (; i < this->letterCount - len; i += len)
        for (int l=0; l<len; l++)
            createLetter(s[l].toLatin1());

    // Fill inn with blanks:
    for (; i < this->letterCount; ++i)
        createLetter(' ');
}

void ItemCircleAnimation::setupGuides()
{
    int x = 0;
    int y = 20;

    this->qtGuide1 = new GuideCircle(QRectF(x, y, 260, 260), -36, 342);
    new GuideLine(QPointF(x + 240, y + 268), this->qtGuide1);
    new GuideLine(QPointF(x + 265, y + 246), this->qtGuide1);
    new GuideLine(QPointF(x + 158, y + 134), this->qtGuide1);
    new GuideLine(QPointF(x + 184, y + 109), this->qtGuide1);
    new GuideLine(QPointF(x + 160, y +  82), this->qtGuide1);
    new GuideLine(QPointF(x +  77, y + 163), this->qtGuide1); // T-top
    new GuideLine(QPointF(x + 100, y + 190), this->qtGuide1);
    new GuideLine(QPointF(x + 132, y + 159), this->qtGuide1);
    new GuideLine(QPointF(x + 188, y + 211), this->qtGuide1);
    new GuideCircle(QRectF(x + 30, y + 30, 200, 200), -30, 336, GuideCircle::CW, this->qtGuide1);
    new GuideLine(QPointF(x + 238, y + 201), this->qtGuide1);

    y = 30;
    this->qtGuide2 = new GuideCircle(QRectF(x + 30, y + 30, 200, 200), 135, 270, GuideCircle::CCW);
    new GuideLine(QPointF(x + 222, y + 38), this->qtGuide2);
    new GuideCircle(QRectF(x, y, 260, 260), 135, 270, GuideCircle::CW, this->qtGuide2);
    new GuideLine(QPointF(x + 59, y + 59), this->qtGuide2);

    x = 115;
    y = 10;
    this->qtGuide3 = new GuideLine(QLineF(x, y, x + 30, y));
    new GuideLine(QPointF(x + 30, y + 170), this->qtGuide3);
    new GuideLine(QPointF(x, y + 170), this->qtGuide3);
    new GuideLine(QPointF(x, y), this->qtGuide3);

    this->qtGuide1->setFence(QRectF(0, 0, 800, 600));
    this->qtGuide2->setFence(QRectF(0, 0, 800, 600));
    this->qtGuide3->setFence(QRectF(0, 0, 800, 600));
}

void ItemCircleAnimation::useGuide(Guide *guide, int firstLetter, int lastLetter)
{
    float padding = guide->lengthAll() / float(lastLetter - firstLetter);
    for (int i=firstLetter; i<lastLetter; i++){
        LetterItem *letter = this->letterList->at(i);
        letter->useGuide(guide, (i - firstLetter) * padding);
    }
}

void ItemCircleAnimation::useGuideQt()
{
    if (this->currGuide != this->qtGuide1){
        this->useGuide(qtGuide1, 0, this->letterCount);
        this->currGuide = qtGuide1;
    }
}

void ItemCircleAnimation::useGuideTt()
{
    if (this->currGuide != this->qtGuide2){
        int split = int(this->letterCount * 5.0 / 7.0);
        this->useGuide(qtGuide2, 0, split);
        this->useGuide(qtGuide3, split, this->letterCount);
        this->currGuide = qtGuide2;
    }
}

QRectF ItemCircleAnimation::boundingRect() const
{
    return QRectF(0, 0, 300, 320);
}

void ItemCircleAnimation::prepare()
{
}

void ItemCircleAnimation::switchToNextEffect()
{
    ++this->showCount;
    delete this->effect;

    switch (this->showCount){
    case 1:
        this->effect = new EffectSnake(this->letterList);
        break;
    case 2:
        this->effect = new EffectLine(this->letterList);
        this->effect->setPostEffect(new PostRotateXYTwist(0.01f, 0.0f, 0.003f, 0.0f));
        break;
    case 3:
        this->effect = new EffectRaindrops(this->letterList);
        this->effect->setPostEffect(new PostRotateXYTwist(0.01f, 0.005f, 0.003f, 0.003f));
        break;
    case 4:
        this->effect = new EffectScan(this->letterList);
        this->effect->normalMoveSpeed = 0;
        this->effect->setPostEffect(new PostRotateXY(0.008f, 0.0f, 0.005f, 0.0f));
        break;
    default:
        this->showCount = 0;
        this->effect = new EffectWhirlWind(this->letterList);
    }
}

void ItemCircleAnimation::animationStarted(int id)
{
    if (id == DemoItemAnimation::ANIM_IN){
        if (this->doIntroTransitions){
            // Make all letters dissapear
            for (int i=0; i<this->letterList->size(); i++){
                LetterItem *letter = this->letterList->at(i);
                letter->setPos(1000, 0);
            }
            this->switchToNextEffect();
            this->useGuideQt();
            this->scale = 1;
            // The first time we run, we have a rather large
            // delay to perform benchmark before the ticker shows.
            // But now, since we are showing, use a more appropriate value:
            this->currentAnimation->startDelay = 1500;
        }
    }
    else if (this->effect)
        this->effect->useSheepDog = false;

    this->tickTimer = QTime::currentTime();
}

void ItemCircleAnimation::animationStopped(int)
{
    // Nothing to do.
}

void ItemCircleAnimation::swapModel(){
    if (this->currGuide == this->qtGuide2)
        this->useGuideQt();
    else
        this->useGuideTt();
}

void ItemCircleAnimation::hoverEnterEvent(QGraphicsSceneHoverEvent *)
{
//  Skip swap here to enhance ticker dragging
//    this->swapModel();
}

void ItemCircleAnimation::hoverLeaveEvent(QGraphicsSceneHoverEvent *)
{
    this->swapModel();
}

void ItemCircleAnimation::setTickerScale(float s)
{
    this->scale = s;
    qtGuide1->setScale(this->scale, this->scale);
    qtGuide2->setScale(this->scale, this->scale);
    qtGuide3->setScale(this->scale, this->scale);
}

void ItemCircleAnimation::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    this->mouseMoveLastPosition = event->scenePos();
    if (event->button() == Qt::LeftButton)
        this->setCursor(Qt::ClosedHandCursor);
    else
        this->switchToNextEffect();
}

void ItemCircleAnimation::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        this->setCursor(Qt::OpenHandCursor);
}

void ItemCircleAnimation::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QPointF newPosition = event->scenePos();
    this->setPosUsingSheepDog(this->pos() + newPosition - this->mouseMoveLastPosition, QRectF(-260, -280, 1350, 1160));
    this->mouseMoveLastPosition = newPosition;
}

void ItemCircleAnimation::wheelEvent(QGraphicsSceneWheelEvent *event)
{
    this->effect->moveSpeed = this->effect->moveSpeed + (event->delta() > 0 ? -0.20 : 0.20);
    if (this->effect->moveSpeed < 0)
        this->effect->moveSpeed = 0;
}

void ItemCircleAnimation::pause(bool on)
{
    this->paused = on;
    this->tickTimer = QTime::currentTime();
}

void ItemCircleAnimation::tick()
{
    if (this->paused || !this->effect)
        return;

    float t = this->tickTimer.msecsTo(QTime::currentTime());
    this->tickTimer = QTime::currentTime();
    this->effect->tick(t/10.0f);
}

void ItemCircleAnimation::paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *)
{
    if (this->tickOnPaint)
        tick();
}




