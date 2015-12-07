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
TRANSLATOR qdesigner_internal::TableWidgetEditor
*/

#include "tablewidgeteditor.h"
#include <abstractformbuilder.h>
#include <iconloader_p.h>
#include <qdesigner_command_p.h>
#include "formwindowbase_p.h"
#include "qdesigner_utils_p.h"
#include <designerpropertymanager.h>
#include <qttreepropertybrowser.h>

#include <QtDesigner/QDesignerFormWindowInterface>
#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QDesignerIconCacheInterface>
#include <QtCore/QDir>
#include <QtCore/QQueue>
#include <QtCore/QTextStream>

QT_BEGIN_NAMESPACE

using namespace qdesigner_internal;

TableWidgetEditor::TableWidgetEditor(QDesignerFormWindowInterface *form, QWidget *parent)
    : AbstractItemEditor(form, parent), m_updatingBrowser(false)
{
    m_columnEditor = new ItemListEditor(form, this);
    m_columnEditor->setObjectName(QLatin1String("columnEditor"));
    m_columnEditor->setNewItemText(tr("New Column"));
    m_rowEditor = new ItemListEditor(form, this);
    m_rowEditor->setObjectName(QLatin1String("rowEditor"));
    m_rowEditor->setNewItemText(tr("New Row"));
    ui.setupUi(this);

    injectPropertyBrowser(ui.itemsTab, ui.widget);
    connect(ui.showPropertiesButton, SIGNAL(clicked()),
            this, SLOT(togglePropertyBrowser()));
    togglePropertyBrowser();

    ui.tabWidget->insertTab(0, m_columnEditor, tr("&Columns"));
    ui.tabWidget->insertTab(1, m_rowEditor, tr("&Rows"));
    ui.tabWidget->setCurrentIndex(0);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    ui.tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);

    connect(iconCache(), SIGNAL(reloaded()), this, SLOT(cacheReloaded()));
}

static AbstractItemEditor::PropertyDefinition tableHeaderPropList[] = {
    { Qt::DisplayPropertyRole, 0, DesignerPropertyManager::designerStringTypeId, "text" },
    { Qt::DecorationPropertyRole, 0, DesignerPropertyManager::designerIconTypeId, "icon" },
    { Qt::ToolTipPropertyRole, 0, DesignerPropertyManager::designerStringTypeId, "toolTip" },
//    { Qt::StatusTipPropertyRole, 0, DesignerPropertyManager::designerStringTypeId, "statusTip" },
    { Qt::WhatsThisPropertyRole, 0, DesignerPropertyManager::designerStringTypeId, "whatsThis" },
    { Qt::FontRole, QVariant::Font, 0, "font" },
    { Qt::TextAlignmentRole, 0, DesignerPropertyManager::designerAlignmentTypeId, "textAlignment" },
    { Qt::BackgroundRole, QVariant::Color, 0, "background" },
    { Qt::ForegroundRole, QVariant::Brush, 0, "foreground" },
    { 0, 0, 0, 0 }
};

static AbstractItemEditor::PropertyDefinition tableItemPropList[] = {
    { Qt::DisplayPropertyRole, 0, DesignerPropertyManager::designerStringTypeId, "text" },
    { Qt::DecorationPropertyRole, 0, DesignerPropertyManager::designerIconTypeId, "icon" },
    { Qt::ToolTipPropertyRole, 0, DesignerPropertyManager::designerStringTypeId, "toolTip" },
//    { Qt::StatusTipPropertyRole, 0, DesignerPropertyManager::designerStringTypeId, "statusTip" },
    { Qt::WhatsThisPropertyRole, 0, DesignerPropertyManager::designerStringTypeId, "whatsThis" },
    { Qt::FontRole, QVariant::Font, 0, "font" },
    { Qt::TextAlignmentRole, 0, DesignerPropertyManager::designerAlignmentTypeId, "textAlignment" },
    { Qt::BackgroundRole, QVariant::Brush, 0, "background" },
    { Qt::ForegroundRole, QVariant::Brush, 0, "foreground" },
    { ItemFlagsShadowRole, 0, QtVariantPropertyManager::flagTypeId, "flags" },
    { Qt::CheckStateRole, 0, QtVariantPropertyManager::enumTypeId, "checkState" },
    { 0, 0, 0, 0 }
};

TableWidgetContents TableWidgetEditor::fillContentsFromTableWidget(QTableWidget *tableWidget)
{
    TableWidgetContents tblCont;
    tblCont.fromTableWidget(tableWidget, false);
    tblCont.applyToTableWidget(ui.tableWidget, iconCache(), true);

    tblCont.m_verticalHeader.applyToListWidget(m_rowEditor->listWidget(), iconCache(), true);
    m_rowEditor->setupEditor(tableWidget, tableHeaderPropList);

    tblCont.m_horizontalHeader.applyToListWidget(m_columnEditor->listWidget(), iconCache(), true);
    m_columnEditor->setupEditor(tableWidget, tableHeaderPropList);

    setupEditor(tableWidget, tableItemPropList);
    if (ui.tableWidget->columnCount() > 0 && ui.tableWidget->rowCount() > 0)
        ui.tableWidget->setCurrentCell(0, 0);

    updateEditor();

    return tblCont;
}

TableWidgetContents TableWidgetEditor::contents() const
{
    TableWidgetContents retVal;
    retVal.fromTableWidget(ui.tableWidget, true);
    return retVal;
}

void TableWidgetEditor::setItemData(int role, const QVariant &v)
{
    QTableWidgetItem *item = ui.tableWidget->currentItem();
    BoolBlocker block(m_updatingBrowser);
    if (!item) {
        item = new QTableWidgetItem;
        ui.tableWidget->setItem(ui.tableWidget->currentRow(), ui.tableWidget->currentColumn(), item);
    }
    QVariant newValue = v;
    if (role == Qt::FontRole && newValue.type() == QVariant::Font) {
        QFont oldFont = ui.tableWidget->font();
        QFont newFont = qVariantValue<QFont>(newValue).resolve(oldFont);
        newValue = qVariantFromValue(newFont);
        item->setData(role, QVariant()); // force the right font with the current resolve mask is set (item view bug)
    }
    item->setData(role, newValue);
}

QVariant TableWidgetEditor::getItemData(int role) const
{
    QTableWidgetItem *item = ui.tableWidget->currentItem();
    if (!item)
        return QVariant();
    return item->data(role);
}

void TableWidgetEditor::on_tableWidget_currentCellChanged(int currentRow, int currentCol, int, int /* XXX remove me */)
{
    m_rowEditor->setCurrentIndex(currentRow);
    m_columnEditor->setCurrentIndex(currentCol);
    updateBrowser();
}

void TableWidgetEditor::on_tableWidget_itemChanged(QTableWidgetItem *item)
{
    if (m_updatingBrowser)
        return;

    PropertySheetStringValue val = qVariantValue<PropertySheetStringValue>(item->data(Qt::DisplayPropertyRole));
    val.setValue(item->text());
    BoolBlocker block(m_updatingBrowser);
    item->setData(Qt::DisplayPropertyRole, qVariantFromValue(val));

    updateBrowser();
}

void TableWidgetEditor::on_columnEditor_indexChanged(int col)
{
    ui.tableWidget->setCurrentCell(ui.tableWidget->currentRow(), col);
}

void TableWidgetEditor::on_columnEditor_itemChanged(int idx, int role, const QVariant &v)
{
    ui.tableWidget->horizontalHeaderItem(idx)->setData(role, v);
}

void TableWidgetEditor::on_rowEditor_indexChanged(int col)
{
    ui.tableWidget->setCurrentCell(col, ui.tableWidget->currentColumn());
}

void TableWidgetEditor::on_rowEditor_itemChanged(int idx, int role, const QVariant &v)
{
    ui.tableWidget->verticalHeaderItem(idx)->setData(role, v);
}

void TableWidgetEditor::togglePropertyBrowser()
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

void TableWidgetEditor::updateEditor()
{
    const bool wasEnabled = ui.tabWidget->isTabEnabled(2);
    const bool isEnabled = ui.tableWidget->columnCount() && ui.tableWidget->rowCount();
    ui.tabWidget->setTabEnabled(2, isEnabled);
    if (!wasEnabled && isEnabled)
        ui.tableWidget->setCurrentCell(0, 0);

    QMetaObject::invokeMethod(ui.tableWidget, "updateGeometries");
    ui.tableWidget->viewport()->update();
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

void TableWidgetEditor::on_columnEditor_itemInserted(int idx)
{
    const int columnCount = ui.tableWidget->columnCount();
    ui.tableWidget->setColumnCount(columnCount + 1);

    QTableWidgetItem *newItem = new QTableWidgetItem(m_columnEditor->newItemText());
    newItem->setData(Qt::DisplayPropertyRole, qVariantFromValue(PropertySheetStringValue(m_columnEditor->newItemText())));
    ui.tableWidget->setHorizontalHeaderItem(columnCount, newItem);

    moveColumnsLeft(idx, columnCount);

    int row = ui.tableWidget->currentRow();
    if (row >= 0)
        ui.tableWidget->setCurrentCell(row, idx);

    updateEditor();
}

void TableWidgetEditor::on_columnEditor_itemDeleted(int idx)
{
    const int columnCount = ui.tableWidget->columnCount();

    moveColumnsRight(idx, columnCount - 1);
    ui.tableWidget->setColumnCount(columnCount - 1);

    updateEditor();
}

void TableWidgetEditor::on_columnEditor_itemMovedUp(int idx)
{
    moveColumnsRight(idx - 1, idx);

    ui.tableWidget->setCurrentCell(ui.tableWidget->currentRow(), idx - 1);
}

void TableWidgetEditor::on_columnEditor_itemMovedDown(int idx)
{
    moveColumnsLeft(idx, idx + 1);

    ui.tableWidget->setCurrentCell(ui.tableWidget->currentRow(), idx + 1);
}

void TableWidgetEditor::on_rowEditor_itemInserted(int idx)
{
    const int rowCount = ui.tableWidget->rowCount();
    ui.tableWidget->setRowCount(rowCount + 1);

    QTableWidgetItem *newItem = new QTableWidgetItem(m_rowEditor->newItemText());
    newItem->setData(Qt::DisplayPropertyRole, qVariantFromValue(PropertySheetStringValue(m_rowEditor->newItemText())));
    ui.tableWidget->setVerticalHeaderItem(rowCount, newItem);

    moveRowsDown(idx, rowCount);

    int col = ui.tableWidget->currentColumn();
    if (col >= 0)
        ui.tableWidget->setCurrentCell(idx, col);

    updateEditor();
}

void TableWidgetEditor::on_rowEditor_itemDeleted(int idx)
{
    const int rowCount = ui.tableWidget->rowCount();

    moveRowsUp(idx, rowCount - 1);
    ui.tableWidget->setRowCount(rowCount - 1);

    updateEditor();
}

void TableWidgetEditor::on_rowEditor_itemMovedUp(int idx)
{
    moveRowsUp(idx - 1, idx);

    ui.tableWidget->setCurrentCell(idx - 1, ui.tableWidget->currentColumn());
}

void TableWidgetEditor::on_rowEditor_itemMovedDown(int idx)
{
    moveRowsDown(idx, idx + 1);

    ui.tableWidget->setCurrentCell(idx + 1, ui.tableWidget->currentColumn());
}

void TableWidgetEditor::cacheReloaded()
{
    reloadIconResources(iconCache(), ui.tableWidget);
}

QT_END_NAMESPACE
