/*
*********************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt Linguist of the Qt Toolkit.
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
*********************************************************************
*/

/********************************************************************************
** Form generated from reading ui file 'translatedialog.ui'
**
** Created: Thu Mar 5 20:53:50 2009
**      by: Qt User Interface Compiler version 4.5.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_TRANSLATEDIALOG_H
#define UI_TRANSLATEDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_TranslateDialog
{
public:
    QHBoxLayout *hboxLayout;
    QVBoxLayout *vboxLayout;
    QGridLayout *gridLayout;
    QLineEdit *ledTranslateTo;
    QLabel *findWhat;
    QLabel *translateTo;
    QLineEdit *ledFindWhat;
    QGroupBox *groupBox;
    QVBoxLayout *vboxLayout1;
    QCheckBox *ckMatchCase;
    QCheckBox *ckMarkFinished;
    QSpacerItem *spacerItem;
    QVBoxLayout *vboxLayout2;
    QPushButton *findNxt;
    QPushButton *translate;
    QPushButton *translateAll;
    QPushButton *cancel;
    QSpacerItem *spacerItem1;

    void setupUi(QDialog *translateDialog)
    {
        if (translateDialog->objectName().isEmpty())
            translateDialog->setObjectName(QString::fromUtf8("translateDialog"));
        translateDialog->resize(407, 174);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(translateDialog->sizePolicy().hasHeightForWidth());
        translateDialog->setSizePolicy(sizePolicy);
        hboxLayout = new QHBoxLayout(translateDialog);
        hboxLayout->setSpacing(6);
        hboxLayout->setMargin(9);
        hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
        vboxLayout = new QVBoxLayout();
        vboxLayout->setSpacing(6);
        vboxLayout->setMargin(0);
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        gridLayout = new QGridLayout();
        gridLayout->setSpacing(6);
        gridLayout->setMargin(0);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        ledTranslateTo = new QLineEdit(translateDialog);
        ledTranslateTo->setObjectName(QString::fromUtf8("ledTranslateTo"));

        gridLayout->addWidget(ledTranslateTo, 1, 1, 1, 1);

        findWhat = new QLabel(translateDialog);
        findWhat->setObjectName(QString::fromUtf8("findWhat"));

        gridLayout->addWidget(findWhat, 0, 0, 1, 1);

        translateTo = new QLabel(translateDialog);
        translateTo->setObjectName(QString::fromUtf8("translateTo"));

        gridLayout->addWidget(translateTo, 1, 0, 1, 1);

        ledFindWhat = new QLineEdit(translateDialog);
        ledFindWhat->setObjectName(QString::fromUtf8("ledFindWhat"));

        gridLayout->addWidget(ledFindWhat, 0, 1, 1, 1);


        vboxLayout->addLayout(gridLayout);

        groupBox = new QGroupBox(translateDialog);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        vboxLayout1 = new QVBoxLayout(groupBox);
        vboxLayout1->setSpacing(6);
        vboxLayout1->setMargin(11);
        vboxLayout1->setObjectName(QString::fromUtf8("vboxLayout1"));
        ckMatchCase = new QCheckBox(groupBox);
        ckMatchCase->setObjectName(QString::fromUtf8("ckMatchCase"));

        vboxLayout1->addWidget(ckMatchCase);

        ckMarkFinished = new QCheckBox(groupBox);
        ckMarkFinished->setObjectName(QString::fromUtf8("ckMarkFinished"));

        vboxLayout1->addWidget(ckMarkFinished);

        spacerItem = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        vboxLayout1->addItem(spacerItem);


        vboxLayout->addWidget(groupBox);


        hboxLayout->addLayout(vboxLayout);

        vboxLayout2 = new QVBoxLayout();
        vboxLayout2->setSpacing(6);
        vboxLayout2->setMargin(0);
        vboxLayout2->setObjectName(QString::fromUtf8("vboxLayout2"));
        findNxt = new QPushButton(translateDialog);
        findNxt->setObjectName(QString::fromUtf8("findNxt"));
        findNxt->setDefault(true);
        findNxt->setFlat(false);

        vboxLayout2->addWidget(findNxt);

        translate = new QPushButton(translateDialog);
        translate->setObjectName(QString::fromUtf8("translate"));

        vboxLayout2->addWidget(translate);

        translateAll = new QPushButton(translateDialog);
        translateAll->setObjectName(QString::fromUtf8("translateAll"));

        vboxLayout2->addWidget(translateAll);

        cancel = new QPushButton(translateDialog);
        cancel->setObjectName(QString::fromUtf8("cancel"));

        vboxLayout2->addWidget(cancel);

        spacerItem1 = new QSpacerItem(20, 51, QSizePolicy::Minimum, QSizePolicy::Expanding);

        vboxLayout2->addItem(spacerItem1);


        hboxLayout->addLayout(vboxLayout2);

#ifndef QT_NO_SHORTCUT
        findWhat->setBuddy(ledFindWhat);
        translateTo->setBuddy(ledTranslateTo);
#endif // QT_NO_SHORTCUT
        QWidget::setTabOrder(ledFindWhat, ledTranslateTo);
        QWidget::setTabOrder(ledTranslateTo, findNxt);
        QWidget::setTabOrder(findNxt, translate);
        QWidget::setTabOrder(translate, translateAll);
        QWidget::setTabOrder(translateAll, cancel);
        QWidget::setTabOrder(cancel, ckMatchCase);
        QWidget::setTabOrder(ckMatchCase, ckMarkFinished);

        retranslateUi(translateDialog);
        QObject::connect(cancel, SIGNAL(clicked()), translateDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(translateDialog);
    } // setupUi

    void retranslateUi(QDialog *translateDialog)
    {
#ifndef QT_NO_WHATSTHIS
        translateDialog->setWhatsThis(QApplication::translate("TranslateDialog", "This window allows you to search for some text in the translation source file.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
#ifndef QT_NO_WHATSTHIS
        ledTranslateTo->setWhatsThis(QApplication::translate("TranslateDialog", "Type in the text to search for.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        findWhat->setText(QApplication::translate("TranslateDialog", "Find &source text:", 0, QApplication::UnicodeUTF8));
        translateTo->setText(QApplication::translate("TranslateDialog", "&Translate to:", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_WHATSTHIS
        ledFindWhat->setWhatsThis(QApplication::translate("TranslateDialog", "Type in the text to search for.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        groupBox->setTitle(QApplication::translate("TranslateDialog", "Search options", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_WHATSTHIS
        ckMatchCase->setWhatsThis(QApplication::translate("TranslateDialog", "Texts such as 'TeX' and 'tex' are considered as different when checked.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        ckMatchCase->setText(QApplication::translate("TranslateDialog", "Match &case", 0, QApplication::UnicodeUTF8));
        ckMarkFinished->setText(QApplication::translate("TranslateDialog", "Mark new translation as &finished", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_WHATSTHIS
        findNxt->setWhatsThis(QApplication::translate("TranslateDialog", "Click here to find the next occurrence of the text you typed in.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        findNxt->setText(QApplication::translate("TranslateDialog", "Find Next", 0, QApplication::UnicodeUTF8));
        translate->setText(QApplication::translate("TranslateDialog", "Translate", 0, QApplication::UnicodeUTF8));
        translateAll->setText(QApplication::translate("TranslateDialog", "Translate All", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_WHATSTHIS
        cancel->setWhatsThis(QApplication::translate("TranslateDialog", "Click here to close this window.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        cancel->setText(QApplication::translate("TranslateDialog", "Cancel", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(translateDialog);
    } // retranslateUi

};

namespace Ui {
    class TranslateDialog: public Ui_TranslateDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TRANSLATEDIALOG_H
