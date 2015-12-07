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

#include "q3widgetstack_container.h"
#include "qdesigner_q3widgetstack_p.h"

#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

Q3WidgetStackContainer::Q3WidgetStackContainer(QDesignerQ3WidgetStack *widget, QObject *parent)
    : QObject(parent),
      m_widget(widget)
{}

int Q3WidgetStackContainer::count() const
{ return m_pages.count(); }

QWidget *Q3WidgetStackContainer::widget(int index) const
{
    if (index == -1)
        return 0;

    return m_pages.at(index);
}

int Q3WidgetStackContainer::currentIndex() const
{ return m_pages.indexOf(m_widget->visibleWidget()); }

void Q3WidgetStackContainer::setCurrentIndex(int index)
{ m_widget->raiseWidget(m_pages.at(index)); }

void Q3WidgetStackContainer::addWidget(QWidget *widget)
{
    m_pages.append(widget);
    m_widget->addWidget(widget);
}

void Q3WidgetStackContainer::insertWidget(int index, QWidget *widget)
{
    m_pages.insert(index, widget);
    m_widget->addWidget(widget);
    m_widget->setCurrentIndex(index);
}

void Q3WidgetStackContainer::remove(int index)
{
    int current = currentIndex();
    m_widget->removeWidget(m_pages.at(index));
    m_pages.removeAt(index);
    if (index == current) {
        if (count() > 0)
            m_widget->setCurrentIndex((index == count()) ? index-1 : index);
    } else if (index < current) {
        if (current > 0)
            m_widget->setCurrentIndex(current-1);
    }
}

Q3WidgetStackContainerFactory::Q3WidgetStackContainerFactory(QExtensionManager *parent)
    : QExtensionFactory(parent)
{
}

QObject *Q3WidgetStackContainerFactory::createExtension(QObject *object, const QString &iid, QObject *parent) const
{
    if (iid != Q_TYPEID(QDesignerContainerExtension))
        return 0;

    if (QDesignerQ3WidgetStack *w = qobject_cast<QDesignerQ3WidgetStack*>(object))
        return new Q3WidgetStackContainer(w, parent);

    return 0;
}


QT_END_NAMESPACE
