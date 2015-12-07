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

#ifndef MENUTASKMENU_H
#define MENUTASKMENU_H

#include <QtDesigner/QDesignerTaskMenuExtension>

#include <qdesigner_menu_p.h>
#include <qdesigner_menubar_p.h>
#include <extensionfactory_p.h>

QT_BEGIN_NAMESPACE

class QDesignerFormWindowInterface;

namespace qdesigner_internal {

    class PromotionTaskMenu;

// The QMenu task menu provides promotion and a remove option. The actual
// menu context options are not forwarded since they make only sense 
// when a menu is being edited/visible.

class MenuTaskMenu : public QObject, public QDesignerTaskMenuExtension
{
    Q_OBJECT
    Q_INTERFACES(QDesignerTaskMenuExtension)
public:
    explicit MenuTaskMenu(QDesignerMenu *menu, QObject *parent = 0);

    virtual QAction *preferredEditAction() const;
    virtual QList<QAction*> taskActions() const;

private slots:
    void removeMenu();

private:
    QDesignerMenu *m_menu;
    QAction *m_removeAction;
    PromotionTaskMenu *m_promotionTaskMenu;
};

// The QMenuBar task menu forwards the actions of QDesignerMenuBar,
// making them available in the object inspector.

class MenuBarTaskMenu : public QObject, public QDesignerTaskMenuExtension
{
    Q_OBJECT
    Q_INTERFACES(QDesignerTaskMenuExtension)
public:
    explicit MenuBarTaskMenu(QDesignerMenuBar *bar, QObject *parent = 0);

    virtual QAction *preferredEditAction() const;
    virtual QList<QAction*> taskActions() const;

private:
    QDesignerMenuBar *m_bar;
};

typedef ExtensionFactory<QDesignerTaskMenuExtension, QDesignerMenu, MenuTaskMenu> MenuTaskMenuFactory;
typedef ExtensionFactory<QDesignerTaskMenuExtension, QDesignerMenuBar, MenuBarTaskMenu> MenuBarTaskMenuFactory;

}  // namespace qdesigner_internal

QT_END_NAMESPACE

#endif // MENUTASKMENU_H
