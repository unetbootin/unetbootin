/****************************************************************************
**
** Copyright (C) 2005-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
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

#include <QtGui>

#include "settingstree.h"
#include "variantdelegate.h"

SettingsTree::SettingsTree(QWidget *parent)
    : QTreeWidget(parent)
{
    setItemDelegate(new VariantDelegate(this));

    QStringList labels;
    labels << tr("Setting") << tr("Type") << tr("Value");
    setHeaderLabels(labels);
    header()->setResizeMode(0, QHeaderView::Stretch);
    header()->setResizeMode(2, QHeaderView::Stretch);

    settings = 0;
    refreshTimer.setInterval(2000);
    autoRefresh = false;

    groupIcon.addPixmap(style()->standardPixmap(QStyle::SP_DirClosedIcon),
                        QIcon::Normal, QIcon::Off);
    groupIcon.addPixmap(style()->standardPixmap(QStyle::SP_DirOpenIcon),
                        QIcon::Normal, QIcon::On);
    keyIcon.addPixmap(style()->standardPixmap(QStyle::SP_FileIcon));

    connect(&refreshTimer, SIGNAL(timeout()), this, SLOT(maybeRefresh()));
}

void SettingsTree::setSettingsObject(QSettings *settings)
{
    delete this->settings;
    this->settings = settings;
    clear();

    if (settings) {
        settings->setParent(this);
        refresh();
        if (autoRefresh)
            refreshTimer.start();
    } else {
        refreshTimer.stop();
    }
}

QSize SettingsTree::sizeHint() const
{
    return QSize(800, 600);
}

void SettingsTree::setAutoRefresh(bool autoRefresh)
{
    this->autoRefresh = autoRefresh;
    if (settings) {
        if (autoRefresh) {
            maybeRefresh();
            refreshTimer.start();
        } else {
            refreshTimer.stop();
        }
    }
}

void SettingsTree::setFallbacksEnabled(bool enabled)
{
    if (settings) {
        settings->setFallbacksEnabled(enabled);
        refresh();
    }
}

void SettingsTree::maybeRefresh()
{
    if (state() != EditingState)
        refresh();
}

void SettingsTree::refresh()
{
    if (!settings)
        return;

    disconnect(this, SIGNAL(itemChanged(QTreeWidgetItem *, int)),
               this, SLOT(updateSetting(QTreeWidgetItem *)));

    settings->sync();
    updateChildItems(0);

    connect(this, SIGNAL(itemChanged(QTreeWidgetItem *, int)),
            this, SLOT(updateSetting(QTreeWidgetItem *)));
}

bool SettingsTree::event(QEvent *event)
{
    if (event->type() == QEvent::WindowActivate) {
        if (isActiveWindow() && autoRefresh)
            maybeRefresh();
    }
    return QTreeWidget::event(event);
}

void SettingsTree::updateSetting(QTreeWidgetItem *item)
{
    QString key = item->text(0);
    QTreeWidgetItem *ancestor = item->parent();
    while (ancestor) {
        key.prepend(ancestor->text(0) + "/");
        ancestor = ancestor->parent();
    }

    settings->setValue(key, item->data(2, Qt::UserRole));
    if (autoRefresh)
        refresh();
}

void SettingsTree::updateChildItems(QTreeWidgetItem *parent)
{
    int dividerIndex = 0;

    foreach (QString group, settings->childGroups()) {
        QTreeWidgetItem *child;
        int childIndex = findChild(parent, group, dividerIndex);
        if (childIndex != -1) {
            child = childAt(parent, childIndex);
            child->setText(1, "");
            child->setText(2, "");
            child->setData(2, Qt::UserRole, QVariant());
            moveItemForward(parent, childIndex, dividerIndex);
        } else {
            child = createItem(group, parent, dividerIndex);
        }
        child->setIcon(0, groupIcon);
        ++dividerIndex;

        settings->beginGroup(group);
        updateChildItems(child);
        settings->endGroup();
    }

    foreach (QString key, settings->childKeys()) {
        QTreeWidgetItem *child;
        int childIndex = findChild(parent, key, 0);

        if (childIndex == -1 || childIndex >= dividerIndex) {
            if (childIndex != -1) {
                child = childAt(parent, childIndex);
                for (int i = 0; i < child->childCount(); ++i)
                    delete childAt(child, i);
                moveItemForward(parent, childIndex, dividerIndex);
            } else {
                child = createItem(key, parent, dividerIndex);
            }
            child->setIcon(0, keyIcon);
            ++dividerIndex;
        } else {
            child = childAt(parent, childIndex);
        }

        QVariant value = settings->value(key);
        if (value.type() == QVariant::Invalid) {
            child->setText(1, "Invalid");
        } else {
            child->setText(1, value.typeName());
        }
        child->setText(2, VariantDelegate::displayText(value));
        child->setData(2, Qt::UserRole, value);
    }

    while (dividerIndex < childCount(parent))
        delete childAt(parent, dividerIndex);
}

QTreeWidgetItem *SettingsTree::createItem(const QString &text,
                                          QTreeWidgetItem *parent, int index)
{
    QTreeWidgetItem *after = 0;
    if (index != 0)
        after = childAt(parent, index - 1);

    QTreeWidgetItem *item;
    if (parent)
        item = new QTreeWidgetItem(parent, after);
    else
        item = new QTreeWidgetItem(this, after);

    item->setText(0, text);
    item->setFlags(item->flags() | Qt::ItemIsEditable);
    return item;
}

QTreeWidgetItem *SettingsTree::childAt(QTreeWidgetItem *parent, int index)
{
    if (parent)
        return parent->child(index);
    else
        return topLevelItem(index);
}

int SettingsTree::childCount(QTreeWidgetItem *parent)
{
    if (parent)
        return parent->childCount();
    else
        return topLevelItemCount();
}

int SettingsTree::findChild(QTreeWidgetItem *parent, const QString &text,
                            int startIndex)
{
    for (int i = startIndex; i < childCount(parent); ++i) {
        if (childAt(parent, i)->text(0) == text)
            return i;
    }
    return -1;
}

void SettingsTree::moveItemForward(QTreeWidgetItem *parent, int oldIndex,
                                   int newIndex)
{
    for (int i = 0; i < oldIndex - newIndex; ++i)
        delete childAt(parent, newIndex);
}
