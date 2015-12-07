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

#include <QtGui/QMessageBox>
#include "generalpage.h"

QT_BEGIN_NAMESPACE

GeneralPage::GeneralPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("General Settings"));
    setSubTitle(tr("Specify the namespace and the virtual "
        "folder for the documentation."));

    m_ui.setupUi(this);
    connect(m_ui.namespaceLineEdit, SIGNAL(textChanged(const QString&)),
        this, SIGNAL(completeChanged()));
    connect(m_ui.folderLineEdit, SIGNAL(textChanged(const QString&)),
        this, SIGNAL(completeChanged()));

    m_ui.namespaceLineEdit->setText(QLatin1String("mycompany.com"));
    m_ui.folderLineEdit->setText(QLatin1String("product_1.0"));

    registerField(QLatin1String("namespaceName"), m_ui.namespaceLineEdit);
    registerField(QLatin1String("virtualFolder"), m_ui.folderLineEdit);
}

bool GeneralPage::isComplete() const
{
    if (m_ui.namespaceLineEdit->text().isEmpty()
        || m_ui.folderLineEdit->text().isEmpty())
        return false;
    return true;
}

bool GeneralPage::validatePage()
{
    QString s = m_ui.namespaceLineEdit->text();
    if (s.contains(QLatin1Char('/')) || s.contains(QLatin1Char('\\'))) {
        QMessageBox::critical(this, tr("Namespace Error"),
            tr("The namespace contains some invalid characters."));
        return false;
    }
    s = m_ui.folderLineEdit->text();
    if (s.contains(QLatin1Char('/')) || s.contains(QLatin1Char('\\'))) {
        QMessageBox::critical(this, tr("Virtual Folder Error"),
            tr("The virtual folder contains some invalid characters."));
        return false;
    }
    return true;
}

QT_END_NAMESPACE
