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

#include "qdesigner_dockwidget_p.h"
#include "layoutinfo_p.h"

#include <QtDesigner/QDesignerFormWindowInterface>
#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QDesignerContainerExtension>
#include <QtDesigner/QExtensionManager>

#include <QtGui/QMainWindow>
#include <QtGui/QLayout>

QT_BEGIN_NAMESPACE

QDesignerDockWidget::QDesignerDockWidget(QWidget *parent)
    : QDockWidget(parent)
{
}

QDesignerDockWidget::~QDesignerDockWidget()
{
}

bool QDesignerDockWidget::docked() const
{
    return qobject_cast<QMainWindow*>(parentWidget()) != 0;
}

void QDesignerDockWidget::setDocked(bool b)
{
    if (QMainWindow *mainWindow = findMainWindow()) {
        QDesignerFormEditorInterface *core = formWindow()->core();
        QDesignerContainerExtension *c;
        c = qt_extension<QDesignerContainerExtension*>(core->extensionManager(), mainWindow);
        if (b && !docked()) {
            // Dock it
            // ### undo/redo stack
            setParent(0);
            c->addWidget(this);
            formWindow()->emitSelectionChanged();
        } else if (!b && docked()) {
            // Undock it
            for (int i = 0; i < c->count(); ++i) {
                if (c->widget(i) == this) {
                    c->remove(i);
                    break;
                }
            }
            // #### restore the position
            setParent(mainWindow->centralWidget());
            show();
            formWindow()->emitSelectionChanged();
        }
    }
}

Qt::DockWidgetArea QDesignerDockWidget::dockWidgetArea() const
{
    if (QMainWindow *mainWindow = qobject_cast<QMainWindow*>(parentWidget()))
        return mainWindow->dockWidgetArea(const_cast<QDesignerDockWidget*>(this));

    return Qt::LeftDockWidgetArea;
}

void QDesignerDockWidget::setDockWidgetArea(Qt::DockWidgetArea dockWidgetArea)
{
    if (QMainWindow *mainWindow = qobject_cast<QMainWindow*>(parentWidget())) {
        if ((dockWidgetArea != Qt::NoDockWidgetArea)
            && isAreaAllowed(dockWidgetArea)) {
            mainWindow->addDockWidget(dockWidgetArea, this);
        }
    }
}

bool QDesignerDockWidget::inMainWindow() const
{
    QMainWindow *mw = findMainWindow();
    if (mw && !mw->centralWidget()->layout()) {
        if (mw == parentWidget())
            return true;
        if (mw->centralWidget() == parentWidget())
            return true;
    }
    return false;
}

QDesignerFormWindowInterface *QDesignerDockWidget::formWindow() const
{
    return QDesignerFormWindowInterface::findFormWindow(const_cast<QDesignerDockWidget*>(this));
}

QMainWindow *QDesignerDockWidget::findMainWindow() const
{
    if (QDesignerFormWindowInterface *fw = formWindow())
        return qobject_cast<QMainWindow*>(fw->mainContainer());
    return 0;
}

QT_END_NAMESPACE
