/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt3Support module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "q3rangecontrol.h"

#ifndef QT_NO_SPINWIDGET

#include "qabstractspinbox.h"
#include "qevent.h"
#include "qpainter.h"
#include "qrect.h"
#include "qstyle.h"
#include "qstyleoption.h"
#include "qtimer.h"

QT_BEGIN_NAMESPACE

class Q3SpinWidgetPrivate
{
public:
    Q3SpinWidgetPrivate()
        : upEnabled(true),
          downEnabled(true),
          theButton(0),
          buttonDown(0),
          timerUp(0),
          bsyms(Q3SpinWidget::UpDownArrows),
          ed (0) {}
    uint upEnabled :1;
    uint downEnabled :1;
    uint theButton :2;
    uint buttonDown :2;
    uint timerUp : 1;
    QRect up;
    QRect down;
    QTimer auRepTimer;
    Q3SpinWidget::ButtonSymbols bsyms;
    QWidget *ed;
    void startTimer(int msec) { auRepTimer.start(msec, true); }
    void startTimer(bool up, int msec) { timerUp = up; startTimer(msec); }
    void stopTimer() { auRepTimer.stop(); }
};

/*!
    \class Q3SpinWidget qspinwidget.h
    \brief The Q3SpinWidget class is an internal range control related class.

    \internal

    Constructs an empty range control widget with parent \a parent
    called \a name.

*/

Q3SpinWidget::Q3SpinWidget(QWidget* parent, const char* name)
    : QWidget(parent, name)
{
    d = new Q3SpinWidgetPrivate();
    connect(&d->auRepTimer, SIGNAL(timeout()), this, SLOT(timerDone()));
    setFocusPolicy(Qt::StrongFocus);

    arrange();
    updateDisplay();
}


/*! Destroys the object and frees any allocated resources.

*/

Q3SpinWidget::~Q3SpinWidget()
{
    delete d;
}

/*! */
QWidget * Q3SpinWidget::editWidget()
{
    return d->ed;
}

/*!
    Sets the editing widget to \a w.
*/
void Q3SpinWidget::setEditWidget(QWidget * w)
{
    if (w) {
        if (w->parentWidget() != this)
            w->setParent(this);
        setFocusProxy(w);
    }
    d->ed = w;
    arrange();
    updateDisplay();
}

/*! \reimp

*/

void Q3SpinWidget::mousePressEvent(QMouseEvent *e)
{
    if (e->button() != Qt::LeftButton) {
        d->stopTimer();
        d->buttonDown = 0;
        d->theButton = 0;
        repaint(d->down.united(d->up));
        return;
    }

    uint oldButtonDown = d->buttonDown;

    if (d->down.contains(e->pos()) && d->downEnabled)
        d->buttonDown = 1;
    else if (d->up.contains(e->pos()) && d->upEnabled)
        d->buttonDown = 2;
    else
        d->buttonDown = 0;

    d->theButton = d->buttonDown;
    if (oldButtonDown != d->buttonDown) {
        if (!d->buttonDown) {
            repaint(d->down.united(d->up));
        } else if (d->buttonDown & 1) {
            repaint(d->down);
            stepDown();
            d->startTimer(false, 300);
        } else if (d->buttonDown & 2) {
            repaint(d->up);
            stepUp();
            d->startTimer(true, 300);
        }
    }

    if (!oldButtonDown && !d->buttonDown)
        e->ignore();

}

static QStyleOptionSpinBox getStyleOption(const Q3SpinWidget *spin)
{
    QStyleOptionSpinBox opt;
    opt.init(spin);
    opt.frame = true;
    opt.subControls = 0;
    opt.buttonSymbols = (QAbstractSpinBox::ButtonSymbols)spin->buttonSymbols();
    opt.stepEnabled = 0;
    if (spin->isUpEnabled())
        opt.stepEnabled |= QAbstractSpinBox::StepUpEnabled;
    if (spin->isDownEnabled())
        opt.stepEnabled |= QAbstractSpinBox::StepDownEnabled;
    opt.activeSubControls = 0;
    return opt;
}

/*!

*/

void Q3SpinWidget::arrange()
{
    QStyleOptionSpinBox opt = getStyleOption(this);
    d->up = style()->subControlRect(QStyle::CC_SpinBox, &opt, QStyle::SC_SpinBoxUp, this);
    d->down = style()->subControlRect(QStyle::CC_SpinBox, &opt, QStyle::SC_SpinBoxDown, this);
    if (d->ed) {
        QRect r = style()->subControlRect(QStyle::CC_SpinBox, &opt,
                                          QStyle::SC_SpinBoxEditField, this);
        d->ed->setGeometry(r);
    }
}

/*!

*/

void Q3SpinWidget::stepUp()
{
    emit stepUpPressed();
}

void Q3SpinWidget::resizeEvent(QResizeEvent*)
{
    arrange();
}

/*!

*/

void Q3SpinWidget::stepDown()
{
    emit stepDownPressed();
}


void Q3SpinWidget::timerDone()
{
    // we use a double timer to make it possible for users to do
    // something with 0-timer on valueChanged.
    QTimer::singleShot(1, this, SLOT(timerDoneEx()));
}

void Q3SpinWidget::timerDoneEx()
{
    if (!d->buttonDown)
        return;
    if (d->timerUp)
        stepUp();
    else
        stepDown();
    d->startTimer(100);
}


/*!
    The event is passed in \a e.
*/

void Q3SpinWidget::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() != Qt::LeftButton)
        return;

    uint oldButtonDown = d->theButton;
    d->theButton = 0;
    if (oldButtonDown != d->theButton) {
        if (oldButtonDown & 1)
            repaint(d->down);
        else if (oldButtonDown & 2)
            repaint(d->up);
    }
    d->stopTimer();
    d->buttonDown = 0;

    if (!oldButtonDown && !d->buttonDown)
        e->ignore();
}


/*!
    The event is passed in \a e.
*/

void Q3SpinWidget::mouseMoveEvent(QMouseEvent *e)
{
    if (!(e->state() & Qt::LeftButton))
        return;

    uint oldButtonDown = d->theButton;
    if (oldButtonDown & 1 && !d->down.contains(e->pos())) {
        d->stopTimer();
        d->theButton = 0;
        repaint(d->down);
    } else if (oldButtonDown & 2 && !d->up.contains(e->pos())) {
        d->stopTimer();
        d->theButton = 0;
        repaint(d->up);
    } else if (!oldButtonDown && d->up.contains(e->pos()) && d->buttonDown & 2) {
        d->startTimer(500);
        d->theButton = 2;
        repaint(d->up);
    } else if (!oldButtonDown && d->down.contains(e->pos()) && d->buttonDown & 1) {
        d->startTimer(500);
        d->theButton = 1;
        repaint(d->down);
    }

    if (!oldButtonDown && !d->buttonDown)
        e->ignore();
}


/*!
    The event is passed in \a e.
*/
#ifndef QT_NO_WHEELEVENT
void Q3SpinWidget::wheelEvent(QWheelEvent *e)
{
    e->accept();
    static float offset = 0;
    static Q3SpinWidget* offset_owner = 0;
    if (offset_owner != this) {
        offset_owner = this;
        offset = 0;
    }
    offset += -e->delta()/120;
    if (QABS(offset) < 1)
        return;
    int ioff = int(offset);
    int i;
    for(i=0; i < QABS(ioff); i++)
        offset > 0 ? stepDown() : stepUp();
    offset -= ioff;
}
#endif

/*!

*/
void Q3SpinWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    QStyleOptionSpinBox opt = getStyleOption(this);

    if (d->theButton & 1)
        opt.activeSubControls = QStyle::SC_SpinBoxDown;
    else if (d->theButton & 2)
        opt.activeSubControls = QStyle::SC_SpinBoxUp;
    else
        opt.activeSubControls = QStyle::SC_None;
    opt.rect = style()->subControlRect(QStyle::CC_SpinBox, &opt, QStyle::SC_SpinBoxFrame, this);
    opt.subControls = QStyle::SC_All;
    style()->drawComplexControl(QStyle::CC_SpinBox, &opt, &p, this);
}


// ### What does the QEvent passed in contain? It used to be the previous style.
/*!
    The previous style is passed in \a ev.
*/
void Q3SpinWidget::changeEvent(QEvent *ev)
{
    if(ev->type() == QEvent::StyleChange) {
        arrange();
    } else if(ev->type() == QEvent::ActivationChange) {
        if (!isActiveWindow() && d->buttonDown) {         //was active, but lost focus
            d->stopTimer();
            d->buttonDown = 0;
            d->theButton = 0;
        }
    } else if(ev->type() == QEvent::EnabledChange) {
        d->upEnabled = isEnabled();
        d->downEnabled = isEnabled();
        updateDisplay();
    }
    QWidget::changeEvent(ev);
}

/*!
*/

QRect Q3SpinWidget::upRect() const
{
    return d->up;
}

/*!
*/

QRect Q3SpinWidget::downRect() const
{
    return d->down;
}

/*!
*/

void Q3SpinWidget::updateDisplay()
{
    if (!isEnabled()) {
        d->upEnabled = false;
        d->downEnabled = false;
    }
    if (d->theButton & 1 && (d->downEnabled) == 0) {
        d->theButton &= ~1;
        d->buttonDown &= ~1;
    }

    if (d->theButton & 2 && (d->upEnabled) == 0) {
        d->theButton &= ~2;
        d->buttonDown &= ~2;
    }
    repaint();
}

/*!
    Sets up-enabled to \a on.
*/

void Q3SpinWidget::setUpEnabled(bool on)
{
    if ((bool)d->upEnabled != on) {
        d->upEnabled = on;
        updateDisplay();
    }
}

/*!
*/

bool Q3SpinWidget::isUpEnabled() const
{
    return d->upEnabled;
}

/*!
    Sets down-enabled to \a on.
*/

void Q3SpinWidget::setDownEnabled(bool on)
{
    if ((bool)d->downEnabled != on) {
        d->downEnabled = on;
        updateDisplay();
    }
}

/*!
*/

bool Q3SpinWidget::isDownEnabled() const
{
    return d->downEnabled;
}

/*!
    Sets the button symbol to \a bs.
*/

void Q3SpinWidget::setButtonSymbols(ButtonSymbols bs)
{
    d->bsyms = bs;
}

/*!
*/

Q3SpinWidget::ButtonSymbols Q3SpinWidget::buttonSymbols() const
{
    return d->bsyms;
}

QT_END_NAMESPACE

#endif
