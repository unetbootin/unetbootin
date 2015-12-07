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

#include "fontsettingsdialog.h"
#include "fontpanel.h"
#include "config.h"

#include <QtGui/QLabel>
#include <QtGui/QComboBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QApplication>
#include <QtGui/QStackedWidget>
#include <QtGui/QDialogButtonBox>

QT_BEGIN_NAMESPACE

FontSettingsDialog::FontSettingsDialog(QWidget *parent)
    : QDialog(parent)
    , m_windowFontPanel(new FontPanel(this))
    , m_browserFontPanel(new FontPanel(this))
    , m_dialogButtonBox(new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel))
{
    setModal(true);
    setWindowTitle(tr("Font Settings"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    QVBoxLayout *mainVLayout = new QVBoxLayout(this);
    QHBoxLayout *hboxLayout = new QHBoxLayout;
    mainVLayout->addLayout(hboxLayout);

    QLabel *label = new QLabel(tr("Font settings for:"), this);
    label->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    hboxLayout->addWidget(label);
    QComboBox *comboBox = new QComboBox(this);
    comboBox->addItem(tr("Browser"));
    comboBox->addItem(tr("Application"));
    hboxLayout->addWidget(comboBox);

    m_windowFontPanel->setCheckable(true);
    m_browserFontPanel->setCheckable(true);

    const QString customSettings(tr("Use custom settings"));
    m_windowFontPanel->setTitle(customSettings);
    m_browserFontPanel->setTitle(customSettings);

    QStackedWidget *stackWidget = new QStackedWidget(this);
    stackWidget->addWidget(m_browserFontPanel);
    stackWidget->addWidget(m_windowFontPanel);

    mainVLayout->addWidget(stackWidget);
    mainVLayout->addWidget(m_dialogButtonBox);

    connect(m_dialogButtonBox , SIGNAL(rejected()), this, SLOT(reject()));
    connect(m_dialogButtonBox , SIGNAL(accepted()), this, SLOT(accept()));
    connect(comboBox, SIGNAL(activated(int)), stackWidget, SLOT(setCurrentIndex(int)));
}

FontSettingsDialog::~FontSettingsDialog()
{
    // nothing todo
}

bool FontSettingsDialog::showDialog(FontSettings *settings)
{
    setupFontSettingsDialog(settings);

    if (exec() != Accepted)
        return false;

    updateFontSettings(settings);
    return true;
}

void FontSettingsDialog::updateFontSettings(FontSettings *settings)
{
    settings->useWindowFont = m_windowFontPanel->isChecked();
    settings->useBrowserFont = m_browserFontPanel->isChecked();

    settings->windowFont = settings->useWindowFont ? m_windowFontPanel->selectedFont() : qApp->font();
    settings->browserFont = settings->useBrowserFont ? m_browserFontPanel->selectedFont() : qApp->font();

    settings->windowWritingSystem = settings->useWindowFont ? m_windowFontPanel->writingSystem() : QFontDatabase::Latin;
    settings->browserWritingSystem = settings->useBrowserFont ? m_browserFontPanel->writingSystem() : QFontDatabase::Latin;
}

void FontSettingsDialog::setupFontSettingsDialog(const FontSettings *settings)
{
    m_windowFontPanel->setSelectedFont(settings->windowFont);
    m_browserFontPanel->setSelectedFont(settings->browserFont);

    m_windowFontPanel->setWritingSystem(settings->windowWritingSystem);
    m_browserFontPanel->setWritingSystem(settings->browserWritingSystem);

    m_windowFontPanel->setChecked(settings->useWindowFont);
    m_browserFontPanel->setChecked(settings->useBrowserFont);
}

QT_END_NAMESPACE
