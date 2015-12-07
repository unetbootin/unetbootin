/****************************************************************************
**
** Copyright (C) 2004-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the demonstration applications of the Qt Toolkit.
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

#include "connectionwidget.h"

#include <QtGui>
#include <QtSql>

ConnectionWidget::ConnectionWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    tree = new QTreeWidget(this);
    tree->setObjectName(QLatin1String("tree"));
    tree->setHeaderLabels(QStringList(tr("database")));
    tree->header()->setResizeMode(QHeaderView::Stretch);
    QAction *refreshAction = new QAction(tr("Refresh"), tree);
    metaDataAction = new QAction(tr("Show Schema"), tree);
    connect(refreshAction, SIGNAL(triggered()), SLOT(refresh()));
    connect(metaDataAction, SIGNAL(triggered()), SLOT(showMetaData()));
    tree->addAction(refreshAction);
    tree->addAction(metaDataAction);
    tree->setContextMenuPolicy(Qt::ActionsContextMenu);

    layout->addWidget(tree);

    QMetaObject::connectSlotsByName(this);
}

ConnectionWidget::~ConnectionWidget()
{
}

static QString qDBCaption(const QSqlDatabase &db)
{
    QString nm = db.driverName();
    nm.append(QLatin1Char(':'));
    if (!db.userName().isEmpty())
        nm.append(db.userName()).append(QLatin1Char('@'));
    nm.append(db.databaseName());
    return nm;
}

void ConnectionWidget::refresh()
{
    tree->clear();
    QStringList connectionNames = QSqlDatabase::connectionNames();

    bool gotActiveDb = false;
    for (int i = 0; i < connectionNames.count(); ++i) {
        QTreeWidgetItem *root = new QTreeWidgetItem(tree);
        QSqlDatabase db = QSqlDatabase::database(connectionNames.at(i), false);
        root->setText(0, qDBCaption(db));
        if (connectionNames.at(i) == activeDb) {
            gotActiveDb = true;
            setActive(root);
        }
        if (db.isOpen()) {
            QStringList tables = db.tables();
            for (int t = 0; t < tables.count(); ++t) {
                QTreeWidgetItem *table = new QTreeWidgetItem(root);
                table->setText(0, tables.at(t));
            }
        }
    }
    if (!gotActiveDb) {
        activeDb = connectionNames.value(0);
        setActive(tree->topLevelItem(0));
    }

    tree->doItemsLayout(); // HACK
}

QSqlDatabase ConnectionWidget::currentDatabase() const
{
    return QSqlDatabase::database(activeDb);
}

static void qSetBold(QTreeWidgetItem *item, bool bold)
{
    QFont font = item->font(0);
    font.setBold(bold);
    item->setFont(0, font);
}

void ConnectionWidget::setActive(QTreeWidgetItem *item)
{
    for (int i = 0; i < tree->topLevelItemCount(); ++i) {
        if (tree->topLevelItem(i)->font(0).bold())
            qSetBold(tree->topLevelItem(i), false);
    }

    if (!item)
        return;

    qSetBold(item, true);
    activeDb = QSqlDatabase::connectionNames().value(tree->indexOfTopLevelItem(item));
}

void ConnectionWidget::on_tree_itemActivated(QTreeWidgetItem *item, int /* column */)
{

    if (!item)
        return;

    if (!item->parent()) {
        setActive(item);
    } else {
        setActive(item->parent());
        emit tableActivated(item->text(0));
    }
}

void ConnectionWidget::showMetaData()
{
    QTreeWidgetItem *cItem = tree->currentItem();
    if (!cItem || !cItem->parent())
        return;
    setActive(cItem->parent());
    emit metaDataRequested(cItem->text(0));
}

void ConnectionWidget::on_tree_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *)
{
    metaDataAction->setEnabled(current && current->parent());
}

