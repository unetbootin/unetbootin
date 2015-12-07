/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the plugins of the Qt Toolkit.
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

#ifndef RANGECONTROLS_H
#define RANGECONTROLS_H

#include <QtGui/qaccessiblewidget.h>
#include <QtGui/qaccessible2.h>

QT_BEGIN_NAMESPACE

#ifndef QT_NO_ACCESSIBILITY

class QAbstractSpinBox;
class QAbstractSlider;
class QScrollBar;
class QSlider;
class QSpinBox;
class QDoubleSpinBox;
class QDial;

#ifndef QT_NO_SPINBOX
class QAccessibleAbstractSpinBox: public QAccessibleWidgetEx, public QAccessibleValueInterface
{
public:
    explicit QAccessibleAbstractSpinBox(QWidget *w);

    enum SpinBoxElements {
        SpinBoxSelf        = 0,
        Editor,
        ValueUp,
        ValueDown
    };

    int childCount() const;
    QRect rect(int child) const;

    int navigate(RelationFlag rel, int entry, QAccessibleInterface **target) const;

    QString text(Text t, int child) const;
    Role role(int child) const;

    bool doAction(int action, int child, const QVariantList &params);

    QVariant invokeMethodEx(Method method, int child, const QVariantList &params);

    // QAccessibleValueInterface
    QVariant currentValue();
    void setCurrentValue(const QVariant &value);
    QVariant maximumValue();
    QVariant minimumValue();

protected:
    QAbstractSpinBox *abstractSpinBox() const;
};

class QAccessibleSpinBox : public QAccessibleAbstractSpinBox
{
public:
    explicit QAccessibleSpinBox(QWidget *w);

    State state(int child) const;

    bool doAction(int action, int child, const QVariantList &params);

protected:
    QSpinBox *spinBox() const;
};

class QAccessibleDoubleSpinBox : public QAccessibleWidgetEx
{
public:
    explicit QAccessibleDoubleSpinBox(QWidget *widget);

    enum DoubleSpinBoxElements {
        SpinBoxSelf = 0,
        Editor,
        ValueUp,
        ValueDown
    };

    int childCount() const;
    QRect rect(int child) const;
    int navigate(RelationFlag rel, int entry, QAccessibleInterface **target) const;
    QVariant invokeMethodEx(QAccessible::Method method, int child, const QVariantList &params);
    QString text(Text t, int child) const;
    Role role(int child) const;
    State state(int child) const;

protected:
    QDoubleSpinBox *doubleSpinBox() const;
};
#endif // QT_NO_SPINBOX

class QAccessibleAbstractSlider: public QAccessibleWidgetEx, public QAccessibleValueInterface
{
public:
    explicit QAccessibleAbstractSlider(QWidget *w, Role r = Slider);

    QVariant invokeMethodEx(Method method, int child, const QVariantList &params);

    // QAccessibleValueInterface
    QVariant currentValue();
    void setCurrentValue(const QVariant &value);
    QVariant maximumValue();
    QVariant minimumValue();

protected:
    QAbstractSlider *abstractSlider() const;
};

#ifndef QT_NO_SCROLLBAR
class QAccessibleScrollBar : public QAccessibleAbstractSlider
{
public:
    explicit QAccessibleScrollBar(QWidget *w);

    enum ScrollBarElements {
        ScrollBarSelf        = 0,
        LineUp,
        PageUp,
        Position,
        PageDown,
        LineDown
    };

    int childCount() const;

    QRect rect(int child) const;
    QString text(Text t, int child) const;
    Role role(int child) const;
    State state(int child) const;

protected:
    QScrollBar *scrollBar() const;
};
#endif // QT_NO_SCROLLBAR

#ifndef QT_NO_SLIDER
class QAccessibleSlider : public QAccessibleAbstractSlider
{
public:
    explicit QAccessibleSlider(QWidget *w);

    enum SliderElements {
        SliderSelf  = 0,
        PageLeft,
        Position,
        PageRight
    };

    int childCount() const;

    QRect rect(int child) const;
    QString text(Text t, int child) const;
    Role role(int child) const;
    State state(int child) const;

    int defaultAction(int child) const;
    QString actionText(int action, Text t, int child) const;

protected:
    QSlider *slider() const;
};
#endif // QT_NO_SLIDER

#ifndef QT_NO_DIAL
class QAccessibleDial : public QAccessibleWidgetEx
{
public:
    explicit QAccessibleDial(QWidget *w);

    enum DialElements {
        Self  = 0,
        SpeedoMeter,
        SliderHandle
    };

    int childCount() const;
    QRect rect(int child) const;
    QString text(Text textType, int child) const;
    Role role(int child) const;
    State state(int child) const;
    QVariant invokeMethodEx(Method method, int child, const QVariantList &params);

protected:
    QDial *dial() const;
};
#endif // QT_NO_DIAL

#endif // QT_NO_ACCESSIBILITY

QT_END_NAMESPACE

#endif // RANGECONTROLS_H
