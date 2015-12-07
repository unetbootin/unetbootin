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
TRANSLATOR qdesigner_internal::TableWidgetTaskMenu
*/

#include "tablewidget_taskmenu.h"
#include "tablewidgeteditor.h"

#include <QtDesigner/QDesignerFormWindowInterface>

#include <QtGui/QTableWidget>
#include <QtGui/QAction>
#include <QtGui/QLineEdit>
#include <QtGui/QStyle>
#include <QtGui/QStyleOption>

#include <QtCore/QEvent>
#include <QtCore/QVariant>
#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

using namespace qdesigner_internal;

TableWidgetTaskMenu::TableWidgetTaskMenu(QTableWidget *button, QObject *parent)
    : QDesignerTaskMenu(button, parent),
      m_tableWidget(button),
      m_editItemsAction(new QAction(tr("Edit Items..."), this))
{
    connect(m_editItemsAction, SIGNAL(triggered()), this, SLOT(editItems()));
    m_taskActions.append(m_editItemsAction);

    QAction *sep = new QAction(this);
    sep->setSeparator(true);
    m_taskActions.append(sep);
}


TableWidgetTaskMenu::~TableWidgetTaskMenu()
{
}

QAction *TableWidgetTaskMenu::preferredEditAction() const
{
    return m_editItemsAction;
}

QList<QAction*> TableWidgetTaskMenu::taskActions() const
{
    return m_taskActions + QDesignerTaskMenu::taskActions();
}

void TableWidgetTaskMenu::editItems()
{
    m_formWindow = QDesignerFormWindowInterface::findFormWindow(m_tableWidget);
    if (m_formWindow.isNull())
        return;

    Q_ASSERT(m_tableWidget != 0);

    TableWidgetEditor dlg(m_formWindow, m_tableWidget->window());
    TableWidgetContents oldCont = dlg.fillContentsFromTableWidget(m_tableWidget);
    if (dlg.exec() == QDialog::Accepted) {
        TableWidgetContents newCont = dlg.contents();
        if (newCont != oldCont) {
            ChangeTableContentsCommand *cmd = new ChangeTableContentsCommand(m_formWindow);
            cmd->init(m_tableWidget, oldCont, newCont);
            m_formWindow->commandHistory()->push(cmd);
        }
    }
}

void TableWidgetTaskMenu::updateSelection()
{
    if (m_editor)
        m_editor->deleteLater();
}

QT_END_NAMESPACE
