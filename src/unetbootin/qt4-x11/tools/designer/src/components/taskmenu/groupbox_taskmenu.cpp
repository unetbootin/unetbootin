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
TRANSLATOR qdesigner_internal::GroupBoxTaskMenu
*/

#include "groupbox_taskmenu.h"
#include "inplace_editor.h"

#include <QtDesigner/QDesignerFormWindowInterface>

#include <QtGui/QAction>
#include <QtGui/QStyle>
#include <QtGui/QStyleOption>

QT_BEGIN_NAMESPACE

namespace qdesigner_internal {

// -------- GroupBoxTaskMenuInlineEditor
class GroupBoxTaskMenuInlineEditor : public  TaskMenuInlineEditor
{
public:
    GroupBoxTaskMenuInlineEditor(QGroupBox *button, QObject *parent);

protected:
    virtual QRect editRectangle() const;
};

GroupBoxTaskMenuInlineEditor::GroupBoxTaskMenuInlineEditor(QGroupBox *w, QObject *parent) :
      TaskMenuInlineEditor(w, ValidationSingleLine, QLatin1String("title"), parent)
{
}

QRect GroupBoxTaskMenuInlineEditor::editRectangle() const
{
    QWidget *w = widget();
    QStyleOption opt; // ## QStyleOptionGroupBox
    opt.init(w);
    return QRect(QPoint(), QSize(w->width(),20));
}

// --------------- GroupBoxTaskMenu

GroupBoxTaskMenu::GroupBoxTaskMenu(QGroupBox *groupbox, QObject *parent)
    : QDesignerTaskMenu(groupbox, parent),
      m_editTitleAction(new QAction(tr("Change title..."), this))

{
    TaskMenuInlineEditor *editor = new GroupBoxTaskMenuInlineEditor(groupbox, this);
    connect(m_editTitleAction, SIGNAL(triggered()), editor, SLOT(editText()));
    m_taskActions.append(m_editTitleAction);

    QAction *sep = new QAction(this);
    sep->setSeparator(true);
    m_taskActions.append(sep);
}

QList<QAction*> GroupBoxTaskMenu::taskActions() const
{
    return m_taskActions + QDesignerTaskMenu::taskActions();
}

QAction *GroupBoxTaskMenu::preferredEditAction() const
{
    return m_editTitleAction;
}

}
QT_END_NAMESPACE
