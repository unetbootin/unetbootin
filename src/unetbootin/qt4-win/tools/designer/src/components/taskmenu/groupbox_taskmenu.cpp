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
