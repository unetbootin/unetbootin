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
TRANSLATOR qdesigner_internal::FormWindowManager
*/

#include "formwindowmanager.h"
#include "formwindow_dnditem.h"
#include "formwindow.h"
#include "formeditor.h"

#include "widgetdatabase_p.h"
#include "iconloader_p.h"
#include "widgetselection.h"
#include "connectionedit_p.h"
#include "qtresourcemodel_p.h"

#include <qdesigner_dnditem_p.h>
#include <qdesigner_command_p.h>
#include <layoutinfo_p.h>
#include <qlayout_widget_p.h>
#include <qdesigner_objectinspector_p.h>
#include <actioneditor_p.h>

#include <QtDesigner/QDesignerWidgetFactoryInterface>
#include <QtDesigner/QExtensionManager>
#include <QtDesigner/QDesignerContainerExtension>
#include <QtDesigner/QDesignerWidgetBoxInterface>

#include <QtGui/QUndoGroup>
#include <QtGui/QAction>
#include <QtGui/QSplitter>
#include <QtGui/QMouseEvent>
#include <QtGui/QApplication>
#include <QtGui/QSizeGrip>
#include <QtGui/QClipboard>
#include <QtGui/QMdiArea>
#include <QtGui/QMdiSubWindow>
#include <QtGui/QDesktopWidget>

#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

namespace {
    enum { debugFWM = 0 };
}

static inline QString whatsThisFrom(const QString &str) { /// ### implement me!
    return str;
}

// find the first child of w in a sequence
template <class Iterator>
static inline Iterator findFirstChildOf(Iterator it,Iterator end, const QWidget *w)
{
    for  (;it != end; ++it) {
        if (w->isAncestorOf(*it))
            return  it;
    }
    return it;
}

namespace qdesigner_internal {

FormWindowManager::FormWindowManager(QDesignerFormEditorInterface *core, QObject *parent) :
    QDesignerFormWindowManagerInterface(parent),
    m_core(core),
    m_activeFormWindow(0),
    m_layoutChilds(false)
{
    setupActions();
    qApp->installEventFilter(this);
}

FormWindowManager::~FormWindowManager()
{
    qDeleteAll(m_formWindows);
}

QDesignerFormEditorInterface *FormWindowManager::core() const
{
    return m_core;
}

QDesignerFormWindowInterface *FormWindowManager::activeFormWindow() const
{
    return m_activeFormWindow;
}

int FormWindowManager::formWindowCount() const
{
    return m_formWindows.size();
}

QDesignerFormWindowInterface *FormWindowManager::formWindow(int index) const
{
    return m_formWindows.at(index);
}

bool FormWindowManager::eventFilter(QObject *o, QEvent *e)
{
    if (!o->isWidgetType())
        return false;

    // If we don't have an active form, we only listen for WindowActivate to speed up integrations
    const QEvent::Type eventType = e->type();
    if (m_activeFormWindow == 0 && eventType != QEvent::WindowActivate)
        return false;

    switch (eventType) { // Uninteresting events
    case QEvent::Create:
    case QEvent::Destroy:
    case QEvent::AccessibilityDescription:
    case QEvent::AccessibilityHelp:
    case QEvent::AccessibilityPrepare:
    case QEvent::ActionAdded:
    case QEvent::ActionChanged:
    case QEvent::ActionRemoved:
    case QEvent::ChildAdded:
    case QEvent::ChildPolished:
    case QEvent::ChildRemoved:
    case QEvent::Clipboard:
    case QEvent::ContentsRectChange:
    case QEvent::DeferredDelete:
    case QEvent::FileOpen:
    case QEvent::LanguageChange:
    case QEvent::MetaCall:
    case QEvent::ModifiedChange:
    case QEvent::Paint:
    case QEvent::PaletteChange:
    case QEvent::ParentAboutToChange:
    case QEvent::ParentChange:
    case QEvent::Polish:
    case QEvent::PolishRequest:
    case QEvent::QueryWhatsThis:
    case QEvent::StatusTip:
    case QEvent::StyleChange:
    case QEvent::Timer:
    case QEvent::ToolBarChange:
    case QEvent::ToolTip:
    case QEvent::WhatsThis:
    case QEvent::WhatsThisClicked:
    case QEvent::DynamicPropertyChange:
    case QEvent::HoverEnter:
    case QEvent::HoverLeave:
    case QEvent::HoverMove:
        return false;
    default:
        break;
    }

    QWidget *widget = static_cast<QWidget*>(o);

    if (qobject_cast<WidgetHandle*>(widget)) { // ### remove me
        return false;
    }

    FormWindow *fw = FormWindow::findFormWindow(widget);
    if (fw == 0) {
        return false;
    }

    if (QWidget *managedWidget = findManagedWidget(fw, widget)) {
        // Prevent MDI and QWorkspace subwindows from being closed by clicking at the title bar
        if (managedWidget != widget && eventType == QEvent::Close) {
            e->ignore();
            return true;
        }
        switch (eventType) {

        case QEvent::WindowActivate: {
            if (fw->parentWidget()->isWindow() && fw->isMainContainer(managedWidget) && activeFormWindow() != fw) {
                setActiveFormWindow(fw);
            }
        } break;

        case QEvent::WindowDeactivate: {
            if (o == fw && o == activeFormWindow())
                fw->repaintSelection();
        } break;

        case QEvent::KeyPress: {
            QKeyEvent *ke = static_cast<QKeyEvent*>(e);
            if (ke->key() == Qt::Key_Escape) {
                ke->accept();
                return true;
            }
        }
        // don't break...

        default: {
            if (fw->handleEvent(widget, managedWidget, e)) {
                return true;
            }
        } break;

        } // end switch
    }

    return false;
}

void FormWindowManager::addFormWindow(QDesignerFormWindowInterface *w)
{
    FormWindow *formWindow = qobject_cast<FormWindow*>(w);
    if (!formWindow || m_formWindows.contains(formWindow))
        return;

    connect(formWindow, SIGNAL(selectionChanged()), this, SLOT(slotUpdateActions()));
    connect(formWindow->commandHistory(), SIGNAL(indexChanged(int)), this, SLOT(slotUpdateActions()));
    connect(formWindow, SIGNAL(toolChanged(int)), this, SLOT(slotUpdateActions()));

    if (ActionEditor *ae = qobject_cast<ActionEditor *>(m_core->actionEditor()))
        connect(w, SIGNAL(mainContainerChanged(QWidget*)), ae, SLOT(mainContainerChanged()));
    if (QDesignerObjectInspector *oi = qobject_cast<QDesignerObjectInspector *>(m_core->objectInspector()))
        connect(w, SIGNAL(mainContainerChanged(QWidget*)), oi, SLOT(mainContainerChanged()));

    m_formWindows.append(formWindow);
    emit formWindowAdded(formWindow);
}

void FormWindowManager::removeFormWindow(QDesignerFormWindowInterface *w)
{
    FormWindow *formWindow = qobject_cast<FormWindow*>(w);

    int idx = m_formWindows.indexOf(formWindow);
    if (!formWindow || idx == -1)
        return;

    formWindow->disconnect(this);
    m_formWindows.removeAt(idx);
    emit formWindowRemoved(formWindow);

    if (formWindow == m_activeFormWindow)
        setActiveFormWindow(0);
}

void FormWindowManager::setActiveFormWindow(QDesignerFormWindowInterface *w)
{
    FormWindow *formWindow = qobject_cast<FormWindow*>(w);

    if (formWindow == m_activeFormWindow)
        return;

    FormWindow *old = m_activeFormWindow;

    m_activeFormWindow = formWindow;

    QtResourceSet *resourceSet = 0;
    if (formWindow)
        resourceSet = formWindow->resourceSet();
    m_core->resourceModel()->setCurrentResourceSet(resourceSet);

    slotUpdateActions();

    if (m_activeFormWindow) {
        m_activeFormWindow->repaintSelection();
        if (old)
            old->repaintSelection();
    }

    emit activeFormWindowChanged(m_activeFormWindow);

    if (m_activeFormWindow) {
        m_activeFormWindow->emitSelectionChanged();
        m_activeFormWindow->commandHistory()->setActive();
        // Trigger setActiveSubWindow on mdi area unless we are in toplevel mode
        QMdiSubWindow *mdiSubWindow = 0;
        if (QWidget *formwindow = m_activeFormWindow->parentWidget()) {
            mdiSubWindow = qobject_cast<QMdiSubWindow *>(formwindow->parentWidget());
        }
        if (mdiSubWindow) {
            for (QWidget *parent = mdiSubWindow->parentWidget(); parent; parent = parent->parentWidget()) {
                if (QMdiArea *mdiArea = qobject_cast<QMdiArea*>(parent)) {
                    mdiArea->setActiveSubWindow(mdiSubWindow);
                    break;
                }
            }
        }
    }
}

QWidget *FormWindowManager::findManagedWidget(FormWindow *fw, QWidget *w)
{
    while (w && w != fw) {
        if (fw->isManaged(w))
            break;
        w = w->parentWidget();
    }
    return w;
}

void FormWindowManager::setupActions()
{
    m_actionCut = new QAction(createIconSet(QLatin1String("editcut.png")), tr("Cu&t"), this);
    m_actionCut->setObjectName(QLatin1String("__qt_cut_action"));
    m_actionCut->setShortcut(Qt::CTRL + Qt::Key_X);
    m_actionCut->setStatusTip(tr("Cuts the selected widgets and puts them on the clipboard"));
    m_actionCut->setWhatsThis(whatsThisFrom(QLatin1String("Edit|Cut")));
    connect(m_actionCut, SIGNAL(triggered()), this, SLOT(slotActionCutActivated()));
    m_actionCut->setEnabled(false);

    m_actionCopy = new QAction(createIconSet(QLatin1String("editcopy.png")), tr("&Copy"), this);
    m_actionCopy->setObjectName(QLatin1String("__qt_copy_action"));
    m_actionCopy->setShortcut(Qt::CTRL + Qt::Key_C);
    m_actionCopy->setStatusTip(tr("Copies the selected widgets to the clipboard"));
    m_actionCopy->setWhatsThis(whatsThisFrom(QLatin1String("Edit|Copy")));
    connect(m_actionCopy, SIGNAL(triggered()), this, SLOT(slotActionCopyActivated()));
    m_actionCopy->setEnabled(false);

    m_actionPaste = new QAction(createIconSet(QLatin1String("editpaste.png")), tr("&Paste"), this);
    m_actionPaste->setObjectName(QLatin1String("__qt_paste_action"));
    m_actionPaste->setShortcut(Qt::CTRL + Qt::Key_V);
    m_actionPaste->setStatusTip(tr("Pastes the clipboard's contents"));
    m_actionPaste->setWhatsThis(whatsThisFrom(QLatin1String("Edit|Paste")));
    connect(m_actionPaste, SIGNAL(triggered()), this, SLOT(slotActionPasteActivated()));
    m_actionPaste->setEnabled(false);

    m_actionDelete = new QAction(tr("&Delete"), this);
    m_actionDelete->setObjectName(QLatin1String("__qt_delete_action"));
    m_actionDelete->setStatusTip(tr("Deletes the selected widgets"));
    m_actionDelete->setWhatsThis(whatsThisFrom(QLatin1String("Edit|Delete")));
    connect(m_actionDelete, SIGNAL(triggered()), this, SLOT(slotActionDeleteActivated()));
    m_actionDelete->setEnabled(false);

    m_actionSelectAll = new QAction(tr("Select &All"), this);
    m_actionSelectAll->setObjectName(QLatin1String("__qt_select_all_action"));
    m_actionSelectAll->setShortcut(Qt::CTRL + Qt::Key_A);
    m_actionSelectAll->setStatusTip(tr("Selects all widgets"));
    m_actionSelectAll->setWhatsThis(whatsThisFrom(QLatin1String("Edit|Select All")));
    connect(m_actionSelectAll, SIGNAL(triggered()), this, SLOT(slotActionSelectAllActivated()));
    m_actionSelectAll->setEnabled(false);

    m_actionRaise = new QAction(createIconSet(QLatin1String("editraise.png")), tr("Bring to &Front"), this);
    m_actionRaise->setObjectName(QLatin1String("__qt_raise_action"));
    m_actionRaise->setShortcut(Qt::CTRL + Qt::Key_L);
    m_actionRaise->setStatusTip(tr("Raises the selected widgets"));
    m_actionRaise->setWhatsThis(tr("Raises the selected widgets"));
    connect(m_actionRaise, SIGNAL(triggered()), this, SLOT(slotActionRaiseActivated()));
    m_actionRaise->setEnabled(false);

    m_actionLower = new QAction(createIconSet(QLatin1String("editlower.png")), tr("Send to &Back"), this);
    m_actionLower->setObjectName(QLatin1String("__qt_lower_action"));
    m_actionLower->setShortcut(Qt::CTRL + Qt::Key_K);
    m_actionLower->setStatusTip(tr("Lowers the selected widgets"));
    m_actionLower->setWhatsThis(tr("Lowers the selected widgets"));
    connect(m_actionLower, SIGNAL(triggered()), this, SLOT(slotActionLowerActivated()));
    m_actionLower->setEnabled(false);

    m_actionAdjustSize = new QAction(createIconSet(QLatin1String("adjustsize.png")), tr("Adjust &Size"), this);
    m_actionAdjustSize->setObjectName(QLatin1String("__qt_adjust_size_action"));
    m_actionAdjustSize->setShortcut(Qt::CTRL + Qt::Key_J);
    m_actionAdjustSize->setStatusTip(tr("Adjusts the size of the selected widget"));
    m_actionAdjustSize->setWhatsThis(whatsThisFrom(QLatin1String("Layout|Adjust Size")));
    connect(m_actionAdjustSize, SIGNAL(triggered()), this, SLOT(slotActionAdjustSizeActivated()));
    m_actionAdjustSize->setEnabled(false);

    m_actionHorizontalLayout = new QAction(createIconSet(QLatin1String("edithlayout.png")), tr("Lay Out &Horizontally"), this);
    m_actionHorizontalLayout->setObjectName(QLatin1String("__qt_horizontal_layout_action"));
    m_actionHorizontalLayout->setShortcut(Qt::CTRL + Qt::Key_1);
    m_actionHorizontalLayout->setStatusTip(tr("Lays out the selected widgets horizontally"));
    m_actionHorizontalLayout->setWhatsThis(whatsThisFrom(QLatin1String("Layout|Lay Out Horizontally")));
    connect(m_actionHorizontalLayout, SIGNAL(triggered()), this, SLOT(slotActionHorizontalLayoutActivated()));
    m_actionHorizontalLayout->setEnabled(false);

    m_actionVerticalLayout = new QAction(createIconSet(QLatin1String("editvlayout.png")), tr("Lay Out &Vertically"), this);
    m_actionVerticalLayout->setObjectName(QLatin1String("__qt_vertical_layout_action"));
    m_actionVerticalLayout->setShortcut(Qt::CTRL + Qt::Key_2);
    m_actionVerticalLayout->setStatusTip(tr("Lays out the selected widgets vertically"));
    m_actionVerticalLayout->setWhatsThis(whatsThisFrom(QLatin1String("Layout|Lay Out Vertically")));
    connect(m_actionVerticalLayout, SIGNAL(triggered()), this, SLOT(slotActionVerticalLayoutActivated()));
    m_actionVerticalLayout->setEnabled(false);

    QAction *actionFormLayout = new QAction(createIconSet(QLatin1String("editform.png")), tr("Lay Out in a &Form Layout"), this);
    actionFormLayout->setObjectName(QLatin1String("__qt_form_layout_action"));
    actionFormLayout->setShortcut(Qt::CTRL + Qt::Key_6);
    actionFormLayout->setStatusTip(tr("Lays out the selected widgets in a form layout"));
    actionFormLayout->setWhatsThis(whatsThisFrom(QLatin1String("Layout|Lay Out in a Form")));
    connect(actionFormLayout, SIGNAL(triggered()), this, SLOT(slotActionFormLayoutActivated()));
    actionFormLayout->setEnabled(false);
    setActionFormLayout(actionFormLayout);

    m_actionGridLayout = new QAction(createIconSet(QLatin1String("editgrid.png")), tr("Lay Out in a &Grid"), this);
    m_actionGridLayout->setObjectName(QLatin1String("__qt_grid_layout_action"));
    m_actionGridLayout->setShortcut(Qt::CTRL + Qt::Key_5);
    m_actionGridLayout->setStatusTip(tr("Lays out the selected widgets in a grid"));
    m_actionGridLayout->setWhatsThis(whatsThisFrom(QLatin1String("Layout|Lay Out in a Grid")));
    connect(m_actionGridLayout, SIGNAL(triggered()), this, SLOT(slotActionGridLayoutActivated()));
    m_actionGridLayout->setEnabled(false);

    m_actionSplitHorizontal = new QAction(createIconSet(QLatin1String("editvlayoutsplit.png")),
                                          tr("Lay Out Horizontally in S&plitter"), this);
    m_actionSplitHorizontal->setObjectName(QLatin1String("__qt_split_horizontal_action"));
    m_actionSplitHorizontal->setShortcut(Qt::CTRL + Qt::Key_3);
    m_actionSplitHorizontal->setStatusTip(tr("Lays out the selected widgets horizontally in a splitter"));
    m_actionSplitHorizontal->setWhatsThis(whatsThisFrom(QLatin1String("Layout|Lay Out Horizontally in Splitter")));
    connect(m_actionSplitHorizontal, SIGNAL(triggered()), this, SLOT(slotActionSplitHorizontalActivated()));
    m_actionSplitHorizontal->setEnabled(false);

    m_actionSplitVertical = new QAction(createIconSet(QLatin1String("edithlayoutsplit.png")),
                                        tr("Lay Out Vertically in Sp&litter"), this);
    m_actionSplitVertical->setObjectName(QLatin1String("__qt_split_vertical_action"));
    m_actionSplitVertical->setShortcut(Qt::CTRL + Qt::Key_4);
    m_actionSplitVertical->setStatusTip(tr("Lays out the selected widgets vertically in a splitter"));
    m_actionSplitVertical->setWhatsThis(whatsThisFrom(QLatin1String("Layout|Lay Out Vertically in Splitter")));
    connect(m_actionSplitVertical, SIGNAL(triggered()), this, SLOT(slotActionSplitVerticalActivated()));
    m_actionSplitVertical->setEnabled(false);

    m_actionBreakLayout = new QAction(createIconSet(QLatin1String("editbreaklayout.png")), tr("&Break Layout"), this);
    m_actionBreakLayout->setObjectName(QLatin1String("__qt_break_layout_action"));
    m_actionBreakLayout->setShortcut(Qt::CTRL + Qt::Key_0);
    m_actionBreakLayout->setStatusTip(tr("Breaks the selected layout"));
    m_actionBreakLayout->setWhatsThis(whatsThisFrom(QLatin1String("Layout|Break Layout")));
    connect(m_actionBreakLayout, SIGNAL(triggered()), this, SLOT(slotActionBreakLayoutActivated()));
    m_actionBreakLayout->setEnabled(false);

    QAction *simplifyLayoutAction = new QAction(tr("Si&mplify Grid Layout"), this);
    simplifyLayoutAction->setObjectName(QLatin1String("__qt_simplify_layout_action"));
    simplifyLayoutAction->setStatusTip(tr("Removes empty columns and rows"));
    simplifyLayoutAction->setWhatsThis(whatsThisFrom(QLatin1String("Layout|Simplify Layout")));
    connect(simplifyLayoutAction, SIGNAL(triggered()), this, SLOT(slotActionSimplifyLayoutActivated()));
    simplifyLayoutAction->setEnabled(false);
    setActionSimplifyLayout(simplifyLayoutAction);

    m_undoGroup = new QUndoGroup(this);

    m_actionUndo = m_undoGroup->createUndoAction(this);
    m_actionUndo->setEnabled(false);
    m_actionUndo->setIcon(createIconSet(QLatin1String("undo.png")));
    m_actionRedo = m_undoGroup->createRedoAction(this);
    m_actionRedo->setEnabled(false);
    m_actionRedo->setIcon(createIconSet(QLatin1String("redo.png")));
}

void FormWindowManager::slotActionCutActivated()
{
    m_activeFormWindow->cut();
}

void FormWindowManager::slotActionCopyActivated()
{
    m_activeFormWindow->copy();
    slotUpdateActions();
}

void FormWindowManager::slotActionPasteActivated()
{
    m_activeFormWindow->paste();
}

void FormWindowManager::slotActionDeleteActivated()
{
    m_activeFormWindow->deleteWidgets();
}

void FormWindowManager::slotActionLowerActivated()
{
    m_activeFormWindow->lowerWidgets();
}

void FormWindowManager::slotActionRaiseActivated()
{
    m_activeFormWindow->raiseWidgets();
}

void FormWindowManager::slotActionHorizontalLayoutActivated()
{
    if (m_layoutChilds)
        layoutContainerHorizontal();
    else
        m_activeFormWindow->layoutHorizontal();
}

void FormWindowManager::slotActionVerticalLayoutActivated()
{
    if (m_layoutChilds)
        layoutContainerVertical();
    else
        m_activeFormWindow->layoutVertical();
}

void FormWindowManager::slotActionFormLayoutActivated()
{
    if (m_layoutChilds)
        layoutContainerFormLayout();
    else
        m_activeFormWindow->layoutFormLayout();
}

void FormWindowManager::slotActionGridLayoutActivated()
{
    if (m_layoutChilds)
        layoutContainerGrid();
    else
        m_activeFormWindow->layoutGrid();
}

void FormWindowManager::slotActionSplitHorizontalActivated()
{
    if (m_layoutChilds)
        ; // no way to do that
    else
        m_activeFormWindow->layoutHorizontalSplit();
}

void FormWindowManager::slotActionSplitVerticalActivated()
{
    if (m_layoutChilds)
        ; // no way to do that
    else
        m_activeFormWindow->layoutVerticalSplit();
}

void FormWindowManager::slotActionBreakLayoutActivated()
{
    const QList<QWidget *> layouts = layoutsToBeBroken();
    if (layouts.isEmpty())
        return;

    if (debugFWM) {
        qDebug() << "slotActionBreakLayoutActivated: " << layouts.size();
        foreach (QWidget *w, layouts) {
            qDebug() << w;
        }
    }

    m_activeFormWindow->beginCommand(tr("Break Layout"));
    foreach (QWidget *layout, layouts) {
        m_activeFormWindow->breakLayout(layout);
    }
    m_activeFormWindow->endCommand();
}

void FormWindowManager::slotActionSimplifyLayoutActivated()
{
    Q_ASSERT(m_activeFormWindow != 0);
    QWidgetList selectedWidgets = m_activeFormWindow->selectedWidgets();
    m_activeFormWindow->simplifySelection(&selectedWidgets);
    if (selectedWidgets.size() != 1)
        return;
    SimplifyLayoutCommand *cmd = new SimplifyLayoutCommand(m_activeFormWindow);
    if (cmd->init(selectedWidgets.front())) {
        m_activeFormWindow->commandHistory()->push(cmd);
    } else {
        delete cmd;
    }
}

void FormWindowManager::slotActionAdjustSizeActivated()
{
    Q_ASSERT(m_activeFormWindow != 0);

    m_activeFormWindow->beginCommand(tr("Adjust Size"));

    QList<QWidget*> selectedWidgets = m_activeFormWindow->selectedWidgets();
    m_activeFormWindow->simplifySelection(&selectedWidgets);

    if (selectedWidgets.isEmpty()) {
        Q_ASSERT(m_activeFormWindow->mainContainer() != 0);
        selectedWidgets.append(m_activeFormWindow->mainContainer());
    }

    foreach (QWidget *widget, selectedWidgets) {
        bool unlaidout = LayoutInfo::layoutType(core(), widget->parentWidget()) == LayoutInfo::NoLayout;
        bool isMainContainer = m_activeFormWindow->isMainContainer(widget);

        if (unlaidout || isMainContainer) {
            AdjustWidgetSizeCommand *cmd = new AdjustWidgetSizeCommand(m_activeFormWindow);
            cmd->init(widget);
            m_activeFormWindow->commandHistory()->push(cmd);
        }
    }

    m_activeFormWindow->endCommand();
}

void FormWindowManager::slotActionSelectAllActivated()
{
    m_activeFormWindow->selectAll();
}


QList<QWidget *> FormWindowManager::layoutsToBeBroken(QWidget *w) const
{
    if (!w)
        return QList<QWidget *>();

    if (debugFWM)
        qDebug() << "layoutsToBeBroken: " << w;

    QWidget *parent = w->parentWidget();
    if (m_activeFormWindow->isMainContainer(w))
        parent = 0;

    QWidget *widget = core()->widgetFactory()->containerOfWidget(w);

    // maybe we want to remove following block
    const QDesignerWidgetDataBaseInterface *db = m_core->widgetDataBase();
    const QDesignerWidgetDataBaseItemInterface *item = db->item(db->indexOfObject(widget));
    if (!item) {
        if (debugFWM)
            qDebug() << "layoutsToBeBroken: Don't have an item, recursing for parent";
        return layoutsToBeBroken(parent);
    }

    const bool layoutContainer = (item->isContainer() || m_activeFormWindow->isMainContainer(widget));

    if (!layoutContainer) {
        if (debugFWM)
            qDebug() << "layoutsToBeBroken: Not a container, recursing for parent";
        return layoutsToBeBroken(parent);
    }

    QLayout *widgetLayout = widget->layout();
    QLayout *managedLayout = LayoutInfo::managedLayout(m_core, widgetLayout);
    if (!managedLayout) {
        if (qobject_cast<const QSplitter *>(widget)) {
            if (debugFWM)
                qDebug() << "layoutsToBeBroken: Splitter special";
            QList<QWidget *> list = layoutsToBeBroken(parent);
            list.append(widget);
            return list;
        }
        if (debugFWM)
            qDebug() << "layoutsToBeBroken: Is a container but doesn't have a managed layout (has an internal layout), returning 0";
        return QList<QWidget *>();
    }

    if (managedLayout) {
        QList<QWidget *> list;
        if (debugFWM)
            qDebug() << "layoutsToBeBroken: Is a container and has a layout";
        if (qobject_cast<const QLayoutWidget *>(widget)) {
            if (debugFWM)
                qDebug() << "layoutsToBeBroken: red layout special case";
            list = layoutsToBeBroken(parent);
        }
        list.append(widget);
        return list;
    }
    if (debugFWM)
        qDebug() << "layoutsToBeBroken: Is a container but doesn't have a layout at all, returning 0";
    return QList<QWidget *>();

}

QMap<QWidget *, bool> FormWindowManager::getUnsortedLayoutsToBeBroken(bool firstOnly) const
{
    // Return a set of layouts to be broken.
    QMap<QWidget *, bool> layouts;

    QList<QWidget *> selection = m_activeFormWindow->selectedWidgets();
    if (selection.isEmpty() && m_activeFormWindow->mainContainer())
        selection.append(m_activeFormWindow->mainContainer());

    const QList<QWidget *>::const_iterator scend = selection.constEnd();
    for (QList<QWidget *>::const_iterator sit = selection.constBegin(); sit != scend; ++sit) {
        // find all layouts
        const QList<QWidget *> list = layoutsToBeBroken(*sit);
        if (!list.empty()) {
            const QList<QWidget *>::const_iterator lbcend = list.constEnd();
            for (QList<QWidget *>::const_iterator lbit = list.constBegin(); lbit != lbcend; ++lbit) {
                layouts.insert(*lbit, true);
            }
            if (firstOnly)
                return layouts;
        }
    }
    return layouts;
}

bool FormWindowManager::hasLayoutsToBeBroken() const
{
    // Quick check for layouts to be broken
    return !getUnsortedLayoutsToBeBroken(true).isEmpty();
}

QList<QWidget *> FormWindowManager::layoutsToBeBroken() const
{
    // Get all layouts

    QMap<QWidget *, bool> unsortedLayouts = getUnsortedLayoutsToBeBroken(false);
    // Sort in order of hierarchy
    QList<QWidget *> orderedLayoutList;
    const QMap<QWidget *, bool>::const_iterator lscend  = unsortedLayouts.constEnd();
    for (QMap<QWidget *, bool>::const_iterator itLay = unsortedLayouts.constBegin(); itLay != lscend; ++itLay) {
        QWidget *wToBeInserted = itLay.key();
        if (!orderedLayoutList.contains(wToBeInserted)) {
            // try to find first child, use as insertion position, else append
            const QList<QWidget *>::iterator firstChildPos = findFirstChildOf(orderedLayoutList.begin(), orderedLayoutList.end(), wToBeInserted);
            if (firstChildPos == orderedLayoutList.end()) {
                orderedLayoutList.push_back(wToBeInserted);
            } else {
                orderedLayoutList.insert(firstChildPos, wToBeInserted);
            }
        }
    }
    return orderedLayoutList;
}

void FormWindowManager::slotUpdateActions()
{
    m_layoutChilds = false;

    int selectedWidgetCount = 0;
    int laidoutWidgetCount = 0;
    int unlaidoutWidgetCount = 0;
    bool pasteAvailable = false;
    bool layoutAvailable = false;
    bool breakAvailable = false;
    bool simplifyAvailable = false;
    bool layoutContainer = false;

    do {
        if (m_activeFormWindow == 0 || m_activeFormWindow->currentTool() != 0)
            break;

        breakAvailable = hasLayoutsToBeBroken();

        QList<QWidget*> simplifiedSelection = m_activeFormWindow->selectedWidgets();

        selectedWidgetCount = simplifiedSelection.count();
        pasteAvailable = qApp->clipboard()->mimeData() && qApp->clipboard()->mimeData()->hasText();

        m_activeFormWindow->simplifySelection(&simplifiedSelection);
        if (simplifiedSelection.isEmpty() && m_activeFormWindow->mainContainer())
            simplifiedSelection.append(m_activeFormWindow->mainContainer());

        foreach (QWidget *widget, simplifiedSelection) {
            if (LayoutInfo::isWidgetLaidout(m_core, widget))
                ++laidoutWidgetCount;
            else
                ++unlaidoutWidgetCount;
        }

        // Figure out layouts: Looking at a group of dangling widgets
        if (simplifiedSelection.count() != 1) {
            layoutAvailable = unlaidoutWidgetCount > 1;
            //breakAvailable = false;
            break;
        }
        // Manipulate layout of a single widget
        m_layoutChilds = false;
        QWidget *widget = core()->widgetFactory()->containerOfWidget(simplifiedSelection.first());

        const QDesignerWidgetDataBaseInterface *db = m_core->widgetDataBase();
        const QDesignerWidgetDataBaseItemInterface *item = db->item(db->indexOfObject(widget));
        if (!item)
            break;

        QLayout *widgetLayout = LayoutInfo::internalLayout(widget);
        QLayout *managedLayout = LayoutInfo::managedLayout(m_core, widgetLayout);
        // We don't touch a layout createds by a custom widget
        if (widgetLayout && !managedLayout)
            break;

        layoutContainer = (item->isContainer() || m_activeFormWindow->isMainContainer(widget));

        layoutAvailable = layoutContainer && m_activeFormWindow->hasInsertedChildren(widget) && managedLayout == 0;
        simplifyAvailable = SimplifyLayoutCommand::canSimplify(m_core, widget);
        m_layoutChilds = layoutAvailable;

    } while(false);

    m_actionCut->setEnabled(selectedWidgetCount > 0);
    m_actionCopy->setEnabled(selectedWidgetCount > 0);
    m_actionDelete->setEnabled(selectedWidgetCount > 0);
    m_actionLower->setEnabled(selectedWidgetCount > 0);
    m_actionRaise->setEnabled(selectedWidgetCount > 0);

    m_actionPaste->setEnabled(pasteAvailable);

    m_actionSelectAll->setEnabled(m_activeFormWindow != 0);

    m_actionAdjustSize->setEnabled(unlaidoutWidgetCount > 0);

    m_actionHorizontalLayout->setEnabled(layoutAvailable);
    m_actionVerticalLayout->setEnabled(layoutAvailable);
    m_actionSplitHorizontal->setEnabled(layoutAvailable && !layoutContainer);
    m_actionSplitVertical->setEnabled(layoutAvailable && !layoutContainer);
    actionFormLayout()->setEnabled(layoutAvailable);
    m_actionGridLayout->setEnabled(layoutAvailable);

    m_actionBreakLayout->setEnabled(breakAvailable);
    actionSimplifyLayout()->setEnabled(simplifyAvailable);
}

static inline QWidget *findLayoutContainer(const FormWindow *fw)
{
    QWidget *w = fw->mainContainer();
    QList<QWidget*> l(fw->selectedWidgets());
    fw->simplifySelection(&l);
    return l.empty() ? w : l.front();
}

void FormWindowManager::layoutContainerHorizontal()
{
    if (QWidget *container = findLayoutContainer(m_activeFormWindow))
        m_activeFormWindow->layoutHorizontalContainer(container);
}

void FormWindowManager::layoutContainerVertical()
{
    if (QWidget *container = findLayoutContainer(m_activeFormWindow))
        m_activeFormWindow->layoutVerticalContainer(container);
}

void FormWindowManager::layoutContainerGrid()
{
    if (QWidget *container = findLayoutContainer(m_activeFormWindow))
        m_activeFormWindow->layoutGridContainer(container);
}

void FormWindowManager::layoutContainerFormLayout()
{
    if (QWidget *container = findLayoutContainer(m_activeFormWindow))
        m_activeFormWindow->layoutFormLayoutContainer(container);
}

QDesignerFormWindowInterface *FormWindowManager::createFormWindow(QWidget *parentWidget, Qt::WindowFlags flags)
{
    FormWindow *formWindow = new FormWindow(qobject_cast<FormEditor*>(core()), parentWidget, flags);
    addFormWindow(formWindow);
    return formWindow;
}

QAction *FormWindowManager::actionUndo() const
{
    return m_actionUndo;
}

QAction *FormWindowManager::actionRedo() const
{
    return m_actionRedo;
}

// DnD stuff

void FormWindowManager::dragItems(const QList<QDesignerDnDItemInterface*> &item_list)
{
    QDesignerMimeData::execDrag(item_list, m_core->topLevel());
}

QUndoGroup *FormWindowManager::undoGroup() const
{
    return m_undoGroup;
}
}

QT_END_NAMESPACE
