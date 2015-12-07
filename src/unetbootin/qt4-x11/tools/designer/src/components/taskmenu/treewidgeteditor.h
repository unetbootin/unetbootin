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

#ifndef TREEWIDGETEDITOR_H
#define TREEWIDGETEDITOR_H

#include "ui_treewidgeteditor.h"

#include "listwidgeteditor.h"

QT_BEGIN_NAMESPACE

class QTreeWidget;
class QDesignerFormWindowInterface;

namespace qdesigner_internal {

class FormWindowBase;
class PropertySheetIconValue;

class TreeWidgetEditor: public AbstractItemEditor
{
    Q_OBJECT
public:
    TreeWidgetEditor(QDesignerFormWindowInterface *form, QWidget *parent);

    TreeWidgetContents fillContentsFromTreeWidget(QTreeWidget *treeWidget);
    TreeWidgetContents contents() const;

private slots:
    void on_newItemButton_clicked();
    void on_newSubItemButton_clicked();
    void on_deleteItemButton_clicked();
    void on_moveItemUpButton_clicked();
    void on_moveItemDownButton_clicked();
    void on_moveItemRightButton_clicked();
    void on_moveItemLeftButton_clicked();

    void on_treeWidget_currentItemChanged();
    void on_treeWidget_itemChanged(QTreeWidgetItem *item, int column);

    void on_columnEditor_indexChanged(int idx);
    void on_columnEditor_itemChanged(int idx, int role, const QVariant &v);

    void on_columnEditor_itemInserted(int idx);
    void on_columnEditor_itemDeleted(int idx);
    void on_columnEditor_itemMovedUp(int idx);
    void on_columnEditor_itemMovedDown(int idx);

    void togglePropertyBrowser();
    void cacheReloaded();

protected:
    virtual void setItemData(int role, const QVariant &v);
    virtual QVariant getItemData(int role) const;

private:
    QtVariantProperty *setupPropertyGroup(const QString &title, PropertyDefinition *propDefs);
    void updateEditor();
    void moveColumnItems(const PropertyDefinition *propList, QTreeWidgetItem *item, int fromColumn, int toColumn, int step);
    void moveColumns(int fromColumn, int toColumn, int step);
    void moveColumnsLeft(int fromColumn, int toColumn);
    void moveColumnsRight(int fromColumn, int toColumn);
    void closeEditors();

    Ui::TreeWidgetEditor ui;
    ItemListEditor *m_columnEditor;
    bool m_updatingBrowser;
};

}  // namespace qdesigner_internal

QT_END_NAMESPACE

#endif // TREEWIDGETEDITOR_H
