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

#ifndef TABLEWIDGETEDITOR_H
#define TABLEWIDGETEDITOR_H

#include "ui_tablewidgeteditor.h"

QT_BEGIN_NAMESPACE

class QTableWidget;
class QDesignerFormWindowInterface;

namespace qdesigner_internal {

class FormWindowBase;
class PropertySheetIconValue;

class TableWidgetEditor: public QDialog
{
    Q_OBJECT
public:
    TableWidgetEditor(QDesignerFormWindowInterface *form, QWidget *parent);
    virtual ~TableWidgetEditor();

    void fillContentsFromTableWidget(QTableWidget *tableWidget);

    void fillTableWidgetFromContents(QTableWidget *tableWidget);

private slots:

    void on_tableWidget_currentCellChanged(int currentRow, int currnetCol, int, int);
    void on_tableWidget_itemChanged(QTableWidgetItem *item);

    void on_itemIconSelector_iconChanged(const PropertySheetIconValue &icon);

    void on_columnsListWidget_currentRowChanged(int col);
    void on_columnsListWidget_itemChanged(QListWidgetItem *item);

    void on_newColumnButton_clicked();
    void on_deleteColumnButton_clicked();
    void on_moveColumnUpButton_clicked();
    void on_moveColumnDownButton_clicked();

    void on_columnIconSelector_iconChanged(const PropertySheetIconValue &icon);

    void on_rowsListWidget_currentRowChanged(int row);
    void on_rowsListWidget_itemChanged(QListWidgetItem *item);

    void on_newRowButton_clicked();
    void on_deleteRowButton_clicked();
    void on_moveRowUpButton_clicked();
    void on_moveRowDownButton_clicked();

    void on_rowIconSelector_iconChanged(const PropertySheetIconValue &icon);

    void cacheReloaded();
private:
    void copyContents(QTableWidget *sourceWidget, QTableWidget *destWidget);
    void updateEditor();
    void moveColumnsLeft(int fromColumn, int toColumn);
    void moveColumnsRight(int fromColumn, int toColumn);
    void moveRowsUp(int fromRow, int toRow);
    void moveRowsDown(int fromRow, int toRow);

    Ui::TableWidgetEditor ui;
    FormWindowBase *m_form;
    bool m_updating;
};

}  // namespace qdesigner_internal

QT_END_NAMESPACE

#endif // TABLEWIDGETEDITOR_H
