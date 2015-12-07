/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the documentation of the Qt Toolkit.
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

QAbstractSliderPrivate::QAbstractSliderPrivate()
    : minimum(0), maximum(99), singleStep(1), pageStep(10),
      value(0), position(0), pressValue(-1), tracking(true), blocktracking(false), pressed(false),
      invertedAppearance(false), invertedControls(false),
      orientation(Qt::Horizontal), repeatAction(QAbstractSlider::SliderNoAction)
{
}

QAbstractSliderPrivate::~QAbstractSliderPrivate()
{
}

oid QAbstractSlider::setRange(int min, int max)
{
    Q_D(QAbstractSlider);
    int oldMin = d->minimum;
    int oldMax = d->maximum;
    d->minimum = min;
    d->maximum = qMax(min, max);
    if (oldMin != d->minimum || oldMax != d->maximum) {
        sliderChange(SliderRangeChange);
        emit rangeChanged(d->minimum, d->maximum);
        setValue(d->value); // re-bound
    }
}


void QAbstractSliderPrivate::setSteps(int single, int page)
{
    Q_Q(QAbstractSlider);
    singleStep = qAbs(single);
    pageStep = qAbs(page);
    q->sliderChange(QAbstractSlider::SliderStepsChange);
}

AbstractSlider::QAbstractSlider(QWidget *parent)
    :QWidget(*new QAbstractSliderPrivate, parent, 0)
{
}

QAbstractSlider::QAbstractSlider(QAbstractSliderPrivate &dd, QWidget *parent)
    :QWidget(dd, parent, 0)
{
}

QAbstractSlider::~QAbstractSlider()
{
}

void QAbstractSlider::setOrientation(Qt::Orientation orientation)
{
    Q_D(QAbstractSlider);
    if (d->orientation == orientation)
        return;

    d->orientation = orientation;
    if (!testAttribute(Qt::WA_WState_OwnSizePolicy)) {
        QSizePolicy sp = sizePolicy();
        sp.transpose();
        setSizePolicy(sp);
        setAttribute(Qt::WA_WState_OwnSizePolicy, false);
    }
    update();
    updateGeometry();
}

Qt::Orientation QAbstractSlider::orientation() const
{
    Q_D(const QAbstractSlider);
    return d->orientation;
}


void QAbstractSlider::setMinimum(int min)
{
    Q_D(QAbstractSlider);
    setRange(min, qMax(d->maximum, min));
}

int QAbstractSlider::minimum() const
{
    Q_D(const QAbstractSlider);
    return d->minimum;
}


void QAbstractSlider::setMaximum(int max)
{
    Q_D(QAbstractSlider);
    setRange(qMin(d->minimum, max), max);
}

int QAbstractSlider::maximum() const
{
    Q_D(const QAbstractSlider);
    return d->maximum;
}



void QAbstractSlider::setSingleStep(int step)
{
    Q_D(QAbstractSlider);
    d->setSteps(step, d->pageStep);
}

int QAbstractSlider::singleStep() const
{
    Q_D(const QAbstractSlider);
    return d->singleStep;
}


void QAbstractSlider::setPageStep(int step)
{
    Q_D(QAbstractSlider);
    d->setSteps(d->singleStep, step);
}

int QAbstractSlider::pageStep() const
{
    Q_D(const QAbstractSlider);
    return d->pageStep;
}

oid QAbstractSlider::setTracking(bool enable)
{
    Q_D(QAbstractSlider);
    d->tracking = enable;
}

bool QAbstractSlider::hasTracking() const
{
    Q_D(const QAbstractSlider);
    return d->tracking;
}



void QAbstractSlider::setSliderDown(bool down)
{
    Q_D(QAbstractSlider);
    bool doEmit = d->pressed != down;

    d->pressed = down;

    if (doEmit) {
        if (down)
            emit sliderPressed();
        else
            emit sliderReleased();
    }

    if (!down && d->position != d->value)
        triggerAction(SliderMove);
}

bool QAbstractSlider::isSliderDown() const
{
    Q_D(const QAbstractSlider);
    return d->pressed;
}


void QAbstractSlider::setSliderPosition(int position)
{
    Q_D(QAbstractSlider);
    position = d->bound(position);
    if (position == d->position)
        return;
    d->position = position;
    if (!d->tracking)
        update();
    if (d->pressed)
        emit sliderMoved(position);
    if (d->tracking && !d->blocktracking)
        triggerAction(SliderMove);
}

int QAbstractSlider::sliderPosition() const
{
    Q_D(const QAbstractSlider);
    return d->position;
}



int QAbstractSlider::value() const
{
    Q_D(const QAbstractSlider);
    return d->value;
}

//! [0]
void QAbstractSlider::setValue(int value)
//! [0]
{
    Q_D(QAbstractSlider);
    value = d->bound(value);
    if (d->value == value && d->position == value)
        return;
    d->value = value;
    if (d->position != value) {
        d->position = value;
        if (d->pressed)
            emit sliderMoved((d->position = value));
    }
#ifndef QT_NO_ACCESSIBILITY
//! [1]
    QAccessible::updateAccessibility(this, 0, QAccessible::ValueChanged);
//! [1]
#endif
    sliderChange(SliderValueChange);
    emit valueChanged(value);
//! [2]
}
//! [2]

bool QAbstractSlider::invertedAppearance() const
{
    Q_D(const QAbstractSlider);
    return d->invertedAppearance;
}

void QAbstractSlider::setInvertedAppearance(bool invert)
{
    Q_D(QAbstractSlider);
    d->invertedAppearance = invert;
    update();
}



bool QAbstractSlider::invertedControls() const
{
    Q_D(const QAbstractSlider);
    return d->invertedControls;
}

void QAbstractSlider::setInvertedControls(bool invert)
{
    Q_D(QAbstractSlider);
    d->invertedControls = invert;
}

void QAbstractSlider::triggerAction(SliderAction action)
{
    Q_D(QAbstractSlider);
    d->blocktracking = true;
    switch (action) {
    case SliderSingleStepAdd:
        setSliderPosition(d->value + d->singleStep);
        break;
    case SliderSingleStepSub:
        setSliderPosition(d->value - d->singleStep);
        break;
    case SliderPageStepAdd:
        setSliderPosition(d->value + d->pageStep);
        break;
    case SliderPageStepSub:
        setSliderPosition(d->value - d->pageStep);
        break;
    case SliderToMinimum:
        setSliderPosition(d->minimum);
        break;
    case SliderToMaximum:
        setSliderPosition(d->maximum);
        break;
    case SliderMove:
    case SliderNoAction:
        break;
    };
    emit actionTriggered(action);
    d->blocktracking = false;
    setValue(d->position);
}

void QAbstractSlider::setRepeatAction(SliderAction action, int thresholdTime, int repeatTime)
{
    Q_D(QAbstractSlider);
    if ((d->repeatAction = action) == SliderNoAction) {
        d->repeatActionTimer.stop();
    } else {
        d->repeatActionTime = repeatTime;
        d->repeatActionTimer.start(thresholdTime, this);
    }
}

QAbstractSlider::SliderAction QAbstractSlider::repeatAction() const
{
    Q_D(const QAbstractSlider);
    return d->repeatAction;
}

void QAbstractSlider::timerEvent(QTimerEvent *e)
{
    Q_D(QAbstractSlider);
    if (e->timerId() == d->repeatActionTimer.timerId()) {
        if (d->repeatActionTime) { // was threshold time, use repeat time next time
            d->repeatActionTimer.start(d->repeatActionTime, this);
            d->repeatActionTime = 0;
        }
        if (d->repeatAction == SliderPageStepAdd)
            d->setAdjustedSliderPosition(d->value + d->pageStep);
        else if (d->repeatAction == SliderPageStepSub)
            d->setAdjustedSliderPosition(d->value - d->pageStep);
        else
            triggerAction(d->repeatAction);
    }
}

oid QAbstractSlider::sliderChange(SliderChange)
{
    update();
}


#ifndef QT_NO_WHEELEVENT
void QAbstractSlider::wheelEvent(QWheelEvent * e)
{
    Q_D(QAbstractSlider);
    e->ignore();
    if (e->orientation() != d->orientation && !rect().contains(e->pos()))
        return;

    static qreal offset = 0;
    static QAbstractSlider *offset_owner = 0;
    if (offset_owner != this){
        offset_owner = this;
        offset = 0;
    }

    int step = qMin(QApplication::wheelScrollLines() * d->singleStep, d->pageStep);
    if ((e->modifiers() & Qt::ControlModifier) || (e->modifiers() & Qt::ShiftModifier))
        step = d->pageStep;
    offset += e->delta() * step / 120;
    if (d->invertedControls)
        offset = -offset;

    if (qAbs(offset) < 1)
        return;

    int prevValue = d->value;
    d->position = d->value + int(offset); // value will be updated by triggerAction()
    triggerAction(SliderMove);
    if (prevValue == d->value) {
        offset = 0;
    } else {
        offset -= int(offset);
        e->accept();
    }
}
#endif
void QAbstractSlider::keyPressEvent(QKeyEvent *ev)
{
    Q_D(QAbstractSlider);
    SliderAction action = SliderNoAction;
    switch (ev->key()) {
#ifdef QT_KEYPAD_NAVIGATION
        case Qt::Key_Select:
            if (QApplication::keypadNavigationEnabled())
                setEditFocus(!hasEditFocus());
            else
                ev->ignore();
            break;
        case Qt::Key_Back:
            if (QApplication::keypadNavigationEnabled() && hasEditFocus()) {
                setValue(d->origValue);
                setEditFocus(false);
            } else
                ev->ignore();
            break;
#endif

        // It seems we need to use invertedAppearance for Left and right, otherwise, things look weird.
        case Qt::Key_Left:
#ifdef QT_KEYPAD_NAVIGATION
            if (QApplication::keypadNavigationEnabled() && !hasEditFocus()) {
                ev->ignore();
                return;
            }
            if (QApplication::keypadNavigationEnabled() && d->orientation == Qt::Vertical)
                action = d->invertedControls ? SliderSingleStepSub : SliderSingleStepAdd;
            else
#endif
            action = !d->invertedAppearance ? SliderSingleStepSub : SliderSingleStepAdd;
            break;
        case Qt::Key_Right:
#ifdef QT_KEYPAD_NAVIGATION
            if (QApplication::keypadNavigationEnabled() && !hasEditFocus()) {
                ev->ignore();
                return;
            }
            if (QApplication::keypadNavigationEnabled() && d->orientation == Qt::Vertical)
                action = d->invertedControls ? SliderSingleStepAdd : SliderSingleStepSub;
            else
#endif
            action = !d->invertedAppearance ? SliderSingleStepAdd : SliderSingleStepSub;
            break;
        case Qt::Key_Up:
#ifdef QT_KEYPAD_NAVIGATION
            if (QApplication::keypadNavigationEnabled()) {
                ev->ignore();
                break;
            }
#endif
            action = d->invertedControls ? SliderSingleStepSub : SliderSingleStepAdd;
            break;
        case Qt::Key_Down:
#ifdef QT_KEYPAD_NAVIGATION
            if (QApplication::keypadNavigationEnabled()) {
                ev->ignore();
                break;
            }
#endif
            action = d->invertedControls ? SliderSingleStepAdd : SliderSingleStepSub;
            break;
        case Qt::Key_PageUp:
            action = d->invertedControls ? SliderPageStepSub : SliderPageStepAdd;
            break;
        case Qt::Key_PageDown:
            action = d->invertedControls ? SliderPageStepAdd : SliderPageStepSub;
            break;
        case Qt::Key_Home:
            action = SliderToMinimum;
            break;
        case Qt::Key_End:
            action = SliderToMaximum;
            break;
        default:
            ev->ignore();
            break;
    }
    if (action)
        triggerAction(action);
}

void QAbstractSlider::changeEvent(QEvent *ev)
{
    Q_D(QAbstractSlider);
    switch (ev->type()) {
    case QEvent::EnabledChange:
        if (!isEnabled()) {
            d->repeatActionTimer.stop();
            setSliderDown(false);
        }
        // fall through...
    default:
        QWidget::changeEvent(ev);
    }
}

bool QAbstractSlider::event(QEvent *e)
{
#ifdef QT_KEYPAD_NAVIGATION
    Q_D(QAbstractSlider);
    switch (e->type()) {
    case QEvent::FocusIn:
        d->origValue = d->value;
        break;
    default:
        break;
    }
#endif
    
    return QWidget::event(e);
}


