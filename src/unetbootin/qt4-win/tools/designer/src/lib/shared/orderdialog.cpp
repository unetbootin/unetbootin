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
TRANSLATOR qdesigner_internal::OrderDialog
*/

#include "orderdialog_p.h"
#include "iconloader_p.h"
#include "ui_orderdialog.h"

#include <QtDesigner/QExtensionManager>
#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QDesignerContainerExtension>
#include <QtCore/QAbstractItemModel>
#include <QtCore/QModelIndex>
#include <QtGui/QPushButton>

QT_BEGIN_NAMESPACE

// OrderDialog: Used to reorder the pages of QStackedWidget and QToolBox.
// Provides up and down buttons as well as  DnD via QAbstractItemView::InternalMove mode
namespace qdesigner_internal {

OrderDialog::OrderDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::OrderDialog),
    m_format(PageOrderFormat)
{
    m_ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    m_ui->upButton->setIcon(createIconSet(QString::fromUtf8("up.png")));
    m_ui->downButton->setIcon(createIconSet(QString::fromUtf8("down.png")));
    m_ui->buttonBox->button(QDialogButtonBox::Ok)->setDefault(true);
    connect(m_ui->buttonBox->button(QDialogButtonBox::Reset), SIGNAL(clicked()), this, SLOT(slotReset()));
    // Catch the remove operation of a DnD operation in QAbstractItemView::InternalMove mode to enable buttons
    // Selection mode is 'contiguous' to enable DnD of groups
    connect(m_ui->pageList->model(), SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(slotEnableButtonsAfterDnD()));

    m_ui->upButton->setEnabled(false);
    m_ui->downButton->setEnabled(false);
}

OrderDialog::~OrderDialog()
{
    delete  m_ui;
}

void OrderDialog::setDescription(const QString &d)
{
     m_ui->groupBox->setTitle(d);
}

void OrderDialog::setPageList(const QWidgetList &pages)
{
    // The QWidget* are stored in a map indexed by the old index.
    // The old index is set as user data on the item instead of the QWidget*
    // because DnD is enabled which requires the user data to serializable
    m_orderMap.clear();
    const int count = pages.count();
    for (int i=0; i < count; ++i)
        m_orderMap.insert(i, pages.at(i));
    buildList();
}

void OrderDialog::buildList()
{
    m_ui->pageList->clear();
    const OrderMap::const_iterator cend = m_orderMap.constEnd();
    for (OrderMap::const_iterator it = m_orderMap.constBegin(); it != cend; ++it) {
        QListWidgetItem *item = new QListWidgetItem();
        const int index = it.key();
        switch (m_format) {
        case PageOrderFormat:
            item->setText(tr("Index %1 (%2)").arg(index).arg(it.value()->objectName()));
            break;
        case TabOrderFormat:
            item->setText(tr("%1 %2").arg(index+1).arg(it.value()->objectName()));
            break;
        }
        item->setData(Qt::UserRole, QVariant(index));
        m_ui->pageList->addItem(item);
    }

    if (m_ui->pageList->count() > 0)
        m_ui->pageList->setCurrentRow(0);
}

void OrderDialog::slotReset()
{
    buildList();
}

QWidgetList OrderDialog::pageList() const
{
    QWidgetList rc;
    const int count = m_ui->pageList->count();
    for (int i=0; i < count; ++i) {
        const int oldIndex = m_ui->pageList->item(i)->data(Qt::UserRole).toInt();
        rc.append(m_orderMap.value(oldIndex));
    }
    return rc;
}

void OrderDialog::on_upButton_clicked()
{
    const int row = m_ui->pageList->currentRow();
    if (row <= 0)
        return;

    m_ui->pageList->insertItem(row - 1, m_ui->pageList->takeItem(row));
    m_ui->pageList->setCurrentRow(row - 1);
}

void OrderDialog::on_downButton_clicked()
{
    const int row = m_ui->pageList->currentRow();
    if (row == -1 || row == m_ui->pageList->count() - 1)
        return;

    m_ui->pageList->insertItem(row + 1, m_ui->pageList->takeItem(row));
    m_ui->pageList->setCurrentRow(row + 1);
}

void OrderDialog::slotEnableButtonsAfterDnD()
{
    enableButtons(m_ui->pageList->currentRow());
}

void OrderDialog::on_pageList_currentRowChanged(int r)
{
    enableButtons(r);
}

void OrderDialog::enableButtons(int r)
{
    m_ui->upButton->setEnabled(r > 0);
    m_ui->downButton->setEnabled(r >= 0 && r < m_ui->pageList->count() - 1);
}

QWidgetList OrderDialog::pagesOfContainer(const QDesignerFormEditorInterface *core, QWidget *container)
{
    QWidgetList rc;
    if (QDesignerContainerExtension* ce = qt_extension<QDesignerContainerExtension*>(core->extensionManager(), container)) {
        const int count = ce->count();
        for (int i = 0; i < count ;i ++)
            rc.push_back(ce->widget(i));
    }
    return rc;
}

}

QT_END_NAMESPACE
