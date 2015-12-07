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
** Form generated from reading ui file 'formwindowsettings.ui'
**
** Created: Thu Mar 5 20:47:29 2009
**      by: Qt User Interface Compiler version 4.5.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_FORMWINDOWSETTINGS_H
#define UI_FORMWINDOWSETTINGS_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QTextEdit>
#include <QtGui/QVBoxLayout>
#include <gridpanel_p.h>

QT_BEGIN_NAMESPACE

class Ui_FormWindowSettings
{
public:
    QGridLayout *gridLayout;
    QHBoxLayout *hboxLayout;
    QGroupBox *layoutDefaultGroupBox;
    QGridLayout *gridLayout1;
    QLabel *label_2;
    QLabel *label;
    QSpinBox *defaultSpacingSpinBox;
    QSpinBox *defaultMarginSpinBox;
    QGroupBox *layoutFunctionGroupBox;
    QGridLayout *gridLayout2;
    QLineEdit *spacingFunctionLineEdit;
    QLineEdit *marginFunctionLineEdit;
    QLabel *label_3;
    QLabel *label_3_2;
    QHBoxLayout *hboxLayout1;
    QGroupBox *pixmapFunctionGroupBox;
    QVBoxLayout *vboxLayout;
    QLineEdit *pixmapFunctionLineEdit;
    QSpacerItem *spacerItem;
    QDialogButtonBox *buttonBox;
    QFrame *line;
    QGroupBox *includeHintsGroupBox;
    QVBoxLayout *vboxLayout1;
    QTextEdit *includeHintsTextEdit;
    qdesigner_internal::GridPanel *gridPanel;
    QGroupBox *embeddedGroupBox;
    QVBoxLayout *verticalLayout;
    QLabel *deviceProfileLabel;
    QGroupBox *pixmapFunctionGroupBox_2;
    QVBoxLayout *vboxLayout2;
    QLineEdit *authorLineEdit;

    void setupUi(QDialog *FormWindowSettings)
    {
        if (FormWindowSettings->objectName().isEmpty())
            FormWindowSettings->setObjectName(QString::fromUtf8("FormWindowSettings"));
        FormWindowSettings->resize(470, 466);
        gridLayout = new QGridLayout(FormWindowSettings);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        hboxLayout = new QHBoxLayout();
#ifndef Q_OS_MAC
        hboxLayout->setSpacing(6);
#endif
#ifndef Q_OS_MAC
        hboxLayout->setMargin(0);
#endif
        hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
        layoutDefaultGroupBox = new QGroupBox(FormWindowSettings);
        layoutDefaultGroupBox->setObjectName(QString::fromUtf8("layoutDefaultGroupBox"));
        layoutDefaultGroupBox->setCheckable(true);
        gridLayout1 = new QGridLayout(layoutDefaultGroupBox);
#ifndef Q_OS_MAC
        gridLayout1->setSpacing(6);
#endif
        gridLayout1->setMargin(8);
        gridLayout1->setObjectName(QString::fromUtf8("gridLayout1"));
        label_2 = new QLabel(layoutDefaultGroupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout1->addWidget(label_2, 1, 0, 1, 1);

        label = new QLabel(layoutDefaultGroupBox);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout1->addWidget(label, 0, 0, 1, 1);

        defaultSpacingSpinBox = new QSpinBox(layoutDefaultGroupBox);
        defaultSpacingSpinBox->setObjectName(QString::fromUtf8("defaultSpacingSpinBox"));

        gridLayout1->addWidget(defaultSpacingSpinBox, 1, 1, 1, 1);

        defaultMarginSpinBox = new QSpinBox(layoutDefaultGroupBox);
        defaultMarginSpinBox->setObjectName(QString::fromUtf8("defaultMarginSpinBox"));

        gridLayout1->addWidget(defaultMarginSpinBox, 0, 1, 1, 1);


        hboxLayout->addWidget(layoutDefaultGroupBox);

        layoutFunctionGroupBox = new QGroupBox(FormWindowSettings);
        layoutFunctionGroupBox->setObjectName(QString::fromUtf8("layoutFunctionGroupBox"));
        layoutFunctionGroupBox->setCheckable(true);
        gridLayout2 = new QGridLayout(layoutFunctionGroupBox);
#ifndef Q_OS_MAC
        gridLayout2->setSpacing(6);
#endif
        gridLayout2->setMargin(8);
        gridLayout2->setObjectName(QString::fromUtf8("gridLayout2"));
        spacingFunctionLineEdit = new QLineEdit(layoutFunctionGroupBox);
        spacingFunctionLineEdit->setObjectName(QString::fromUtf8("spacingFunctionLineEdit"));

        gridLayout2->addWidget(spacingFunctionLineEdit, 1, 1, 1, 1);

        marginFunctionLineEdit = new QLineEdit(layoutFunctionGroupBox);
        marginFunctionLineEdit->setObjectName(QString::fromUtf8("marginFunctionLineEdit"));

        gridLayout2->addWidget(marginFunctionLineEdit, 0, 1, 1, 1);

        label_3 = new QLabel(layoutFunctionGroupBox);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout2->addWidget(label_3, 0, 0, 1, 1);

        label_3_2 = new QLabel(layoutFunctionGroupBox);
        label_3_2->setObjectName(QString::fromUtf8("label_3_2"));

        gridLayout2->addWidget(label_3_2, 1, 0, 1, 1);


        hboxLayout->addWidget(layoutFunctionGroupBox);


        gridLayout->addLayout(hboxLayout, 3, 0, 1, 2);

        hboxLayout1 = new QHBoxLayout();
#ifndef Q_OS_MAC
        hboxLayout1->setSpacing(6);
#endif
        hboxLayout1->setMargin(0);
        hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));
        pixmapFunctionGroupBox = new QGroupBox(FormWindowSettings);
        pixmapFunctionGroupBox->setObjectName(QString::fromUtf8("pixmapFunctionGroupBox"));
        pixmapFunctionGroupBox->setCheckable(true);
        vboxLayout = new QVBoxLayout(pixmapFunctionGroupBox);
#ifndef Q_OS_MAC
        vboxLayout->setSpacing(6);
#endif
        vboxLayout->setMargin(8);
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        pixmapFunctionLineEdit = new QLineEdit(pixmapFunctionGroupBox);
        pixmapFunctionLineEdit->setObjectName(QString::fromUtf8("pixmapFunctionLineEdit"));

        vboxLayout->addWidget(pixmapFunctionLineEdit);


        hboxLayout1->addWidget(pixmapFunctionGroupBox);


        gridLayout->addLayout(hboxLayout1, 4, 1, 1, 1);

        spacerItem = new QSpacerItem(111, 115, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(spacerItem, 5, 1, 1, 1);

        buttonBox = new QDialogButtonBox(FormWindowSettings);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        gridLayout->addWidget(buttonBox, 7, 0, 1, 2);

        line = new QFrame(FormWindowSettings);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        gridLayout->addWidget(line, 6, 0, 1, 2);

        includeHintsGroupBox = new QGroupBox(FormWindowSettings);
        includeHintsGroupBox->setObjectName(QString::fromUtf8("includeHintsGroupBox"));
        vboxLayout1 = new QVBoxLayout(includeHintsGroupBox);
#ifndef Q_OS_MAC
        vboxLayout1->setSpacing(6);
#endif
        vboxLayout1->setMargin(8);
        vboxLayout1->setObjectName(QString::fromUtf8("vboxLayout1"));
        includeHintsTextEdit = new QTextEdit(includeHintsGroupBox);
        includeHintsTextEdit->setObjectName(QString::fromUtf8("includeHintsTextEdit"));

        vboxLayout1->addWidget(includeHintsTextEdit);


        gridLayout->addWidget(includeHintsGroupBox, 4, 0, 2, 1);

        gridPanel = new qdesigner_internal::GridPanel(FormWindowSettings);
        gridPanel->setObjectName(QString::fromUtf8("gridPanel"));

        gridLayout->addWidget(gridPanel, 2, 0, 1, 2);

        embeddedGroupBox = new QGroupBox(FormWindowSettings);
        embeddedGroupBox->setObjectName(QString::fromUtf8("embeddedGroupBox"));
        verticalLayout = new QVBoxLayout(embeddedGroupBox);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        deviceProfileLabel = new QLabel(embeddedGroupBox);
        deviceProfileLabel->setObjectName(QString::fromUtf8("deviceProfileLabel"));
        deviceProfileLabel->setText(QString::fromUtf8("TextLabel"));

        verticalLayout->addWidget(deviceProfileLabel);


        gridLayout->addWidget(embeddedGroupBox, 0, 0, 1, 2);

        pixmapFunctionGroupBox_2 = new QGroupBox(FormWindowSettings);
        pixmapFunctionGroupBox_2->setObjectName(QString::fromUtf8("pixmapFunctionGroupBox_2"));
        vboxLayout2 = new QVBoxLayout(pixmapFunctionGroupBox_2);
#ifndef Q_OS_MAC
        vboxLayout2->setSpacing(6);
#endif
        vboxLayout2->setMargin(8);
        vboxLayout2->setObjectName(QString::fromUtf8("vboxLayout2"));
        authorLineEdit = new QLineEdit(pixmapFunctionGroupBox_2);
        authorLineEdit->setObjectName(QString::fromUtf8("authorLineEdit"));

        vboxLayout2->addWidget(authorLineEdit);


        gridLayout->addWidget(pixmapFunctionGroupBox_2, 1, 0, 1, 2);

#ifndef QT_NO_SHORTCUT
        label_2->setBuddy(defaultSpacingSpinBox);
        label->setBuddy(defaultMarginSpinBox);
        label_3->setBuddy(marginFunctionLineEdit);
        label_3_2->setBuddy(spacingFunctionLineEdit);
#endif // QT_NO_SHORTCUT
        QWidget::setTabOrder(authorLineEdit, defaultMarginSpinBox);
        QWidget::setTabOrder(defaultMarginSpinBox, defaultSpacingSpinBox);
        QWidget::setTabOrder(defaultSpacingSpinBox, marginFunctionLineEdit);
        QWidget::setTabOrder(marginFunctionLineEdit, spacingFunctionLineEdit);
        QWidget::setTabOrder(spacingFunctionLineEdit, pixmapFunctionLineEdit);

        retranslateUi(FormWindowSettings);
        QObject::connect(buttonBox, SIGNAL(accepted()), FormWindowSettings, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), FormWindowSettings, SLOT(reject()));

        QMetaObject::connectSlotsByName(FormWindowSettings);
    } // setupUi

    void retranslateUi(QDialog *FormWindowSettings)
    {
        FormWindowSettings->setWindowTitle(QApplication::translate("FormWindowSettings", "Form Settings", 0, QApplication::UnicodeUTF8));
        layoutDefaultGroupBox->setTitle(QApplication::translate("FormWindowSettings", "Layout &Default", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("FormWindowSettings", "&Spacing:", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("FormWindowSettings", "&Margin:", 0, QApplication::UnicodeUTF8));
        layoutFunctionGroupBox->setTitle(QApplication::translate("FormWindowSettings", "&Layout Function", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("FormWindowSettings", "Ma&rgin:", 0, QApplication::UnicodeUTF8));
        label_3_2->setText(QApplication::translate("FormWindowSettings", "Spa&cing:", 0, QApplication::UnicodeUTF8));
        pixmapFunctionGroupBox->setTitle(QApplication::translate("FormWindowSettings", "&Pixmap Function", 0, QApplication::UnicodeUTF8));
        includeHintsGroupBox->setTitle(QApplication::translate("FormWindowSettings", "&Include Hints", 0, QApplication::UnicodeUTF8));
        gridPanel->setTitle(QApplication::translate("FormWindowSettings", "Grid", 0, QApplication::UnicodeUTF8));
        embeddedGroupBox->setTitle(QApplication::translate("FormWindowSettings", "Embedded Design", 0, QApplication::UnicodeUTF8));
        pixmapFunctionGroupBox_2->setTitle(QApplication::translate("FormWindowSettings", "&Author", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(FormWindowSettings);
    } // retranslateUi

};

namespace Ui {
    class FormWindowSettings: public Ui_FormWindowSettings {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FORMWINDOWSETTINGS_H
