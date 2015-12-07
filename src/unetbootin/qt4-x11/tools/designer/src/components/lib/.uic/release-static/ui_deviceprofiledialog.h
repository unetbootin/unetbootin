/********************************************************************************
** Form generated from reading ui file 'deviceprofiledialog.ui'
**
** Created: Thu Mar 5 20:47:29 2009
**      by: Qt User Interface Compiler version 4.5.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_DEVICEPROFILEDIALOG_H
#define UI_DEVICEPROFILEDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFontComboBox>
#include <QtGui/QFormLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include "dpi_chooser.h"

QT_BEGIN_NAMESPACE

class Ui_DeviceProfileDialog
{
public:
    QVBoxLayout *verticalLayout;
    QWidget *SystemSettingsWidget;
    QFormLayout *formLayout;
    QLabel *m_systemFontFamilyLabel;
    QFontComboBox *m_systemFontComboBox;
    QLabel *m_systemFontSizeLabel;
    QComboBox *m_systemFontSizeCombo;
    QLabel *m_styleLabel;
    QComboBox *m_styleCombo;
    QLabel *m_systemDPILabel;
    qdesigner_internal::DPI_Chooser *m_dpiChooser;
    QLabel *m_nameLabel;
    QLineEdit *m_nameLineEdit;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *dialog)
    {
        if (dialog->objectName().isEmpty())
            dialog->setObjectName(QString::fromUtf8("dialog"));
        dialog->resize(348, 209);
        verticalLayout = new QVBoxLayout(dialog);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        SystemSettingsWidget = new QWidget(dialog);
        SystemSettingsWidget->setObjectName(QString::fromUtf8("SystemSettingsWidget"));
        formLayout = new QFormLayout(SystemSettingsWidget);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        m_systemFontFamilyLabel = new QLabel(SystemSettingsWidget);
        m_systemFontFamilyLabel->setObjectName(QString::fromUtf8("m_systemFontFamilyLabel"));

        formLayout->setWidget(1, QFormLayout::LabelRole, m_systemFontFamilyLabel);

        m_systemFontComboBox = new QFontComboBox(SystemSettingsWidget);
        m_systemFontComboBox->setObjectName(QString::fromUtf8("m_systemFontComboBox"));

        formLayout->setWidget(1, QFormLayout::FieldRole, m_systemFontComboBox);

        m_systemFontSizeLabel = new QLabel(SystemSettingsWidget);
        m_systemFontSizeLabel->setObjectName(QString::fromUtf8("m_systemFontSizeLabel"));

        formLayout->setWidget(2, QFormLayout::LabelRole, m_systemFontSizeLabel);

        m_systemFontSizeCombo = new QComboBox(SystemSettingsWidget);
        m_systemFontSizeCombo->setObjectName(QString::fromUtf8("m_systemFontSizeCombo"));

        formLayout->setWidget(2, QFormLayout::FieldRole, m_systemFontSizeCombo);

        m_styleLabel = new QLabel(SystemSettingsWidget);
        m_styleLabel->setObjectName(QString::fromUtf8("m_styleLabel"));

        formLayout->setWidget(3, QFormLayout::LabelRole, m_styleLabel);

        m_styleCombo = new QComboBox(SystemSettingsWidget);
        m_styleCombo->setObjectName(QString::fromUtf8("m_styleCombo"));

        formLayout->setWidget(3, QFormLayout::FieldRole, m_styleCombo);

        m_systemDPILabel = new QLabel(SystemSettingsWidget);
        m_systemDPILabel->setObjectName(QString::fromUtf8("m_systemDPILabel"));

        formLayout->setWidget(4, QFormLayout::LabelRole, m_systemDPILabel);

        m_dpiChooser = new qdesigner_internal::DPI_Chooser(SystemSettingsWidget);
        m_dpiChooser->setObjectName(QString::fromUtf8("m_dpiChooser"));

        formLayout->setWidget(4, QFormLayout::FieldRole, m_dpiChooser);

        m_nameLabel = new QLabel(SystemSettingsWidget);
        m_nameLabel->setObjectName(QString::fromUtf8("m_nameLabel"));

        formLayout->setWidget(0, QFormLayout::LabelRole, m_nameLabel);

        m_nameLineEdit = new QLineEdit(SystemSettingsWidget);
        m_nameLineEdit->setObjectName(QString::fromUtf8("m_nameLineEdit"));

        formLayout->setWidget(0, QFormLayout::FieldRole, m_nameLineEdit);


        verticalLayout->addWidget(SystemSettingsWidget);

        buttonBox = new QDialogButtonBox(dialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok|QDialogButtonBox::Open|QDialogButtonBox::Save);

        verticalLayout->addWidget(buttonBox);

#ifndef QT_NO_SHORTCUT
        m_systemFontFamilyLabel->setBuddy(m_systemFontComboBox);
        m_systemFontSizeLabel->setBuddy(m_systemFontSizeCombo);
        m_styleLabel->setBuddy(m_styleCombo);
#endif // QT_NO_SHORTCUT

        retranslateUi(dialog);

        QMetaObject::connectSlotsByName(dialog);
    } // setupUi

    void retranslateUi(QDialog *dialog)
    {
        m_systemFontFamilyLabel->setText(QApplication::translate("DeviceProfileDialog", "&Family", 0, QApplication::UnicodeUTF8));
        m_systemFontSizeLabel->setText(QApplication::translate("DeviceProfileDialog", "&Point Size", 0, QApplication::UnicodeUTF8));
        m_styleLabel->setText(QApplication::translate("DeviceProfileDialog", "Style", 0, QApplication::UnicodeUTF8));
        m_systemDPILabel->setText(QApplication::translate("DeviceProfileDialog", "Device DPI", 0, QApplication::UnicodeUTF8));
        m_nameLabel->setText(QApplication::translate("DeviceProfileDialog", "Name", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(dialog);
    } // retranslateUi

};

namespace Ui {
    class DeviceProfileDialog: public Ui_DeviceProfileDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DEVICEPROFILEDIALOG_H
