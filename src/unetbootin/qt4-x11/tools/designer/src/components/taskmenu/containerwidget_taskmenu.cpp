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

// -------------- WizardContainerWidgetTaskMenu
WizardContainerWidgetTaskMenu::WizardContainerWidgetTaskMenu(QWizard *w, QObject *parent) :
    ContainerWidgetTaskMenu(w, WizardContainer, parent),
    m_nextAction(new QAction(tr("Next"), this)),
    m_previousAction(new QAction(tr("Back"), this))
{
    connect(m_nextAction, SIGNAL(triggered()), w, SLOT(next()));
    connect(m_previousAction, SIGNAL(triggered()), w, SLOT(back()));
    QList<QAction*> &l = containerActions();
    l.push_front(createSeparator());
    l.push_front(m_nextAction);
    l.push_front(m_previousAction);
    l.push_front(createSeparator());
}

QList<QAction*> WizardContainerWidgetTaskMenu::taskActions() const
{
    // Enable
    const QDesignerContainerExtension *ce = containerExtension();
    const int index = ce->currentIndex();
    m_previousAction->setEnabled(index > 0);
    m_nextAction->setEnabled(index >= 0 && index < (ce->count() - 1));
    return ContainerWidgetTaskMenu::taskActions();
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
        return new WizardContainerWidgetTaskMenu(wz, parent);
    return new ContainerWidgetTaskMenu(widget, PageContainer, parent);
}

}
QT_END_NAMESPACE
