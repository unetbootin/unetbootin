/********************************************************************************
** Form generated from reading ui file 'waitingdialog.ui'
**
** Created: Wed Apr 13 19:35:30 2016
**      by: Qt User Interface Compiler version 4.5.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_WAITINGDIALOG_H
#define UI_WAITINGDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>

QT_BEGIN_NAMESPACE

class Ui_WaitingDialog
{
public:
    QLabel *infolabel;

    void setupUi(QDialog *WaitingDialog)
    {
        if (WaitingDialog->objectName().isEmpty())
            WaitingDialog->setObjectName(QString::fromUtf8("WaitingDialog"));
        WaitingDialog->resize(500, 40);
        infolabel = new QLabel(WaitingDialog);
        infolabel->setObjectName(QString::fromUtf8("infolabel"));
        infolabel->setGeometry(QRect(10, 10, 481, 20));

        retranslateUi(WaitingDialog);

        QMetaObject::connectSlotsByName(WaitingDialog);
    } // setupUi

    void retranslateUi(QDialog *WaitingDialog)
    {
        WaitingDialog->setWindowTitle(QApplication::translate("WaitingDialog", "Dialog", 0, QApplication::UnicodeUTF8));
        infolabel->setText(QApplication::translate("WaitingDialog", "Installation on FAT32 format hard disk will take a while. Please wait...", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(WaitingDialog);
    } // retranslateUi

};

namespace Ui {
    class WaitingDialog: public Ui_WaitingDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WAITINGDIALOG_H
