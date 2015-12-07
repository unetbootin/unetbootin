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

#ifndef INPLACE_EDITOR_H
#define INPLACE_EDITOR_H

#include <textpropertyeditor_p.h>
#include <shared_enums_p.h>

#include "inplace_widget_helper.h"

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
};

}  // namespace qdesigner_internal

QT_END_NAMESPACE

#endif // INPLACE_EDITOR_H
