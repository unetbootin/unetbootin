/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt Assistant of the Qt Toolkit.
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
