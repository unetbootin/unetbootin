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
TRANSLATOR qdesigner_internal::SignalSlotEditorTool
*/

#include "signalsloteditor_tool.h"
#include "signalsloteditor.h"
#include "ui4_p.h"

#include <QtDesigner/QDesignerFormWindowInterface>

#include <QtGui/QAction>
#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

using namespace qdesigner_internal;

SignalSlotEditorTool::SignalSlotEditorTool(QDesignerFormWindowInterface *formWindow, QObject *parent)
    : QDesignerFormWindowToolInterface(parent),
      m_formWindow(formWindow),
      m_action(new QAction(tr("Edit Signals/Slots"), this))
{
}

SignalSlotEditorTool::~SignalSlotEditorTool()
{
}

QDesignerFormEditorInterface *SignalSlotEditorTool::core() const
{
    return m_formWindow->core();
}

QDesignerFormWindowInterface *SignalSlotEditorTool::formWindow() const
{
    return m_formWindow;
}

bool SignalSlotEditorTool::handleEvent(QWidget *widget, QWidget *managedWidget, QEvent *event)
{
    Q_UNUSED(widget);
    Q_UNUSED(managedWidget);
    Q_UNUSED(event);

    return false;
}

QWidget *SignalSlotEditorTool::editor() const
{
    if (!m_editor) {
        Q_ASSERT(formWindow() != 0);
        m_editor = new qdesigner_internal::SignalSlotEditor(formWindow(), 0);
        connect(formWindow(), SIGNAL(mainContainerChanged(QWidget*)), m_editor, SLOT(setBackground(QWidget*)));
        connect(formWindow(), SIGNAL(changed()),
                m_editor, SLOT(updateBackground()));
    }

    return m_editor;
}

QAction *SignalSlotEditorTool::action() const
{
    return m_action;
}

void SignalSlotEditorTool::activated()
{
    m_editor->enableUpdateBackground(true);
}

void SignalSlotEditorTool::deactivated()
{
    m_editor->enableUpdateBackground(false);
}

void SignalSlotEditorTool::saveToDom(DomUI *ui, QWidget*)
{
    ui->setElementConnections(m_editor->toUi());
}

void SignalSlotEditorTool::loadFromDom(DomUI *ui, QWidget *mainContainer)
{
    m_editor->fromUi(ui->elementConnections(), mainContainer);
}

QT_END_NAMESPACE
