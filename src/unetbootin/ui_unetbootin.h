/********************************************************************************
** Form generated from reading ui file 'unetbootin.ui'
**
** Created: Sun Feb 24 14:43:26 2008
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
    QLabel *labeldrive;
    QComboBox *driveselect;
    QLabel *labeldrive_2;
    QComboBox *typeselect;
    QLineEdit *OptionEnter;
    QLabel *labeloption;

    void setupUi(QWidget *unetbootin)
    {
    if (unetbootin->objectName().isEmpty())
        unetbootin->setObjectName(QString::fromUtf8("unetbootin"));
    unetbootin->resize(524, 300);
    unetbootin->setMinimumSize(QSize(524, 300));
    unetbootin->setMaximumSize(QSize(524, 300));
    FloppyFileSelector = new QPushButton(unetbootin);
    FloppyFileSelector->setObjectName(QString::fromUtf8("FloppyFileSelector"));
    FloppyFileSelector->setGeometry(QRect(488, 100, 25, 25));
    FloppyFileSelector->setAcceptDrops(false);
    FloppyPath = new QLineEdit(unetbootin);
    FloppyPath->setObjectName(QString::fromUtf8("FloppyPath"));
    FloppyPath->setGeometry(QRect(140, 100, 341, 25));
    radioFloppy = new QRadioButton(unetbootin);
    radioFloppy->setObjectName(QString::fromUtf8("radioFloppy"));
    radioFloppy->setGeometry(QRect(10, 100, 121, 25));
    radioManual = new QRadioButton(unetbootin);
    radioManual->setObjectName(QString::fromUtf8("radioManual"));
    radioManual->setGeometry(QRect(10, 150, 71, 25));
    KernelPath = new QLineEdit(unetbootin);
    KernelPath->setObjectName(QString::fromUtf8("KernelPath"));
    KernelPath->setGeometry(QRect(140, 150, 341, 25));
    KernelFileSelector = new QPushButton(unetbootin);
    KernelFileSelector->setObjectName(QString::fromUtf8("KernelFileSelector"));
    KernelFileSelector->setGeometry(QRect(488, 150, 25, 25));
    KernelFileSelector->setAcceptDrops(false);
    labelkernel = new QLabel(unetbootin);
    labelkernel->setObjectName(QString::fromUtf8("labelkernel"));
    labelkernel->setGeometry(QRect(90, 150, 41, 25));
    labelinitrd = new QLabel(unetbootin);
    labelinitrd->setObjectName(QString::fromUtf8("labelinitrd"));
    labelinitrd->setGeometry(QRect(90, 180, 41, 25));
    InitrdFileSelector = new QPushButton(unetbootin);
    InitrdFileSelector->setObjectName(QString::fromUtf8("InitrdFileSelector"));
    InitrdFileSelector->setGeometry(QRect(488, 180, 25, 25));
    InitrdFileSelector->setAcceptDrops(false);
    InitrdPath = new QLineEdit(unetbootin);
    InitrdPath->setObjectName(QString::fromUtf8("InitrdPath"));
    InitrdPath->setGeometry(QRect(140, 180, 341, 25));
    radioDistro = new QRadioButton(unetbootin);
    radioDistro->setObjectName(QString::fromUtf8("radioDistro"));
    radioDistro->setGeometry(QRect(10, 50, 121, 25));
    radioDistro->setChecked(true);
    distroselect = new QComboBox(unetbootin);
    distroselect->setObjectName(QString::fromUtf8("distroselect"));
    distroselect->setGeometry(QRect(140, 50, 371, 25));
    intromessage = new QLabel(unetbootin);
    intromessage->setObjectName(QString::fromUtf8("intromessage"));
    intromessage->setGeometry(QRect(20, 10, 491, 25));
    okbutton = new QPushButton(unetbootin);
    okbutton->setObjectName(QString::fromUtf8("okbutton"));
    okbutton->setGeometry(QRect(340, 260, 80, 25));
    cancelbutton = new QPushButton(unetbootin);
    cancelbutton->setObjectName(QString::fromUtf8("cancelbutton"));
    cancelbutton->setGeometry(QRect(430, 260, 80, 25));
    labeldrive = new QLabel(unetbootin);
    labeldrive->setObjectName(QString::fromUtf8("labeldrive"));
    labeldrive->setGeometry(QRect(10, 260, 41, 25));
    driveselect = new QComboBox(unetbootin);
    driveselect->setObjectName(QString::fromUtf8("driveselect"));
    driveselect->setEnabled(false);
    driveselect->setGeometry(QRect(50, 260, 51, 25));
    labeldrive_2 = new QLabel(unetbootin);
    labeldrive_2->setObjectName(QString::fromUtf8("labeldrive_2"));
    labeldrive_2->setGeometry(QRect(140, 260, 32, 25));
    typeselect = new QComboBox(unetbootin);
    typeselect->setObjectName(QString::fromUtf8("typeselect"));
    typeselect->setEnabled(false);
    typeselect->setGeometry(QRect(180, 260, 91, 25));
    OptionEnter = new QLineEdit(unetbootin);
    OptionEnter->setObjectName(QString::fromUtf8("OptionEnter"));
    OptionEnter->setGeometry(QRect(140, 210, 341, 25));
    labeloption = new QLabel(unetbootin);
    labeloption->setObjectName(QString::fromUtf8("labeloption"));
    labeloption->setGeometry(QRect(90, 210, 49, 25));

    retranslateUi(unetbootin);

    distroselect->setCurrentIndex(2);


    QMetaObject::connectSlotsByName(unetbootin);
    } // setupUi

    void retranslateUi(QWidget *unetbootin)
    {
    unetbootin->setWindowTitle(QApplication::translate("unetbootin", "UNetbootin", 0, QApplication::UnicodeUTF8));
    FloppyFileSelector->setToolTip(QApplication::translate("unetbootin", "Select disk image file", 0, QApplication::UnicodeUTF8));
    FloppyFileSelector->setText(QApplication::translate("unetbootin", "...", 0, QApplication::UnicodeUTF8));
    FloppyPath->setToolTip(QApplication::translate("unetbootin", "Specify a floppy or hard disk image file to load", 0, QApplication::UnicodeUTF8));
    radioFloppy->setToolTip(QApplication::translate("unetbootin", "Specify a floppy or hard disk image file to load", 0, QApplication::UnicodeUTF8));
    radioFloppy->setText(QApplication::translate("unetbootin", "Floppy Image:", 0, QApplication::UnicodeUTF8));
    radioManual->setToolTip(QApplication::translate("unetbootin", "Manually specify a kernel and initrd to load", 0, QApplication::UnicodeUTF8));
    radioManual->setText(QApplication::translate("unetbootin", "Manual:", 0, QApplication::UnicodeUTF8));
    KernelPath->setToolTip(QApplication::translate("unetbootin", "Specify a kernel file to load", 0, QApplication::UnicodeUTF8));
    KernelFileSelector->setToolTip(QApplication::translate("unetbootin", "Select kernel file", 0, QApplication::UnicodeUTF8));
    KernelFileSelector->setText(QApplication::translate("unetbootin", "...", 0, QApplication::UnicodeUTF8));
    labelkernel->setToolTip(QApplication::translate("unetbootin", "Specify a kernel file to load", 0, QApplication::UnicodeUTF8));
    labelkernel->setText(QApplication::translate("unetbootin", "Kernel", 0, QApplication::UnicodeUTF8));
    labelinitrd->setToolTip(QApplication::translate("unetbootin", "Specify an initrd file to load", 0, QApplication::UnicodeUTF8));
    labelinitrd->setText(QApplication::translate("unetbootin", "Initrd", 0, QApplication::UnicodeUTF8));
    InitrdFileSelector->setToolTip(QApplication::translate("unetbootin", "Select initrd file", 0, QApplication::UnicodeUTF8));
    InitrdFileSelector->setText(QApplication::translate("unetbootin", "...", 0, QApplication::UnicodeUTF8));
    InitrdPath->setToolTip(QApplication::translate("unetbootin", "Specify an initrd file to load", 0, QApplication::UnicodeUTF8));
    radioDistro->setToolTip(QApplication::translate("unetbootin", "Select from a list of supported distributions", 0, QApplication::UnicodeUTF8));
    radioDistro->setText(QApplication::translate("unetbootin", "Distribution:", 0, QApplication::UnicodeUTF8));
    distroselect->clear();
    distroselect->insertItems(0, QStringList()
     << QApplication::translate("unetbootin", "Ubuntu 8.04", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("unetbootin", "Ubuntu 8.04 x64", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("unetbootin", "Ubuntu 7.10", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("unetbootin", "Ubuntu 7.10 x64", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("unetbootin", "Ubuntu 7.04", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("unetbootin", "Ubuntu 7.04 x64", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("unetbootin", "Ubuntu 6.10", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("unetbootin", "Ubuntu 6.10 x64", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("unetbootin", "Ubuntu 6.06", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("unetbootin", "Ubuntu 6.06 x64", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("unetbootin", "Fedora 9 Alpha", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("unetbootin", "Fedora 9 Alpha x64", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("unetbootin", "Fedora 8", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("unetbootin", "Fedora 8 x64", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("unetbootin", "Fedora 7", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("unetbootin", "Fedora 7 x64", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("unetbootin", "openSUSE Factory", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("unetbootin", "openSUSE Factory x64", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("unetbootin", "openSUSE 10.3", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("unetbootin", "openSUSE 10.3 x64", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("unetbootin", "openSUSE 10.2", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("unetbootin", "openSUSE 10.2 x64", 0, QApplication::UnicodeUTF8)
    );
    distroselect->setToolTip(QApplication::translate("unetbootin", "Select from a list of supported distributions", 0, QApplication::UnicodeUTF8));
    intromessage->setText(QApplication::translate("unetbootin", "Choose a mode of operation. If unsure, select an option under \"Distribution\"", 0, QApplication::UnicodeUTF8));
    okbutton->setText(QApplication::translate("unetbootin", "OK", 0, QApplication::UnicodeUTF8));
    cancelbutton->setText(QApplication::translate("unetbootin", "Cancel", 0, QApplication::UnicodeUTF8));
    labeldrive->setToolTip(QApplication::translate("unetbootin", "Select the target drive to install to", 0, QApplication::UnicodeUTF8));
    labeldrive->setText(QApplication::translate("unetbootin", "Drive:", 0, QApplication::UnicodeUTF8));
    driveselect->clear();
    driveselect->insertItems(0, QStringList()
     << QApplication::translate("unetbootin", "C:", 0, QApplication::UnicodeUTF8)
    );
    driveselect->setToolTip(QApplication::translate("unetbootin", "Select the target drive to install to", 0, QApplication::UnicodeUTF8));
    labeldrive_2->setToolTip(QApplication::translate("unetbootin", "Select the installation target type", 0, QApplication::UnicodeUTF8));
    labeldrive_2->setText(QApplication::translate("unetbootin", "Type:", 0, QApplication::UnicodeUTF8));
    typeselect->clear();
    typeselect->insertItems(0, QStringList()
     << QApplication::translate("unetbootin", "Hard Disk", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("unetbootin", "USB Drive", 0, QApplication::UnicodeUTF8)
    );
    typeselect->setToolTip(QApplication::translate("unetbootin", "Select the installation target type", 0, QApplication::UnicodeUTF8));
    OptionEnter->setToolTip(QApplication::translate("unetbootin", "Specify parameters and options to pass to the kernel", 0, QApplication::UnicodeUTF8));
    labeloption->setToolTip(QApplication::translate("unetbootin", "Specify parameters and options to pass to the kernel", 0, QApplication::UnicodeUTF8));
    labeloption->setText(QApplication::translate("unetbootin", "Option", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(unetbootin);
    } // retranslateUi

};

namespace Ui {
    class unetbootin: public Ui_unetbootin {};
} // namespace Ui

#endif // UI_UNETBOOTIN_H
