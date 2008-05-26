/********************************************************************************
** Form generated from reading ui file 'unetbootin.ui'
**
** Created: Mon May 26 15:18:14 2008
**      by: Qt User Interface Compiler version 4.3.4
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
#include <QtGui/QProgressBar>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QTextBrowser>
#include <QtGui/QWidget>

class Ui_unetbootinui
{
public:
    QWidget *firstlayer;
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
    QPushButton *okbutton;
    QPushButton *cancelbutton;
    QLabel *labeldrive;
    QComboBox *driveselect;
    QLabel *labeldrive_2;
    QComboBox *typeselect;
    QLineEdit *OptionEnter;
    QLabel *labeloption;
    QComboBox *dverselect;
    QTextBrowser *intromessage;
    QComboBox *diskimagetypeselect;
    QPushButton *CfgFileSelector;
    QWidget *secondlayer;
    QProgressBar *tprogress;
    QLabel *pdesc5;
    QLabel *pdesc4;
    QLabel *pdesc3;
    QLabel *pdesc2;
    QLabel *pdesc1;
    QLabel *sdesc5;
    QLabel *sdesc4;
    QLabel *sdesc3;
    QLabel *sdesc2;
    QLabel *sdesc1;
    QWidget *thirdlayer;
    QPushButton *fexitbutton;
    QPushButton *frebootbutton;
    QTextBrowser *rebootmsgtext;
    QLabel *fsdesc1;
    QLabel *fsdesc2;
    QLabel *fsdesc3;
    QLabel *fsdesc5;
    QLabel *fsdesc4;

    void setupUi(QWidget *unetbootinui)
    {
    if (unetbootinui->objectName().isEmpty())
        unetbootinui->setObjectName(QString::fromUtf8("unetbootinui"));
    unetbootinui->resize(524, 360);
    unetbootinui->setMinimumSize(QSize(524, 360));
    unetbootinui->setMaximumSize(QSize(524, 360));
    firstlayer = new QWidget(unetbootinui);
    firstlayer->setObjectName(QString::fromUtf8("firstlayer"));
    firstlayer->setEnabled(false);
    firstlayer->setGeometry(QRect(0, 0, 524, 360));
    FloppyFileSelector = new QPushButton(firstlayer);
    FloppyFileSelector->setObjectName(QString::fromUtf8("FloppyFileSelector"));
    FloppyFileSelector->setGeometry(QRect(488, 180, 25, 25));
    FloppyFileSelector->setAcceptDrops(false);
    FloppyPath = new QLineEdit(firstlayer);
    FloppyPath->setObjectName(QString::fromUtf8("FloppyPath"));
    FloppyPath->setGeometry(QRect(190, 180, 291, 25));
    radioFloppy = new QRadioButton(firstlayer);
    radioFloppy->setObjectName(QString::fromUtf8("radioFloppy"));
    radioFloppy->setGeometry(QRect(10, 180, 101, 25));
    radioManual = new QRadioButton(firstlayer);
    radioManual->setObjectName(QString::fromUtf8("radioManual"));
    radioManual->setGeometry(QRect(10, 220, 81, 25));
    KernelPath = new QLineEdit(firstlayer);
    KernelPath->setObjectName(QString::fromUtf8("KernelPath"));
    KernelPath->setGeometry(QRect(190, 220, 291, 25));
    KernelFileSelector = new QPushButton(firstlayer);
    KernelFileSelector->setObjectName(QString::fromUtf8("KernelFileSelector"));
    KernelFileSelector->setGeometry(QRect(488, 220, 25, 25));
    KernelFileSelector->setAcceptDrops(false);
    labelkernel = new QLabel(firstlayer);
    labelkernel->setObjectName(QString::fromUtf8("labelkernel"));
    labelkernel->setGeometry(QRect(110, 220, 61, 25));
    labelinitrd = new QLabel(firstlayer);
    labelinitrd->setObjectName(QString::fromUtf8("labelinitrd"));
    labelinitrd->setGeometry(QRect(110, 250, 61, 25));
    InitrdFileSelector = new QPushButton(firstlayer);
    InitrdFileSelector->setObjectName(QString::fromUtf8("InitrdFileSelector"));
    InitrdFileSelector->setGeometry(QRect(488, 250, 25, 25));
    InitrdFileSelector->setAcceptDrops(false);
    InitrdPath = new QLineEdit(firstlayer);
    InitrdPath->setObjectName(QString::fromUtf8("InitrdPath"));
    InitrdPath->setGeometry(QRect(190, 250, 291, 25));
    radioDistro = new QRadioButton(firstlayer);
    radioDistro->setObjectName(QString::fromUtf8("radioDistro"));
    radioDistro->setGeometry(QRect(10, 20, 111, 25));
    radioDistro->setChecked(true);
    distroselect = new QComboBox(firstlayer);
    distroselect->setObjectName(QString::fromUtf8("distroselect"));
    distroselect->setGeometry(QRect(110, 20, 200, 25));
    okbutton = new QPushButton(firstlayer);
    okbutton->setObjectName(QString::fromUtf8("okbutton"));
    okbutton->setGeometry(QRect(343, 320, 80, 25));
    cancelbutton = new QPushButton(firstlayer);
    cancelbutton->setObjectName(QString::fromUtf8("cancelbutton"));
    cancelbutton->setGeometry(QRect(433, 320, 80, 25));
    labeldrive = new QLabel(firstlayer);
    labeldrive->setObjectName(QString::fromUtf8("labeldrive"));
    labeldrive->setGeometry(QRect(150, 320, 41, 25));
    driveselect = new QComboBox(firstlayer);
    driveselect->setObjectName(QString::fromUtf8("driveselect"));
    driveselect->setGeometry(QRect(190, 320, 111, 25));
    labeldrive_2 = new QLabel(firstlayer);
    labeldrive_2->setObjectName(QString::fromUtf8("labeldrive_2"));
    labeldrive_2->setGeometry(QRect(10, 320, 32, 25));
    typeselect = new QComboBox(firstlayer);
    typeselect->setObjectName(QString::fromUtf8("typeselect"));
    typeselect->setGeometry(QRect(50, 320, 91, 25));
    OptionEnter = new QLineEdit(firstlayer);
    OptionEnter->setObjectName(QString::fromUtf8("OptionEnter"));
    OptionEnter->setGeometry(QRect(190, 280, 291, 25));
    labeloption = new QLabel(firstlayer);
    labeloption->setObjectName(QString::fromUtf8("labeloption"));
    labeloption->setGeometry(QRect(110, 280, 61, 25));
    dverselect = new QComboBox(firstlayer);
    dverselect->setObjectName(QString::fromUtf8("dverselect"));
    dverselect->setGeometry(QRect(313, 20, 200, 25));
    intromessage = new QTextBrowser(firstlayer);
    intromessage->setObjectName(QString::fromUtf8("intromessage"));
    intromessage->setGeometry(QRect(9, 60, 503, 101));
    intromessage->setFrameShape(QFrame::NoFrame);
    intromessage->setOpenExternalLinks(true);
    diskimagetypeselect = new QComboBox(firstlayer);
    diskimagetypeselect->setObjectName(QString::fromUtf8("diskimagetypeselect"));
    diskimagetypeselect->setGeometry(QRect(110, 180, 71, 25));
    CfgFileSelector = new QPushButton(firstlayer);
    CfgFileSelector->setObjectName(QString::fromUtf8("CfgFileSelector"));
    CfgFileSelector->setGeometry(QRect(488, 280, 25, 25));
    CfgFileSelector->setAcceptDrops(false);
    secondlayer = new QWidget(unetbootinui);
    secondlayer->setObjectName(QString::fromUtf8("secondlayer"));
    secondlayer->setEnabled(false);
    secondlayer->setGeometry(QRect(0, 0, 524, 360));
    tprogress = new QProgressBar(secondlayer);
    tprogress->setObjectName(QString::fromUtf8("tprogress"));
    tprogress->setGeometry(QRect(10, 320, 501, 23));
    tprogress->setValue(24);
    pdesc5 = new QLabel(secondlayer);
    pdesc5->setObjectName(QString::fromUtf8("pdesc5"));
    pdesc5->setGeometry(QRect(10, 160, 501, 20));
    pdesc5->setOpenExternalLinks(true);
    pdesc4 = new QLabel(secondlayer);
    pdesc4->setObjectName(QString::fromUtf8("pdesc4"));
    pdesc4->setGeometry(QRect(10, 190, 501, 20));
    pdesc4->setOpenExternalLinks(true);
    pdesc3 = new QLabel(secondlayer);
    pdesc3->setObjectName(QString::fromUtf8("pdesc3"));
    pdesc3->setGeometry(QRect(10, 220, 501, 20));
    pdesc3->setOpenExternalLinks(true);
    pdesc2 = new QLabel(secondlayer);
    pdesc2->setObjectName(QString::fromUtf8("pdesc2"));
    pdesc2->setGeometry(QRect(10, 250, 501, 20));
    pdesc2->setOpenExternalLinks(true);
    pdesc1 = new QLabel(secondlayer);
    pdesc1->setObjectName(QString::fromUtf8("pdesc1"));
    pdesc1->setGeometry(QRect(10, 280, 501, 20));
    pdesc1->setOpenExternalLinks(true);
    sdesc5 = new QLabel(secondlayer);
    sdesc5->setObjectName(QString::fromUtf8("sdesc5"));
    sdesc5->setGeometry(QRect(10, 130, 501, 20));
    sdesc4 = new QLabel(secondlayer);
    sdesc4->setObjectName(QString::fromUtf8("sdesc4"));
    sdesc4->setGeometry(QRect(10, 100, 501, 20));
    sdesc3 = new QLabel(secondlayer);
    sdesc3->setObjectName(QString::fromUtf8("sdesc3"));
    sdesc3->setGeometry(QRect(10, 70, 501, 20));
    sdesc2 = new QLabel(secondlayer);
    sdesc2->setObjectName(QString::fromUtf8("sdesc2"));
    sdesc2->setGeometry(QRect(10, 40, 501, 20));
    sdesc1 = new QLabel(secondlayer);
    sdesc1->setObjectName(QString::fromUtf8("sdesc1"));
    sdesc1->setGeometry(QRect(10, 10, 501, 20));
    thirdlayer = new QWidget(unetbootinui);
    thirdlayer->setObjectName(QString::fromUtf8("thirdlayer"));
    thirdlayer->setEnabled(false);
    thirdlayer->setGeometry(QRect(0, 0, 524, 360));
    fexitbutton = new QPushButton(thirdlayer);
    fexitbutton->setObjectName(QString::fromUtf8("fexitbutton"));
    fexitbutton->setGeometry(QRect(420, 320, 100, 29));
    frebootbutton = new QPushButton(thirdlayer);
    frebootbutton->setObjectName(QString::fromUtf8("frebootbutton"));
    frebootbutton->setGeometry(QRect(310, 320, 100, 29));
    rebootmsgtext = new QTextBrowser(thirdlayer);
    rebootmsgtext->setObjectName(QString::fromUtf8("rebootmsgtext"));
    rebootmsgtext->setGeometry(QRect(20, 160, 501, 151));
    rebootmsgtext->setFrameShape(QFrame::NoFrame);
    rebootmsgtext->setFrameShadow(QFrame::Plain);
    rebootmsgtext->setLineWidth(0);
    rebootmsgtext->setOpenExternalLinks(true);
    fsdesc1 = new QLabel(thirdlayer);
    fsdesc1->setObjectName(QString::fromUtf8("fsdesc1"));
    fsdesc1->setGeometry(QRect(20, 20, 501, 20));
    fsdesc2 = new QLabel(thirdlayer);
    fsdesc2->setObjectName(QString::fromUtf8("fsdesc2"));
    fsdesc2->setGeometry(QRect(20, 50, 501, 20));
    fsdesc3 = new QLabel(thirdlayer);
    fsdesc3->setObjectName(QString::fromUtf8("fsdesc3"));
    fsdesc3->setGeometry(QRect(20, 80, 501, 20));
    fsdesc5 = new QLabel(thirdlayer);
    fsdesc5->setObjectName(QString::fromUtf8("fsdesc5"));
    fsdesc5->setGeometry(QRect(20, 140, 501, 20));
    fsdesc4 = new QLabel(thirdlayer);
    fsdesc4->setObjectName(QString::fromUtf8("fsdesc4"));
    fsdesc4->setGeometry(QRect(20, 110, 501, 20));

    retranslateUi(unetbootinui);

    distroselect->setCurrentIndex(-1);
    dverselect->setCurrentIndex(-1);


    QMetaObject::connectSlotsByName(unetbootinui);
    } // setupUi

    void retranslateUi(QWidget *unetbootinui)
    {
    unetbootinui->setWindowTitle(QApplication::translate("unetbootinui", "UNetbootin", 0, QApplication::UnicodeUTF8));
    FloppyFileSelector->setToolTip(QApplication::translate("unetbootinui", "Select disk image file", 0, QApplication::UnicodeUTF8));
    FloppyFileSelector->setText(QApplication::translate("unetbootinui", "...", 0, QApplication::UnicodeUTF8));
    FloppyPath->setToolTip(QApplication::translate("unetbootinui", "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:10pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Specify a floppy/hard disk image, or CD image (ISO) file to load</p></body></html>", 0, QApplication::UnicodeUTF8));
    radioFloppy->setToolTip(QApplication::translate("unetbootinui", "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:10pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Specify a disk image file to load</p></body></html>", 0, QApplication::UnicodeUTF8));
    radioFloppy->setText(QApplication::translate("unetbootinui", "Disk Image", 0, QApplication::UnicodeUTF8));
    radioManual->setToolTip(QApplication::translate("unetbootinui", "Manually specify a kernel and initrd to load", 0, QApplication::UnicodeUTF8));
    radioManual->setText(QApplication::translate("unetbootinui", "Custom", 0, QApplication::UnicodeUTF8));
    KernelPath->setToolTip(QApplication::translate("unetbootinui", "Specify a kernel file to load", 0, QApplication::UnicodeUTF8));
    KernelFileSelector->setToolTip(QApplication::translate("unetbootinui", "Select kernel file", 0, QApplication::UnicodeUTF8));
    KernelFileSelector->setText(QApplication::translate("unetbootinui", "...", 0, QApplication::UnicodeUTF8));
    labelkernel->setToolTip(QApplication::translate("unetbootinui", "Specify a kernel file to load", 0, QApplication::UnicodeUTF8));
    labelkernel->setText(QApplication::translate("unetbootinui", "Kernel:", 0, QApplication::UnicodeUTF8));
    labelinitrd->setToolTip(QApplication::translate("unetbootinui", "Specify an initrd file to load", 0, QApplication::UnicodeUTF8));
    labelinitrd->setText(QApplication::translate("unetbootinui", "Initrd:", 0, QApplication::UnicodeUTF8));
    InitrdFileSelector->setToolTip(QApplication::translate("unetbootinui", "Select initrd file", 0, QApplication::UnicodeUTF8));
    InitrdFileSelector->setText(QApplication::translate("unetbootinui", "...", 0, QApplication::UnicodeUTF8));
    InitrdPath->setToolTip(QApplication::translate("unetbootinui", "Specify an initrd file to load", 0, QApplication::UnicodeUTF8));
    radioDistro->setToolTip(QApplication::translate("unetbootinui", "Select from a list of supported distributions", 0, QApplication::UnicodeUTF8));
    radioDistro->setText(QApplication::translate("unetbootinui", "Distribution", 0, QApplication::UnicodeUTF8));
    distroselect->setToolTip(QApplication::translate("unetbootinui", "Select from a list of supported distributions", 0, QApplication::UnicodeUTF8));
    okbutton->setText(QApplication::translate("unetbootinui", "OK", 0, QApplication::UnicodeUTF8));
    cancelbutton->setText(QApplication::translate("unetbootinui", "Cancel", 0, QApplication::UnicodeUTF8));
    labeldrive->setToolTip(QApplication::translate("unetbootinui", "Select the target drive to install to", 0, QApplication::UnicodeUTF8));
    labeldrive->setText(QApplication::translate("unetbootinui", "Drive:", 0, QApplication::UnicodeUTF8));
    driveselect->setToolTip(QApplication::translate("unetbootinui", "Select the target drive to install to", 0, QApplication::UnicodeUTF8));
    labeldrive_2->setToolTip(QApplication::translate("unetbootinui", "Select the installation target type", 0, QApplication::UnicodeUTF8));
    labeldrive_2->setText(QApplication::translate("unetbootinui", "Type:", 0, QApplication::UnicodeUTF8));
    typeselect->clear();
    typeselect->insertItems(0, QStringList()
     << QApplication::translate("unetbootinui", "Hard Disk", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("unetbootinui", "USB Drive", 0, QApplication::UnicodeUTF8)
    );
    typeselect->setToolTip(QApplication::translate("unetbootinui", "Select the installation target type", 0, QApplication::UnicodeUTF8));
    OptionEnter->setToolTip(QApplication::translate("unetbootinui", "Specify parameters and options to pass to the kernel", 0, QApplication::UnicodeUTF8));
    labeloption->setToolTip(QApplication::translate("unetbootinui", "Specify parameters and options to pass to the kernel", 0, QApplication::UnicodeUTF8));
    labeloption->setText(QApplication::translate("unetbootinui", "Options:", 0, QApplication::UnicodeUTF8));
    dverselect->setToolTip(QApplication::translate("unetbootinui", "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:10pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Select the distribution version</p></body></html>", 0, QApplication::UnicodeUTF8));
    intromessage->setStyleSheet(QApplication::translate("unetbootinui", "background: transparent;", 0, QApplication::UnicodeUTF8));
    diskimagetypeselect->clear();
    diskimagetypeselect->insertItems(0, QStringList()
     << QApplication::translate("unetbootinui", "ISO", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("unetbootinui", "Floppy", 0, QApplication::UnicodeUTF8)
    );
    diskimagetypeselect->setToolTip(QApplication::translate("unetbootinui", "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:10pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Select the disk image type</p></body></html>", 0, QApplication::UnicodeUTF8));
    CfgFileSelector->setToolTip(QApplication::translate("unetbootinui", "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:10pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Select syslinux.cfg or isolinux.cfg file</p></body></html>", 0, QApplication::UnicodeUTF8));
    CfgFileSelector->setText(QApplication::translate("unetbootinui", "...", 0, QApplication::UnicodeUTF8));
    pdesc5->setText(QString());
    pdesc4->setText(QString());
    pdesc3->setText(QString());
    pdesc2->setText(QString());
    pdesc1->setText(QString());
    sdesc5->setText(QString());
    sdesc4->setText(QApplication::translate("unetbootinui", "4. Installation Complete, Reboot", 0, QApplication::UnicodeUTF8));
    sdesc3->setText(QApplication::translate("unetbootinui", "3. Installing Bootloader", 0, QApplication::UnicodeUTF8));
    sdesc2->setText(QApplication::translate("unetbootinui", "2. Extracting and Copying Files", 0, QApplication::UnicodeUTF8));
    sdesc1->setText(QApplication::translate("unetbootinui", "1. Downloading Files", 0, QApplication::UnicodeUTF8));
    fexitbutton->setText(QApplication::translate("unetbootinui", "Exit", 0, QApplication::UnicodeUTF8));
    frebootbutton->setText(QApplication::translate("unetbootinui", "Reboot Now", 0, QApplication::UnicodeUTF8));
    rebootmsgtext->setStyleSheet(QApplication::translate("unetbootinui", "background: transparent;", 0, QApplication::UnicodeUTF8));
    fsdesc1->setText(QApplication::translate("unetbootinui", "1. Downloading Files (Done)", 0, QApplication::UnicodeUTF8));
    fsdesc2->setText(QApplication::translate("unetbootinui", "2. Extracting and Copying Files (Done)", 0, QApplication::UnicodeUTF8));
    fsdesc3->setText(QApplication::translate("unetbootinui", "3. Installing Bootloader (Done)", 0, QApplication::UnicodeUTF8));
    fsdesc5->setText(QString());
    fsdesc4->setText(QApplication::translate("unetbootinui", "<b>4. Installation Complete, Reboot (Current)</b>", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(unetbootinui);
    } // retranslateUi

};

namespace Ui {
    class unetbootinui: public Ui_unetbootinui {};
} // namespace Ui

#endif // UI_UNETBOOTIN_H
