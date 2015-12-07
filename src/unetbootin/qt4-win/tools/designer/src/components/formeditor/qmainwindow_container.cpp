/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt Designer of the Qt Toolkit.
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

#include "qmainwindow_container.h"
#include "qdesigner_toolbar_p.h"
#include "formwindow.h"

#include <QtCore/qdebug.h>

#include <QtGui/QLayout>
#include <QtGui/QMenuBar>
#include <QtGui/QToolBar>
#include <QtGui/QStatusBar>
#include <QtGui/QDockWidget>

QT_BEGIN_NAMESPACE

using namespace qdesigner_internal;

QMainWindowContainer::QMainWindowContainer(QMainWindow *widget, QObject *parent)
    : QObject(parent),
      m_mainWindow(widget)
{
}

int QMainWindowContainer::count() const
{
    return m_widgets.count();
}

QWidget *QMainWindowContainer::widget(int index) const
{
    if (index == -1)
        return 0;

    return m_widgets.at(index);
}

int QMainWindowContainer::currentIndex() const
{
    return m_mainWindow->centralWidget() ? 0 : -1;
}

void QMainWindowContainer::setCurrentIndex(int index)
{
    Q_UNUSED(index);
}


namespace {
    // Pair of <area,break_before>
    typedef QPair<Qt::ToolBarArea,bool> ToolBarData;
    
    ToolBarData toolBarData(QToolBar *me) {
        const QMainWindow *mw = qobject_cast<const QMainWindow*>(me->parentWidget());
        if (!mw || !mw->layout() ||  mw->layout()->indexOf(me) == -1)
            return ToolBarData(Qt::TopToolBarArea,false);
        return ToolBarData(mw->toolBarArea(me), mw->toolBarBreak(me));
    }

Qt::DockWidgetArea dockWidgetArea(QDockWidget *me) 
{
    if (const QMainWindow *mw = qobject_cast<const QMainWindow*>(me->parentWidget())) {
        // Make sure that me is actually managed by mw, otherwise
        // QMainWindow::dockWidgetArea() will be VERY upset
        QList<QLayout*> candidates;
        if (mw->layout()) {
            candidates.append(mw->layout());
            candidates += qFindChildren<QLayout*>(mw->layout());
        }
        foreach (QLayout *l, candidates) {
            if (l->indexOf(me) != -1) {
                return mw->dockWidgetArea(me);
            }
        }
    }
    return Qt::LeftDockWidgetArea;
}
}

void QMainWindowContainer::addWidget(QWidget *widget)
{
    // remove all the occurrences of widget
    m_widgets.removeAll(widget);

    // the
    if (QToolBar *toolBar = qobject_cast<QToolBar*>(widget)) {
        m_widgets.append(widget);
        const ToolBarData data = toolBarData(toolBar);
        m_mainWindow->addToolBar(data.first, toolBar);
        if (data.second) m_mainWindow->insertToolBarBreak(toolBar);
        toolBar->show();
    }

    else if (QMenuBar *menuBar = qobject_cast<QMenuBar*>(widget)) {
        if (menuBar != m_mainWindow->menuBar())
            m_mainWindow->setMenuBar(menuBar);

        m_widgets.append(widget);
        menuBar->show();
    }

    else if (QStatusBar *statusBar = qobject_cast<QStatusBar*>(widget)) {
        if (statusBar != m_mainWindow->statusBar())
            m_mainWindow->setStatusBar(statusBar);

        m_widgets.append(widget);
        statusBar->show();
    }

    else if (QDockWidget *dockWidget = qobject_cast<QDockWidget*>(widget)) {
        m_widgets.append(widget);
        m_mainWindow->addDockWidget(dockWidgetArea(dockWidget), dockWidget);
        dockWidget->show();

        if (FormWindow *fw = FormWindow::findFormWindow(m_mainWindow)) {
            fw->manageWidget(widget);
        }
    }

    else if (widget) {
        m_widgets.prepend(widget);

        if (widget != m_mainWindow->centralWidget()) {
            // note that qmainwindow will delete the current central widget if you
            // call setCentralWidget(), we end up with dangeling pointers in m_widgets list
            m_widgets.removeAll(m_mainWindow->centralWidget());

            widget->setParent(m_mainWindow);
            m_mainWindow->setCentralWidget(widget);
        }
    }
}

void QMainWindowContainer::insertWidget(int index, QWidget *widget)
{
    Q_UNUSED(index);

    addWidget(widget);
}

void QMainWindowContainer::remove(int index)
{
    QWidget *widget = m_widgets.at(index);
    if (QToolBar *toolBar = qobject_cast<QToolBar*>(widget)) {
        m_mainWindow->removeToolBar(toolBar);
    } else if (QMenuBar *menuBar = qobject_cast<QMenuBar*>(widget)) {
        menuBar->hide();
        menuBar->setParent(0);
        m_mainWindow->setMenuBar(0);
    } else if (QStatusBar *statusBar = qobject_cast<QStatusBar*>(widget)) {
        statusBar->hide();
        statusBar->setParent(0);
        m_mainWindow->setStatusBar(0);
    } else if (QDockWidget *dockWidget = qobject_cast<QDockWidget*>(widget)) {
        m_mainWindow->removeDockWidget(dockWidget);
    }
    m_widgets.removeAt(index);
}

QT_END_NAMESPACE
