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

#include "q3wizard_plugin.h"
#include "q3wizard_container.h"

#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QExtensionManager>

#include <QtCore/qplugin.h>
#include <QtGui/QPushButton>
#include <Qt3Support/Q3Wizard>

QT_BEGIN_NAMESPACE

Q3WizardPlugin::Q3WizardPlugin(const QIcon &icon, QObject *parent)
    : QObject(parent), m_initialized(false), m_icon(icon)
{}

QString Q3WizardPlugin::name() const
{ return QLatin1String("Q3Wizard"); }

QString Q3WizardPlugin::group() const
{ return QLatin1String("[invisible]"); }

QString Q3WizardPlugin::toolTip() const
{ return QString(); }

QString Q3WizardPlugin::whatsThis() const
{ return QString(); }

QString Q3WizardPlugin::includeFile() const
{ return QLatin1String("q3wizard.h"); }

QIcon Q3WizardPlugin::icon() const
{ return m_icon; }

bool Q3WizardPlugin::isContainer() const
{ return true; }

QWidget *Q3WizardPlugin::createWidget(QWidget *parent)
{
    Q3Wizard *wizard = new Q3Wizard(parent);
    new Q3WizardHelper(wizard);
    wizard->backButton()->setObjectName(QLatin1String("__qt__passive_") + wizard->backButton()->objectName());
    wizard->nextButton()->setObjectName(QLatin1String("__qt__passive_") + wizard->nextButton()->objectName());
    return wizard;
}

bool Q3WizardPlugin::isInitialized() const
{ return m_initialized; }

void Q3WizardPlugin::initialize(QDesignerFormEditorInterface *core)
{
    Q_UNUSED(core);

    if (m_initialized)
        return;

    m_initialized = true;
    QExtensionManager *mgr = core->extensionManager();
    Q3WizardPropertySheetFactory::registerExtension(mgr);
    mgr->registerExtensions(new Q3WizardContainerFactory(mgr), Q_TYPEID(QDesignerContainerExtension));
    mgr->registerExtensions(new Q3WizardExtraInfoFactory(core, mgr), Q_TYPEID(QDesignerExtraInfoExtension));
}

QString Q3WizardPlugin::codeTemplate() const
{ return QString(); }

QString Q3WizardPlugin::domXml() const
{
    return QLatin1String("\
    <widget class=\"Q3Wizard\" name=\"wizard\">\
        <property name=\"geometry\">\
            <rect>\
                <x>0</x>\
                <y>0</y>\
                <width>100</width>\
                <height>80</height>\
            </rect>\
        </property>\
        <widget class=\"QWidget\" />\
        <widget class=\"QWidget\" />\
    </widget>\
    ");
}


QT_END_NAMESPACE
