/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt Designer of the Qt Toolkit.
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
