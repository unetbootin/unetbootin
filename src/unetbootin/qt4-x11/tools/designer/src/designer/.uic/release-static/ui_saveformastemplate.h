/*
*********************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt Designer of the Qt Toolkit.
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
** Form generated from reading ui file 'saveformastemplate.ui'
**
** Created: Thu Mar 5 20:52:48 2009
**      by: Qt User Interface Compiler version 4.5.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_SAVEFORMASTEMPLATE_H
#define UI_SAVEFORMASTEMPLATE_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFormLayout>
#include <QtGui/QFrame>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_SaveFormAsTemplate
{
public:
    QVBoxLayout *vboxLayout;
    QFormLayout *formLayout;
    QLabel *label;
    QLineEdit *templateNameEdit;
    QLabel *label_2;
    QComboBox *categoryCombo;
    QFrame *horizontalLine;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *SaveFormAsTemplate)
    {
        if (SaveFormAsTemplate->objectName().isEmpty())
            SaveFormAsTemplate->setObjectName(QString::fromUtf8("SaveFormAsTemplate"));
        vboxLayout = new QVBoxLayout(SaveFormAsTemplate);
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        formLayout = new QFormLayout();
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        label = new QLabel(SaveFormAsTemplate);
        label->setObjectName(QString::fromUtf8("label"));
        label->setFrameShape(QFrame::NoFrame);
        label->setFrameShadow(QFrame::Plain);
        label->setTextFormat(Qt::AutoText);

        formLayout->setWidget(0, QFormLayout::LabelRole, label);

        templateNameEdit = new QLineEdit(SaveFormAsTemplate);
        templateNameEdit->setObjectName(QString::fromUtf8("templateNameEdit"));
        templateNameEdit->setMinimumSize(QSize(222, 0));
        templateNameEdit->setEchoMode(QLineEdit::Normal);

        formLayout->setWidget(0, QFormLayout::FieldRole, templateNameEdit);

        label_2 = new QLabel(SaveFormAsTemplate);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setFrameShape(QFrame::NoFrame);
        label_2->setFrameShadow(QFrame::Plain);
        label_2->setTextFormat(Qt::AutoText);

        formLayout->setWidget(1, QFormLayout::LabelRole, label_2);

        categoryCombo = new QComboBox(SaveFormAsTemplate);
        categoryCombo->setObjectName(QString::fromUtf8("categoryCombo"));

        formLayout->setWidget(1, QFormLayout::FieldRole, categoryCombo);


        vboxLayout->addLayout(formLayout);

        horizontalLine = new QFrame(SaveFormAsTemplate);
        horizontalLine->setObjectName(QString::fromUtf8("horizontalLine"));
        horizontalLine->setFrameShape(QFrame::HLine);
        horizontalLine->setFrameShadow(QFrame::Sunken);

        vboxLayout->addWidget(horizontalLine);

        buttonBox = new QDialogButtonBox(SaveFormAsTemplate);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        vboxLayout->addWidget(buttonBox);

#ifndef QT_NO_SHORTCUT
        label->setBuddy(templateNameEdit);
        label_2->setBuddy(categoryCombo);
#endif // QT_NO_SHORTCUT

        retranslateUi(SaveFormAsTemplate);
        QObject::connect(buttonBox, SIGNAL(accepted()), SaveFormAsTemplate, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), SaveFormAsTemplate, SLOT(reject()));

        QMetaObject::connectSlotsByName(SaveFormAsTemplate);
    } // setupUi

    void retranslateUi(QDialog *SaveFormAsTemplate)
    {
        SaveFormAsTemplate->setWindowTitle(QApplication::translate("SaveFormAsTemplate", "Save Form As Template", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("SaveFormAsTemplate", "&Name:", 0, QApplication::UnicodeUTF8));
        templateNameEdit->setText(QString());
        label_2->setText(QApplication::translate("SaveFormAsTemplate", "&Category:", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(SaveFormAsTemplate);
    } // retranslateUi

};

namespace Ui {
    class SaveFormAsTemplate: public Ui_SaveFormAsTemplate {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SAVEFORMASTEMPLATE_H
