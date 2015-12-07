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

#include "q3complexwidgets.h"

#include <q3header.h>
#include <private/q3titlebar_p.h>

#include <qapplication.h>

QT_BEGIN_NAMESPACE

Q3AccessibleHeader::Q3AccessibleHeader(QWidget *w)
    : QAccessibleWidget(w)
{
    Q_ASSERT(header());
    addControllingSignal(QLatin1String("clicked(int)"));
}

/*! Returns the Q3Header. */
Q3Header *Q3AccessibleHeader::header() const
{
    return qobject_cast<Q3Header*>(object());
}

/*! \reimp */
QRect Q3AccessibleHeader::rect(int child) const
{
    QPoint zero = header()->mapToGlobal(QPoint(0, 0));
    QRect sect = header()->sectionRect(child - 1);
    return QRect(sect.x() + zero.x(), sect.y() + zero.y(), sect.width(), sect.height());
}

/*! \reimp */
int Q3AccessibleHeader::childCount() const
{
    return header()->count();
}

/*! \reimp */
QString Q3AccessibleHeader::text(Text t, int child) const
{
    QString str;

    if (child <= childCount()) {
        switch (t) {
            case Name:
                str = header()->label(child - 1);
                break;
            case Description: {
                QAccessibleEvent event(QEvent::AccessibilityDescription, child);
                if (QApplication::sendEvent(widget(), &event))
                    str = event.value();
                break; }
            case Help: {
                QAccessibleEvent event(QEvent::AccessibilityHelp, child);
                if (QApplication::sendEvent(widget(), &event))
                    str = event.value();
                break; }
            default:
                break;
        }
    }
    if (str.isEmpty())
        str = QAccessibleWidget::text(t, child);;
    return str;
}

/*! \reimp */
QAccessible::Role Q3AccessibleHeader::role(int) const
{
    return (header()->orientation() == Qt::Horizontal) ? ColumnHeader : RowHeader;
}

/*! \reimp */
QAccessible::State Q3AccessibleHeader::state(int child) const
{
    State state = QAccessibleWidget::state(child);

    int section = child ? child - 1 : -1;
    if (!header()->isClickEnabled(section))
        state |= Unavailable;
    else
        state |= Selectable;
    if (child && section == header()->sortIndicatorSection())
        state |= Selected;
    if (header()->isResizeEnabled(section))
        state |= Sizeable;
    if (child && header()->isMovingEnabled())
        state |= Movable;
    return state;
}

/*!
  \class Q3AccessibleTitleBar qaccessiblewidget.h
  \brief The Q3AccessibleTitleBar class implements the QAccessibleInterface for title bars.
  \internal

  \ingroup accessibility
*/

/*!
  Constructs a Q3AccessibleTitleBar object for \a w.
*/
Q3AccessibleTitleBar::Q3AccessibleTitleBar(QWidget *w)
: QAccessibleWidget(w, TitleBar)
{
    Q_ASSERT(titleBar());
}

/*!
  Returns the title bar.
*/
Q3TitleBar *Q3AccessibleTitleBar::titleBar() const
{
    return qobject_cast<Q3TitleBar*>(object());
}

/*! \reimp */
QRect Q3AccessibleTitleBar::rect(int child) const
{
    if (!child)
        return QAccessibleWidget::rect(child);

    QStyle::SubControl sc;
    switch (child) {
    case 1:
        sc = QStyle::SC_TitleBarSysMenu;
        break;
    case 2:
        sc = QStyle::SC_TitleBarLabel;
        break;
    case 3:
        sc = QStyle::SC_TitleBarMinButton;
        break;
    case 4:
        sc = QStyle::SC_TitleBarMaxButton;
        break;
    case 5:
        sc = QStyle::SC_TitleBarCloseButton;
        break;
    default:
        sc = QStyle::SC_None;
        break;
    }

    QRect r;
    if (sc != QStyle::SC_None) {
        QStyleOptionTitleBar option;
        r = titleBar()->style()->subControlRect(QStyle::CC_TitleBar, &option, sc, titleBar());
    }

    QPoint tp = titleBar()->mapToGlobal(QPoint(0,0));
    return QRect(tp.x() + r.x(), tp.y() + r.y(), r.width(), r.height());
}

/* \reimp
int Q3AccessibleTitleBar::navigate(NavDirection direction, int startControl) const
{
    if (direction != NavFirstChild && direction != NavLastChild && direction != NavFocusChild && !startControl)
        return QAccessibleWidget::navigate(direction, startControl);

    switch (direction) {
    case NavFirstChild:
        return 1;
        break;
    case NavLastChild:
        return childCount();
        break;
    case NavNext:
    case NavRight:
        return startControl + 1 > childCount() ? -1 : startControl + 1;
    case NavPrevious:
    case NavLeft:
        return startControl -1 < 1 ? -1 : startControl - 1;
    default:
        break;
    }
    return -1;
}
*/

/*! \reimp */
int Q3AccessibleTitleBar::childCount() const
{
    if (!(titleBar()->windowFlags() & Qt::WStyle_SysMenu))
        return 0;
    int control = 3;
    if (!(titleBar()->windowFlags() & Qt::WStyle_Minimize))
        ++control;
    if (!(titleBar()->windowFlags() & Qt::WStyle_Maximize))
        ++control;
    return control;
}

/*! \reimp */
QString Q3AccessibleTitleBar::text(Text t, int child) const
{
    QString str = QAccessibleWidget::text(t, child);
    if (str.size())
        return str;

    QWidget *window = titleBar()->window();
    switch (t) {
    case Name:
        switch (child) {
        case 1:
            return Q3TitleBar::tr("System");
        case 3:
            if (window && window->isMinimized())
                return Q3TitleBar::tr("Restore up");
            return Q3TitleBar::tr("Minimize");
        case 4:
            if (window && window->isMaximized())
                return Q3TitleBar::tr("Restore down");
            return Q3TitleBar::tr("Maximize");
        case 5:
            return Q3TitleBar::tr("Close");
        default:
            break;
        }
        break;
    case Value:
        if (!child || child == 2)
            return window ? window->windowTitle() : QString();
        break;
/*
    case DefaultAction:
        if (child > 2)
            return Q3TitleBar::tr("Press");
        break;
*/
    case Description:
        switch (child) {
        case 1:
            return Q3TitleBar::tr("Contains commands to manipulate the window");
        case 3:
            if (window && window->isMinimized())
                return Q3TitleBar::tr("Puts a minimized back to normal");
            return Q3TitleBar::tr("Moves the window out of the way");
        case 4:
            if (window && window->isMaximized())
                return Q3TitleBar::tr("Puts a maximized window back to normal");
            return Q3TitleBar::tr("Makes the window full screen");
        case 5:
            return Q3TitleBar::tr("Closes the window");
        default:
            return Q3TitleBar::tr("Displays the name of the window and contains controls to manipulate it");
        }
    default:
        break;
    }
    return str;
}

/*! \reimp */
QAccessible::Role Q3AccessibleTitleBar::role(int child) const
{
    switch (child)
    {
    case 1:
    case 3:
    case 4:
    case 5:
        return PushButton;
    default:
        return TitleBar;
    }
}

/*! \reimp */
QAccessible::State Q3AccessibleTitleBar::state(int child) const
{
    return QAccessibleWidget::state(child);
}

/*! \reimp */
bool Q3AccessibleTitleBar::doAction(int, int child, const QVariantList &)
{
    switch (child) {
    case 3:
        if (titleBar()->window()->isMinimized())
            titleBar()->window()->showNormal();
        else
            titleBar()->window()->showMinimized();
        return true;
    case 4:
        if (titleBar()->window()->isMaximized())
            titleBar()->window()->showNormal();
        else
            titleBar()->window()->showMaximized();
        return true;
    case 5:
        titleBar()->window()->close();
        return true;
    default:
        break;
    }
    return false;
}

QT_END_NAMESPACE
