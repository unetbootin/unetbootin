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

#ifndef QDESIGNER_COMPONENTS_H
#define QDESIGNER_COMPONENTS_H

#include <QtDesigner/qdesigner_components_global.h>
#include <QtCore/qglobal.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QObject;
class QWidget;

class QDesignerFormEditorInterface;
class QDesignerWidgetBoxInterface;
class QDesignerPropertyEditorInterface;
class QDesignerObjectInspectorInterface;
class QDesignerActionEditorInterface;

class QDESIGNER_COMPONENTS_EXPORT QDesignerComponents
{
public:
    static void initializeResources();
    static void initializePlugins(QDesignerFormEditorInterface *core);

    static QDesignerFormEditorInterface *createFormEditor(QObject *parent);
    static QDesignerWidgetBoxInterface *createWidgetBox(QDesignerFormEditorInterface *core, QWidget *parent);
    static QDesignerPropertyEditorInterface *createPropertyEditor(QDesignerFormEditorInterface *core, QWidget *parent);
    static QDesignerObjectInspectorInterface *createObjectInspector(QDesignerFormEditorInterface *core, QWidget *parent);
    static QDesignerActionEditorInterface *createActionEditor(QDesignerFormEditorInterface *core, QWidget *parent);

    static QObject *createTaskMenu(QDesignerFormEditorInterface *core, QObject *parent);
    static QWidget *createResourceEditor(QDesignerFormEditorInterface *core, QWidget *parent);
    static QWidget *createSignalSlotEditor(QDesignerFormEditorInterface *core, QWidget *parent);
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QDESIGNER_COMPONENTS_H
