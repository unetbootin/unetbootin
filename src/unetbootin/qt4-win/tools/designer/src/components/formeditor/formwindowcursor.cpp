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
TRANSLATOR qdesigner_internal::FormWindowCursor
*/

#include "formwindowcursor.h"
#include "formwindow.h"

// sdk
#include <QtDesigner/propertysheet.h>
#include <QtDesigner/QExtensionManager>
#include <qdesigner_propertycommand_p.h>

#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

namespace qdesigner_internal {

FormWindowCursor::FormWindowCursor(FormWindow *fw, QObject *parent)
    : QObject(parent),
      m_formWindow(fw)
{
    update();
    connect(fw, SIGNAL(changed()), this, SLOT(update()));
}

FormWindowCursor::~FormWindowCursor()
{
}

QDesignerFormWindowInterface *FormWindowCursor::formWindow() const
{
    return m_formWindow;
}

bool FormWindowCursor::movePosition(MoveOperation op, MoveMode mode)
{
    if (widgetCount() == 0)
        return false;

    int iterator = position();

    if (mode == MoveAnchor)
        m_formWindow->clearSelection(false);

    switch (op) {
    case Next:
        ++iterator;
        if (iterator >= widgetCount())
            iterator = 0;

        m_formWindow->selectWidget(m_formWindow->widgetAt(iterator), true);
        return true;

    case Prev:
        --iterator;
        if (iterator < 0)
            iterator = widgetCount() - 1;

        if (iterator < 0)
            return false;

        m_formWindow->selectWidget(m_formWindow->widgetAt(iterator), true);
        return true;

    default:
        return false;
    }
}

int FormWindowCursor::position() const
{
    const int index = m_formWindow->widgets().indexOf(current());
    return index == -1 ? 0 : index;
}

void FormWindowCursor::setPosition(int pos, MoveMode mode)
{
    if (!widgetCount())
        return;

    if (mode == MoveAnchor)
        m_formWindow->clearSelection(false);

    if (pos >= widgetCount())
        pos = 0;

    m_formWindow->selectWidget(m_formWindow->widgetAt(pos), true);
}

QWidget *FormWindowCursor::current() const
{
    return m_formWindow->currentWidget();
}

bool FormWindowCursor::hasSelection() const
{
    return !m_formWindow->selectedWidgets().isEmpty();
}

int FormWindowCursor::selectedWidgetCount() const
{
    int N = m_formWindow->selectedWidgets().count();
    return N ? N : 1;
}

QWidget *FormWindowCursor::selectedWidget(int index) const
{
    return hasSelection()
        ? m_formWindow->selectedWidgets().at(index)
        : m_formWindow->mainContainer();
}

void FormWindowCursor::update()
{
    // ### todo
}

int FormWindowCursor::widgetCount() const
{
    return m_formWindow->widgetCount();
}

QWidget *FormWindowCursor::widget(int index) const
{
    return m_formWindow->widgetAt(index);
}

void FormWindowCursor::setProperty(const QString &name, const QVariant &value)
{

    // build selection
    const int N = selectedWidgetCount();
    Q_ASSERT(N);
    
    SetPropertyCommand::ObjectList selection;
    for (int i=0; i<N; ++i) 
         selection.push_back(selectedWidget(i));

    
    SetPropertyCommand* setPropertyCommand = new SetPropertyCommand(m_formWindow);
    if (setPropertyCommand->init(selection, name, value, current())) {
        m_formWindow->commandHistory()->push(setPropertyCommand);
    } else {
        delete setPropertyCommand;
        qDebug() << "Unable to set property " << name << '.';
    }
}

void FormWindowCursor::setWidgetProperty(QWidget *widget, const QString &name, const QVariant &value)
{
    SetPropertyCommand *cmd = new SetPropertyCommand(m_formWindow);
    if (cmd->init(widget, name, value)) {
        m_formWindow->commandHistory()->push(cmd);
    } else {
        delete cmd;
        qDebug() << "Unable to set property " << name << '.';
    }
}

void FormWindowCursor::resetWidgetProperty(QWidget *widget, const QString &name)
{
    ResetPropertyCommand *cmd = new ResetPropertyCommand(m_formWindow);
    if (cmd->init(widget, name)) {
        m_formWindow->commandHistory()->push(cmd);
    } else {
        delete cmd;
        qDebug() << "Unable to reset property " << name << '.';
    }
}

}

QT_END_NAMESPACE
