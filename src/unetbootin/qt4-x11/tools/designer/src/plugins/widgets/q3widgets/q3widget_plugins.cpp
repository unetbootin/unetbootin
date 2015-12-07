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

#include "q3widget_plugins.h"

#include <QtCore/qplugin.h>
#include <QtGui/QIcon>
#include <QtGui/QLayout>
#include <Qt3Support/Q3ButtonGroup>
#include <Qt3Support/Q3ComboBox>
#include <Qt3Support/Q3DateEdit>
#include <Qt3Support/Q3DateTimeEdit>
#include <Qt3Support/Q3Frame>
#include <Qt3Support/Q3GroupBox>
#include <Qt3Support/Q3ProgressBar>
#include <Qt3Support/Q3TextBrowser>
#include <Qt3Support/Q3TimeEdit>

static const char *groupNameC = "Qt 3 Support";

QT_BEGIN_NAMESPACE

Q3ButtonGroupPlugin::Q3ButtonGroupPlugin(const QIcon &icon, QObject *parent) :
    QObject(parent),
    m_initialized(false),
    m_icon(icon)
{
}

Q3ButtonGroupPlugin::~Q3ButtonGroupPlugin()
{
}

QString Q3ButtonGroupPlugin::name() const
{
    return QLatin1String("Q3ButtonGroup");
}

QString Q3ButtonGroupPlugin::group() const
{
    return QLatin1String(groupNameC);
}

QString Q3ButtonGroupPlugin::toolTip() const
{
    return QString();
}

QString Q3ButtonGroupPlugin::whatsThis() const
{
    return QString();
}

QString Q3ButtonGroupPlugin::includeFile() const
{
    return QLatin1String("Qt3Support/Q3ButtonGroup");
}

QIcon Q3ButtonGroupPlugin::icon() const
{
    return m_icon;
}

bool Q3ButtonGroupPlugin::isContainer() const
{
    return true;
}

QWidget *Q3ButtonGroupPlugin::createWidget(QWidget *parent)
{
    Q3ButtonGroup *g = new Q3ButtonGroup(parent);
    g->setColumnLayout(0, Qt::Vertical);
    g->setInsideMargin(0);
    g->layout()->setSpacing(-1);
    return g;
}

bool Q3ButtonGroupPlugin::isInitialized() const
{
    return m_initialized;
}

void Q3ButtonGroupPlugin::initialize(QDesignerFormEditorInterface *core)
{
    Q_UNUSED(core);
    m_initialized = true;
}

QString Q3ButtonGroupPlugin::domXml() const
{
    return QLatin1String("\
<ui language=\"c++\">\
    <widget class=\"Q3ButtonGroup\" name=\"buttonGroup\">\
        <property name=\"geometry\">\
            <rect>\
                <x>0</x>\
                <y>0</y>\
                <width>100</width>\
                <height>80</height>\
            </rect>\
        </property>\
    </widget>\
</ui>");
}

Q3ComboBoxPlugin::Q3ComboBoxPlugin(const QIcon &icon, QObject *parent) :
    QObject(parent),
    m_initialized(false),
    m_icon(icon)
{
}

Q3ComboBoxPlugin::~Q3ComboBoxPlugin()
{
}

QString Q3ComboBoxPlugin::name() const
{
    return QLatin1String("Q3ComboBox");
}

QString Q3ComboBoxPlugin::group() const
{
    return QLatin1String(groupNameC);
}

QString Q3ComboBoxPlugin::toolTip() const
{
    return QString();
}

QString Q3ComboBoxPlugin::whatsThis() const
{
    return QString();
}

QString Q3ComboBoxPlugin::includeFile() const
{
    return QLatin1String("Qt3Support/Q3ComboBox");
}

QIcon Q3ComboBoxPlugin::icon() const
{
    return m_icon;
}

bool Q3ComboBoxPlugin::isContainer() const
{
    return false;
}

QWidget *Q3ComboBoxPlugin::createWidget(QWidget *parent)
{
    return new Q3ComboBox(parent);
}

bool Q3ComboBoxPlugin::isInitialized() const
{
    return m_initialized;
}

QString Q3ComboBoxPlugin::domXml() const
{
    return QLatin1String("\
<ui language=\"c++\">\
<widget class=\"Q3ComboBox\" name=\"comboBox\"/>\
</ui>");
}

void Q3ComboBoxPlugin::initialize(QDesignerFormEditorInterface *core)
{
    Q_UNUSED(core);
    m_initialized = true;
}

Q3DateEditPlugin::Q3DateEditPlugin(const QIcon &icon, QObject *parent)
        : QObject(parent), m_initialized(false), m_icon(icon)
{}

QString Q3DateEditPlugin::name() const
{ return QLatin1String("Q3DateEdit"); }

QString Q3DateEditPlugin::group() const
{ return QLatin1String(groupNameC); }

QString Q3DateEditPlugin::toolTip() const
{ return QString(); }

QString Q3DateEditPlugin::whatsThis() const
{ return QString(); }

QString Q3DateEditPlugin::includeFile() const
{ return QLatin1String("Qt3Support/Q3DateEdit"); }

QIcon Q3DateEditPlugin::icon() const
{ return m_icon; }

bool Q3DateEditPlugin::isContainer() const
{ return false; }

QWidget *Q3DateEditPlugin::createWidget(QWidget *parent)
{ return new Q3DateEdit(parent); }

bool Q3DateEditPlugin::isInitialized() const
{ return m_initialized; }

void Q3DateEditPlugin::initialize(QDesignerFormEditorInterface *core)
{
    Q_UNUSED(core);
    m_initialized = true;
}

QString Q3DateEditPlugin::domXml() const
{
    return QLatin1String("\
<ui language=\"c++\">\
    <widget class=\"Q3DateEdit\" name=\"dateEdit\"/>\
</ui>");
}

Q3DateTimeEditPlugin::Q3DateTimeEditPlugin(const QIcon &icon, QObject *parent)
        : QObject(parent), m_initialized(false), m_icon(icon)
{}

QString Q3DateTimeEditPlugin::name() const
{ return QLatin1String("Q3DateTimeEdit"); }

QString Q3DateTimeEditPlugin::group() const
{ return QLatin1String(groupNameC); }

QString Q3DateTimeEditPlugin::toolTip() const
{ return QString(); }

QString Q3DateTimeEditPlugin::whatsThis() const
{ return QString(); }

QString Q3DateTimeEditPlugin::includeFile() const
{ return QLatin1String("Qt3Support/Q3DateTimeEdit"); }

QIcon Q3DateTimeEditPlugin::icon() const
{ return m_icon; }

bool Q3DateTimeEditPlugin::isContainer() const
{ return false; }

QWidget *Q3DateTimeEditPlugin::createWidget(QWidget *parent)
{ return new Q3DateTimeEdit(parent); }

bool Q3DateTimeEditPlugin::isInitialized() const
{ return m_initialized; }

void Q3DateTimeEditPlugin::initialize(QDesignerFormEditorInterface *core)
{
    Q_UNUSED(core);
    m_initialized = true;
}

QString Q3DateTimeEditPlugin::domXml() const
{
    return QLatin1String("\
<ui language=\"c++\">\
    <widget class=\"Q3DateTimeEdit\" name=\"dateTimeEdit\"/>\
</ui>");
}

Q3FramePlugin::Q3FramePlugin(const QIcon &icon, QObject *parent)
    : QObject(parent),
      m_initialized(false),
      m_icon(icon)
{
}

Q3FramePlugin::~Q3FramePlugin()
{
}

QString Q3FramePlugin::name() const
{
    return QLatin1String("Q3Frame");
}

QString Q3FramePlugin::group() const
{
    return QLatin1String(groupNameC);
}

QString Q3FramePlugin::toolTip() const
{
    return QString();
}

QString Q3FramePlugin::whatsThis() const
{
    return QString();
}

QString Q3FramePlugin::includeFile() const
{
    return QLatin1String("Qt3Support/Q3Frame");
}

QIcon Q3FramePlugin::icon() const
{
    return m_icon;
}

bool Q3FramePlugin::isContainer() const
{
    return true;
}

QWidget *Q3FramePlugin::createWidget(QWidget *parent)
{
    return new Q3Frame(parent);
}

bool Q3FramePlugin::isInitialized() const
{
    return m_initialized;
}

void Q3FramePlugin::initialize(QDesignerFormEditorInterface *core)
{
    Q_UNUSED(core);
    m_initialized = true;
}

QString Q3FramePlugin::domXml() const
{
    return QLatin1String("\
<ui language=\"c++\">\
    <widget class=\"Q3Frame\" name=\"frame\">\
        <property name=\"geometry\">\
            <rect>\
                <x>0</x>\
                <y>0</y>\
                <width>100</width>\
                <height>80</height>\
            </rect>\
        </property>\
    </widget>\
</ui>");
}

Q3GroupBoxPlugin::Q3GroupBoxPlugin(const QIcon &icon, QObject *parent)
    : QObject(parent),
      m_initialized(false),
      m_icon(icon)
{
}

Q3GroupBoxPlugin::~Q3GroupBoxPlugin()
{
}

QString Q3GroupBoxPlugin::name() const
{
    return QLatin1String("Q3GroupBox");
}

QString Q3GroupBoxPlugin::group() const
{
    return QLatin1String(groupNameC);
}

QString Q3GroupBoxPlugin::toolTip() const
{
    return QString();
}

QString Q3GroupBoxPlugin::whatsThis() const
{
    return QString();
}

QString Q3GroupBoxPlugin::includeFile() const
{
    return QLatin1String("Qt3Support/Q3GroupBox");
}

QIcon Q3GroupBoxPlugin::icon() const
{
    return m_icon;
}

bool Q3GroupBoxPlugin::isContainer() const
{
    return true;
}

QWidget *Q3GroupBoxPlugin::createWidget(QWidget *parent)
{
    Q3GroupBox *g = new Q3GroupBox(parent);
    g->setColumnLayout(0, Qt::Vertical);
    g->setInsideMargin(0);
    g->layout()->setSpacing(-1);
    return g;
}

bool Q3GroupBoxPlugin::isInitialized() const
{
    return m_initialized;
}

void Q3GroupBoxPlugin::initialize(QDesignerFormEditorInterface *core)
{
    Q_UNUSED(core);
    m_initialized = true;
}

QString  Q3GroupBoxPlugin::domXml() const
{
    return QLatin1String("\
<ui language=\"c++\">\
    <widget class=\"Q3GroupBox\" name=\"groupBox\">\
        <property name=\"geometry\">\
            <rect>\
                <x>0</x>\
                <y>0</y>\
                <width>100</width>\
                <height>80</height>\
            </rect>\
        </property>\
    </widget>\
</ui>");
}

Q3ProgressBarPlugin::Q3ProgressBarPlugin(const QIcon &icon, QObject *parent)
        : QObject(parent), m_initialized(false), m_icon(icon)
{}

QString Q3ProgressBarPlugin::name() const
{ return QLatin1String("Q3ProgressBar"); }

QString Q3ProgressBarPlugin::group() const
{ return QLatin1String(groupNameC); }

QString Q3ProgressBarPlugin::toolTip() const
{ return QString(); }

QString Q3ProgressBarPlugin::whatsThis() const
{ return QString(); }

QString Q3ProgressBarPlugin::includeFile() const
{ return QLatin1String("Qt3Support/Q3ProgressBar"); }

QIcon Q3ProgressBarPlugin::icon() const
{ return m_icon; }

bool Q3ProgressBarPlugin::isContainer() const
{ return false; }

QWidget *Q3ProgressBarPlugin::createWidget(QWidget *parent)
{ return new Q3ProgressBar(parent); }

bool Q3ProgressBarPlugin::isInitialized() const
{ return m_initialized; }

void Q3ProgressBarPlugin::initialize(QDesignerFormEditorInterface *core)
{
    Q_UNUSED(core);
    m_initialized = true;
}

QString Q3ProgressBarPlugin::domXml() const
{
    return QLatin1String("\
<ui language=\"c++\">\
    <widget class=\"Q3ProgressBar\" name=\"progressBar\"/>\
</ui>");
}

Q3TextBrowserPlugin::Q3TextBrowserPlugin(const QIcon &icon, QObject *parent)
        : QObject(parent), m_initialized(false), m_icon(icon)
{}

QString Q3TextBrowserPlugin::name() const
{ return QLatin1String("Q3TextBrowser"); }

QString Q3TextBrowserPlugin::group() const
{ return QLatin1String(groupNameC); }

QString Q3TextBrowserPlugin::toolTip() const
{ return QString(); }

QString Q3TextBrowserPlugin::whatsThis() const
{ return QString(); }

QString Q3TextBrowserPlugin::includeFile() const
{ return QLatin1String("Qt3Support/Q3TextBrowser"); }

QIcon Q3TextBrowserPlugin::icon() const
{ return m_icon; }

bool Q3TextBrowserPlugin::isContainer() const
{ return false; }

QWidget *Q3TextBrowserPlugin::createWidget(QWidget *parent)
{ return new Q3TextBrowser(parent); }

bool Q3TextBrowserPlugin::isInitialized() const
{ return m_initialized; }

void Q3TextBrowserPlugin::initialize(QDesignerFormEditorInterface *core)
{
    Q_UNUSED(core);
    m_initialized = true;
}

QString Q3TextBrowserPlugin::domXml() const
{
    return QLatin1String("\
<ui language=\"c++\">\
    <widget class=\"Q3TextBrowser\" name=\"textBrowser\"/>\
</ui>");
}

Q3TimeEditPlugin::Q3TimeEditPlugin(const QIcon &icon, QObject *parent)
        : QObject(parent), m_initialized(false), m_icon(icon)
{}

QString Q3TimeEditPlugin::name() const
{ return QLatin1String("Q3TimeEdit"); }

QString Q3TimeEditPlugin::group() const
{ return QLatin1String(groupNameC); }

QString Q3TimeEditPlugin::toolTip() const
{ return QString(); }

QString Q3TimeEditPlugin::whatsThis() const
{ return QString(); }

QString Q3TimeEditPlugin::includeFile() const
{ return QLatin1String("Qt3Support/Q3TimeEdit"); }

QIcon Q3TimeEditPlugin::icon() const
{ return m_icon; }

bool Q3TimeEditPlugin::isContainer() const
{ return false; }

QWidget *Q3TimeEditPlugin::createWidget(QWidget *parent)
{ return new Q3TimeEdit(parent); }

bool Q3TimeEditPlugin::isInitialized() const
{ return m_initialized; }

void Q3TimeEditPlugin::initialize(QDesignerFormEditorInterface *core)
{
    Q_UNUSED(core);
    m_initialized = true;
}

QString Q3TimeEditPlugin::domXml() const
{
    return QLatin1String("\
<ui language=\"c++\">\
    <widget class=\"Q3TimeEdit\" name=\"timeEdit\"/>\
</ui>");
}

QT_END_NAMESPACE
