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

#ifndef QSCRIPTDEBUGGERSCRIPTSMODEL_P_H
#define QSCRIPTDEBUGGERSCRIPTSMODEL_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtCore/qabstractitemmodel.h>
#include <QtCore/qpair.h>

#include "qscriptscriptdata_p.h"

QT_BEGIN_NAMESPACE

class QScriptDebuggerScriptsModelPrivate;
class Q_AUTOTEST_EXPORT QScriptDebuggerScriptsModel
    : public QAbstractItemModel
{
    Q_OBJECT
public:
    QScriptDebuggerScriptsModel(QObject *parent = 0);
    ~QScriptDebuggerScriptsModel();

    void removeScript(qint64 id);
    void addScript(qint64 id, const QScriptScriptData &data);
    void addExtraScriptInfo(
        qint64 id, const QMap<QString, int> &functionsInfo,
        const QSet<int> &executableLineNumbers);
    void commit();

    QScriptScriptData scriptData(qint64 id) const;
    QScriptScriptMap scripts() const;
    qint64 resolveScript(const QString &fileName) const;
    QSet<int> executableLineNumbers(qint64 scriptId) const;

    QModelIndex indexFromScriptId(qint64 id) const;
    qint64 scriptIdFromIndex(const QModelIndex &index) const;
    QPair<QString, int> scriptFunctionInfoFromIndex(const QModelIndex &index) const;

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &child) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

private:
    Q_DECLARE_PRIVATE(QScriptDebuggerScriptsModel)
    Q_DISABLE_COPY(QScriptDebuggerScriptsModel)
};

QT_END_NAMESPACE

#endif
