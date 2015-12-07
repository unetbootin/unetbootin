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

/*
TRANSLATOR qdesigner_internal::BuddyEditor
*/

#include "buddyeditor.h"

#include <QtDesigner/QDesignerFormWindowInterface>
#include <QtDesigner/QDesignerPropertySheetExtension>
#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QExtensionManager>

#include <qdesigner_command_p.h>
#include <qdesigner_propertycommand_p.h>
#include <qdesigner_utils_p.h>
#include <qlayout_widget_p.h>
#include <connectionedit_p.h>

#include <QtCore/qdebug.h>
#include <QtGui/QLabel>
#include <QtGui/QMenu>
#include <QtGui/QAction>
#include <QtGui/QApplication>

QT_BEGIN_NAMESPACE

static const char *buddyPropertyC = "buddy";

static bool canBeBuddy(QWidget *w, QDesignerFormWindowInterface *form)
{
    if (qobject_cast<const QLayoutWidget*>(w) || qobject_cast<const QLabel*>(w))
        return false;
    if (w == form->mainContainer() || w->isHidden() )
        return false;

    QExtensionManager *ext = form->core()->extensionManager();
    if (QDesignerPropertySheetExtension *sheet = qt_extension<QDesignerPropertySheetExtension*>(ext, w)) {
        const int index = sheet->indexOf(QLatin1String("focusPolicy"));
        if (index != -1) {
            bool ok = false;
            const Qt::FocusPolicy q = static_cast<Qt::FocusPolicy>(qdesigner_internal::Utils::valueOf(sheet->property(index), &ok));
            return ok && q != Qt::NoFocus;
        }
    }
    return false;
}

static QString buddy(QLabel *label, QDesignerFormEditorInterface *core)
{
    QDesignerPropertySheetExtension *sheet = qt_extension<QDesignerPropertySheetExtension*>(core->extensionManager(), label);
    if (sheet == 0)
        return QString();
    const int prop_idx = sheet->indexOf(QLatin1String(buddyPropertyC));
    if (prop_idx == -1)
        return QString();
    return sheet->property(prop_idx).toString();
}

typedef QList<QLabel*> LabelList;

namespace qdesigner_internal {

/*******************************************************************************
** BuddyEditor
*/

BuddyEditor::BuddyEditor(QDesignerFormWindowInterface *form, QWidget *parent) :
    ConnectionEdit(parent, form),
    m_formWindow(form),
    m_updating(false)
{
}


QWidget *BuddyEditor::widgetAt(const QPoint &pos) const
{
    QWidget *w = ConnectionEdit::widgetAt(pos);

    while (w != 0 && !m_formWindow->isManaged(w))
        w = w->parentWidget();
    if (!w)
        return w;

    if (state() == Editing) {
        QLabel *label = qobject_cast<QLabel*>(w);
        if (label == 0)
            return 0;
        const int cnt = connectionCount();
        for (int i = 0; i < cnt; ++i) {
            Connection *con = connection(i);
            if (con->widget(EndPoint::Source) == w)
                return 0;
        }
    } else {
        if (!canBeBuddy(w, m_formWindow))
            return 0;
    }

    return w;
}

Connection *BuddyEditor::createConnection(QWidget *source, QWidget *destination)
{
    return new Connection(this, source, destination);
}

QDesignerFormWindowInterface *BuddyEditor::formWindow() const
{
    return m_formWindow;
}

void BuddyEditor::updateBackground()
{
    if (m_updating || background() == 0)
        return;
    ConnectionEdit::updateBackground();

    m_updating = true;
    QList<Connection *> newList;
    const LabelList label_list = qFindChildren<QLabel*>(background());
    foreach (QLabel *label, label_list) {
        const QString buddy_name = buddy(label, m_formWindow->core());
        if (buddy_name.isEmpty())
            continue;

        const QList<QWidget *> targets = qFindChildren<QWidget*>(background(), buddy_name);
        if (targets.isEmpty())
            continue;

        QWidget *target = 0;

        QListIterator<QWidget *> it(targets);
        while (it.hasNext()) {
            QWidget *widget = it.next();
            if (widget && !widget->isHidden()) {
                target = widget;
                break;
            }
        }

        if (target == 0)
            continue;

        Connection *con = new Connection(this);
        con->setEndPoint(EndPoint::Source, label, widgetRect(label).center());
        con->setEndPoint(EndPoint::Target, target, widgetRect(target).center());
        newList.append(con);
    }

    QList<Connection *> toRemove;

    const int c = connectionCount();
    for (int i = 0; i < c; i++) {
        Connection *con = connection(i);
        QObject *source = con->object(EndPoint::Source);
        QObject *target = con->object(EndPoint::Target);
        bool found = false;
        QListIterator<Connection *> it(newList);
        while (it.hasNext()) {
            Connection *newConn = it.next();
            if (newConn->object(EndPoint::Source) == source && newConn->object(EndPoint::Target) == target) {
                found = true;
                break;
            }
        }
        if (found == false)
            toRemove.append(con);
    }
    if (!toRemove.isEmpty()) {
        DeleteConnectionsCommand command(this, toRemove);
        command.redo();
        foreach (Connection *con, toRemove)
            delete takeConnection(con);
    }

    QListIterator<Connection *> it(newList);
    while (it.hasNext()) {
        Connection *newConn = it.next();

        bool found = false;
        const int c = connectionCount();
        for (int i = 0; i < c; i++) {
            Connection *con = connection(i);
            if (con->object(EndPoint::Source) == newConn->object(EndPoint::Source) &&
                            con->object(EndPoint::Target) == newConn->object(EndPoint::Target)) {
                found = true;
                break;
            }
        }
        if (found == false) {
            AddConnectionCommand command(this, newConn);
            command.redo();
        } else {
            delete newConn;
        }
    }
    m_updating = false;
}

void BuddyEditor::setBackground(QWidget *background)
{
    clear();
    ConnectionEdit::setBackground(background);

    const LabelList label_list = qFindChildren<QLabel*>(background);
    foreach (QLabel *label, label_list) {
        const QString buddy_name = buddy(label, m_formWindow->core());
        if (buddy_name.isEmpty())
            continue;
        QWidget *target = qFindChild<QWidget*>(background, buddy_name);
        if (target == 0)
            continue;

        Connection *con = new Connection(this);
        con->setEndPoint(EndPoint::Source, label, widgetRect(label).center());
        con->setEndPoint(EndPoint::Target, target, widgetRect(target).center());
        addConnection(con);
    }
}

static QUndoCommand *createBuddyCommand(QDesignerFormWindowInterface *fw, QLabel *label, QWidget *buddy)
{
    SetPropertyCommand *command = new SetPropertyCommand(fw);
    command->init(label, QLatin1String(buddyPropertyC), buddy->objectName());
    command->setText(BuddyEditor::tr("Add buddy"));
    return command;
}

void BuddyEditor::endConnection(QWidget *target, const QPoint &pos)
{
    Connection *tmp_con = newlyAddedConnection();
    Q_ASSERT(tmp_con != 0);

    tmp_con->setEndPoint(EndPoint::Target, target, pos);

    QWidget *source = tmp_con->widget(EndPoint::Source);
    Q_ASSERT(source != 0);
    Q_ASSERT(target != 0);
    setEnabled(false);
    Connection *new_con = createConnection(source, target);
    setEnabled(true);
    if (new_con != 0) {
        new_con->setEndPoint(EndPoint::Source, source, tmp_con->endPointPos(EndPoint::Source));
        new_con->setEndPoint(EndPoint::Target, target, tmp_con->endPointPos(EndPoint::Target));

        selectNone();
        addConnection(new_con);
        QLabel *source = qobject_cast<QLabel*>(new_con->widget(EndPoint::Source));
        QWidget *target = new_con->widget(EndPoint::Target);
        if (source) {
            undoStack()->push(createBuddyCommand(m_formWindow, source, target));
        } else {
            qDebug("BuddyEditor::endConnection(): not a label");
        }
        setSelected(new_con, true);
    }

    clearNewlyAddedConnection();
    findObjectsUnderMouse(mapFromGlobal(QCursor::pos()));
}

void BuddyEditor::widgetRemoved(QWidget *widget)
{
    QList<QWidget*> child_list = qFindChildren<QWidget*>(widget);
    child_list.prepend(widget);

    ConnectionSet remove_set;
    foreach (QWidget *w, child_list) {
        const ConnectionList &cl = connectionList();
        foreach (Connection *con, cl) {
            if (con->widget(EndPoint::Source) == w || con->widget(EndPoint::Target) == w)
                remove_set.insert(con, con);
        }
    }

    if (!remove_set.isEmpty()) {
        undoStack()->beginMacro(tr("Remove buddies"));
        foreach (Connection *con, remove_set) {
            setSelected(con, false);
            con->update();
            QWidget *source = con->widget(EndPoint::Source);
            if (qobject_cast<QLabel*>(source) == 0) {
                qDebug("BuddyConnection::widgetRemoved(): not a label");
            } else {
                ResetPropertyCommand *command = new ResetPropertyCommand(formWindow());
                command->init(source, QLatin1String(buddyPropertyC));
                undoStack()->push(command);
            }
            delete takeConnection(con);
        }
        undoStack()->endMacro();
    }
}

void BuddyEditor::deleteSelected()
{
    const ConnectionSet selectedConnections = selection(); // want copy for unselect
    if (selectedConnections.isEmpty())
        return;

    undoStack()->beginMacro(tr("Remove %n buddies", 0, selectedConnections.size()));
    foreach (Connection *con, selectedConnections) {
        setSelected(con, false);
        con->update();
        QWidget *source = con->widget(EndPoint::Source);
        if (qobject_cast<QLabel*>(source) == 0) {
            qDebug("BuddyConnection::deleteSelected(): not a label");
        } else {
            ResetPropertyCommand *command = new ResetPropertyCommand(formWindow());
            command->init(source, QLatin1String(buddyPropertyC));
            undoStack()->push(command);
        }
        delete takeConnection(con);
    }
    undoStack()->endMacro();
}

void BuddyEditor::autoBuddy()
{
    // Any labels?
    LabelList labelList = qFindChildren<QLabel*>(background());
    if (labelList.empty())
        return;
    // Find already used buddies
    QWidgetList usedBuddies;
    const ConnectionList &beforeConnections = connectionList();
    foreach (const Connection *c, beforeConnections)
        usedBuddies.push_back(c->widget(EndPoint::Target));
    // Find potential new buddies, keep lists in sync
    QWidgetList buddies;
    for (LabelList::iterator it = labelList.begin(); it != labelList.end(); ) {
        QLabel *label = *it;
        QWidget *newBuddy = 0;
        if (m_formWindow->isManaged(label)) {
            const QString buddy_name = buddy(label, m_formWindow->core());
            if (buddy_name.isEmpty())
                newBuddy = findBuddy(label, usedBuddies);
        }
        if (newBuddy) {
            buddies.push_back(newBuddy);
            usedBuddies.push_back(newBuddy);
            ++it;
        } else {
            it = labelList.erase(it);
        }
    }
    // Add the list in one go.
    if (labelList.empty())
        return;
    const int count = labelList.size();
    Q_ASSERT(count == buddies.size());
    undoStack()->beginMacro(tr("Add %n buddies", 0, count));
    for (int i = 0; i < count; i++)
        undoStack()->push(createBuddyCommand(m_formWindow, labelList.at(i), buddies.at(i)));
    undoStack()->endMacro();
    // Now select all new ones
    const ConnectionList &connections = connectionList();
    foreach (Connection *con, connections)
        setSelected(con, buddies.contains(con->widget(EndPoint::Target)));
}

// Geometrically find  a potential buddy for label by checking neighbouring children of parent
QWidget *BuddyEditor::findBuddy(QLabel *l, const QWidgetList &existingBuddies) const
{
    enum { DeltaX = 5 };
    const QWidget *parent = l->parentWidget();
    // Try to find next managed neighbour on horizontal line
    const QRect geom = l->geometry();
    const int y = geom.center().y();
    QWidget *neighbour = 0;
    switch (QApplication::layoutDirection()) {
    case Qt::LeftToRight: { // Walk right to find next managed neighbour
        const int xEnd = parent->size().width();
        for (int x = geom.right() + 1; x < xEnd; x += DeltaX)
            if (QWidget *c = parent->childAt (x, y))
                if (m_formWindow->isManaged(c)) {
                    neighbour = c;
                    break;
                }
    }
        break;
    case Qt::RightToLeft:  // Walk left to find next managed neighbour
        for (int x = geom.x() - 1; x >= 0; x -= DeltaX)
            if (QWidget *c = parent->childAt (x, y))
                if (m_formWindow->isManaged(c)) {
                    neighbour = c;
                    break;
                }
        break;
    }
    if (neighbour && !existingBuddies.contains(neighbour) && canBeBuddy(neighbour, m_formWindow))
        return neighbour;

    return 0;
}

void BuddyEditor::createContextMenu(QMenu &menu)
{
    QAction *autoAction = menu.addAction(tr("Set automatically"));
    connect(autoAction, SIGNAL(triggered()), this, SLOT(autoBuddy()));
    menu.addSeparator();
    ConnectionEdit::createContextMenu(menu);
}

}

QT_END_NAMESPACE
