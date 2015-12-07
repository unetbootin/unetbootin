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

#include "model.h"
#include <QIcon>
#include <QPixmap>

Model::Model(int rows, int columns, QObject *parent)
    : QAbstractItemModel(parent),
      rc(rows), cc(columns),
      tree(new QVector<Node>(rows, Node(0)))
{

}

Model::~Model()
{
    delete tree;
}

QModelIndex Model::index(int row, int column, const QModelIndex &parent) const
{
    if (row < rc && row >= 0 && column < cc && column >= 0) {
        Node *p = static_cast<Node*>(parent.internalPointer());
        Node *n = node(row, p);
	if (n)
	    return createIndex(row, column, n);
    }
    return QModelIndex();
}

QModelIndex Model::parent(const QModelIndex &child) const
{
    if (child.isValid()) {
        Node *n = static_cast<Node*>(child.internalPointer());
        Node *p = parent(n);
        if (p)
            return createIndex(row(p), 0, p);
    }
    return QModelIndex();
}

int Model::rowCount(const QModelIndex &parent) const
{
    return (parent.isValid() && parent.column() != 0) ? 0 : rc;
}

int Model::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return cc;
}

QVariant Model::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    if (role == Qt::DisplayRole)
	return "Item " + QString::number(index.row()) + ":" + QString::number(index.column());
    if (role == Qt::DecorationRole) {
        if (index.column() == 0)
            return iconProvider.icon(QFileIconProvider::Folder);
        return iconProvider.icon(QFileIconProvider::File);
    }
    return QVariant();
}

QVariant Model::headerData(int section, Qt::Orientation orientation, int role) const
{
    static QIcon services(QPixmap(":/images/services.png"));
    if (role == Qt::DisplayRole)
        return QString::number(section);
    if (role == Qt::DecorationRole)
        return qVariantFromValue(services);
    return QAbstractItemModel::headerData(section, orientation, role);
}

bool Model::hasChildren(const QModelIndex &parent) const
{
    if (parent.isValid() && parent.column() != 0)
        return false;
    return rc > 0 && cc > 0;
}

Qt::ItemFlags Model::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;
    return (Qt::ItemIsDragEnabled|Qt::ItemIsSelectable|Qt::ItemIsEnabled);
}

Model::Node *Model::node(int row, Node *parent) const
{
    if (parent && !parent->children)
	parent->children = new QVector<Node>(rc, Node(parent));
    QVector<Node> *v = parent ? parent->children : tree;
    return const_cast<Node*>(&(v->at(row)));
}

Model::Node *Model::parent(Node *child) const
{
    return child ? child->parent : 0;
}

int Model::row(Node *node) const
{
     const Node *first = node->parent ? &(node->parent->children->at(0)) : &(tree->at(0));
     return (node - first);
}
