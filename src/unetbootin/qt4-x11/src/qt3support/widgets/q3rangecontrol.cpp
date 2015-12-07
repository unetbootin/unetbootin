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
#ifndef QT_NO_RANGECONTROL
#include "qglobal.h"
#include <limits.h>

QT_BEGIN_NAMESPACE

/*!
    \class Q3RangeControl qrangecontrol.h
    \brief The Q3RangeControl class provides an integer value within a range.

    \compat

    Although originally designed for the QScrollBar widget, the
    Q3RangeControl can also be used in conjunction with other widgets
    such as QSlider and QSpinBox. Here are the five main concepts in
    the class:

    \list 1

    \i \e{Current value} The bounded integer that
    Q3RangeControl maintains. value() returns it, and several
    functions, including setValue(), set it.

    \i \e{Minimum} The lowest value that value() can ever
    return. Returned by minValue() and set by setRange() or one of the
    constructors.

    \i \e{Maximum} The highest value that value() can ever
    return. Returned by maxValue() and set by setRange() or one of the
    constructors.

    \i \e{Line step} The smaller of two natural steps that
    Q3RangeControl provides and typically corresponds to the user
    pressing an arrow key. The line step is returned by lineStep()
    and set using setSteps(). The functions addLine() and
    subtractLine() respectively increment and decrement the current
    value by lineStep().

    \i \e{Page step} The larger of two natural steps that
    Q3RangeControl provides and typically corresponds to the user
    pressing PageUp or PageDown. The page step is returned by
    pageStep() and set using setSteps(). The functions addPage() and
    substractPage() respectively increment and decrement the current
    value by pageStep().

    \endlist

    Unity (1) may be viewed as a third step size. setValue() lets you
    set the current value to any integer in the allowed range, not
    just minValue() + \e n * lineStep() for integer values of \e n.
    Some widgets may allow the user to set any value at all; others
    may just provide multiples of lineStep() or pageStep().

    Q3RangeControl provides three virtual functions that are well
    suited for updating the on-screen representation of range controls
    and emitting signals: valueChange(), rangeChange() and
    stepChange().

    Q3RangeControl also provides a function called bound() which lets
    you force arbitrary integers to be within the allowed range of the
    range control.

    We recommend that all widgets that inherit Q3RangeControl provide
    at least a signal called valueChanged(); many widgets will want to
    provide addStep(), addPage(), substractStep() and substractPage()
    as slots.

    Note that you must use multiple inheritance if you plan to
    implement a widget using Q3RangeControl because Q3RangeControl is
    not derived from QWidget.
*/


/*!
    Constructs a range control with a minimum value of 0, maximum
    value of 99, line step of 1, page step of 10 and initial value 0.
*/

Q3RangeControl::Q3RangeControl()
{
    minVal  = 0;
    maxVal  = 99;
    line    = 1;
    page    = 10;
    val            = 0;
    prevVal = -1;
    d            = 0;
}

/*!
    Constructs a range control whose value can never be smaller than
    \a minValue or greater than \a maxValue, whose line step size is
    \a lineStep and page step size is \a pageStep and whose value is
    initially \a value (which is guaranteed to be in range using
    bound()).
*/

Q3RangeControl::Q3RangeControl(int minValue, int maxValue,
                              int lineStep, int pageStep,
                              int value)
{
    minVal  = minValue;
    maxVal  = maxValue;
    line    = QABS(lineStep);
    page    = QABS(pageStep);
    prevVal = minVal - 1;
    val            = bound(value);
    d            = 0;
}

/*!
    Destroys the range control
*/

Q3RangeControl::~Q3RangeControl()
{
}


/*!
    \fn int Q3RangeControl::value() const

    Returns the current range control value. This is guaranteed to be
    within the range [minValue(), maxValue()].

    \sa setValue() prevValue()
*/

/*!
    \fn int Q3RangeControl::prevValue() const

    Returns the previous value of the range control. "Previous value"
    means the value before the last change occurred. Setting a new
    range may affect the value, too, because the value is forced to be
    inside the specified range. When the range control is initially
    created, this is the same as value().

    prevValue() can be outside the current legal range if a call to
    setRange() causes the current value to change. For example, if the
    range was [0, 1000] and the current value is 500, setRange(0, 400)
    makes value() return 400 and prevValue() return 500.

    \sa value() setRange()
*/

/*!
    Sets the range control's value to \a value and forces it to be
    within the legal range.

    Calls the virtual valueChange() function if the new value is
    different from the previous value. The old value can still be
    retrieved using prevValue().

    \sa value()
*/

void Q3RangeControl::setValue(int value)
{
    directSetValue(value);
    if (prevVal != val)
        valueChange();
}

/*!
    Sets the range control \a value directly without calling
    valueChange().

    Forces the new \a value to be within the legal range.

    You will rarely have to call this function. However, if you want
    to change the range control's value inside the overloaded method
    valueChange(), setValue() would call the function valueChange()
    again. To avoid this recursion you must use directSetValue()
    instead.

    \sa setValue()
*/

void Q3RangeControl::directSetValue(int value)
{
    prevVal = val;
    val = bound(value);
}

/*!
    Equivalent to \c{setValue(value() + pageStep())}.

    If the value is changed, then valueChange() is called.

    \sa subtractPage() addLine() setValue()
*/

void Q3RangeControl::addPage()
{
    setValue(value() + pageStep());
}

/*!
    Equivalent to \c{setValue(value() - pageStep())}.

    If the value is changed, then valueChange() is called.

    \sa addPage() subtractLine() setValue()
*/

void Q3RangeControl::subtractPage()
{
    setValue(value() - pageStep());
}

/*!
    Equivalent to \c{setValue(value() + lineStep())}.

    If the value is changed, then valueChange() is called.

    \sa subtractLine() addPage() setValue()
*/

void Q3RangeControl::addLine()
{
    setValue(value() + lineStep());
}

/*!
    Equivalent to \c{setValue(value() - lineStep())}.

    If the value is changed, then valueChange() is called.

    \sa addLine() subtractPage() setValue()
*/

void Q3RangeControl::subtractLine()
{
    setValue(value() - lineStep());
}


/*!
    \fn int Q3RangeControl::minValue() const

    Returns the minimum value of the range.

    \sa setMinValue() setRange() maxValue()
*/

/*!
    \fn int Q3RangeControl::maxValue() const

    Returns the maximum value of the range.

    \sa setMaxValue() setRange() minValue()
*/

/*!
    Sets the minimum value of the range to \a minVal.

    If necessary, the maxValue() is adjusted so that the range remains
    valid.

    \sa minValue() setMaxValue()
*/
void Q3RangeControl::setMinValue(int minVal)
{
    int maxVal = maxValue();
    if (maxVal < minVal)
        maxVal = minVal;
    setRange(minVal, maxVal);
}

/*!
    Sets the minimum value of the range to \a maxVal.

    If necessary, the minValue() is adjusted so that the range remains
    valid.

    \sa maxValue() setMinValue()
*/
void Q3RangeControl::setMaxValue(int maxVal)
{
    int minVal = minValue();
    if (minVal > maxVal)
        minVal = maxVal;
    setRange(minVal, maxVal);
}

/*!
    Sets the range control's minimum value to \a minValue and its
    maximum value to \a maxValue.

    Calls the virtual rangeChange() function if one or both of the new
    minimum and maximum values are different from the previous
    setting. Calls the virtual valueChange() function if the current
    value is adjusted because it was outside the new range.

    If \a maxValue is smaller than \a minValue, \a minValue becomes
    the only legal value.

    \sa minValue() maxValue()
*/

void Q3RangeControl::setRange(int minValue, int maxValue)
{
    if (minValue > maxValue) {
        qWarning("Q3RangeControl::setRange: minValue %d > maxValue %d",
                  minValue, maxValue);
        maxValue = minValue;
    }
    if (minValue == minVal && maxValue == maxVal)
        return;
    minVal = minValue;
    maxVal = maxValue;
    int tmp = bound(val);
    rangeChange();
    if (tmp != val) {
        prevVal = val;
        val = tmp;
        valueChange();
    }
}


/*!
    \fn int Q3RangeControl::lineStep() const

    Returns the line step.

    \sa setSteps() pageStep()
*/

/*!
    \fn int Q3RangeControl::pageStep() const

    Returns the page step.

    \sa setSteps() lineStep()
*/

/*!
    Sets the range's line step to \a lineStep and page step to \a
    pageStep.

    Calls the virtual stepChange() function if the new line step
    or page step are different from the previous settings.

    \sa lineStep() pageStep() setRange()
*/

void Q3RangeControl::setSteps(int lineStep, int pageStep)
{
    if (lineStep != line || pageStep != page) {
        line = QABS(lineStep);
        page = QABS(pageStep);
        stepChange();
    }
}


/*!
    This virtual function is called whenever the range control value
    changes. You can reimplement it if you want to be notified when
    the value changes. The default implementation does nothing.

    Note that this method is called after the value has changed. The
    previous value can be retrieved using prevValue().

    \sa setValue(), addPage(), subtractPage(), addLine(),
    subtractLine() rangeChange(), stepChange()
*/

void Q3RangeControl::valueChange()
{
}


/*!
    This virtual function is called whenever the range control's range
    changes. You can reimplement it if you want to be notified when
    the range changes. The default implementation does nothing.

    Note that this method is called after the range has changed.

    \sa setRange(), valueChange(), stepChange()
*/

void Q3RangeControl::rangeChange()
{
}


/*!
    This virtual function is called whenever the range control's
    line or page step settings change. You can reimplement it if you
    want to be notified when the step changes. The default
    implementation does nothing.

    Note that this method is called after a step setting has changed.

    \sa setSteps(), rangeChange(), valueChange()
*/

void Q3RangeControl::stepChange()
{
}


/*!
    Forces the value \a v to be within the range from minValue() to
    maxValue() inclusive, and returns the result.

    This function is provided so that you can easily force other
    numbers than value() into the allowed range. You do not need to
    call it in order to use Q3RangeControl itself.

    \sa setValue() value() minValue() maxValue()
*/

int Q3RangeControl::bound(int v) const
{
    if (v < minVal)
        return minVal;
    if (v > maxVal)
        return maxVal;
    return v;
}


/*!
    Converts \a logical_val to a pixel position. minValue() maps to 0,
    maxValue() maps to \a span and other values are distributed evenly
    in-between.

    This function can handle the entire integer range without
    overflow, providing \a span is \<= 4096.

    Calling this method is useful when actually drawing a range
    control such as a QScrollBar on-screen.

    \sa valueFromPosition()
*/

int Q3RangeControl::positionFromValue(int logical_val, int span) const
{
    if (span <= 0 || logical_val < minValue() || maxValue() <= minValue())
        return 0;
    if (logical_val > maxValue())
        return span;

    uint range = maxValue() - minValue();
    uint p = logical_val - minValue();

    if (range > (uint)INT_MAX/4096) {
        const int scale = 4096*2;
        return ((p/scale) * span) / (range/scale);
        // ### the above line is probably not 100% correct
        // ### but fixing it isn't worth the extreme pain...
    } else if (range > (uint)span) {
        return (2*p*span + range) / (2*range);
    } else {
        uint div = span / range;
        uint mod = span % range;
        return p*div + (2*p*mod + range) / (2*range);
    }
    //equiv. to (p*span)/range + 0.5
    // no overflow because of this implicit assumption:
    // span <= 4096
}


/*!
    Converts the pixel position \a pos to a value. 0 maps to
    minValue(), \a span maps to maxValue() and other values are
    distributed evenly in-between.

    This function can handle the entire integer range without
    overflow.

    Calling this method is useful if you actually implemented a range
    control widget such as QScrollBar and want to handle mouse press
    events. This function then maps screen coordinates to the logical
    values.

    \sa positionFromValue()
*/

int Q3RangeControl::valueFromPosition(int pos, int span) const
{
    if (span <= 0 || pos <= 0)
        return minValue();
    if (pos >= span)
        return maxValue();

    uint range = maxValue() - minValue();

    if ((uint)span > range)
        return  minValue() + (2*pos*range + span) / (2*span);
    else {
        uint div = range / span;
        uint mod = range % span;
        return  minValue() + pos*div + (2*pos*mod + span) / (2*span);
    }
    // equiv. to minValue() + (pos*range)/span + 0.5
    // no overflow because of this implicit assumption:
    // pos <= span < sqrt(INT_MAX+0.0625)+0.25 ~ sqrt(INT_MAX)
}

QT_END_NAMESPACE

#endif // QT_NO_RANGECONTROL
