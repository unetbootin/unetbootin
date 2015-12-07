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

#ifndef TOOL_WIDGETEDITOR_H
#define TOOL_WIDGETEDITOR_H

#include <QtDesigner/QDesignerFormWindowToolInterface>

#include <QtGui/qevent.h>
#include <QtCore/QPointer>

QT_BEGIN_NAMESPACE

class QAction;
class QMainWindow;

namespace qdesigner_internal {

class FormWindow;
class QDesignerMimeData;

class WidgetEditorTool: public QDesignerFormWindowToolInterface
{
    Q_OBJECT
public:
    explicit WidgetEditorTool(FormWindow *formWindow);
    virtual ~WidgetEditorTool();

    virtual QDesignerFormEditorInterface *core() const;
    virtual QDesignerFormWindowInterface *formWindow() const;
    virtual QWidget *editor() const;
    virtual QAction *action() const;

    virtual void activated();
    virtual void deactivated();

    virtual bool handleEvent(QWidget *widget, QWidget *managedWidget, QEvent *event);

    bool handleContextMenu(QWidget *widget, QWidget *managedWidget, QContextMenuEvent *e);
    bool handleMouseButtonDblClickEvent(QWidget *widget, QWidget *managedWidget, QMouseEvent *e);
    bool handleMousePressEvent(QWidget *widget, QWidget *managedWidget, QMouseEvent *e);
    bool handleMouseMoveEvent(QWidget *widget, QWidget *managedWidget, QMouseEvent *e);
    bool handleMouseReleaseEvent(QWidget *widget, QWidget *managedWidget, QMouseEvent *e);
    bool handleKeyPressEvent(QWidget *widget, QWidget *managedWidget, QKeyEvent *e);
    bool handleKeyReleaseEvent(QWidget *widget, QWidget *managedWidget, QKeyEvent *e);
    bool handlePaintEvent(QWidget *widget, QWidget *managedWidget, QPaintEvent *e);

    bool handleDragEnterMoveEvent(QWidget *widget, QWidget *managedWidget, QDragMoveEvent *e, bool isEnter);
    bool handleDragLeaveEvent(QWidget *widget, QWidget *managedWidget, QDragLeaveEvent *e);
    bool handleDropEvent(QWidget *widget, QWidget *managedWidget, QDropEvent *e);

private:
    bool restoreDropHighlighting();
    void detectDockDrag(const QDesignerMimeData *mimeData);

    FormWindow *m_formWindow;
    QAction *m_action;

    bool mainWindowSeparatorEvent(QWidget *widget, QEvent *event);
    QPointer<QMainWindow> m_separator_drag_mw;
    QPointer<QWidget> m_lastDropTarget;
    bool m_specialDockDrag;
};

}  // namespace qdesigner_internal

QT_END_NAMESPACE

#endif // TOOL_WIDGETEDITOR_H
