/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#include "qradiobutton.h"
#include "qapplication.h"
#include "qbitmap.h"
#include "qbuttongroup.h"
#include "qstylepainter.h"
#include "qstyle.h"
#include "qstyleoption.h"
#include "qevent.h"

#include "private/qabstractbutton_p.h"

QT_BEGIN_NAMESPACE

class QRadioButtonPrivate : public QAbstractButtonPrivate
{
    Q_DECLARE_PUBLIC(QRadioButton)

public:
    QRadioButtonPrivate() : QAbstractButtonPrivate(QSizePolicy::RadioButton), hovering(true) {}
    void init();
    uint hovering : 1;
};

/*
    Initializes the radio button.
*/
void QRadioButtonPrivate::init()
{
    Q_Q(QRadioButton);
    q->setCheckable(true);
    q->setAutoExclusive(true);
    q->setMouseTracking(true);
    q->setForegroundRole(QPalette::WindowText);
    setLayoutItemMargins(QStyle::SE_RadioButtonLayoutItem);
}

/*!
    \class QRadioButton
    \brief The QRadioButton widget provides a radio button with a text label.

    \ingroup basicwidgets
    \mainclass

    A QRadioButton is an option button that can be switched on (checked) or
    off (unchecked). Radio buttons typically present the user with a "one
    of many" choice. In a group of radio buttons only one radio button at
    a time can be checked; if the user selects another button, the
    previously selected button is switched off.

    Radio buttons are autoExclusive by default. If auto-exclusive is
    enabled, radio buttons that belong to the same parent widget
    behave as if they were part of the same exclusive button group. If
    you need multiple exclusive button groups for radio buttons that
    belong to the same parent widget, put them into a QButtonGroup.

    Whenever a button is switched on or off it emits the toggled() signal.
    Connect to this signal if you want to trigger an action each time the
    button changes state. Use isChecked() to see if a particular button is
    selected.

    Just like QPushButton, a radio button displays text, and
    optionally a small icon. The icon is set with setIcon(). The text
    can be set in the constructor or with setText(). A shortcut key
    can be specified by preceding the preferred character with an
    ampersand in the text. For example:

    \snippet doc/src/snippets/code/src.gui.widgets.qradiobutton.cpp 0

    In this example the shortcut is \e{Alt+c}. See the \l
    {QShortcut#mnemonic}{QShortcut} documentation for details (to
    display an actual ampersand, use '&&').

    Important inherited members: text(), setText(), text(),
    setDown(), isDown(), autoRepeat(), group(), setAutoRepeat(),
    toggle(), pressed(), released(), clicked(), and toggled().

    \table 100%
    \row \o \inlineimage plastique-radiobutton.png Screenshot of a Plastique radio button
         \o A radio button shown in the \l{Plastique Style Widget Gallery}{Plastique widget style}.
    \row \o \inlineimage windows-radiobutton.png Screenshot of a Windows XP radio button
         \o A radio button shown in the \l{Windows XP Style Widget Gallery}{Windows XP widget style}.
    \row \o \inlineimage macintosh-radiobutton.png Screenshot of a Macintosh radio button
         \o A radio button shown in the \l{Macintosh Style Widget Gallery}{Macintosh widget style}.
    \endtable

    \sa QPushButton, QToolButton, QCheckBox, {fowler}{GUI Design Handbook: Radio Button},
        {Group Box Example}
*/


/*!
    Constructs a radio button with the given \a parent, but with no text or
    pixmap.

    The \a parent argument is passed on to the QAbstractButton constructor.
*/

QRadioButton::QRadioButton(QWidget *parent)
    : QAbstractButton(*new QRadioButtonPrivate, parent)
{
    Q_D(QRadioButton);
    d->init();
}

/*!
    Constructs a radio button with the given \a parent and a \a text string.

    The \a parent argument is passed on to the QAbstractButton constructor.
*/

QRadioButton::QRadioButton(const QString &text, QWidget *parent)
    : QAbstractButton(*new QRadioButtonPrivate, parent)
{
    Q_D(QRadioButton);
    d->init();
    setText(text);
}

/*!
    Initialize \a option with the values from this QRadioButton. This method is useful
    for subclasses when they need a QStyleOptionButton, but don't want to fill
    in all the information themselves.

    \sa QStyleOption::initFrom()
*/
void QRadioButton::initStyleOption(QStyleOptionButton *option) const
{
    if (!option)
        return;
    Q_D(const QRadioButton);
    option->initFrom(this);
    option->text = d->text;
    option->icon = d->icon;
    option->iconSize = iconSize();
    if (d->down)
        option->state |= QStyle::State_Sunken;
    option->state |= (d->checked) ? QStyle::State_On : QStyle::State_Off;
    if (testAttribute(Qt::WA_Hover) && underMouse()) {
        if (d->hovering)
            option->state |= QStyle::State_MouseOver;
        else
            option->state &= ~QStyle::State_MouseOver;
    }
}

/*!
    \reimp
*/
QSize QRadioButton::sizeHint() const
{
    Q_D(const QRadioButton);
    if (d->sizeHint.isValid())
        return d->sizeHint;
    ensurePolished();
    QStyleOptionButton opt;
    initStyleOption(&opt);
    QSize sz = style()->itemTextRect(fontMetrics(), QRect(0, 0, 1, 1), Qt::TextShowMnemonic,
                                     false, text()).size();
    if (!opt.icon.isNull())
        sz = QSize(sz.width() + opt.iconSize.width() + 4, qMax(sz.height(), opt.iconSize.height()));
    d->sizeHint = (style()->sizeFromContents(QStyle::CT_RadioButton, &opt, sz, this).
                  expandedTo(QApplication::globalStrut()));
    return d->sizeHint;
}

/*!
    \reimp
*/
bool QRadioButton::hitButton(const QPoint &pos) const
{
    QStyleOptionButton opt;
    initStyleOption(&opt);
    return style()->subElementRect(QStyle::SE_RadioButtonClickRect, &opt, this).contains(pos);
}

/*!
    \reimp
*/
void QRadioButton::mouseMoveEvent(QMouseEvent *e)
{
    Q_D(QRadioButton);
    if (testAttribute(Qt::WA_Hover)) {
        bool hit = false;
        if (underMouse())
            hit = hitButton(e->pos());

        if (hit != d->hovering) {
            update();
            d->hovering = hit;
        }
    }

    QAbstractButton::mouseMoveEvent(e);
}

/*!\reimp
 */
void QRadioButton::paintEvent(QPaintEvent *)
{
    QStylePainter p(this);
    QStyleOptionButton opt;
    initStyleOption(&opt);
    p.drawControl(QStyle::CE_RadioButton, opt);
}

/*! \reimp */
bool QRadioButton::event(QEvent *e)
{
    Q_D(QRadioButton);
    if (e->type() == QEvent::StyleChange
#ifdef Q_WS_MAC
            || e->type() == QEvent::MacSizeChange
#endif
            )
        d->setLayoutItemMargins(QStyle::SE_RadioButtonLayoutItem);
    return QAbstractButton::event(e);
}

#ifdef QT3_SUPPORT
/*!
    Use one of the constructors that doesn't take the \a name
    argument and then use setObjectName() instead.
*/
QRadioButton::QRadioButton(QWidget *parent, const char* name)
    : QAbstractButton(*new QRadioButtonPrivate, parent)
{
    Q_D(QRadioButton);
    d->init();
    setObjectName(QString::fromAscii(name));
}

/*!
    Use one of the constructors that doesn't take the \a name
    argument and then use setObjectName() instead.
*/
QRadioButton::QRadioButton(const QString &text, QWidget *parent, const char* name)
    : QAbstractButton(*new QRadioButtonPrivate, parent)
{
    Q_D(QRadioButton);
    d->init();
    setObjectName(QString::fromAscii(name));
    setText(text);
}

#endif

QT_END_NAMESPACE
