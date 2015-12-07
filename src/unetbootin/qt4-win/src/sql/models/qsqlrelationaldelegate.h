/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtSql module of the Qt Toolkit.
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

#ifndef QSQLRELATIONALDELEGATE_H
#define QSQLRELATIONALDELEGATE_H

#ifdef QT_GUI_LIB

#include <QtGui/qitemdelegate.h>
#include <QtGui/qlistview.h>
#include <QtGui/qcombobox.h>
#include <QtSql/qsqlrelationaltablemodel.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Sql)

class QSqlRelationalDelegate: public QItemDelegate
{
public:

explicit QSqlRelationalDelegate(QObject *parent = 0)
    : QItemDelegate(parent)
{}

~QSqlRelationalDelegate()
{}

QWidget *createEditor(QWidget *parent,
                      const QStyleOptionViewItem &option,
                      const QModelIndex &index) const
{
    const QSqlRelationalTableModel *sqlModel = qobject_cast<const QSqlRelationalTableModel *>(index.model());
    QSqlTableModel *childModel = sqlModel ? sqlModel->relationModel(index.column()) : 0;
    if (!childModel)
        return QItemDelegate::createEditor(parent, option, index);

    QComboBox *combo = new QComboBox(parent);
    combo->setModel(childModel);
    combo->setModelColumn(childModel->fieldIndex(sqlModel->relation(index.column()).displayColumn()));
    combo->installEventFilter(const_cast<QSqlRelationalDelegate *>(this));

    return combo;
}

void setEditorData(QWidget *editor, const QModelIndex &index) const
{
    const QSqlRelationalTableModel *sqlModel = qobject_cast<const QSqlRelationalTableModel *>(index.model());
    QComboBox *combo = qobject_cast<QComboBox *>(editor);
    if (!sqlModel || !combo) {
        QItemDelegate::setEditorData(editor, index);
        return;
    }
    combo->setCurrentIndex(combo->findText(sqlModel->data(index).toString()));
}

void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if (!index.isValid())
        return;

    QSqlRelationalTableModel *sqlModel = qobject_cast<QSqlRelationalTableModel *>(model);
    QSqlTableModel *childModel = sqlModel ? sqlModel->relationModel(index.column()) : 0;
    QComboBox *combo = qobject_cast<QComboBox *>(editor);
    if (!sqlModel || !childModel || !combo) {
        QItemDelegate::setModelData(editor, model, index);
        return;
    }

    int currentItem = combo->currentIndex();
    int childColIndex = childModel->fieldIndex(sqlModel->relation(index.column()).displayColumn());
    int childEditIndex = childModel->fieldIndex(sqlModel->relation(index.column()).indexColumn());
    sqlModel->setData(index,
            childModel->data(childModel->index(currentItem, childColIndex), Qt::DisplayRole),
            Qt::DisplayRole);
    sqlModel->setData(index,
            childModel->data(childModel->index(currentItem, childEditIndex), Qt::EditRole),
            Qt::EditRole);
}

};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QT_GUI_LIB

#endif // QSQLRELATIONALDELEGATE_H
