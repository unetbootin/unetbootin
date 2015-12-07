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
** Form generated from reading ui file 'batchtranslation.ui'
**
** Created: Thu Mar 5 20:53:50 2009
**      by: Qt User Interface Compiler version 4.5.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_BATCHTRANSLATION_H
#define UI_BATCHTRANSLATION_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDialog>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QListView>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_BatchTranslationDialog
{
public:
    QVBoxLayout *vboxLayout;
    QGroupBox *groupBox;
    QVBoxLayout *vboxLayout1;
    QCheckBox *ckMarkFinished;
    QCheckBox *ckTranslateTranslated;
    QCheckBox *ckTranslateFinished;
    QGroupBox *groupBox_2;
    QVBoxLayout *vboxLayout2;
    QHBoxLayout *hboxLayout;
    QListView *phrasebookList;
    QVBoxLayout *vboxLayout3;
    QPushButton *moveUpButton;
    QPushButton *moveDownButton;
    QSpacerItem *spacerItem;
    QLabel *label;
    QHBoxLayout *hboxLayout1;
    QSpacerItem *spacerItem1;
    QPushButton *runButton;
    QPushButton *cancelButton;

    void setupUi(QDialog *batchTranslationDialog)
    {
        if (batchTranslationDialog->objectName().isEmpty())
            batchTranslationDialog->setObjectName(QString::fromUtf8("batchTranslationDialog"));
        batchTranslationDialog->resize(437, 492);
        vboxLayout = new QVBoxLayout(batchTranslationDialog);
#ifndef Q_OS_MAC
        vboxLayout->setSpacing(6);
#endif
#ifndef Q_OS_MAC
        vboxLayout->setMargin(9);
#endif
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        groupBox = new QGroupBox(batchTranslationDialog);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(groupBox->sizePolicy().hasHeightForWidth());
        groupBox->setSizePolicy(sizePolicy);
        vboxLayout1 = new QVBoxLayout(groupBox);
#ifndef Q_OS_MAC
        vboxLayout1->setSpacing(6);
#endif
#ifndef Q_OS_MAC
        vboxLayout1->setMargin(9);
#endif
        vboxLayout1->setObjectName(QString::fromUtf8("vboxLayout1"));
        ckMarkFinished = new QCheckBox(groupBox);
        ckMarkFinished->setObjectName(QString::fromUtf8("ckMarkFinished"));
        ckMarkFinished->setChecked(true);

        vboxLayout1->addWidget(ckMarkFinished);

        ckTranslateTranslated = new QCheckBox(groupBox);
        ckTranslateTranslated->setObjectName(QString::fromUtf8("ckTranslateTranslated"));
        ckTranslateTranslated->setChecked(false);

        vboxLayout1->addWidget(ckTranslateTranslated);

        ckTranslateFinished = new QCheckBox(groupBox);
        ckTranslateFinished->setObjectName(QString::fromUtf8("ckTranslateFinished"));

        vboxLayout1->addWidget(ckTranslateFinished);


        vboxLayout->addWidget(groupBox);

        groupBox_2 = new QGroupBox(batchTranslationDialog);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Minimum);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(groupBox_2->sizePolicy().hasHeightForWidth());
        groupBox_2->setSizePolicy(sizePolicy1);
        vboxLayout2 = new QVBoxLayout(groupBox_2);
#ifndef Q_OS_MAC
        vboxLayout2->setSpacing(6);
#endif
#ifndef Q_OS_MAC
        vboxLayout2->setMargin(9);
#endif
        vboxLayout2->setObjectName(QString::fromUtf8("vboxLayout2"));
        hboxLayout = new QHBoxLayout();
#ifndef Q_OS_MAC
        hboxLayout->setSpacing(6);
#endif
#ifndef Q_OS_MAC
        hboxLayout->setMargin(0);
#endif
        hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
        phrasebookList = new QListView(groupBox_2);
        phrasebookList->setObjectName(QString::fromUtf8("phrasebookList"));
        phrasebookList->setUniformItemSizes(true);

        hboxLayout->addWidget(phrasebookList);

        vboxLayout3 = new QVBoxLayout();
#ifndef Q_OS_MAC
        vboxLayout3->setSpacing(6);
#endif
        vboxLayout3->setMargin(0);
        vboxLayout3->setObjectName(QString::fromUtf8("vboxLayout3"));
        moveUpButton = new QPushButton(groupBox_2);
        moveUpButton->setObjectName(QString::fromUtf8("moveUpButton"));

        vboxLayout3->addWidget(moveUpButton);

        moveDownButton = new QPushButton(groupBox_2);
        moveDownButton->setObjectName(QString::fromUtf8("moveDownButton"));

        vboxLayout3->addWidget(moveDownButton);

        spacerItem = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        vboxLayout3->addItem(spacerItem);


        hboxLayout->addLayout(vboxLayout3);


        vboxLayout2->addLayout(hboxLayout);

        label = new QLabel(groupBox_2);
        label->setObjectName(QString::fromUtf8("label"));
        label->setWordWrap(true);

        vboxLayout2->addWidget(label);


        vboxLayout->addWidget(groupBox_2);

        hboxLayout1 = new QHBoxLayout();
#ifndef Q_OS_MAC
        hboxLayout1->setSpacing(6);
#endif
        hboxLayout1->setMargin(0);
        hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));
        spacerItem1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout1->addItem(spacerItem1);

        runButton = new QPushButton(batchTranslationDialog);
        runButton->setObjectName(QString::fromUtf8("runButton"));

        hboxLayout1->addWidget(runButton);

        cancelButton = new QPushButton(batchTranslationDialog);
        cancelButton->setObjectName(QString::fromUtf8("cancelButton"));

        hboxLayout1->addWidget(cancelButton);


        vboxLayout->addLayout(hboxLayout1);


        retranslateUi(batchTranslationDialog);
        QObject::connect(cancelButton, SIGNAL(clicked()), batchTranslationDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(batchTranslationDialog);
    } // setupUi

    void retranslateUi(QDialog *batchTranslationDialog)
    {
        batchTranslationDialog->setWindowTitle(QApplication::translate("BatchTranslationDialog", "Qt Linguist - Batch Translation", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("BatchTranslationDialog", "Options", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        ckMarkFinished->setToolTip(QString());
#endif // QT_NO_TOOLTIP
        ckMarkFinished->setText(QApplication::translate("BatchTranslationDialog", "Set translated entries to finished", 0, QApplication::UnicodeUTF8));
        ckTranslateTranslated->setText(QApplication::translate("BatchTranslationDialog", "Retranslate entries with existing translation", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        ckTranslateFinished->setToolTip(QApplication::translate("BatchTranslationDialog", "Note that the modified entries will be reset to unfinished if 'Set translated entries to finished' above is unchecked.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        ckTranslateFinished->setText(QApplication::translate("BatchTranslationDialog", "Translate also finished entries", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("BatchTranslationDialog", "Phrase book preference", 0, QApplication::UnicodeUTF8));
        moveUpButton->setText(QApplication::translate("BatchTranslationDialog", "Move up", 0, QApplication::UnicodeUTF8));
        moveDownButton->setText(QApplication::translate("BatchTranslationDialog", "Move down", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("BatchTranslationDialog", "The batch translator will search through the selected phrase books in the order given above.", 0, QApplication::UnicodeUTF8));
        runButton->setText(QApplication::translate("BatchTranslationDialog", "&Run", 0, QApplication::UnicodeUTF8));
        cancelButton->setText(QApplication::translate("BatchTranslationDialog", "Cancel", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(batchTranslationDialog);
    } // retranslateUi

};

namespace Ui {
    class BatchTranslationDialog: public Ui_BatchTranslationDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BATCHTRANSLATION_H
