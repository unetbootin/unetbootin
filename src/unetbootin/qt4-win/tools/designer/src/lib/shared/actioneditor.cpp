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
TRANSLATOR qdesigner_internal::ActionEditor
*/

#include "actioneditor_p.h"
#include "actionrepository_p.h"
#include "iconloader_p.h"
#include "newactiondialog_p.h"
#include "qdesigner_menu_p.h"
#include "qdesigner_command_p.h"
#include "qdesigner_propertycommand_p.h"
#include "qdesigner_objectinspector_p.h"
#include "qdesigner_utils_p.h"
#include "qsimpleresource_p.h"
#include "formwindowbase_p.h"

#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QDesignerPropertyEditorInterface>
#include <QtDesigner/QDesignerPropertySheetExtension>
#include <QtDesigner/QExtensionManager>
#include <QtDesigner/QDesignerMetaDataBaseInterface>
#include <QtDesigner/QDesignerIconCacheInterface>

#include <QtGui/QMenu>
#include <QtGui/QToolBar>
#include <QtGui/QSplitter>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QClipboard>
#include <QtGui/QItemDelegate>
#include <QtGui/QPainter>
#include <QtGui/QVBoxLayout>
#include <QtGui/QLineEdit>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QToolButton>
#include <QtGui/QContextMenuEvent>
#include <QtGui/QItemSelection>

#include <QtCore/QRegExp>
#include <QtCore/QDebug>
#include <QtCore/QSignalMapper>
#include <QtCore/QBuffer>

Q_DECLARE_METATYPE(QAction*)

QT_BEGIN_NAMESPACE

namespace qdesigner_internal {

//-------- ActionFilterWidget
class ActionFilterWidget: public QWidget
{
    Q_OBJECT
public:
    ActionFilterWidget(ActionEditor *actionEditor, QToolBar *parent)
        : QWidget(parent),
          m_button(new QPushButton),
          m_editor(new QLineEdit),
          m_actionEditor(actionEditor)
    {
        QHBoxLayout *l = new QHBoxLayout(this);
        l->setMargin(0);
        l->setSpacing(0);

        l->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));

        QLabel *label = new QLabel(tr("Filter: "));
        label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        l->addWidget(label);

        l->addWidget(m_editor);

        connect(m_editor, SIGNAL(textChanged(QString)), actionEditor, SLOT(setFilter(QString)));

        m_button->setIcon(createIconSet(QLatin1String("resetproperty.png")));
        m_button->setIconSize(QSize(8, 8));
        m_button->setFlat(true);
        l->addWidget(m_button);

        connect(m_button, SIGNAL(clicked()), m_editor, SLOT(clear()));
        connect(m_editor, SIGNAL(textChanged(QString)), this, SLOT(checkButton(QString)));
    }

private slots:
    void checkButton(const QString &text)
    {
        m_button->setEnabled(!text.isEmpty());
    }

private:
    QPushButton *m_button;
    QLineEdit *m_editor;
    ActionEditor *m_actionEditor;
};

//--------  ActionGroupDelegate
class ActionGroupDelegate: public QItemDelegate
{
public:
    ActionGroupDelegate(QObject *parent)
        : QItemDelegate(parent) {}

    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        if (option.state & QStyle::State_Selected)
            painter->fillRect(option.rect, option.palette.highlight());

        QItemDelegate::paint(painter, option, index);
    }

    virtual void drawFocus(QPainter * /*painter*/, const QStyleOptionViewItem &/*option*/, const QRect &/*rect*/) const {}
};

//--------  ActionEditor
ActionEditor::ActionEditor(QDesignerFormEditorInterface *core, QWidget *parent, Qt::WindowFlags flags) :
    QDesignerActionEditorInterface(parent, flags),
    m_core(core),
    m_actionGroups(0),
    m_actionView(new ActionView),
    m_actionNew(new QAction(tr("New..."), this)),
    m_actionEdit(new QAction(tr("Edit..."), this)),
    m_actionCopy(new QAction(tr("Copy"), this)),
    m_actionCut(new QAction(tr("Cut"), this)),
    m_actionPaste(new QAction(tr("Paste"), this)),
    m_actionSelectAll(new QAction(tr("Select all"), this)),
    m_actionDelete(new QAction(tr("Delete"), this)),
    m_viewModeGroup(new  QActionGroup(this)),
    m_iconViewAction(0),
    m_listViewAction(0),
    m_filterWidget(0),
    m_selectAssociatedWidgetsMapper(0)
{
    m_actionView->initialize(m_core);
    m_actionView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    setWindowTitle(tr("Actions"));

    QVBoxLayout *l = new QVBoxLayout(this);
    l->setMargin(0);
    l->setSpacing(0);

    QToolBar *toolbar = new QToolBar;
    toolbar->setIconSize(QSize(22, 22));
    toolbar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    l->addWidget(toolbar);
    // edit actions
    m_actionNew->setIcon(createIconSet(QLatin1String("filenew.png")));
    m_actionNew->setEnabled(false);
    connect(m_actionNew, SIGNAL(triggered()), this, SLOT(slotNewAction()));
    toolbar->addAction(m_actionNew);

    connect(m_actionSelectAll, SIGNAL(triggered()), m_actionView, SLOT(selectAll()));

    m_actionCut->setEnabled(false);
    connect(m_actionCut, SIGNAL(triggered()), this, SLOT(slotCut()));
    m_actionCut->setIcon(createIconSet(QLatin1String("editcut.png")));

    m_actionCopy->setEnabled(false);
    connect(m_actionCopy, SIGNAL(triggered()), this, SLOT(slotCopy()));
    m_actionCopy->setIcon(createIconSet(QLatin1String("editcopy.png")));
    toolbar->addAction(m_actionCopy);

    connect(m_actionPaste, SIGNAL(triggered()), this, SLOT(slotPaste()));
    m_actionPaste->setIcon(createIconSet(QLatin1String("editpaste.png")));
    toolbar->addAction(m_actionPaste);

    m_actionEdit->setEnabled(false);
    connect(m_actionEdit, SIGNAL(triggered()), this, SLOT(editCurrentAction()));

    m_actionDelete->setIcon(createIconSet(QLatin1String("editdelete.png")));
    m_actionDelete->setEnabled(false);
    connect(m_actionDelete, SIGNAL(triggered()), this, SLOT(slotDelete()));
    toolbar->addAction(m_actionDelete);

    // Toolbutton with menu containing action group for detailed/icon view. Steal the icons from the file dialog.
    QToolButton *configureButton = new QToolButton;
    QAction *configureAction = new QAction(tr("Configure Action Editor"), this);
    configureAction->setIcon(createIconSet(QLatin1String("configure.png")));
    QMenu *configureMenu = new QMenu(this);
    configureAction->setMenu(configureMenu);
    configureButton->setDefaultAction(configureAction);
    configureButton->setPopupMode(QToolButton::InstantPopup);
    toolbar->addWidget(configureButton);

    m_viewModeGroup->setExclusive(true);
    connect(m_viewModeGroup, SIGNAL(triggered(QAction*)), this, SLOT(slotViewMode(QAction*)));

    m_iconViewAction = m_viewModeGroup->addAction(tr("Icon View"));
    m_iconViewAction->setData(QVariant(ActionView::IconView));
    m_iconViewAction->setCheckable(true);
    m_iconViewAction->setIcon(style()->standardIcon (QStyle::SP_FileDialogListView));
    configureMenu->addAction(m_iconViewAction);

    m_listViewAction = m_viewModeGroup->addAction(tr("Detailed View"));
    m_listViewAction->setData(QVariant(ActionView::DetailedView));
    m_listViewAction->setCheckable(true);
    m_listViewAction->setIcon(style()->standardIcon (QStyle::SP_FileDialogDetailedView));
    configureMenu->addAction(m_listViewAction);
    // filter
    m_filterWidget = new ActionFilterWidget(this, toolbar);
    m_filterWidget->setEnabled(false);
    toolbar->addWidget(m_filterWidget);

    // main layout
    QSplitter *splitter = new QSplitter(Qt::Horizontal);
    splitter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    splitter->addWidget(m_actionView);
    l->addWidget(splitter);

#if 0 // ### implement me
    m_actionGroups = new QListWidget(splitter);
    splitter->addWidget(m_actionGroups);
    m_actionGroups->setItemDelegate(new ActionGroupDelegate(m_actionGroups));
    m_actionGroups->setMovement(QListWidget::Static);
    m_actionGroups->setResizeMode(QListWidget::Fixed);
    m_actionGroups->setIconSize(QSize(48, 48));
    m_actionGroups->setFlow(QListWidget::TopToBottom);
    m_actionGroups->setViewMode(QListWidget::IconMode);
    m_actionGroups->setWrapping(false);
#endif

    connect(m_actionView, SIGNAL(resourceImageDropped(QString,QAction*)),
            this, SLOT(resourceImageDropped(QString,QAction*)));

    connect(m_actionView, SIGNAL(currentChanged(QAction*)),this, SLOT(slotCurrentItemChanged(QAction*)));
    // make it possible for vs integration to reimplement edit action dialog
    connect(m_actionView, SIGNAL(activated(QAction*)), this, SIGNAL(itemActivated(QAction*)));

    connect(m_actionView,SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(slotSelectionChanged(QItemSelection,QItemSelection)));

    connect(m_actionView, SIGNAL(contextMenuRequested(QContextMenuEvent*, QAction*)),
            this, SLOT(slotContextMenuRequested(QContextMenuEvent*, QAction*)));

    connect(this, SIGNAL(itemActivated(QAction*)), this, SLOT(editAction(QAction*)));

    updateViewModeActions();
}

ActionEditor::~ActionEditor()
{
}

QAction *ActionEditor::actionNew() const
{
    return m_actionNew;
}

QAction *ActionEditor::actionDelete() const
{
    return m_actionDelete;
}

QDesignerFormWindowInterface *ActionEditor::formWindow() const
{
    return m_formWindow;
}

void ActionEditor::setFormWindow(QDesignerFormWindowInterface *formWindow)
{
    if (formWindow != 0 && formWindow->mainContainer() == 0)
        formWindow = 0;

    // we do NOT rely on this function to update the action editor
    if (m_formWindow == formWindow)
        return;

    if (m_formWindow != 0) {
        const ActionList actionList = qFindChildren<QAction*>(m_formWindow->mainContainer());
        foreach (QAction *action, actionList)
            disconnect(action, SIGNAL(changed()), this, SLOT(slotActionChanged()));
    }

    m_formWindow = formWindow;

    m_actionView->model()->clearActions();

    m_actionEdit->setEnabled(false);
    m_actionCopy->setEnabled(false);
    m_actionCut->setEnabled(false);
    m_actionDelete->setEnabled(false);

    if (!formWindow || !formWindow->mainContainer()) {
        m_actionNew->setEnabled(false);
        m_filterWidget->setEnabled(false);
        return;
    }

    m_actionNew->setEnabled(true);
    m_filterWidget->setEnabled(true);

    const ActionList actionList = qFindChildren<QAction*>(formWindow->mainContainer());
    foreach (QAction *action, actionList)
        if (!action->isSeparator() && core()->metaDataBase()->item(action) != 0) {
            // Show unless it has a menu. However, listen for change on menu actions also as it might be removed
            if (!action->menu())
                m_actionView->model()->addAction(action);
            connect(action, SIGNAL(changed()), this, SLOT(slotActionChanged()));
        }

    setFilter(m_filter);
}

void  ActionEditor::slotSelectionChanged(const QItemSelection& selected, const QItemSelection& /*deselected*/)
{
    const bool hasSelection = !selected.indexes().empty();
    m_actionCopy->setEnabled(hasSelection);
    m_actionCut->setEnabled(hasSelection);
    m_actionDelete->setEnabled(hasSelection);
}

void ActionEditor::slotCurrentItemChanged(QAction *action)
{
    QDesignerFormWindowInterface *fw = formWindow();
    if (!fw)
        return;

    const bool hasCurrentAction = action != 0;
    m_actionEdit->setEnabled(hasCurrentAction);

    if (!action) {
        fw->clearSelection();
        return;
    }

    QDesignerObjectInspector *oi = qobject_cast<QDesignerObjectInspector *>(core()->objectInspector());

    if (action->associatedWidgets().empty()) {
        // Special case: action not in object tree. Deselect all and set in property editor
        fw->clearSelection(false);
        if (oi)
            oi->clearSelection();
        core()->propertyEditor()->setObject(action);
    } else {
        if (oi)
            oi->selectObject(action);
    }
}

void ActionEditor::slotActionChanged()
{
    QAction *action = qobject_cast<QAction*>(sender());
    Q_ASSERT(action != 0);

    ActionModel *model = m_actionView->model();
    const int row = model->findAction(action);
    if (row == -1) {
        if (action->menu() == 0) // action got its menu deleted, create item
            model->addAction(action);
    } else if (action->menu() != 0) { // action got its menu created, remove item
        model->removeRow(row);
    } else {
        // action text or icon changed, update item
        model->update(row);
    }
}

QDesignerFormEditorInterface *ActionEditor::core() const
{
    return m_core;
}

QString ActionEditor::filter() const
{
    return m_filter;
}

void ActionEditor::setFilter(const QString &f)
{
    m_filter = f;
    m_actionView->filter(m_filter);
}

// Set changed state of icon property,  reset when icon is cleared
static void refreshIconPropertyChanged(const QAction *action, QDesignerPropertySheetExtension *sheet)
{
    sheet->setChanged(sheet->indexOf(QLatin1String("icon")), !action->icon().isNull());
}

void ActionEditor::manageAction(QAction *action)
{
    action->setParent(formWindow()->mainContainer());
    core()->metaDataBase()->add(action);

    if (action->isSeparator() || action->menu() != 0)
        return;

    QDesignerPropertySheetExtension *sheet = qt_extension<QDesignerPropertySheetExtension*>(core()->extensionManager(), action);
    sheet->setChanged(sheet->indexOf(QLatin1String("objectName")), true);
    sheet->setChanged(sheet->indexOf(QLatin1String("text")), true);
    refreshIconPropertyChanged(action, sheet);

    m_actionView->setCurrentIndex(m_actionView->model()->addAction(action));
    connect(action, SIGNAL(changed()), this, SLOT(slotActionChanged()));
}

void ActionEditor::unmanageAction(QAction *action)
{
    core()->metaDataBase()->remove(action);
    action->setParent(0);

    disconnect(action, SIGNAL(changed()), this, SLOT(slotActionChanged()));

    const int row = m_actionView->model()->findAction(action);
    if (row != -1)
        m_actionView->model()->remove(row);
}

void ActionEditor::slotNewAction()
{
    NewActionDialog dlg(this);
    dlg.setWindowTitle(tr("New action"));

    if (dlg.exec() == QDialog::Accepted) {
        m_actionView->clearSelection();
        QAction *action = new QAction(formWindow());
        action->setObjectName(dlg.actionName());
        formWindow()->ensureUniqueObjectName(action);
        action->setText(dlg.actionText());
        QDesignerPropertySheetExtension *sheet = qt_extension<QDesignerPropertySheetExtension*>(core()->extensionManager(), action);
        sheet->setProperty(sheet->indexOf("icon"), qVariantFromValue(dlg.actionIcon()));

        AddActionCommand *cmd = new AddActionCommand(formWindow());
        cmd->init(action);
        formWindow()->commandHistory()->push(cmd);
    }
}

static inline bool isSameIcon(const QIcon &i1, const QIcon &i2)
{
    return i1.serialNumber() == i2.serialNumber();
}

// return a FormWindow command to apply an icon
static QDesignerFormWindowCommand *setIconPropertyCommand(const PropertySheetIconValue &newIcon, QAction *action, QDesignerFormWindowInterface *fw)
{
    const QString iconProperty = QLatin1String("icon");
    if (newIcon.paths().isEmpty()) {
        ResetPropertyCommand *cmd = new ResetPropertyCommand(fw);
        cmd->init(action, iconProperty);
        return cmd;
    }
    SetPropertyCommand *cmd = new SetPropertyCommand(fw);
    cmd->init(action, iconProperty, qVariantFromValue(newIcon));
    return cmd;
}

void ActionEditor::editAction(QAction *action)
{
    if (!action)
        return;

    NewActionDialog dlg(this);
    dlg.setWindowTitle(tr("Edit action"));
    QDesignerPropertySheetExtension *sheet = qt_extension<QDesignerPropertySheetExtension*>(core()->extensionManager(), action);
    const PropertySheetIconValue icon = qVariantValue<PropertySheetIconValue>(sheet->property(sheet->indexOf("icon")));
    dlg.setActionData(action->text(), action->objectName(), icon);

    if (!dlg.exec())
        return;

    // figure out changes and whether to start a macro
    enum ChangedMask { NameChanged = 1, TextChanged = 2 , IconChanged = 4 };
    const QString newName = dlg.actionName();
    const QString newText = dlg.actionText();
    const PropertySheetIconValue newIcon = dlg.actionIcon();

    int changedMask = 0;

    if (newName != action->objectName())
        changedMask |= NameChanged;
    if (newText != action->text())
        changedMask |= TextChanged;
    if (newIcon.paths() != icon.paths())
        changedMask |= IconChanged;

    if (!changedMask)
        return;

    const bool severalChanges = (changedMask != NameChanged) && (changedMask != TextChanged) && (changedMask != IconChanged);
    if (severalChanges)
        formWindow()->beginCommand(QLatin1String("Edit action"));

    if (changedMask & NameChanged)
        formWindow()->commandHistory()->push(createTextPropertyCommand(QLatin1String("objectName"), newName, action, formWindow()));

    if (changedMask & TextChanged)
        formWindow()->commandHistory()->push(createTextPropertyCommand(QLatin1String("text"), newText, action, formWindow()));

    if (changedMask & IconChanged)
        formWindow()->commandHistory()->push(setIconPropertyCommand(newIcon, action, formWindow()));

    if (severalChanges)
        formWindow()->endCommand();

}

void ActionEditor::editCurrentAction()
{
    if (QAction *a = m_actionView->currentAction())
        editAction(a);
}

void ActionEditor::deleteActions(QDesignerFormWindowInterface *fw, const ActionList &actions)
{
    const bool hasMulti = actions.size() > 1;
    if (hasMulti)
        fw->beginCommand(tr("Remove actions"));

    foreach(QAction *action, actions) {
        RemoveActionCommand *cmd = new RemoveActionCommand(fw);
        cmd->init(action);
        fw->commandHistory()->push(cmd);
    }
    if (hasMulti)
        fw->endCommand();
}

void ActionEditor::copyActions(QDesignerFormWindowInterface *fwi, const ActionList &actions)
{
    FormWindowBase *fw = qobject_cast<FormWindowBase *>(fwi);
    if (!fw )
        return;

    FormBuilderClipboard clipboard;
    clipboard.m_actions = actions;

    if (clipboard.empty())
        return;

    QEditorFormBuilder *formBuilder = fw->createFormBuilder();
    Q_ASSERT(formBuilder);

    QBuffer buffer;
    if (buffer.open(QIODevice::WriteOnly))
        if (formBuilder->copy(&buffer, clipboard))
            qApp->clipboard()->setText(QString::fromUtf8(buffer.buffer()), QClipboard::Clipboard);
    delete formBuilder;
}

void ActionEditor::slotDelete()
{
    QDesignerFormWindowInterface *fw =  formWindow();
    if (!fw)
        return;

    const ActionView::ActionList selection = m_actionView->selectedActions();
    if (selection.empty())
        return;

    deleteActions(fw,  selection);
}

QString ActionEditor::actionTextToName(const QString &text, const QString &prefix)
{
    QString name = text;
    if (name.isEmpty())
        return QString();

    name[0] = name.at(0).toUpper();
    name.prepend(prefix);
    const QString underscore = QString(QLatin1Char('_'));
    name.replace(QRegExp(QString(QLatin1String("[^a-zA-Z_0-9]"))), underscore);
    name.replace(QRegExp(QLatin1String("__*")), underscore);
    if (name.endsWith(underscore.at(0)))
        name.truncate(name.size() - 1);

    return name;
}

void  ActionEditor::resourceImageDropped(const QString &path, QAction *action)
{
    QDesignerFormWindowInterface *fw =  formWindow();
    if (!fw)
        return;

    QDesignerPropertySheetExtension *sheet = qt_extension<QDesignerPropertySheetExtension*>(core()->extensionManager(), action);
    const PropertySheetIconValue oldIcon = qVariantValue<PropertySheetIconValue>(sheet->property(sheet->indexOf("icon")));
    PropertySheetIconValue newIcon;
    newIcon.setPixmap(QIcon::Normal, QIcon::Off, PropertySheetPixmapValue(path));
    if (newIcon.paths().isEmpty() || newIcon.paths() == oldIcon.paths())
        return;

    fw->commandHistory()->push(setIconPropertyCommand(newIcon , action, fw));
}

void ActionEditor::mainContainerChanged()
{
    // Invalidate references to objects kept in model
    if (sender() == formWindow())
        setFormWindow(0);
}

int ActionEditor::viewMode() const
{
    return m_actionView->viewMode();
}

void ActionEditor::setViewMode(int lm)
{
    m_actionView->setViewMode(lm);
    updateViewModeActions();
}

void ActionEditor::slotViewMode(QAction *a)
{
    setViewMode(a->data().toInt());
}

void ActionEditor::slotSelectAssociatedWidget(QWidget *w)
{
    QDesignerFormWindowInterface *fw = formWindow();
    if (!fw )
        return;

    QDesignerObjectInspector *oi = qobject_cast<QDesignerObjectInspector *>(core()->objectInspector());
    if (!oi)
        return;

    fw->clearSelection(); // Actually, there are no widgets selected due to focus in event handling. Just to be sure.
    oi->selectObject(w);
}

void ActionEditor::updateViewModeActions()
{
    switch (viewMode()) {
    case ActionView::IconView:
        m_iconViewAction->setChecked(true);
        break;
    case ActionView::DetailedView:
        m_listViewAction->setChecked(true);
        break;
    }
}

void ActionEditor::slotCopy()
{
    QDesignerFormWindowInterface *fw = formWindow();
    if (!fw )
        return;

    const ActionView::ActionList selection = m_actionView->selectedActions();
    if (selection.empty())
        return;

    copyActions(fw, selection);
}

void ActionEditor::slotCut()
{
    QDesignerFormWindowInterface *fw = formWindow();
    if (!fw )
        return;

    const ActionView::ActionList selection = m_actionView->selectedActions();
    if (selection.empty())
        return;

    copyActions(fw, selection);
    deleteActions(fw, selection);
}

void ActionEditor::slotPaste()
{
    FormWindowBase *fw = qobject_cast<FormWindowBase *>(formWindow());
    if (!fw)
        return;
    m_actionView->clearSelection();
    fw->paste(FormWindowBase::PasteActionsOnly);
}

void ActionEditor::slotContextMenuRequested(QContextMenuEvent *e, QAction *item)
{
    // set up signal mapper
    if (!m_selectAssociatedWidgetsMapper) {
        m_selectAssociatedWidgetsMapper = new QSignalMapper(this);
        connect(m_selectAssociatedWidgetsMapper, SIGNAL(mapped(QWidget*)), this, SLOT(slotSelectAssociatedWidget(QWidget*)));
    }

    QMenu menu(this);
    menu.addAction(m_actionNew);
    menu.addSeparator();
    menu.addAction(m_actionEdit);
    // Associated Widgets
    if (QAction *action = m_actionView->currentAction()) {
        const QWidgetList associatedWidgets = ActionModel::associatedWidgets(action);
        if (!associatedWidgets.empty()) {
            QMenu *associatedWidgetsSubMenu =  menu.addMenu(tr("Used In"));
            foreach (QWidget *w, associatedWidgets) {
                QAction *action = associatedWidgetsSubMenu->addAction(w->objectName());
                m_selectAssociatedWidgetsMapper->setMapping(action, w);
                connect(action, SIGNAL(triggered()), m_selectAssociatedWidgetsMapper, SLOT(map()));
            }
        }
    }
    menu.addSeparator();
    menu.addAction(m_actionCut);
    menu.addAction(m_actionCopy);
    menu.addAction(m_actionPaste);
    menu.addAction(m_actionSelectAll);
    menu.addAction(m_actionDelete);
    menu.addSeparator();
    menu.addAction(m_iconViewAction);
    menu.addAction(m_listViewAction);

    emit contextMenuRequested(&menu, item);

    menu.exec(e->globalPos());
    e->accept();
}

} // namespace qdesigner_internal

QT_END_NAMESPACE

#include "actioneditor.moc"
