/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
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
