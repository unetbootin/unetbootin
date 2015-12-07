/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt Assistant of the Qt Toolkit.
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

#include <QtCore/QEvent>
#include <QtCore/QDebug>

#include <QtGui/QMenu>
#include <QtGui/QLayout>
#include <QtGui/QKeyEvent>
#include <QtGui/QMessageBox>
#include <QtGui/QHeaderView>
#include <QtGui/QTreeWidgetItem>
#include <QtGui/QFocusEvent>

#include "bookmarkwidget.h"
#include "mainwindow.h"
#include "centralwidget.h"

QT_BEGIN_NAMESPACE

namespace {
    QString uniqueFolderName(const QTreeWidget *treeWidget)
    {
        QString folderName = QObject::tr("New Folder");
        QList<QTreeWidgetItem *> list = treeWidget->findItems(folderName, 
            Qt::MatchContains | Qt::MatchRecursive, 0);
        if (!list.isEmpty()) {
            QStringList names;
            foreach (QTreeWidgetItem *item, list)
                names << item->text(0);

            for (int i = 1; i <= names.count(); ++i) {
                folderName = (QObject::tr("New Folder") + QLatin1String(" %1")).arg(i);
                if (!names.contains(folderName))
                    break;
            }
        }
        return folderName;
    }
}


BookmarkWidget::BookmarkWidget(QWidget *parent, bool custom)
    : QTreeWidget(parent)
    , m_custom(custom)
{
    if (m_custom) {
        setColumnCount(2);
        hideColumn(1);
        setHeaderLabel(tr("Bookmarks"));
    } else {
        header()->hide();
    }

    installEventFilter(this);
    setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this, SIGNAL(itemActivated(QTreeWidgetItem*, int)), 
        SLOT(itemActivated(QTreeWidgetItem*, int)));
    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), 
        this, SLOT(showContextMenu(const QPoint&)));
    connect(this, SIGNAL(itemChanged(QTreeWidgetItem*, int)), this,
        SLOT(itemChanged(QTreeWidgetItem*, int)));
    connect(this, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
        this, SLOT(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)));
}

BookmarkWidget::~BookmarkWidget()
{
    // nothing todo
}

QByteArray BookmarkWidget::bookmarks() const
{
    qint32 depth = 0;
    QByteArray bookmarks;
    QDataStream stream(&bookmarks, QIODevice::WriteOnly);

    for (int i = 0; i < topLevelItemCount(); ++i) {
        const QTreeWidgetItem *item = topLevelItem(i);
        stream << depth; // root
        stream << item->data(0, Qt::DisplayRole).toString();
        stream << item->data(1, Qt::DisplayRole).toString();
        stream << item->isExpanded();

        if (item->childCount() > 0) {
            readBookmarkFolderRecursive(item, stream, (depth +1));
        }
    }
    return bookmarks;
}

QStringList BookmarkWidget::bookmarkFolders() const
{
    QStringList folders(tr("Bookmarks"));

    QList<QTreeWidgetItem *> list = findItems(QLatin1String("Folder"), 
        Qt::MatchCaseSensitive | Qt::MatchRecursive, 1);

    foreach (QTreeWidgetItem *item, list)
        folders << item->data(0, Qt::DisplayRole).toString();

    return folders;
}

void BookmarkWidget::setBookmarks(const QByteArray &bookmarks)
{
    clear();

    qint32 depth;
    bool expanded;
    QString name, type;
    QList<int> lastDepths;
    QList<QTreeWidgetItem*> parents;

    QDataStream stream(bookmarks);
    while (!stream.atEnd()) {
        stream >> depth >> name >> type >> expanded;

        QTreeWidgetItem *item = 0;
        if (depth == 0) {
            parents.clear(); lastDepths.clear();
            item = new QTreeWidgetItem(this, QStringList(name) << type);
            parents << item; lastDepths << depth;
        } 
		else {
			while (depth <= lastDepths.last() && parents.count() > 0) {
				parents.pop_back();
				lastDepths.pop_back();
			}
			item = new QTreeWidgetItem(parents.last(), QStringList(name) << type);
			if (type == QLatin1String("Folder")) {
				parents << item; lastDepths << depth;
			}
		}

        if (m_custom)
            item->setFlags(item->flags() &~ Qt::ItemIsSelectable);

        if (type == QLatin1String("Folder")) {
            item->setExpanded(expanded);
            item->setIcon(0, QIcon(style()->standardPixmap(QStyle::SP_DirClosedIcon)));
        } else
            item->setHidden(m_custom);
    }
}

void BookmarkWidget::removeBookmarkOrFolder()
{
    int index;
    QTreeWidgetItem *currentItem = this->currentItem();
    if (currentItem) {
        QString data = currentItem->data(1, Qt::DisplayRole).toString();
        if (data == QLatin1String("Folder") && currentItem->childCount()) {
            int value = QMessageBox::question(this, tr("Remove"), 
                        tr("You are going to delete a Folder, this will also<br>"
                        "remove it's content. Are you sure to continue?"), 
                        QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Cancel);

            if (value == QMessageBox::Cancel)
                return;
        }

        QTreeWidgetItem *parent = currentItem->parent();
        if (parent) {
            index = parent->indexOfChild(currentItem);
            delete parent->takeChild(index);
        } else {
            index = indexOfTopLevelItem(currentItem);
            delete takeTopLevelItem(index);
        }
    }

    QString text;
    currentItem = this->currentItem();
    if (currentItem && QLatin1String("Folder") == currentItem->data(0, Qt::DisplayRole))
        text = currentItem->text(0);

    emit currentItemChanged(text);
    emit saveBookmarks(bookmarks());
}

void BookmarkWidget::selectBookmarkFolder(const QString &title)
{
    if (title.isEmpty())
        return;

    if (title == tr("Bookmarks")) {
        setItemSelected(currentItem(), false);
        setCurrentItem(0, 0);
        return;
    }

    QList<QTreeWidgetItem *> list = findItems(title,
        Qt::MatchCaseSensitive | Qt::MatchRecursive, 0);
    if (list.isEmpty())
        return;

    setCurrentItem(list.at(0), 0);
}

void BookmarkWidget::itemChanged(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);
    if (item) {
        if (item->text(0).isEmpty()) 
            item->setText(0, uniqueFolderName(this));
        emit currentItemChanged(item->text(0));
    }
}

void BookmarkWidget::currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    Q_UNUSED(previous)
    QString text;
    if (current)
        text = current->text(0);

    emit saveBookmarks(bookmarks());
}

void BookmarkWidget::addNewFolder(const QString &title)
{
    QString folderName = title;
    if (folderName.isEmpty())
        folderName = uniqueFolderName(this);

    QTreeWidgetItem *newItem = 0;
    QTreeWidgetItem *treeItem = itemIfNotDirectory();
    
    QStringList columnStrings(folderName);
    columnStrings << QLatin1String("Folder");
    if (treeItem) {
        newItem = new QTreeWidgetItem(columnStrings);
        treeItem->addChild(newItem);
    } else {
        newItem = new QTreeWidgetItem(this, columnStrings);
    }

    setCurrentItem(newItem);
    newItem->setIcon(0, QIcon(style()->standardPixmap(QStyle::SP_DirClosedIcon)));
    newItem->setFlags(newItem->flags() &~ (Qt::ItemIsEditable | Qt::ItemIsSelectable));
}

void BookmarkWidget::addNewBookmark(const QString &title, const QString &url)
{
    QTreeWidgetItem *newItem = 0;
    QTreeWidgetItem *treeItem = itemIfNotDirectory();

    QStringList columnStrings(title);
    if (treeItem) {
        newItem = new QTreeWidgetItem(columnStrings << url);
        treeItem->insertChild(0, newItem);
    } else {
        newItem = new QTreeWidgetItem(this, columnStrings << url);
    }

    emit saveBookmarks(bookmarks());
    setCurrentItem(newItem);
}

void BookmarkWidget::showContextMenu(const QPoint &point)
{
    QTreeWidgetItem *item = itemAt(point);
    if (!item)
        return;

    QAction *showItem;
    QAction *removeItem;
    QAction *renameItem;
    QAction *showItemNewTab;

    QMenu menu(QLatin1String(""), this);
    QString data = item->data(1, Qt::DisplayRole).toString();
    if (data == QLatin1String("Folder")) {
        removeItem = menu.addAction(tr("Delete Folder"));
        renameItem = menu.addAction(tr("Rename Folder"));
    } else {
        showItem = menu.addAction(tr("Show Bookmark"));
        showItemNewTab = menu.addAction(tr("Show Bookmark in New Tab"));
        menu.addSeparator();
        removeItem = menu.addAction(tr("Delete Bookmark"));
        renameItem = menu.addAction(tr("Rename Bookmark"));
    }

    QAction *picked_action = menu.exec(mapToGlobal(point));
    if (!picked_action)
        return;

    if (picked_action == showItem) {
        emit requestShowLink(data);
    }
    else if (picked_action == showItemNewTab) {
        CentralWidget::instance()->setSourceInNewTab(data);
    }
    else if (picked_action == removeItem) {
        setCurrentItem(item);
        removeBookmarkOrFolder();
    }
    else if (picked_action == renameItem) {
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        editItem(item);
        item->setFlags(item->flags() &~ Qt::ItemIsEditable);
    }
}

void BookmarkWidget::itemActivated(QTreeWidgetItem* item, int column)
{
    Q_UNUSED(column)

    if (item) {
        QString data = item->data(1, Qt::DisplayRole).toString();
        if (data != QLatin1String("Folder"))
            emit requestShowLink(data);
    }
}

QTreeWidgetItem* BookmarkWidget::itemIfNotDirectory()
{
    QTreeWidgetItem *currentItem = this->currentItem();

    if (currentItem) {
        QString data = currentItem->data(1, Qt::DisplayRole).toString();
        if (data != QLatin1String("Folder"))
            currentItem = currentItem->parent();
    }

    return currentItem;
}

bool BookmarkWidget::eventFilter(QObject *object, QEvent *e)
{
    if (object == this && e->type() == QEvent::KeyPress) {
        QKeyEvent *ke = static_cast<QKeyEvent*>(e);
        switch (ke->key()) {
        case Qt::Key_F2: {
            QTreeWidgetItem *item = currentItem();
            if (item) {
                item->setFlags(item->flags() | Qt::ItemIsEditable);
                editItem(item);
                item->setFlags(item->flags() &~ Qt::ItemIsEditable);
            }
            break;
            }
        case Qt::Key_Delete:
            removeBookmarkOrFolder();
            break;
        case Qt::Key_Escape:
            MainWindow::activateCurrentCentralWidgetTab();
            break;
        default:
            break;
        }
    }
    return QObject::eventFilter(object, e);
}

void BookmarkWidget::readBookmarkFolderRecursive(const QTreeWidgetItem *item, 
                                        QDataStream &stream, const qint32 depth) const
{
    for (int j = 0; j < item->childCount(); ++j) {
        const QTreeWidgetItem *child = item->child(j);
        stream << depth;
        stream << child->data(0, Qt::DisplayRole).toString();
        stream << child->data(1, Qt::DisplayRole).toString();
        stream << child->isExpanded();

        if (child->childCount() > 0)
            readBookmarkFolderRecursive(child, stream, (depth +1));
    }
}

QT_END_NAMESPACE
