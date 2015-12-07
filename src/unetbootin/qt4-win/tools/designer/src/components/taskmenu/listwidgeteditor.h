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

#ifndef LISTWIDGETEDITOR_H
#define LISTWIDGETEDITOR_H

#include "ui_listwidgeteditor.h"

QT_BEGIN_NAMESPACE

class QListWidget;
class QComboBox;
class QDesignerFormWindowInterface;

namespace qdesigner_internal {

class FormWindowBase;
class PropertySheetIconValue;

class ListWidgetEditor: public QDialog
{
    Q_OBJECT
public:
    ListWidgetEditor(QDesignerFormWindowInterface *form, QWidget *parent);
    virtual ~ListWidgetEditor();

    void fillContentsFromListWidget(QListWidget *listWidget);
    void fillContentsFromComboBox(QComboBox *comboBox);

    int count() const;
    PropertySheetIconValue icon(int row) const;
    QString text(int row) const;

private slots:
    void on_newItemButton_clicked();
    void on_deleteItemButton_clicked();
    void on_moveItemUpButton_clicked();
    void on_moveItemDownButton_clicked();
    void on_listWidget_currentRowChanged(int currentRow);
    void on_listWidget_itemChanged(QListWidgetItem *item);
    void on_itemIconSelector_iconChanged(const PropertySheetIconValue &icon);
    void cacheReloaded();

private:
    void updateEditor();
    Ui::ListWidgetEditor ui;
    FormWindowBase *m_form;
};

}  // namespace qdesigner_internal

QT_END_NAMESPACE

#endif // LISTWIDGETEDITOR_H
