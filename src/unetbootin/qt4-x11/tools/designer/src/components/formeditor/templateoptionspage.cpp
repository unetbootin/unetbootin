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

#include "templateoptionspage.h"
#include "ui_templateoptionspage.h"

#include <shared_settings_p.h>
#include <iconloader_p.h>

#include <QtDesigner/QDesignerFormEditorInterface>
#include <abstractdialoggui_p.h>

QT_BEGIN_NAMESPACE

namespace qdesigner_internal {

// ----------------- TemplateOptionsWidget
TemplateOptionsWidget::TemplateOptionsWidget(QDesignerFormEditorInterface *core, QWidget *parent) :
    QWidget(parent),
    m_core(core),
    m_ui(new Ui::TemplateOptionsWidget)
{
    m_ui->setupUi(this);

    m_ui->m_addTemplatePathButton->setIcon(
            qdesigner_internal::createIconSet(QString::fromUtf8("plus.png")));
    m_ui->m_removeTemplatePathButton->setIcon(
            qdesigner_internal::createIconSet(QString::fromUtf8("minus.png")));

    connect(m_ui->m_templatePathListWidget, SIGNAL(itemSelectionChanged()),
            this, SLOT(templatePathSelectionChanged()));
    connect(m_ui->m_addTemplatePathButton, SIGNAL(clicked()), this, SLOT(addTemplatePath()));
    connect(m_ui->m_removeTemplatePathButton, SIGNAL(clicked()), this, SLOT(removeTemplatePath()));
}

TemplateOptionsWidget::~TemplateOptionsWidget()
{
    delete m_ui;
}

QStringList TemplateOptionsWidget::templatePaths() const
{
    QStringList rc;
    const int count = m_ui->m_templatePathListWidget->count();
    for (int i = 0; i < count; i++) {
        rc += m_ui->m_templatePathListWidget->item(i)->text();
    }
    return rc;
}

void TemplateOptionsWidget::setTemplatePaths(const QStringList &l)
{
    // add paths and select 0
    m_ui->m_templatePathListWidget->clear();
    if (l.empty()) {
        // disable button
        templatePathSelectionChanged();
    } else {
        const QStringList::const_iterator cend = l.constEnd();
        for (QStringList::const_iterator it = l.constBegin(); it != cend; ++it)
            m_ui->m_templatePathListWidget->addItem(*it);
        m_ui->m_templatePathListWidget->setCurrentItem(m_ui->m_templatePathListWidget->item(0));
    }
}

void TemplateOptionsWidget::addTemplatePath()
{
    const QString templatePath = chooseTemplatePath(m_core, this);
    if (templatePath.isEmpty())
        return;

    const QList<QListWidgetItem *> existing
            = m_ui->m_templatePathListWidget->findItems(templatePath, Qt::MatchExactly);
    if (!existing.empty())
        return;

    QListWidgetItem *newItem = new QListWidgetItem(templatePath);
    m_ui->m_templatePathListWidget->addItem(newItem);
    m_ui->m_templatePathListWidget->setCurrentItem(newItem);
}

void TemplateOptionsWidget::removeTemplatePath()
{
    const QList<QListWidgetItem *> selectedPaths
            = m_ui->m_templatePathListWidget->selectedItems();
    if (selectedPaths.empty())
        return;
    delete selectedPaths.front();
}

void TemplateOptionsWidget::templatePathSelectionChanged()
{
    const QList<QListWidgetItem *> selectedPaths = m_ui->m_templatePathListWidget->selectedItems();
    m_ui->m_removeTemplatePathButton->setEnabled(!selectedPaths.empty());
}

QString TemplateOptionsWidget::chooseTemplatePath(QDesignerFormEditorInterface *core, QWidget *parent)
{
    QString rc = core->dialogGui()->getExistingDirectory(parent,
                                                   tr("Pick a directory to save templates in"));
    if (rc.isEmpty())
        return rc;

    if (rc.endsWith(QDir::separator()))
        rc.remove(rc.size() - 1, 1);
    return rc;
}

// ----------------- TemplateOptionsPage
TemplateOptionsPage::TemplateOptionsPage(QDesignerFormEditorInterface *core) :
    m_core(core)
{
}

QString TemplateOptionsPage::name() const
{
    //: Tab in preferences dialog
    return QCoreApplication::translate("TemplateOptionsPage", "Template Paths");
}

QWidget *TemplateOptionsPage::createPage(QWidget *parent)
{
    m_widget = new TemplateOptionsWidget(m_core, parent);
    m_initialTemplatePaths = QDesignerSharedSettings(m_core).additionalFormTemplatePaths();
    m_widget->setTemplatePaths(m_initialTemplatePaths);
    return m_widget;
}

void TemplateOptionsPage::apply()
{
    if (m_widget) {
        const QStringList newTemplatePaths = m_widget->templatePaths();
        if (newTemplatePaths != m_initialTemplatePaths) {
            QDesignerSharedSettings settings(m_core);
            settings.setAdditionalFormTemplatePaths(newTemplatePaths);
            m_initialTemplatePaths = newTemplatePaths;
        }
    }
}

void TemplateOptionsPage::finish()
{
}
}
QT_END_NAMESPACE
