/********************************************************************************
** Form generated from reading ui file 'formlayoutrowdialog.ui'
**
** Created: Thu Mar 5 20:40:12 2009
**      by: Qt User Interface Compiler version 4.5.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_FORMLAYOUTROWDIALOG_H
#define UI_FORMLAYOUTROWDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFormLayout>
#include <QtGui/QFrame>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QSpinBox>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_FormLayoutRowDialog
{
public:
    QVBoxLayout *verticalLayout;
    QFormLayout *formLayout;
    QLabel *labelTextLabel;
    QLineEdit *labelTextLineEdit;
    QLineEdit *labelNameLineEdit;
    QLabel *fieldClassLabel;
    QComboBox *fieldClassComboBox;
    QLabel *fieldNameLabel;
    QLabel *buddyLabel;
    QCheckBox *buddyCheckBox;
    QLabel *rowLabel;
    QSpinBox *rowSpinBox;
    QLineEdit *fieldNameLineEdit;
    QLabel *labelNameLabel;
    QFrame *line;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *FormLayoutRowDialog)
    {
        if (FormLayoutRowDialog->objectName().isEmpty())
            FormLayoutRowDialog->setObjectName(QString::fromUtf8("FormLayoutRowDialog"));
        verticalLayout = new QVBoxLayout(FormLayoutRowDialog);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        formLayout = new QFormLayout();
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        formLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
        labelTextLabel = new QLabel(FormLayoutRowDialog);
        labelTextLabel->setObjectName(QString::fromUtf8("labelTextLabel"));

        formLayout->setWidget(0, QFormLayout::LabelRole, labelTextLabel);

        labelTextLineEdit = new QLineEdit(FormLayoutRowDialog);
        labelTextLineEdit->setObjectName(QString::fromUtf8("labelTextLineEdit"));
        labelTextLineEdit->setMinimumSize(QSize(180, 0));

        formLayout->setWidget(0, QFormLayout::FieldRole, labelTextLineEdit);

        labelNameLineEdit = new QLineEdit(FormLayoutRowDialog);
        labelNameLineEdit->setObjectName(QString::fromUtf8("labelNameLineEdit"));

        formLayout->setWidget(1, QFormLayout::FieldRole, labelNameLineEdit);

        fieldClassLabel = new QLabel(FormLayoutRowDialog);
        fieldClassLabel->setObjectName(QString::fromUtf8("fieldClassLabel"));

        formLayout->setWidget(2, QFormLayout::LabelRole, fieldClassLabel);

        fieldClassComboBox = new QComboBox(FormLayoutRowDialog);
        fieldClassComboBox->setObjectName(QString::fromUtf8("fieldClassComboBox"));
        QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(fieldClassComboBox->sizePolicy().hasHeightForWidth());
        fieldClassComboBox->setSizePolicy(sizePolicy);

        formLayout->setWidget(2, QFormLayout::FieldRole, fieldClassComboBox);

        fieldNameLabel = new QLabel(FormLayoutRowDialog);
        fieldNameLabel->setObjectName(QString::fromUtf8("fieldNameLabel"));

        formLayout->setWidget(3, QFormLayout::LabelRole, fieldNameLabel);

        buddyLabel = new QLabel(FormLayoutRowDialog);
        buddyLabel->setObjectName(QString::fromUtf8("buddyLabel"));

        formLayout->setWidget(4, QFormLayout::LabelRole, buddyLabel);

        buddyCheckBox = new QCheckBox(FormLayoutRowDialog);
        buddyCheckBox->setObjectName(QString::fromUtf8("buddyCheckBox"));

        formLayout->setWidget(4, QFormLayout::FieldRole, buddyCheckBox);

        rowLabel = new QLabel(FormLayoutRowDialog);
        rowLabel->setObjectName(QString::fromUtf8("rowLabel"));

        formLayout->setWidget(5, QFormLayout::LabelRole, rowLabel);

        rowSpinBox = new QSpinBox(FormLayoutRowDialog);
        rowSpinBox->setObjectName(QString::fromUtf8("rowSpinBox"));

        formLayout->setWidget(5, QFormLayout::FieldRole, rowSpinBox);

        fieldNameLineEdit = new QLineEdit(FormLayoutRowDialog);
        fieldNameLineEdit->setObjectName(QString::fromUtf8("fieldNameLineEdit"));

        formLayout->setWidget(3, QFormLayout::FieldRole, fieldNameLineEdit);

        labelNameLabel = new QLabel(FormLayoutRowDialog);
        labelNameLabel->setObjectName(QString::fromUtf8("labelNameLabel"));

        formLayout->setWidget(1, QFormLayout::LabelRole, labelNameLabel);


        verticalLayout->addLayout(formLayout);

        line = new QFrame(FormLayoutRowDialog);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(line);

        buttonBox = new QDialogButtonBox(FormLayoutRowDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);

#ifndef QT_NO_SHORTCUT
        labelTextLabel->setBuddy(labelTextLineEdit);
        fieldClassLabel->setBuddy(fieldClassComboBox);
        fieldNameLabel->setBuddy(fieldNameLineEdit);
        buddyLabel->setBuddy(buddyCheckBox);
        rowLabel->setBuddy(rowSpinBox);
        labelNameLabel->setBuddy(labelNameLineEdit);
#endif // QT_NO_SHORTCUT

        retranslateUi(FormLayoutRowDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), FormLayoutRowDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), FormLayoutRowDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(FormLayoutRowDialog);
    } // setupUi

    void retranslateUi(QDialog *FormLayoutRowDialog)
    {
        FormLayoutRowDialog->setWindowTitle(QApplication::translate("FormLayoutRowDialog", "Add Form Layout Row", 0, QApplication::UnicodeUTF8));
        labelTextLabel->setText(QApplication::translate("FormLayoutRowDialog", "&Label text:", 0, QApplication::UnicodeUTF8));
        fieldClassLabel->setText(QApplication::translate("FormLayoutRowDialog", "Field &type:", 0, QApplication::UnicodeUTF8));
        fieldNameLabel->setText(QApplication::translate("FormLayoutRowDialog", "&Field name:", 0, QApplication::UnicodeUTF8));
        buddyLabel->setText(QApplication::translate("FormLayoutRowDialog", "&Buddy:", 0, QApplication::UnicodeUTF8));
        buddyCheckBox->setText(QString());
        rowLabel->setText(QApplication::translate("FormLayoutRowDialog", "&Row:", 0, QApplication::UnicodeUTF8));
        labelNameLabel->setText(QApplication::translate("FormLayoutRowDialog", "Label &name:", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(FormLayoutRowDialog);
    } // retranslateUi

};

namespace Ui {
    class FormLayoutRowDialog: public Ui_FormLayoutRowDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FORMLAYOUTROWDIALOG_H
