/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the tools applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "featuretreemodel.h"
#include "feature.h"
#include <QPalette>
#include <QColor>
#include <QApplication>
#include <QtDebug>

QT_BEGIN_NAMESPACE

class Node
{
public:
    Node(Feature *f, Node *p = 0) : feature(f), parent(p) {}
    ~Node();
    Node* find(const Feature *child) const;
    bool contains(const Feature *child) const { return find(child) != 0; }
    bool insert(Node *n);

    Feature *feature;
    Node *parent;
    QList<Node*> children; // maybe convert to Map to get keys sorted
};

Node::~Node()
{
    while (!children.isEmpty())
        delete children.takeFirst();
}

Node* Node::find(const Feature *f) const
{
    if (this->feature == f)
	return const_cast<Node*>(this);

    foreach (Node *n, children)
	if (Node *m = n->find(f))
	    return m;

    return 0;
}

static bool nodePtrLessThan(const Node *n1, const Node *n2)
{
    return (n1->feature->key() < n2->feature->key());
}

/*
  Try insert \a n into the tree with this node as root.
  n is inserted as a child if it has a dependency to this node.
  Returns true if child is inserted into the tree, false otherwise.
*/
bool Node::insert(Node *n)
{
    Feature *f = const_cast<Feature*>(n->feature);
    if (feature->supports().contains(f)) {
        children.append(n);
	qSort(children.begin(), children.end(), nodePtrLessThan);
	n->parent = this;
        return true;
    }
    foreach (Node *child, children)
        if (child->insert(n))
            return true;
    return false;
}

static bool isSection(const QModelIndex &index)
{
    return index.isValid() && (index.internalId() == 0);
}

FeatureTreeModel::FeatureTreeModel(QObject *parent)
    : QAbstractItemModel(parent)
{
}

FeatureTreeModel::~FeatureTreeModel()
{
    foreach (QString section, sections.keys())
	while (!sections[section].isEmpty())
	    delete sections[section].takeFirst();
}

/*
  Returns true if the model already contains \a in \a section, false otherwise.
*/
bool FeatureTreeModel::contains(const QString &section, const Feature *f) const
{
    return (find(section, f) != 0);
}

Node* FeatureTreeModel::find(const QString &section, const Feature *f) const
{
    QList<Node*> roots = sections[section];
    foreach (Node *root, roots)
        if (Node *n = root->find(f))
            return n;
    return 0;
}

/*
  Add new \a feature to the tree.
  When all feature is added, buildTree() must be called to build the
  dependency tree.
*/
void FeatureTreeModel::addFeature(Feature *feature)
{
    const QString section = feature->section();
    Q_ASSERT(!contains(section, feature));

    connect(feature, SIGNAL(changed()), this, SLOT(featureChanged()));

    Node *node = new Node(feature, 0);

    // try insert any toplevel nodes as child of this one
    foreach (Node *n, sections[section])
	if (node->insert(n))
	    sections[section].removeAll(n);

    // try insert this node as a child of any existing node
    foreach (Node *n, sections[section])
	if (n->insert(node)) {
            emit layoutChanged();
	    return;
        }

    // not a child, insert as a toplevel node
    sections[section].append(node);
    qSort(sections[section].begin(), sections[section].end(), nodePtrLessThan);
    emit layoutChanged();
}

QModelIndex FeatureTreeModel::createIndex(int row, int column,
					  const QModelIndex &parent,
					  const Node *node) const
{
    QModelIndex index = QAbstractItemModel::createIndex(row, column,
							(void*)node);
    if (parent.isValid())
	parentMap[index] = parent;
    if (node)
        featureIndexMap[node->feature] = index;
    return index;
}

QModelIndex FeatureTreeModel::index(int row, int column,
                                    const QModelIndex &parent) const
{
    if (!parent.isValid()) { // index is a section
        if (row < sections.size() && column == 0)
            return QAbstractItemModel::createIndex(row, column, 0);
	return QModelIndex();
    }

    if (isSection(parent)) { // index is a toplevel feature
        const int parentRow = parent.row();
        if (parentRow < sections.size()) {
            QString section = sections.keys().at(parentRow);
	    QList<Node*> nodes = sections[section];
            if (row < nodes.size() && column < 2)
                return createIndex(row, column, parent, nodes.at(row));
        }
	return QModelIndex();
    }

    // parent is a feature
    Node *parentNode = static_cast<Node*>(parent.internalPointer());
    QList<Node*> children = parentNode->children;
    if (row < children.size() && column < 2)
	return createIndex(row, column, parent, children.at(row));

    return QModelIndex();
}

QModelIndex FeatureTreeModel::index(const QModelIndex &parent,
                                    const Feature *feature) const
{
    const int rows = rowCount(parent);
    for (int i = 0; i < rows; ++i) {
        QModelIndex child = index(i, 0, parent);
        Node *node = static_cast<Node*>(child.internalPointer());
        if (node && node->feature == feature)
            return child;
        QModelIndex childSearch = index(child, feature);
        if (childSearch.isValid())
            return childSearch;
    }
    return QModelIndex();
}

QModelIndex FeatureTreeModel::index(const Feature *feature) const
{
    if (featureIndexMap.contains(feature))
        return featureIndexMap.value(feature);

    // exhaustive search
    int sectionRow = sections.keys().indexOf(feature->section());
    QModelIndex sectionIndex = index(sectionRow, 0, QModelIndex());

    return index(sectionIndex, feature);
}

QModelIndex FeatureTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    if (parentMap.contains(index))
	return parentMap.value(index);
    return QModelIndex();
}

int FeatureTreeModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return sections.size();

    if (isSection(parent)) {
	const QString section = sections.keys().at(parent.row());
	return sections[section].size();
    }

    const Node *node = static_cast<Node*>(parent.internalPointer());
    return node->children.size();
}

int FeatureTreeModel::columnCount(const QModelIndex &parent) const
{
#if 0
    if (!parent.isValid())
        return 0;

    if (isSection(parent))
       return 1;
#endif
    Q_UNUSED(parent);
    return 2; // Feature: [key, name]
}

QVariant FeatureTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const Node *node = static_cast<Node*>(index.internalPointer());

    switch (role) {
	case Qt::DisplayRole: {
	    if (node == 0)  // index is a section
		return sections.keys().at(index.row());
	    if (index.column() == 0)
		return node->feature->key();
	    Q_ASSERT(index.column() == 1);
	    return node->feature->title();
	}
	case Qt::CheckStateRole: {
	    if (node && index.column() == 0)
		return (node->feature->enabled() ?
			Qt::Checked : Qt::Unchecked);
	    break;
	}
	case Qt::TextColorRole: {
	    if (node && index.column() == 0)  // feature key
		if (node->feature->selectable())
		    return QApplication::palette().color(QPalette::Link);
	    break;
	}
	case Qt::TextAlignmentRole:
	case Qt::BackgroundColorRole:
	case Qt::FontRole:
	case Qt::ToolTipRole: // TODO
	case Qt::StatusTipRole: // TODO
	case Qt::WhatsThisRole: // TODO
	case Qt::DecorationRole:
	case Qt::EditRole:
 	default:
	    break;
    }
    return QVariant();
}

bool FeatureTreeModel::setData(const QModelIndex &index,
                               const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    Node *node = static_cast<Node*>(index.internalPointer());
    if (!node)
        return false;

    if (role == Qt::CheckStateRole) {
        Qt::CheckState state = static_cast<Qt::CheckState>(value.toInt());
        if (state == Qt::Checked)
            node->feature->setEnabled(true);
        else if (state == Qt::Unchecked)
            node->feature->setEnabled(false);
        emit dataChanged(index, index);
        return true;
    }
    return false;
}

Qt::ItemFlags FeatureTreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid() || index.internalPointer() == 0)
	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    const Node *node = static_cast<Node*>(index.internalPointer());
    const Feature *feature = node->feature;
    Qt::ItemFlags flags = Qt::ItemIsUserCheckable | Qt::ItemIsSelectable;

    if (feature->selectable())
	flags |= Qt::ItemIsEnabled;

    return flags;
}

QVariant FeatureTreeModel::headerData(int section, Qt::Orientation orientation,
				      int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        if (section == 0)
            return QString("Id");
        else if (section == 1)
            return QString("Name");
    }

    return QVariant();
}

Feature* FeatureTreeModel::getFeature(const QModelIndex &index) const
{
    if (!index.isValid())
	return 0;
    if (isSection(index))
	return 0;
    Node *node = static_cast<Node*>(index.internalPointer());
    return const_cast<Feature*>(node->feature);
}

void FeatureTreeModel::featureChanged()
{
    Feature *feature = qobject_cast<Feature*>(sender());
    if (feature) {
        QModelIndex featureIndex = index(feature);
        emit dataChanged(featureIndex, featureIndex);
    } else {
        emit layoutChanged();
    }
}

void FeatureTreeModel::readConfig(QTextStream &stream)
{
    static QRegExp regexp("\\s*#\\s*define\\s+QT_NO_(\\S+)\\s*");

    while (!stream.atEnd()) {
	QString line = stream.readLine();
        if (regexp.exactMatch(line)) {
            Feature *f = Feature::getInstance(regexp.cap(1));
            f->setEnabled(false);
        }
    }
}
/*
  Search for all disabled child features of \a parent.
  Returns a list of feature keys for the disabled items.
*/
QStringList FeatureTreeModel::findDisabled(const QModelIndex &parent) const
{
    QStringList stringList;

    const int rows = rowCount(parent);
    for (int i = 0; i < rows; ++i) {
        QModelIndex child = index(i, 0, parent);
        Node *node = static_cast<Node*>(child.internalPointer());
        if (node && node->feature && !node->feature->enabled())
            stringList << node->feature->key();
        stringList << findDisabled(child);
    }
    return stringList;
}

void FeatureTreeModel::writeConfig(QTextStream &stream) const
{
    const int sectionCount = rowCount(QModelIndex());

    for (int i = 0; i < sectionCount; ++i) {
        QModelIndex section = index(i, 0, QModelIndex());
        QStringList disabled = findDisabled(section);
        if (disabled.size() > 0) {
            stream << '\n' << "/* " << sections.keys().at(i) << " */" << '\n';
            foreach (QString feature, disabled)
                stream << "#ifndef QT_NO_" << feature << '\n'
                       << "#  define QT_NO_" << feature << '\n'
                       << "#endif" << '\n';
        }
    }
}

void FeatureTreeModel::clear()
{
    Feature::clear();
    sections.clear();
    parentMap.clear();
    featureIndexMap.clear();
    emit layoutChanged();
}

QT_END_NAMESPACE
