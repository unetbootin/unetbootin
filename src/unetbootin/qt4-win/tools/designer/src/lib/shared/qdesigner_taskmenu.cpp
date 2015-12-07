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
TRANSLATOR qdesigner_internal::QDesignerTaskMenu
*/

#include "qdesigner_taskmenu_p.h"
#include "qdesigner_command_p.h"
#include "richtexteditor_p.h"
#include "plaintexteditor_p.h"
#include "stylesheeteditor_p.h"
#include "qlayout_widget_p.h"
#include "layout_p.h"
#include "spacer_widget_p.h"
#include "textpropertyeditor_p.h"
#include "promotiontaskmenu_p.h"
#include "metadatabase_p.h"
#include "scriptdialog_p.h"
#include "scriptcommand_p.h"
#include "signalslotdialog_p.h"
#include "qdesigner_membersheet_p.h"
#include "qdesigner_propertycommand_p.h"
#include <shared_enums_p.h>

#include <QtDesigner/QDesignerFormWindowInterface>
#include <QtDesigner/QDesignerFormWindowCursorInterface>
#include <QtDesigner/QDesignerPropertySheetExtension>
#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QDesignerLanguageExtension>
#include <QtDesigner/QExtensionManager>

#include <QtGui/QAction>
#include <QtGui/QActionGroup>
#include <QtGui/QWidget>
#include <QtGui/QMenuBar>
#include <QtGui/QMainWindow>
#include <QtGui/QStatusBar>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QVBoxLayout>
#include <QtGui/QPushButton>
#include <QtGui/QUndoStack>
#include <QtCore/QDebug>

QT_BEGIN_NAMESPACE

static QMenuBar *findMenuBar(const QWidget *widget)
{
    const QList<QObject*> children = widget->children();
    foreach (QObject *obj, widget->children()) {
        if (QMenuBar *mb = qobject_cast<QMenuBar*>(obj)) {
            return mb;
        }
    }

    return 0;
}

static QStatusBar *findStatusBar(const QWidget *widget)
{
    const QList<QObject*> children = widget->children();
    foreach (QObject *obj, widget->children()) {
        if (QStatusBar *sb = qobject_cast<QStatusBar*>(obj)) {
            return sb;
        }
    }

    return 0;
}

static inline QAction *createSeparatorHelper(QObject *parent) {
    QAction *rc = new QAction(parent);
    rc->setSeparator(true);
    return rc;
}

enum { ApplyMinimumWidth = 0x1, ApplyMinimumHeight = 0x2, ApplyMaximumWidth = 0x4, ApplyMaximumHeight = 0x8 };

namespace  {
// --------------- ObjectNameDialog
class ObjectNameDialog : public QDialog
{
     public:
         ObjectNameDialog(QWidget *parent, const QString &oldName);
         QString newObjectName() const;

     private:
         qdesigner_internal::TextPropertyEditor *m_editor;
};

ObjectNameDialog::ObjectNameDialog(QWidget *parent, const QString &oldName)
    : QDialog(parent),
      m_editor( new qdesigner_internal::TextPropertyEditor(this, qdesigner_internal::TextPropertyEditor::EmbeddingNone,
                                                           qdesigner_internal::ValidationObjectName))
{
    setWindowTitle(QObject::tr("Change Object Name"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    QVBoxLayout *vboxLayout = new QVBoxLayout(this);
    vboxLayout->addWidget(new QLabel(QObject::tr("Object Name")));

    m_editor->setText(oldName);
    m_editor->selectAll();
    m_editor->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    vboxLayout->addWidget(m_editor);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                                       Qt::Horizontal, this);
    QPushButton *okButton = buttonBox->button(QDialogButtonBox::Ok);
    okButton->setDefault(true);
    vboxLayout->addWidget(buttonBox);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

QString ObjectNameDialog::newObjectName() const
{
    return m_editor->text();
}

}

namespace qdesigner_internal {
// -------------- QDesignerTaskMenuPrivate
class QDesignerTaskMenuPrivate {
public:
    QDesignerTaskMenuPrivate(QWidget *widget, QObject *parent);
    QPointer<QWidget> m_widget;
    QAction *m_separator;
    QAction *m_separator2;
    QAction *m_separator3;
    QAction *m_separator4;
    QAction *m_separator5;
    QAction *m_separator6;
    QAction *m_changeObjectNameAction;
    QAction *m_changeToolTip;
    QAction *m_changeWhatsThis;
    QAction *m_changeStyleSheet;

    QAction *m_addMenuBar;
    QAction *m_addToolBar;
    QAction *m_addStatusBar;
    QAction *m_removeStatusBar;
    QAction *m_changeScript;
    QAction *m_containerFakeMethods;
    PromotionTaskMenu* m_promotionTaskMenu;
    QActionGroup *m_sizeActionGroup;
    QAction *m_sizeActionsSubMenu;
};

QDesignerTaskMenuPrivate::QDesignerTaskMenuPrivate(QWidget *widget, QObject *parent) :
    m_widget(widget),
    m_separator(createSeparatorHelper(parent)),
    m_separator2(createSeparatorHelper(parent)),
    m_separator3(createSeparatorHelper(parent)),
    m_separator4(createSeparatorHelper(parent)),
    m_separator5(createSeparatorHelper(parent)),
    m_separator6(createSeparatorHelper(parent)),
    m_changeObjectNameAction(new QAction(QDesignerTaskMenu::tr("Change objectName..."), parent)),
    m_changeToolTip(new QAction(QDesignerTaskMenu::tr("Change toolTip..."), parent)),
    m_changeWhatsThis(new QAction(QDesignerTaskMenu::tr("Change whatsThis..."), parent)),
    m_changeStyleSheet(new QAction(QDesignerTaskMenu::tr("Change styleSheet..."), parent)),
    m_addMenuBar(new QAction(QDesignerTaskMenu::tr("Create Menu Bar"), parent)),
    m_addToolBar(new QAction(QDesignerTaskMenu::tr("Add Tool Bar"), parent)),
    m_addStatusBar(new QAction(QDesignerTaskMenu::tr("Create Status Bar"), parent)),
    m_removeStatusBar(new QAction(QDesignerTaskMenu::tr("Remove Status Bar"), parent)),
    m_changeScript(new QAction(QDesignerTaskMenu::tr("Change script..."), parent)),
    m_containerFakeMethods(new QAction(QDesignerTaskMenu::tr("Change signals/slots..."), parent)),
    m_promotionTaskMenu(new PromotionTaskMenu(widget, PromotionTaskMenu::ModeMultiSelection, parent)),
    m_sizeActionGroup(new QActionGroup(parent)),
    m_sizeActionsSubMenu(new QAction(QDesignerTaskMenu::tr("Size Constraints"), parent))
{
    QMenu *sizeMenu = new QMenu;
    m_sizeActionsSubMenu->setMenu(sizeMenu);
    QAction *sizeAction = m_sizeActionGroup->addAction(QDesignerTaskMenu::tr("Set Minimum Width"));
    sizeAction->setData(ApplyMinimumWidth);
    sizeMenu->addAction(sizeAction);

    sizeAction = m_sizeActionGroup->addAction(QDesignerTaskMenu::tr("Set Minimum Height"));
    sizeAction->setData(ApplyMinimumHeight);
    sizeMenu->addAction(sizeAction);

    sizeAction = m_sizeActionGroup->addAction(QDesignerTaskMenu::tr("Set Minimum Size"));
    sizeAction->setData(ApplyMinimumWidth|ApplyMinimumHeight);
    sizeMenu->addAction(sizeAction);

    sizeMenu->addSeparator();

    sizeAction = m_sizeActionGroup->addAction(QDesignerTaskMenu::tr("Set Maximum Width"));
    sizeAction->setData(ApplyMaximumWidth);
    sizeMenu->addAction(sizeAction);

    sizeAction = m_sizeActionGroup->addAction(QDesignerTaskMenu::tr("Set Maximum Height"));
    sizeAction->setData(ApplyMaximumHeight);
    sizeMenu->addAction(sizeAction);

    sizeAction = m_sizeActionGroup->addAction(QDesignerTaskMenu::tr("Set Maximum Size"));
    sizeAction->setData(ApplyMaximumWidth|ApplyMaximumHeight);
    sizeMenu->addAction(sizeAction);
}
// --------- QDesignerTaskMenu
QDesignerTaskMenu::QDesignerTaskMenu(QWidget *widget, QObject *parent) :
    QObject(parent),
    d(new QDesignerTaskMenuPrivate(widget, parent))
{
    Q_ASSERT(qobject_cast<QDesignerFormWindowInterface*>(widget) == 0);

    connect(d->m_changeObjectNameAction, SIGNAL(triggered()), this, SLOT(changeObjectName()));
    connect(d->m_changeToolTip, SIGNAL(triggered()), this, SLOT(changeToolTip()));
    connect(d->m_changeWhatsThis, SIGNAL(triggered()), this, SLOT(changeWhatsThis()));
    connect(d->m_changeStyleSheet, SIGNAL(triggered()), this,  SLOT(changeStyleSheet()));
    connect(d->m_addMenuBar, SIGNAL(triggered()), this, SLOT(createMenuBar()));
    connect(d->m_addToolBar, SIGNAL(triggered()), this, SLOT(addToolBar()));
    connect(d->m_addStatusBar, SIGNAL(triggered()), this, SLOT(createStatusBar()));
    connect(d->m_removeStatusBar, SIGNAL(triggered()), this, SLOT(removeStatusBar()));
    connect(d->m_changeScript, SIGNAL(triggered()), this, SLOT(changeScript()));
    connect(d->m_containerFakeMethods, SIGNAL(triggered()), this, SLOT(containerFakeMethods()));
    connect(d->m_sizeActionGroup, SIGNAL(triggered(QAction*)), this,  SLOT(applySize(QAction*)));
}

QDesignerTaskMenu::~QDesignerTaskMenu()
{
    delete d;
}

QAction *QDesignerTaskMenu::createSeparator()
{
    return createSeparatorHelper(this);
}

QWidget *QDesignerTaskMenu::widget() const
{
    return d->m_widget;
}

QDesignerFormWindowInterface *QDesignerTaskMenu::formWindow() const
{
    QDesignerFormWindowInterface *result = QDesignerFormWindowInterface::findFormWindow(widget());
    Q_ASSERT(result != 0);
    return result;
}

void QDesignerTaskMenu::createMenuBar()
{
    if (QDesignerFormWindowInterface *fw = formWindow()) {
        QMainWindow *mw = qobject_cast<QMainWindow*>(fw->mainContainer());
        if (!mw) {
            // ### warning message
            return;
        }

        CreateMenuBarCommand *cmd = new CreateMenuBarCommand(fw);
        cmd->init(mw);
        fw->commandHistory()->push(cmd);
    }
}

void QDesignerTaskMenu::addToolBar()
{
    if (QDesignerFormWindowInterface *fw = formWindow()) {
        QMainWindow *mw = qobject_cast<QMainWindow*>(fw->mainContainer());
        if (!mw) {
            // ### warning message
            return;
        }

        AddToolBarCommand *cmd = new AddToolBarCommand(fw);
        cmd->init(mw);
        fw->commandHistory()->push(cmd);
    }
}

void QDesignerTaskMenu::createStatusBar()
{
    if (QDesignerFormWindowInterface *fw = formWindow()) {
        QMainWindow *mw = qobject_cast<QMainWindow*>(fw->mainContainer());
        if (!mw) {
            // ### warning message
            return;
        }

        CreateStatusBarCommand *cmd = new CreateStatusBarCommand(fw);
        cmd->init(mw);
        fw->commandHistory()->push(cmd);
    }
}

void QDesignerTaskMenu::removeStatusBar()
{
    if (QDesignerFormWindowInterface *fw = formWindow()) {
        QMainWindow *mw = qobject_cast<QMainWindow*>(fw->mainContainer());
        if (!mw) {
            // ### warning message
            return;
        }

        DeleteStatusBarCommand *cmd = new DeleteStatusBarCommand(fw);
        cmd->init(findStatusBar(mw));
        fw->commandHistory()->push(cmd);
    }
}

QList<QAction*> QDesignerTaskMenu::taskActions() const
{
    QDesignerFormWindowInterface *formWindow = QDesignerFormWindowInterface::findFormWindow(widget());
    Q_ASSERT(formWindow);

    const bool isMainContainer = formWindow->mainContainer() == widget();

    QList<QAction*> actions;

    if (const QMainWindow *mw = qobject_cast<const QMainWindow*>(formWindow->mainContainer()))  {
        if (isMainContainer || mw->centralWidget() == widget()) {
            if (!findMenuBar(mw)) {
                actions.append(d->m_addMenuBar);
            }

            actions.append(d->m_addToolBar);
            // ### create the status bar
            if (!findStatusBar(mw))
                actions.append(d->m_addStatusBar);
            else
                actions.append(d->m_removeStatusBar);
            actions.append(d->m_separator);
        }
    }
    actions.append(d->m_changeObjectNameAction);
    actions.append(d->m_separator2);
    actions.append(d->m_changeToolTip);
    actions.append(d->m_changeWhatsThis);
    actions.append(d->m_changeStyleSheet);
    actions.append(d->m_separator6);
    actions.append(d->m_sizeActionsSubMenu);
    d->m_promotionTaskMenu->addActions(formWindow, PromotionTaskMenu::LeadingSeparator, actions);

#ifdef WANT_SCRIPT_OPTION
    if (!isMainContainer) {
        actions.append(d->m_separator4);
        actions.append(d->m_changeScript);
    }
#endif
    if (isMainContainer && !qt_extension<QDesignerLanguageExtension*>(formWindow->core()->extensionManager(), formWindow->core())) {
        actions.append(d->m_separator5);
        actions.append(d->m_containerFakeMethods);
    }
    return actions;
}

void QDesignerTaskMenu::changeObjectName()
{
    QDesignerFormWindowInterface *fw = formWindow();
    Q_ASSERT(fw != 0);

    QDesignerFormEditorInterface *core = fw->core();
    QDesignerPropertySheetExtension *sheet = qt_extension<QDesignerPropertySheetExtension*>(core->extensionManager(), widget());
    Q_ASSERT(sheet != 0);

    const QString objectNameProperty = QLatin1String("objectName");
    const QString oldObjectName = sheet->property(sheet->indexOf(objectNameProperty)).toString();
    ObjectNameDialog dialog(fw, oldObjectName);
    if (dialog.exec() == QDialog::Accepted) {
        const QString newObjectName = dialog.newObjectName();
        if (!newObjectName.isEmpty() && newObjectName  != oldObjectName )
            fw->cursor()->setProperty(objectNameProperty, newObjectName);
    }
}

void QDesignerTaskMenu::changeTextProperty(const QString &propertyName, const QString &windowTitle, PropertyMode pm, Qt::TextFormat desiredFormat)
{
    QDesignerFormWindowInterface *fw = formWindow();
    if (!fw)
        return;
    Q_ASSERT(d->m_widget->parentWidget() != 0);

    const QDesignerPropertySheetExtension *sheet = qt_extension<QDesignerPropertySheetExtension*>(fw->core()->extensionManager(), d->m_widget);
    const int index = sheet->indexOf(propertyName);
    if (index == -1) {
        qDebug() << "** WARNING Invalid property" << propertyName << " passed to changeTextProperty!";
        return;
    }
    const QString oldText = sheet->property(index).toString();
    // Pop up respective dialog
    bool accepted = false;
    QString newText;
    switch (desiredFormat) {
    case Qt::PlainText: {
        PlainTextEditorDialog dlg(fw);
        if (!windowTitle.isEmpty())
            dlg.setWindowTitle(windowTitle);
        dlg.setDefaultFont(d->m_widget->font());
        dlg.setText(oldText);
        accepted = dlg.showDialog() == QDialog::Accepted;
        newText = dlg.text();
    }
        break;
    default: {
        RichTextEditorDialog dlg(fw->core(), fw);
        if (!windowTitle.isEmpty())
            dlg.setWindowTitle(windowTitle);
        dlg.setDefaultFont(d->m_widget->font());
        dlg.setText(oldText);
        accepted = dlg.showDialog() == QDialog::Accepted;
        newText = dlg.text(desiredFormat);
    }
        break;
    }
    // change property
    if (!accepted || oldText == newText)
          return;

    switch (pm) {
    case CurrentWidgetMode:
        fw->cursor()->setWidgetProperty(d->m_widget, propertyName, QVariant(newText));
        break;
    case MultiSelectionMode:
        fw->cursor()->setProperty(propertyName, QVariant(newText));
        break;
    }
}

void QDesignerTaskMenu::changeToolTip()
{
    changeTextProperty(QLatin1String("toolTip"), tr("Edit ToolTip"), MultiSelectionMode, Qt::AutoText);
}

void QDesignerTaskMenu::changeWhatsThis()
{
    changeTextProperty(QLatin1String("whatsThis"), tr("Edit WhatsThis"), MultiSelectionMode, Qt::AutoText);
}

void QDesignerTaskMenu::changeStyleSheet()
{
    if (QDesignerFormWindowInterface *fw = formWindow()) {
        StyleSheetPropertyEditorDialog dlg(fw, fw, d->m_widget);
        dlg.exec();
    }
}

void QDesignerTaskMenu::changeScript()
{
    QDesignerFormWindowInterface *fw = formWindow();
    if (!fw)
        return;

    MetaDataBase *metaDataBase = qobject_cast<MetaDataBase*>(fw->core()->metaDataBase());
    if (!metaDataBase)
        return;

    const MetaDataBaseItem* item = metaDataBase->metaDataBaseItem(d->m_widget);
    if (!item)
        return;

    const QString oldScript = item->script();
    QString newScript = oldScript;

    ScriptDialog scriptDialog(fw->core()->dialogGui(), fw);
    if (!scriptDialog.editScript(newScript))
        return;

    // compile list of selected objects
    ScriptCommand::ObjectList objects;
    objects += (QWidget *)d->m_widget;

    const QDesignerFormWindowCursorInterface *cursor = fw->cursor();
    const int selectionCount =  cursor->selectedWidgetCount();
    for (int i = 0; i < selectionCount; i++) {
        QWidget *w = cursor->selectedWidget(i);
        if (w != d->m_widget)
             objects += w;
    }
    ScriptCommand *scriptCommand = new ScriptCommand(fw);
    if (!scriptCommand->init(objects, newScript)) {
        delete scriptCommand;
        return;
    }

    fw->commandHistory()->push(scriptCommand);
}

void QDesignerTaskMenu::containerFakeMethods()
{
    QDesignerFormWindowInterface *fw = formWindow();
    if (!fw)
        return;
    SignalSlotDialog::editMetaDataBase(fw, d->m_widget, fw);
}

// Add a command that takes over the value of the current geometry as
// minimum/maximum size according to the flags.
static void createSizeCommand(QDesignerFormWindowInterface *fw, QWidget *w, int flags)
{
    const QSize size = w->size();
    if (flags & (ApplyMinimumWidth|ApplyMinimumHeight)) {
        QSize minimumSize = w-> minimumSize();
        if (flags & ApplyMinimumWidth)
            minimumSize.setWidth(size.width());
        if (flags & ApplyMinimumHeight)
             minimumSize.setHeight(size.height());
        SetPropertyCommand* cmd = new SetPropertyCommand(fw);
        cmd->init(w, QLatin1String("minimumSize"), minimumSize);
        fw->commandHistory()->push(cmd);
    }
    if (flags & (ApplyMaximumWidth|ApplyMaximumHeight)) {
        QSize maximumSize = w-> maximumSize();
        if (flags & ApplyMaximumWidth)
            maximumSize.setWidth(size.width());
        if (flags & ApplyMaximumHeight)
             maximumSize.setHeight(size.height());
        SetPropertyCommand* cmd = new SetPropertyCommand(fw);
        cmd->init(w, QLatin1String("maximumSize"), maximumSize);
        fw->commandHistory()->push(cmd);
    }
}

void QDesignerTaskMenu::applySize(QAction *a)
{
    QDesignerFormWindowInterface *fw = formWindow();
    if (!fw)
        return;
    const QDesignerFormWindowCursorInterface *cursor = fw->cursor();
    const int selectionCount =  cursor->selectedWidgetCount();
    if (selectionCount == 0)
        return;

    const int mask = a->data().toInt();
    fw->commandHistory()->beginMacro(tr("Set size constraint on %n widget(s)", 0, selectionCount));
    for (int i = 0; i < selectionCount; i++)
        createSizeCommand(fw, cursor->selectedWidget(i), mask);
    fw->commandHistory()->endMacro();
}

// ---------------- QDesignerTaskMenuFactory
QDesignerTaskMenuFactory::QDesignerTaskMenuFactory(QExtensionManager *extensionManager)
    : QExtensionFactory(extensionManager)
{
}

QObject *QDesignerTaskMenuFactory::createExtension(QObject *object, const QString &iid, QObject *parent) const
{
    if (iid != QLatin1String("QDesignerInternalTaskMenuExtension"))
        return 0;

    QWidget *widget = qobject_cast<QWidget*>(object);
    if (widget == 0)
        return 0;

    // check if is an internal widget (### generalize)
    if (qobject_cast<const QLayoutWidget*>(widget) || qobject_cast<const Spacer*>(widget))
        return 0;

    return new QDesignerTaskMenu(widget, parent);
}

} // namespace qdesigner_internal

QT_END_NAMESPACE
