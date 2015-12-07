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

#ifndef QTWINDOWLISTMENU_H
#define QTWINDOWLISTMENU_H

#include <QMap>
#include <QMenu>
#include <QActionGroup>

QT_BEGIN_NAMESPACE

class QMenuBar;
class QWidget;
class QString;
class QWorkspace;
class QAction;

class QtWindowListMenu : public QMenu
{
    Q_OBJECT
public:
	QtWindowListMenu(QWorkspace *workspace, QWidget *parent = 0, const char *name = 0);
	QAction *addTo(const QString &text, QMenuBar *menubar, int idx = -1);
	void removeWindow(QWidget *w, bool windowDestroyed = false);

	virtual bool eventFilter(QObject *obj, QEvent *e);

    void setWindowIcon(QWidget *widget, const QIcon &icon);
    void setDefaultIcon(const QIcon &icon);

    void setCloseIcon(const QIcon &icon);
    void setCloseAllIcon(const QIcon &icon);
    void setCascadeIcon(const QIcon &icon);
    void setTileIcon(const QIcon &icon);

public slots:
	void addWindow(QWidget *w);
    void addWindow(QWidget *w, const QIcon &icon);
	virtual void setEnabled(bool b);
	void windowDestroyed(QObject *obj);

private slots:
    void setSenderChecked(bool checked);

private:
	QMenuBar *m_menubar;
	QAction *m_my_action;
    QAction *m_close_current_action;
    QAction *m_close_all_action;
    QAction *m_cascade_action;
    QAction *m_tile_action;

    QIcon m_default_icon;

	/* A list of window/QAction* pairs. If the QAction-pointer is 0, we are keeping
	   track of the window, but it's hidden so it's not in the menu. */
	typedef QMap<QWidget *, QAction *> WindowList;
	WindowList m_window_list;
	QWorkspace *m_workspace;
    QActionGroup groupWindows;

	bool isEmpty();
    void setChecked(bool checked, QAction *a);
};

QT_END_NAMESPACE

#endif
