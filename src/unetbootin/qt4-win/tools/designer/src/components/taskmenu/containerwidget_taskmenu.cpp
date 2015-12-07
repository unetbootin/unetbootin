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

/*
TRANSLATOR qdesigner_internal::ContainerWidgetTaskMenu
*/

#include "containerwidget_taskmenu.h"


#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QDesignerFormWindowInterface>
#include <QtDesigner/QExtensionManager>
#include <QtDesigner/QDesignerContainerExtension>

#include <qdesigner_command_p.h>
#include <qdesigner_dockwidget_p.h>
#include <promotiontaskmenu_p.h>
#include <widgetdatabase_p.h>

#include <QtGui/QAction>
#include <QtGui/QMainWindow>
#include <QtGui/QToolBox>
#include <QtGui/QStackedWidget>
#include <QtGui/QTabWidget>
#include <QtGui/QScrollArea>
#include <QtGui/QMdiArea>
#include <QtGui/QWorkspace>
#include <QtGui/QWizard>
#include <QtGui/QMenu>

#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

namespace qdesigner_internal {

ContainerWidgetTaskMenu::ContainerWidgetTaskMenu(QWidget *widget, ContainerType type, QObject *parent) :
    QDesignerTaskMenu(widget, parent),
    m_type(type),
    m_containerWidget(widget),
    m_core(formWindow()->core()),
    m_pagePromotionTaskMenu(new PromotionTaskMenu(0, PromotionTaskMenu::ModeSingleWidget, this)),
    m_pageMenuAction(new QAction(this)),
    m_pageMenu(new QMenu),
    m_actionDeletePage(new QAction(tr("Delete"), this))
{
    Q_ASSERT(m_core);
    m_taskActions.append(createSeparator());

    connect(m_actionDeletePage, SIGNAL(triggered()), this, SLOT(removeCurrentPage()));

    QAction *actionInsertPageAfter = new QAction(this);
    connect(actionInsertPageAfter, SIGNAL(triggered()), this, SLOT(addPageAfter()));
    // Empty Per-Page submenu, deletion and promotion. Updated on demand due to promotion state
    switch (m_type) {
    case WizardContainer:
    case PageContainer:
        m_taskActions.append(createSeparator()); // for the browse actions
        break;
    case MdiContainer:
        break;
    }
    // submenu
    m_pageMenuAction->setMenu(m_pageMenu);
    m_taskActions.append(m_pageMenuAction);
    // Insertion
    switch (m_type) {
    case WizardContainer:
    case PageContainer: { // Before and after in a submenu
        QAction *insertMenuAction = new QAction(tr("Insert"), this);
        QMenu *insertMenu = new QMenu;
        // before
        QAction *actionInsertPage = new QAction(tr("Insert Page Before Current Page"), this);
        connect(actionInsertPage, SIGNAL(triggered()), this, SLOT(addPage()));
        insertMenu->addAction(actionInsertPage);
        // after
        actionInsertPageAfter->setText(tr("Insert Page After Current Page"));
        insertMenu->addAction(actionInsertPageAfter);

        insertMenuAction->setMenu(insertMenu);
        m_taskActions.append(insertMenuAction);
    }
        break;
    case MdiContainer: // No concept of order
        actionInsertPageAfter->setText(tr("Add Subwindow"));
        m_taskActions.append(actionInsertPageAfter);
        break;
    }
}

ContainerWidgetTaskMenu::~ContainerWidgetTaskMenu()
{
}

QAction *ContainerWidgetTaskMenu::preferredEditAction() const
{
    return 0;
}

bool ContainerWidgetTaskMenu::canDeletePage() const
{
    switch (pageCount()) {
    case 0:
        return false;
    case 1:
        return m_type != PageContainer; // Do not delete last page of page-type container
    default:
        break;
    }
    return true;
}

int ContainerWidgetTaskMenu::pageCount() const
{
    if (const QDesignerContainerExtension *ce = containerExtension())
        return ce->count();
    return 0;
}

QString ContainerWidgetTaskMenu::pageMenuText(ContainerType ct, int index, int count)
{
    if (ct == MdiContainer)
        return tr("Subwindow"); // No concept of order, same text everywhere
    if (index < 0)
        return tr("Page");
    return tr("Page %1 of %2").arg(index + 1).arg(count);
}

QList<QAction*> ContainerWidgetTaskMenu::taskActions() const
{
    QList<QAction*> actions = QDesignerTaskMenu::taskActions();
    actions += m_taskActions;
    // Update the page submenu, deletion and promotion. Updated on demand due to promotion state.
    m_pageMenu->clear();
    m_pageMenu->addAction(m_actionDeletePage);
    m_actionDeletePage->setEnabled(canDeletePage());
    const QDesignerContainerExtension *ce = containerExtension();
    const int index = ce->currentIndex();
    m_pageMenuAction->setText(pageMenuText(m_type, index, ce->count()));
    if (index != -1) { // Has a page
        m_pageMenuAction->setEnabled(true);
        m_pagePromotionTaskMenu->setWidget(ce->widget(index));
        m_pagePromotionTaskMenu->addActions(PromotionTaskMenu::LeadingSeparator|PromotionTaskMenu::SuppressGlobalEdit, m_pageMenu);
    } else { // No page
        m_pageMenuAction->setEnabled(false);
    }

    return actions;
}

QDesignerFormWindowInterface *ContainerWidgetTaskMenu::formWindow() const
{
    return QDesignerFormWindowInterface::findFormWindow(m_containerWidget);
}

QDesignerContainerExtension *ContainerWidgetTaskMenu::containerExtension() const
{
    QExtensionManager *mgr = m_core->extensionManager();
    return qt_extension<QDesignerContainerExtension*>(mgr, m_containerWidget);
}

void ContainerWidgetTaskMenu::removeCurrentPage()
{
    if (QDesignerContainerExtension *c = containerExtension()) {
        if (c->currentIndex() == -1)
            return;

        QDesignerFormWindowInterface *fw = formWindow();
        DeleteContainerWidgetPageCommand *cmd = new DeleteContainerWidgetPageCommand(fw);
        cmd->init(m_containerWidget, m_type);
        fw->commandHistory()->push(cmd);
    }
}

void ContainerWidgetTaskMenu::addPage()
{
    if (containerExtension()) {
        QDesignerFormWindowInterface *fw = formWindow();
        AddContainerWidgetPageCommand *cmd = new AddContainerWidgetPageCommand(fw);
        cmd->init(m_containerWidget, m_type, AddContainerWidgetPageCommand::InsertBefore);
        fw->commandHistory()->push(cmd);
    }
}

void ContainerWidgetTaskMenu::addPageAfter()
{
    if (containerExtension()) {
        QDesignerFormWindowInterface *fw = formWindow();
        AddContainerWidgetPageCommand *cmd = new AddContainerWidgetPageCommand(fw);
        cmd->init(m_containerWidget, m_type, AddContainerWidgetPageCommand::InsertAfter);
        fw->commandHistory()->push(cmd);
    }
}

// -------------- MdiContainerWidgetTaskMenu

MdiContainerWidgetTaskMenu::MdiContainerWidgetTaskMenu(QMdiArea *m, QObject *parent) :
    ContainerWidgetTaskMenu(m, MdiContainer, parent)
{
    initializeActions();
    connect(m_nextAction, SIGNAL(triggered()), m, SLOT( activateNextSubWindow ()));
    connect(m_previousAction, SIGNAL(triggered()), m , SLOT(activatePreviousSubWindow()));
    connect(m_tileAction, SIGNAL(triggered()), m, SLOT(tileSubWindows()));
    connect(m_cascadeAction, SIGNAL(triggered()), m, SLOT(cascadeSubWindows ()));
}

MdiContainerWidgetTaskMenu::MdiContainerWidgetTaskMenu(QWorkspace *m, QObject *parent) :
    ContainerWidgetTaskMenu(m, MdiContainer, parent)
{
    initializeActions();
    connect(m_nextAction, SIGNAL(triggered()), m, SLOT(activateNextWindow()));
    connect(m_previousAction, SIGNAL(triggered()), m, SLOT(activatePreviousWindow()));
    connect(m_tileAction, SIGNAL(triggered()),m , SLOT(tile()));
    connect(m_cascadeAction, SIGNAL(triggered()), m, SLOT(cascade()));
}

void MdiContainerWidgetTaskMenu::initializeActions()
{
    m_nextAction =new QAction(tr("Next Subwindow"), this);
    m_previousAction = new QAction(tr("Previous Subwindow"), this);
    m_tileAction = new QAction(tr("Tile"), this);
    m_cascadeAction = new QAction(tr("Cascade"), this);

    QList<QAction*> &l = containerActions();
    l.push_front(createSeparator());
    l.push_front(m_tileAction);
    l.push_front(m_cascadeAction);
    l.push_front(m_previousAction);
    l.push_front(m_nextAction);
    l.push_front(createSeparator());
}

QList<QAction*> MdiContainerWidgetTaskMenu::taskActions() const
{
    const QList<QAction*> rc = ContainerWidgetTaskMenu::taskActions();
    // Enable
    const int count = pageCount();
    m_nextAction->setEnabled(count > 1);
    m_previousAction->setEnabled(count > 1);
    m_tileAction->setEnabled(count);
    m_cascadeAction->setEnabled(count);
    return rc;
}

// --------------  ContainerWidgetTaskMenuFactory

ContainerWidgetTaskMenuFactory::ContainerWidgetTaskMenuFactory(QDesignerFormEditorInterface *core, QExtensionManager *extensionManager) :
    QExtensionFactory(extensionManager),
    m_core(core)
{
}

QObject *ContainerWidgetTaskMenuFactory::createExtension(QObject *object, const QString &iid, QObject *parent) const
{
    if (iid != QLatin1String("QDesignerInternalTaskMenuExtension") || !object->isWidgetType())
        return 0;

    QWidget *widget = qobject_cast<QWidget*>(object);

    if (qobject_cast<QStackedWidget*>(widget)
            || qobject_cast<QToolBox*>(widget)
            || qobject_cast<QTabWidget*>(widget)
            || qobject_cast<QDesignerDockWidget*>(widget)
            || qobject_cast<QScrollArea*>(widget)
            || qobject_cast<QMainWindow*>(widget)) {
        // Are we using Designer's own container extensions and task menus or did
        // someone provide an extra one with an addpage method, for example for a QScrollArea?
        if (const WidgetDataBase *wb = qobject_cast<const WidgetDataBase *>(m_core->widgetDataBase())) {
            const int idx = wb->indexOfObject(widget);
            const WidgetDataBaseItem *item = static_cast<const WidgetDataBaseItem *>(wb->item(idx));
            if (item->addPageMethod().isEmpty())
                return 0;
        }
    }

    if (qt_extension<QDesignerContainerExtension*>(extensionManager(), object) == 0)
        return 0;

    if (QMdiArea* ma = qobject_cast<QMdiArea*>(widget))
        return new MdiContainerWidgetTaskMenu(ma, parent);
    if (QWorkspace *ws = qobject_cast<QWorkspace*>(widget))
        return new MdiContainerWidgetTaskMenu(ws, parent);
    if (QWizard *wz = qobject_cast<QWizard *>(widget))
        return new ContainerWidgetTaskMenu(wz, WizardContainer, parent);
    return new ContainerWidgetTaskMenu(widget, PageContainer, parent);
}

}
QT_END_NAMESPACE
