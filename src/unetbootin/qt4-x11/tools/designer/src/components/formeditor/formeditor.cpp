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

#include "formeditor.h"
#include "formeditor_optionspage.h"
#include "embeddedoptionspage.h"
#include "templateoptionspage.h"
#include "metadatabase_p.h"
#include "widgetdatabase_p.h"
#include "widgetfactory_p.h"
#include "formwindowmanager.h"
#include "qmainwindow_container.h"
#include "qworkspace_container.h"
#include "qmdiarea_container.h"
#include "qwizard_container.h"
#include "default_container.h"
#include "default_layoutdecoration.h"
#include "default_actionprovider.h"
#include "qlayoutwidget_propertysheet.h"
#include "spacer_propertysheet.h"
#include "line_propertysheet.h"
#include "layout_propertysheet.h"
#include "qdesigner_stackedbox_p.h"
#include "qdesigner_toolbox_p.h"
#include "qdesigner_tabwidget_p.h"
#include "qtbrushmanager.h"
#include "brushmanagerproxy.h"
#include "iconcache.h"
#include "qtresourcemodel_p.h"
#include "qdesigner_integration_p.h"
#include "itemview_propertysheet.h"

// sdk
#include <QtDesigner/QExtensionManager>

// shared
#include <pluginmanager_p.h>
#include <qdesigner_taskmenu_p.h>
#include <qdesigner_membersheet_p.h>
#include <qdesigner_promotion_p.h>
#include <dialoggui_p.h>
#include <qdesigner_introspection_p.h>
#include <qdesigner_qsettings_p.h>

QT_BEGIN_NAMESPACE

namespace qdesigner_internal {

FormEditor::FormEditor(QObject *parent)
    : QDesignerFormEditorInterface(parent)
{
    setIntrospection(new QDesignerIntrospection);
    setDialogGui(new DialogGui);
    QDesignerPluginManager *pluginManager = new QDesignerPluginManager(this);
    setPluginManager(pluginManager);

    WidgetDataBase *widgetDatabase = new WidgetDataBase(this, this);
    setWidgetDataBase(widgetDatabase);

    MetaDataBase *metaDataBase = new MetaDataBase(this, this);
    setMetaDataBase(metaDataBase);

    WidgetFactory *widgetFactory = new WidgetFactory(this, this);
    setWidgetFactory(widgetFactory);

    FormWindowManager *formWindowManager = new FormWindowManager(this, this);
    setFormManager(formWindowManager);
    connect(formWindowManager, SIGNAL(formWindowAdded(QDesignerFormWindowInterface*)), widgetFactory, SLOT(formWindowAdded(QDesignerFormWindowInterface*)));
    connect(formWindowManager, SIGNAL(activeFormWindowChanged(QDesignerFormWindowInterface*)), widgetFactory, SLOT(activeFormWindowChanged(QDesignerFormWindowInterface*)));

    QExtensionManager *mgr = new QExtensionManager(this);
    const QString containerExtensionId = Q_TYPEID(QDesignerContainerExtension);

    QDesignerStackedWidgetContainerFactory::registerExtension(mgr, containerExtensionId);
    QDesignerTabWidgetContainerFactory::registerExtension(mgr, containerExtensionId);
    QDesignerToolBoxContainerFactory::registerExtension(mgr, containerExtensionId);
    QMainWindowContainerFactory::registerExtension(mgr, containerExtensionId);
    QDockWidgetContainerFactory::registerExtension(mgr, containerExtensionId);
    QScrollAreaContainerFactory::registerExtension(mgr, containerExtensionId);
    QWorkspaceContainerFactory::registerExtension(mgr, containerExtensionId);
    QMdiAreaContainerFactory::registerExtension(mgr, containerExtensionId);
    QWizardContainerFactory::registerExtension(mgr, containerExtensionId);

    mgr->registerExtensions(new QDesignerLayoutDecorationFactory(mgr),
                            Q_TYPEID(QDesignerLayoutDecorationExtension));

    const QString actionProviderExtensionId = Q_TYPEID(QDesignerActionProviderExtension);
    QToolBarActionProviderFactory::registerExtension(mgr, actionProviderExtensionId);
    QMenuBarActionProviderFactory::registerExtension(mgr, actionProviderExtensionId);
    QMenuActionProviderFactory::registerExtension(mgr, actionProviderExtensionId);

    QDesignerDefaultPropertySheetFactory::registerExtension(mgr);
    QLayoutWidgetPropertySheetFactory::registerExtension(mgr);
    SpacerPropertySheetFactory::registerExtension(mgr);
    LinePropertySheetFactory::registerExtension(mgr);
    LayoutPropertySheetFactory::registerExtension(mgr);
    QStackedWidgetPropertySheetFactory::registerExtension(mgr);
    QToolBoxWidgetPropertySheetFactory::registerExtension(mgr);
    QTabWidgetPropertySheetFactory::registerExtension(mgr);
    QMdiAreaPropertySheetFactory::registerExtension(mgr);
    QWorkspacePropertySheetFactory::registerExtension(mgr);
    QWizardPagePropertySheetFactory::registerExtension(mgr);
    QWizardPropertySheetFactory::registerExtension(mgr);

    QTreeViewPropertySheetFactory::registerExtension(mgr);
    QTableViewPropertySheetFactory::registerExtension(mgr);

    const QString internalTaskMenuId = QLatin1String("QDesignerInternalTaskMenuExtension");
    QDesignerTaskMenuFactory::registerExtension(mgr, internalTaskMenuId);

    mgr->registerExtensions(new QDesignerMemberSheetFactory(mgr),
                            Q_TYPEID(QDesignerMemberSheetExtension));

    setExtensionManager(mgr);

    setIconCache(new IconCache(this));

    QtBrushManager *brushManager = new QtBrushManager(this);
    setBrushManager(brushManager);

    BrushManagerProxy *brushProxy = new BrushManagerProxy(this, this);
    brushProxy->setBrushManager(brushManager);
    setPromotion(new QDesignerPromotion(this));

    QtResourceModel *resourceModel = new QtResourceModel(this);
    setResourceModel(resourceModel);
    connect(resourceModel, SIGNAL(qrcFileModifiedExternally(const QString &)),
            this, SLOT(slotQrcFileChangedExternally(const QString &)));

    QList<QDesignerOptionsPageInterface*> optionsPages;
    optionsPages << new TemplateOptionsPage(this) << new FormEditorOptionsPage(this) << new EmbeddedOptionsPage(this);
    setOptionsPages(optionsPages);

    setSettingsManager(new QDesignerQSettings());
}

FormEditor::~FormEditor()
{
}

void FormEditor::slotQrcFileChangedExternally(const QString &path)
{
    QDesignerIntegration *designerIntegration = qobject_cast<QDesignerIntegration *>(integration());
    if (!designerIntegration)
        return;

    QDesignerIntegration::ResourceFileWatcherBehaviour behaviour = designerIntegration->resourceFileWatcherBehaviour();
    if (behaviour == QDesignerIntegration::NoWatcher) {
        return;
    } else if (behaviour == QDesignerIntegration::PromptAndReload) {
        QMessageBox::StandardButton button = dialogGui()->message(topLevel(), QDesignerDialogGuiInterface::FileChangedMessage, QMessageBox::Warning,
                tr("Resource File Changed"),
                tr("The file \"%1\" has changed outside Designer. Do you want to reload it?").arg(path),
                QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

        if (button != QMessageBox::Yes)
            return;
    }

    resourceModel()->reload(path);
}

}

QT_END_NAMESPACE
