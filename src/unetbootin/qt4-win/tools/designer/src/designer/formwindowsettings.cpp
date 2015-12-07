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

#include "formwindowsettings.h"
#include <formwindowbase_p.h>
#include <QtGui/QStyle>

QT_BEGIN_NAMESPACE

FormWindowSettings::FormWindowSettings(QDesignerFormWindowInterface *parent) :
    QDialog(parent),
    m_formWindow(qobject_cast<qdesigner_internal::FormWindowBase*>(parent))
{
    Q_ASSERT(m_formWindow);
    ui.setupUi(this);
    ui.gridPanel->setCheckable(true);
    ui.gridPanel->setResetButtonVisible(false);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    int defaultMargin = INT_MIN, defaultSpacing = INT_MIN;
    m_formWindow->layoutDefault(&defaultMargin, &defaultSpacing);

    QStyle *style = m_formWindow->style();
    ui.defaultMarginSpinBox->setValue(style->pixelMetric(QStyle::PM_DefaultChildMargin, 0));
    ui.defaultSpacingSpinBox->setValue(style->pixelMetric(QStyle::PM_DefaultLayoutSpacing, 0));

    if (defaultMargin != INT_MIN || defaultMargin != INT_MIN) {
        ui.layoutDefaultGroupBox->setChecked(true);

        if (defaultMargin != INT_MIN)
            ui.defaultMarginSpinBox->setValue(defaultMargin);

        if (defaultSpacing != INT_MIN)
            ui.defaultSpacingSpinBox->setValue(defaultSpacing);
    } else {
        ui.layoutDefaultGroupBox->setChecked(false);
    }

    QString marginFunction, spacingFunction;
    m_formWindow->layoutFunction(&marginFunction, &spacingFunction);
    if (!marginFunction.isEmpty() || !spacingFunction.isEmpty()) {
        ui.layoutFunctionGroupBox->setChecked(true);
        ui.marginFunctionLineEdit->setText(marginFunction);
        ui.spacingFunctionLineEdit->setText(spacingFunction);
    } else {
        ui.layoutFunctionGroupBox->setChecked(false);
    }

    const QString pixFunction = m_formWindow->pixmapFunction();
    ui.pixmapFunctionGroupBox->setChecked(!pixFunction.isEmpty());
    ui.pixmapFunctionLineEdit->setText(pixFunction);

    ui.authorLineEdit->setText(m_formWindow->author());

    foreach (QString includeHint, m_formWindow->includeHints()) {
        if (includeHint.isEmpty())
            continue;

        ui.includeHintsTextEdit->append(includeHint);
    }

    const bool hasFormGrid = m_formWindow->hasFormGrid();
    ui.gridPanel->setChecked(hasFormGrid);
    ui.gridPanel->setGrid(hasFormGrid ? m_formWindow->designerGrid() : qdesigner_internal::FormWindowBase::defaultDesignerGrid());
}

void FormWindowSettings::accept()
{
    m_formWindow->setAuthor(ui.authorLineEdit->text());

    if (ui.pixmapFunctionGroupBox->isChecked())
        m_formWindow->setPixmapFunction(ui.pixmapFunctionLineEdit->text());
    else
        m_formWindow->setPixmapFunction(QString());

    if (ui.layoutDefaultGroupBox->isChecked())
        m_formWindow->setLayoutDefault(ui.defaultMarginSpinBox->value(), ui.defaultSpacingSpinBox->value());
    else
        m_formWindow->setLayoutDefault(INT_MIN, INT_MIN);

    if (ui.layoutFunctionGroupBox->isChecked())
        m_formWindow->setLayoutFunction(ui.marginFunctionLineEdit->text(), ui.spacingFunctionLineEdit->text());
    else
        m_formWindow->setLayoutFunction(QString(), QString());

    m_formWindow->setIncludeHints(ui.includeHintsTextEdit->toPlainText().split(QString(QLatin1Char('\n'))));

    const bool hadFormGrid = m_formWindow->hasFormGrid();
    const bool wantsFormGrid = ui.gridPanel->isChecked();
    m_formWindow->setHasFormGrid(wantsFormGrid);
    if (wantsFormGrid || hadFormGrid != wantsFormGrid)
        m_formWindow->setDesignerGrid(wantsFormGrid ? ui.gridPanel->grid() : qdesigner_internal::FormWindowBase::defaultDesignerGrid());

    m_formWindow->setDirty(true);

    QDialog::accept();
}

QT_END_NAMESPACE
