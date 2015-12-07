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
