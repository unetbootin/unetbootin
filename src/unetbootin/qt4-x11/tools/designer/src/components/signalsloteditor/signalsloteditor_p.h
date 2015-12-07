/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt Designer of the Qt Toolkit.
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

#ifndef SIGNALSLOTEDITOR_P_H
#define SIGNALSLOTEDITOR_P_H

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

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QList>
#include <QtCore/QPointer>
#include <QtCore/QAbstractItemModel>

#include <connectionedit_p.h>

QT_BEGIN_NAMESPACE

class QDesignerFormWindowInterface;
class QDesignerFormEditorInterface;
class DomConnection;

namespace qdesigner_internal {

class SignalSlotEditor;

class SignalSlotConnection : public Connection
{
public:
    explicit SignalSlotConnection(ConnectionEdit *edit, QWidget *source = 0, QWidget *target = 0);

    void setSignal(const QString &signal);
    void setSlot(const QString &slot);

    QString sender() const;
    QString receiver() const;
    inline QString signal() const { return m_signal; }
    inline QString slot() const { return m_slot; }

    DomConnection *toUi() const;

    virtual void updateVisibility();

    enum State { Valid, ObjectDeleted, InvalidMethod, NotAncestor };
    State isValid(const QWidget *background) const;

    // format for messages, etc.
    QString toString() const;

private:
    QString m_signal, m_slot;
};

class ConnectionModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit ConnectionModel(QObject *parent = 0);
    void setEditor(SignalSlotEditor *editor = 0);

    virtual QModelIndex index(int row, int column,
                              const QModelIndex &parent = QModelIndex()) const;
    virtual QModelIndex parent(const QModelIndex &child) const;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual bool setData(const QModelIndex &index, const QVariant &data, int role = Qt::DisplayRole);
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation,
                                int role = Qt::DisplayRole) const;

    QModelIndex connectionToIndex(Connection *con) const;
    Connection *indexToConnection(const QModelIndex &index) const;
    void updateAll();

private slots:
    void connectionAdded(Connection *con);
    void connectionRemoved(int idx);
    void aboutToRemoveConnection(Connection *con);
    void aboutToAddConnection(int idx);
    void connectionChanged(Connection *con);

private:
    QPointer<SignalSlotEditor> m_editor;
};

} // namespace qdesigner_internal

QT_END_NAMESPACE

#endif // SIGNALSLOTEDITOR_P_H
