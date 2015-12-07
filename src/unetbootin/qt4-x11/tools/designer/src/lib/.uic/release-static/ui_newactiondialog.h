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
** Form generated from reading ui file 'newactiondialog.ui'
**
** Created: Thu Mar 5 20:40:10 2009
**      by: Qt User Interface Compiler version 4.5.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_NEWACTIONDIALOG_H
#define UI_NEWACTIONDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFormLayout>
#include <QtGui/QFrame>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QSpacerItem>
#include <QtGui/QToolButton>
#include <QtGui/QVBoxLayout>
#include "iconselector_p.h"
#include "qtpropertybrowserutils_p.h"
#include "textpropertyeditor_p.h"

QT_BEGIN_NAMESPACE

namespace qdesigner_internal {

class Ui_NewActionDialog
{
public:
    QVBoxLayout *verticalLayout;
    QFormLayout *formLayout;
    QLabel *textLabel;
    QLineEdit *editActionText;
    QLabel *objectNameLabel;
    QLineEdit *editObjectName;
    QLabel *iconLabel;
    QHBoxLayout *horizontalLayout;
    qdesigner_internal::IconSelector *iconSelector;
    QSpacerItem *spacerItem;
    QLabel *shortcutLabel;
    QCheckBox *checkableCheckBox;
    QLabel *checkableLabel;
    QLabel *toolTipLabel;
    QHBoxLayout *toolTipLayout;
    TextPropertyEditor *tooltipEditor;
    QToolButton *toolTipToolButton;
    QHBoxLayout *keysequenceLayout;
    QtKeySequenceEdit *keySequenceEdit;
    QToolButton *keysequenceResetToolButton;
    QSpacerItem *verticalSpacer;
    QFrame *line;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *qdesigner_internal__NewActionDialog)
    {
        if (qdesigner_internal__NewActionDialog->objectName().isEmpty())
            qdesigner_internal__NewActionDialog->setObjectName(QString::fromUtf8("qdesigner_internal__NewActionDialog"));
        verticalLayout = new QVBoxLayout(qdesigner_internal__NewActionDialog);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        formLayout = new QFormLayout();
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        textLabel = new QLabel(qdesigner_internal__NewActionDialog);
        textLabel->setObjectName(QString::fromUtf8("textLabel"));

        formLayout->setWidget(0, QFormLayout::LabelRole, textLabel);

        editActionText = new QLineEdit(qdesigner_internal__NewActionDialog);
        editActionText->setObjectName(QString::fromUtf8("editActionText"));
        editActionText->setMinimumSize(QSize(255, 0));

        formLayout->setWidget(0, QFormLayout::FieldRole, editActionText);

        objectNameLabel = new QLabel(qdesigner_internal__NewActionDialog);
        objectNameLabel->setObjectName(QString::fromUtf8("objectNameLabel"));

        formLayout->setWidget(1, QFormLayout::LabelRole, objectNameLabel);

        editObjectName = new QLineEdit(qdesigner_internal__NewActionDialog);
        editObjectName->setObjectName(QString::fromUtf8("editObjectName"));

        formLayout->setWidget(1, QFormLayout::FieldRole, editObjectName);

        iconLabel = new QLabel(qdesigner_internal__NewActionDialog);
        iconLabel->setObjectName(QString::fromUtf8("iconLabel"));

        formLayout->setWidget(3, QFormLayout::LabelRole, iconLabel);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        iconSelector = new qdesigner_internal::IconSelector(qdesigner_internal__NewActionDialog);
        iconSelector->setObjectName(QString::fromUtf8("iconSelector"));

        horizontalLayout->addWidget(iconSelector);

        spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(spacerItem);


        formLayout->setLayout(3, QFormLayout::FieldRole, horizontalLayout);

        shortcutLabel = new QLabel(qdesigner_internal__NewActionDialog);
        shortcutLabel->setObjectName(QString::fromUtf8("shortcutLabel"));

        formLayout->setWidget(5, QFormLayout::LabelRole, shortcutLabel);

        checkableCheckBox = new QCheckBox(qdesigner_internal__NewActionDialog);
        checkableCheckBox->setObjectName(QString::fromUtf8("checkableCheckBox"));

        formLayout->setWidget(4, QFormLayout::FieldRole, checkableCheckBox);

        checkableLabel = new QLabel(qdesigner_internal__NewActionDialog);
        checkableLabel->setObjectName(QString::fromUtf8("checkableLabel"));

        formLayout->setWidget(4, QFormLayout::LabelRole, checkableLabel);

        toolTipLabel = new QLabel(qdesigner_internal__NewActionDialog);
        toolTipLabel->setObjectName(QString::fromUtf8("toolTipLabel"));

        formLayout->setWidget(2, QFormLayout::LabelRole, toolTipLabel);

        toolTipLayout = new QHBoxLayout();
        toolTipLayout->setObjectName(QString::fromUtf8("toolTipLayout"));
        tooltipEditor = new TextPropertyEditor(qdesigner_internal__NewActionDialog);
        tooltipEditor->setObjectName(QString::fromUtf8("tooltipEditor"));
        QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(tooltipEditor->sizePolicy().hasHeightForWidth());
        tooltipEditor->setSizePolicy(sizePolicy);

        toolTipLayout->addWidget(tooltipEditor);

        toolTipToolButton = new QToolButton(qdesigner_internal__NewActionDialog);
        toolTipToolButton->setObjectName(QString::fromUtf8("toolTipToolButton"));

        toolTipLayout->addWidget(toolTipToolButton);


        formLayout->setLayout(2, QFormLayout::FieldRole, toolTipLayout);

        keysequenceLayout = new QHBoxLayout();
        keysequenceLayout->setObjectName(QString::fromUtf8("keysequenceLayout"));
        keySequenceEdit = new QtKeySequenceEdit(qdesigner_internal__NewActionDialog);
        keySequenceEdit->setObjectName(QString::fromUtf8("keySequenceEdit"));
        sizePolicy.setHeightForWidth(keySequenceEdit->sizePolicy().hasHeightForWidth());
        keySequenceEdit->setSizePolicy(sizePolicy);

        keysequenceLayout->addWidget(keySequenceEdit);

        keysequenceResetToolButton = new QToolButton(qdesigner_internal__NewActionDialog);
        keysequenceResetToolButton->setObjectName(QString::fromUtf8("keysequenceResetToolButton"));

        keysequenceLayout->addWidget(keysequenceResetToolButton);


        formLayout->setLayout(5, QFormLayout::FieldRole, keysequenceLayout);


        verticalLayout->addLayout(formLayout);

        verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        line = new QFrame(qdesigner_internal__NewActionDialog);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(line);

        buttonBox = new QDialogButtonBox(qdesigner_internal__NewActionDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);

#ifndef QT_NO_SHORTCUT
        textLabel->setBuddy(editActionText);
        objectNameLabel->setBuddy(editObjectName);
        iconLabel->setBuddy(iconSelector);
#endif // QT_NO_SHORTCUT
        QWidget::setTabOrder(editActionText, editObjectName);

        retranslateUi(qdesigner_internal__NewActionDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), qdesigner_internal__NewActionDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), qdesigner_internal__NewActionDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(qdesigner_internal__NewActionDialog);
    } // setupUi

    void retranslateUi(QDialog *qdesigner_internal__NewActionDialog)
    {
        qdesigner_internal__NewActionDialog->setWindowTitle(QApplication::translate("qdesigner_internal::NewActionDialog", "New Action...", 0, QApplication::UnicodeUTF8));
        textLabel->setText(QApplication::translate("qdesigner_internal::NewActionDialog", "&Text:", 0, QApplication::UnicodeUTF8));
        objectNameLabel->setText(QApplication::translate("qdesigner_internal::NewActionDialog", "Object &name:", 0, QApplication::UnicodeUTF8));
        iconLabel->setText(QApplication::translate("qdesigner_internal::NewActionDialog", "&Icon:", 0, QApplication::UnicodeUTF8));
        shortcutLabel->setText(QApplication::translate("qdesigner_internal::NewActionDialog", "Shortcut:", 0, QApplication::UnicodeUTF8));
        checkableCheckBox->setText(QString());
        checkableLabel->setText(QApplication::translate("qdesigner_internal::NewActionDialog", "Checkable:", 0, QApplication::UnicodeUTF8));
        toolTipLabel->setText(QApplication::translate("qdesigner_internal::NewActionDialog", "ToolTip:", 0, QApplication::UnicodeUTF8));
        toolTipToolButton->setText(QApplication::translate("qdesigner_internal::NewActionDialog", "...", 0, QApplication::UnicodeUTF8));
        keysequenceResetToolButton->setText(QApplication::translate("qdesigner_internal::NewActionDialog", "...", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(qdesigner_internal__NewActionDialog);
    } // retranslateUi

};

} // namespace qdesigner_internal

namespace qdesigner_internal {
namespace Ui {
    class NewActionDialog: public Ui_NewActionDialog {};
} // namespace Ui
} // namespace qdesigner_internal

QT_END_NAMESPACE

#endif // UI_NEWACTIONDIALOG_H
