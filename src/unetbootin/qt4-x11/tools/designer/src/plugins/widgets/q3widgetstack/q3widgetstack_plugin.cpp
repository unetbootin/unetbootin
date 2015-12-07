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

#include "q3widgetstack_plugin.h"
#include "q3widgetstack_container.h"

#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QExtensionManager>

#include <QtCore/qplugin.h>
#include "qdesigner_q3widgetstack_p.h"

QT_BEGIN_NAMESPACE

Q3WidgetStackPlugin::Q3WidgetStackPlugin(const QIcon &icon, QObject *parent)
    : QObject(parent), m_initialized(false), m_icon(icon)
{}

QString Q3WidgetStackPlugin::name() const
{ return QLatin1String("Q3WidgetStack"); }

QString Q3WidgetStackPlugin::group() const
{ return QLatin1String("Qt 3 Support"); }

QString Q3WidgetStackPlugin::toolTip() const
{ return QString(); }

QString Q3WidgetStackPlugin::whatsThis() const
{ return QString(); }

QString Q3WidgetStackPlugin::includeFile() const
{ return QLatin1String("q3widgetstack.h"); }

QIcon Q3WidgetStackPlugin::icon() const
{ return m_icon; }

bool Q3WidgetStackPlugin::isContainer() const
{ return true; }

QWidget *Q3WidgetStackPlugin::createWidget(QWidget *parent)
{ return new QDesignerQ3WidgetStack(parent); }

bool Q3WidgetStackPlugin::isInitialized() const
{ return m_initialized; }

void Q3WidgetStackPlugin::initialize(QDesignerFormEditorInterface *core)
{
    Q_UNUSED(core);

    if (m_initialized)
        return;

    m_initialized = true;
    QExtensionManager *mgr = core->extensionManager();
    mgr->registerExtensions(new Q3WidgetStackContainerFactory(mgr), Q_TYPEID(QDesignerContainerExtension));
}

QString Q3WidgetStackPlugin::codeTemplate() const
{ return QString(); }

QString Q3WidgetStackPlugin::domXml() const
{
    return QLatin1String("\
<ui language=\"c++\">\
    <widget class=\"Q3WidgetStack\" name=\"widgetStack\">\
        <property name=\"geometry\">\
            <rect>\
                <x>0</x>\
                <y>0</y>\
                <width>100</width>\
                <height>80</height>\
            </rect>\
        </property>\
        <widget class=\"QWidget\" name=\"page\"/>\
        <widget class=\"QWidget\" name=\"page_2\"/>\
    </widget>\
</ui>");
}

QT_END_NAMESPACE
