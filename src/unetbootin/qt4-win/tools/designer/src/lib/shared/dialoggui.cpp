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

#include "dialoggui_p.h"

QT_BEGIN_NAMESPACE

namespace qdesigner_internal {

DialogGui::DialogGui()
{
}

QMessageBox::StandardButton
        DialogGui::message(QWidget *parent, Message /*context*/, QMessageBox::Icon icon,
                           const QString &title, const QString &text, QMessageBox::StandardButtons buttons,
                           QMessageBox::StandardButton defaultButton)
{
    QMessageBox::StandardButton rc = QMessageBox::NoButton;
    switch (icon) {
    case QMessageBox::Information:
        rc = QMessageBox::information(parent, title, text, buttons, defaultButton);
        break;
    case QMessageBox::Warning:
        rc = QMessageBox::warning(parent, title, text, buttons, defaultButton);
        break;
    case QMessageBox::Critical:
        rc = QMessageBox::critical(parent, title, text, buttons, defaultButton);
        break;
    case QMessageBox::Question:
        rc = QMessageBox::question(parent, title, text, buttons, defaultButton);
        break;
    case QMessageBox::NoIcon:
        break;
    }
    return rc;
}

QMessageBox::StandardButton
        DialogGui::message(QWidget *parent, Message /*context*/, QMessageBox::Icon icon,
                          const QString &title, const QString &text, const QString &informativeText,
                          QMessageBox::StandardButtons buttons, QMessageBox::StandardButton defaultButton)
{
    QMessageBox msgBox(icon, title, text, buttons, parent);
    msgBox.setDefaultButton(defaultButton);
    msgBox.setInformativeText(informativeText);
    return static_cast<QMessageBox::StandardButton>(msgBox.exec());
}

QMessageBox::StandardButton
        DialogGui::message(QWidget *parent, Message /*context*/, QMessageBox::Icon icon,
                          const QString &title, const QString &text, const QString &informativeText, const QString &detailedText,
                          QMessageBox::StandardButtons buttons, QMessageBox::StandardButton defaultButton)
{
    QMessageBox msgBox(icon, title, text, buttons, parent);
    msgBox.setDefaultButton(defaultButton);
    msgBox.setInformativeText(informativeText);
    msgBox.setDetailedText(detailedText);
    return static_cast<QMessageBox::StandardButton>(msgBox.exec());
}

QString DialogGui::getExistingDirectory(QWidget *parent, const QString &caption, const QString &dir, QFileDialog::Options options)
{
    return QFileDialog::getExistingDirectory(parent, caption, dir, options);
}

QString DialogGui::getOpenFileName(QWidget *parent, const QString &caption, const QString &dir, const QString &filter, QString *selectedFilter, QFileDialog::Options options)
{
    return QFileDialog::getOpenFileName(parent, caption, dir, filter, selectedFilter, options);
}

QStringList DialogGui::getOpenFileNames(QWidget *parent, const QString &caption, const QString &dir, const QString &filter, QString *selectedFilter, QFileDialog::Options options)
{
    return QFileDialog::getOpenFileNames(parent, caption, dir, filter, selectedFilter, options);
}

QString DialogGui::getSaveFileName(QWidget *parent, const QString &caption, const QString &dir, const QString &filter, QString *selectedFilter, QFileDialog::Options options)
{
    return QFileDialog::getSaveFileName(parent, caption, dir, filter, selectedFilter, options);
}
}

QT_END_NAMESPACE
