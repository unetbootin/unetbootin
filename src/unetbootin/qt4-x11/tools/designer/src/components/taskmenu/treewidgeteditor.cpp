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
TRANSLATOR qdesigner_internal::TreeWidgetEditor
*/

#include "treewidgeteditor.h"
#include <formwindowbase_p.h>
#include <iconloader_p.h>
#include <qdesigner_command_p.h>
#include <qdesigner_utils_p.h>
#include <abstractformbuilder.h>
#include <designerpropertymanager.h>
#include <qttreepropertybrowser.h>

#include <QtDesigner/QDesignerFormWindowInterface>
#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QDesignerIconCacheInterface>
#include <QtCore/QDir>
#include <QtCore/QQueue>
#include <QtGui/QHeaderView>
#include <QtGui/QTreeWidgetItemIterator>

QT_BEGIN_NAMESPACE

namespace qdesigner_internal {

TreeWidgetEditor::TreeWidgetEditor(QDesignerFormWindowInterface *form, QWidget *parent)
    : AbstractItemEditor(form, parent), m_updatingBrowser(false)
{
    m_columnEditor = new ItemListEditor(form, this);
    m_columnEditor->setObjectName(QLatin1String("columnEditor"));
    m_columnEditor->setNewItemText(tr("New Column"));
    ui.setupUi(this);

    injectPropertyBrowser(ui.itemsTab, ui.widget);
    connect(ui.showPropertiesButton, SIGNAL(clicked()),
            this, SLOT(togglePropertyBrowser()));
    togglePropertyBrowser();

    ui.tabWidget->insertTab(0, m_columnEditor, tr("&Columns"));
    ui.tabWidget->setCurrentIndex(0);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    ui.newItemButton->setIcon(createIconSet(QString::fromUtf8("plus.png")));
    ui.newSubItemButton->setIcon(createIconSet(QString::fromUtf8("downplus.png")));
    ui.deleteItemButton->setIcon(createIconSet(QString::fromUtf8("minus.png")));
    ui.moveItemUpButton->setIcon(createIconSet(QString::fromUtf8("up.png")));
    ui.moveItemDownButton->setIcon(createIconSet(QString::fromUtf8("down.png")));
    ui.moveItemRightButton->setIcon(createIconSet(QString::fromUtf8("leveldown.png")));
    ui.moveItemLeftButton->setIcon(createIconSet(QString::fromUtf8("levelup.png")));

    ui.treeWidget->header()->setMovable(false);

    connect(iconCache(), SIGNAL(reloaded()), this, SLOT(cacheReloaded()));
}

static AbstractItemEditor::PropertyDefinition treeHeaderPropList[] = {
    { Qt::DisplayPropertyRole, 0, DesignerPropertyManager::designerStringTypeId, "text" },
    { Qt::DecorationPropertyRole, 0, DesignerPropertyManager::designerIconTypeId, "icon" },
    { Qt::ToolTipPropertyRole, 0, DesignerPropertyManager::designerStringTypeId, "toolTip" },
    { Qt::StatusTipPropertyRole, 0, DesignerPropertyManager::designerStringTypeId, "statusTip" },
    { Qt::WhatsThisPropertyRole, 0, DesignerPropertyManager::designerStringTypeId, "whatsThis" },
    { Qt::FontRole, QVariant::Font, 0, "font" },
    { Qt::TextAlignmentRole, 0, DesignerPropertyManager::designerAlignmentTypeId, "textAlignment" },
    { Qt::BackgroundRole, QVariant::Color, 0, "background" },
    { Qt::ForegroundRole, QVariant::Brush, 0, "foreground" },
    { 0, 0, 0, 0 }
};

static AbstractItemEditor::PropertyDefinition treeItemColumnPropList[] = {
    { Qt::DisplayPropertyRole, 0, DesignerPropertyManager::designerStringTypeId, "text" },
    { Qt::DecorationPropertyRole, 0, DesignerPropertyManager::designerIconTypeId, "icon" },
    { Qt::ToolTipPropertyRole, 0, DesignerPropertyManager::designerStringTypeId, "toolTip" },
    { Qt::StatusTipPropertyRole, 0, DesignerPropertyManager::designerStringTypeId, "statusTip" },
    { Qt::WhatsThisPropertyRole, 0, DesignerPropertyManager::designerStringTypeId, "whatsThis" },
    { Qt::FontRole, QVariant::Font, 0, "font" },
    { Qt::TextAlignmentRole, 0, DesignerPropertyManager::designerAlignmentTypeId, "textAlignment" },
    { Qt::BackgroundRole, QVariant::Brush, 0, "background" },
    { Qt::ForegroundRole, QVariant::Brush, 0, "foreground" },
    { Qt::CheckStateRole, 0, QtVariantPropertyManager::enumTypeId, "checkState" },
    { 0, 0, 0, 0 }
};

static AbstractItemEditor::PropertyDefinition treeItemCommonPropList[] = {
    { ItemFlagsShadowRole, 0, QtVariantPropertyManager::flagTypeId, "flags" },
    { 0, 0, 0, 0 }
};

QtVariantProperty *TreeWidgetEditor::setupPropertyGroup(const QString &title, PropertyDefinition *propDefs)
{
    setupProperties(propDefs);
    QtVariantProperty *groupProp = m_propertyManager->addProperty(QtVariantPropertyManager::groupTypeId(), title);
    foreach (QtVariantProperty *prop, m_rootProperties)
        groupProp->addSubProperty(prop);
    m_rootProperties.clear();
    return groupProp;
}

TreeWidgetContents TreeWidgetEditor::fillContentsFromTreeWidget(QTreeWidget *treeWidget)
{
    TreeWidgetContents treeCont;
    treeCont.fromTreeWidget(treeWidget, false);
    treeCont.applyToTreeWidget(ui.treeWidget, iconCache(), true);

    treeCont.m_headerItem.applyToListWidget(m_columnEditor->listWidget(), iconCache(), true);
    m_columnEditor->setupEditor(treeWidget, treeHeaderPropList);

    QList<QtVariantProperty*> rootProperties;
    rootProperties.append(setupPropertyGroup(tr("Per column properties"), treeItemColumnPropList));
    rootProperties.append(setupPropertyGroup(tr("Common properties"), treeItemCommonPropList));
    m_rootProperties = rootProperties;
    m_propertyBrowser->setPropertiesWithoutValueMarked(true);
    m_propertyBrowser->setRootIsDecorated(false);
    setupObject(treeWidget);

    if (ui.treeWidget->topLevelItemCount() > 0)
        ui.treeWidget->setCurrentItem(ui.treeWidget->topLevelItem(0));

    updateEditor();

    return treeCont;
}

TreeWidgetContents TreeWidgetEditor::contents() const
{
    TreeWidgetContents retVal;
    retVal.fromTreeWidget(ui.treeWidget, true);
    return retVal;
}

void TreeWidgetEditor::setItemData(int role, const QVariant &v)
{
    const int col = (role == ItemFlagsShadowRole) ? 0 : ui.treeWidget->currentColumn();
    QVariant newValue = v;
    BoolBlocker block(m_updatingBrowser);
    if (role == Qt::FontRole && newValue.type() == QVariant::Font) {
        QFont oldFont = ui.treeWidget->font();
        QFont newFont = qVariantValue<QFont>(newValue).resolve(oldFont);
        newValue = qVariantFromValue(newFont);
        ui.treeWidget->currentItem()->setData(col, role, QVariant()); // force the right font with the current resolve mask is set (item view bug)
    }
    ui.treeWidget->currentItem()->setData(col, role, newValue);
}

QVariant TreeWidgetEditor::getItemData(int role) const
{
    const int col = (role == ItemFlagsShadowRole) ? 0 : ui.treeWidget->currentColumn();
    return ui.treeWidget->currentItem()->data(col, role);
}

void TreeWidgetEditor::on_newItemButton_clicked()
{
    QTreeWidgetItem *curItem = ui.treeWidget->currentItem();
    QTreeWidgetItem *newItem = 0;
    ui.treeWidget->blockSignals(true);
    if (curItem) {
        if (curItem->parent())
            newItem = new QTreeWidgetItem(curItem->parent(), curItem);
        else
            newItem = new QTreeWidgetItem(ui.treeWidget, curItem);
    } else
        newItem = new QTreeWidgetItem(ui.treeWidget);
    const QString newItemText = tr("New Item");
    newItem->setText(0, newItemText);
    newItem->setData(0, Qt::DisplayPropertyRole, qVariantFromValue(PropertySheetStringValue(newItemText)));
    newItem->setFlags(newItem->flags() | Qt::ItemIsEditable);
    ui.treeWidget->blockSignals(false);

    ui.treeWidget->setCurrentItem(newItem, qMax(ui.treeWidget->currentColumn(), 0));
    updateEditor();
    ui.treeWidget->editItem(newItem, ui.treeWidget->currentColumn());
}

void TreeWidgetEditor::on_newSubItemButton_clicked()
{
    QTreeWidgetItem *curItem = ui.treeWidget->currentItem();
    if (!curItem)
        return;

    ui.treeWidget->blockSignals(true);
    QTreeWidgetItem *newItem = new QTreeWidgetItem(curItem);
    const QString newItemText = tr("New Subitem");
    newItem->setText(0, newItemText);
    newItem->setData(0, Qt::DisplayPropertyRole, qVariantFromValue(PropertySheetStringValue(newItemText)));
    newItem->setFlags(newItem->flags() | Qt::ItemIsEditable);
    ui.treeWidget->blockSignals(false);

    ui.treeWidget->setCurrentItem(newItem, ui.treeWidget->currentColumn());
    updateEditor();
    ui.treeWidget->editItem(newItem, ui.treeWidget->currentColumn());
}

void TreeWidgetEditor::on_deleteItemButton_clicked()
{
    QTreeWidgetItem *curItem = ui.treeWidget->currentItem();
    if (!curItem)
        return;

    QTreeWidgetItem *nextCurrent = 0;
    if (curItem->parent()) {
        int idx = curItem->parent()->indexOfChild(curItem);
        if (idx == curItem->parent()->childCount() - 1)
            idx--;
        else
            idx++;
        if (idx < 0)
            nextCurrent = curItem->parent();
        else
            nextCurrent = curItem->parent()->child(idx);
    } else {
        int idx = ui.treeWidget->indexOfTopLevelItem(curItem);
        if (idx == ui.treeWidget->topLevelItemCount() - 1)
            idx--;
        else
            idx++;
        if (idx >= 0)
            nextCurrent = ui.treeWidget->topLevelItem(idx);
    }
    closeEditors();
    ui.treeWidget->blockSignals(true);
    delete curItem;
    ui.treeWidget->blockSignals(false);

    if (nextCurrent)
        ui.treeWidget->setCurrentItem(nextCurrent, ui.treeWidget->currentColumn());
    updateEditor();
}

void TreeWidgetEditor::on_moveItemUpButton_clicked()
{
    QTreeWidgetItem *curItem = ui.treeWidget->currentItem();
    if (!curItem)
        return;

    int idx;
    if (curItem->parent())
        idx = curItem->parent()->indexOfChild(curItem);
    else
        idx = ui.treeWidget->indexOfTopLevelItem(curItem);
    if (idx == 0)
        return;

    QTreeWidgetItem *takenItem;
    ui.treeWidget->blockSignals(true);
    if (curItem->parent()) {
        QTreeWidgetItem *parentItem = curItem->parent();
        takenItem = parentItem->takeChild(idx);
        parentItem->insertChild(idx - 1, takenItem);
    } else {
        takenItem = ui.treeWidget->takeTopLevelItem(idx);
        ui.treeWidget->insertTopLevelItem(idx - 1, takenItem);
    }
    ui.treeWidget->blockSignals(false);

    ui.treeWidget->setCurrentItem(takenItem, ui.treeWidget->currentColumn());
    updateEditor();
}

void TreeWidgetEditor::on_moveItemDownButton_clicked()
{
    QTreeWidgetItem *curItem = ui.treeWidget->currentItem();
    if (!curItem)
        return;

    int idx, idxCount;
    if (curItem->parent()) {
        idx = curItem->parent()->indexOfChild(curItem);
        idxCount = curItem->parent()->childCount();
    } else {
        idx = ui.treeWidget->indexOfTopLevelItem(curItem);
        idxCount = ui.treeWidget->topLevelItemCount();
    }
    if (idx == idxCount - 1)
        return;

    QTreeWidgetItem *takenItem;
    ui.treeWidget->blockSignals(true);
    if (curItem->parent()) {
        QTreeWidgetItem *parentItem = curItem->parent();
        takenItem = parentItem->takeChild(idx);
        parentItem->insertChild(idx + 1, takenItem);
    } else {
        takenItem = ui.treeWidget->takeTopLevelItem(idx);
        ui.treeWidget->insertTopLevelItem(idx + 1, takenItem);
    }
    ui.treeWidget->blockSignals(false);

    ui.treeWidget->setCurrentItem(takenItem, ui.treeWidget->currentColumn());
    updateEditor();
}

void TreeWidgetEditor::on_moveItemLeftButton_clicked()
{
    QTreeWidgetItem *curItem = ui.treeWidget->currentItem();
    if (!curItem)
        return;

    QTreeWidgetItem *parentItem = curItem->parent();
    if (!parentItem)
        return;

    ui.treeWidget->blockSignals(true);
    QTreeWidgetItem *takenItem = parentItem->takeChild(parentItem->indexOfChild(curItem));
    if (parentItem->parent()) {
        int idx = parentItem->parent()->indexOfChild(parentItem);
        parentItem->parent()->insertChild(idx, takenItem);
    } else {
        int idx = ui.treeWidget->indexOfTopLevelItem(parentItem);
        ui.treeWidget->insertTopLevelItem(idx, takenItem);
    }
    ui.treeWidget->blockSignals(false);

    ui.treeWidget->setCurrentItem(takenItem, ui.treeWidget->currentColumn());
    updateEditor();
}

void TreeWidgetEditor::on_moveItemRightButton_clicked()
{
    QTreeWidgetItem *curItem = ui.treeWidget->currentItem();
    if (!curItem)
        return;

    int idx, idxCount;
    if (curItem->parent()) {
        idx = curItem->parent()->indexOfChild(curItem);
        idxCount = curItem->parent()->childCount();
    } else {
        idx = ui.treeWidget->indexOfTopLevelItem(curItem);
        idxCount = ui.treeWidget->topLevelItemCount();
    }
    if (idx == idxCount - 1)
        return;

    QTreeWidgetItem *takenItem;
    ui.treeWidget->blockSignals(true);
    if (curItem->parent()) {
        QTreeWidgetItem *parentItem = curItem->parent()->child(idx + 1);
        takenItem = curItem->parent()->takeChild(idx);
        parentItem->insertChild(0, takenItem);
    } else {
        QTreeWidgetItem *parentItem = ui.treeWidget->topLevelItem(idx + 1);
        takenItem = ui.treeWidget->takeTopLevelItem(idx);
        parentItem->insertChild(0, takenItem);
    }
    ui.treeWidget->blockSignals(false);

    ui.treeWidget->setCurrentItem(takenItem, ui.treeWidget->currentColumn());
    updateEditor();
}

void TreeWidgetEditor::togglePropertyBrowser()
{
    // Always hide in case parent widget is not visible -> on startup
    const bool isVisible =
            !this->isVisible() ? true : m_propertyBrowser->isVisible();
    if (isVisible)
        ui.showPropertiesButton->setText(tr("Properties &<<"));
    else
        ui.showPropertiesButton->setText(tr("Properties &>>"));

    m_propertyBrowser->setVisible(!isVisible);
}

void TreeWidgetEditor::on_treeWidget_currentItemChanged()
{
    m_columnEditor->setCurrentIndex(ui.treeWidget->currentColumn());
    updateEditor();
}

void TreeWidgetEditor::on_treeWidget_itemChanged(QTreeWidgetItem *item, int column)
{
    if (m_updatingBrowser)
        return;

    PropertySheetStringValue val = qVariantValue<PropertySheetStringValue>(item->data(column, Qt::DisplayPropertyRole));
    val.setValue(item->text(column));
    BoolBlocker block(m_updatingBrowser);
    item->setData(column, Qt::DisplayPropertyRole, qVariantFromValue(val));

    updateBrowser();
}

void TreeWidgetEditor::on_columnEditor_indexChanged(int idx)
{
    if (QTreeWidgetItem *item = ui.treeWidget->currentItem())
        ui.treeWidget->setCurrentItem(item, idx);
}

void TreeWidgetEditor::on_columnEditor_itemChanged(int idx, int role, const QVariant &v)
{
    if (role == Qt::DisplayPropertyRole)
        ui.treeWidget->headerItem()->setData(idx, Qt::EditRole, qVariantValue<PropertySheetStringValue>(v).value());
    ui.treeWidget->headerItem()->setData(idx, role, v);
}

void TreeWidgetEditor::updateEditor()
{
    QTreeWidgetItem *current = ui.treeWidget->currentItem();

    bool itemsEnabled = false;
    bool currentItemEnabled = false;
    bool moveItemUpEnabled = false;
    bool moveItemDownEnabled = false;
    bool moveItemRightEnabled = false;
    bool moveItemLeftEnabled = false;

    if (ui.treeWidget->columnCount() > 0) {
        itemsEnabled = true;
        if (current) {
            int idx;
            int idxCount;
            currentItemEnabled = true;
            if (current->parent()) {
                moveItemLeftEnabled = true;
                idx = current->parent()->indexOfChild(current);
                idxCount = current->parent()->childCount();
            } else {
                idx = ui.treeWidget->indexOfTopLevelItem(current);
                idxCount = ui.treeWidget->topLevelItemCount();
            }
            if (idx > 0)
                moveItemUpEnabled = true;
            if (idx < idxCount - 1) {
                moveItemDownEnabled = true;
                moveItemRightEnabled = true;
            }
        }
    }
    ui.tabWidget->setTabEnabled(1, itemsEnabled);
    ui.newSubItemButton->setEnabled(currentItemEnabled);
    ui.deleteItemButton->setEnabled(currentItemEnabled);

    ui.moveItemUpButton->setEnabled(moveItemUpEnabled);
    ui.moveItemDownButton->setEnabled(moveItemDownEnabled);
    ui.moveItemRightButton->setEnabled(moveItemRightEnabled);
    ui.moveItemLeftButton->setEnabled(moveItemLeftEnabled);

    if (current)
        updateBrowser();
    else
        m_propertyBrowser->clear();
}

void TreeWidgetEditor::moveColumnItems(const PropertyDefinition *propList,
        QTreeWidgetItem *item, int fromColumn, int toColumn, int step)
{
    BoolBlocker block(m_updatingBrowser);

    QList<QVariant> saveCol;
    for (int j = 0; propList[j].name; j++)
        saveCol.append(item->data(toColumn, propList[j].role));
    QVariant editVariant = item->data(toColumn, Qt::EditRole);
    QVariant toolTipVariant = item->data(toColumn, Qt::ToolTipRole);
    QVariant statusTipVariant = item->data(toColumn, Qt::StatusTipRole);
    QVariant whatsThisVariant = item->data(toColumn, Qt::WhatsThisRole);
    QVariant decorationVariant = item->data(toColumn, Qt::DecorationRole);
    for (int i = toColumn; i != fromColumn; i += step) {
        for (int j = 0; propList[j].name; j++)
            item->setData(i, propList[j].role, item->data(i + step, propList[j].role));
        item->setData(i, Qt::EditRole, item->data(i + step, Qt::EditRole));
        item->setData(i, Qt::ToolTipRole, item->data(i + step, Qt::ToolTipRole));
        item->setData(i, Qt::StatusTipRole, item->data(i + step, Qt::StatusTipRole));
        item->setData(i, Qt::WhatsThisRole, item->data(i + step, Qt::WhatsThisRole));
        item->setData(i, Qt::DecorationRole, item->data(i + step, Qt::DecorationRole));
    }
    for (int j = 0; propList[j].name; j++)
        item->setData(fromColumn, propList[j].role, saveCol[j]);
    item->setData(fromColumn, Qt::EditRole, editVariant);
    item->setData(fromColumn, Qt::ToolTipRole, toolTipVariant);
    item->setData(fromColumn, Qt::StatusTipRole, statusTipVariant);
    item->setData(fromColumn, Qt::WhatsThisRole, whatsThisVariant);
    item->setData(fromColumn, Qt::DecorationRole, decorationVariant);
}

void TreeWidgetEditor::moveColumns(int fromColumn, int toColumn, int step)
{
    ui.treeWidget->blockSignals(true);

    moveColumnItems(treeHeaderPropList, ui.treeWidget->headerItem(), fromColumn, toColumn, step);

    QQueue<QTreeWidgetItem *> pendingQueue;
    for (int i = 0; i < ui.treeWidget->topLevelItemCount(); i++)
        pendingQueue.enqueue(ui.treeWidget->topLevelItem(i));

    while (!pendingQueue.isEmpty()) {
        QTreeWidgetItem *item = pendingQueue.dequeue();
        for (int i = 0; i < item->childCount(); i++)
            pendingQueue.enqueue(item->child(i));

        moveColumnItems(treeItemColumnPropList, item, fromColumn, toColumn, step);
    }

    ui.treeWidget->blockSignals(false);
}

void TreeWidgetEditor::moveColumnsLeft(int fromColumn, int toColumn)
{
    if (fromColumn >= toColumn)
        return;

    moveColumns(fromColumn, toColumn, -1);
}

void TreeWidgetEditor::moveColumnsRight(int fromColumn, int toColumn)
{
    if (fromColumn >= toColumn)
        return;

    moveColumns(toColumn, fromColumn, 1);
}

void TreeWidgetEditor::on_columnEditor_itemInserted(int idx)
{
    int columnCount = ui.treeWidget->columnCount();
    ui.treeWidget->setColumnCount(columnCount + 1);
    ui.treeWidget->headerItem()->setText(columnCount, m_columnEditor->newItemText());
    moveColumnsLeft(idx, columnCount);

    updateEditor();
}

void TreeWidgetEditor::on_columnEditor_itemDeleted(int idx)
{
    closeEditors();

    int columnCount = ui.treeWidget->columnCount() - 1;
    if (!columnCount)
        ui.treeWidget->clear();
    else
        moveColumnsRight(idx, columnCount);
    ui.treeWidget->setColumnCount(columnCount);

    updateEditor();
}

void TreeWidgetEditor::on_columnEditor_itemMovedUp(int idx)
{
    moveColumnsRight(idx - 1, idx);

    ui.treeWidget->setCurrentItem(ui.treeWidget->currentItem(), idx - 1);
    updateEditor();
}

void TreeWidgetEditor::on_columnEditor_itemMovedDown(int idx)
{
    moveColumnsLeft(idx, idx + 1);

    ui.treeWidget->setCurrentItem(ui.treeWidget->currentItem(), idx + 1);
    updateEditor();
}

void TreeWidgetEditor::closeEditors()
{
    if (QTreeWidgetItem *cur = ui.treeWidget->currentItem() ) {
        const int numCols = cur->columnCount ();
        for (int i = 0; i < numCols; i++)
            ui.treeWidget->closePersistentEditor (cur, i);
    }
}

void TreeWidgetEditor::cacheReloaded()
{
    reloadIconResources(iconCache(), ui.treeWidget);
}

}
QT_END_NAMESPACE
