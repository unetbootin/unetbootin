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

#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"
#include "preferences.h"
#include <iconloader_p.h>

#include <QtGui/QFileDialog>

QT_BEGIN_NAMESPACE

PreferencesDialog::PreferencesDialog(QDesignerFormEditorInterface *core, QWidget *parentWidget) :
    QDialog(parentWidget)
{
    m_ui = new Ui::PreferencesDialog();
    m_ui->setupUi(this);

    m_ui->m_addTemplatePathButton->setIcon(qdesigner_internal::createIconSet(QString::fromUtf8("plus.png")));
    m_ui->m_removeTemplatePathButton->setIcon(qdesigner_internal::createIconSet(QString::fromUtf8("minus.png")));

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    m_ui->m_uiModeCombo->addItem(tr("Docked Window"), QVariant(DockedMode));
    m_ui->m_uiModeCombo->addItem(tr("Multiple Top-Level Windows"), QVariant(TopLevelMode));

    m_ui->m_fontPanel->setCheckable(true);
    m_ui->m_fontPanel->setTitle(tr("Toolwindow Font"));
    m_ui->m_gridPanel->setTitle(tr("Default Grid"));

    m_ui->m_previewConfigurationWidget->setCore(core);

    connect(m_ui->m_templatePathListWidget, SIGNAL(itemSelectionChanged()), this, SLOT(templatePathSelectionChanged()));
    connect(m_ui->m_addTemplatePathButton, SIGNAL(clicked()), this, SLOT(addTemplatePath()));
    connect(m_ui->m_removeTemplatePathButton, SIGNAL(clicked()), this, SLOT(removeTemplatePath()));
    connect(m_ui->m_dialogButtonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(m_ui->m_dialogButtonBox, SIGNAL(accepted()), this, SLOT(accept()));
}

PreferencesDialog::~PreferencesDialog()
{
    delete m_ui;
}

bool PreferencesDialog::showDialog(Preferences &p)
{
    setPreferences(p);
    if (exec() != Accepted)
        return false;

    getPreferences(p);
    return true;
}

void PreferencesDialog::setPreferences(const Preferences &p)
{
    m_ui->m_uiModeCombo->setCurrentIndex(m_ui->m_uiModeCombo->findData(QVariant(p.m_uiMode)));
    m_ui->m_fontPanel->setWritingSystem(p.m_writingSystem);
    m_ui->m_fontPanel->setSelectedFont(p.m_font);
    m_ui->m_fontPanel->setChecked(p.m_useFont);
    // add paths and select 0
    m_ui->m_templatePathListWidget->clear();
    if (p.m_additionalTemplatePaths.empty()) {
        // disable button
        templatePathSelectionChanged();
    } else {
        foreach (QString templatePath, p.m_additionalTemplatePaths)
            m_ui->m_templatePathListWidget->addItem(templatePath);
        m_ui->m_templatePathListWidget->setCurrentItem(m_ui->m_templatePathListWidget->item(0));
    }
    m_ui->m_gridPanel->setGrid(p.m_defaultGrid);
    m_ui->m_previewConfigurationWidget->setPreviewConfigurationWidgetState(p.m_previewConfigurationWidgetState);
    m_ui->m_previewConfigurationWidget->setPreviewConfiguration(p.m_previewConfiguration);
}

void PreferencesDialog::getPreferences(Preferences &p) const
{
    p.m_uiMode = static_cast<UIMode>(m_ui->m_uiModeCombo->itemData(m_ui->m_uiModeCombo->currentIndex()).toInt());
    p.m_font = m_ui->m_fontPanel->selectedFont();
    p.m_useFont = m_ui->m_fontPanel->isChecked();
    p.m_writingSystem = m_ui->m_fontPanel->writingSystem();

    p.m_additionalTemplatePaths.clear();
    const int count = m_ui->m_templatePathListWidget->count();
    for (int i = 0; i <  count; i++) {
        p.m_additionalTemplatePaths += m_ui->m_templatePathListWidget->item(i)->text();
    }
    p.m_defaultGrid = m_ui->m_gridPanel->grid();
    p.m_previewConfigurationWidgetState = m_ui->m_previewConfigurationWidget->previewConfigurationWidgetState();
    p.m_previewConfiguration = m_ui->m_previewConfigurationWidget->previewConfiguration();
}

void PreferencesDialog::addTemplatePath()
{
    const QString templatePath = chooseTemplatePath(this);
    if (templatePath.isEmpty())
        return;

    const QList<QListWidgetItem *> existing = m_ui->m_templatePathListWidget->findItems(templatePath, Qt::MatchExactly);
    if (!existing.empty())
        return;

    QListWidgetItem *newItem = new QListWidgetItem(templatePath);
    m_ui->m_templatePathListWidget->addItem(newItem);
    m_ui->m_templatePathListWidget->setCurrentItem(newItem);
}


void PreferencesDialog::removeTemplatePath()
{
    const QList<QListWidgetItem *> selectedPaths = m_ui->m_templatePathListWidget->selectedItems();
    if (selectedPaths.empty())
        return;
    delete selectedPaths.front();
}

void PreferencesDialog::templatePathSelectionChanged()
{
    const QList<QListWidgetItem *> selectedPaths = m_ui->m_templatePathListWidget->selectedItems();
    m_ui->m_removeTemplatePathButton->setEnabled(!selectedPaths.empty());
}

QString PreferencesDialog::chooseTemplatePath(QWidget *parent)
{
    QString rc = QFileDialog::getExistingDirectory(parent, tr("Pick a directory to save templates in"));
    if (rc.isEmpty())
        return rc;

    if (rc.endsWith(QDir::separator()))
        rc.remove(rc.size() - 1, 1);
    return rc;
}

QT_END_NAMESPACE
