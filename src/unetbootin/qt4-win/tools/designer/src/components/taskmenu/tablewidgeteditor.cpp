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
TRANSLATOR qdesigner_internal::TableWidgetEditor
*/

#include "tablewidgeteditor.h"
#include <abstractformbuilder.h>
#include <iconloader_p.h>
#include <qdesigner_command_p.h>
#include "formwindowbase_p.h"
#include "qdesigner_utils_p.h"

#include <QtDesigner/QDesignerFormWindowInterface>
#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QDesignerIconCacheInterface>
#include <QtCore/QDir>
#include <QtCore/QQueue>
#include <QtCore/QTextStream>

QT_BEGIN_NAMESPACE

using namespace qdesigner_internal;

TableWidgetEditor::TableWidgetEditor(QDesignerFormWindowInterface *form, QWidget *parent)
    : QDialog(parent),
      m_form(qobject_cast<FormWindowBase *>(form)),
      m_updating(false)
{
    ui.setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    ui.itemIconSelector->setFormEditor(form->core());
    ui.itemIconSelector->setEnabled(false);
    ui.itemIconSelector->setPixmapCache(m_form->pixmapCache());
    ui.itemIconSelector->setIconCache(m_form->iconCache());

    ui.columnIconSelector->setFormEditor(form->core());
    ui.columnIconSelector->setEnabled(false);
    ui.columnIconSelector->setPixmapCache(m_form->pixmapCache());
    ui.columnIconSelector->setIconCache(m_form->iconCache());

    ui.rowIconSelector->setFormEditor(form->core());
    ui.rowIconSelector->setEnabled(false);
    ui.rowIconSelector->setPixmapCache(m_form->pixmapCache());
    ui.rowIconSelector->setIconCache(m_form->iconCache());

    QIcon upIcon = createIconSet(QString::fromUtf8("up.png"));
    QIcon downIcon = createIconSet(QString::fromUtf8("down.png"));
    QIcon minusIcon = createIconSet(QString::fromUtf8("minus.png"));
    QIcon plusIcon = createIconSet(QString::fromUtf8("plus.png"));

    ui.newColumnButton->setIcon(plusIcon);
    ui.deleteColumnButton->setIcon(minusIcon);
    ui.moveColumnUpButton->setIcon(upIcon);
    ui.moveColumnDownButton->setIcon(downIcon);
    ui.newRowButton->setIcon(plusIcon);
    ui.deleteRowButton->setIcon(minusIcon);
    ui.moveRowUpButton->setIcon(upIcon);
    ui.moveRowDownButton->setIcon(downIcon);

    ui.tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);

    connect(m_form->iconCache(), SIGNAL(reloaded()), this, SLOT(cacheReloaded()));
}

TableWidgetEditor::~TableWidgetEditor()
{
}

void TableWidgetEditor::fillContentsFromTableWidget(QTableWidget *tableWidget)
{
    m_updating = true;
    copyContents(tableWidget, ui.tableWidget);

    ui.columnsListWidget->clear();
    ui.rowsListWidget->clear();

    const int colCount = ui.tableWidget->columnCount();
    for (int col = 0; col < colCount; col++) {
        const QTableWidgetItem *headerItem = ui.tableWidget->horizontalHeaderItem(col);
        QListWidgetItem *item = new QListWidgetItem(ui.columnsListWidget);
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        if (headerItem) {
            item->setText(headerItem->text());
            item->setIcon(headerItem->icon());
            item->setData(QAbstractFormBuilder::resourceRole(), headerItem->data(QAbstractFormBuilder::resourceRole()));
        } else
            item->setText(TableWidgetContents::defaultHeaderText(col));
    }
    if (colCount > 0)
        ui.columnsListWidget->setCurrentRow(0);

    const int rowCount = ui.tableWidget->rowCount();
    for (int row = 0; row < rowCount; row++) {
        const QTableWidgetItem *headerItem = ui.tableWidget->verticalHeaderItem(row);
        QListWidgetItem *item = new QListWidgetItem(ui.rowsListWidget);
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        if (headerItem) {
            item->setText(headerItem->text());
            item->setIcon(headerItem->icon());
            item->setData(QAbstractFormBuilder::resourceRole(), headerItem->data(QAbstractFormBuilder::resourceRole()));
        } else
            item->setText(TableWidgetContents::defaultHeaderText(row));
    }
    if (rowCount > 0)
        ui.rowsListWidget->setCurrentRow(0);

    if (ui.tableWidget->columnCount() > 0 && ui.tableWidget->rowCount() > 0)
        ui.tableWidget->setCurrentCell(0, 0);
    m_updating = false;
    updateEditor();
}

void TableWidgetEditor::fillTableWidgetFromContents(QTableWidget *tableWidget)
{
    ChangeTableContentsCommand *cmd = new ChangeTableContentsCommand(m_form);
    if (cmd->init(tableWidget, ui.tableWidget))
        m_form->commandHistory()->push(cmd);
    else
        delete cmd;
}


void TableWidgetEditor::copyContents(QTableWidget *sourceWidget, QTableWidget *destWidget)
{
    destWidget->clear();

    const int colCount = sourceWidget->columnCount();
    destWidget->setColumnCount(colCount);
    for (int col = 0; col < colCount; col++) {
        QTableWidgetItem *origHeaderItem = sourceWidget->horizontalHeaderItem(col);
        QTableWidgetItem *headerItem = destWidget->horizontalHeaderItem(col);
        if (origHeaderItem) {
            if (!headerItem)
                headerItem = new QTableWidgetItem;
            headerItem->setText(origHeaderItem->text());
            headerItem->setIcon(origHeaderItem->icon());
            headerItem->setData(QAbstractFormBuilder::resourceRole(), origHeaderItem->data(QAbstractFormBuilder::resourceRole()));
            destWidget->setHorizontalHeaderItem(col, headerItem);
        } else {
            if (headerItem)
                delete headerItem;
        }
    }

    const int rowCount = sourceWidget->rowCount();
    destWidget->setRowCount(rowCount);
    for (int row = 0; row < rowCount; row++) {
        QTableWidgetItem *origHeaderItem = sourceWidget->verticalHeaderItem(row);
        QTableWidgetItem *headerItem = destWidget->verticalHeaderItem(row);
        if (origHeaderItem) {
            if (!headerItem)
                headerItem = new QTableWidgetItem;
            headerItem->setText(origHeaderItem->text());
            headerItem->setIcon(origHeaderItem->icon());
            headerItem->setData(QAbstractFormBuilder::resourceRole(), origHeaderItem->data(QAbstractFormBuilder::resourceRole()));
            destWidget->setVerticalHeaderItem(row, headerItem);
        } else {
            if (headerItem)
                delete headerItem;
        }
    }

    for (int col = 0; col < colCount; col++) {
        for (int row = 0; row < rowCount; row++) {
            QTableWidgetItem *origItem = sourceWidget->item(row, col);
            QTableWidgetItem *newItem = destWidget->item(row, col);
            if (origItem) {
                const QString text = origItem->text();
                const QIcon icon = origItem->icon();
                const QVariant data = origItem->data(QAbstractFormBuilder::resourceRole());
                if (text.isEmpty() && icon.isNull()) {
                    if (newItem)
                        delete newItem;
                } else {
                    if (!newItem)
                        newItem = new QTableWidgetItem;
                    newItem->setText(text);
                    newItem->setIcon(icon);
                    newItem->setData(QAbstractFormBuilder::resourceRole(), data);
                    destWidget->setItem(row, col, newItem);
                }
            } else {
                if (newItem)
                    delete newItem;
            }
        }
    }
}

void TableWidgetEditor::on_tableWidget_currentCellChanged(int currentRow, int currentCol, int, int)
{
    if (m_updating)
        return;
    m_updating = true;
    ui.rowsListWidget->setCurrentRow(currentRow);
    ui.columnsListWidget->setCurrentRow(currentCol);
    m_updating = false;
    updateEditor();
}

void TableWidgetEditor::on_tableWidget_itemChanged(QTableWidgetItem *)
{
    if (m_updating)
        return;
    updateEditor();
}

void TableWidgetEditor::on_columnsListWidget_currentRowChanged(int col)
{
    if (m_updating)
        return;
    m_updating = true;
    QListWidgetItem *currentRow = ui.rowsListWidget->currentItem();
    if (currentRow) {
        int row = ui.rowsListWidget->currentRow();
        ui.tableWidget->setCurrentCell(row, col);
    }
    m_updating = false;
    updateEditor();
}

void TableWidgetEditor::on_columnsListWidget_itemChanged(QListWidgetItem *)
{
    if (m_updating)
        return;
    updateEditor();
}

void TableWidgetEditor::on_rowsListWidget_currentRowChanged(int row)
{
    if (m_updating)
        return;
    m_updating = true;
    QListWidgetItem *currentColumn = ui.columnsListWidget->currentItem();
    if (currentColumn) {
        int col = ui.columnsListWidget->currentRow();
        ui.tableWidget->setCurrentCell(row, col);
    }
    m_updating = false;
    updateEditor();
}

void TableWidgetEditor::on_rowsListWidget_itemChanged(QListWidgetItem *)
{
    if (m_updating)
        return;
    updateEditor();
}

void TableWidgetEditor::updateEditor()
{
    for (int i = 0; i < ui.columnsListWidget->count(); i++) {
        QTableWidgetItem *headerItem = ui.tableWidget->horizontalHeaderItem(i);
        if (!headerItem) {
            headerItem = new QTableWidgetItem;
            ui.tableWidget->setHorizontalHeaderItem(i, headerItem);
        }
        headerItem->setText(ui.columnsListWidget->item(i)->text());
    }
    for (int i = 0; i < ui.rowsListWidget->count(); i++) {
        QTableWidgetItem *headerItem = ui.tableWidget->verticalHeaderItem(i);
        if (!headerItem) {
            headerItem = new QTableWidgetItem;
            ui.tableWidget->setVerticalHeaderItem(i, headerItem);
        }
        headerItem->setText(ui.rowsListWidget->item(i)->text());
    }
    QListWidgetItem *currentColumn = ui.columnsListWidget->currentItem();
    QListWidgetItem *currentRow = ui.rowsListWidget->currentItem();

    bool itemsEnabled = false;
    bool currentItemEnabled = false;

    bool currentColumnEnabled = false;
    bool moveColumnUpEnabled = false;
    bool moveColumnDownEnabled = false;

    bool currentRowEnabled = false;
    bool moveRowUpEnabled = false;
    bool moveRowDownEnabled = false;

    if (currentColumn) {
        currentColumnEnabled = true;
        const int idx = ui.columnsListWidget->currentRow();
        if (idx > 0)
            moveColumnUpEnabled = true;
        if (idx < ui.columnsListWidget->count() - 1)
            moveColumnDownEnabled = true;
    }

    if (currentRow) {
        currentRowEnabled = true;
        const int idx = ui.rowsListWidget->currentRow();
        if (idx > 0)
            moveRowUpEnabled = true;
        if (idx < ui.rowsListWidget->count() - 1)
            moveRowDownEnabled = true;
    }

    if (currentColumn && currentRow)
        currentItemEnabled = true;
    if (currentColumn || currentRow)
        itemsEnabled = true;

    ui.itemsBox->setEnabled(itemsEnabled);
    ui.itemIconSelector->setEnabled(currentItemEnabled);

    ui.deleteColumnButton->setEnabled(currentColumnEnabled);
    ui.columnIconSelector->setEnabled(currentColumnEnabled);
    ui.moveColumnUpButton->setEnabled(moveColumnUpEnabled);
    ui.moveColumnDownButton->setEnabled(moveColumnDownEnabled);

    ui.deleteRowButton->setEnabled(currentRowEnabled);
    ui.rowIconSelector->setEnabled(currentRowEnabled);
    ui.moveRowUpButton->setEnabled(moveRowUpEnabled);
    ui.moveRowDownButton->setEnabled(moveRowDownEnabled);

    QString itemText;
    PropertySheetIconValue itemIcon, columnIcon, rowIcon;

    if (currentColumn) {
        const int col = ui.columnsListWidget->currentRow();
        if (ui.tableWidget->horizontalHeaderItem(col))
            columnIcon = qVariantValue<PropertySheetIconValue>(ui.tableWidget->horizontalHeaderItem(col)->data(QAbstractFormBuilder::resourceRole()));
    }

    if (currentRow) {
        const int row = ui.rowsListWidget->currentRow();
        if (ui.tableWidget->verticalHeaderItem(row))
            rowIcon = qVariantValue<PropertySheetIconValue>(ui.tableWidget->verticalHeaderItem(row)->data(QAbstractFormBuilder::resourceRole()));
    }

    if (currentColumn && currentRow) {
        QTableWidgetItem *current = ui.tableWidget->item(ui.rowsListWidget->currentRow(),
                    ui.columnsListWidget->currentRow());
        if (current) {
            itemText = current->text();
            itemIcon = qVariantValue<PropertySheetIconValue>(current->data(QAbstractFormBuilder::resourceRole()));
        }
    }

    ui.columnIconSelector->setIcon(columnIcon);
    ui.rowIconSelector->setIcon(rowIcon);
    ui.itemIconSelector->setIcon(itemIcon);

    QMetaObject::invokeMethod(ui.tableWidget, "updateGeometries");
    ui.tableWidget->viewport()->update();
}

void TableWidgetEditor::on_itemIconSelector_iconChanged(const PropertySheetIconValue &icon)
{
    QListWidgetItem *currentColumn = ui.columnsListWidget->currentItem();
    QListWidgetItem *currentRow = ui.rowsListWidget->currentItem();
    if (!currentColumn || !currentRow)
        return;

    const int row = ui.rowsListWidget->currentRow();
    const int col = ui.columnsListWidget->currentRow();
    QTableWidgetItem *curItem = ui.tableWidget->item(row, col);
    if (!curItem)
        curItem = new QTableWidgetItem;

    curItem->setData(QAbstractFormBuilder::resourceRole(), qVariantFromValue(icon));
    curItem->setIcon(m_form->iconCache()->icon(icon));
    ui.tableWidget->setItem(row, col, curItem);
}

void TableWidgetEditor::moveColumnsLeft(int fromColumn, int toColumn)
{
    if (fromColumn >= toColumn)
        return;

    QTableWidgetItem *lastItem = ui.tableWidget->takeHorizontalHeaderItem(toColumn);
    for (int i = toColumn; i > fromColumn; i--) {
        ui.tableWidget->setHorizontalHeaderItem(i,
                    ui.tableWidget->takeHorizontalHeaderItem(i - 1));
    }
    ui.tableWidget->setHorizontalHeaderItem(fromColumn, lastItem);

    for (int i = 0; i < ui.tableWidget->rowCount(); i++) {
        QTableWidgetItem *lastItem = ui.tableWidget->takeItem(i, toColumn);
        for (int j = toColumn; j > fromColumn; j--)
            ui.tableWidget->setItem(i, j, ui.tableWidget->takeItem(i, j - 1));
        ui.tableWidget->setItem(i, fromColumn, lastItem);
    }
}

void TableWidgetEditor::moveColumnsRight(int fromColumn, int toColumn)
{
    if (fromColumn >= toColumn)
        return;

    QTableWidgetItem *lastItem = ui.tableWidget->takeHorizontalHeaderItem(fromColumn);
    for (int i = fromColumn; i < toColumn; i++) {
        ui.tableWidget->setHorizontalHeaderItem(i,
                    ui.tableWidget->takeHorizontalHeaderItem(i + 1));
    }
    ui.tableWidget->setHorizontalHeaderItem(toColumn, lastItem);

    for (int i = 0; i < ui.tableWidget->rowCount(); i++) {
        QTableWidgetItem *lastItem = ui.tableWidget->takeItem(i, fromColumn);
        for (int j = fromColumn; j < toColumn; j++)
            ui.tableWidget->setItem(i, j, ui.tableWidget->takeItem(i, j + 1));
        ui.tableWidget->setItem(i, toColumn, lastItem);
    }
}

void TableWidgetEditor::moveRowsDown(int fromRow, int toRow)
{
    if (fromRow >= toRow)
        return;

    QTableWidgetItem *lastItem = ui.tableWidget->takeVerticalHeaderItem(toRow);
    for (int i = toRow; i > fromRow; i--) {
        ui.tableWidget->setVerticalHeaderItem(i,
                    ui.tableWidget->takeVerticalHeaderItem(i - 1));
    }
    ui.tableWidget->setVerticalHeaderItem(fromRow, lastItem);

    for (int i = 0; i < ui.tableWidget->columnCount(); i++) {
        QTableWidgetItem *lastItem = ui.tableWidget->takeItem(toRow, i);
        for (int j = toRow; j > fromRow; j--)
            ui.tableWidget->setItem(j, i, ui.tableWidget->takeItem(j - 1, i));
        ui.tableWidget->setItem(fromRow, i, lastItem);
    }
}

void TableWidgetEditor::moveRowsUp(int fromRow, int toRow)
{
    if (fromRow >= toRow)
        return;

    QTableWidgetItem *lastItem = ui.tableWidget->takeVerticalHeaderItem(fromRow);
    for (int i = fromRow; i < toRow; i++) {
        ui.tableWidget->setVerticalHeaderItem(i,
                    ui.tableWidget->takeVerticalHeaderItem(i + 1));
    }
    ui.tableWidget->setVerticalHeaderItem(toRow, lastItem);

    for (int i = 0; i < ui.tableWidget->columnCount(); i++) {
        QTableWidgetItem *lastItem = ui.tableWidget->takeItem(fromRow, i);
        for (int j = fromRow; j < toRow; j++)
            ui.tableWidget->setItem(j, i, ui.tableWidget->takeItem(j + 1, i));
        ui.tableWidget->setItem(toRow, i, lastItem);
    }
}

void TableWidgetEditor::on_newColumnButton_clicked()
{
    m_updating = true;
    QListWidgetItem *currentColumn = ui.columnsListWidget->currentItem();
    int idx = ui.columnsListWidget->count();
    if (currentColumn)
        idx = ui.columnsListWidget->currentRow() + 1;

    const QString newColumnString = tr("New Column");

    const int columnCount = ui.tableWidget->columnCount();
    ui.tableWidget->setColumnCount(columnCount + 1);

    QTableWidgetItem *headerItem = ui.tableWidget->horizontalHeaderItem(columnCount);
    if (!headerItem)
        headerItem = new QTableWidgetItem;
    headerItem->setText(newColumnString);
    ui.tableWidget->setHorizontalHeaderItem(columnCount, headerItem);

    moveColumnsLeft(idx, columnCount);

    QListWidgetItem *currentRow = ui.rowsListWidget->currentItem();
    if (currentRow) {
        int row = ui.rowsListWidget->currentRow();
        ui.tableWidget->setCurrentCell(row, idx);
    }

    QListWidgetItem *item = new QListWidgetItem();
    item->setFlags(item->flags() | Qt::ItemIsEditable);
    item->setText(newColumnString);
    ui.columnsListWidget->insertItem(idx, item);
    ui.columnsListWidget->setCurrentItem(item);

    m_updating = false;
    updateEditor();

    ui.columnsListWidget->editItem(item);
}

void TableWidgetEditor::on_deleteColumnButton_clicked()
{
    QListWidgetItem *currentColumn = ui.columnsListWidget->currentItem();
    if (!currentColumn)
        return;

    m_updating = true;

    int idx = ui.columnsListWidget->currentRow();
    const int columnCount = ui.tableWidget->columnCount();

    moveColumnsRight(idx, columnCount - 1);
    ui.tableWidget->setColumnCount(columnCount - 1);

    delete currentColumn;
    if (idx == columnCount - 1)
        idx--;
    if (idx >= 0)
        ui.columnsListWidget->setCurrentRow(idx);

    m_updating = false;
    updateEditor();
}

void TableWidgetEditor::on_moveColumnUpButton_clicked()
{
    QListWidgetItem *currentColumn = ui.columnsListWidget->currentItem();
    if (!currentColumn)
        return;

    const int idx = ui.columnsListWidget->currentRow();

    if (idx == 0)
        return;

    m_updating = true;

    moveColumnsRight(idx - 1, idx);
    ui.columnsListWidget->takeItem(idx);
    ui.columnsListWidget->insertItem(idx - 1, currentColumn);
    ui.columnsListWidget->setCurrentItem(currentColumn);

    QListWidgetItem *currentRow = ui.rowsListWidget->currentItem();
    if (currentRow) {
        int row = ui.rowsListWidget->currentRow();
        ui.tableWidget->setCurrentCell(row, idx - 1);
    }

    m_updating = false;
    updateEditor();
}

void TableWidgetEditor::on_moveColumnDownButton_clicked()
{
    QListWidgetItem *currentColumn = ui.columnsListWidget->currentItem();
    if (!currentColumn)
        return;

    const int idx = ui.columnsListWidget->currentRow();
    const int columnCount = ui.tableWidget->columnCount();

    if (idx == columnCount - 1)
        return;

    m_updating = true;

    moveColumnsLeft(idx, idx + 1);
    ui.columnsListWidget->takeItem(idx);
    ui.columnsListWidget->insertItem(idx + 1, currentColumn);
    ui.columnsListWidget->setCurrentItem(currentColumn);

    QListWidgetItem *currentRow = ui.rowsListWidget->currentItem();
    if (currentRow) {
        const int row = ui.rowsListWidget->currentRow();
        ui.tableWidget->setCurrentCell(row, idx + 1);
    }

    m_updating = false;
    updateEditor();
}

void TableWidgetEditor::on_columnIconSelector_iconChanged(const PropertySheetIconValue &icon)
{
    QListWidgetItem *curItem = ui.columnsListWidget->currentItem();
    if (!curItem)
        return;

    m_updating = true;
    const QVariant data = qVariantFromValue(icon);
    const QIcon itemIcon = m_form->iconCache()->icon(icon);
    curItem->setData(QAbstractFormBuilder::resourceRole(), data);
    curItem->setIcon(itemIcon);
    const int col = ui.columnsListWidget->currentRow();
    QTableWidgetItem *headerItem = ui.tableWidget->horizontalHeaderItem(col);
    if (headerItem) {
        headerItem->setData(QAbstractFormBuilder::resourceRole(), data);
        headerItem->setIcon(itemIcon);
    }
    m_updating = false;
    updateEditor();
}

void TableWidgetEditor::on_newRowButton_clicked()
{
    m_updating = true;

    QListWidgetItem *currentRow = ui.rowsListWidget->currentItem();
    int idx = ui.rowsListWidget->count();
    if (currentRow)
        idx = ui.rowsListWidget->currentRow() + 1;

    QString newRowString = tr("New Row");

    const int rowCount = ui.tableWidget->rowCount();
    ui.tableWidget->setRowCount(rowCount + 1);

    QTableWidgetItem *headerItem = ui.tableWidget->verticalHeaderItem(rowCount);
    if (!headerItem)
        headerItem = new QTableWidgetItem;
    headerItem->setText(newRowString);
    ui.tableWidget->setVerticalHeaderItem(rowCount, headerItem);

    moveRowsDown(idx, rowCount);

    QListWidgetItem *currentCol = ui.columnsListWidget->currentItem();
    if (currentCol) {
        const int col = ui.columnsListWidget->currentRow();
        ui.tableWidget->setCurrentCell(idx, col);
    }

    QListWidgetItem *item = new QListWidgetItem();
    item->setFlags(item->flags() | Qt::ItemIsEditable);
    item->setText(newRowString);
    ui.rowsListWidget->insertItem(idx, item);
    ui.rowsListWidget->setCurrentItem(item);

    m_updating = false;
    updateEditor();

    ui.rowsListWidget->editItem(item);
}

void TableWidgetEditor::on_deleteRowButton_clicked()
{
    m_updating = true;

    QListWidgetItem *currentRow = ui.rowsListWidget->currentItem();
    if (!currentRow)
        return;

    int idx = ui.rowsListWidget->currentRow();
    const int rowCount = ui.tableWidget->rowCount();

    moveRowsUp(idx, rowCount - 1);
    ui.tableWidget->setRowCount(rowCount - 1);

    delete currentRow;
    if (idx == rowCount - 1)
        idx--;
    if (idx >= 0)
        ui.rowsListWidget->setCurrentRow(idx);

    m_updating = false;
    updateEditor();
}

void TableWidgetEditor::on_moveRowUpButton_clicked()
{
    QListWidgetItem *currentRow = ui.rowsListWidget->currentItem();
    if (!currentRow)
        return;

    const int idx = ui.rowsListWidget->currentRow();

    if (idx == 0)
        return;

    m_updating = true;

    moveRowsUp(idx - 1, idx);
    ui.rowsListWidget->takeItem(idx);
    ui.rowsListWidget->insertItem(idx - 1, currentRow);
    ui.rowsListWidget->setCurrentItem(currentRow);

    QListWidgetItem *currentColumn = ui.columnsListWidget->currentItem();
    if (currentColumn) {
        int col = ui.columnsListWidget->currentRow();
        ui.tableWidget->setCurrentCell(idx - 1, col);
    }

    m_updating = false;
    updateEditor();
}

void TableWidgetEditor::on_moveRowDownButton_clicked()
{
    QListWidgetItem *currentRow = ui.rowsListWidget->currentItem();
    if (!currentRow)
        return;

    const int idx = ui.rowsListWidget->currentRow();
    const int rowCount = ui.tableWidget->rowCount();

    if (idx == rowCount - 1)
        return;

    m_updating = true;

    moveRowsDown(idx, idx + 1);
    ui.rowsListWidget->takeItem(idx);
    ui.rowsListWidget->insertItem(idx + 1, currentRow);
    ui.rowsListWidget->setCurrentItem(currentRow);

    QListWidgetItem *currentColumn = ui.columnsListWidget->currentItem();
    if (currentColumn) {
        int col = ui.columnsListWidget->currentRow();
        ui.tableWidget->setCurrentCell(idx + 1, col);
    }

    m_updating = false;
    updateEditor();
}

void TableWidgetEditor::on_rowIconSelector_iconChanged(const PropertySheetIconValue &icon)
{
    QListWidgetItem *curItem = ui.rowsListWidget->currentItem();
    if (!curItem)
        return;

    m_updating = true;
    const QVariant data = qVariantFromValue(icon);
    const QIcon itemIcon = m_form->iconCache()->icon(icon);
    curItem->setData(QAbstractFormBuilder::resourceRole(), data);
    curItem->setIcon(itemIcon);
    const int row = ui.rowsListWidget->currentRow();
    QTableWidgetItem *headerItem = ui.tableWidget->verticalHeaderItem(row);
    if (headerItem) {
        headerItem->setData(QAbstractFormBuilder::resourceRole(), data);
        headerItem->setIcon(itemIcon);
    }
    m_updating = false;
    updateEditor();
}

void TableWidgetEditor::cacheReloaded()
{
    reloadIconResources(m_form->iconCache(), ui.tableWidget);
    reloadIconResources(m_form->iconCache(), ui.rowsListWidget);
    reloadIconResources(m_form->iconCache(), ui.columnsListWidget);
}

QT_END_NAMESPACE
