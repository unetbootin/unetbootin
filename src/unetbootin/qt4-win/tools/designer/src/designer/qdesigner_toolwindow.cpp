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

#include "qdesigner.h"
#include "qdesigner_toolwindow.h"
#include "qdesigner_settings.h"
#include "qdesigner_workbench.h"

#include <QtCore/QEvent>
#include <QtCore/qdebug.h>
#include <QtGui/QAction>
#include <QtGui/QCloseEvent>

QT_BEGIN_NAMESPACE

QDesignerToolWindow::QDesignerToolWindow(QDesignerWorkbench *workbench, QWidget *parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags),
      m_workbench(workbench),
      m_saveSettings(false)
{
    Q_ASSERT(workbench != 0);

#ifndef Q_WS_MAC
    setWindowIcon(qDesigner->windowIcon());
#endif    

    m_action = new QAction(this);
    m_action->setShortcutContext(Qt::ApplicationShortcut);
    m_action->setText(windowTitle());
    m_action->setCheckable(true);
    connect(m_action, SIGNAL(triggered(bool)), this, SLOT(showMe(bool)));
}

QDesignerToolWindow::~QDesignerToolWindow()
{
    if (workbench())
        workbench()->removeToolWindow(this);
}

void QDesignerToolWindow::showMe(bool v)
{
    QWidget *target = parentWidget() == 0 ? this : parentWidget();

    if (v)
        target->setWindowState(target->windowState() & ~Qt::WindowMinimized);

    target->setVisible(v);
}

void QDesignerToolWindow::showEvent(QShowEvent *e)
{
    Q_UNUSED(e);

    bool blocked = m_action->blockSignals(true);
    m_action->setChecked(true);
    m_action->blockSignals(blocked);
}

void QDesignerToolWindow::hideEvent(QHideEvent *e)
{
    Q_UNUSED(e);

    bool blocked = m_action->blockSignals(true);
    m_action->setChecked(false);
    m_action->blockSignals(blocked);
}

QAction *QDesignerToolWindow::action() const
{
    return m_action;
}

void QDesignerToolWindow::changeEvent(QEvent *e)
{
    switch (e->type()) {
        case QEvent::WindowTitleChange:
            m_action->setText(windowTitle());
            break;
        case QEvent::WindowIconChange:
            m_action->setIcon(windowIcon());
            break;
        default:
            break;
    }
    QMainWindow::changeEvent(e);
}

QDesignerWorkbench *QDesignerToolWindow::workbench() const
{
    return m_workbench;
}

QRect QDesignerToolWindow::geometryHint() const
{
    return QRect();
}

void QDesignerToolWindow::closeEvent(QCloseEvent *ev)
{
    if (m_saveSettings) {
        ev->setAccepted(workbench()->handleClose());
        if (ev->isAccepted() && qDesigner->mainWindow() == this)
            QMetaObject::invokeMethod(qDesigner, "quit", Qt::QueuedConnection);  // We're going down!
    } else {
        QMainWindow::closeEvent(ev);
    }
}

bool QDesignerToolWindow::saveSettingsOnClose() const
{
    return m_saveSettings;
}

void QDesignerToolWindow::setSaveSettingsOnClose(bool save)
{
    m_saveSettings = save;
}

Qt::DockWidgetArea QDesignerToolWindow::dockWidgetAreaHint() const
{
    return Qt::RightDockWidgetArea;
}


QT_END_NAMESPACE
