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
TRANSLATOR qdesigner_internal::ToolBarTaskMenu
*/

#include "toolbar_taskmenu.h"
#include "qdesigner_toolbar_p.h"

#include <QtDesigner/QDesignerFormWindowInterface>

#include <promotiontaskmenu_p.h>
#include <qdesigner_command_p.h>

#include <QtGui/QAction>
#include <QtGui/QUndoStack>

#include <QtCore/QDebug>

QT_BEGIN_NAMESPACE

namespace qdesigner_internal {
    // ------------ ToolBarTaskMenu
    ToolBarTaskMenu::ToolBarTaskMenu(QToolBar *tb, QObject *parent) :
        QObject(parent),
        m_toolBar(tb)
    {
    }

    QAction *ToolBarTaskMenu::preferredEditAction() const
    {
        return 0;
    }

    QList<QAction*> ToolBarTaskMenu::taskActions() const
    {
        if (ToolBarEventFilter *ef = ToolBarEventFilter::eventFilterOf(m_toolBar))
            return ef->contextMenuActions();
        return QList<QAction*>();
    }

    // ------------ StatusBarTaskMenu
    StatusBarTaskMenu::StatusBarTaskMenu(QStatusBar *sb, QObject *parent) :
        QObject(parent),
        m_statusBar(sb),
        m_removeAction(new QAction(tr("Remove"), this)),
        m_promotionTaskMenu(new PromotionTaskMenu(sb, PromotionTaskMenu::ModeSingleWidget, this))
    {
        connect(m_removeAction, SIGNAL(triggered()), this, SLOT(removeStatusBar()));
    }

    QAction *StatusBarTaskMenu::preferredEditAction() const
    {
        return 0;
    }

    QList<QAction*> StatusBarTaskMenu::taskActions() const
    {
        QList<QAction*> rc;
        rc.push_back(m_removeAction);
        m_promotionTaskMenu->addActions(PromotionTaskMenu::LeadingSeparator, rc);
        return rc;
    }

    void StatusBarTaskMenu::removeStatusBar()
    {
        if (QDesignerFormWindowInterface *fw = QDesignerFormWindowInterface::findFormWindow(m_statusBar)) {
            DeleteStatusBarCommand *cmd = new DeleteStatusBarCommand(fw);
            cmd->init(m_statusBar);
            fw->commandHistory()->push(cmd);
        }
    }
}

QT_END_NAMESPACE

