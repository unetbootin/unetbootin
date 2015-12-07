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

#include "formwindowsettings.h"
#include "ui_formwindowsettings.h"

#include <formwindowbase_p.h>
#include <grid_p.h>

#include <QtGui/QStyle>

#include <QtCore/QRegExp>
#include <QtCore/QDebug>

QT_BEGIN_NAMESPACE

namespace qdesigner_internal {

// Data structure containing form dialog data providing comparison
struct FormWindowData {
    FormWindowData();

    bool equals(const FormWindowData&) const;

    void fromFormWindow(FormWindowBase* fw);
    void applyToFormWindow(FormWindowBase* fw) const;

    bool layoutDefaultEnabled;
    int defaultMargin;
    int defaultSpacing;

    bool layoutFunctionsEnabled;
    QString marginFunction;
    QString spacingFunction;

    QString pixFunction;

    QString author;

    QStringList includeHints;

    bool hasFormGrid;
    Grid grid;
};

inline bool operator==(const FormWindowData &fd1, const FormWindowData &fd2) { return fd1.equals(fd2); }
inline bool operator!=(const FormWindowData &fd1, const FormWindowData &fd2) { return !fd1.equals(fd2); }

QDebug operator<<(QDebug str, const  FormWindowData &d)
{
    str.nospace() << "LayoutDefault=" << d.layoutDefaultEnabled        << ',' << d.defaultMargin
        <<  ',' << d.defaultSpacing << " LayoutFunctions=" << d.layoutFunctionsEnabled << ','
        << d.marginFunction << ',' << d.spacingFunction << " PixFunction="
        << d.pixFunction << " Author=" << d.author << " Hints=" << d.includeHints
        << " Grid=" << d.hasFormGrid << d.grid.deltaX() << d.grid.deltaY() << '\n';
    return str;
}

FormWindowData::FormWindowData() :
    layoutDefaultEnabled(false),
    defaultMargin(0),
    defaultSpacing(0),
    layoutFunctionsEnabled(false),
    hasFormGrid(false)
{
}

bool FormWindowData::equals(const FormWindowData &rhs) const
{
    return layoutDefaultEnabled   == rhs.layoutDefaultEnabled &&
           defaultMargin          == rhs.defaultMargin &&
           defaultSpacing         == rhs.defaultSpacing &&
           layoutFunctionsEnabled == rhs.layoutFunctionsEnabled &&
           marginFunction         == rhs.marginFunction &&
           spacingFunction        == rhs.spacingFunction &&
           pixFunction            == rhs.pixFunction  &&
           author                 == rhs.author &&
           includeHints           == rhs.includeHints &&
           hasFormGrid            == rhs.hasFormGrid &&
           grid                   == rhs.grid;
}

void FormWindowData::fromFormWindow(FormWindowBase* fw)
{
    defaultMargin =  defaultSpacing = INT_MIN;
    fw->layoutDefault(&defaultMargin, &defaultSpacing);

    QStyle *style = fw->formContainer()->style();
    layoutDefaultEnabled = defaultMargin != INT_MIN || defaultMargin != INT_MIN;
    if (defaultMargin == INT_MIN)
        defaultMargin = style->pixelMetric(QStyle::PM_DefaultChildMargin, 0);
    if (defaultSpacing == INT_MIN)
        defaultSpacing = style->pixelMetric(QStyle::PM_DefaultLayoutSpacing, 0);


    marginFunction.clear();
    spacingFunction.clear();
    fw->layoutFunction(&marginFunction, &spacingFunction);
    layoutFunctionsEnabled = !marginFunction.isEmpty() || !spacingFunction.isEmpty();

    pixFunction = fw->pixmapFunction();

    author = fw->author();

    includeHints = fw->includeHints();
    includeHints.removeAll(QString());

    hasFormGrid = fw->hasFormGrid();
    grid = hasFormGrid ? fw->designerGrid() : FormWindowBase::defaultDesignerGrid();
}

void FormWindowData::applyToFormWindow(FormWindowBase* fw) const
{
    fw->setAuthor(author);
    fw->setPixmapFunction(pixFunction);

    if (layoutDefaultEnabled) {
        fw->setLayoutDefault(defaultMargin, defaultSpacing);
    } else {
        fw->setLayoutDefault(INT_MIN, INT_MIN);
    }

    if (layoutFunctionsEnabled) {
        fw->setLayoutFunction(marginFunction, spacingFunction);
    } else {
        fw->setLayoutFunction(QString(), QString());
    }

    fw->setIncludeHints(includeHints);

    const bool hadFormGrid = fw->hasFormGrid();
    fw->setHasFormGrid(hasFormGrid);
    if (hasFormGrid || hadFormGrid != hasFormGrid)
        fw->setDesignerGrid(hasFormGrid ? grid : FormWindowBase::defaultDesignerGrid());
}

// -------------------------- FormWindowSettings

FormWindowSettings::FormWindowSettings(QDesignerFormWindowInterface *parent) :
    QDialog(parent),
    m_ui(new ::Ui::FormWindowSettings),
    m_formWindow(qobject_cast<FormWindowBase*>(parent)),
    m_oldData(new FormWindowData)
{
    Q_ASSERT(m_formWindow);

    m_ui->setupUi(this);
    m_ui->gridPanel->setCheckable(true);
    m_ui->gridPanel->setResetButtonVisible(false);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    QString deviceProfileName = m_formWindow->deviceProfileName();
    if (deviceProfileName.isEmpty())
        deviceProfileName = tr("None");
    m_ui->deviceProfileLabel->setText(tr("Device Profile: %1").arg(deviceProfileName));

    m_oldData->fromFormWindow(m_formWindow);
    setData(*m_oldData);
}

FormWindowSettings::~FormWindowSettings()
{
    delete m_oldData;
    delete m_ui;
}

FormWindowData FormWindowSettings::data() const
{
    FormWindowData rc;
    rc.author = m_ui->authorLineEdit->text();

    if (m_ui->pixmapFunctionGroupBox->isChecked()) {
        rc.pixFunction = m_ui->pixmapFunctionLineEdit->text();
    } else {
        rc.pixFunction.clear();
    }

    rc.layoutDefaultEnabled = m_ui->layoutDefaultGroupBox->isChecked();
    rc.defaultMargin = m_ui->defaultMarginSpinBox->value();
    rc.defaultSpacing = m_ui->defaultSpacingSpinBox->value();

    rc.layoutFunctionsEnabled = m_ui->layoutFunctionGroupBox->isChecked();
    rc.marginFunction = m_ui->marginFunctionLineEdit->text();
    rc.spacingFunction = m_ui->spacingFunctionLineEdit->text();

    const QString hints = m_ui->includeHintsTextEdit->toPlainText();
    if (!hints.isEmpty()) {
        rc.includeHints = hints.split(QString(QLatin1Char('\n')));
        // Purge out any lines consisting of blanks only
        const QRegExp blankLine = QRegExp(QLatin1String("^\\s*$"));
        Q_ASSERT(blankLine.isValid());
        for (QStringList::iterator it = rc.includeHints.begin(); it != rc.includeHints.end(); )
            if (blankLine.exactMatch(*it)) {
                it = rc.includeHints.erase(it);
            } else {
                ++it;
            }
        rc.includeHints.removeAll(QString());
    }

    rc.hasFormGrid = m_ui->gridPanel->isChecked();
    rc.grid = m_ui->gridPanel->grid();
    return rc;
}

void FormWindowSettings::setData(const FormWindowData &data)
{
    m_ui->layoutDefaultGroupBox->setChecked(data.layoutDefaultEnabled);
    m_ui->defaultMarginSpinBox->setValue(data.defaultMargin);
    m_ui->defaultSpacingSpinBox->setValue(data.defaultSpacing);

    m_ui->layoutFunctionGroupBox->setChecked(data.layoutFunctionsEnabled);
    m_ui->marginFunctionLineEdit->setText(data.marginFunction);
    m_ui->spacingFunctionLineEdit->setText(data.spacingFunction);

    m_ui->pixmapFunctionLineEdit->setText(data.pixFunction);
    m_ui->pixmapFunctionGroupBox->setChecked(!data.pixFunction.isEmpty());

    m_ui->authorLineEdit->setText(data.author);

    if (data.includeHints.empty()) {
        m_ui->includeHintsTextEdit->clear();
    } else {
        m_ui->includeHintsTextEdit->setText(data.includeHints.join(QLatin1String("\n")));
    }

    m_ui->gridPanel->setChecked(data.hasFormGrid);
    m_ui->gridPanel->setGrid(data.grid);
}

void FormWindowSettings::accept()
{
    // Anything changed? -> Apply and set dirty
    const FormWindowData newData = data();
    if (newData != *m_oldData) {
        newData.applyToFormWindow(m_formWindow);
        m_formWindow->setDirty(true);
    }

    QDialog::accept();
}
}
QT_END_NAMESPACE
