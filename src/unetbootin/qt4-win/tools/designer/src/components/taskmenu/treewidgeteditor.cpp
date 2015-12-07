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
TRANSLATOR qdesigner_internal::TreeWidgetEditor
*/

#include "treewidgeteditor.h"
#include <formwindowbase_p.h>
#include <iconloader_p.h>
#include <qdesigner_command_p.h>
#include <qdesigner_utils_p.h>
#include <abstractformbuilder.h>

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
    : QDialog(parent), m_updating(false)
{
    ui.setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    m_form = qobject_cast<FormWindowBase *>(form);
    ui.itemIconSelector->setFormEditor(form->core());
    ui.itemIconSelector->setEnabled(false);
    ui.itemIconSelector->setPixmapCache(m_form->pixmapCache());
    ui.itemIconSelector->setIconCache(m_form->iconCache());

    ui.columnIconSelector->setFormEditor(form->core());
    ui.columnIconSelector->setEnabled(false);
    ui.columnIconSelector->setPixmapCache(m_form->pixmapCache());
    ui.columnIconSelector->setIconCache(m_form->iconCache());

    QIcon upIcon = createIconSet(QString::fromUtf8("up.png"));
    QIcon downIcon = createIconSet(QString::fromUtf8("down.png"));
    QIcon backIcon = createIconSet(QString::fromUtf8("back.png"));
    QIcon forwardIcon = createIconSet(QString::fromUtf8("forward.png"));
    QIcon minusIcon = createIconSet(QString::fromUtf8("minus.png"));
    QIcon plusIcon = createIconSet(QString::fromUtf8("plus.png"));
    ui.newColumnButton->setIcon(plusIcon);
    ui.deleteColumnButton->setIcon(minusIcon);
    ui.moveColumnUpButton->setIcon(upIcon);
    ui.moveColumnDownButton->setIcon(downIcon);
    ui.newItemButton->setIcon(plusIcon);
    ui.deleteItemButton->setIcon(minusIcon);
    ui.moveItemUpButton->setIcon(upIcon);
    ui.moveItemDownButton->setIcon(downIcon);
    ui.moveItemRightButton->setIcon(forwardIcon);
    ui.moveItemLeftButton->setIcon(backIcon);

    ui.treeWidget->header()->setMovable(false);

    connect(m_form->iconCache(), SIGNAL(reloaded()), this, SLOT(cacheReloaded()));
}

TreeWidgetEditor::~TreeWidgetEditor()
{
}

void TreeWidgetEditor::fillContentsFromTreeWidget(QTreeWidget *treeWidget)
{
    m_updating = true;
    copyContents(treeWidget, ui.treeWidget);

    ui.listWidget->clear();

    QTreeWidgetItem *headerItem = ui.treeWidget->headerItem();
    int colCount = ui.treeWidget->columnCount();
    for (int col = 0; col < colCount; col++) {
        QListWidgetItem *item = new QListWidgetItem(ui.listWidget);
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        item->setText(headerItem->text(col));
        item->setIcon(headerItem->icon(col));
        item->setData(QAbstractFormBuilder::resourceRole(), headerItem->data(col, QAbstractFormBuilder::resourceRole()));
    }

    if (colCount > 0) {
        ui.listWidget->setCurrentRow(0);
        if (ui.treeWidget->topLevelItemCount() > 0)
            ui.treeWidget->setCurrentItem(ui.treeWidget->topLevelItem(0));
    }
    m_updating = false;
    updateEditor();
}

void TreeWidgetEditor::fillTreeWidgetFromContents(QTreeWidget *treeWidget)
{
    ChangeTreeContentsCommand *cmd = new ChangeTreeContentsCommand(m_form);
    cmd->init(treeWidget, ui.treeWidget);
    m_form->commandHistory()->push(cmd);
}

void TreeWidgetEditor::copyContents(QTreeWidget *sourceWidget, QTreeWidget *destWidget)
{
    destWidget->clear();

    int colCount = sourceWidget->columnCount();
    destWidget->setColumnCount(colCount);
    destWidget->setHeaderItem(sourceWidget->headerItem()->clone());

    const int topLevelItemCount = sourceWidget->topLevelItemCount();
    for (int i = 0; i < topLevelItemCount; i++)
        destWidget->addTopLevelItem(sourceWidget->topLevelItem(i)->clone());

    for (QTreeWidgetItemIterator it(destWidget); *it ; ++it)
        (*it)->setFlags((*it)->flags() | Qt::ItemIsEditable);
}

void TreeWidgetEditor::on_newItemButton_clicked()
{
    m_updating = true;
    QTreeWidgetItem *curItem = ui.treeWidget->currentItem();
    QTreeWidgetItem *newItem = 0;
    if (curItem) {
        if (curItem->parent())
            newItem = new QTreeWidgetItem(curItem->parent(), curItem);
        else
            newItem = new QTreeWidgetItem(ui.treeWidget, curItem);
    } else
        newItem = new QTreeWidgetItem(ui.treeWidget);
    newItem->setText(0, tr("New Item"));
    newItem->setFlags(newItem->flags() | Qt::ItemIsEditable);

    ui.treeWidget->setCurrentItem(newItem, ui.listWidget->currentRow());
    m_updating = false;
    updateEditor();
    ui.treeWidget->editItem(newItem, ui.listWidget->currentRow());
}

void TreeWidgetEditor::on_newSubItemButton_clicked()
{
    QTreeWidgetItem *curItem = ui.treeWidget->currentItem();
    if (!curItem)
        return;

    m_updating = true;

    QTreeWidgetItem *newItem = new QTreeWidgetItem(curItem);
    newItem->setText(0, tr("New Sub Item"));
    newItem->setFlags(newItem->flags() | Qt::ItemIsEditable);

    ui.treeWidget->setCurrentItem(newItem, ui.listWidget->currentRow());
    m_updating = false;
    updateEditor();
    ui.treeWidget->editItem(newItem, ui.listWidget->currentRow());
}

void TreeWidgetEditor::on_deleteItemButton_clicked()
{
    QTreeWidgetItem *curItem = ui.treeWidget->currentItem();
    if (!curItem)
        return;

    m_updating = true;

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
    delete curItem;

    if (nextCurrent)
        ui.treeWidget->setCurrentItem(nextCurrent, ui.listWidget->currentRow());

    m_updating = false;
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

    m_updating = true;

    QTreeWidgetItem *takenItem = 0;
    if (curItem->parent()) {
        QTreeWidgetItem *parentItem = curItem->parent();
        takenItem = parentItem->takeChild(idx);
        parentItem->insertChild(idx - 1, takenItem);
    } else {
        takenItem = ui.treeWidget->takeTopLevelItem(idx);
        ui.treeWidget->insertTopLevelItem(idx - 1, takenItem);
    }
    ui.treeWidget->setCurrentItem(takenItem, ui.listWidget->currentRow());

    m_updating = false;
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

    m_updating = true;

    QTreeWidgetItem *takenItem = 0;
    if (curItem->parent()) {
        QTreeWidgetItem *parentItem = curItem->parent();
        takenItem = parentItem->takeChild(idx);
        parentItem->insertChild(idx + 1, takenItem);
    } else {
        takenItem = ui.treeWidget->takeTopLevelItem(idx);
        ui.treeWidget->insertTopLevelItem(idx + 1, takenItem);
    }
    ui.treeWidget->setCurrentItem(takenItem, ui.listWidget->currentRow());

    m_updating = false;
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

    m_updating = true;

    QTreeWidgetItem *takenItem = parentItem->takeChild(parentItem->indexOfChild(curItem));
    if (parentItem->parent()) {
        int idx = parentItem->parent()->indexOfChild(parentItem);
        parentItem->parent()->insertChild(idx, takenItem);
    } else {
        int idx = ui.treeWidget->indexOfTopLevelItem(parentItem);
        ui.treeWidget->insertTopLevelItem(idx, takenItem);
    }
    ui.treeWidget->setCurrentItem(takenItem, ui.listWidget->currentRow());

    m_updating = false;
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

    m_updating = true;

    QTreeWidgetItem *takenItem = 0;
    if (curItem->parent()) {
        QTreeWidgetItem *parentItem = curItem->parent()->child(idx + 1);
        takenItem = curItem->parent()->takeChild(idx);
        parentItem->insertChild(0, takenItem);
    } else {
        QTreeWidgetItem *parentItem = ui.treeWidget->topLevelItem(idx + 1);
        takenItem = ui.treeWidget->takeTopLevelItem(idx);
        parentItem->insertChild(0, takenItem);
    }
    ui.treeWidget->setCurrentItem(takenItem, ui.listWidget->currentRow());

    m_updating = false;
    updateEditor();
}

void TreeWidgetEditor::on_treeWidget_currentItemChanged(QTreeWidgetItem *,
            QTreeWidgetItem *)
{
    if (m_updating)
        return;
    QModelIndex idx = ui.treeWidget->selectionModel()->currentIndex();
    m_updating = true;
    ui.listWidget->setCurrentRow(idx.column());
    m_updating = false;
    updateEditor();
}

void TreeWidgetEditor::on_treeWidget_itemChanged(QTreeWidgetItem *)
{
    if (m_updating)
        return;
    updateEditor();
}

void TreeWidgetEditor::on_listWidget_currentRowChanged(int)
{
    if (m_updating)
        return;
    m_updating = true;
    QListWidgetItem *currentColumn = ui.listWidget->currentItem();
    QTreeWidgetItem *currentItem = ui.treeWidget->currentItem();
    if (currentItem && currentColumn) {
        int col = ui.listWidget->currentRow();
        ui.treeWidget->setCurrentItem(currentItem, col);
    }
    m_updating = false;
    updateEditor();
}

void TreeWidgetEditor::on_listWidget_itemChanged(QListWidgetItem *)
{
    if (m_updating)
        return;
    updateEditor();
}

void TreeWidgetEditor::updateEditor()
{
    for (int i = 0; i < ui.listWidget->count(); i++)
        ui.treeWidget->headerItem()->setText(i, ui.listWidget->item(i)->text());

    QTreeWidgetItem *current = ui.treeWidget->currentItem();
    QListWidgetItem *currentColumn = ui.listWidget->currentItem();

    bool itemsEnabled = false;
    bool currentItemEnabled = false;
    bool moveItemUpEnabled = false;
    bool moveItemDownEnabled = false;
    bool moveItemRightEnabled = false;
    bool moveItemLeftEnabled = false;

    bool currentColumnEnabled = false;
    bool moveColumnUpEnabled = false;
    bool moveColumnDownEnabled = false;

    if (currentColumn) {
        currentColumnEnabled = true;
        int idx = ui.listWidget->currentRow();
        if (idx > 0)
            moveColumnUpEnabled = true;
        if (idx < ui.listWidget->count() - 1)
            moveColumnDownEnabled = true;

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
    ui.itemsBox->setEnabled(itemsEnabled);
    ui.itemIconSelector->setEnabled(currentItemEnabled);
    ui.newSubItemButton->setEnabled(currentItemEnabled);
    ui.deleteItemButton->setEnabled(currentItemEnabled);

    ui.moveItemUpButton->setEnabled(moveItemUpEnabled);
    ui.moveItemDownButton->setEnabled(moveItemDownEnabled);
    ui.moveItemRightButton->setEnabled(moveItemRightEnabled);
    ui.moveItemLeftButton->setEnabled(moveItemLeftEnabled);

    ui.deleteColumnButton->setEnabled(currentColumnEnabled);
    ui.columnIconSelector->setEnabled(currentColumnEnabled);
    ui.moveColumnUpButton->setEnabled(moveColumnUpEnabled);
    ui.moveColumnDownButton->setEnabled(moveColumnDownEnabled);

    QString itemText, columnText;
    PropertySheetIconValue itemIcon, columnIcon;

    if (currentColumn) {
        int idx = ui.listWidget->currentRow();
        columnIcon = qVariantValue<PropertySheetIconValue>(ui.treeWidget->headerItem()->data(idx, QAbstractFormBuilder::resourceRole()));

        if (current) {
            itemText = current->text(idx);
            itemIcon = qVariantValue<PropertySheetIconValue>(current->data(idx, QAbstractFormBuilder::resourceRole()));
        }
    }

    ui.itemIconSelector->setIcon(itemIcon);
    ui.columnIconSelector->setIcon(columnIcon);
}

void TreeWidgetEditor::on_itemIconSelector_iconChanged(const PropertySheetIconValue &icon)
{
    QTreeWidgetItem *curItem = ui.treeWidget->currentItem();
    if (!curItem)
        return;

    if (!ui.listWidget->currentItem())
        return;

    curItem->setData(ui.listWidget->currentRow(), QAbstractFormBuilder::resourceRole(), qVariantFromValue(icon));
    curItem->setIcon(ui.listWidget->currentRow(), m_form->iconCache()->icon(icon));
}

void TreeWidgetEditor::moveColumnsLeft(int fromColumn, int toColumn)
{
    if (fromColumn >= toColumn)
        return;

    QTreeWidgetItem *headerItem = ui.treeWidget->headerItem();
    const QString text = headerItem->text(toColumn);
    const QIcon icon = headerItem->icon(toColumn);
    const QVariant data = headerItem->data(toColumn, QAbstractFormBuilder::resourceRole());
    for (int i = toColumn; i > fromColumn; i--) {
        headerItem->setText(i, headerItem->text(i - 1));
        headerItem->setIcon(i, headerItem->icon(i - 1));
        headerItem->setData(i, QAbstractFormBuilder::resourceRole(), headerItem->data(i - 1, QAbstractFormBuilder::resourceRole()));
    }
    headerItem->setText(fromColumn, text);
    headerItem->setIcon(fromColumn, icon);
    headerItem->setData(fromColumn, QAbstractFormBuilder::resourceRole(), data);

    QQueue<QTreeWidgetItem *> pendingQueue;
    for (int i = 0; i < ui.treeWidget->topLevelItemCount(); i++)
        pendingQueue.enqueue(ui.treeWidget->topLevelItem(i));

    while (!pendingQueue.isEmpty()) {
        QTreeWidgetItem *item = pendingQueue.dequeue();
        for (int i = 0; i < item->childCount(); i++)
            pendingQueue.enqueue(item->child(i));

        const QString text = item->text(toColumn);
        const QIcon icon = item->icon(toColumn);
        const QVariant data = item->data(toColumn, QAbstractFormBuilder::resourceRole());
        for (int i = toColumn; i > fromColumn; i--) {
            item->setText(i, item->text(i - 1));
            item->setIcon(i, item->icon(i - 1));
            item->setData(i, QAbstractFormBuilder::resourceRole(), headerItem->data(i - 1, QAbstractFormBuilder::resourceRole()));
        }
        item->setText(fromColumn, text);
        item->setIcon(fromColumn, icon);
        item->setData(fromColumn, QAbstractFormBuilder::resourceRole(), data);
    }
}

void TreeWidgetEditor::moveColumnsRight(int fromColumn, int toColumn)
{
    if (fromColumn >= toColumn)
        return;

    QTreeWidgetItem *headerItem = ui.treeWidget->headerItem();
    const QString text = headerItem->text(fromColumn);
    const QIcon icon = headerItem->icon(fromColumn);
    const QVariant data = headerItem->data(fromColumn, QAbstractFormBuilder::resourceRole());
    for (int i = fromColumn; i < toColumn; i++) {
        headerItem->setText(i, headerItem->text(i + 1));
        headerItem->setIcon(i, headerItem->icon(i + 1));
        headerItem->setData(i, QAbstractFormBuilder::resourceRole(), headerItem->data(i + 1, QAbstractFormBuilder::resourceRole()));
    }
    headerItem->setText(toColumn, text);
    headerItem->setIcon(toColumn, icon);
    headerItem->setData(toColumn, QAbstractFormBuilder::resourceRole(), data);

    QQueue<QTreeWidgetItem *> pendingQueue;
    for (int i = 0; i < ui.treeWidget->topLevelItemCount(); i++)
        pendingQueue.enqueue(ui.treeWidget->topLevelItem(i));

    while (!pendingQueue.isEmpty()) {
        QTreeWidgetItem *item = pendingQueue.dequeue();
        for (int i = 0; i < item->childCount(); i++)
            pendingQueue.enqueue(item->child(i));

        const QString text = item->text(fromColumn);
        const QIcon icon = item->icon(fromColumn);
        const QVariant data = item->data(fromColumn, QAbstractFormBuilder::resourceRole());
        for (int i = fromColumn; i < toColumn; i++) {
            item->setText(i, item->text(i + 1));
            item->setIcon(i, item->icon(i + 1));
            item->setData(i, QAbstractFormBuilder::resourceRole(), item->data(i + 1, QAbstractFormBuilder::resourceRole()));
        }
        item->setText(toColumn, text);
        item->setIcon(toColumn, icon);
        item->setData(toColumn, QAbstractFormBuilder::resourceRole(), icon);
    }
}

void TreeWidgetEditor::on_newColumnButton_clicked()
{
    m_updating = true;
    QListWidgetItem *currentColumn = ui.listWidget->currentItem();
    int idx = ui.listWidget->count();
    if (currentColumn)
        idx = ui.listWidget->currentRow() + 1;

    QString newColumnString = tr("New Column");

    int columnCount = ui.treeWidget->columnCount();
    ui.treeWidget->setColumnCount(columnCount + 1);

    ui.treeWidget->headerItem()->setText(columnCount, newColumnString);
    moveColumnsLeft(idx, columnCount);

    QListWidgetItem *item = new QListWidgetItem();
    item->setFlags(item->flags() | Qt::ItemIsEditable);
    item->setText(newColumnString);
    ui.listWidget->insertItem(idx, item);
    ui.listWidget->setCurrentItem(item);

    ui.treeWidget->setCurrentItem(ui.treeWidget->currentItem(), ui.listWidget->currentRow());

    m_updating = false;
    updateEditor();

    ui.listWidget->editItem(item);
}

void TreeWidgetEditor::on_deleteColumnButton_clicked()
{
    QListWidgetItem *currentColumn = ui.listWidget->currentItem();
    if (!currentColumn)
        return;

    m_updating = true;

    int idx = ui.listWidget->currentRow();
    int columnCount = ui.treeWidget->columnCount();

    moveColumnsRight(idx, columnCount - 1);
    ui.treeWidget->setColumnCount(columnCount - 1);

    closeEditors();
    delete currentColumn;
    if (idx == columnCount - 1)
        idx--;
    if (idx >= 0)
        ui.listWidget->setCurrentRow(idx);

    m_updating = false;
    updateEditor();
}

void TreeWidgetEditor::on_moveColumnUpButton_clicked()
{
    QListWidgetItem *currentColumn = ui.listWidget->currentItem();
    if (!currentColumn)
        return;

    int idx = ui.listWidget->currentRow();

    if (idx == 0)
        return;

    m_updating = true;

    moveColumnsRight(idx - 1, idx);
    ui.listWidget->takeItem(idx);
    ui.listWidget->insertItem(idx - 1, currentColumn);
    ui.listWidget->setCurrentItem(currentColumn);

    ui.treeWidget->setCurrentItem(ui.treeWidget->currentItem(), ui.listWidget->currentRow());

    m_updating = false;
    updateEditor();
}

void TreeWidgetEditor::on_moveColumnDownButton_clicked()
{
    QListWidgetItem *currentColumn = ui.listWidget->currentItem();
    if (!currentColumn)
        return;

    int idx = ui.listWidget->currentRow();
    int columnCount = ui.treeWidget->columnCount();

    if (idx == columnCount - 1)
        return;

    m_updating = true;

    moveColumnsLeft(idx, idx + 1);
    ui.listWidget->takeItem(idx);
    ui.listWidget->insertItem(idx + 1, currentColumn);
    ui.listWidget->setCurrentItem(currentColumn);

    ui.treeWidget->setCurrentItem(ui.treeWidget->currentItem(), ui.listWidget->currentRow());

    m_updating = false;
    updateEditor();
}

void TreeWidgetEditor::on_columnIconSelector_iconChanged(const PropertySheetIconValue &icon)
{
    QListWidgetItem *curItem = ui.listWidget->currentItem();
    if (!curItem)
        return;

    m_updating = true;
    const QVariant data = qVariantFromValue(icon);
    const QIcon itemIcon = m_form->iconCache()->icon(icon);
    curItem->setData(QAbstractFormBuilder::resourceRole(), data);
    curItem->setIcon(itemIcon);
    ui.treeWidget->headerItem()->setData(ui.listWidget->currentRow(), QAbstractFormBuilder::resourceRole(), data);
    ui.treeWidget->headerItem()->setIcon(ui.listWidget->currentRow(), itemIcon);
    m_updating = false;
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
    reloadIconResources(m_form->iconCache(), ui.treeWidget);
    reloadIconResources(m_form->iconCache(), ui.listWidget);
}

}
QT_END_NAMESPACE
