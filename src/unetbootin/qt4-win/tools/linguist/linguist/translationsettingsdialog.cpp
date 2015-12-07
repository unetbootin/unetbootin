/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt Linguist of the Qt Toolkit.
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


#include "translationsettingsdialog.h"
#include "messagemodel.h"
#include <QtCore/QLocale>

QT_BEGIN_NAMESPACE

TranslationSettingsDialog::TranslationSettingsDialog(QWidget *w /*= 0*/) : QDialog(w)
{
    m_ui.setupUi(this);

    for (int i = QLocale::C + 1; i < QLocale::LastLanguage; ++i) {
        QString lang = QLocale::languageToString(QLocale::Language(i));
        m_ui.cbLanguageList->addItem(lang, QVariant(int(i)));
    }
    m_ui.cbLanguageList->model()->sort(0, Qt::AscendingOrder);

    for (int i = QLocale::AnyCountry; i < QLocale::LastCountry; ++i) {
        QString country = QLocale::countryToString(QLocale::Country(i));
        m_ui.cbCountryList->addItem(country, QVariant(int(i)));
    }
    m_ui.cbCountryList->model()->sort(0, Qt::AscendingOrder);
    m_ui.cbCountryList->insertItem(0, tr("Any Country"), QVariant(0));

}

void TranslationSettingsDialog::setMessageModel(MessageModel *model)
{
    m_messageModel = model;
}

void TranslationSettingsDialog::on_buttonBox_accepted()
{
    int itemindex = m_ui.cbLanguageList->currentIndex();
    QVariant var = m_ui.cbLanguageList->itemData(itemindex);
    QLocale::Language lang = QLocale::Language(var.toInt());
    m_messageModel->setLanguage(lang);

    itemindex = m_ui.cbCountryList->currentIndex();
    var = m_ui.cbCountryList->itemData(itemindex);
    QLocale::Country country = QLocale::Country(var.toInt());
    m_messageModel->setCountry(country);
    accept();
}

void TranslationSettingsDialog::showEvent(QShowEvent *e)
{
    Q_UNUSED(e);
    QLocale::Language lang = m_messageModel->language();
    int itemindex = m_ui.cbLanguageList->findData(QVariant(int(lang)));
    m_ui.cbLanguageList->setCurrentIndex(itemindex == -1 ? 0 : itemindex);


    QLocale::Country country = m_messageModel->country();
    itemindex = m_ui.cbCountryList->findData(QVariant(int(country)));
    m_ui.cbCountryList->setCurrentIndex(itemindex == -1 ? 0 : itemindex);
}

QT_END_NAMESPACE
