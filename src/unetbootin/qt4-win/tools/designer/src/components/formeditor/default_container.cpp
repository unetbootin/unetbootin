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

QScrollAreaContainer::QScrollAreaContainer(QScrollArea *widget, QObject *parent) :
    QObject(parent),
    SingleChildContainer<QScrollArea>(widget)
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
