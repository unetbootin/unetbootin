/********************************************************************************
** Form generated from reading ui file 'selectsignaldialog.ui'
**
** Created: Thu Mar 5 20:40:11 2009
**      by: Qt User Interface Compiler version 4.5.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_SELECTSIGNALDIALOG_H
#define UI_SELECTSIGNALDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QTreeWidget>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_SelectSignalDialog
{
public:
    QVBoxLayout *verticalLayout;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout_2;
    QTreeWidget *signalList;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *SelectSignalDialog)
    {
        if (SelectSignalDialog->objectName().isEmpty())
            SelectSignalDialog->setObjectName(QString::fromUtf8("SelectSignalDialog"));
        SelectSignalDialog->resize(514, 183);
        verticalLayout = new QVBoxLayout(SelectSignalDialog);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        groupBox = new QGroupBox(SelectSignalDialog);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        verticalLayout_2 = new QVBoxLayout(groupBox);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        signalList = new QTreeWidget(groupBox);
        signalList->setObjectName(QString::fromUtf8("signalList"));
        signalList->setSortingEnabled(false);
        signalList->header()->setVisible(false);

        verticalLayout_2->addWidget(signalList);


        verticalLayout->addWidget(groupBox);

        buttonBox = new QDialogButtonBox(SelectSignalDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(SelectSignalDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), SelectSignalDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), SelectSignalDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(SelectSignalDialog);
    } // setupUi

    void retranslateUi(QDialog *SelectSignalDialog)
    {
        SelectSignalDialog->setWindowTitle(QApplication::translate("SelectSignalDialog", "Go to slot", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("SelectSignalDialog", "Select signal", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem = signalList->headerItem();
        ___qtreewidgetitem->setText(1, QApplication::translate("SelectSignalDialog", "class", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem->setText(0, QApplication::translate("SelectSignalDialog", "signal", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(SelectSignalDialog);
    } // retranslateUi

};

namespace Ui {
    class SelectSignalDialog: public Ui_SelectSignalDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SELECTSIGNALDIALOG_H
