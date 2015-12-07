/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt Designer of the Qt Toolkit.
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

#include "q3mainwindow_container.h"

#include <Qt3Support/Q3MainWindow>

#include <QtCore/qdebug.h>
#include <QtGui/QToolBar>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>

#include <Qt3Support/Q3ToolBar>

QT_BEGIN_NAMESPACE

Q3MainWindowContainer::Q3MainWindowContainer(Q3MainWindow *widget, QObject *parent)
    : QObject(parent),
      m_mainWindow(widget)
{}

int Q3MainWindowContainer::count() const
{
    return m_widgets.count();
}

QWidget *Q3MainWindowContainer::widget(int index) const
{
    if (index == -1)
        return 0;

    return m_widgets.at(index);
}

int Q3MainWindowContainer::currentIndex() const
{
    return m_mainWindow->centralWidget() ? 0 : -1;
}

void Q3MainWindowContainer::setCurrentIndex(int index)
{
    Q_UNUSED(index);
}

void Q3MainWindowContainer::addWidget(QWidget *widget)
{
    if (qobject_cast<QToolBar*>(widget)) {
        m_widgets.append(widget);
    } else if (qobject_cast<Q3ToolBar*>(widget)) {
        m_widgets.append(widget);
    } else if (qobject_cast<QMenuBar*>(widget)) {
        (void) m_mainWindow->menuBar();
        m_widgets.append(widget);
    } else if (qobject_cast<QStatusBar*>(widget)) {
        (void) m_mainWindow->statusBar();
        m_widgets.append(widget);
    } else {
        Q_ASSERT(m_mainWindow->centralWidget() == 0);
        widget->setParent(m_mainWindow);
        m_mainWindow->setCentralWidget(widget);
        m_widgets.prepend(widget);
    }
}

void Q3MainWindowContainer::insertWidget(int index, QWidget *widget)
{
    m_widgets.insert(index, widget);
}

void Q3MainWindowContainer::remove(int index)
{
    m_widgets.removeAt(index);
}

Q3MainWindowContainerFactory::Q3MainWindowContainerFactory(QExtensionManager *parent)
    : QExtensionFactory(parent)
{
}

QObject *Q3MainWindowContainerFactory::createExtension(QObject *object, const QString &iid, QObject *parent) const
{
    if (iid != Q_TYPEID(QDesignerContainerExtension))
        return 0;

    if (Q3MainWindow *w = qobject_cast<Q3MainWindow*>(object))
        return new Q3MainWindowContainer(w, parent);

    return 0;
}


QT_END_NAMESPACE
