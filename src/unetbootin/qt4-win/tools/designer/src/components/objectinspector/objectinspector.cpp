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
TRANSLATOR qdesigner_internal::ObjectInspector
*/

#include "objectinspector.h"
#include "objectinspectormodel_p.h"
#include "formwindow.h"

// sdk
#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QDesignerTaskMenuExtension>
#include <QtDesigner/QExtensionManager>
#include <QtDesigner/QDesignerFormWindowCursorInterface>
#include <QtDesigner/QDesignerFormWindowManagerInterface>
#include <QtDesigner/QDesignerContainerExtension>
#include <QtDesigner/QDesignerMetaDataBaseInterface>
#include <QtDesigner/QDesignerPropertyEditorInterface>

// shared
#include <formwindowbase_p.h>
#include <qdesigner_dnditem_p.h>
#include <textpropertyeditor_p.h>

// Qt
#include <QtGui/QApplication>
#include <QtGui/QHeaderView>
#include <QtGui/QScrollBar>
#include <QtGui/QPainter>
#include <QtGui/QVBoxLayout>
#include <QtGui/QItemSelectionModel>
#include <QtGui/QMenu>
#include <QtGui/QTreeView>
#include <QtGui/QItemDelegate>
#include <QtGui/qevent.h>

#include <QtCore/QVector>
#include <QtCore/QDebug>

QT_BEGIN_NAMESPACE

namespace {
    // Selections: Basically, ObjectInspector has to ensure a consistent
    // selection, that is, either form-managed widgets (represented
    // by the cursor interface selection), or unmanaged widgets/objects,
    // for example actions, container pages, menu bars, tool bars
    // and the like. The selection state of the latter is managed only in the object inspector.
    // As soon as a managed widget is selected, unmanaged objects
    // have to be unselected
    // Normally, an empty selection is not allowed, the main container
    // should be selected in this case (applyCursorSelection()).
    // An exception is when clearSelection is called directly for example
    // by the action editor that puts an unassociated action into the property
    // editor. A hack exists to avoid the update in this case.

    enum SelectionType {
        NoSelection,
        // A QObject that has a meta database entry
        QObjectSelection,
        // Unmanaged widget, menu bar or the like
        UnmanagedWidgetSelection,
        // A widget managed by the form window cursor
        ManagedWidgetSelection };

    typedef QVector<QObject*> QObjectVector;
}

static inline SelectionType selectionType(const QDesignerFormWindowInterface *fw, QObject *o)
{
    if (!o->isWidgetType())
        return fw->core()->metaDataBase()->item(o) ?  QObjectSelection : NoSelection;
    return fw->isManaged(qobject_cast<QWidget *>(o)) ? ManagedWidgetSelection :  UnmanagedWidgetSelection;
}

// Return an offset for dropping (when dropping widgets on the object
// inspector, we fake a position on the form based on the widget dropped on).
// Position the dropped widget with form grid offset to avoid overlapping unless we
// drop on a layout. Position doesn't matter in the layout case
// and this enables us to drop on a squeezed layout widget of size zero

static inline QPoint dropPointOffset(const qdesigner_internal::FormWindowBase *fw, const QWidget *dropTarget)
{
    if (!dropTarget || dropTarget->layout())
        return QPoint(0, 0);
    return QPoint(fw->designerGrid().deltaX(), fw->designerGrid().deltaY());
}

namespace qdesigner_internal {
// Delegate with object name validator for the object name column
class ObjectInspectorDelegate : public QItemDelegate {
public:
    explicit ObjectInspectorDelegate(QObject *parent = 0);

    virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

ObjectInspectorDelegate::ObjectInspectorDelegate(QObject *parent) :
    QItemDelegate(parent)
{
}

QWidget *ObjectInspectorDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem & option, const QModelIndex &index) const
{
    if (index.column() != ObjectInspectorModel::ObjectNameColumn)
        return QItemDelegate::createEditor(parent, option, index);
    // Object name editor
    const bool isMainContainer = !index.parent().isValid();
    return new TextPropertyEditor(parent, TextPropertyEditor::EmbeddingTreeView,
                                  isMainContainer ? ValidationObjectNameScope : ValidationObjectName);
}

// ------------ ObjectInspectorTreeView:
// - Makes the Space key start editing
// - Suppresses a range selection by dragging or Shift-up/down, which does not really work due
//   to the need to maintain a consistent selection.

class ObjectInspectorTreeView : public QTreeView {
public:
    ObjectInspectorTreeView(QWidget *parent = 0) :  QTreeView(parent) {}

protected:
    virtual void mouseMoveEvent (QMouseEvent * event);
    virtual void keyPressEvent(QKeyEvent *event);

};

void ObjectInspectorTreeView::mouseMoveEvent(QMouseEvent *event)
{
    event->ignore(); // suppress a range selection by dragging
}

void ObjectInspectorTreeView::keyPressEvent(QKeyEvent *event)
{
    bool handled = false;
    switch (event->key()) {
    case Qt::Key_Up:
    case Qt::Key_Down: // suppress shift-up/down range selection
        if (event->modifiers() & Qt::ShiftModifier) {
            event->ignore();
            handled = true;
        }
        break;
    case Qt::Key_Space: { // Space pressed: Start editing
        const QModelIndex index = currentIndex();
        if (index.isValid() && index.column() == 0 && !model()->hasChildren(index) && model()->flags(index) & Qt::ItemIsEditable) {
            event->accept();
            handled = true;
            edit(index);
        }
    }
        break;
    default:
        break;
    }
    if (!handled)
        QTreeView::keyPressEvent(event);
}

// ------------ ObjectInspectorPrivate

class ObjectInspector::ObjectInspectorPrivate {
public:
    ObjectInspectorPrivate(QDesignerFormEditorInterface *core);

    QTreeView *treeView() const { return m_treeView; }
    QDesignerFormEditorInterface *core() const { return m_core; }
    const QPointer<FormWindowBase> &formWindow() const { return m_formWindow; }

    void clear();
    void setFormWindow(QDesignerFormWindowInterface *fwi);

    QWidget *managedWidgetAt(const QPoint &global_mouse_pos);

    void restoreDropHighlighting();
    void handleDragEnterMoveEvent(const QWidget *objectInspectorWidget, QDragMoveEvent * event, bool isDragEnter);
    void dropEvent (QDropEvent * event);

    void clearSelection();
    bool selectObject(QObject *o);
    void slotSelectionChanged(const QItemSelection & selected, const QItemSelection &deselected);
    void getSelection(Selection &s) const;

    void slotHeaderDoubleClicked(int column)       {  m_treeView->resizeColumnToContents(column); }
    void slotPopupContextMenu(QWidget *parent, const QPoint &pos);

private:
    void setFormWindowBlocked(QDesignerFormWindowInterface *fwi);
    void applyCursorSelection();
    void synchronizeSelection(const QItemSelection & selected, const QItemSelection &deselected);
    bool checkManagedWidgetSelection(const QModelIndexList &selection);
    void showContainersCurrentPage(QWidget *widget);

    enum  SelectionFlags { AddToSelection = 1, MakeCurrent = 2};
    void selectIndexRange(const QModelIndexList &indexes, unsigned flags);

    QDesignerFormEditorInterface *m_core;
    QTreeView *m_treeView;
    ObjectInspectorModel *m_model;
    QPointer<FormWindowBase> m_formWindow;
    QPointer<QWidget> m_formFakeDropTarget;
    bool m_withinClearSelection;
};

ObjectInspector::ObjectInspectorPrivate::ObjectInspectorPrivate(QDesignerFormEditorInterface *core) :
    m_core(core),
    m_treeView(new ObjectInspectorTreeView),
    m_model(new ObjectInspectorModel(m_treeView)),
    m_withinClearSelection(false)
{
    m_treeView->setModel(m_model);
    m_treeView->setItemDelegate(new ObjectInspectorDelegate);
    m_treeView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    m_treeView->header()->setResizeMode(1, QHeaderView::Stretch);
    m_treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_treeView->setAlternatingRowColors(true);
    m_treeView->setTextElideMode (Qt::ElideMiddle);

    m_treeView->setContextMenuPolicy(Qt::CustomContextMenu);
}

void ObjectInspector::ObjectInspectorPrivate::clearSelection()
{
    m_withinClearSelection = true;
    m_treeView->clearSelection();
    m_withinClearSelection = false;
}

QWidget *ObjectInspector::ObjectInspectorPrivate::managedWidgetAt(const QPoint &global_mouse_pos)
{
    if (!m_formWindow)
        return 0;

    const  QPoint pos = m_treeView->viewport()->mapFromGlobal(global_mouse_pos);
    QObject *o = m_model->objectAt(m_treeView->indexAt(pos));

    if (!o || !o->isWidgetType())
        return 0;

    QWidget *rc = qobject_cast<QWidget *>(o);
    if (!m_formWindow->isManaged(rc))
        return 0;
    return rc;
}

void ObjectInspector::ObjectInspectorPrivate::showContainersCurrentPage(QWidget *widget)
{
    if (!widget)
        return;

    FormWindow *fw = FormWindow::findFormWindow(widget);
    if (!fw)
        return;

    QWidget *w = widget->parentWidget();
    // Find a multipage container (tab widgets, etc.) in the hierarchy and set the right page.
    while (w != 0) {
        if (fw->isManaged(w)) { // Rule out unmanaged internal scroll areas, for example, on QToolBoxes.
            if (QDesignerContainerExtension *c = qt_extension<QDesignerContainerExtension*>(m_core->extensionManager(), w)) {
                const int count = c->count();
                if (count > 1 && !c->widget(c->currentIndex())->isAncestorOf(widget)) {
                    for (int i = 0; i < count; i++)
                        if (c->widget(i)->isAncestorOf(widget)) {
                            c->setCurrentIndex(i);
                            break;
                        }
                }
            }
        }
        w = w->parentWidget();
    }
}

void ObjectInspector::ObjectInspectorPrivate::restoreDropHighlighting()
{
    if (m_formFakeDropTarget) {
        if (m_formWindow) {
            m_formWindow->highlightWidget(m_formFakeDropTarget, QPoint(5, 5), FormWindow::Restore);
        }
        m_formFakeDropTarget = 0;
    }
}

void ObjectInspector::ObjectInspectorPrivate::handleDragEnterMoveEvent(const QWidget *objectInspectorWidget, QDragMoveEvent * event, bool isDragEnter)
{
    if (!m_formWindow) {
        event->ignore();
        return;
    }

    const QDesignerMimeData *mimeData =  qobject_cast<const QDesignerMimeData *>(event->mimeData());
    if (!mimeData) {
        event->ignore();
        return;
    }

    QWidget *dropTarget = 0;
    QPoint fakeDropTargetOffset = QPoint(0, 0);
    if (QWidget *managedWidget = managedWidgetAt(objectInspectorWidget->mapToGlobal(event->pos()))) {
        fakeDropTargetOffset = dropPointOffset(m_formWindow, managedWidget);
        // pretend we drag over the managed widget on the form
        const QPoint fakeFormPos = m_formWindow->mapFromGlobal(managedWidget->mapToGlobal(fakeDropTargetOffset));
        const FormWindowBase::WidgetUnderMouseMode wum = mimeData->items().size() == 1 ? FormWindowBase::FindSingleSelectionDropTarget : FormWindowBase::FindMultiSelectionDropTarget;
        dropTarget = m_formWindow->widgetUnderMouse(fakeFormPos, wum);
    }

    if (m_formFakeDropTarget && dropTarget != m_formFakeDropTarget)
        m_formWindow->highlightWidget(m_formFakeDropTarget, fakeDropTargetOffset, FormWindow::Restore);

    m_formFakeDropTarget =  dropTarget;
    if (m_formFakeDropTarget)
        m_formWindow->highlightWidget(m_formFakeDropTarget, fakeDropTargetOffset, FormWindow::Highlight);

    // Do not refuse drag enter even if the area is not droppable
    if (isDragEnter || m_formFakeDropTarget)
        mimeData->acceptEvent(event);
    else
        event->ignore();
}
void  ObjectInspector::ObjectInspectorPrivate::dropEvent (QDropEvent * event)
{
    if (!m_formWindow || !m_formFakeDropTarget) {
        event->ignore();
        return;
    }

    const QDesignerMimeData *mimeData =  qobject_cast<const QDesignerMimeData *>(event->mimeData());
    if (!mimeData) {
        event->ignore();
        return;
    }
    const QPoint fakeGlobalDropFormPos = m_formFakeDropTarget->mapToGlobal(dropPointOffset(m_formWindow , m_formFakeDropTarget));
    mimeData->moveDecoration(fakeGlobalDropFormPos + mimeData->hotSpot());
    if (!m_formWindow->dropWidgets(mimeData->items(), m_formFakeDropTarget, fakeGlobalDropFormPos)) {
        event->ignore();
        return;
    }
    mimeData->acceptEvent(event);
}

bool ObjectInspector::ObjectInspectorPrivate::selectObject(QObject *o)
{
    if (!m_core->metaDataBase()->item(o))
        return false;

    typedef QSet<QModelIndex> ModelIndexSet;

    const QModelIndexList objectIndexes = m_model->indexesOf(o);
    if (objectIndexes.empty())
        return false;

    QItemSelectionModel *selectionModel = m_treeView->selectionModel();
    const ModelIndexSet currentSelectedItems = selectionModel->selectedRows(0).toSet();

    // Change in selection?
    if (!currentSelectedItems.empty() && currentSelectedItems == objectIndexes.toSet())
        return true;

    // do select and update
    selectIndexRange(objectIndexes, MakeCurrent);
    return true;
}

void ObjectInspector::ObjectInspectorPrivate::selectIndexRange(const QModelIndexList &indexes, unsigned flags)
{
    if (indexes.empty())
        return;

    QItemSelectionModel::SelectionFlags selectFlags = QItemSelectionModel::Select|QItemSelectionModel::Rows;
    if (!(flags & AddToSelection))
        selectFlags |= QItemSelectionModel::Clear;
    if (flags & MakeCurrent)
        selectFlags |= QItemSelectionModel::Current;

    QItemSelectionModel *selectionModel = m_treeView->selectionModel();
    const QModelIndexList::const_iterator cend = indexes.constEnd();
    for (QModelIndexList::const_iterator it = indexes.constBegin(); it != cend; ++it)
        if (it->column() == 0) {
            selectionModel->select(*it, selectFlags);
            selectFlags &= ~(QItemSelectionModel::Clear|QItemSelectionModel::Current);
        }
    if (flags & MakeCurrent)
        m_treeView->scrollTo(indexes.front(), QAbstractItemView::EnsureVisible);
}

void ObjectInspector::ObjectInspectorPrivate::clear()
{
    m_formFakeDropTarget = 0;
    m_formWindow = 0;
}

// Form window cursor is in state 'main container only'
static inline bool mainContainerIsCurrent(const QDesignerFormWindowInterface *fw)
{
    const QDesignerFormWindowCursorInterface *cursor = fw->cursor();
    if (cursor->selectedWidgetCount() > 1)
        return false;
    const QWidget *current = cursor->current();
    return current == fw || current == fw->mainContainer();
}

void ObjectInspector::ObjectInspectorPrivate::setFormWindow(QDesignerFormWindowInterface *fwi)
{
    const bool blocked = m_treeView->selectionModel()->blockSignals(true);
    m_treeView->setUpdatesEnabled(false);
    setFormWindowBlocked(fwi);
    m_treeView->setUpdatesEnabled(true);
    m_treeView->update();
    m_treeView->selectionModel()->blockSignals(blocked);
}

void ObjectInspector::ObjectInspectorPrivate::setFormWindowBlocked(QDesignerFormWindowInterface *fwi)
{
    FormWindowBase *fw = qobject_cast<FormWindowBase *>(fwi);
    const bool formWindowChanged = m_formWindow != fw;

    m_formWindow = fw;

    const int oldWidth = m_treeView->columnWidth(0);
    const int xoffset = m_treeView->horizontalScrollBar()->value();
    const int yoffset = m_treeView->verticalScrollBar()->value();

    if (formWindowChanged)
        m_formFakeDropTarget = 0;

    switch (m_model->update(m_formWindow)) {
    case ObjectInspectorModel::NoForm:
        clear();
        return;
    case ObjectInspectorModel::Rebuilt: // Complete rebuild: Just apply cursor selection
        applyCursorSelection();
        m_treeView->expandAll();
        if (formWindowChanged) {
            m_treeView->resizeColumnToContents(0);
        } else {
            m_treeView->setColumnWidth(0, oldWidth);
            m_treeView->horizontalScrollBar()->setValue(xoffset);
            m_treeView->verticalScrollBar()->setValue(yoffset);
        }
        break;
    case ObjectInspectorModel::Updated: {
        // Same structure (property changed or click on the form)
        // We maintain a selection of unmanaged objects
        // only if the cursor is in state "mainContainer() == current".
        // and we have a non-managed selection.
        // Else we take over the cursor selection.
        bool applySelection = !mainContainerIsCurrent(m_formWindow);
        if (!applySelection) {
            const QModelIndexList currentIndexes = m_treeView->selectionModel()->selectedRows(0);
            if (currentIndexes.empty()) {
                applySelection = true;
            } else {
                applySelection = selectionType(m_formWindow, m_model->objectAt(currentIndexes.front())) == ManagedWidgetSelection;
            }
        }
        if (applySelection)
            applyCursorSelection();
    }
        break;
    }
}

// Apply selection of form window cursor to object inspector, set current
void ObjectInspector::ObjectInspectorPrivate::applyCursorSelection()
{
    const QDesignerFormWindowCursorInterface *cursor = m_formWindow->cursor();
    const int count = cursor->selectedWidgetCount();
    if (!count)
        return;

    // Set the current widget first which also clears the selection
    QWidget *currentWidget = cursor->current();
    if (currentWidget)
        selectIndexRange(m_model->indexesOf(currentWidget), MakeCurrent);
    else
        m_treeView->selectionModel()->clearSelection();

    for (int i = 0;i < count; i++) {
        QWidget *widget = cursor->selectedWidget(i);
        if (widget != currentWidget)
            selectIndexRange(m_model->indexesOf(widget), AddToSelection);
    }
}

// Synchronize managed widget in the form (select in cursor). Block updates
static int selectInCursor(FormWindowBase *fw, const QObjectVector &objects, bool value)
{
    int rc = 0;
    const bool blocked = fw->blockSelectionChanged(true);
    const QObjectVector::const_iterator ocend = objects.constEnd();
    for (QObjectVector::const_iterator it = objects.constBegin(); it != ocend; ++it)
        if (selectionType(fw, *it) == ManagedWidgetSelection) {
            fw->selectWidget(static_cast<QWidget *>(*it), value);
            rc++;
        }
    fw->blockSelectionChanged(blocked);
    return rc;
}

void ObjectInspector::ObjectInspectorPrivate::slotSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    if (m_formWindow) {
        synchronizeSelection(selected, deselected);
        QMetaObject::invokeMethod(m_core->formWindowManager(), "slotUpdateActions");
    }
}

// Convert indexes to object vectors taking into account that
// some index lists are multicolumn ranges
static inline QObjectVector indexesToObjects(const ObjectInspectorModel *model, const QModelIndexList &indexes)
{
    if (indexes.empty())
        return  QObjectVector();
    QObjectVector rc;
    rc.reserve(indexes.size());
    const QModelIndexList::const_iterator icend = indexes.constEnd();
    for (QModelIndexList::const_iterator it = indexes.constBegin(); it != icend; ++it)
        if (it->column() == 0)
            rc.push_back(model->objectAt(*it));
    return rc;
}

// Check if any managed widgets are selected. If so, iterate over
// selection and deselect all unmanaged objects
bool ObjectInspector::ObjectInspectorPrivate::checkManagedWidgetSelection(const QModelIndexList &rowSelection)
{
    bool isManagedWidgetSelection = false;
    QItemSelectionModel *selectionModel = m_treeView->selectionModel();
    const QModelIndexList::const_iterator cscend = rowSelection.constEnd();
    for (QModelIndexList::const_iterator it = rowSelection.constBegin(); it != cscend; ++it) {
        QObject *object = m_model->objectAt(*it);
        if (selectionType(m_formWindow, object) == ManagedWidgetSelection) {
            isManagedWidgetSelection = true;
            break;
        }
    }

    if (!isManagedWidgetSelection)
        return false;
    // Need to unselect unmanaged ones
    const bool blocked = selectionModel->blockSignals(true);
    for (QModelIndexList::const_iterator it = rowSelection.constBegin(); it != cscend; ++it) {
        QObject *object = m_model->objectAt(*it);
        if (selectionType(m_formWindow, object) != ManagedWidgetSelection)
            selectionModel->select(*it, QItemSelectionModel::Deselect|QItemSelectionModel::Rows);
    }
    selectionModel->blockSignals(blocked);
    return true;
}

void ObjectInspector::ObjectInspectorPrivate::synchronizeSelection(const QItemSelection & selectedSelection, const QItemSelection &deselectedSelection)
{
    // Synchronize form window cursor.
    const QObjectVector deselected = indexesToObjects(m_model, deselectedSelection.indexes());
    const QObjectVector newlySelected = indexesToObjects(m_model, selectedSelection.indexes());

    const QModelIndexList currentSelectedIndexes = m_treeView->selectionModel()->selectedRows(0);

    int deselectedManagedWidgetCount = 0;
    if (!deselected.empty())
        deselectedManagedWidgetCount = selectInCursor(m_formWindow, deselected, false);

    if (newlySelected.empty()) { // Nothing selected
        if (currentSelectedIndexes.empty()) // Do not allow a null-selection, reset to main container
            m_formWindow->clearSelection(!m_withinClearSelection);
        return;
    }

    const int selectManagedWidgetCount = selectInCursor(m_formWindow, newlySelected, true);
    // Check consistency: Make sure either  managed widgets or  unmanaged objects are selected.
    // No newly-selected managed widgets: Unless there are ones in the (old) current selection,
    // select the unmanaged object
    if (selectManagedWidgetCount == 0) {
        if (checkManagedWidgetSelection(currentSelectedIndexes)) {
            // Managed selection exists, refuse and update if necessary
            if (deselectedManagedWidgetCount != 0 || selectManagedWidgetCount != 0)
                m_formWindow->emitSelectionChanged();
            return;
        }
        // And now for the unmanaged selection
        m_formWindow->clearSelection(false);
        QObject *unmanagedObject = newlySelected.front();
        m_core->propertyEditor()->setObject(unmanagedObject);
        m_core->propertyEditor()->setEnabled(true);
        // open container page if it is a single widget
        if (newlySelected.size() == 1 && unmanagedObject->isWidgetType())
            showContainersCurrentPage(static_cast<QWidget*>(unmanagedObject));
        return;
    }
    // Open container page if it is a single widget
    if (newlySelected.size() == 1) {
        QObject *object = newlySelected.back();
        if (object->isWidgetType())
            showContainersCurrentPage(static_cast<QWidget*>(object));
    }

    // A managed widget was newly selected. Make sure  there are no unmanaged objects
    // in the whole unless just single selection
    if (currentSelectedIndexes.size() > selectManagedWidgetCount)
        checkManagedWidgetSelection(currentSelectedIndexes);
    // Update form
    if (deselectedManagedWidgetCount != 0 || selectManagedWidgetCount != 0)
        m_formWindow->emitSelectionChanged();
}


void ObjectInspector::ObjectInspectorPrivate::getSelection(Selection &s) const
{
    s.clear();

    if (!m_formWindow)
        return;

    const QModelIndexList currentSelectedIndexes = m_treeView->selectionModel()->selectedRows(0);
    if (currentSelectedIndexes.empty())
        return;

    // sort objects
    foreach (const QModelIndex &index, currentSelectedIndexes)
        if (QObject *object = m_model->objectAt(index))
            switch (selectionType(m_formWindow, object)) {
            case NoSelection:
                break;
            case QObjectSelection:
            case UnmanagedWidgetSelection:
                // It is actually possible to select an action
                // twice if it is in a menu bar and in a tool bar.
                if (!s.m_selectedObjects.contains(object))
                    s.m_selectedObjects.push_back(object);
                break;
            case ManagedWidgetSelection:
                s.m_cursorSelection.push_back(qobject_cast<QWidget *>(object));
                break;
            }
}

void ObjectInspector::ObjectInspectorPrivate::slotPopupContextMenu(QWidget *parent, const QPoint &pos)
{
    if (m_formWindow == 0 || m_formWindow->currentTool() != 0)
        return;

    const QModelIndex index =  m_treeView->indexAt (pos);
    QObject *object = m_model->objectAt(m_treeView->indexAt(pos));

    if (!object)
        return;

    QMenu *menu = 0;
    if (object->isWidgetType()) {
        if (qdesigner_internal::FormWindowBase *fwb = qobject_cast<qdesigner_internal::FormWindowBase*>(m_formWindow))
            menu = fwb->initializePopupMenu(qobject_cast<QWidget *>(object));
    } else {
        // Pull extension for non-widget
        QDesignerTaskMenuExtension *taskMenu = qt_extension<QDesignerTaskMenuExtension*>(core()->extensionManager(), object);
        if (taskMenu) {
            QList<QAction*> actions = taskMenu->taskActions();
            if (!actions.isEmpty()) {
                menu = new QMenu(parent);
                menu->addActions(actions);
            }
        }
    }

    if (menu) {
        menu->exec(m_treeView->viewport()->mapToGlobal(pos));
        delete menu;
    }
}

// ------------ ObjectInspector
ObjectInspector::ObjectInspector(QDesignerFormEditorInterface *core, QWidget *parent) :
    QDesignerObjectInspector(parent),
    m_impl(new ObjectInspectorPrivate(core))
{
    QVBoxLayout *vbox = new QVBoxLayout(this);
    vbox->setMargin(0);

    QTreeView *treeView = m_impl->treeView();
    vbox->addWidget(treeView);

    connect(treeView, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(slotPopupContextMenu(QPoint)));

    connect(treeView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(slotSelectionChanged(QItemSelection,QItemSelection)));

    connect(treeView->header(), SIGNAL(sectionDoubleClicked(int)), this, SLOT(slotHeaderDoubleClicked(int)));
    setAcceptDrops(true);
}

ObjectInspector::~ObjectInspector()
{
    delete m_impl;
}

QDesignerFormEditorInterface *ObjectInspector::core() const
{
    return m_impl->core();
}

void ObjectInspector::slotPopupContextMenu(const QPoint &pos)
{
    m_impl->slotPopupContextMenu(this, pos);
}

void ObjectInspector::setFormWindow(QDesignerFormWindowInterface *fwi)
{
    m_impl->setFormWindow(fwi);
}

void ObjectInspector::slotSelectionChanged(const QItemSelection & selected, const QItemSelection &deselected)
{
    m_impl->slotSelectionChanged(selected, deselected);
}

void ObjectInspector::getSelection(Selection &s) const
{
    m_impl->getSelection(s);
}

bool ObjectInspector::selectObject(QObject *o)
{
   return m_impl->selectObject(o);
}

void ObjectInspector::clearSelection()
{
    m_impl->clearSelection();
}

void ObjectInspector::slotHeaderDoubleClicked(int column)
{
    m_impl->slotHeaderDoubleClicked(column);
}

void ObjectInspector::mainContainerChanged()
{
    // Invalidate references to objects kept in items
    if (sender() == m_impl->formWindow())
        setFormWindow(0);
}

void  ObjectInspector::dragEnterEvent (QDragEnterEvent * event)
{
    m_impl->handleDragEnterMoveEvent(this, event, true);
}

void  ObjectInspector::dragMoveEvent(QDragMoveEvent * event)
{
    m_impl->handleDragEnterMoveEvent(this, event, false);
}

void  ObjectInspector::dragLeaveEvent(QDragLeaveEvent * /* event*/)
{
    m_impl->restoreDropHighlighting();
}

void  ObjectInspector::dropEvent (QDropEvent * event)
{
    m_impl->dropEvent(event);

QT_END_NAMESPACE
}
}
