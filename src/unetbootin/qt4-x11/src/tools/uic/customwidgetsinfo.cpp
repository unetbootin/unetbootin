/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the tools applications of the Qt Toolkit.
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

#include "customwidgetsinfo.h"
#include "driver.h"
#include "ui4.h"
#include "utils.h"

QT_BEGIN_NAMESPACE

CustomWidgetsInfo::CustomWidgetsInfo()
{
}

void CustomWidgetsInfo::acceptUI(DomUI *node)
{
    m_customWidgets.clear();

    if (node->elementCustomWidgets())
        acceptCustomWidgets(node->elementCustomWidgets());
}

void CustomWidgetsInfo::acceptCustomWidgets(DomCustomWidgets *node)
{
    TreeWalker::acceptCustomWidgets(node);
}

void CustomWidgetsInfo::acceptCustomWidget(DomCustomWidget *node)
{
    if (node->elementClass().isEmpty())
        return;

    m_customWidgets.insert(node->elementClass(), node);
}

bool CustomWidgetsInfo::extends(const QString &className, const QLatin1String &baseClassName) const
{
    if (className == baseClassName)
        return true;

    if (DomCustomWidget *c = customWidget(className))
        return c->elementExtends() == baseClassName;

    return false;
}

QString CustomWidgetsInfo::realClassName(const QString &className) const
{
    if (className == QLatin1String("Line"))
        return QLatin1String("QFrame");

    return className;
}

DomScript *CustomWidgetsInfo::customWidgetScript(const QString &name) const
{
    if (m_customWidgets.empty())
        return 0;

    const NameCustomWidgetMap::const_iterator it = m_customWidgets.constFind(name);
    if (it == m_customWidgets.constEnd())
        return 0;

    return it.value()->elementScript();
}

QString CustomWidgetsInfo::customWidgetAddPageMethod(const QString &name) const
{
    if (DomCustomWidget *dcw = m_customWidgets.value(name, 0))
        return dcw->elementAddPageMethod();
    return QString();
}


QT_END_NAMESPACE
