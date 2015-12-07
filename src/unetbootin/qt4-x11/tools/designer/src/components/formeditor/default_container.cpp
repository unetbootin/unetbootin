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

#include "default_container.h"
#include <QtCore/QDebug>

QT_BEGIN_NAMESPACE

template <class Container>
static inline void setCurrentContainerIndex(int index, Container *container)
{
    const bool blocked = container->signalsBlocked();
    container->blockSignals(true);
    container->setCurrentIndex(index);
    container->blockSignals(blocked);
}

static inline void ensureNoParent(QWidget *widget)
{
    if (widget->parentWidget())
        widget->setParent(0);
}

static const char *PageLabel = "Page";

namespace qdesigner_internal {

// --------- QStackedWidgetContainer
QStackedWidgetContainer::QStackedWidgetContainer(QStackedWidget *widget, QObject *parent) :
    QObject(parent),
    m_widget(widget)
{
}

void QStackedWidgetContainer::setCurrentIndex(int index)
{
    setCurrentContainerIndex(index, m_widget);
}

void QStackedWidgetContainer::addWidget(QWidget *widget)
{
    ensureNoParent(widget);
    m_widget->addWidget(widget);
}

void QStackedWidgetContainer::insertWidget(int index, QWidget *widget)
{
    ensureNoParent(widget);
    m_widget->insertWidget(index, widget);
}

void QStackedWidgetContainer::remove(int index)
{
    m_widget->removeWidget(widget(index));
}

// --------- QTabWidgetContainer
QTabWidgetContainer::QTabWidgetContainer(QTabWidget *widget, QObject *parent) :
    QObject(parent),
    m_widget(widget)
{
}

void QTabWidgetContainer::setCurrentIndex(int index)
{
    setCurrentContainerIndex(index, m_widget);
}

void QTabWidgetContainer::addWidget(QWidget *widget)
{
    ensureNoParent(widget);
    m_widget->addTab(widget, QString::fromUtf8(PageLabel));
}

void QTabWidgetContainer::insertWidget(int index, QWidget *widget)
{
    ensureNoParent(widget);
    m_widget->insertTab(index, widget, QString::fromUtf8(PageLabel));
}

void QTabWidgetContainer::remove(int index)
{
    m_widget->removeTab(index);
}

// ------------------- QToolBoxContainer
QToolBoxContainer::QToolBoxContainer(QToolBox *widget, QObject *parent) :
    QObject(parent),
    m_widget(widget)
{
}

void QToolBoxContainer::setCurrentIndex(int index)
{
    setCurrentContainerIndex(index, m_widget);
}

void QToolBoxContainer::addWidget(QWidget *widget)
{
    ensureNoParent(widget);
    m_widget->addItem(widget, QString::fromUtf8(PageLabel));
}

void QToolBoxContainer::insertWidget(int index, QWidget *widget)
{
    ensureNoParent(widget);
    m_widget->insertItem(index, widget, QString::fromUtf8(PageLabel));
}

void QToolBoxContainer::remove(int index)
{
    m_widget->removeItem(index);
}

// ------------------- QScrollAreaContainer
// We pass on active=true only if there are no children yet.
// If there are children, it is a legacy custom widget QScrollArea that has an internal,
// unmanaged child, in which case we deactivate the extension (otherwise we crash).
// The child will then not show up in the task menu

QScrollAreaContainer::QScrollAreaContainer(QScrollArea *widget, QObject *parent) :
    QObject(parent),
    SingleChildContainer<QScrollArea>(widget, widget->widget() == 0)
{
}
// ------------------- QDockWidgetContainer
QDockWidgetContainer::QDockWidgetContainer(QDockWidget *widget, QObject *parent) :
    QObject(parent),
    SingleChildContainer<QDockWidget>(widget)
{
}

}

QT_END_NAMESPACE
