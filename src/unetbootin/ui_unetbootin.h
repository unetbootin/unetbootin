/********************************************************************************
** Form generated from reading ui file 'unetbootin.ui'
**
** Created: Mon Feb 18 10:44:12 2008
**      by: Qt User Interface Compiler version 4.3.2
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_UNETBOOTIN_H
#define UI_UNETBOOTIN_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QWidget>

class Ui_unetbootin
{
public:
    QPushButton *FloppyFileSelector;
    QLineEdit *FloppyPath;
    QRadioButton *radioFloppy;
    QRadioButton *radioManual;
    QLineEdit *KernelPath;
    QPushButton *KernelFileSelector;
    QLabel *labelkernel;
    QLabel *labelinitrd;
    QPushButton *InitrdFileSelector;
    QLineEdit *InitrdPath;
    QRadioButton *radioDistro;
    QComboBox *distroselect;
    QLabel *intromessage;
    QPushButton *okbutton;
    QPushButton *cancelbutton;

    void setupUi(QWidget *unetbootin)
    {
    if (unetbootin->objectName().isEmpty())
        unetbootin->setObjectName(QString::fromUtf8("unetbootin"));
    unetbootin->resize(524, 283);
    FloppyFileSelector = new QPushButton(unetbootin);
    FloppyFileSelector->setObjectName(QString::fromUtf8("FloppyFileSelector"));
    FloppyFileSelector->setGeometry(QRect(488, 120, 25, 25));
    FloppyFileSelector->setAcceptDrops(false);
    FloppyPath = new QLineEdit(unetbootin);
    FloppyPath->setObjectName(QString::fromUtf8("FloppyPath"));
    FloppyPath->setGeometry(QRect(140, 120, 341, 25));
    radioFloppy = new QRadioButton(unetbootin);
    radioFloppy->setObjectName(QString::fromUtf8("radioFloppy"));
    radioFloppy->setGeometry(QRect(10, 120, 161, 25));
    radioManual = new QRadioButton(unetbootin);
    radioManual->setObjectName(QString::fromUtf8("radioManual"));
    radioManual->setGeometry(QRect(10, 170, 71, 25));
    KernelPath = new QLineEdit(unetbootin);
    KernelPath->setObjectName(QString::fromUtf8("KernelPath"));
    KernelPath->setGeometry(QRect(140, 170, 341, 25));
    KernelFileSelector = new QPushButton(unetbootin);
    KernelFileSelector->setObjectName(QString::fromUtf8("KernelFileSelector"));
    KernelFileSelector->setGeometry(QRect(488, 170, 25, 25));
    KernelFileSelector->setAcceptDrops(false);
    labelkernel = new QLabel(unetbootin);
    labelkernel->setObjectName(QString::fromUtf8("labelkernel"));
    labelkernel->setGeometry(QRect(90, 170, 59, 25));
    labelinitrd = new QLabel(unetbootin);
    labelinitrd->setObjectName(QString::fromUtf8("labelinitrd"));
    labelinitrd->setGeometry(QRect(90, 200, 59, 25));
    InitrdFileSelector = new QPushButton(unetbootin);
    InitrdFileSelector->setObjectName(QString::fromUtf8("InitrdFileSelector"));
    InitrdFileSelector->setGeometry(QRect(488, 200, 25, 25));
    InitrdFileSelector->setAcceptDrops(false);
    InitrdPath = new QLineEdit(unetbootin);
    InitrdPath->setObjectName(QString::fromUtf8("InitrdPath"));
    InitrdPath->setGeometry(QRect(140, 200, 341, 25));
    radioDistro = new QRadioButton(unetbootin);
    radioDistro->setObjectName(QString::fromUtf8("radioDistro"));
    radioDistro->setGeometry(QRect(10, 70, 141, 25));
    distroselect = new QComboBox(unetbootin);
    distroselect->setObjectName(QString::fromUtf8("distroselect"));
    distroselect->setGeometry(QRect(140, 70, 371, 26));
    intromessage = new QLabel(unetbootin);
    intromessage->setObjectName(QString::fromUtf8("intromessage"));
    intromessage->setGeometry(QRect(20, 20, 491, 31));
    okbutton = new QPushButton(unetbootin);
    okbutton->setObjectName(QString::fromUtf8("okbutton"));
    okbutton->setGeometry(QRect(340, 240, 80, 29));
    cancelbutton = new QPushButton(unetbootin);
    cancelbutton->setObjectName(QString::fromUtf8("cancelbutton"));
    cancelbutton->setGeometry(QRect(430, 240, 80, 29));

    retranslateUi(unetbootin);

    QMetaObject::connectSlotsByName(unetbootin);
    } // setupUi

    void retranslateUi(QWidget *unetbootin)
    {
    unetbootin->setWindowTitle(QApplication::translate("unetbootin", "UNetbootin", 0, QApplication::UnicodeUTF8));
    FloppyFileSelector->setText(QApplication::translate("unetbootin", "...", 0, QApplication::UnicodeUTF8));
    radioFloppy->setText(QApplication::translate("unetbootin", "Floppy Image:", 0, QApplication::UnicodeUTF8));
    radioManual->setText(QApplication::translate("unetbootin", "Manual:", 0, QApplication::UnicodeUTF8));
    KernelFileSelector->setText(QApplication::translate("unetbootin", "...", 0, QApplication::UnicodeUTF8));
    labelkernel->setText(QApplication::translate("unetbootin", "Kernel", 0, QApplication::UnicodeUTF8));
    labelinitrd->setText(QApplication::translate("unetbootin", "Initrd", 0, QApplication::UnicodeUTF8));
    InitrdFileSelector->setText(QApplication::translate("unetbootin", "...", 0, QApplication::UnicodeUTF8));
    radioDistro->setText(QApplication::translate("unetbootin", "Distribution:", 0, QApplication::UnicodeUTF8));
    intromessage->setText(QApplication::translate("unetbootin", "Choose a mode of operation. If unsure, select an option under \"Distribution\"", 0, QApplication::UnicodeUTF8));
    okbutton->setText(QApplication::translate("unetbootin", "OK", 0, QApplication::UnicodeUTF8));
    cancelbutton->setText(QApplication::translate("unetbootin", "Cancel", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(unetbootin);
    } // retranslateUi

};

namespace Ui {
    class unetbootin: public Ui_unetbootin {};
} // namespace Ui

#endif // UI_UNETBOOTIN_H
