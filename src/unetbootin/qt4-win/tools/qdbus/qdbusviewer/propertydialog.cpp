/****************************************************************************
**
** Copyright (C) 2004-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the tools applications of the Qt Toolkit.
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

#include "propertydialog.h"

#include <QHeaderView>
#include <QLayout>
#include <QDebug>

PropertyDialog::PropertyDialog(QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f)
{
    buttonBox = new QDialogButtonBox;
    propertyTable = new QTableWidget;
    label = new QLabel;

    buttonBox->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    propertyTable->setColumnCount(2);
    const QStringList labels = QStringList() << "Name" << "Value";
    propertyTable->setHorizontalHeaderLabels(labels);
    propertyTable->horizontalHeader()->setStretchLastSection(true);
    propertyTable->setEditTriggers(QAbstractItemView::AllEditTriggers);

    connect(buttonBox, SIGNAL(accepted()), SLOT(accept()), Qt::QueuedConnection);
    connect(buttonBox, SIGNAL(rejected()), SLOT(reject()), Qt::QueuedConnection);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(label);
    layout->addWidget(propertyTable);
    layout->addWidget(buttonBox);
}

void PropertyDialog::setInfo(const QString &caption)
{
    label->setText(caption);
}

void PropertyDialog::addProperty(const QString &aname, QVariant::Type type)
{
    int rowCount = propertyTable->rowCount();
    propertyTable->setRowCount(rowCount + 1);

    QString name = aname;
    if (name.isEmpty())
        name = "argument " + QString::number(rowCount + 1);
    QTableWidgetItem *nameItem = new QTableWidgetItem(name + " (" + QVariant::typeToName(type) + ")");
    nameItem->setFlags(nameItem->flags() &
            ~(Qt::ItemIsEditable | Qt::ItemIsSelectable));
    propertyTable->setItem(rowCount, 0, nameItem);

    QTableWidgetItem *valueItem = new QTableWidgetItem;
    valueItem->setData(Qt::DisplayRole, QVariant(type));
    propertyTable->setItem(rowCount, 1, valueItem);
}

int PropertyDialog::exec()
{
    propertyTable->resizeColumnToContents(0);
    propertyTable->setFocus();
    propertyTable->setCurrentCell(0, 1);
    return QDialog::exec();
}

QList<QVariant> PropertyDialog::values() const
{
    QList<QVariant> result;

    for (int i = 0; i < propertyTable->rowCount(); ++i)
        result << propertyTable->item(i, 1)->data(Qt::EditRole);

    return result;
}

