/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtScriptTools module of the Qt Toolkit.
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

#include "qscriptdebuggerlocalsmodel_p.h"
#include "qscriptdebuggercommandschedulerjob_p.h"
#include "qscriptdebuggervalue_p.h"
#include "qscriptdebuggerresponse_p.h"
#include "qscriptdebuggerevent_p.h"
#include "qscriptdebuggervalueproperty_p.h"
#include "qscriptdebuggercommandschedulerinterface_p.h"
#include "qscriptdebuggercommandschedulerfrontend_p.h"
#include "qscriptdebuggerjobschedulerinterface_p.h"
#include "qscriptdebuggerobjectsnapshotdelta_p.h"

#include "private/qabstractitemmodel_p.h"

#include <QtCore/qdebug.h>
#include <QtGui/qbrush.h>
#include <QtGui/qfont.h>

Q_DECLARE_METATYPE(QScriptDebuggerObjectSnapshotDelta)

QT_BEGIN_NAMESPACE

struct QScriptDebuggerLocalsModelNode
{
    enum PopulationState {
        NotPopulated,
        Populating,
        Populated
    };

    QScriptDebuggerLocalsModelNode()
        : parent(0), populationState(NotPopulated), snapshotId(-1), changed(false) {}

    QScriptDebuggerLocalsModelNode(
        const QScriptDebuggerValueProperty &prop,
        QScriptDebuggerLocalsModelNode *par)
        : property(prop), parent(par),
          populationState(NotPopulated), snapshotId(-1), changed(false)
    {
        parent->children.append(this);
    }

    ~QScriptDebuggerLocalsModelNode() { qDeleteAll(children); }

    QScriptDebuggerLocalsModelNode *findChild(const QString &name)
    {
        for (int i = 0; i < children.size(); ++i) {
            QScriptDebuggerLocalsModelNode *child = children.at(i);
            if (child->property.name() == name)
                return child;
        }
        return 0;
    }

    QScriptDebuggerValueProperty property;
    QScriptDebuggerLocalsModelNode *parent;
    QList<QScriptDebuggerLocalsModelNode*> children;
    PopulationState populationState;
    int snapshotId;
    bool changed;
};

class QScriptDebuggerLocalsModelPrivate
    : public QAbstractItemModelPrivate
{
    Q_DECLARE_PUBLIC(QScriptDebuggerLocalsModel)
public:
    QScriptDebuggerLocalsModelPrivate();
    ~QScriptDebuggerLocalsModelPrivate();

    static QScriptDebuggerLocalsModelPrivate *get(QScriptDebuggerLocalsModel *q);

    QModelIndex addTopLevelObject(const QString &name, const QScriptDebuggerValue &object);

    QScriptDebuggerLocalsModelNode *nodeFromIndex(const QModelIndex &index) const;
    QModelIndex indexFromNode(QScriptDebuggerLocalsModelNode *node) const;
    bool isTopLevelNode(QScriptDebuggerLocalsModelNode *node) const;

    void populateIndex(const QModelIndex &index);
    void reallyPopulateIndex(const QModelIndex &index,
                             const QScriptDebuggerValuePropertyList &props);
    void syncIndex(const QModelIndex &index);
    void reallySyncIndex(const QModelIndex &index,
                         const QScriptDebuggerObjectSnapshotDelta &delta);
    void syncTopLevelNodes();
    void removeTopLevelNodes();
    void emitScopeObjectAvailable(const QModelIndex &index);

    void emitDataChanged(const QModelIndex &tl, const QModelIndex &br);
    void removeChild(const QModelIndex &parentIndex,
                     QScriptDebuggerLocalsModelNode *parentNode, int row);
    void depopulate(QScriptDebuggerLocalsModelNode *node);
    void repopulate(QScriptDebuggerLocalsModelNode *node);
    void addChildren(const QModelIndex &parentIndex,
                     QScriptDebuggerLocalsModelNode *parentNode,
                     const QScriptDebuggerValuePropertyList &props);

    void deleteObjectSnapshots(const QList<qint64> &snapshotIds);

    QScriptDebuggerJobSchedulerInterface *jobScheduler;
    QScriptDebuggerCommandSchedulerInterface *commandScheduler;
    QScriptDebuggerLocalsModelNode *invisibleRootNode;
    int frameIndex;
};

QScriptDebuggerLocalsModelPrivate::QScriptDebuggerLocalsModelPrivate()
{
    invisibleRootNode = new QScriptDebuggerLocalsModelNode();
    frameIndex = -1;
}

QScriptDebuggerLocalsModelPrivate::~QScriptDebuggerLocalsModelPrivate()
{
    delete invisibleRootNode;
}

void QScriptDebuggerLocalsModelPrivate::emitDataChanged(const QModelIndex &tl, const QModelIndex &br)
{
    q_func()->dataChanged(tl, br);
}

static QList<qint64> findSnapshotIdsRecursively(QScriptDebuggerLocalsModelNode *root)
{
    QList<qint64> result;
    if (root->snapshotId == -1) {
        Q_ASSERT(root->children.isEmpty());
        return result;
    }
    QList<QScriptDebuggerLocalsModelNode*> nodeStack;
    nodeStack.append(root);
    while (!nodeStack.isEmpty()) {
        QScriptDebuggerLocalsModelNode *node = nodeStack.takeFirst();
        result.append(node->snapshotId);
        for (int i = 0; i < node->children.count(); ++i) {
            QScriptDebuggerLocalsModelNode *child = node->children.at(i);
            if (child->snapshotId != -1)
                nodeStack.prepend(child);
        }
    }
    return result;
}

void QScriptDebuggerLocalsModelPrivate::removeChild(const QModelIndex &parentIndex,
                                                    QScriptDebuggerLocalsModelNode *parentNode,
                                                    int row)
{
    Q_Q(QScriptDebuggerLocalsModel);
    q->beginRemoveRows(parentIndex, row, row);
    QScriptDebuggerLocalsModelNode *child = parentNode->children.takeAt(row);
    QList<qint64> snapshotIds = findSnapshotIdsRecursively(child);
    delete child;
    q->endRemoveRows();
    deleteObjectSnapshots(snapshotIds);
}

void QScriptDebuggerLocalsModelPrivate::depopulate(QScriptDebuggerLocalsModelNode *node)
{
    Q_Q(QScriptDebuggerLocalsModel);
    bool hasChildren = !node->children.isEmpty();
    if (hasChildren)
        q->beginRemoveRows(indexFromNode(node), 0, node->children.count() - 1);
    QList<qint64> snapshotIds = findSnapshotIdsRecursively(node);
    qDeleteAll(node->children);
    node->children.clear();
    node->snapshotId = -1;
    node->populationState = QScriptDebuggerLocalsModelNode::NotPopulated;
    if (hasChildren)
        q->endRemoveRows();
    deleteObjectSnapshots(snapshotIds);
}

void QScriptDebuggerLocalsModelPrivate::repopulate(QScriptDebuggerLocalsModelNode *node)
{
    if (node->populationState != QScriptDebuggerLocalsModelNode::Populated)
        return;
    depopulate(node);
    if (node->property.value().type() == QScriptDebuggerValue::ObjectValue)
        populateIndex(indexFromNode(node));
}

void QScriptDebuggerLocalsModelPrivate::addChildren(const QModelIndex &parentIndex,
                                                    QScriptDebuggerLocalsModelNode *parentNode,
                                                    const QScriptDebuggerValuePropertyList &props)
{
    Q_Q(QScriptDebuggerLocalsModel);
    if (props.isEmpty())
        return;
    int first = parentNode->children.size();
    int last = first + props.size() - 1;
    q->beginInsertRows(parentIndex, first, last);
    for (int i = 0; i < props.size(); ++i)
        new QScriptDebuggerLocalsModelNode(props.at(i), parentNode);
    q->endInsertRows();
}

void QScriptDebuggerLocalsModelPrivate::deleteObjectSnapshots(const QList<qint64> &snapshotIds)
{
    QScriptDebuggerCommandSchedulerFrontend frontend(commandScheduler, 0);
    for (int i = 0; i < snapshotIds.size(); ++i)
        frontend.scheduleDeleteScriptObjectSnapshot(snapshotIds.at(i));
}

QScriptDebuggerLocalsModelPrivate *QScriptDebuggerLocalsModelPrivate::get(QScriptDebuggerLocalsModel *q)
{
    return q->d_func();
}

namespace {

class SetPropertyJob : public QScriptDebuggerCommandSchedulerJob
{
public:
    SetPropertyJob(const QPersistentModelIndex &index,
                   const QString &expression,
                   QScriptDebuggerCommandSchedulerInterface *scheduler)
        : QScriptDebuggerCommandSchedulerJob(scheduler),
          m_index(index), m_expression(expression), m_state(0) {}

    QScriptDebuggerLocalsModelPrivate *model() const
    {
        if (!m_index.isValid())
            return 0;
        QAbstractItemModel *m = const_cast<QAbstractItemModel*>(m_index.model());
        QScriptDebuggerLocalsModel *lm = qobject_cast<QScriptDebuggerLocalsModel*>(m);
        return QScriptDebuggerLocalsModelPrivate::get(lm);
    }

    void start()
    {
        if (!m_index.isValid()) {
            // nothing to do, the node has been removed
            return;
        }
        QScriptDebuggerLocalsModelNode *node = model()->nodeFromIndex(m_index);
        QScriptDebuggerCommandSchedulerFrontend frontend(commandScheduler(), this);
        frontend.scheduleEvaluate(model()->frameIndex, m_expression,
                                  QString::fromLatin1("set property '%0' (%1)")
                                  .arg(node->property.name())
                                  .arg(QDateTime::currentDateTime().toString()));
    }

    void handleResponse(const QScriptDebuggerResponse &, int)
    {
        switch (m_state) {
        case 0:
            hibernateUntilEvaluateFinished();
            ++m_state;
            break;
        case 1:
            finish();
            break;
        }
    }

    void evaluateFinished(const QScriptDebuggerValue &result)
    {
        if (!m_index.isValid()) {
            // nothing to do, the node has been removed
            return;
        }
        QScriptDebuggerLocalsModelNode *node = model()->nodeFromIndex(m_index);
        Q_ASSERT(node->parent != 0);
        QScriptDebuggerValue object = node->parent->property.value();
        QScriptDebuggerCommandSchedulerFrontend frontend(commandScheduler(), this);
        frontend.scheduleSetScriptValueProperty(object, node->property.name(), result);
    }
    
private:
    QPersistentModelIndex m_index;
    QString m_expression;
    int m_state;
};

} // namespace

QScriptDebuggerLocalsModelNode *QScriptDebuggerLocalsModelPrivate::nodeFromIndex(
    const QModelIndex &index) const
{
    if (!index.isValid())
        return invisibleRootNode;
    return static_cast<QScriptDebuggerLocalsModelNode*>(index.internalPointer());
}

QModelIndex QScriptDebuggerLocalsModelPrivate::indexFromNode(
    QScriptDebuggerLocalsModelNode *node) const
{
    if (!node || (node == invisibleRootNode))
        return QModelIndex();
    QScriptDebuggerLocalsModelNode *par = node->parent;
    int row = par ? par->children.indexOf(node) : 0;
    return createIndex(row, 0, node);
}

bool QScriptDebuggerLocalsModelPrivate::isTopLevelNode(QScriptDebuggerLocalsModelNode *node) const
{
    return node && (node->parent == invisibleRootNode);
}

namespace {

class PopulateModelIndexJob : public QScriptDebuggerCommandSchedulerJob
{
public:
    PopulateModelIndexJob(const QPersistentModelIndex &index,
                          QScriptDebuggerCommandSchedulerInterface *scheduler)
        : QScriptDebuggerCommandSchedulerJob(scheduler),
          m_index(index), m_state(0)
    { }

    QScriptDebuggerLocalsModelPrivate *model() const
    {
        if (!m_index.isValid())
            return 0;
        QAbstractItemModel *m = const_cast<QAbstractItemModel*>(m_index.model());
        QScriptDebuggerLocalsModel *lm = qobject_cast<QScriptDebuggerLocalsModel*>(m);
        return QScriptDebuggerLocalsModelPrivate::get(lm);
    }

    void start()
    {
        if (!m_index.isValid()) {
            // nothing to do, the node has been removed
            return;
        }
        QScriptDebuggerCommandSchedulerFrontend frontend(commandScheduler(), this);
        frontend.scheduleNewScriptObjectSnapshot();
    }

    void handleResponse(const QScriptDebuggerResponse &response,
                        int)
    {
        if (!m_index.isValid()) {
            // the node has been removed
            finish();
            return;
        }
        switch (m_state) {
        case 0: {
            QScriptDebuggerLocalsModelNode *node = model()->nodeFromIndex(m_index);
            Q_ASSERT(node->populationState == QScriptDebuggerLocalsModelNode::Populating);
            node->snapshotId = response.resultAsInt();
            QScriptDebuggerCommandSchedulerFrontend frontend(commandScheduler(), this);
            frontend.scheduleScriptObjectSnapshotCapture(node->snapshotId, node->property.value());
            ++m_state;
        }   break;
        case 1: {
            QScriptDebuggerObjectSnapshotDelta delta;
            delta = qvariant_cast<QScriptDebuggerObjectSnapshotDelta>(response.result());
            Q_ASSERT(delta.removedProperties.isEmpty());
            Q_ASSERT(delta.changedProperties.isEmpty());
            QScriptDebuggerValuePropertyList props = delta.addedProperties;
            model()->reallyPopulateIndex(m_index, props);
            finish();
          } break;
        }
    }

private:
    QPersistentModelIndex m_index;
    int m_state;
};

} // namespace

void QScriptDebuggerLocalsModelPrivate::populateIndex(
    const QModelIndex &index)
{
    if (!index.isValid())
        return;
    QScriptDebuggerLocalsModelNode *node = nodeFromIndex(index);
    if (node->populationState != QScriptDebuggerLocalsModelNode::NotPopulated)
        return;
    if (node->property.value().type() != QScriptDebuggerValue::ObjectValue)
        return;
    node->populationState = QScriptDebuggerLocalsModelNode::Populating;
    QScriptDebuggerJob *job = new PopulateModelIndexJob(index, commandScheduler);
    jobScheduler->scheduleJob(job);
}

void QScriptDebuggerLocalsModelPrivate::reallyPopulateIndex(
    const QModelIndex &index,
    const QScriptDebuggerValuePropertyList &props)
{
    if (!index.isValid())
        return;
    QScriptDebuggerLocalsModelNode *node = nodeFromIndex(index);
    Q_ASSERT(node->populationState == QScriptDebuggerLocalsModelNode::Populating);
    node->populationState = QScriptDebuggerLocalsModelNode::Populated;
    addChildren(index, node, props);
}

QScriptDebuggerLocalsModel::QScriptDebuggerLocalsModel(
    QScriptDebuggerJobSchedulerInterface *jobScheduler,
    QScriptDebuggerCommandSchedulerInterface *commandScheduler,
    QObject *parent)
    : QAbstractItemModel(*new QScriptDebuggerLocalsModelPrivate, parent)
{
    Q_D(QScriptDebuggerLocalsModel);
    d->jobScheduler = jobScheduler;
    d->commandScheduler = commandScheduler;
}

QScriptDebuggerLocalsModel::~QScriptDebuggerLocalsModel()
{
    Q_D(QScriptDebuggerLocalsModel);
    QList<qint64> snapshotIds;
    for (int i = 0; i < d->invisibleRootNode->children.count(); ++i)
        snapshotIds += findSnapshotIdsRecursively(d->invisibleRootNode->children.at(i));
    QScriptDebuggerCommandSchedulerFrontend frontend(d->commandScheduler, 0);
    for (int j = 0; j < snapshotIds.size(); ++j)
        frontend.scheduleDeleteScriptObjectSnapshot(snapshotIds.at(j));
}

QModelIndex QScriptDebuggerLocalsModelPrivate::addTopLevelObject(const QString &name, const QScriptDebuggerValue &object)
{
    Q_Q(QScriptDebuggerLocalsModel);
    QScriptDebuggerLocalsModelNode *node = invisibleRootNode->findChild(name);
    if (node)
        return indexFromNode(node);
    QScriptDebuggerValueProperty prop(name, object,
                                      QString::fromLatin1(""), // ### string representation of object
                                      /*flags=*/0);
    int rowIndex = invisibleRootNode->children.size();
    q->beginInsertRows(QModelIndex(), rowIndex, rowIndex);
    node = new QScriptDebuggerLocalsModelNode(prop, invisibleRootNode);
    q->endInsertRows();
    return indexFromNode(node);
}

namespace {

class InitModelJob : public QScriptDebuggerCommandSchedulerJob
{
public:
    InitModelJob(QScriptDebuggerLocalsModelPrivate *model,
                 int frameIndex,
                 QScriptDebuggerCommandSchedulerInterface *scheduler)
        : QScriptDebuggerCommandSchedulerJob(scheduler),
          m_model(model), m_frameIndex(frameIndex), m_state(0)
    { }

    void start()
    {
        QScriptDebuggerCommandSchedulerFrontend frontend(commandScheduler(), this);
        frontend.scheduleGetScopeChain(m_frameIndex);
    }

    void handleResponse(const QScriptDebuggerResponse &response,
                        int)
    {
        QScriptDebuggerCommandSchedulerFrontend frontend(commandScheduler(), this);
        switch (m_state) {
        case 0: {
            QScriptDebuggerValueList scopeChain = response.resultAsScriptValueList();
            for (int i = 0; i < scopeChain.size(); ++i) {
                const QScriptDebuggerValue &scopeObject = scopeChain.at(i);
                QString name = QString::fromLatin1("Scope");
                if (i > 0)
                    name.append(QString::fromLatin1(" (%0)").arg(i));
                QModelIndex index = m_model->addTopLevelObject(name, scopeObject);
                if (i == 0)
                    m_model->emitScopeObjectAvailable(index);
            }
            frontend.scheduleGetThisObject(m_frameIndex);
            ++m_state;
        }   break;
        case 1: {
            QScriptDebuggerValue thisObject = response.resultAsScriptValue();
            m_model->addTopLevelObject(QLatin1String("this"), thisObject);
            finish();
          } break;
        }
    }

private:
    QScriptDebuggerLocalsModelPrivate *m_model;
    int m_frameIndex;
    int m_state;
};

} // namespace

void QScriptDebuggerLocalsModel::init(int frameIndex)
{
    Q_D(QScriptDebuggerLocalsModel);
    d->frameIndex = frameIndex;
    QScriptDebuggerJob *job = new InitModelJob(d, frameIndex, d->commandScheduler);
    d->jobScheduler->scheduleJob(job);
}

namespace {

class SyncModelJob : public QScriptDebuggerCommandSchedulerJob
{
public:
    SyncModelJob(QScriptDebuggerLocalsModelPrivate *model,
                 int frameIndex,
                 QScriptDebuggerCommandSchedulerInterface *scheduler)
        : QScriptDebuggerCommandSchedulerJob(scheduler),
          m_model(model), m_frameIndex(frameIndex), m_state(0)
    { }

    void start()
    {
        QScriptDebuggerCommandSchedulerFrontend frontend(commandScheduler(), this);
        frontend.scheduleGetScopeChain(m_frameIndex);
    }

    void handleResponse(const QScriptDebuggerResponse &response,
                        int)
    {
        QScriptDebuggerCommandSchedulerFrontend frontend(commandScheduler(), this);
        switch (m_state) {
        case 0: {
            QScriptDebuggerValueList scopeChain = response.resultAsScriptValueList();
            m_topLevelObjects << scopeChain;
            frontend.scheduleGetThisObject(m_frameIndex);
            ++m_state;
        }   break;
        case 1: {
            QScriptDebuggerValue thisObject = response.resultAsScriptValue();
            m_topLevelObjects.append(thisObject);
            bool equal = (m_topLevelObjects.size() == m_model->invisibleRootNode->children.size());
            for (int i = 0; equal && (i < m_topLevelObjects.size()); ++i) {
                const QScriptDebuggerValue &object = m_topLevelObjects.at(i);
                equal = (object == m_model->invisibleRootNode->children.at(i)->property.value());
            }
            if (!equal) {
                // the scope chain and/or this-object changed, so invalidate the model.
                // we could try to be more clever, i.e. figure out
                // exactly which objects were popped/pushed
                m_model->removeTopLevelNodes();
                for (int j = 0; j < m_topLevelObjects.size(); ++j) {
                    const QScriptDebuggerValue &object = m_topLevelObjects.at(j);
                    QString name;
                    if (j == m_topLevelObjects.size()-1) {
                        name = QString::fromLatin1("this");
                    } else {
                        name = QString::fromLatin1("Scope");
                        if (j > 0)
                            name.append(QString::fromLatin1(" (%0)").arg(j));
                    }
                    QModelIndex index = m_model->addTopLevelObject(name, object);
                    if (j == 0)
                        m_model->emitScopeObjectAvailable(index);
                }
            } else {
                m_model->syncTopLevelNodes();
            }
            finish();
          } break;
        }
    }

private:
    QScriptDebuggerLocalsModelPrivate *m_model;
    int m_frameIndex;
    int m_state;
    QScriptDebuggerValueList m_topLevelObjects;
};

} // namespace

void QScriptDebuggerLocalsModel::sync(int frameIndex)
{
    Q_D(QScriptDebuggerLocalsModel);
    d->frameIndex = frameIndex;
    QScriptDebuggerJob *job = new SyncModelJob(d, frameIndex, d->commandScheduler);
    d->jobScheduler->scheduleJob(job);
}

namespace {

class SyncModelIndexJob : public QScriptDebuggerCommandSchedulerJob
{
public:
    SyncModelIndexJob(const QPersistentModelIndex &index,
                      QScriptDebuggerCommandSchedulerInterface *scheduler)
        : QScriptDebuggerCommandSchedulerJob(scheduler),
          m_index(index)
    { }

    QScriptDebuggerLocalsModelPrivate *model() const
    {
        if (!m_index.isValid())
            return 0;
        QAbstractItemModel *m = const_cast<QAbstractItemModel*>(m_index.model());
        QScriptDebuggerLocalsModel *lm = qobject_cast<QScriptDebuggerLocalsModel*>(m);
        return QScriptDebuggerLocalsModelPrivate::get(lm);
    }

    void start()
    {
        if (!m_index.isValid()) {
            // nothing to do, the node has been removed
            return;
        }
        QScriptDebuggerCommandSchedulerFrontend frontend(commandScheduler(), this);
        QScriptDebuggerLocalsModelNode *node = model()->nodeFromIndex(m_index);
        frontend.scheduleScriptObjectSnapshotCapture(node->snapshotId, node->property.value());
    }

    void handleResponse(const QScriptDebuggerResponse &response,
                        int)
    {
        QScriptDebuggerObjectSnapshotDelta delta;
        delta = qvariant_cast<QScriptDebuggerObjectSnapshotDelta>(response.result());
        model()->reallySyncIndex(m_index, delta);
        finish();
    }

private:
    QPersistentModelIndex m_index;
};

} // namespace

void QScriptDebuggerLocalsModelPrivate::syncIndex(const QModelIndex &index)
{
    if (!index.isValid())
        return;
    QScriptDebuggerLocalsModelNode *node = nodeFromIndex(index);
    if (node->populationState != QScriptDebuggerLocalsModelNode::Populated)
        return;
    QScriptDebuggerJob *job = new SyncModelIndexJob(index, commandScheduler);
    jobScheduler->scheduleJob(job);
}

void QScriptDebuggerLocalsModelPrivate::reallySyncIndex(const QModelIndex &index,
                                                        const QScriptDebuggerObjectSnapshotDelta &delta)
{
    if (!index.isValid())
        return;
    QScriptDebuggerLocalsModelNode *node = nodeFromIndex(index);
    // update or remove existing children
    for (int i = 0; i < node->children.count(); ++i) {
        QScriptDebuggerLocalsModelNode *child = node->children.at(i);
        int j;
        for (j = 0; j < delta.changedProperties.count(); ++j) {
            if (child->property.name() == delta.changedProperties.at(j).name()) {
                child->property = delta.changedProperties.at(j);
                child->changed = true;
                emitDataChanged(index, index.sibling(0, 1));
                repopulate(child);
                break;
            }
        }
        if (j != delta.changedProperties.count())
            continue; // was changed
        for (j = 0; j < delta.removedProperties.count(); ++j) {
            if (child->property.name() == delta.removedProperties.at(j)) {
                removeChild(index, node, i);
                --i;
                break;
            }
        }
        if (j != delta.removedProperties.count())
            continue; // was removed
        // neither changed nor removed, but its children might be
        if (child->populationState == QScriptDebuggerLocalsModelNode::Populated) {
            QScriptDebuggerJob *job = new SyncModelIndexJob(indexFromNode(child), commandScheduler);
            jobScheduler->scheduleJob(job);
        }
    }
    addChildren(index, node, delta.addedProperties);
}

void QScriptDebuggerLocalsModelPrivate::syncTopLevelNodes()
{
    Q_Q(QScriptDebuggerLocalsModel);
    for (int i = 0; i < invisibleRootNode->children.count(); ++i) {
        QModelIndex index = q->index(i, 0, QModelIndex());
        syncIndex(index);
        if (i == 0)
            emit q->scopeObjectAvailable(index);
    }
}

void QScriptDebuggerLocalsModelPrivate::removeTopLevelNodes()
{
    while (!invisibleRootNode->children.isEmpty())
        removeChild(QModelIndex(), invisibleRootNode, 0);
}

void QScriptDebuggerLocalsModelPrivate::emitScopeObjectAvailable(const QModelIndex &index)
{
    emit q_func()->scopeObjectAvailable(index);
}

int QScriptDebuggerLocalsModel::frameIndex() const
{
    Q_D(const QScriptDebuggerLocalsModel);
    return d->frameIndex;
}

/*!
  \reimp
*/
QModelIndex QScriptDebuggerLocalsModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_D(const QScriptDebuggerLocalsModel);
    QScriptDebuggerLocalsModelNode *node = d->nodeFromIndex(parent);
    if ((row < 0) || (row >= node->children.count()))
        return QModelIndex();
    return createIndex(row, column, node->children.at(row));
}

/*!
  \reimp
*/
QModelIndex QScriptDebuggerLocalsModel::parent(const QModelIndex &index) const
{
    Q_D(const QScriptDebuggerLocalsModel);
    if (!index.isValid())
        return QModelIndex();
    QScriptDebuggerLocalsModelNode *node = d->nodeFromIndex(index);
    return d->indexFromNode(node->parent);
}

/*!
  \reimp
*/
int QScriptDebuggerLocalsModel::columnCount(const QModelIndex &) const
{
    return 2;
}

/*!
  \reimp
*/
int QScriptDebuggerLocalsModel::rowCount(const QModelIndex &parent) const
{
    Q_D(const QScriptDebuggerLocalsModel);
    // ### need this to make it work with a sortfilterproxymodel (QSFPM is too eager)
    const_cast<QScriptDebuggerLocalsModel*>(this)->fetchMore(parent);
    QScriptDebuggerLocalsModelNode *node = d->nodeFromIndex(parent);
    return node ? node->children.count() : 0;
}

/*!
  \reimp
*/
QVariant QScriptDebuggerLocalsModel::data(const QModelIndex &index, int role) const
{
    Q_D(const QScriptDebuggerLocalsModel);
    if (!index.isValid())
        return QVariant();
    QScriptDebuggerLocalsModelNode *node = d->nodeFromIndex(index);
    if (role == Qt::DisplayRole) {
        if (index.column() == 0)
            return node->property.name();
        else if (index.column() == 1) {
            QString str = node->property.valueAsString();
            if (str.indexOf(QLatin1Char('\n')) != -1) {
                QStringList lines = str.split(QLatin1Char('\n'));
                int lineCount = lines.size();
                if (lineCount > 1) {
                    lines = lines.mid(0, 1);
                    lines.append(QString::fromLatin1("(... %0 more lines ...)").arg(lineCount - 1));
                }
                str = lines.join(QLatin1String("\n"));
            }
            return str;
        }
    } else if (role == Qt::EditRole) {
        if ((index.column() == 1) && !d->isTopLevelNode(node)) {
            QString str = node->property.valueAsString();
            if (node->property.value().type() == QScriptDebuggerValue::StringValue) {
                // escape
                str.replace(QLatin1String("\""), QLatin1String("\\\""));
                str.prepend(QLatin1Char('\"'));
                str.append(QLatin1Char('\"'));
            }
            return str;
        }
    } else if (role == Qt::ToolTipRole) {
        if (index.column() == 1) {
            QString str = node->property.valueAsString();
            if (str.indexOf(QLatin1Char('\n')) != -1)
                return str;
        }
    }
    // ### do this in the delegate
    else if (role == Qt::BackgroundRole) {
        if (d->isTopLevelNode(node))
            return QBrush(Qt::darkGray);
    } else if (role == Qt::TextColorRole) {
        if (d->isTopLevelNode(node))
            return QColor(Qt::white);
    } else if (role == Qt::FontRole) {
        if (d->isTopLevelNode(node) || node->changed) {
            QFont fnt;
            fnt.setBold(true);
            return fnt;
        }
    }
    return QVariant();
}

/*!
  \reimp
*/
bool QScriptDebuggerLocalsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_D(QScriptDebuggerLocalsModel);
    if (!index.isValid())
        return false;
    if (role != Qt::EditRole)
        return false;
    QScriptDebuggerLocalsModelNode *node = d->nodeFromIndex(index);
    if (!node)
        return false;
    QString expr = value.toString().trimmed();
    if (expr.isEmpty())
        return false;
    QScriptDebuggerJob *job = new SetPropertyJob(index, expr, d->commandScheduler);
    d->jobScheduler->scheduleJob(job);
    return true;
}

/*!
  \reimp
*/
QVariant QScriptDebuggerLocalsModel::headerData(int section, Qt::Orientation orient, int role) const
{
    if (orient == Qt::Horizontal) {
        if (role == Qt::DisplayRole) {
            if (section == 0)
                return QObject::tr("Name");
            else if (section == 1)
                return QObject::tr("Value");
        }
    }
    return QVariant();
}

/*!
  \reimp
*/
Qt::ItemFlags QScriptDebuggerLocalsModel::flags(const QModelIndex &index) const
{
    Q_D(const QScriptDebuggerLocalsModel);
    if (!index.isValid())
        return 0;
    Qt::ItemFlags ret = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    if ((index.column() == 1) && index.parent().isValid()) {
        QScriptDebuggerLocalsModelNode *node = d->nodeFromIndex(index);
        if (!(node->property.flags() & QScriptValue::ReadOnly))
            ret |= Qt::ItemIsEditable;
    }
    return ret;
}

/*!
  \reimp
*/
bool QScriptDebuggerLocalsModel::hasChildren(const QModelIndex &parent) const
{
    Q_D(const QScriptDebuggerLocalsModel);
    QScriptDebuggerLocalsModelNode *node = d->nodeFromIndex(parent);
    if (!node)
        return false;
    return !node->children.isEmpty()
        || ((node->property.value().type() == QScriptDebuggerValue::ObjectValue)
            && (node->populationState == QScriptDebuggerLocalsModelNode::NotPopulated));
}

/*!
  \reimp
*/
bool QScriptDebuggerLocalsModel::canFetchMore(const QModelIndex &parent) const
{
    Q_D(const QScriptDebuggerLocalsModel);
    if (!parent.isValid())
        return false;
    QScriptDebuggerLocalsModelNode *node = d->nodeFromIndex(parent);
    return node
        && (node->property.value().type() == QScriptDebuggerValue::ObjectValue)
        && (node->populationState == QScriptDebuggerLocalsModelNode::NotPopulated);
}

/*!
  \reimp
*/
void QScriptDebuggerLocalsModel::fetchMore(const QModelIndex &parent)
{
    Q_D(QScriptDebuggerLocalsModel);
    d->populateIndex(parent);
}

QT_END_NAMESPACE
