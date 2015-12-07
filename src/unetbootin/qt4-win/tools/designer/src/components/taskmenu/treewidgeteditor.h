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

#ifndef TREEWIDGETEDITOR_H
#define TREEWIDGETEDITOR_H

#include "ui_treewidgeteditor.h"

QT_BEGIN_NAMESPACE

class QTreeWidget;
class QDesignerFormWindowInterface;

namespace qdesigner_internal {

class FormWindowBase;
class PropertySheetIconValue;

class TreeWidgetEditor: public QDialog
{
    Q_OBJECT
public:
    TreeWidgetEditor(QDesignerFormWindowInterface *form, QWidget *parent);
    virtual ~TreeWidgetEditor();

    void fillContentsFromTreeWidget(QTreeWidget *treeWidget);

    void fillTreeWidgetFromContents(QTreeWidget *treeWidget);

private slots:
    void on_newItemButton_clicked();
    void on_newSubItemButton_clicked();
    void on_deleteItemButton_clicked();
    void on_moveItemUpButton_clicked();
    void on_moveItemDownButton_clicked();
    void on_moveItemRightButton_clicked();
    void on_moveItemLeftButton_clicked();

    void on_treeWidget_currentItemChanged(QTreeWidgetItem *current,
            QTreeWidgetItem *previous);
    void on_treeWidget_itemChanged(QTreeWidgetItem *current);

    void on_itemIconSelector_iconChanged(const PropertySheetIconValue &icon);

    void on_listWidget_currentRowChanged(int currentRow);
    void on_listWidget_itemChanged(QListWidgetItem *item);

    void on_newColumnButton_clicked();
    void on_deleteColumnButton_clicked();
    void on_moveColumnUpButton_clicked();
    void on_moveColumnDownButton_clicked();

    void on_columnIconSelector_iconChanged(const PropertySheetIconValue &icon);

    void cacheReloaded();
private:
    void copyContents(QTreeWidget *sourceWidget, QTreeWidget *destWidget);
    void updateEditor();
    void moveColumnsLeft(int fromColumn, int toColumn);
    void moveColumnsRight(int fromColumn, int toColumn);
    void closeEditors();

    Ui::TreeWidgetEditor ui;
    FormWindowBase *m_form;
    bool m_updating;
};

}  // namespace qdesigner_internal

QT_END_NAMESPACE

#endif // TREEWIDGETEDITOR_H
