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

#include "qdbusmodel.h"

#include <QtCore/qvector.h>
#include <QtXml/QtXml>

struct QDBusItem
{
    inline QDBusItem(QDBusModel::Type aType, const QString &aName, QDBusItem *aParent = 0)
        : type(aType), parent(aParent), isPrefetched(type != QDBusModel::PathItem), name(aName)
        {}
    inline ~QDBusItem()
    {
        qDeleteAll(children);
    }

    QString path() const
    {
        Q_ASSERT(type == QDBusModel::PathItem);

        QString s;
        const QDBusItem *item = this;
        while (item) {
            s.prepend(item->name);
            item = item->parent;
        }
        if (s.length() > 1)
            s.chop(1); // remove tailing slash
        return s;
    }

    QDBusModel::Type type;
    QDBusItem *parent;
    QVector<QDBusItem *> children;
    bool isPrefetched;
    QString name;
    QString caption;
};

QDomDocument QDBusModel::introspect(const QString &path)
{
    QDomDocument doc;

    QDBusInterface iface(service, path, "org.freedesktop.DBus.Introspectable", c);
    if (!iface.isValid()) {
        QDBusError err(iface.lastError());
        emit busError(QString("Cannot introspect object %1 at %2:\n  %3 (%4)\n").arg(path).arg(
                      service).arg(err.name()).arg(err.message()));
        return doc;
    }

    QDBusReply<QString> xml = iface.call("Introspect");

    if (!xml.isValid()) {
        QDBusError err(xml.error());
        if (err.isValid()) {
            emit busError(QString("Call to object %1 at %2:\n  %3 (%4) failed\n").arg(
                        path).arg(service).arg(err.name()).arg(err.message()));
        } else {
            emit busError(QString("Invalid XML received from object %1 at %2\n").arg(
                    path).arg(service));
        }
        return doc;
    }

    doc.setContent(xml);
    return doc;
}

void QDBusModel::addMethods(QDBusItem *parent, const QDomElement &iface)
{
    Q_ASSERT(parent);

    QDomElement child = iface.firstChildElement();
    while (!child.isNull()) {
        QDBusItem *item = 0;
        if (child.tagName() == QLatin1String("method")) {
            item = new QDBusItem(QDBusModel::MethodItem,
                    child.attribute("name"), parent);
            item->caption = "Method: " + item->name;
        } else if (child.tagName() == QLatin1String("signal")) {
            item = new QDBusItem(QDBusModel::SignalItem,
                    child.attribute("name"), parent);
            item->caption = "Signal: " + item->name;
        } else if (child.tagName() == QLatin1String("property")) {
            item = new QDBusItem(QDBusModel::PropertyItem,
                    child.attribute("name"), parent);
            item->caption = "Property: " + item->name;
        } else {
            qDebug() << "addMethods: unknown tag:" << child.tagName();
        }
        if (item)
            parent->children.append(item);

        child = child.nextSiblingElement();
    }
}

void QDBusModel::addPath(QDBusItem *parent)
{
    Q_ASSERT(parent);

    QString path = parent->path();

    QDomDocument doc = introspect(path);
    QDomElement node = doc.documentElement();
    QDomElement child = node.firstChildElement();
    while (!child.isNull()) {
        if (child.tagName() == QLatin1String("node")) {
            QDBusItem *item = new QDBusItem(QDBusModel::PathItem,
                        child.attribute("name") + QLatin1Char('/'), parent);
            parent->children.append(item);

            addMethods(item, child);
        } else if (child.tagName() == QLatin1String("interface")) {
            QDBusItem *item = new QDBusItem(QDBusModel::InterfaceItem,
                        child.attribute("name"), parent);
            parent->children.append(item);

            addMethods(item, child);
        } else {
            qDebug() << "addPath: Unknown tag name:" << child.tagName();
        }
        child = child.nextSiblingElement();
    }

    parent->isPrefetched = true;
}

QDBusModel::QDBusModel(const QString &aService, const QDBusConnection &connection)
    : service(aService), c(connection), root(0)
{
    root = new QDBusItem(QDBusModel::PathItem, QLatin1String("/"));
}

QDBusModel::~QDBusModel()
{
    delete root;
}

QModelIndex QDBusModel::index(int row, int column, const QModelIndex &parent) const
{
    const QDBusItem *item = static_cast<QDBusItem *>(parent.internalPointer());
    if (!item)
        item = root;

    if (column != 0 || row < 0 || row >= item->children.count())
        return QModelIndex();

    return createIndex(row, 0, item->children.at(row));
}

QModelIndex QDBusModel::parent(const QModelIndex &child) const
{
    QDBusItem *item = static_cast<QDBusItem *>(child.internalPointer());
    if (!item || !item->parent || !item->parent->parent)
        return QModelIndex();

    return createIndex(item->parent->parent->children.indexOf(item->parent), 0, item->parent);
}

int QDBusModel::rowCount(const QModelIndex &parent) const
{
    QDBusItem *item = static_cast<QDBusItem *>(parent.internalPointer());
    if (!item)
        item = root;
    if (!item->isPrefetched)
        const_cast<QDBusModel *>(this)->addPath(item);

    return item->children.count();
}

int QDBusModel::columnCount(const QModelIndex &) const
{
    return 1;
}

QVariant QDBusModel::data(const QModelIndex &index, int role) const
{
    const QDBusItem *item = static_cast<QDBusItem *>(index.internalPointer());
    if (!item)
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    return item->caption.isEmpty() ? item->name : item->caption;
}

QVariant QDBusModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation == Qt::Vertical || section != 0)
        return QVariant();

    return QLatin1String("Methods");
}

QDBusModel::Type QDBusModel::itemType(const QModelIndex &index) const
{
    const QDBusItem *item = static_cast<QDBusItem *>(index.internalPointer());
    return item ? item->type : PathItem;
}

void QDBusModel::refresh(const QModelIndex &aIndex)
{
    QModelIndex index = aIndex;
    while (index.isValid() && static_cast<QDBusItem *>(index.internalPointer())->type != PathItem) {
        index = index.parent();
    }

    QDBusItem *item = static_cast<QDBusItem *>(index.internalPointer());
    if (!item)
        item = root;

    if (!item->children.isEmpty()) {
        beginRemoveRows(index, 0, item->children.count() - 1);
        qDeleteAll(item->children);
        item->children.clear();
        endRemoveRows();
    }

    addPath(item);
    if (!item->children.isEmpty()) {
        beginInsertRows(index, 0, item->children.count() - 1);
        endInsertRows();
    }
}

QString QDBusModel::dBusPath(const QModelIndex &aIndex) const
{
    QModelIndex index = aIndex;
    while (index.isValid() && static_cast<QDBusItem *>(index.internalPointer())->type != PathItem) {
        index = index.parent();
    }

    QDBusItem *item = static_cast<QDBusItem *>(index.internalPointer());
    if (!item)
        item = root;

    return item->path();
}

QString QDBusModel::dBusInterface(const QModelIndex &index) const
{
    QDBusItem *item = static_cast<QDBusItem *>(index.internalPointer());
    if (!item)
        return QString();
    if (item->type == InterfaceItem)
        return item->name;
    if (item->parent && item->parent->type == InterfaceItem)
        return item->parent->name;
    return QString();
}

QString QDBusModel::dBusMethodName(const QModelIndex &index) const
{
    QDBusItem *item = static_cast<QDBusItem *>(index.internalPointer());
    return item ? item->name : QString();
}

QModelIndex QDBusModel::findObject(const QDBusObjectPath &objectPath)
{
    QStringList path = objectPath.path().split(QLatin1Char('/'), QString::SkipEmptyParts);

    QDBusItem *item = root;
    int childIdx = -1;
    while (item && !path.isEmpty()) {
        const QString branch = path.takeFirst() + QLatin1Char('/');
        childIdx = -1;

        // do a linear search over all the children
        for (int i = 0; i < item->children.count(); ++i) {
            QDBusItem *child = item->children.at(i);
            if (child->type == PathItem && child->name == branch) {
                item = child;
                childIdx = i;

                // prefetch the found branch
                if (!item->isPrefetched)
                    addPath(item);
                break;
            }
        }

        // branch not found - bail out
        if (childIdx == -1)
            return QModelIndex();
    }

    // found the right item
    if (childIdx != -1 && item && path.isEmpty())
        return createIndex(childIdx, 0, item);

    return QModelIndex();
}

