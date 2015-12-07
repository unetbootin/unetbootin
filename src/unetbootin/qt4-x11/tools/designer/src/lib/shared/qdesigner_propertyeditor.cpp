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

#include "qdesigner_propertyeditor_p.h"
#ifdef Q_OS_WIN
#  include <widgetfactory_p.h>
#endif
#include <QAction>
#include <QLineEdit>
#include <QAbstractButton>

QT_BEGIN_NAMESPACE

namespace qdesigner_internal {

QDesignerPropertyEditor::QDesignerPropertyEditor(QWidget *parent, Qt::WindowFlags flags) :
    QDesignerPropertyEditorInterface(parent, flags)
{
    // Make old signal work for  compatibility
    connect(this, SIGNAL(propertyChanged(QString,QVariant)), this, SLOT(slotPropertyChanged(QString,QVariant)));
}

QDesignerPropertyEditor::StringPropertyParameters QDesignerPropertyEditor::textPropertyValidationMode(
        QDesignerFormEditorInterface *core, const QObject *object,
        const QString &propertyName, bool isMainContainer)
{
    // object name - no comment
    if (propertyName == QLatin1String("objectName")) {
        const TextPropertyValidationMode vm =  isMainContainer ? ValidationObjectNameScope : ValidationObjectName;
        return StringPropertyParameters(vm, false);
    }

    // Accessibility. Both are texts the narrator reads
    if (propertyName == QLatin1String("accessibleDescription") || propertyName == QLatin1String("accessibleName"))
        return StringPropertyParameters(ValidationRichText, true);

    // Any names
    if (propertyName == QLatin1String("buddy") || propertyName.endsWith(QLatin1String("Name")))
        return StringPropertyParameters(ValidationObjectName, false);

    // Multi line?
    if (propertyName == QLatin1String("styleSheet"))
        return StringPropertyParameters(ValidationStyleSheet, false);

    if (propertyName == QLatin1String("description") || propertyName == QLatin1String("iconText")) // QCommandLinkButton
        return StringPropertyParameters(ValidationMultiLine, true);

    if (propertyName == QLatin1String("toolTip")         || propertyName.endsWith(QLatin1String("ToolTip")) ||
            propertyName == QLatin1String("whatsThis")       ||
            propertyName == QLatin1String("windowIconText")  || propertyName == QLatin1String("html"))
        return StringPropertyParameters(ValidationRichText, true);

    // text: Check according to widget type.
    if (propertyName == QLatin1String("text")) {
        if (qobject_cast<const QAction *>(object) || qobject_cast<const QLineEdit *>(object))
            return StringPropertyParameters(ValidationSingleLine, true);
        if (qobject_cast<const QAbstractButton *>(object))
            return StringPropertyParameters(ValidationMultiLine, true);
        return StringPropertyParameters(ValidationRichText, true);
    }
    if (propertyName == QLatin1String("pageId")) // A QWizard page id
        return StringPropertyParameters(ValidationSingleLine, false);

    if (propertyName == QLatin1String("plainText")) // QPlainTextEdit
        return StringPropertyParameters(ValidationMultiLine, true);

#ifdef Q_OS_WIN // No translation for the active X "control" property
    if (propertyName == QLatin1String("control") && WidgetFactory::classNameOf(core, object) == QLatin1String("QAxWidget"))
        return StringPropertyParameters(ValidationSingleLine, false);
#else
    Q_UNUSED(core);
#endif

    // default to single
    return StringPropertyParameters(ValidationSingleLine, true);
}

void QDesignerPropertyEditor::slotPropertyChanged(const QString &name, const QVariant &value)
{
    emit propertyValueChanged(name, value, true);
}

}

QT_END_NAMESPACE
