/********************************************************************************
** Form generated from reading ui file 'dataimageconfigdialog.ui'
**
** Created: Wed Apr 20 17:01:11 2016
**      by: Qt User Interface Compiler version 4.5.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_DATAIMAGECONFIGDIALOG_H
#define UI_DATAIMAGECONFIGDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QSplitter>

QT_BEGIN_NAMESPACE

class Ui_DataImageConfigDialog
{
public:
    QPushButton *pushButton;
    QLabel *label_2;
    QSplitter *splitter;
    QLabel *label;
    QRadioButton *radioButton1;
    QRadioButton *radioButton2;
    QRadioButton *radioButton3;

    void setupUi(QDialog *DataImageConfigDialog)
    {
        if (DataImageConfigDialog->objectName().isEmpty())
            DataImageConfigDialog->setObjectName(QString::fromUtf8("DataImageConfigDialog"));
        DataImageConfigDialog->resize(521, 137);
        pushButton = new QPushButton(DataImageConfigDialog);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(400, 100, 99, 27));
        label_2 = new QLabel(DataImageConfigDialog);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(11, 11, 498, 17));
        splitter = new QSplitter(DataImageConfigDialog);
        splitter->setObjectName(QString::fromUtf8("splitter"));
        splitter->setGeometry(QRect(12, 38, 179, 83));
        splitter->setOrientation(Qt::Vertical);
        label = new QLabel(splitter);
        label->setObjectName(QString::fromUtf8("label"));
        splitter->addWidget(label);
        radioButton1 = new QRadioButton(splitter);
        radioButton1->setObjectName(QString::fromUtf8("radioButton1"));
        splitter->addWidget(radioButton1);
        radioButton2 = new QRadioButton(splitter);
        radioButton2->setObjectName(QString::fromUtf8("radioButton2"));
        radioButton2->setChecked(false);
        splitter->addWidget(radioButton2);
        radioButton3 = new QRadioButton(splitter);
        radioButton3->setObjectName(QString::fromUtf8("radioButton3"));
        radioButton3->setCheckable(true);
        radioButton3->setChecked(true);
        splitter->addWidget(radioButton3);

        retranslateUi(DataImageConfigDialog);

        QMetaObject::connectSlotsByName(DataImageConfigDialog);
    } // setupUi

    void retranslateUi(QDialog *DataImageConfigDialog)
    {
        DataImageConfigDialog->setWindowTitle(QApplication::translate("DataImageConfigDialog", "Dialog", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("DataImageConfigDialog", "Ok", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("DataImageConfigDialog", "A larger system size is recommended because you can install more apps.", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("DataImageConfigDialog", "Choose system size:", 0, QApplication::UnicodeUTF8));
        radioButton1->setText(QApplication::translate("DataImageConfigDialog", "8GB", 0, QApplication::UnicodeUTF8));
        radioButton2->setText(QApplication::translate("DataImageConfigDialog", "16GB", 0, QApplication::UnicodeUTF8));
        radioButton3->setText(QApplication::translate("DataImageConfigDialog", "32GB(Recommended)", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(DataImageConfigDialog);
    } // retranslateUi

};

namespace Ui {
    class DataImageConfigDialog: public Ui_DataImageConfigDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DATAIMAGECONFIGDIALOG_H
