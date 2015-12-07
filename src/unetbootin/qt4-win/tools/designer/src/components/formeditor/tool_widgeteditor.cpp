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
TRANSLATOR qdesigner_internal::WidgetEditorTool
*/

#include "tool_widgeteditor.h"
#include "formwindow.h"

// sdk
#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QDesignerWidgetFactoryInterface>
#include <QtDesigner/QDesignerWidgetBoxInterface>

#include <layoutinfo_p.h>
#include <qdesigner_dnditem_p.h>

#include <QtGui/qevent.h>
#include <QtGui/QAction>
#include <QtGui/QMainWindow>
#include <QtGui/QCursor>
#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

using namespace qdesigner_internal;

WidgetEditorTool::WidgetEditorTool(FormWindow *formWindow)
    : QDesignerFormWindowToolInterface(formWindow),
      m_formWindow(formWindow),
      m_action(new QAction(tr("Edit Widgets"), this))
{
}

QAction *WidgetEditorTool::action() const
{
    return m_action;
}

WidgetEditorTool::~WidgetEditorTool()
{
}

QDesignerFormEditorInterface *WidgetEditorTool::core() const
{
    return m_formWindow->core();
}

QDesignerFormWindowInterface *WidgetEditorTool::formWindow() const
{
    return m_formWindow;
}

bool WidgetEditorTool::mainWindowSeparatorEvent(QWidget *widget, QEvent *event)
{
    QMainWindow *mw = qobject_cast<QMainWindow*>(widget);
    if (mw == 0)
        return false;

    if (event->type() != QEvent::MouseButtonPress
            && event->type() != QEvent::MouseMove
            && event->type() != QEvent::MouseButtonRelease)
        return false;

    QMouseEvent *e = static_cast<QMouseEvent*>(event);

    if (event->type() == QEvent::MouseButtonPress) {
        if (mw->isSeparator(e->pos())) {
            m_separator_drag_mw = mw;
            return true;
        }
        return false;
    }

    if (event->type() == QEvent::MouseMove)
        return m_separator_drag_mw == mw;

    if (event->type() == QEvent::MouseButtonRelease) {
        if (m_separator_drag_mw != mw)
            return false;
        m_separator_drag_mw = 0;
        return true;
    }

    return false;
}

bool WidgetEditorTool::handleEvent(QWidget *widget, QWidget *managedWidget, QEvent *event)
{
    const bool passive = core()->widgetFactory()->isPassiveInteractor(widget) != 0
                    || mainWindowSeparatorEvent(widget, event); // separators in QMainWindow
                                                                // are no longer widgets
    switch (event->type()) {
    case QEvent::Resize:
    case QEvent::Move:
        m_formWindow->updateSelection(widget);
        if (event->type() != QEvent::Resize)
            m_formWindow->updateChildSelections(widget);
        break;

    case QEvent::FocusOut:
    case QEvent::FocusIn: // Popup cancelled over a form widget: Reset its focus frame
        return !(passive || widget == m_formWindow || widget == m_formWindow->mainContainer());

    case QEvent::Wheel: // Prevent spinboxes and combos from reacting
        return !passive;

    case QEvent::KeyPress:
        return !passive && handleKeyPressEvent(widget, managedWidget, static_cast<QKeyEvent*>(event));

    case QEvent::KeyRelease:
        return !passive && handleKeyReleaseEvent(widget, managedWidget, static_cast<QKeyEvent*>(event));

    case QEvent::MouseMove:
        return !passive && handleMouseMoveEvent(widget, managedWidget, static_cast<QMouseEvent*>(event));

    case QEvent::MouseButtonPress:
        return !passive && handleMousePressEvent(widget, managedWidget, static_cast<QMouseEvent*>(event));

    case QEvent::MouseButtonRelease:
        return !passive && handleMouseReleaseEvent(widget, managedWidget, static_cast<QMouseEvent*>(event));

    case QEvent::MouseButtonDblClick:
        return !passive && handleMouseButtonDblClickEvent(widget, managedWidget, static_cast<QMouseEvent*>(event));

    case QEvent::ContextMenu:
        return !passive && handleContextMenu(widget, managedWidget, static_cast<QContextMenuEvent*>(event));

    case QEvent::DragEnter:
        return handleDragEnterMoveEvent(widget, managedWidget, static_cast<QDragEnterEvent *>(event), true);
    case QEvent::DragMove:
        return handleDragEnterMoveEvent(widget, managedWidget, static_cast<QDragEnterEvent *>(event), false);
    case QEvent::DragLeave:
        return handleDragLeaveEvent(widget, managedWidget, static_cast<QDragLeaveEvent *>(event));
    case QEvent::Drop:
        return handleDropEvent(widget, managedWidget, static_cast<QDropEvent *>(event));
    default:
        break;

    } // end switch

    return false;
}

// ### remove me

bool WidgetEditorTool::handleContextMenu(QWidget *widget, QWidget *managedWidget, QContextMenuEvent *e)
{
    return m_formWindow->handleContextMenu(widget, managedWidget, e);
}

bool WidgetEditorTool::handleMouseButtonDblClickEvent(QWidget *widget, QWidget *managedWidget, QMouseEvent *e)
{
    return m_formWindow->handleMouseButtonDblClickEvent(widget, managedWidget, e);
}

bool WidgetEditorTool::handleMousePressEvent(QWidget *widget, QWidget *managedWidget, QMouseEvent *e)
{
    return m_formWindow->handleMousePressEvent(widget, managedWidget, e);
}

bool WidgetEditorTool::handleMouseMoveEvent(QWidget *widget, QWidget *managedWidget, QMouseEvent *e)
{
    return m_formWindow->handleMouseMoveEvent(widget, managedWidget, e);
}

bool WidgetEditorTool::handleMouseReleaseEvent(QWidget *widget, QWidget *managedWidget, QMouseEvent *e)
{
    return m_formWindow->handleMouseReleaseEvent(widget, managedWidget, e);
}

bool WidgetEditorTool::handleKeyPressEvent(QWidget *widget, QWidget *managedWidget, QKeyEvent *e)
{
    return m_formWindow->handleKeyPressEvent(widget, managedWidget, e);
}

bool WidgetEditorTool::handleKeyReleaseEvent(QWidget *widget, QWidget *managedWidget, QKeyEvent *e)
{
    return m_formWindow->handleKeyReleaseEvent(widget, managedWidget, e);
}

bool WidgetEditorTool::handlePaintEvent(QWidget *widget, QWidget *managedWidget, QPaintEvent *e)
{
    Q_UNUSED(widget);
    Q_UNUSED(managedWidget);
    Q_UNUSED(e);

    return false;
}

bool WidgetEditorTool::handleDragEnterMoveEvent(QWidget *, QWidget *, QDragMoveEvent *e, bool isEnter)
{
    const QDesignerMimeData *mimeData = qobject_cast<const QDesignerMimeData *>(e->mimeData());
    if (!mimeData)
        return false;

    if (!m_formWindow->hasFeature(QDesignerFormWindowInterface::EditFeature)) {
        e->ignore();
        return true;
    }

    const QPoint formPos = e->pos();
    const QPoint globalPos = m_formWindow->mapToGlobal(formPos);
    const FormWindowBase::WidgetUnderMouseMode wum = mimeData->items().size() == 1 ? FormWindowBase::FindSingleSelectionDropTarget : FormWindowBase::FindMultiSelectionDropTarget;
    QWidget *dropTarget = m_formWindow->widgetUnderMouse(formPos, wum);
    if (m_lastDropTarget && dropTarget != m_lastDropTarget)
        m_formWindow->highlightWidget(m_lastDropTarget, m_lastDropTarget->mapFromGlobal(globalPos), FormWindow::Restore);

    m_lastDropTarget = dropTarget;

    if (m_lastDropTarget)
        m_formWindow->highlightWidget(m_lastDropTarget, m_lastDropTarget->mapFromGlobal(globalPos), FormWindow::Highlight);

    if (isEnter || m_lastDropTarget)
        mimeData->acceptEvent(e);
    else
        e->ignore();
    return true;
}

bool WidgetEditorTool::handleDropEvent(QWidget *widget, QWidget *, QDropEvent *e)
{
    const QDesignerMimeData *mimeData = qobject_cast<const QDesignerMimeData *>(e->mimeData());
    if (!mimeData)
        return false;

    if (!m_lastDropTarget ||
        !m_formWindow->hasFeature(QDesignerFormWindowInterface::EditFeature)) {
        e->ignore();
        return true;
    }
    // FormWindow determines the position from the decoration.
    const QPoint globalPos = widget->mapToGlobal(e->pos());
    mimeData->moveDecoration(globalPos);
    if (!m_formWindow->dropWidgets(mimeData->items(), m_lastDropTarget, globalPos)) {
        e->ignore();
        return true;
    }
    mimeData->acceptEvent(e);
    return true;
}

bool WidgetEditorTool::restoreDropHighlighting()
{
    if (!m_lastDropTarget)
        return false;

    m_formWindow->highlightWidget(m_lastDropTarget, m_lastDropTarget->mapFromGlobal(QCursor::pos()), FormWindow::Restore);
    m_lastDropTarget = 0;
    return true;
}

bool WidgetEditorTool::handleDragLeaveEvent(QWidget *, QWidget *, QDragLeaveEvent *event)
{
    if (restoreDropHighlighting()) {
        event->accept();
        return true;
    }
    return false;
}

QWidget *WidgetEditorTool::editor() const
{
    Q_ASSERT(formWindow() != 0);
    return formWindow()->mainContainer();
}

void WidgetEditorTool::activated()
{
    if (core()->widgetBox())
        core()->widgetBox()->setEnabled(true);

    if (m_formWindow == 0)
        return;

    QList<QWidget*> sel = m_formWindow->selectedWidgets();
    foreach (QWidget *w, sel)
        m_formWindow->raiseSelection(w);
}

void WidgetEditorTool::deactivated()
{
    if (core()->widgetBox())
        core()->widgetBox()->setEnabled(false);

    if (m_formWindow == 0)
        return;

    m_formWindow->clearSelection();
}

QT_END_NAMESPACE
