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

#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtGui/QMessageBox>

#include "outputpage.h"

QT_BEGIN_NAMESPACE

OutputPage::OutputPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Output File Names"));
    setSubTitle(tr("Specify the file names for the output files."));
    setButtonText(QWizard::NextButton, tr("Convert..."));

    m_ui.setupUi(this);
    connect(m_ui.projectLineEdit, SIGNAL(textChanged(const QString&)),
        this, SIGNAL(completeChanged()));
    connect(m_ui.collectionLineEdit, SIGNAL(textChanged(const QString&)),
        this, SIGNAL(completeChanged()));

    registerField(QLatin1String("ProjectFileName"),
        m_ui.projectLineEdit);
    registerField(QLatin1String("CollectionFileName"),
        m_ui.collectionLineEdit);
}

void OutputPage::setPath(const QString &path)
{
    m_path = path;
}

void OutputPage::setCollectionComponentEnabled(bool enabled)
{
    m_ui.collectionLineEdit->setEnabled(enabled);
    m_ui.label_2->setEnabled(enabled);
}

bool OutputPage::isComplete() const
{
    if (m_ui.projectLineEdit->text().isEmpty()
        || m_ui.collectionLineEdit->text().isEmpty())
        return false;
    return true;
}

bool OutputPage::validatePage()
{
    return checkFile(m_ui.projectLineEdit->text(),
        tr("Qt Help Project File"))
        && checkFile(m_ui.collectionLineEdit->text(),
        tr("Qt Help Collection Project File"));
}

bool OutputPage::checkFile(const QString &fileName, const QString &title)
{
    QFile fi(m_path + QDir::separator() + fileName);
    if (!fi.exists())
        return true;
    
    if (QMessageBox::warning(this, title,
        tr("The specified file %1 already exist.\n\nDo you want to remove it?")
        .arg(fileName), tr("Remove"), tr("Cancel")) == 0) {
        return fi.remove();
    }
    return false;
}

QT_END_NAMESPACE
