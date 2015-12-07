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

#include "qscriptbreakpointsmodel_p.h"
#include "qscriptdebuggerjobschedulerinterface_p.h"
#include "qscriptdebuggercommandschedulerjob_p.h"
#include "qscriptdebuggercommandschedulerfrontend_p.h"

#include "private/qabstractitemmodel_p.h"

#include <QtCore/qpair.h>
#include <QtGui/qicon.h>
#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

/*!
  \since 4.5
  \class QScriptBreakpointsModel
  \internal
*/

class QScriptBreakpointsModelPrivate
    : public QAbstractItemModelPrivate
{
    Q_DECLARE_PUBLIC(QScriptBreakpointsModel)
public:
    QScriptBreakpointsModelPrivate();
    ~QScriptBreakpointsModelPrivate();

    QScriptDebuggerJobSchedulerInterface *jobScheduler;
    QScriptDebuggerCommandSchedulerInterface *commandScheduler;
    QList<QPair<int, QScriptBreakpointData> > breakpoints;
};

QScriptBreakpointsModelPrivate::QScriptBreakpointsModelPrivate()
{
}

QScriptBreakpointsModelPrivate::~QScriptBreakpointsModelPrivate()
{
}

QScriptBreakpointsModel::QScriptBreakpointsModel(
    QScriptDebuggerJobSchedulerInterface *jobScheduler,
    QScriptDebuggerCommandSchedulerInterface *commandScheduler,
    QObject *parent)
    : QAbstractItemModel(*new QScriptBreakpointsModelPrivate, parent)
{
    Q_D(QScriptBreakpointsModel);
    d->jobScheduler = jobScheduler;
    d->commandScheduler = commandScheduler;
}

QScriptBreakpointsModel::~QScriptBreakpointsModel()
{
}

namespace
{

class SetBreakpointJob : public QScriptDebuggerCommandSchedulerJob
{
public:
    SetBreakpointJob(const QScriptBreakpointData &data,
                     QScriptDebuggerCommandSchedulerInterface *scheduler)
        : QScriptDebuggerCommandSchedulerJob(scheduler),
          m_data(data)
    { }

    void start()
    {
        QScriptDebuggerCommandSchedulerFrontend frontend(commandScheduler(), this);
        frontend.scheduleSetBreakpoint(m_data);
    }

    void handleResponse(const QScriptDebuggerResponse &, int)
    {
        finish();
    }

private:
    QScriptBreakpointData m_data;
};

} // namespace

/*!
  Sets a breakpoint defined by the given \a data.
  A new row will be inserted into the model if the breakpoint could be
  successfully set.
*/
void QScriptBreakpointsModel::setBreakpoint(const QScriptBreakpointData &data)
{
    Q_D(QScriptBreakpointsModel);
    QScriptDebuggerJob *job = new SetBreakpointJob(data, d->commandScheduler);
    d->jobScheduler->scheduleJob(job);
}

namespace
{

class SetBreakpointDataJob : public QScriptDebuggerCommandSchedulerJob
{
public:
    SetBreakpointDataJob(int id, const QScriptBreakpointData &data,
                         QScriptDebuggerCommandSchedulerInterface *scheduler)
        : QScriptDebuggerCommandSchedulerJob(scheduler),
          m_id(id), m_data(data)
    { }

    void start()
    {
        QScriptDebuggerCommandSchedulerFrontend frontend(commandScheduler(), this);
        frontend.scheduleSetBreakpointData(m_id, m_data);
    }

    void handleResponse(const QScriptDebuggerResponse &, int)
    {
        finish();
    }

private:
    int m_id;
    QScriptBreakpointData m_data;
};

} // namespace

/*!
  Sets the \a data associated with the breakpoint identified by \a id.
  A dataChanged() signal will be emitted if the breakpoint data could
  be successfully changed.
*/
void QScriptBreakpointsModel::setBreakpointData(int id, const QScriptBreakpointData &data)
{
    Q_D(QScriptBreakpointsModel);
    QScriptDebuggerJob *job = new SetBreakpointDataJob(id, data, d->commandScheduler);
    d->jobScheduler->scheduleJob(job);
}

namespace
{

class DeleteBreakpointJob : public QScriptDebuggerCommandSchedulerJob
{
public:
    DeleteBreakpointJob(int id, QScriptDebuggerCommandSchedulerInterface *scheduler)
        : QScriptDebuggerCommandSchedulerJob(scheduler),
          m_id(id)
    { }

    void start()
    {
        QScriptDebuggerCommandSchedulerFrontend frontend(commandScheduler(), this);
        frontend.scheduleDeleteBreakpoint(m_id);
    }

    void handleResponse(const QScriptDebuggerResponse &, int)
    {
        finish();
    }

private:
    int m_id;
};

} // namespace

/*!
  Deletes the breakpoint with the given \a id.
  The corresponding row in the model will be removed if the breakpoint
  was successfully deleted.
*/
void QScriptBreakpointsModel::deleteBreakpoint(int id)
{
    Q_D(QScriptBreakpointsModel);
    QScriptDebuggerJob *job = new DeleteBreakpointJob(id, d->commandScheduler);
    d->jobScheduler->scheduleJob(job);
}

/*!
  Adds a breakpoint to the model. This function does not actually set
  a breakpoint (i.e. it doesn't communicate with the debugger).
*/
void QScriptBreakpointsModel::addBreakpoint(int id, const QScriptBreakpointData &data)
{
    Q_D(QScriptBreakpointsModel);
    int rowIndex = d->breakpoints.size();
    beginInsertRows(QModelIndex(), rowIndex, rowIndex);
    d->breakpoints.append(qMakePair(id, data));
    endInsertRows();
}

/*!
  Modify the \a data of breakpoint \a id.
*/
void QScriptBreakpointsModel::modifyBreakpoint(int id, const QScriptBreakpointData &data)
{
    Q_D(QScriptBreakpointsModel);
    for (int i = 0; i < d->breakpoints.size(); ++i) {
        if (d->breakpoints.at(i).first == id) {
            d->breakpoints[i] = qMakePair(id, data);
            emit dataChanged(createIndex(i, 0), createIndex(i, columnCount()-1));
            break;
        }
    }
}

/*!
  Remove the breakpoint identified by \a id from the model. This
  function does not delete the breakpoint (i.e. it doesn't communicate
  with the debugger).
*/
void QScriptBreakpointsModel::removeBreakpoint(int id)
{
    Q_D(QScriptBreakpointsModel);
    for (int i = 0; i < d->breakpoints.size(); ++i) {
        if (d->breakpoints.at(i).first == id) {
            beginRemoveRows(QModelIndex(), i, i);
            d->breakpoints.removeAt(i);
            endRemoveRows();
            break;
        }
    }
}

/*!
  Returns the id of the breakpoint at the given \a row.
*/
int QScriptBreakpointsModel::breakpointIdAt(int row) const
{
    Q_D(const QScriptBreakpointsModel);
    return d->breakpoints.at(row).first;
}

/*!
  Returns the data for the breakpoint at the given \a row.
*/
QScriptBreakpointData QScriptBreakpointsModel::breakpointDataAt(int row) const
{
    Q_D(const QScriptBreakpointsModel);
    return d->breakpoints.at(row).second;
}

QScriptBreakpointData QScriptBreakpointsModel::breakpointData(int id) const
{
    Q_D(const QScriptBreakpointsModel);
    for (int i = 0; i < d->breakpoints.size(); ++i) {
        if (d->breakpoints.at(i).first == id)
            return d->breakpoints.at(i).second;
    }
    return QScriptBreakpointData();
}

/*!
  Tries to find a breakpoint with the given \a scriptId and \a
  lineNumber. Returns the id of the first breakpoint that matches, or
  -1 if no such breakpoint is found.
*/
int QScriptBreakpointsModel::resolveBreakpoint(qint64 scriptId, int lineNumber) const
{
    Q_D(const QScriptBreakpointsModel);
    for (int i = 0; i < d->breakpoints.size(); ++i) {
        if ((d->breakpoints.at(i).second.scriptId() == scriptId)
            && (d->breakpoints.at(i).second.lineNumber() == lineNumber)) {
            return d->breakpoints.at(i).first;
        }
    }
    return -1;
}

int QScriptBreakpointsModel::resolveBreakpoint(const QString &fileName, int lineNumber) const
{
    Q_D(const QScriptBreakpointsModel);
    for (int i = 0; i < d->breakpoints.size(); ++i) {
        if ((d->breakpoints.at(i).second.fileName() == fileName)
            && (d->breakpoints.at(i).second.lineNumber() == lineNumber)) {
            return d->breakpoints.at(i).first;
        }
    }
    return -1;
}

/*!
  \reimp
*/
QModelIndex QScriptBreakpointsModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_D(const QScriptBreakpointsModel);
    if (parent.isValid())
        return QModelIndex();
    if ((row < 0) || (row >= d->breakpoints.size()))
        return QModelIndex();
    if ((column < 0) || (column >= columnCount()))
        return QModelIndex();
    return createIndex(row, column);
}

/*!
  \reimp
*/
QModelIndex QScriptBreakpointsModel::parent(const QModelIndex &) const
{
    return QModelIndex();
}

/*!
  \reimp
*/
int QScriptBreakpointsModel::columnCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return 6;
    return 0;
}

/*!
  \reimp
*/
int QScriptBreakpointsModel::rowCount(const QModelIndex &parent) const
{
    Q_D(const QScriptBreakpointsModel);
    if (!parent.isValid())
        return d->breakpoints.size();
    return 0;
}

/*!
  \reimp
*/
QVariant QScriptBreakpointsModel::data(const QModelIndex &index, int role) const
{
    Q_D(const QScriptBreakpointsModel);
    if (!index.isValid() || (index.row() >= d->breakpoints.size()))
        return QVariant();
    const QPair<int, QScriptBreakpointData> &item = d->breakpoints.at(index.row());
    if (role == Qt::DisplayRole) {
        if (index.column() == 0)
            return item.first;
        else if (index.column() == 1) {
            QString loc = item.second.fileName();
            if (loc.isEmpty())
                loc = QString::fromLatin1("<anonymous script, id=%0>").arg(item.second.scriptId());
            loc.append(QString::fromLatin1(":%0").arg(item.second.lineNumber()));
            return loc;
        } else if (index.column() == 2) {
            if (!item.second.condition().isEmpty())
                return item.second.condition();
        } else if (index.column() == 3) {
            if (item.second.ignoreCount() != 0)
                return item.second.ignoreCount();
        } else if (index.column() == 5) {
            return item.second.hitCount();
        }
    } else if (role == Qt::CheckStateRole) {
        if (index.column() == 0) {
            return item.second.isEnabled() ? Qt::Checked : Qt::Unchecked;
        } else if (index.column() == 4) {
            return item.second.isSingleShot() ? Qt::Checked : Qt::Unchecked;
        }
    } else if (role == Qt::EditRole) {
        if (index.column() == 2)
            return item.second.condition();
        else if (index.column() == 3)
            return item.second.ignoreCount();
    }
    return QVariant();
}

/*!
  \reimp
*/
bool QScriptBreakpointsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_D(QScriptBreakpointsModel);
    if (!index.isValid() || (index.row() >= d->breakpoints.size()))
        return false;
    const QPair<int, QScriptBreakpointData> &item = d->breakpoints.at(index.row());
    QScriptBreakpointData modifiedData;
    int col = index.column();
    if ((col == 0) || (col == 4)) {
        if (role == Qt::CheckStateRole) {
            modifiedData = item.second;
            if (col == 0)
                modifiedData.setEnabled(value.toInt() == Qt::Checked);
            else
                modifiedData.setSingleShot(value.toInt() == Qt::Checked);
        }
    } else if (col == 2) {
        if (role == Qt::EditRole) {
            modifiedData = item.second;
            modifiedData.setCondition(value.toString());
        }
    } else if (col == 3) {
        if (role == Qt::EditRole) {
            modifiedData = item.second;
            modifiedData.setIgnoreCount(value.toInt());
        }
    }
    if (!modifiedData.isValid())
        return false;
    QScriptDebuggerJob *job = new SetBreakpointDataJob(item.first, modifiedData, d->commandScheduler);
    d->jobScheduler->scheduleJob(job);
    return true;
}

/*!
  \reimp
*/
QVariant QScriptBreakpointsModel::headerData(int section, Qt::Orientation orient, int role) const
{
    if (orient == Qt::Horizontal) {
        if (role == Qt::DisplayRole) {
            if (section == 0)
                return QObject::tr("ID");
            else if (section == 1)
                return QObject::tr("Location");
            else if (section == 2)
                return QObject::tr("Condition");
            else if (section == 3)
                return QObject::tr("Ignore-count");
            else if (section == 4)
                return QObject::tr("Single-shot");
            else if (section == 5)
                return QObject::tr("Hit-count");
        }
    }
    return QVariant();
}

/*!
  \reimp
*/
Qt::ItemFlags QScriptBreakpointsModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;
    Qt::ItemFlags ret = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    switch (index.column()) {
    case 0:
        ret |= Qt::ItemIsUserCheckable;
        break;
    case 1:
        break;
    case 2:
        ret |= Qt::ItemIsEditable;
        break;
    case 3:
        ret |= Qt::ItemIsEditable;
        break;
    case 4:
        ret |= Qt::ItemIsUserCheckable;
        break;
    }
    return ret;
}

QT_END_NAMESPACE
