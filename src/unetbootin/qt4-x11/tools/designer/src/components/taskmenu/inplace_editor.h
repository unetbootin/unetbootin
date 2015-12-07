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

#ifndef INPLACE_EDITOR_H
#define INPLACE_EDITOR_H

#include <textpropertyeditor_p.h>
#include <shared_enums_p.h>

#include "inplace_widget_helper.h"
#include <qdesigner_utils_p.h>

#include <QtCore/QPointer>

QT_BEGIN_NAMESPACE


class QDesignerFormWindowInterface;

namespace qdesigner_internal {

class InPlaceEditor: public TextPropertyEditor
{
    Q_OBJECT
public:
    InPlaceEditor(QWidget *widget,
                  TextPropertyValidationMode validationMode,
                  QDesignerFormWindowInterface *fw,
                  const QString& text,
                  const QRect& r);
private:
    InPlaceWidgetHelper m_InPlaceWidgetHelper;
};

// Base class for inline editor helpers to be embedded into a task menu.
// Inline-edits a property on a multi-selection.
// To use it for a particular widget/property, overwrite the method
// returning the edit area.

class TaskMenuInlineEditor : public QObject {
    TaskMenuInlineEditor(const TaskMenuInlineEditor&);
    TaskMenuInlineEditor &operator=(const TaskMenuInlineEditor&);
    Q_OBJECT

public slots:
    void editText();

private slots:
    void updateText(const QString &text);
    void updateSelection();

protected:
    TaskMenuInlineEditor(QWidget *w, TextPropertyValidationMode vm, const QString &property, QObject *parent);
    // Overwrite to return the area for the inline editor.
    virtual QRect editRectangle() const = 0;
    QWidget *widget() const { return m_widget; }

private:
    const TextPropertyValidationMode m_vm;
    const QString m_property;
    QWidget *m_widget;
    QPointer<QDesignerFormWindowInterface> m_formWindow;
    QPointer<InPlaceEditor> m_editor;
    bool m_managed;
    qdesigner_internal::PropertySheetStringValue m_value;
};

}  // namespace qdesigner_internal

QT_END_NAMESPACE

#endif // INPLACE_EDITOR_H
