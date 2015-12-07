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

#include <QtCore/QFile>
#include <QtCore/QVariant>

#include <QtGui/QLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QToolButton>
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#include <QtGui/QApplication>

#include "inputpage.h"
#include "adpreader.h"

QT_BEGIN_NAMESPACE

InputPage::InputPage(AdpReader *reader, QWidget *parent)
    : QWizardPage(parent)
{
    m_adpReader = reader;
    setTitle(tr("Input File"));
    setSubTitle(tr("Specify the .adp or .dcf file you want "
        "to convert to the new Qt help project format and/or "
        "collection format."));

    m_ui.setupUi(this);
    connect(m_ui.browseButton, SIGNAL(clicked()),
        this, SLOT(getFileName()));

    registerField(QLatin1String("adpFileName"), m_ui.fileLineEdit);
}

void InputPage::getFileName()
{
    QString f = QFileDialog::getOpenFileName(this, tr("Open file"), QString(),
        tr("Qt Help Files (*.adp *.dcf)"));
    if (!f.isEmpty())
        m_ui.fileLineEdit->setText(f);
}

bool InputPage::validatePage()
{
    QFile f(m_ui.fileLineEdit->text().trimmed());
    if (!f.exists() || !f.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, tr("File Open Error"),
            tr("The specified file could not be opened!"));
        return false;
    }

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    m_adpReader->readData(f.readAll());
    QApplication::restoreOverrideCursor();
    if (m_adpReader->hasError()) {
        QMessageBox::critical(this, tr("File Parsing Error"),
            tr("Parsing error in line %1!").arg(m_adpReader->lineNumber()));
        return false;
    }

    return true;
}

QT_END_NAMESPACE
