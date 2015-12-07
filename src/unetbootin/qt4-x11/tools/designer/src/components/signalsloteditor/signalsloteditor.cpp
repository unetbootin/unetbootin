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

#include "signalsloteditor.h"
#include "signalsloteditor_p.h"
#include "connectdialog_p.h"
#include "signalslot_utils_p.h"

#include <metadatabase_p.h>
#include <ui4_p.h>

#include <QtDesigner/QDesignerFormWindowInterface>
#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QDesignerMetaDataBaseInterface>

#include <QtGui/QApplication>
#include <QtGui/QUndoCommand>
#include <QtGui/QMenu>

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>

QT_BEGIN_NAMESPACE

namespace qdesigner_internal {

/*******************************************************************************
** SignalSlotConnection
*/

SignalSlotConnection::SignalSlotConnection(ConnectionEdit *edit, QWidget *source, QWidget *target)
    : Connection(edit, source, target)
{
}

DomConnection *SignalSlotConnection::toUi() const
{
    DomConnection *result = new DomConnection;

    result->setElementSender(sender());
    result->setElementSignal(signal());
    result->setElementReceiver(receiver());
    result->setElementSlot(slot());

    DomConnectionHints *hints = new DomConnectionHints;
    QList<DomConnectionHint*> list;

    QPoint sp = endPointPos(EndPoint::Source);
    QPoint tp = endPointPos(EndPoint::Target);

    DomConnectionHint *hint = new DomConnectionHint;
    hint->setAttributeType(QLatin1String("sourcelabel"));
    hint->setElementX(sp.x());
    hint->setElementY(sp.y());
    list.append(hint);

    hint = new DomConnectionHint;
    hint->setAttributeType(QLatin1String("destinationlabel"));
    hint->setElementX(tp.x());
    hint->setElementY(tp.y());
    list.append(hint);

    hints->setElementHint(list);
    result->setElementHints(hints);

    return result;
}

void SignalSlotConnection::setSignal(const QString &signal)
{
    m_signal = signal;
    setLabel(EndPoint::Source, m_signal);
}

void SignalSlotConnection::setSlot(const QString &slot)
{
    m_slot = slot;
    setLabel(EndPoint::Target, m_slot);
}

QString SignalSlotConnection::sender() const
{
    QObject *source = object(EndPoint::Source);
    if (!source)
        return QString();

    SignalSlotEditor *edit = qobject_cast<SignalSlotEditor*>(this->edit());
    Q_ASSERT(edit != 0);

    return realObjectName(edit->formWindow()->core(), source);
}

QString SignalSlotConnection::receiver() const
{
    QObject *sink = object(EndPoint::Target);
    if (!sink)
        return QString();

    SignalSlotEditor *edit = qobject_cast<SignalSlotEditor*>(this->edit());
    Q_ASSERT(edit != 0);
    return realObjectName(edit->formWindow()->core(), sink);
}

void SignalSlotConnection::updateVisibility()
{
    Connection::updateVisibility();
    if (isVisible() && (signal().isEmpty() || slot().isEmpty()))
        setVisible(false);
}

QString SignalSlotConnection::toString() const
{
    return QCoreApplication::translate("SignalSlotConnection", "SENDER(%1), SIGNAL(%2), RECEIVER(%3), SLOT(%4)")
        .arg(sender()).arg(signal()).arg(receiver()).arg(slot());
}

SignalSlotConnection::State SignalSlotConnection::isValid(const QWidget *background) const
{
    const QObject *source = object(EndPoint::Source);
    if (!source)
        return ObjectDeleted;

    const QObject *target = object(EndPoint::Target);
    if (!target)
        return ObjectDeleted;

    if (m_slot.isEmpty() || m_signal.isEmpty())
        return InvalidMethod;

    if (const QWidget *sourceWidget = qobject_cast<const QWidget*>(source))
        if (!background->isAncestorOf(sourceWidget))
            return NotAncestor;

    if (const QWidget *targetWidget = qobject_cast<const QWidget*>(target))
        if (!background->isAncestorOf(targetWidget))
             return NotAncestor;

    return Valid;
}

/*******************************************************************************
** Commands
*/

class SetMemberCommand : public QUndoCommand, public CETypes
{
public:
    SetMemberCommand(SignalSlotConnection *con, EndPoint::Type type,
                        const QString &member, SignalSlotEditor *editor);
    virtual void redo();
    virtual void undo();
private:
    const QString m_old_member;
    const QString m_new_member;
    const EndPoint::Type m_type;
    SignalSlotConnection *m_con;
    SignalSlotEditor *m_editor;
};

SetMemberCommand::SetMemberCommand(SignalSlotConnection *con, EndPoint::Type type,
                                   const QString &member, SignalSlotEditor *editor) :
    m_old_member(type == EndPoint::Source ? con->signal() : con->slot()),
    m_new_member(member),
    m_type(type),
    m_con(con),
    m_editor(editor)
{
    if (type == EndPoint::Source)
        setText(QApplication::translate("Command", "Change signal"));
    else
        setText(QApplication::translate("Command", "Change slot"));
}

void SetMemberCommand::redo()
{
    m_con->update();
    if (m_type == EndPoint::Source)
        m_con->setSignal(m_new_member);
    else
        m_con->setSlot(m_new_member);
    m_con->update();
    emit m_editor->connectionChanged(m_con);
}

void SetMemberCommand::undo()
{
    m_con->update();
    if (m_type == EndPoint::Source)
        m_con->setSignal(m_old_member);
    else
        m_con->setSlot(m_old_member);
    m_con->update();
    emit m_editor->connectionChanged(m_con);
}

/*******************************************************************************
** SignalSlotEditor
*/

SignalSlotEditor::SignalSlotEditor(QDesignerFormWindowInterface *form_window, QWidget *parent) :
     ConnectionEdit(parent, form_window),
     m_form_window(form_window),
     m_showAllSignalsSlots(false)
{
}

void SignalSlotEditor::modifyConnection(Connection *con)
{
    SignalSlotConnection *sigslot_con = static_cast<SignalSlotConnection*>(con);

    ConnectDialog dialog(m_form_window,
                         sigslot_con->widget(EndPoint::Source),
                         sigslot_con->widget(EndPoint::Target),
                         m_form_window->core()->topLevel());

    dialog.setSignalSlot(sigslot_con->signal(), sigslot_con->slot());
    dialog.setShowAllSignalsSlots(m_showAllSignalsSlots);

    if (dialog.exec() == QDialog::Accepted) {
        sigslot_con->setSignal(dialog.signal());
        sigslot_con->setSlot(dialog.slot());
    }

    m_showAllSignalsSlots = dialog.showAllSignalsSlots();
}

Connection *SignalSlotEditor::createConnection(QWidget *source, QWidget *destination)
{
    SignalSlotConnection *con = 0;

    Q_ASSERT(source != 0);
    Q_ASSERT(destination != 0);

    ConnectDialog dialog(m_form_window, source, destination, m_form_window->core()->topLevel());
    dialog.setShowAllSignalsSlots(m_showAllSignalsSlots);

    if (dialog.exec() == QDialog::Accepted) {
        con = new SignalSlotConnection(this, source, destination);
        con->setSignal(dialog.signal());
        con->setSlot(dialog.slot());
    }

    m_showAllSignalsSlots = dialog.showAllSignalsSlots();

    return con;
}

DomConnections *SignalSlotEditor::toUi() const
{
    DomConnections *result = new DomConnections;
    QList<DomConnection*> list;

    const int count = connectionCount();
    for (int i = 0; i < count; ++i) {
        const SignalSlotConnection *con = static_cast<const SignalSlotConnection*>(connection(i));
        Q_ASSERT(con != 0);

        // If a widget's parent has been removed or moved to a different form,
        // and the parent was not a managed widget
        // (a page in a tab widget), we never get a widgetRemoved(). So we filter out
        // these child widgets here (check QPointer and verify ancestor).
        // Also, the user might demote a promoted widget or remove a fake
        // slot in the editor, which causes the connection to become invalid
        // once he doubleclicks on the method combo.
        switch (con->isValid(background())) {
        case SignalSlotConnection::Valid:
            list.append(con->toUi());
            break;
        case SignalSlotConnection::ObjectDeleted:
        case SignalSlotConnection::InvalidMethod:
        case SignalSlotConnection::NotAncestor:
            break;
        }
    }
    result->setElementConnection(list);
    return result;
}

QObject *SignalSlotEditor::objectByName(QWidget *topLevel, const QString &name) const
{
    if (name.isEmpty())
        return 0;

    Q_ASSERT(topLevel);
    QObject *object = 0;
    if (topLevel->objectName() == name)
        object = topLevel;
    else
        object = qFindChild<QObject*>(topLevel, name);
    const QDesignerMetaDataBaseInterface *mdb = formWindow()->core()->metaDataBase();
    if (mdb->item(object))
        return object;
    return 0;
}

void SignalSlotEditor::fromUi(const DomConnections *connections, QWidget *parent)
{
    if (connections == 0)
        return;

    setBackground(parent);
    clear();
    const QList<DomConnection*> list = connections->elementConnection();
    foreach (const DomConnection *dom_con, list) {
        QObject *source = objectByName(parent, dom_con->elementSender());
        if (source == 0) {
            qDebug("SignalSlotEditor::fromUi(): no source widget called \"%s\"",
                        dom_con->elementSender().toUtf8().constData());
            continue;
        }
        QObject *destination = objectByName(parent, dom_con->elementReceiver());
        if (destination == 0) {
            qDebug("SignalSlotEditor::fromUi(): no destination widget called \"%s\"",
                        dom_con->elementReceiver().toUtf8().constData());
            continue;
        }

        QPoint sp = QPoint(20, 20), tp = QPoint(20, 20);
        const DomConnectionHints *dom_hints = dom_con->elementHints();
        if (dom_hints != 0) {
            QList<DomConnectionHint*> list = dom_hints->elementHint();
            foreach (DomConnectionHint *hint, list) {
                QString attr_type = hint->attributeType();
                QPoint p = QPoint(hint->elementX(), hint->elementY());
                if (attr_type == QLatin1String("sourcelabel"))
                    sp = p;
                else if (attr_type == QLatin1String("destinationlabel"))
                    tp = p;
            }
        }

        SignalSlotConnection *con = new SignalSlotConnection(this);

        con->setEndPoint(EndPoint::Source, source, sp);
        con->setEndPoint(EndPoint::Target, destination, tp);
        con->setSignal(dom_con->elementSignal());
        con->setSlot(dom_con->elementSlot());
        addConnection(con);
    }
}

static bool skipWidget(const QWidget *w)
{
    const QString name = QLatin1String(w->metaObject()->className());
    if (name == QLatin1String("QDesignerWidget"))
        return true;
    if (name == QLatin1String("QLayoutWidget"))
        return true;
    if (name == QLatin1String("qdesigner_internal::FormWindow"))
        return true;
    if (name == QLatin1String("Spacer"))
        return true;
    return false;
}

QWidget *SignalSlotEditor::widgetAt(const QPoint &pos) const
{
    QWidget *widget = ConnectionEdit::widgetAt(pos);

    if (widget == m_form_window->mainContainer())
        return widget;

    for (; widget != 0; widget = widget->parentWidget()) {
        QDesignerMetaDataBaseItemInterface *item = m_form_window->core()->metaDataBase()->item(widget);
        if (item == 0)
            continue;
        if (skipWidget(widget))
            continue;
        break;
    }

    return widget;
}

void SignalSlotEditor::setSignal(SignalSlotConnection *con, const QString &member)
{
    if (member == con->signal())
        return;

    m_form_window->beginCommand(QApplication::translate("Command", "Change signal"));
    undoStack()->push(new SetMemberCommand(con, EndPoint::Source, member, this));
    if (!signalMatchesSlot(m_form_window->core(), member, con->slot()))
        undoStack()->push(new SetMemberCommand(con, EndPoint::Target, QString(), this));
    m_form_window->endCommand();
}

void SignalSlotEditor::setSlot(SignalSlotConnection *con, const QString &member)
{
    if (member == con->slot())
        return;

    m_form_window->beginCommand(QApplication::translate("Command", "Change slot"));
    undoStack()->push(new SetMemberCommand(con, EndPoint::Target, member, this));
    if (!signalMatchesSlot(m_form_window->core(), con->signal(), member))
        undoStack()->push(new SetMemberCommand(con, EndPoint::Source, QString(), this));
    m_form_window->endCommand();
}

void SignalSlotEditor::setSource(Connection *_con, const QString &obj_name)
{
    SignalSlotConnection *con = static_cast<SignalSlotConnection*>(_con);

   if (con->sender() == obj_name)
        return;

    m_form_window->beginCommand(QApplication::translate("Command", "Change sender"));
    ConnectionEdit::setSource(con, obj_name);

    QObject *sourceObject = con->object(EndPoint::Source);

    if (!memberFunctionListContains(m_form_window->core(), sourceObject, SignalMember, con->signal()))
        undoStack()->push(new SetMemberCommand(con, EndPoint::Source, QString(), this));

    m_form_window->endCommand();
}

void SignalSlotEditor::setTarget(Connection *_con, const QString &obj_name)
{
    SignalSlotConnection *con = static_cast<SignalSlotConnection*>(_con);

    if (con->receiver() == obj_name)
        return;

    m_form_window->beginCommand(QApplication::translate("Command", "Change receiver"));
    ConnectionEdit::setTarget(con, obj_name);

    QObject *targetObject = con->object(EndPoint::Target);
    if (!memberFunctionListContains(m_form_window->core(),  targetObject, SlotMember, con->slot()))
        undoStack()->push(new SetMemberCommand(con, EndPoint::Target, QString(), this));

    m_form_window->endCommand();
}

void SignalSlotEditor::addEmptyConnection()
{
    SignalSlotConnection *con = new SignalSlotConnection(this);
    undoStack()->push(new AddConnectionCommand(this, con));
}

} // namespace qdesigner_internal

QT_END_NAMESPACE
