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

#include "formwindow_widgetstack.h"
#include <QtDesigner/QDesignerFormWindowToolInterface>

#include <QtGui/QWidget>
#include <QtGui/qevent.h>
#include <QtGui/QAction>
#include <QtGui/QStackedLayout>

#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

namespace {
    // Dummy form to be used before the real main container is set
    class DummyForm : public QWidget {
    public:
        DummyForm(QWidget *parent = 0) : QWidget(parent) {}
        virtual QSize sizeHint() const { return QSize(400, 300); }
    };
}

using namespace qdesigner_internal;

FormWindowWidgetStack::FormWindowWidgetStack(QObject *parent) :
    QObject(parent),
    m_layout(new QStackedLayout),
    m_dummyMainContainer(false)
{
    m_layout->setMargin(0);
    m_layout->setSpacing(0);
    m_layout->setStackingMode(QStackedLayout::StackAll);
    m_layout->setSizeConstraint(QLayout::SetMinAndMaxSize);
}

FormWindowWidgetStack::~FormWindowWidgetStack()
{
}

int FormWindowWidgetStack::count() const
{
    return m_tools.count();
}

QDesignerFormWindowToolInterface *FormWindowWidgetStack::currentTool() const
{
    return tool(currentIndex());
}

void FormWindowWidgetStack::setCurrentTool(int index)
{
    const int cnt = count();
    if (index < 0 || index >= cnt) {
        qDebug("FormWindowWidgetStack::setCurrentTool(): invalid index: %d", index);
        return;
    }

    const int cur = currentIndex();
    if (index == cur)
        return;

    if (cur != -1)
        m_tools.at(cur)->deactivated();


    m_layout->setCurrentIndex(index);
    // Show the widget editor and the current tool
    for (int i = 0; i < cnt; i++)
        m_tools.at(i)->editor()->setVisible(i == 0 || i == index);

    QDesignerFormWindowToolInterface *tool = m_tools.at(index);
    tool->activated();

    emit currentToolChanged(index);
}

void FormWindowWidgetStack::setSenderAsCurrentTool()
{
    QDesignerFormWindowToolInterface *tool = 0;
    QAction *action = qobject_cast<QAction*>(sender());
    if (action == 0) {
        qDebug("FormWindowWidgetStack::setSenderAsCurrentTool(): sender is not a QAction");
        return;
    }

    foreach (QDesignerFormWindowToolInterface *t, m_tools) {
        if (action == t->action()) {
            tool = t;
            break;
        }
    }

    if (tool == 0) {
        qDebug("FormWindowWidgetStack::setSenderAsCurrentTool(): unknown tool");
        return;
    }

    setCurrentTool(tool);
}

int FormWindowWidgetStack::indexOf(QDesignerFormWindowToolInterface *tool) const
{
    return m_tools.indexOf(tool);
}

void FormWindowWidgetStack::setCurrentTool(QDesignerFormWindowToolInterface *tool)
{
    int index = indexOf(tool);
    if (index == -1) {
        qDebug("FormWindowWidgetStack::setCurrentTool(): unknown tool");
        return;
    }

    setCurrentTool(index);
}

void FormWindowWidgetStack::setMainContainer(QWidget *w)
{
    // This code is triggered once by the formwindow and
    // by integrations doing "revert to saved". Anything changing?
    if (m_dummyMainContainer && w == 0)
        return;
    QWidget *previousMainContainer = m_layout->widget(0);
    if (previousMainContainer == w)
        return;
    // Insert new first as not to cause the layout to switch indexes, use dummy if 0
    const bool wasDummyMainContainer = m_dummyMainContainer;
    m_dummyMainContainer = w == 0;
    if (m_dummyMainContainer)
        w = new DummyForm;
    m_layout->insertWidget(0, w);
    // Remove previous widget at 0, delete dummy
    const int mcIndex = m_layout->indexOf(previousMainContainer);
    Q_ASSERT(mcIndex != -1);
    delete m_layout->takeAt(mcIndex);
    if (wasDummyMainContainer)
        delete previousMainContainer;
}

void FormWindowWidgetStack::addTool(QDesignerFormWindowToolInterface *tool)
{
    if (QWidget *w = tool->editor()) {
        m_layout->addWidget(w);
    } else {
        // The form editor might not have a tool initially, use dummy. Assert on anything else
        Q_ASSERT(m_tools.empty());
        m_dummyMainContainer = true;
        m_layout->addWidget(new DummyForm);
    }

    m_tools.append(tool);

    connect(tool->action(), SIGNAL(triggered()), this, SLOT(setSenderAsCurrentTool()));
}

QDesignerFormWindowToolInterface *FormWindowWidgetStack::tool(int index) const
{
    if (index < 0 || index >= count())
        return 0;

    return m_tools.at(index);
}

int FormWindowWidgetStack::currentIndex() const
{
    return m_layout->currentIndex();
}

QWidget *FormWindowWidgetStack::defaultEditor() const
{
    if (m_tools.isEmpty())
        return 0;

    return m_tools.at(0)->editor();
}

QLayout *FormWindowWidgetStack::layout() const
{
    return m_layout;
}

QT_END_NAMESPACE
