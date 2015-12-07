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
TRANSLATOR qdesigner_internal::MenuTaskMenu
*/

#include "menutaskmenu.h"

#include <promotiontaskmenu_p.h>

#include <QtGui/QAction>
#include <QtCore/QDebug>

QT_BEGIN_NAMESPACE

namespace qdesigner_internal {
    // ------------ MenuTaskMenu
    MenuTaskMenu::MenuTaskMenu(QDesignerMenu *menu, QObject *parent) :
       QObject(parent),
       m_menu(menu),
       m_removeAction(new QAction(tr("Remove"), this)),
       m_promotionTaskMenu(new PromotionTaskMenu(menu, PromotionTaskMenu::ModeSingleWidget, this))
    {
        connect(m_removeAction, SIGNAL(triggered()), this, SLOT(removeMenu()));
    }

    QAction *MenuTaskMenu::preferredEditAction() const
    {
        return 0;
    }

    QList<QAction*> MenuTaskMenu::taskActions() const
    {
        QList<QAction*> rc;
        rc.push_back(m_removeAction);
        m_promotionTaskMenu->addActions(PromotionTaskMenu::LeadingSeparator, rc);
        return rc;
    }

    void MenuTaskMenu::removeMenu()
    {
        // Are we on a menu bar or on a menu?
        QWidget *pw = m_menu->parentWidget();
        if (QDesignerMenuBar *mb = qobject_cast<QDesignerMenuBar *>(pw)) {
            mb->deleteMenuAction(m_menu->menuAction());
            return;
        }
        if (QDesignerMenu *m = qobject_cast<QDesignerMenu *>(pw)) {
            m->deleteAction(m_menu->menuAction());
        }
    }

    // ------------- MenuBarTaskMenu
    MenuBarTaskMenu::MenuBarTaskMenu(QDesignerMenuBar *bar, QObject *parent) :
        QObject(parent),
        m_bar(bar)
    {
    }

    QAction *MenuBarTaskMenu::preferredEditAction() const
    {
        return 0;
    }

    QList<QAction*> MenuBarTaskMenu::taskActions() const
    {
        return m_bar->contextMenuActions();
    }
}

QT_END_NAMESPACE

