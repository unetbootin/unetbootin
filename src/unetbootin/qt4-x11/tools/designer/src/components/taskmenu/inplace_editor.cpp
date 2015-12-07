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

#include "abstractformwindow.h"
#include "inplace_editor.h"

#include <QtDesigner/QDesignerFormWindowInterface>
#include <QtDesigner/QDesignerFormWindowCursorInterface>
#include <QtDesigner/QDesignerPropertySheetExtension>
#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QDesignerLanguageExtension>
#include <QtDesigner/QExtensionManager>

#include <QtCore/QVariant>

QT_BEGIN_NAMESPACE

namespace qdesigner_internal {

// ----------------- InPlaceEditor

InPlaceEditor::InPlaceEditor(QWidget *widget,
                             TextPropertyValidationMode validationMode,
                             QDesignerFormWindowInterface *fw,
                             const QString& text,
                             const QRect& r) :
    TextPropertyEditor(widget, EmbeddingInPlace, validationMode),
    m_InPlaceWidgetHelper(this, widget, fw)
{
    setAlignment(m_InPlaceWidgetHelper.alignment());
    setObjectName(QLatin1String("__qt__passive_m_editor"));

    setText(text);
    selectAll();

    setGeometry(QRect(widget->mapTo(widget->window(), r.topLeft()), r.size()));
    setFocus();
    show();

    connect(this, SIGNAL(editingFinished()),this, SLOT(close()));
}


// -------------- TaskMenuInlineEditor

TaskMenuInlineEditor::TaskMenuInlineEditor(QWidget *w, TextPropertyValidationMode vm,
                                           const QString &property, QObject *parent) :
    QObject(parent),
    m_vm(vm),
    m_property(property),
    m_widget(w),
    m_managed(true)
{
}

void TaskMenuInlineEditor::editText()
{
    m_formWindow = QDesignerFormWindowInterface::findFormWindow(m_widget);
    if (m_formWindow.isNull())
        return;
    m_managed = m_formWindow->isManaged(m_widget);
    // Close as soon as a different widget is selected
    connect(m_formWindow, SIGNAL(selectionChanged()), this, SLOT(updateSelection()));

    // get old value
    QDesignerFormEditorInterface *core = m_formWindow->core();
    const QDesignerPropertySheetExtension *sheet = qt_extension<QDesignerPropertySheetExtension*>(core->extensionManager(), m_widget);
    const int index = sheet->indexOf(m_property);
    if (index == -1)
        return;
    m_value = qVariantValue<PropertySheetStringValue>(sheet->property(index));
    const QString oldValue = m_value.value();

    m_editor = new InPlaceEditor(m_widget, m_vm, m_formWindow, oldValue, editRectangle());
    connect(m_editor, SIGNAL(textChanged(QString)), this, SLOT(updateText(QString)));
}

void TaskMenuInlineEditor::updateText(const QString &text)
{
    // In the [rare] event we are invoked on an unmanaged widget,
    // do not use the cursor selection
    m_value.setValue(text);
    if (m_managed) {
        m_formWindow->cursor()->setProperty(m_property, qVariantFromValue(m_value));
    } else {
        m_formWindow->cursor()->setWidgetProperty(m_widget, m_property, qVariantFromValue(m_value));
    }
}

void TaskMenuInlineEditor::updateSelection()
{
    if (m_editor)
        m_editor->deleteLater();
}

}

QT_END_NAMESPACE
