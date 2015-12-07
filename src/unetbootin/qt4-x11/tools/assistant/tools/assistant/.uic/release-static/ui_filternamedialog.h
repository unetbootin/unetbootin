/********************************************************************************
** Form generated from reading ui file 'filternamedialog.ui'
**
** Created: Thu Mar 5 20:32:51 2009
**      by: Qt User Interface Compiler version 4.5.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_FILTERNAMEDIALOG_H
#define UI_FILTERNAMEDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QSpacerItem>

QT_BEGIN_NAMESPACE

class Ui_FilterNameDialogClass
{
public:
    QGridLayout *gridLayout;
    QLabel *label;
    QLineEdit *lineEdit;
    QFrame *line;
    QSpacerItem *spacerItem;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *FilterNameDialogClass)
    {
        if (FilterNameDialogClass->objectName().isEmpty())
            FilterNameDialogClass->setObjectName(QString::fromUtf8("FilterNameDialogClass"));
        FilterNameDialogClass->resize(312, 95);
        gridLayout = new QGridLayout(FilterNameDialogClass);
        gridLayout->setSpacing(6);
        gridLayout->setMargin(9);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label = new QLabel(FilterNameDialogClass);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);

        lineEdit = new QLineEdit(FilterNameDialogClass);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));

        gridLayout->addWidget(lineEdit, 0, 1, 1, 2);

        line = new QFrame(FilterNameDialogClass);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        gridLayout->addWidget(line, 1, 0, 1, 3);

        spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(spacerItem, 2, 0, 1, 2);

        buttonBox = new QDialogButtonBox(FilterNameDialogClass);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::NoButton|QDialogButtonBox::Ok);

        gridLayout->addWidget(buttonBox, 2, 2, 1, 1);


        retranslateUi(FilterNameDialogClass);

        QMetaObject::connectSlotsByName(FilterNameDialogClass);
    } // setupUi

    void retranslateUi(QDialog *FilterNameDialogClass)
    {
        FilterNameDialogClass->setWindowTitle(QApplication::translate("FilterNameDialogClass", "Add Filter Name", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("FilterNameDialogClass", "Filter Name:", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(FilterNameDialogClass);
    } // retranslateUi

};

namespace Ui {
    class FilterNameDialogClass: public Ui_FilterNameDialogClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FILTERNAMEDIALOG_H
