/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt Assistant of the Qt Toolkit.
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

#include "preferencesdialog.h"
#include "filternamedialog.h"
#include "installdialog.h"
#include "fontpanel.h"
#include "centralwidget.h"

#include <QtAlgorithms>

#include <QtGui/QHeaderView>
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#include <QtGui/QMenu>
#include <QtGui/QFontDatabase>

#include <QtHelp/QHelpEngineCore>

QT_BEGIN_NAMESPACE

PreferencesDialog::PreferencesDialog(QHelpEngineCore *helpEngine, QWidget *parent)
    : QDialog(parent)
    , m_appFontChanged(false)
    , m_browserFontChanged(false)
{
    m_helpEngine = helpEngine;
    m_ui.setupUi(this);

    connect(m_ui.buttonBox->button(QDialogButtonBox::Ok),
        SIGNAL(clicked()), this, SLOT(applyChanges()));
    connect(m_ui.buttonBox->button(QDialogButtonBox::Cancel),
        SIGNAL(clicked()), this, SLOT(reject()));

    m_hideFiltersTab = !m_helpEngine->customValue(QLatin1String("EnableFilterFunctionality"),
        true).toBool();
    m_hideDocsTab = !m_helpEngine->customValue(QLatin1String("EnableDocumentationManager"),
        true).toBool();

    if (!m_hideFiltersTab) {
        m_ui.attributeWidget->header()->hide();
        m_ui.attributeWidget->setRootIsDecorated(false);
        connect(m_ui.attributeWidget, SIGNAL(itemChanged(QTreeWidgetItem*, int)),
            this, SLOT(updateFilterMap()));

        connect(m_ui.filterWidget,
            SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
            this, SLOT(updateAttributes(QListWidgetItem*)));

        connect(m_ui.filterAddButton, SIGNAL(clicked()),
            this, SLOT(addFilter()));
        connect(m_ui.filterRemoveButton, SIGNAL(clicked()),
            this, SLOT(removeFilter()));

        updateFilterPage();
    } else {
        m_ui.tabWidget->removeTab(m_ui.tabWidget->indexOf(m_ui.filtersTab));
    }

    if (!m_hideDocsTab) {
        connect(m_ui.docAddButton, SIGNAL(clicked()), this,
            SLOT(addDocumentationLocal()));
        connect(m_ui.docRemoveButton, SIGNAL(clicked()), this,
            SLOT(removeDocumentation()));

        m_docsBackup = m_helpEngine->registeredDocumentations();
        m_ui.registeredDocsListWidget->addItems(m_docsBackup);
    } else {
        m_ui.tabWidget->removeTab(m_ui.tabWidget->indexOf(m_ui.docsTab));
    }
    updateFontSettingsPage();
    updateOptionsPage();
}

PreferencesDialog::~PreferencesDialog()
{
    if (m_appFontChanged) {
        m_helpEngine->setCustomValue(QLatin1String("appFont"), m_appFontPanel->selectedFont());
        m_helpEngine->setCustomValue(QLatin1String("useAppFont"), m_appFontPanel->isChecked());
        m_helpEngine->setCustomValue(QLatin1String("appWritingSystem"), m_appFontPanel->writingSystem());
    }

    if (m_browserFontChanged) {
        m_helpEngine->setCustomValue(QLatin1String("browserFont"), m_browserFontPanel->selectedFont());
        m_helpEngine->setCustomValue(QLatin1String("useBrowserFont"), m_browserFontPanel->isChecked());
        m_helpEngine->setCustomValue(QLatin1String("browserWritingSystem"), m_browserFontPanel->writingSystem());
    }

    if (m_appFontChanged || m_browserFontChanged) {
        emit updateApplicationFont();
        emit updateBrowserFont();
    }

    if (!m_ui.homePageLineEdit->text().isEmpty())
        m_helpEngine->setCustomValue("homepage", m_ui.homePageLineEdit->text());
}

void PreferencesDialog::showDialog()
{
    if (exec() != Accepted)
        m_appFontChanged = m_browserFontChanged = false;
}

void PreferencesDialog::updateFilterPage()
{
    if (!m_helpEngine)
        return;

    m_ui.filterWidget->clear();
    m_ui.attributeWidget->clear();

    QHelpEngineCore help(m_helpEngine->collectionFile(), 0);
    help.setupData();
    m_filterMapBackup.clear();
    const QStringList filters = help.customFilters();
    foreach (const QString &filter, filters) {
        QStringList atts = help.filterAttributes(filter);
        m_filterMapBackup.insert(filter, atts);
        if (!m_filterMap.contains(filter))
            m_filterMap.insert(filter, atts);
    }

    m_ui.filterWidget->addItems(m_filterMap.keys());

    foreach (const QString &a, help.filterAttributes())
        new QTreeWidgetItem(m_ui.attributeWidget, QStringList() << a);

    if (m_filterMap.keys().count())
        m_ui.filterWidget->setCurrentRow(0);
}

void PreferencesDialog::updateAttributes(QListWidgetItem *item)
{
    QStringList checkedList;
    if (item)
        checkedList = m_filterMap.value(item->text());
    QTreeWidgetItem *itm;
    for (int i=0; i<m_ui.attributeWidget->topLevelItemCount(); ++i) {
        itm = m_ui.attributeWidget->topLevelItem(i);
        if (checkedList.contains(itm->text(0)))
            itm->setCheckState(0, Qt::Checked);
        else
            itm->setCheckState(0, Qt::Unchecked);
    }
}

void PreferencesDialog::updateFilterMap()
{
    if (!m_ui.filterWidget->currentItem())
        return;
    QString filter = m_ui.filterWidget->currentItem()->text();
    if (!m_filterMap.contains(filter))
        return;

    QStringList newAtts;
    QTreeWidgetItem *itm = 0;
    for (int i=0; i<m_ui.attributeWidget->topLevelItemCount(); ++i) {
        itm = m_ui.attributeWidget->topLevelItem(i);
        if (itm->checkState(0) == Qt::Checked)
            newAtts.append(itm->text(0));
    }
    m_filterMap[filter] = newAtts;
}

void PreferencesDialog::addFilter()
{
    FilterNameDialog dia(this);
    if (dia.exec() == QDialog::Rejected)
        return;

    QString filterName = dia.filterName();
    if (!m_filterMap.contains(filterName)) {
        m_filterMap.insert(filterName, QStringList());
        m_ui.filterWidget->addItem(filterName);
    }

    QList<QListWidgetItem*> lst = m_ui.filterWidget
        ->findItems(filterName, Qt::MatchCaseSensitive);
    m_ui.filterWidget->setCurrentItem(lst.first());
}

void PreferencesDialog::removeFilter()
{
    QListWidgetItem *item = m_ui.filterWidget
        ->takeItem(m_ui.filterWidget->currentRow());
    if (!item)
        return;

    m_filterMap.remove(item->text());
    m_removedFilters.append(item->text());
    delete item;
    if (m_ui.filterWidget->count())
        m_ui.filterWidget->setCurrentRow(0);
}

void PreferencesDialog::addDocumentationLocal()
{
    const QStringList fileNames = QFileDialog::getOpenFileNames(this,
        tr("Add Documentation"), QString(), tr("Qt Compressed Help Files (*.qch)"));
    if (fileNames.isEmpty())
        return;

    foreach (const QString &fileName, fileNames) {
        const QString ns = QHelpEngineCore::namespaceName(fileName);
        if (ns.isEmpty()) {
            QMessageBox::warning(this, tr("Add Documentation"),
                tr("The specified file is not a valid Qt Help File!"));
            continue;
        }

        if (m_ui.registeredDocsListWidget->findItems(ns, Qt::MatchFixedString).count()) {
            QMessageBox::warning(this, tr("Add Documentation"),
                tr("The namespace %1 is already registered!").arg(ns));
            continue;
        }

        m_helpEngine->registerDocumentation(fileName);
        m_ui.registeredDocsListWidget->addItem(ns);
        m_regDocs.append(ns);
        m_unregDocs.removeAll(ns);
    }
    updateFilterPage();
}

void PreferencesDialog::removeDocumentation()
{
    bool foundBefore = false;
    CentralWidget* widget = CentralWidget::instance();
    QMap<int, QString> openedDocList = widget->currentSourceFileList();
    QStringList values(openedDocList.values());

    QList<QListWidgetItem*> l = m_ui.registeredDocsListWidget->selectedItems();
    foreach (QListWidgetItem* item, l) {
        const QString& ns = item->text();
        if (!foundBefore && values.contains(ns)) {
            if (0 == QMessageBox::information(this, tr("Remove Documentation"),
                tr("Some documents currently opened in Assistant reference the "
                   "documentation you are attempting to remove. Removing the "
                   "documentation will close those documents."), tr("Cancel"),
                tr("OK"))) return;
            foundBefore = true;
        }

        m_unregDocs.append(ns);
        m_TabsToClose += openedDocList.keys(ns);
        delete m_ui.registeredDocsListWidget->takeItem(
            m_ui.registeredDocsListWidget->row(item));
    }

    if (m_ui.registeredDocsListWidget->count()) {
        m_ui.registeredDocsListWidget->setCurrentRow(0,
            QItemSelectionModel::ClearAndSelect);
    }
}

void PreferencesDialog::applyChanges()
{
    bool filtersWereChanged = false;
    if (!m_hideFiltersTab) {
        if (m_filterMap.count() != m_filterMapBackup.count()) {
            filtersWereChanged = true;
        } else {
            QMapIterator<QString, QStringList> it(m_filterMapBackup);
            while (it.hasNext() && !filtersWereChanged) {
                it.next();
                if (!m_filterMap.contains(it.key())) {
                    filtersWereChanged = true;
                } else {
                    QStringList a = it.value();
                    QStringList b = m_filterMap.value(it.key());
                    if (a.count() != b.count()) {
                        filtersWereChanged = true;
                    } else {
                        QStringList::const_iterator i(a.constBegin());
                        while (i != a.constEnd()) {
                            if (!b.contains(*i)) {
                                filtersWereChanged = true;
                                break;
                            }
                            ++i;
                        }
                    }
                }
            }
        }
    }

    if (filtersWereChanged) {
        foreach (const QString &filter, m_removedFilters)
            m_helpEngine->removeCustomFilter(filter);
        QMapIterator<QString, QStringList> it(m_filterMap);
        while (it.hasNext()) {
            it.next();
            m_helpEngine->addCustomFilter(it.key(), it.value());
        }
    }

    qSort(m_TabsToClose);
    CentralWidget* widget = CentralWidget::instance();
    for (int i = m_TabsToClose.count(); --i >= 0;)
        widget->closeTabAt(m_TabsToClose.at(i));

    if (m_unregDocs.count()) {
        foreach (const QString &doc, m_unregDocs)
            m_helpEngine->unregisterDocumentation(doc);
    }

    if (filtersWereChanged || m_regDocs.count() || m_unregDocs.count())
        m_helpEngine->setupData();

    accept();
}

void PreferencesDialog::updateFontSettingsPage()
{
    m_browserFontPanel = new FontPanel(this);
    m_browserFontPanel->setCheckable(true);
    m_ui.stackedWidget_2->insertWidget(0, m_browserFontPanel);

    m_appFontPanel = new FontPanel(this);
    m_appFontPanel->setCheckable(true);
    m_ui.stackedWidget_2->insertWidget(1, m_appFontPanel);

    m_ui.stackedWidget_2->setCurrentIndex(0);

    const QString customSettings(tr("Use custom settings"));
    QFont font = qVariantValue<QFont>(m_helpEngine->customValue(QLatin1String("appFont")));
    QFontDatabase::WritingSystem writingSystem = static_cast<QFontDatabase::WritingSystem>
        (m_helpEngine->customValue(QLatin1String("appWritingSystem")).toInt());

    m_appFontPanel->setTitle(customSettings);
    m_appFontPanel->setSelectedFont(font);
    m_appFontPanel->setWritingSystem(writingSystem);
    m_appFontPanel->setChecked(m_helpEngine->customValue(QLatin1String("useAppFont")).toBool());

    QFont font2 = qVariantValue<QFont>(m_helpEngine->customValue(QLatin1String("browserFont")));
    writingSystem = static_cast<QFontDatabase::WritingSystem>
        (m_helpEngine->customValue(QLatin1String("browserWritingSystem")).toInt());

    m_browserFontPanel->setTitle(customSettings);
    m_browserFontPanel->setSelectedFont(font2);
    m_browserFontPanel->setWritingSystem(writingSystem);
    m_browserFontPanel->setChecked(m_helpEngine->customValue(QLatin1String("useBrowserFont")).toBool());

    connect(m_appFontPanel, SIGNAL(toggled(bool)), this, SLOT(appFontSettingToggled(bool)));
    connect(m_browserFontPanel, SIGNAL(toggled(bool)), this, SLOT(browserFontSettingToggled(bool)));

    QList<QComboBox*> allCombos = qFindChildren<QComboBox*>(m_appFontPanel);
    foreach (QComboBox* box, allCombos)
        connect(box, SIGNAL(currentIndexChanged(int)), this, SLOT(appFontSettingChanged(int)));

    allCombos.clear();
    allCombos = qFindChildren<QComboBox*>(m_browserFontPanel);
    foreach (QComboBox* box, allCombos)
        connect(box, SIGNAL(currentIndexChanged(int)), this, SLOT(browserFontSettingChanged(int)));
}

void PreferencesDialog::appFontSettingToggled(bool on)
{
    Q_UNUSED(on)
    m_appFontChanged = true;
}

void PreferencesDialog::appFontSettingChanged(int index)
{
    Q_UNUSED(index)
    m_appFontChanged = true;
}

void PreferencesDialog::browserFontSettingToggled(bool on)
{
    Q_UNUSED(on)
    m_browserFontChanged = true;
}

void PreferencesDialog::browserFontSettingChanged(int index)
{
    Q_UNUSED(index)
    m_browserFontChanged = true;
}

void PreferencesDialog::updateOptionsPage()
{
    QString homepage = m_helpEngine->customValue(QLatin1String("homepage"),
        QLatin1String("")).toString();

    if (homepage.isEmpty()) {
        homepage = m_helpEngine->customValue(QLatin1String("defaultHomepage"),
            QLatin1String("help")).toString();
    }

    m_ui.homePageLineEdit->setText(homepage);
    connect(m_ui.currentPageButton, SIGNAL(pressed()), this,
        SLOT(currentHomepageChanged()));
    connect(m_ui.restoreDefaultHomePageButton, SIGNAL(pressed()), this,
        SLOT(restoreDefaultHomepage()));
}

void PreferencesDialog::restoreDefaultHomepage()
{
    QString homepage = m_helpEngine->customValue(
        QLatin1String("defaultHomepage"), QLatin1String("help")).toString();
    m_ui.homePageLineEdit->setText(homepage);
}

void PreferencesDialog::currentHomepageChanged()
{
    QString homepage = CentralWidget::instance()->currentSource().toString();
    if (homepage.isEmpty())
        homepage = QLatin1String("help");

    m_ui.homePageLineEdit->setText(homepage);
}

QT_END_NAMESPACE
