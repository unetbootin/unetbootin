/****************************************************************************
**
** Copyright (C) 2006-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt Linguist of the Qt Toolkit.
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

#include "qtwindowlistmenu.h"

#include <QString>
#include <QWorkspace>
#include <QObjectList>
#include <QMenuBar>
#include <QList>
#include <QEvent>
#include <QTextStream>

#include <stdio.h>

QT_BEGIN_NAMESPACE


/*!
    \class QtWindowListMenu

    \brief The QtWindowListMenu class is a menu that displays
    the names of the windows in a QWorkspace.

    This class is intended to be used in the "Windows" item in an MDI
    application's menubar. It keeps track of the workspace and updates
    its own contents automatically, noting when windows are added,
    removed or activated. When there are no windows on the workspace it
    disables itself. Additionally, it provides the standard Window
    menu entries: "Tile", "Cascade", "Close" and "Close All".

    The QtWindowListMenu is added to the menubar (or any descendant of
    QMenuBar) using addTo().

    This class inherits publicly from QMenu, so the items
    "Tile", "Cascade", "Close" and "Close All" may be modified
    or removed, and new items may be added. However, removing menu items
    which correspond to windows on the workspace results in undefined
    behavior.
*/

/*!
    Constructs a QtWindowListMenu object. The \a parent
    parameter is passed to the QMenu constructor, and the \a name parameter
    is set as the object name. The
    \a workspace parameter specifies which QWorkspace object the menu
    must keep track of. If it is 0, the QtWindowListMenu will not
    update itself automatically, although it will still keep track of the
    state of any windows specified manually using
    addWindow().
*/

QtWindowListMenu::QtWindowListMenu(QWorkspace *workspace, QWidget *parent,
					    const char *name)
	: QMenu(parent), groupWindows(this)
{
    setObjectName(QLatin1String(name));
    m_menubar = 0;
    m_my_action = 0;
    m_close_current_action = 0;
    m_workspace = workspace;
    groupWindows.setExclusive(false);

    if (m_workspace != 0) {
	    m_workspace->installEventFilter(this);
        connect(m_workspace, SIGNAL(windowActivated(QWidget*)),
                this, SLOT(addWindow(QWidget*)));

	    m_tile_action = addAction(tr("Tile"), m_workspace, SLOT(tile()));
	    m_cascade_action = addAction(tr("Cascade"), m_workspace, SLOT(cascade()));
	    addSeparator();
	    m_close_current_action = addAction(tr("Close"), m_workspace, SLOT(closeActiveWindow()));
	    m_close_current_action->setEnabled(false);
	    m_close_all_action = addAction(tr("Close All"), m_workspace, SLOT(closeAllWindows()));
	    addSeparator();
    }
}

/*
    If \a on is false, the window titles in the menu will have the form:

    \snippet doc/src/snippets/code/tools.linguist.linguist.previewtool.qtwindowlistmenu.cpp 0

    If \a on is true and \a amperstands if false, the window titles in the menu
    will have the form:

    \snippet doc/src/snippets/code/tools.linguist.linguist.previewtool.qtwindowlistmenu.cpp 1

    If both \a on and \a amperstands are true, the window titles in the menu
    will have the form:

    \snippet doc/src/snippets/code/tools.linguist.linguist.previewtool.qtwindowlistmenu.cpp 2

    Calling this method has no effect on window titles that are already in the
    menu at the time when the call is made. It should therefore be called
    right after the QtWindowListMenu is created, before any windows are
    put on the QWorkspace.

void QtWindowListMenu::setNumbering(bool on, bool amperstands)
{
    m_numbering = on;
    m_amperstands = amperstands;
}

*/

/*!
    Adds this window list menu to \a menubar at position \a idx, with the
    name \a text. If \a idx is -1, the window list menu is appended
    as the last item on the menubar. Does nothing and returns null if this
    menu is already assigned to a menubar. Otherwise returns a pointer to
    the QAction object for the inserted menu item.
*/

QAction *QtWindowListMenu::addTo(const QString &text, QMenuBar *menubar, int idx)
{
    if (menubar == 0 || m_menubar != 0)
    	return 0;
    m_menubar = menubar;

    QList<QAction *> actions = m_menubar->actions();

    QAction *action = 0;
    if (idx >= 0 && idx < actions.size())
        action = actions[idx];

    setTitle(text);
    m_my_action = m_menubar->insertMenu(action, this);
    setEnabled(false);

    return m_my_action;
}

/*!
    Removes the widget \a w from this window list menu's list of
    widgets. If \a windowDestroyed is true, QtWindowListMenu assumes
    that \a w is a pointer to a destroyed object, and will not call
    \a w's functions. \a windowDestroyed has a default value of false.
    Normally it's not necessary to call this method, as
    QtWindowListMenu automatically removes windows from the list when
    they recieve a QCloseEvent.
*/

void QtWindowListMenu::removeWindow(QWidget *w, bool windowDestroyed)
{
    WindowList::iterator it = m_window_list.find(w);
    if (it == m_window_list.end())
    	return;

    /* We don't want call any of w's methods if removeWindow() is being
	    called in response to w's destroyed() signal. */
    if (!windowDestroyed) {
	    w->removeEventFilter(this);
	    disconnect(w, 0, this, 0);
    }
    if (*it != 0) {
        removeAction(*it);
        groupWindows.removeAction(*it);
    }

    m_window_list.erase(it);

    if (isEmpty())
    	setEnabled(false);
}

/*!
    Adds the widget \a w to this QtWindowListMenu. Does nothing if \a w is
    already in the menu. Normally it's not necessary to call this
    function, since QtWindowListMenu automatically adds windows to the
    list when they are inserted into the QWorkspace specified in the
    constructor.
*/

void QtWindowListMenu::addWindow(QWidget *w)
{
    if (w == 0) {
	    if (m_menubar != 0 && m_close_current_action != 0)
	        m_close_current_action->setEnabled(false);
	    return;
    }

    if (m_menubar != 0 && m_close_current_action != 0)
    	m_close_current_action->setEnabled(true);

    WindowList::iterator it = m_window_list.find(w);
    if (it == m_window_list.end()) {
	    m_window_list[w] = 0;
	    w->installEventFilter(this);
	    connect(w, SIGNAL(destroyed(QObject*)),
    	    	this, SLOT(windowDestroyed(QObject*)));
    	it = m_window_list.find(w);
    }

    if (!w->isVisible() && !w->isMinimized())
        return;

    if (*it == 0) {
        *it = new QAction(w->windowTitle(), &groupWindows);
        (*it)->setCheckable(true);
        if (!m_default_icon.isNull())
            (*it)->setIcon(m_default_icon);
        addAction(*it);
        connect(*it, SIGNAL(toggled(bool)), this, SLOT(setSenderChecked(bool)));
        connect(*it, SIGNAL(triggered()), w, SLOT(setFocus()));
        connect(*it, SIGNAL(triggered()), w, SLOT(show()));
    }

    setChecked(true, *it);
    setEnabled(true);
}

/*!
    Adds the widget \a widget to this QtWindowListMenu, and sets the icon of its menu item
    to be \a icon.
*/

void QtWindowListMenu::addWindow(QWidget *widget, const QIcon &icon)
{
    addWindow(widget);
    setWindowIcon(widget, icon);
}

/*!
    Sets \a icon as the icon of the menu item belonging to the widget \a widget. If \a icon is
    a null icon, then the menu item's icon will be cleared instead of set.
*/
void QtWindowListMenu::setWindowIcon(QWidget *widget, const QIcon &icon)
{
    WindowList::iterator it = m_window_list.find(widget);
    if (it != m_window_list.end())
        (*it)->setIcon(icon);
}

/*!
    Sets \a icon as the default menu item icon for this QtWindowListMenu. The menu items for each of the windows
    currently listed in the menu will get this as their current icon. New menu items added to the menu will also
    get this icon by default. If \a icon is a null icon, then the default icon will be cleared.
*/
void QtWindowListMenu::setDefaultIcon(const QIcon &icon)
{
    m_default_icon = icon;
    WindowList::iterator it;
    for (it=m_window_list.begin();it!=m_window_list.end();++it)
        (*it)->setIcon(icon);
}

void QtWindowListMenu::setSenderChecked(bool /*checked*/)
{
    setChecked(true, qobject_cast<QAction *>(sender()));
}

void QtWindowListMenu::setChecked(bool checked, QAction *a)
{
    if (a != 0) {
        WindowList::iterator it = m_window_list.begin();
        for (;it != m_window_list.end(); ++it) {
            if (*it != 0) {
                bool wasBlocked = (*it)->signalsBlocked();
                (*it)->blockSignals(true);
                (*it)->setChecked(false);
                (*it)->blockSignals(wasBlocked);
            }
        }

        bool wasBlocked = a->signalsBlocked();
        a->blockSignals(true);
        a->setChecked(checked);
        a->blockSignals(wasBlocked);
    }
}

/*! \internal */

bool QtWindowListMenu::isEmpty()
{
    WindowList::const_iterator it = m_window_list.begin();
    for (; it != m_window_list.end(); ++it) {
    	if (*it != 0)
	        return false;
    }
    return true;
}

/*! \internal */

bool QtWindowListMenu::eventFilter(QObject *obj, QEvent *e)
{
    if (obj == m_workspace) {
        if (e->type() != QEvent::ChildAdded)
	        return false;

    	QChildEvent *child_event = (QChildEvent *) e;
	    QObject *child = child_event->child();
    	if (qstrcmp(child->metaObject()->className(), "QWorkspaceChild"))
	        return false;

        QWidget *child_widget = qobject_cast<QWidget *>(child);
        if (child_widget == 0)
            return false;
        addWindow(child_widget);
    } else {
        // events from registered windows
        QWidget *w = qobject_cast<QWidget *>(obj);
        if (w == 0)
	        return false;
	    WindowList::iterator it = m_window_list.find(w);
	    if (it == m_window_list.end())
            return false;

	    switch (e->type()) {
	    case QEvent::Show:
    		addWindow(w);   // If it was hidden when placed on workspace,
	    	    		    // it will be added to the menu now
		    break;
	    case QEvent::Close:
		    // we keep the window in the m_window_list, it might be show()n again.
            if (*it != 0) {
	    	    removeAction(*it);
                groupWindows.removeAction(*it);
            }
		    *it = 0;
		    if (isEmpty())
	    	    setEnabled(false);
		    break;
	    case QEvent::WindowTitleChange:
    		if (*it != 0)
                (*it)->setText(w->windowTitle());
		    break;
	    default: break;
	    }
    }

    return false;
}

/*! \internal */

void QtWindowListMenu::windowDestroyed(QObject *obj)
{
    removeWindow((QWidget*)obj, true);
}

/*!
    Enables or disables the window list menu, according to the value
    of \a b. Normally it is not necessary to call this function, since
    QtWindowListMenu automatically enables itself whenever there is
    more than one window present in the workspace.

    \sa QAction::setEnabled()
*/

void QtWindowListMenu::setEnabled(bool b)
{
    if (m_menubar != 0)
        m_my_action->setEnabled(b);
}

/*!
    Sets \a icon as the icon of the menu item which closes the current window.
    If \a icon is a null icon, then the menu item's icon will be cleared instead of set.
*/
void QtWindowListMenu::setCloseIcon(const QIcon &icon)
{
    m_close_current_action->setIcon(icon);
}

/*!
    Sets \a icon as the icon of the menu item which cascades the windows in the workspace.
    If \a icon is a null icon, then the menu item's icon will be cleared instead of set.
*/
void QtWindowListMenu::setCascadeIcon(const QIcon &icon)
{
    m_cascade_action->setIcon(icon);
}

/*!
    Sets \a icon as the icon of the menu item which tiles the windows in the workspace.
    If \a icon is a null icon, then the menu item's icon will be cleared instead of set.
*/
void QtWindowListMenu::setTileIcon(const QIcon &icon)
{
    m_tile_action->setIcon(icon);
}

/*!
    Sets \a icon as the icon of the menu item which closes all windows in the workspace.
    If \a icon is a null icon, then the menu item's icon will be cleared instead of set.
*/
void QtWindowListMenu::setCloseAllIcon(const QIcon &icon)
{
    m_close_all_action->setIcon(icon);
}

QT_END_NAMESPACE
