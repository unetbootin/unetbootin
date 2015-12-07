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

#include "qscriptdebuggerstackmodel_p.h"

#include "private/qabstractitemmodel_p.h"

#include <QtScript/qscriptcontextinfo.h>
#include <QtCore/qfileinfo.h>

QT_BEGIN_NAMESPACE

class QScriptDebuggerStackModelPrivate
    : public QAbstractItemModelPrivate
{
    Q_DECLARE_PUBLIC(QScriptDebuggerStackModel)
public:
    QScriptDebuggerStackModelPrivate();
    ~QScriptDebuggerStackModelPrivate();

    QList<QScriptContextInfo> contextInfos;
};

QScriptDebuggerStackModelPrivate::QScriptDebuggerStackModelPrivate()
{
}

QScriptDebuggerStackModelPrivate::~QScriptDebuggerStackModelPrivate()
{
}

QScriptDebuggerStackModel::QScriptDebuggerStackModel(QObject *parent)
    : QAbstractTableModel(*new QScriptDebuggerStackModelPrivate, parent)
{
}

QScriptDebuggerStackModel::~QScriptDebuggerStackModel()
{
}

QList<QScriptContextInfo> QScriptDebuggerStackModel::contextInfos() const
{
    Q_D(const QScriptDebuggerStackModel);
    return d->contextInfos;
}

void QScriptDebuggerStackModel::setContextInfos(const QList<QScriptContextInfo> &infos)
{
    Q_D(QScriptDebuggerStackModel);
    layoutAboutToBeChanged();
    d->contextInfos = infos;
    layoutChanged();
}

/*!
  \reimp
*/
int QScriptDebuggerStackModel::columnCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return 3;
    return 0;
}

/*!
  \reimp
*/
int QScriptDebuggerStackModel::rowCount(const QModelIndex &parent) const
{
    Q_D(const QScriptDebuggerStackModel);
    if (!parent.isValid())
        return d->contextInfos.count();
    return 0;
}

/*!
  \reimp
*/
QVariant QScriptDebuggerStackModel::data(const QModelIndex &index, int role) const
{
    Q_D(const QScriptDebuggerStackModel);
    if (!index.isValid())
        return QVariant();
    if (index.row() >= d->contextInfos.count())
        return QVariant();
    const QScriptContextInfo &info = d->contextInfos.at(index.row());
    if (role == Qt::DisplayRole) {
        if (index.column() == 0) {
            return index.row();
        } else if (index.column() == 1) {
            QString name = info.functionName();
            if (name.isEmpty())
                name = QString::fromLatin1("<anonymous>");
            return name;
        } else if (index.column() == 2) {
            if (info.lineNumber() == -1)
                return QString::fromLatin1("<native>");
            QString fn = QFileInfo(info.fileName()).fileName();
            if (fn.isEmpty())
                fn = QString::fromLatin1("<anonymous script, id=%0>").arg(info.scriptId());
            return QString::fromLatin1("%0:%1").arg(fn).arg(info.lineNumber());
        }
    } else if (role == Qt::ToolTipRole) {
        if (QFileInfo(info.fileName()).fileName() != info.fileName())
            return info.fileName();
    }
    return QVariant();
}

/*!
  \reimp
*/
QVariant QScriptDebuggerStackModel::headerData(int section, Qt::Orientation orient, int role) const
{
    if (orient != Qt::Horizontal)
        return QVariant();
    if (role == Qt::DisplayRole) {
        if (section == 0)
            return QObject::tr("Level");
        else if (section == 1)
            return QObject::tr("Name");
        else if (section == 2)
            return QObject::tr("Location");
    }
    return QVariant();
}

QT_END_NAMESPACE
